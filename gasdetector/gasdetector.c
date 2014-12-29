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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "galileo_gpio.h"
/**
 * Intel Galileo Pins
 */
#define PINMUX 37
#define ANALOGPIN 0
/**
 * Analog Device File Operations
 */
int open_analog_device(int pin_number);
int read_analog_device_value(int device_file);
float read_voltage_scale(int pin_number);

int main(void) {
	/**
	 * Read from Analog 0, mux GPIO Pin 37 to read voltage values
	 */
	if (gpio_set_mode(PINMUX, OUTPUT) < 0) {
		printf("Can't Set GPIO Mux Pin Mode\n");
		return EXIT_FAILURE;
	}
	if (gpio_set_value(PINMUX, LOW) < 0) {
		printf("Can't Set GPIO Mux Pin Value\n");
		return EXIT_FAILURE;
	}
	printf("Pin Mux Successful\n");
	/**
	 * Read Digital Value Scale
	 */
	float v_scale = read_voltage_scale(ANALOGPIN);
	printf("Voltage Scale is %f\n", v_scale);
	/**
	 * Open Analog Device
	 */
	int analog = open_analog_device(ANALOGPIN);
	if (analog > -1) {
		printf("Analog IO File Opened Successfully\n");
	}
	/**
	 * Read Voltage Values from Analog 0
	 */
	while (1) {
		printf("Voltage : %d \n", read_analog_device_value(analog));
		usleep(1000 * 1000);
	}
	close(analog);
	return 0;
}
/**
 * Open Analog Device File for Reading
 * @param analog pin number
 * @return device file
 */
int open_analog_device(int pin_number) {
	//Analog Device Values Read from
	const char* analog_file_path =
			"/sys/bus/iio/devices/iio\:device0/in_voltage%d_raw";
	int fanalog, a_err = -1;
	char analog_file_buffer[MAX_BUF];
	//Set analog reading file path and pin number
	if (sprintf(analog_file_buffer, analog_file_path, pin_number) < 0) {
		printf("Can't create analog file path\n");
		return a_err;
	}
	fanalog = open(analog_file_buffer, O_RDONLY);
	if (fanalog < 0) {
		printf("Can't open analog device\n");
		return a_err;
	}
	return fanalog;
}
/**
 * Read Raw Value
 * @param analog pin device file
 * @return digital value read
 */
int read_analog_device_value(int device_file) {
	char buf[1024];
	int a_err = -1;
	int ret = read(device_file, buf, sizeof(buf));
	if (ret == a_err) {
		printf("Couldn't get value from Analog Pin\n");
		return -1;
	}
	buf[ret] = '\0';
	lseek(device_file, 0, 0);
	return atoi(buf);
}
/**
 * Read Voltage Scale Value
 * One time read
 * @param pin number integer
 * @return scale float
 */
float read_voltage_scale(int pin_number) {
	//Analog Device Values Read from
	const char* scale_file_path =
			"/sys/bus/iio/devices/iio\:device0/in_voltage%d_scale";
	int fscale, a_err = -1;
	char scale_file_buffer[MAX_BUF];
	//Set analog reading file path and pin number
	if (sprintf(scale_file_buffer, scale_file_path, pin_number) < 0) {
		printf("Can't create scale file path\n");
		return a_err;
	}
	fscale = open(scale_file_buffer, O_RDONLY);
	if (fscale < 0) {
		printf("Can't open scale device\n");
		return a_err;
	}
	char buf[1024];
	int ret = read(fscale, buf, sizeof(buf));
	if (ret == a_err) {
		printf("Couldn't get value from Analog Pin\n");
		return -1;
	}
	buf[ret] = '\0';
	return atof(buf);
}
