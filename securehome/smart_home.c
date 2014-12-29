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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <time.h>
#include "serial.h"
#include "thermometer.h"
#include "message.h"
#include "device.h"

int device_file;
const char* serial_device_path = "/dev/ttyUSB0";

pthread_t reader;
pthread_t device;

void execute_command(int choice);
void user_menu();
uint8_t pick_node();
uint8_t pick_binary_sensor();
uint8_t pick_multilevel_sensor();
uint8_t switch_on_off();

void* reader_thread(void *arg) {
	while (1) {
		uint8_t data[256];
		int m_length = read_from_serial_device(device_file, data);
		if (m_length > 0) {
			handle_incoming_message(device_file, data, m_length);
		}
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	//Open Serial Device
	device_file = open_serial_device(serial_device_path);
	if (device_file < 0) {
		printf("Can't Open Serial Device %s", serial_device_path);
		return EXIT_FAILURE;
	}
	/**
	 * Create Reader Thread
	 */
	int err = pthread_create(&reader, NULL, &reader_thread, NULL);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Thread\n");
		return -1;
	}
	/**
	 * Create Device Thread
	 */
	err = pthread_create(&reader, NULL, &update_status, device_file);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Thread\n");
		return -1;
	}
	printf("Starting Home Manager\n");
	int choice = 0;
	user_menu();
	while (choice != 14) {
		printf("Enter Command to Execute:");
		scanf("%d", &choice);
		execute_command(choice);
	}

	if (pthread_join(reader, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	close_serial_device(device_file);

	return EXIT_SUCCESS;
}

void execute_command(int choice) {
	uint8_t nodeid;
	uint8_t sensor;
	switch (choice) {
	case 1:
		printf("Current Temperature is: %f Celcius\n", read_temperature());
		break;
	case 2:
		printf("Relative Humidity is  : %f percent \n", read_humidity());
		break;
	case 3:
		nodeid = pick_node();
		get_device_status(device_file, nodeid, 3);
		break;
	case 4:
		nodeid = pick_node();
		get_binary_switch_status(device_file, nodeid, 4);
		break;
	case 5:
		nodeid = pick_node();
		sensor = pick_binary_sensor();
		get_binary_sensor_value(device_file, nodeid, sensor, 5);
		break;
	case 6:
		nodeid = pick_node();
		sensor = pick_multilevel_sensor();
		get_multilevel_sensor_value(device_file, nodeid, sensor, 6);
		break;
	case 7:
		nodeid = pick_node();
		get_node_power_level(device_file, nodeid, 7);
		break;
	case 8:
		nodeid = pick_node();
		get_battery_level(device_file, nodeid, 8);
		break;
	case 9:
		get_meter_level(device_file, WallPlugNodeID, POWER, 9);
		break;
	case 10:
		get_meter_level(device_file, WallPlugNodeID, ENERGY, 10);
		break;
	case 11:
		nodeid = pick_node();
		uint8_t on_off = switch_on_off();
		binary_switch_on_off(device_file, nodeid, on_off, 10);
		break;
	case 12:
		printf("Quitting.....");
		break;
	default:
		break;
	}
}

void user_menu() {
	printf("1  : Show Current Temperature						\n");
	printf("2  : Show Current Relative Humidity					\n");
	printf("3  : Get Current Status Node						\n");
	printf("4  : Get Current Switch Status on Node				\n");
	printf("5  : Get Current Sensor Status on Node			  	\n");
	printf("6  : Get Current Multilevel Sensor Status on Node 	\n");
	printf("7  : Get Power Level of Node						\n");
	printf("8  : Get Device Battery Level						\n");
	printf("9  : Get Current Power Consumption					\n");
	printf("10 : Get Energy Consumption 						\n");
	printf("11 : Switch On-Off Node							\n");
	printf("12 : Quit											\n");
}

uint8_t pick_node() {
	int node;
	printf("Multi-Sensor Node   :1\n");
	printf("Wall-Plug           :2\n");
	printf("Lamp Holder         :3\n");
	printf("Flood Detector      :4\n");
	printf("Select Device : ");
	scanf("%d", &node);
	switch (node) {
	case 1:
		node = MultiSensorNodeID;
		break;
	case 2:
		node = WallPlugNodeID;
		break;
	case 3:
		node = LampHolderNodeID;
		break;
	case 4:
		node = FloodSensorNodeID;
		break;
	default:
		printf("invalid node\n");
		node = 1;
		break;
	}
	return (uint8_t) node;
}
uint8_t pick_binary_sensor() {
	int sensor;
	printf("Tamper              :1\n");
	printf("Door/Window         :2\n");
	printf("Motion              :3\n");
	printf("Select Binary Sensor: ");
	scanf("%d", &sensor);
	switch (sensor) {
	case 1:
		sensor = TAMPER_SENSOR;
		break;
	case 2:
		sensor = DOOR_WINDOW_SENSOR;
		break;
	case 3:
		sensor = MOTION_DETECTION_SENSOR;
		break;
	default:
		printf("invalid sensor\n");
		sensor = 1;
		break;
	}
	return (uint8_t) sensor;
}
uint8_t pick_multilevel_sensor() {
	int sensor;
	printf("Temperature         :1\n");
	printf("Illumination        :2\n");
	printf("Power               :3\n");
	printf("Select MultiLevel Sensor: ");
	scanf("%d", &sensor);
	switch (sensor) {
	case 1:
		sensor = TEMPERATURE_SENSOR;
		break;
	case 2:
		sensor = LUMINANCE_SENSOR;
		break;
	case 3:
		sensor = POWER_SENSOR;
		break;
	default:
		printf("invalid sensor\n");
		sensor = 1;
		break;
	}
	return (uint8_t) sensor;
}
uint8_t switch_on_off() {
	int res;
	printf("Switch ON   : 1\n");
	printf("Switch OFF  : 0\n");
	printf("Select: ");
	scanf("%d", &res);
	if (res == 1) {
		res = ON;
	} else if (res == 0) {
		res = OFF;
	} else {
		printf("invalid\n");
		res = -1;
	}
	return (uint8_t) res;
}

