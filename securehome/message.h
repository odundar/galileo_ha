/*
 * message.h
 *
 *  Created on: Dec 5, 2014
 *      Author: onur
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Message Buffer
 */
#define BUFFER 256

/**
 * ZWave Message
 */
#define REQUEST 										0x00
#define RESPONSE 										0x01

/**
 * ZWave Message Types with Start of Message
 */
typedef enum ZWAVE_MESSAGE_TYPE {
	SOF = 0x01, ACK = 0x06, NAK = 0x15, CAN = 0x18,
} MESSAGE_TYPE;

/**
 * Function of ZWave Message
 */
enum FUNCTION {
	SEND_DATA = 0x13, RESPONSE_RECEIVED = 0x04
};
/**
 * Sensor Type to Identify Received Data Type
 */
enum MULTILEVEL_SENSOR_TYPE {
	TEMPERATURE_SENSOR = 0x01,
	GENERAL_SENSOR = 0x02,
	LUMINANCE_SENSOR = 0x03,
	POWER_SENSOR = 0x04
};
enum BINARY_SENSOR_TYPE {
	GENERAL_PURPOSE = 0x01,
	WATER_DETECTION_SENSOR = 0x06,
	TAMPER_SENSOR = 0x08,
	DOOR_WINDOW_SENSOR = 0x0A,
	MOTION_DETECTION_SENSOR = 0x0C,
	GLASS_BREAK = 0x0D
};
/**
 * Sensor Alarm Types
 */
enum ALARM_TYPE {
	GENERAL_ALARM = 0x01,
	SMOKE_ALARM = 0x02,
	CARBON_MOXIDE_ALARM = 0x03,
	HEAT_ALARM = 0x04,
	FLOOD_ALARM = 0x05
};
/**
 * ALARM COMMANDS
 */
enum SENSOR_ALARM_COMMAND {
	SENSOR_ALARM_GET = 0x01,
	SENSOR_ALARM_REPORT = 0x02,
	SENSOR_ALARM_SUPPORTED_GET = 0x03,
	SENSOR_ALARM_SUPPORTED_REPORT = 0x04
};
/**
 * ZWave Binary ON/OFF Values
 */
enum BINARY_SENSOR_VALUE {
	ON = 0xFF, OFF = 0x00,
};
/**
 * BASIC Operations for Command Class BASIC
 */
enum BASIC_COMMANDS {
	BASIC_SET = 0x01, BASIC_GET = 0x02, BASIC_REPORT = 0x03
};
/**
 * BASIC Operations for Command Class BASIC
 */
enum SWITCH_COMMANDS {
	SWITCH_SET = 0x01, SWITCH_GET = 0x02, SWITCH_REPORT = 0x03
};
/**
 * Sensor Message Types for Binary and Multilevel
 */
enum SENSOR_COMMANDS {
	BINARY_SET = 0x01, BINARY_GET = 0x02, BINARY_REPORT = 0x03, // Response to the Get Command
	MULTILEVEL_GET = 0x04,
	MULTILEVEL_REPORT = 0x05
};
/**
 * Meter Message Type for Sensors Have Meter Property
 */
enum METER_COMMANDS {
	METER_GET = 0x01,
	METER_REPORT = 0x02,
	METER_SUPPORTED_GET = 0x03,
	METER_SUPPORTED_REPORT = 0x04,
	METER_RESET = 0x05
};
/**
 * Meter Type
 */
enum METER_TYPE {
	METER_ELECTRICITY = 0x01, METER_GAS = 0x02, METER_WATER = 0x03
};
/**
 * Meter Electricity Reading Type
 */
enum METER_ELECTRICITY_TYPE {
	ENERGY = 0x00, POWER = 0x10
};
/*
 * ZWave Command Classes
 */
enum ZWAVE_COMMAND_CLASS {
	COMMAND_CLASS_NO_OPERATION = 0x00,
	COMMAND_CLASS_BASIC = 0x20,
	COMMAND_CLASS_SWITCH_ALL = 0x27,
	COMMAND_CLASS_SWITCH_BINARY = 0x25,
	COMMAND_CLASS_METER = 0x32,
	COMMAND_CLASS_ALARM = 0x71,
	COMMAND_CLASS_SENSOR_ALARM = 0x9C,
	COMMAND_CLASS_SILENCE_ALARM = 0x71,
	COMMAND_CLASS_POWERLEVEL = 0x73,
	COMMAND_CLASS_BATTERY = 0x80,
	COMMAND_CLASS_SENSOR_BINARY = 0x30,
	COMMAND_CLASS_SENSOR_MULTILEVEL = 0x31,
	COMMAND_CLASS_SENSOR_MULTILEVEL_V2 = 0x31,
	COMMAND_CLASS_WAKE_UP = 0x84,
	COMMAND_CLASS_CONTROLLER_REPLICATION = 0x21,
	COMMAND_CLASS_APPLICATION_STATUS = 0x22,
	COMMAND_CLASS_HAIL = 0x82
};

/**
 * Parse Incoming Messages and Create Human Readable Output
 * @param message buffer
 * @param message buffer length
 * @return success/error
 */
int parse_incoming_mesage(uint8_t* message, int length);
/**
 * Handle incoming message to send message to parse
 * @param serial device file
 * @param message buffer
 * @param message length
 * @return success/error
 */
int handle_incoming_message(int serial_device, uint8_t message[], int length);
/**
 * Set Binary Switch on/off
 * @param serial device file
 * @param node id
 * @param switch value ON, OFF
 * @param callback id
 * @return success/failure
 */
int binary_switch_on_off(int serial_device, uint8_t nodeID, uint8_t on_off,
		uint8_t callbackID);
/**
 * Request the Meter Value from given node
 * @param serial device file
 * @param node id
 * @param meter type
 * @param callback id
 * @return success/failure
 */
int get_meter_level(int serial_device, uint8_t nodeID, uint8_t type,
		uint8_t callbackID);
/**
 * Request the status of binary switch device
 * @param serial device file
 * @param node id
 * @param callback id
 * @return success/failure
 */
int get_binary_switch_status(int serial_device, uint8_t nodeID,
		uint8_t callbackID);
/**
 * Request Power Level
 * @param serial device file
 * @param node id
 * @param callback id
 * @return success/failure
 */
int get_node_power_level(int serial_device, uint8_t nodeID, uint8_t callbackID);
/**
 * Request Battery Level of Remote Device
 * @param serial device file
 * @param node id
 * @param callback id
 * @return succes/failure
 */
int get_battery_level(int serial_device, uint8_t nodeID, uint8_t callbackID);
/**
 * Request Multilevel Sensor Value such as (Temperature, Luminance etc)
 * @param serial device file
 * @param node id
 * @param multilevel sensor type
 * @param callback id
 * return success/failure
 */
int get_multilevel_sensor_value(int serial_device, uint8_t nodeID,
		uint8_t sensor_type, uint8_t callbackID);
/**
 * Request Binary Sensor Value such as (Door, Window, Motion etc)
 * @param serial device file
 * @param node id
 * @param Binary sensor type
 * @param callback id
 * return success/failure
 */
int get_binary_sensor_value(int serial_device, uint8_t nodeID,
		uint8_t sensor_type, uint8_t callbackID);
/**
 * Request Basic Device Status
 * @param serial device file
 * @param node id
 * @param callback id
 * return success/failure
 */
int get_device_status(int serial_device, uint8_t nodeID, uint8_t callbackID);
/**
 * Request Sensor Alarm Value such as (Flood, Fire etc)
 * @param serial device file
 * @param node id
 * @param sensor type
 * @param callback id
 * return success/failure
 */
int get_sensor_alarm_value(int serial_device, uint8_t nodeID,
		uint8_t sensor_type, uint8_t callbackID);
#endif /* MESSAGE_H_ */
