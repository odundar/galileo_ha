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
 * Function Definitions of serial.h
 */
#include "serial.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

int open_serial_device(const char* serial_device_path) {
	//Open Device File /dev/ttyUSB0
	//Function is for AEON ZStick
	int device_file = open(serial_device_path, O_RDWR | O_NOCTTY, 0);
	if (device_file < 0) {
		printf("Can't Open Serial Controller\n");
		return -1;
	}
	struct termios options;
	int bits;
	bits = 0;
	ioctl(device_file, TIOCMSET, &bits);
	// Get Current Options
	tcgetattr(device_file, &options);
	// No Parity
	options.c_iflag = IGNPAR;
	options.c_iflag |= IGNBRK;
	// Set 8 Data bits
	options.c_cflag |= CS8 | CREAD | CLOCAL;
	options.c_oflag = 0;
	options.c_lflag = 0;
	int i;
	for (i = 0; i < NCCS; i++) {
		options.c_cc[i] = 0;
	}
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 1;
	// Set Baudrate for Serial Communication
	cfsetspeed(&options, B115200);
	if (tcsetattr(device_file, TCSANOW, &options) < 0) {
		printf("Can't Set The Serial Device Parameters\n");
	} else {
		printf("Successfully Set the Serial Device Parameters\n");
	}
	// Flush the waiting bits/bytes
	tcflush(device_file, TCIOFLUSH);
	return device_file;
}

int close_serial_device(int device_file) {
	return close(device_file);
}

int read_from_serial_device(int device_file, uint8_t *data) {
	int bytesRead;
	uint8_t buffer[256];
	//Read Data From Serial Device to Buffer
	bytesRead = read(device_file, buffer, sizeof(buffer));
	//Print Bytes
	if (bytesRead > 0) {
		int i = 0;
		//printf("Received: ");
		for (; i < bytesRead; i++) {
			data[i] = buffer[i];
			//printf("0x%x, ", data[i]);
		}
		//printf("\n");
	}
	return bytesRead;
}

int write_to_serial_device(int device_file, uint8_t buffer[], int length) {
	int count = 0;
	//printf("Writing: ");
	//Write Byte by Byte
	while (count < length) {
		if (write(device_file, &buffer[count], 1) == 1) {
			//printf("0x%x, ", buffer[count]);
			count++;
		} else {
			printf("\n Can't Write Byte\n");
			break;
		}
	}
	if (count == length) {
		//printf("\nWrite Successful\n");
		return count;
	} else {
		printf("\nCan't Write All Frame to Serial Device\n");
		return -1;
	}
}
