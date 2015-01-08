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

#ifndef DEVICE_H_
#define DEVICE_H_
#include "message_queue.h"
/**
 * Transmit Option for ZWave Controller
 */
#define TRANSMIT_OPTION			0x25	//Aeon USB Stick
/**
 * Staus Variable Buffers
 */
#define BUFFER_MAX 				256
#define FILE_LINE				1024
/**
 * Define Update Frequency
 */
#define UPDATE_FREQUENCY		1 	//Minutes
/**
 * Device Status XML Filename
 */
#define XML_FILE_NAME 			"/home/root/smarthome/home.xml"
#define JSON_FILE_NAME 			"/home/root/smarthome/home.json"
/**
 * Constant Strings
 */
#define status_sleeping  		"Sleeping"
#define status_active  			"Active"
#define device_on  				"ON"
#define device_off  			"OFF"
#define detected  				"DETECTED"
#define not_detected  			"NOT DETECTED"
#define door_open  				"OPEN"
#define door_closed  			"CLOSED"
/**
 * Defined Nodes
 */
#define NumberofNodes 			5
#define ControllerNodeID 		0x01
#define MultiSensorNodeID 		0x02
#define WallPlugNodeID 			0x03
#define LampHolderNodeID 		0x04
#define FloodSensorNodeID		0x05
/**
 * Device Names
 */
#define TemperatureSensorName 		"SHT11 Sensor"
#define GasSensorName 				"MQ-9 CO Sensor"
#define ControllerNodeName 			"AeonUSB Stick"
#define MultiSensorNodeName 		"Philio Multi-Sensor"
#define WallPlugNodeName 			"Fibaro Wall Plug"
#define LampHolderNodeName 			"Everspring Lamp Holder"
#define FloodSensorNodeName 		"Everspring Flood Detector"
#define NetworkCameraName			"D-Link Network Camera"
/**
 * Device Status for ZWave Nodes
 */
char multi_sensor_status[BUFFER_MAX];
char wall_plug_status[BUFFER_MAX];
char lamp_holder_status[BUFFER_MAX];
char flood_detector_status[BUFFER_MAX];
char temperature_sensor_status[BUFFER_MAX];
char gas_sensor_status[BUFFER_MAX];
/**
 * Power Level Statuses for ZWave Nodes
 */
char multi_sensor_power_level[BUFFER_MAX];
char wall_plug_power_level[BUFFER_MAX];
char lamp_holder_power_level[BUFFER_MAX];
char flood_detector_power_level[BUFFER_MAX];
/**
 * Battery Levels
 */
int multi_sensor_battery_level;
int flood_detector_battery_level;

/**
 * Current Environment Status
 */
float temperature_f;
float temperature_c;
float relative_humidity;
float illumination;
float co_level;

/**
 * Energy Meter
 */
float current_energy_consumption;
float cumulative_energy_level;

/**
 * Switch Status
 */
char lamp_holder_switch[BUFFER_MAX];
char wall_plug_switch[BUFFER_MAX];

/**
 * Security Sensor Status
 */
char flood_sensor_status[BUFFER_MAX];
char door_sensor_status[BUFFER_MAX];
char motion_sensor_status[BUFFER_MAX];
/**
 * Camera Related Status
 */
char network_camera_status[BUFFER_MAX];
char network_camera_port[BUFFER_MAX];
/**
 * Periodic update worker
 * @param null
 * @return null
 */
void* update_status(void* arg);
/**
 * Print XML File
 * @param file name
 * @return void
 */
void print_device_xml(const char* file_name);
/**
 * Print JSON File
 * @param file name
 * @return void
 */
void print_device_json(const char* file_name);

#endif /* DEVICE_H_ */
