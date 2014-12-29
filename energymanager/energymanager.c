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

int device_file;
const char* serial_device_path = "/dev/ttyUSB0";

pthread_t reader;

void execute_command(int choice);
void user_menu();

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
	int err = pthread_create(&reader, NULL, &reader_thread, NULL);
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
	switch (choice) {
	case 1:
		printf("Current Temperature is: %f Celcius\n", read_temperature());
		break;
	case 2:
		printf("Relative Humidity is  : %f % \n", read_humidity());
		break;
	case 3:
		get_binary_switch_status(device_file, LampHolderNodeID, 1);
		break;
	case 4:
		binary_switch_on_off(device_file, LampHolderNodeID, ON, 2);
		break;
	case 5:
		binary_switch_on_off(device_file, LampHolderNodeID, OFF, 3);
		break;
	case 6:
		get_binary_switch_status(device_file, WallPlugNodeID, 4);
		break;
	case 7:
		binary_switch_on_off(device_file, WallPlugNodeID, ON, 5);
		break;
	case 8:
		binary_switch_on_off(device_file, WallPlugNodeID, OFF, 6);
		break;
	case 9:
		get_meter_level(device_file, WallPlugNodeID, POWER, 7);
		break;
	case 10:
		get_meter_level(device_file, WallPlugNodeID, ENERGY, 8);
		break;
	case 11:
		get_node_power_level(device_file, WallPlugNodeID, 9);
		break;
	case 12:
		get_node_power_level(device_file, LampHolderNodeID, 10);
		break;
	case 13:
		get_multilevel_sensor_value(device_file, MultiSensorNodeID,
				LUMINANCE_SENSOR, 11);
		break;
	case 14:
		printf("Quitting.....");
		break;
	default:
		break;
	}
}

void user_menu() {
	printf("1 : Show Current Temperature			\n");
	printf("2 : Show Current Relative Humidity		\n");
	printf("3 : Get Current Status of Lamp Holder	\n");
	printf("4 : Switch On Lamp Holder				\n");
	printf("5 : Switch Off Lamp Holder 				\n");
	printf("6 : Get Current Status of Wall Plug		\n");
	printf("7 : Switch On Wall Plug 				\n");
	printf("8 : Switch Off Wall Plug 				\n");
	printf("9 : Get Current Power Consumption		\n");
	printf("10: Get Energy Consumption 				\n");
	printf("11: Get Power Level of Wall Plug		\n");
	printf("12: Get Power Level of Lampholder		\n");
	printf("13: Get Luminance of Environment		\n");
	printf("14: Quit								\n");
}
