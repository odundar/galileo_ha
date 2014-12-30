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

#include "galileo_gpio.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int gpio_export(int gpio_num) {
	//Device File Path Declarations
	const char* gpio_export = "/sys/class/gpio/export";
	//Device File Declarations
	int fd_x = 0, g_err = -1;
	//Buffer
	char g_buf[BUF];
	fd_x = open(gpio_export, O_WRONLY);
	if (fd_x < 0) {
		printf("Couldn't get export FD\n");
		return g_err;
	}
	//Export GPIO Pin
	sprintf(g_buf, "%d", gpio_num);
	if (write(fd_x, g_buf, sizeof(g_buf)) == g_err) {
		printf("Couldn't export GPIO %d\n", gpio_num);
		close(fd_x);
		return g_err;
	}
	close(fd_x);
	return 0;
}
int gpio_set_mode(int gpio_num, const char* mode) {
	//Device Direction File Path Declarations
	const char* gpio_direction_path = "/sys/class/gpio/gpio%d/direction";
	//Device File Declarations
	int fd_d = 0, g_err = -1;
	//Buffers
	char pindirection_buf[MAX_BUF];
	char d_buf[BUF];
	//Set pin number and set gpio path
	if (sprintf(pindirection_buf, gpio_direction_path, gpio_num) < 0) {
		printf("Can't create pin direction file path\n");
		return g_err;
	}
	//Open GPIO Direction File
	fd_d = open(pindirection_buf, O_WRONLY);
	//If GPIO doesn't exist then export gpio pins
	if (fd_d < 0) {
		if (gpio_export(gpio_num) < 0) {
			return g_err;
		}
		fd_d = open(pindirection_buf, O_WRONLY);
		if (fd_d <= 0) {
			printf("Couldn't get direction File for pin %d\n", gpio_num);
			return g_err;
		}
	}
	sprintf(d_buf, mode);
	if (write(fd_d, d_buf, sizeof(d_buf)) == g_err) {
		printf("Couldn't set direction for pin %d\n", gpio_num);
		return g_err;
	}
	close(fd_d);
	return 0;
}
int gpio_set_value(int gpio_num, int value) {
	//Device Direction File Path Declarations
	const char* gpio_value_path = "/sys/class/gpio/gpio%d/value";
	//Device File Declarations
	int fd_v = 0, g_err = -1;
	//Buffers
	char pinvalue_buf[MAX_BUF];
	char v_buf[BUF];
	//Set pin number and set gpio path
	if (sprintf(pinvalue_buf, gpio_value_path, gpio_num) < 0) {
		printf("Can't create pin direction file path\n");
		return g_err;
	}
	//Open GPIO Value File
	fd_v = open(pinvalue_buf, O_WRONLY);
	//If GPIO doesn't exist then export gpio pins
	if (fd_v < 0) {
		if (gpio_export(gpio_num) < 0) {
			return g_err;
		}
		fd_v = open(pinvalue_buf, O_WRONLY);
		if (fd_v <= 0) {
			printf("Couldn't get value File for pin %d\n", gpio_num);
			return g_err;
		}
	}
	sprintf(v_buf, "%d", value);
	if (write(fd_v, v_buf, sizeof(v_buf)) == g_err) {
		printf("Couldn't set value for pin %d\n", gpio_num);
		return g_err;
	}
	close(fd_v);
	return 0;
}
int gpio_get_value(int gpio_num) {
	//Device Direction File Path Declarations
	const char* gpio_value_path = "/sys/class/gpio/gpio%d/value";
	//Device File Declarations
	int fd_v = 0, g_err = -1;
	//Buffers
	char pinvalue_buf[MAX_BUF];
	char v_buf[BUF];
	//Set pin number and set gpio path
	if (sprintf(pinvalue_buf, gpio_value_path, gpio_num) < 0) {
		printf("Can't create pin direction file path\n");
		return g_err;
	}
	//Open GPIO Value File
	fd_v = open(pinvalue_buf, O_RDONLY);
	//If GPIO doesn't exist then export gpio pins
	if (fd_v < 0) {
		if (gpio_export(gpio_num) < 0) {
			return g_err;
		}
		fd_v = open(pinvalue_buf, O_RDONLY);
		if (fd_v <= 0) {
			printf("Couldn't get value File for pin %d\n", gpio_num);
			return g_err;
		}
	}
	if (read(fd_v, v_buf, 1) == g_err) {
		printf("Couldn't get value for pin %d\n", gpio_num);
		return g_err;
	}
	close(fd_v);
	return atoi(v_buf);
}

