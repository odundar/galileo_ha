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
/**
 * This Application Makes a Simple Communication with a Serial Device connected from
 * Intel Galileo Gen2 USB Port, this example shows how to handle raw data
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "serial.h"
//Serial Device on Linux
#define SERIAL_DEVICE "/dev/ttyUSB0"
//Serial Device File
int device_file;
/**
 * Reader Thread Worker
 */
void* reader_thread(void *arg) {
	while (1) {
		read_from_serial_device();
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	//Open Aeon Stick Z-Wave
	device_file = open_serial_device(SERIAL_DEVICE);
	if (device_file < 0) {
		return EXIT_FAILURE;
	}
	//Create Thread
	pthread_t reader;
	int err = pthread_create(&reader, NULL, &reader_thread, NULL);
	if (err != 0) {
		close_serial_device();
		printf("Can't create Thread\n");
		return -1;
	}
	//A simple Serial Command for Aeon Stick
	uint8_t get_energy_meter[] = { 0x01, 0x0a, 0x00, 0x13, 0x03, 0x03, 0x32,
			0x01, 0x00, 0x25, 0x16, 0xe6 };
	//Send Energy Meter Values
	write_to_serial_device(get_energy_meter, 12);
	if (pthread_join(reader, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	close_serial_device(device_file);
	return EXIT_SUCCESS;
}
