/*
 * Copyright (C) Your copyright notice.
 *
 * Author: Onur Dundar
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "message.h"
#include "serial.h"
#include "device.h"
#include "message_queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Constant Values from OpenZWave Project
 */
static char const* power_level_names[] = { "Normal", "-1dB", "-2dB", "-3dB",
		"-4dB", "-5dB", "-6dB", "-7dB", "-8dB", "-9dB", "Unknown" };

static char const* electricity_unit_names[] = { "kWatts/h", "kVA/h", "Watts",
		"pulses", "Volts", "Ampers", "Power Factor", "" };

static char const* electricity_label_names[] = { "Energy", "Energy", "Power",
		"Count", "Voltage", "Current", "Power Factor", "Unknown" };
/**
 * Bitmasks and Constants to Calculate Multilevel Sensor Value
 */
static const uint8_t size_mask = 0x07; //0b00000111
static const uint8_t scale_mask = 0x18; //0b00011000
static const uint8_t scale_shift = 0x03;
static const uint8_t precision_mask = 0xe0; //0b11100000
static const uint8_t precision_shift = 0x05;

/**
 * Function from OpenZWave project
 * Generate Checksum Value to Append End of ZWave Message
 * @param message buffer
 * @param buffer length
 * @return checksum value
 */
static inline uint8_t generate_checksum(uint8_t buffer[], int length) {
	int i = 1;
	uint8_t checksum = 0xFF;
	for (; i < length - 1; i++) {
		checksum ^= buffer[i];
	}
	return checksum;
}
/**
 * Function from OpenZWave project
 * Calculate Multilevel Sensor Value
 * @param command class data values
 * @param length of data
 * @return calculated value
 */
static inline float calculate_sensor_value(uint8_t data[], int length,
		uint8_t *unit_scale) {

	uint32_t value = 0;
	float result = 0.0;
	char sign = ' ';
	uint8_t value_description = data[0];
	//Last Three Bit is the Size of Value
	uint8_t size = value_description & size_mask;
	//First Three Bit is the Precision Value
	uint8_t precision = (value_description & precision_mask) >> precision_shift;
	//Two bits at the middle are the scale Value
	uint8_t scale = (value_description & scale_mask) >> scale_shift;
	*unit_scale = scale;
	int offset = 1;
	int i = 0;
	//Read Value to 32 bit
	for (; i < size; i++) {
		value <<= 8;
		value |= (uint32_t) data[i + offset];
	}
	//Dealing With Sign
	if (data[offset] & 0x80) {
		sign = '-';
		printf("negative\n");
		if (size == 1) {
			value |= 0xffffff00;
		} else if (size == 2) {
			value |= 0xffff0000;
		}
	}
	//Convert Integer to String
	char buf[12] = { 0 };
	if (precision == 0) {
		snprintf(buf, 12, "%d", (signed int) value);
		result = atof(buf);
	} else {
		// Fill the buffer with the value padded with leading zeros.
		snprintf(buf, 12, "%011d", (signed int) value);
		//Calculate Position of Decimal Point
		int32_t decimal = 10 - precision;
		int32_t start = -1;
		int32_t i = 0;
		if (sign == '-') {
			buf[i] = sign;
			i++;
		}
		for (; i < decimal; ++i) {
			buf[i] = buf[i + 1];
			if ((start < 0) && (buf[i] != '0')) {
				start = i;
			}
		}
		if (start < 0) {
			start = decimal - 1;
		}
		buf[decimal] = '.';
		result = atof(buf);
	}
	return result;
}

int parse_incoming_mesage(uint8_t* message, int length) {
	/**
	 * Message Length
	 */
	uint8_t length_of_rest = message[1];
	uint8_t message_type = message[2];
	uint8_t message_function = message[3];
	if (message_function == SEND_DATA && length_of_rest == 0x04) {
		printf("Data Sent to ZWave Stack\n");
		return SEND_DATA;
	} else if (message_function == SEND_DATA && length_of_rest == 0x05) {
		printf("Data Request with Callback ID 0x%x Received\n", message[4]);
		return message[4];
	} else if (message_function == RESPONSE_RECEIVED) {
		printf("Response From ");
		if (message[5] == MultiSensorNodeID) {
			printf("Multi-Sensor Node Received: ");
		} else if (message[5] == WallPlugNodeID) {
			printf("Wall Plug Node Received: ");
		} else if (message[5] == LampHolderNodeID) {
			printf("Lamp Holder Node Received: ");
		} else if (message[5] == FloodSensorNodeID) {
			printf("Flood Sensor Node Received: ");
		}
		/**
		 * Length of Data
		 */
		uint8_t data_length = message[6];
		uint8_t command_class = message[7];
		/**
		 * Check Command Class and Take Action Accordingly
		 */
		if (command_class == COMMAND_CLASS_BATTERY) {
			int battery = 0;
			//If battery is not 0%
			if (message[9] != 0xFF) {
				battery = message[9];
			}
			if (message[5] == MultiSensorNodeID) {
				multi_sensor_battery_level = battery;
			} else if (message[5] == FloodSensorNodeID) {
				flood_detector_battery_level = battery;
			}
			printf("Battery Level of Device is %d % \n", battery);
		} else if (command_class == COMMAND_CLASS_SENSOR_BINARY) {
			if (message[8] == BINARY_REPORT) {
				if (data_length == 0x04) {
					if (message[10] == DOOR_WINDOW_SENSOR) {
						if (message[9] == ON) {
							printf("Door/Window Sensor is OPEN\n");
							snprintf(door_sensor_status, sizeof(door_open),
									door_open);
						} else if (message[9] == OFF) {
							printf("Door/Window Sensor is CLOSE\n");
							snprintf(door_sensor_status, sizeof(door_closed),
									door_closed);
						}
					} else if (message[10] == MOTION_DETECTION_SENSOR) {
						if (message[9] == ON) {
							printf("Motion DETECTED\n");
							snprintf(motion_sensor_status, sizeof(detected),
									detected);
						} else if (message[9] == OFF) {
							printf("NO Motion Detected\n");
							snprintf(motion_sensor_status, sizeof(not_detected),
									not_detected);
						}
					} else if (message[10] == WATER_DETECTION_SENSOR) {
						if (message[9] == ON) {
							printf("Water LEAK DETECTED\n");
							snprintf(flood_sensor_status, sizeof(detected),
									detected);
						} else if (message[9] == OFF) {
							printf("NO Water Leak Detected\n");
							snprintf(flood_sensor_status, sizeof(not_detected),
									not_detected);
						}
					}
				}
			}
		} else if (command_class == COMMAND_CLASS_SENSOR_MULTILEVEL_V2) {
			if (message[8] == MULTILEVEL_REPORT) {
				/**
				 * Calculate Sensor Value
				 */
				int len = data_length - 3;
				uint8_t *data = malloc(len * sizeof(uint8_t));
				uint8_t scale;
				int i = 0;
				for (; i < len; i++) {
					data[i] = message[10 + i];
				}
				float value = calculate_sensor_value(data, len, &scale);
				if (message[9] == TEMPERATURE_SENSOR) {
					printf("Temperature Value is %f ", value);
					temperature_f = value;
					if (scale) {
						printf("Fahrenheit \n");
					} else {
						printf("Celcius \n");
					}
				} else if (message[9] == LUMINANCE_SENSOR) {
					printf("Illumination is %f ", value);
					illumination = value;
					if (scale) {
						printf("Lux \n");
					} else {
						printf("% \n");
					}
				} else if (message[9] == POWER_SENSOR) {
					printf("Power Value is %f \n", value);
					current_energy_consumption = value;
					if (scale) {
						printf("BUT/h \n");
					} else {
						printf("Watts \n");
					}
				}
			}
		} else if (command_class == COMMAND_CLASS_BASIC) {
			printf("Device is ");
			char status_buffer[256];
			if (message[9] == OFF) {
				printf("Sleeping\n");
				snprintf(status_buffer, sizeof(status_sleeping),
						status_sleeping);
			} else if (message[9] == ON) {
				printf("Active\n");
				snprintf(status_buffer, sizeof(status_active), status_active);
			}
			if (message[5] == WallPlugNodeID) {
				snprintf(wall_plug_status, sizeof(status_buffer),
						status_buffer);
			} else if (message[5] == MultiSensorNodeID) {
				snprintf(multi_sensor_status, sizeof(status_buffer),
						status_buffer);
			} else if (message[5] == LampHolderNodeID) {
				snprintf(lamp_holder_status, sizeof(status_buffer),
						status_buffer);
			} else if (message[5] == FloodSensorNodeID) {
				snprintf(flood_detector_status, sizeof(status_buffer),
						status_buffer);
			}
		} else if (command_class == COMMAND_CLASS_SWITCH_BINARY
				|| command_class == COMMAND_CLASS_SWITCH_ALL) {

			printf("Switch is ");

			if (message[8] == SWITCH_REPORT) {
				char switch_buffer[256];
				if (message[9] == OFF) {
					printf("OFF\n");
					snprintf(switch_buffer, sizeof(device_off), device_off);
				} else if (message[9] == ON) {
					printf("ON\n");
					snprintf(switch_buffer, sizeof(device_on), device_on);
				}
				if (message[5] == WallPlugNodeID) {
					snprintf(wall_plug_switch, sizeof(switch_buffer),
							switch_buffer);
				} else if (message[5] == LampHolderNodeID) {
					snprintf(lamp_holder_switch, sizeof(switch_buffer),
							switch_buffer);
				}
			}
		} else if (command_class == COMMAND_CLASS_POWERLEVEL) {

			printf("Power Level of Device is %s Timeout %d\n",
					power_level_names[message[9]], message[10]);
			if (message[5] == MultiSensorNodeID) {
				snprintf(multi_sensor_power_level,
						sizeof(power_level_names[message[9]]),
						power_level_names[message[9]]);
			} else if (message[5] == WallPlugNodeID) {
				snprintf(multi_sensor_power_level,
						sizeof(power_level_names[message[9]]),
						power_level_names[message[9]]);
			} else if (message[5] == LampHolderNodeID) {
				snprintf(multi_sensor_power_level,
						sizeof(power_level_names[message[9]]),
						power_level_names[message[9]]);
			} else if (message[5] == FloodSensorNodeID) {
				snprintf(multi_sensor_power_level,
						sizeof(power_level_names[message[9]]),
						power_level_names[message[9]]);
			}

		} else if (command_class == COMMAND_CLASS_METER) {
			//Open ZWave Meter
			int len = data_length - 3;
			uint8_t *data = malloc(len * sizeof(uint8_t));
			uint8_t scale;
			int i = 0;
			for (; i < len; i++) {
				data[i] = message[10 + i];
			}
			float value = calculate_sensor_value(data, len, &scale);
			if (message[8] == METER_REPORT) {
				uint8_t meter_type = message[9] & 0x1F;
				if (meter_type == METER_ELECTRICITY) {
					const char* label = electricity_label_names[scale];
					const char* unit = electricity_unit_names[scale];
					printf("Electicity Meter Report %s %f %s\n", label, value,
							unit);
					if (scale == 2) {
						current_energy_consumption = value;
					} else if (scale == 0) {
						cumulative_energy_level = value;
					}
				} else if (meter_type == METER_GAS) {
				} else if (meter_type == METER_WATER) {
				}
			}
		} else if (command_class == COMMAND_CLASS_SENSOR_ALARM) {
			printf("Sensor Alarm ");
			if (message[8] == SENSOR_ALARM_REPORT) {
				printf("Reported ");
				if (message[10] == FLOOD_ALARM) {
					if (message[11] == ON) {
						printf("FLOOD Detected\n");
						snprintf(flood_sensor_status,sizeof(detected),detected);
					} else if (message[11] == OFF) {
						printf("NO Flood Danger\n");
						snprintf(flood_sensor_status,sizeof(not_detected),not_detected);
					}
				} else if (message[10] == SMOKE_ALARM) {
					if (message[11] == ON) {
						printf("SMOKE Detected\n");
					} else if (message[11] == OFF) {
						printf("NO Fire Danger\n");
					}
				}
			}
		}
		return RESPONSE_RECEIVED;
	}
	return 0;
}

int get_meter_level(uint8_t nodeID, uint8_t type, uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_METER, METER_GET, type, TRANSMIT_OPTION,
			callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_binary_switch_status(uint8_t nodeID, uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_SWITCH_BINARY, SWITCH_GET,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_node_power_level(uint8_t nodeID, uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_POWERLEVEL, SENSOR_GET, TRANSMIT_OPTION,
			callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int binary_switch_on_off(uint8_t nodeID, uint8_t on_off, uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SWITCH_BINARY, SWITCH_SET, on_off,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_multilevel_sensor_value(uint8_t nodeID, uint8_t sensor_type,
		uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SENSOR_MULTILEVEL_V2, MULTILEVEL_GET,
			sensor_type,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_binary_sensor_value(uint8_t nodeID, uint8_t sensor_type,
		uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SENSOR_BINARY, SENSOR_GET, sensor_type,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_battery_level(uint8_t nodeID, uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_BATTERY, BASIC_GET,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_device_status(uint8_t nodeID, uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_BASIC, BASIC_GET,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}

int get_sensor_alarm_value(uint8_t nodeID, uint8_t sensor_type,
		uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SENSOR_ALARM, SENSOR_ALARM_GET,
			sensor_type,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	//return write_to_serial_device(serial_device, message_buffer, message_length);
	add_message(message_buffer, message_length, callbackID);
	return 0;
}
