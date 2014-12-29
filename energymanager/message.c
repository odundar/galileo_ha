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
int handle_incoming_message(int serial_device, uint8_t message[], int length) {
	int rc = 0;
	uint8_t type = message[0];
	switch (type) {
	case SOF:
		//Handle Incoming Message
		parse_incoming_mesage(message, length);
		break;
	case ACK:
		//ACK Received
		rc = ACK;
		printf("ACK Received\n");
		uint8_t buffer[] = { ACK };
		//Send ACK to Controller
		write_to_serial_device(serial_device, buffer, 1);
		break;
	case NAK:
		break;
	case CAN:
		//Resend Last Message
		//usleep(1000);
		break;
	default:
		break;
	}
	return rc;
}

int parse_incoming_mesage(uint8_t* message, int length) {
	/**
	 * Message Length
	 */
	uint8_t length_of_rest = message[1];
	uint8_t message_type = message[2];
	uint8_t message_function = message[3];

	//TODO: Do some checks when Send Data and Callback id Received
	if (message_function == SEND_DATA && length_of_rest == 0x04) {

		printf("Data Sent to ZWave Stack\n");

	} else if (message_function == SEND_DATA && length_of_rest == 0x05) {

		printf("Data Request with Callback ID 0x%x Received\n", message[4]);

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

			printf("Battery Level of Device is %d % \n", battery);

		} else if (command_class == COMMAND_CLASS_SENSOR_BINARY) {

			if (message[8] == BINARY_REPORT) {

				if (data_length == 0x04) {

					if (message[10] == DOOR_WINDOW_SENSOR) {

						if (message[9] == ON) {
							printf("Door/Window Sensor is OPEN\n");
						} else if (message[9] == OFF) {
							printf("Door/Window Sensor is CLOSE\n");
						}

					} else if (message[10] == MOTION_DETECTION_SENSOR) {

						if (message[9] == ON) {
							printf("Motion DETECTED\n");
						} else if (message[9] == OFF) {
							printf("NO Motion Detected\n");
						}

					} else if (message[10] == WATER_DETECTION_SENSOR) {

						if (message[9] == ON) {
							printf("Water LEAK DETECTED\n");
						} else if (message[9] == OFF) {
							printf("NO Water Leak Detected\n");
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
					if (scale) {
						printf("Fahrenheit \n");
					} else {
						printf("Celcius \n");
					}

				} else if (message[9] == LUMINANCE_SENSOR) {

					printf("Illumination is %f ", value);
					if (scale) {
						printf("Lux \n");
					} else {
						printf("% \n");
					}

				} else if (message[9] == POWER_SENSOR) {

					printf("Power Value is %f \n", value);

					if (scale) {
						printf("BUT/h \n");
					} else {
						printf("Watts \n");
					}

				}
			}
		} else if (command_class == COMMAND_CLASS_BASIC) {

			printf("Device is ");

			if (message[9] == OFF) {

				printf("Sleeping\n");

			} else if (message[9] == ON) {

				printf("Active\n");

			}

		} else if (command_class == COMMAND_CLASS_SWITCH_BINARY
				|| command_class == COMMAND_CLASS_SWITCH_ALL) {

			printf("Switch is ");

			if (message[8] == SWITCH_REPORT) {
				if (message[9] == OFF) {

					printf("OFF\n");

				} else if (message[9] == ON) {

					printf("ON\n");

				}
			}
		} else if (command_class == COMMAND_CLASS_POWERLEVEL) {

			printf("Power Level of Device is %s Timeout %d\n",
					power_level_names[message[9]], message[10]);

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
				} else if (meter_type == METER_GAS) {

				} else if (meter_type == METER_WATER) {

				}
			}
		}
	}
	return 0;
}

int get_meter_level(int serial_device, uint8_t nodeID, uint8_t type,
		uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_METER, METER_GET, type, TRANSMIT_OPTION,
			callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	return write_to_serial_device(serial_device, message_buffer, message_length);
}

int get_binary_switch_status(int serial_device, uint8_t nodeID,
		uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_SWITCH_BINARY, BINARY_GET,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	return write_to_serial_device(serial_device, message_buffer, message_length);
}

int get_node_power_level(int serial_device, uint8_t nodeID, uint8_t callbackID) {
	int message_length = 11;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x02, COMMAND_CLASS_POWERLEVEL, BINARY_GET, TRANSMIT_OPTION,
			callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	return write_to_serial_device(serial_device, message_buffer, message_length);
}

int binary_switch_on_off(int serial_device, uint8_t nodeID, uint8_t on_off,
		uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SWITCH_BINARY, BINARY_SET, on_off,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	return write_to_serial_device(serial_device, message_buffer, message_length);
}

int get_multilevel_sensor_value(int serial_device, uint8_t nodeID,
		uint8_t sensor_type, uint8_t callbackID) {
	int message_length = 12;
	uint8_t checksum = 0x00;

	uint8_t message_buffer[] = { SOF, (message_length - 2), REQUEST, SEND_DATA,
			nodeID, 0x03, COMMAND_CLASS_SENSOR_MULTILEVEL_V2, MULTILEVEL_GET,
			sensor_type,
			TRANSMIT_OPTION, callbackID, checksum };

	checksum = generate_checksum(message_buffer, message_length);
	message_buffer[message_length - 1] = checksum;

	return write_to_serial_device(serial_device, message_buffer, message_length);
}

