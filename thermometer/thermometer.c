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
 * This Application is written to communicate with Sensirion SHT11 Temperature and Humidity Sensor
 * from Intel Galile Gen 2 GPIO Pins
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>

//Bit Order
#define LSBFIRST 0
#define MSBFIRST 1

#define HIGH 1
#define LOW 0
#define INPUT "in"
#define OUTPUT "out"
//GPIOs
#define BUF 8
#define MAX_BUF 256
#define DATA_PIN 38  //Intel Galileo Gen2 IO 7
#define CLOCK_PIN 40 //Intel Galileo Gen2 IO 8

//SHT11 Commands from Datasheet
const uint8_t measure_temperature = 0b00000011;
const uint8_t measure_humidity = 0b00000101;
const uint8_t read_status = 0b00000111;
const uint8_t write_status = 0b00000110;

//Temperature Measurement Constants as in Datasheet
// T =  d1 + d2 * SOt
const float D1_5V = -40.1;
const float D2_14bit = 0.01;
const float D2_12bit = 0.04;
//Relative Humidity Measurement Constants as in Datasheet
// RH = c1 + c2 * SOrh + c3 * SOrh * SOrh
const float C1 = -2.0468;
const float C2 = 0.0367;
const float C3 = -0.00395484;
//Temperature and Relative Humidity Storage
float temperature = 0.0; //Celcius
float humidity = 0.0;	//Percentage

//GPIO Function Declarations
int gpio_set_value(int gpio_num, int value);
int gpio_set_mode(int gpio_num, const char* mode);
int gpio_export(int gpio_num);
int gpio_get_value(int gpio_num);
//Byte/Bit Reading
int shiftIn(int bit_order, int n_bits);
void shiftOut(uint8_t bitOrder, uint8_t val);
void skip_crc();
//Timing Function Declarations
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
//Sensor Reading Function Declarations
void read_humidity();
void read_temperature();

int main(void) {
	while (1) {
		//Delay 1 Second to Read Temperature and Humidity
		delay(1000);
		read_humidity();
		read_temperature();
		printf("Temperature: %f Celcius \n"
				"Relative Humidity: %f\n", temperature, humidity);
		char c;
	}
	return 0;
}
/**
 * Export GPIO Pin Driver Files
 * @param gpio_num integer gpio number in linux
 * @return success/error
 */
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
/**
 * Set GPIO Mode In/Out
 * @param gpio_num integer gpio number in linux
 * @param mode string to define direction "in" or "out"
 * @return success/error
 */
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
/**
 * GPIO Set Value
 * @param gpio_num integer gpio number in linux
 * @param value integer LOW/HIGH to set
 * @return success/error
 */
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
/**
 * Get current value of GPIO
 * @param gpio_num integer gpio number in linux
 * @return HIGH/LOW
 */
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

/**
 * Write digital value to GPIO Data Pin, bit by bt
 * @param bitorder LSB/MSB
 * @param 1 byte, 8 bit
 * @return void
 */
void shiftOut(uint8_t bitOrder, uint8_t val) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if (bitOrder == LSBFIRST) {
			gpio_set_value(DATA_PIN, !!(val & (1 << i)));
		} else {
			gpio_set_value(DATA_PIN, !!(val & (1 << ((8 - 1 - i)))));
			gpio_set_value(CLOCK_PIN, HIGH);
			delayMicroseconds(80);
			gpio_set_value(CLOCK_PIN, LOW);
		}
	}
}

/**
 * Read bits from given GPIO Data Pin
 * @param bitorder LSB/MSB
 * @param number of bits
 * @return value
 */
int shiftIn(int bit_order, int n_bits) {
	int ret = 0;
	int i;
	gpio_set_value(CLOCK_PIN, LOW);
	for (i = 0; i < n_bits; ++i) {
		gpio_set_value(CLOCK_PIN, HIGH);
		if (bit_order == LSBFIRST) {
			ret |= gpio_get_value(DATA_PIN) << i;
		} else {
			ret |= gpio_get_value(DATA_PIN) << n_bits - i;
		}
		delayMicroseconds(20);
		gpio_set_value(CLOCK_PIN, LOW);
	}
	return (ret);
}

/**
 * suspend execution for milisecond intervals
 * @param miliseconds
 * @return void
 */
void delay(unsigned long ms) {
	usleep(ms * 1000);
}

/**
 * suspend execution for microsecond intervals
 * @param microseconds
 */
void delayMicroseconds(unsigned int us) {
	usleep(us);
	return;
}

/**
 * Function to send a command (byte/s) to SHT11 Sensor
 * See the SHT11 Datasheet for execution sequence
 * @param command as integer
 * @return success/error
 */
int send_command_SHT11(int command) {
	int ack;
	//Start Transmission
	gpio_set_mode(DATA_PIN, OUTPUT);
	gpio_set_mode(CLOCK_PIN, OUTPUT);
	gpio_set_value(DATA_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_value(DATA_PIN, LOW);
	gpio_set_value(CLOCK_PIN, LOW);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_value(DATA_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, LOW);
	//Shift Out
	shiftOut(MSBFIRST, command);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_mode(DATA_PIN, INPUT);
	delay(20);
	ack = gpio_get_value(DATA_PIN);
	if (ack != LOW) {
		perror("ACK Error 0");
	}
	gpio_set_value(CLOCK_PIN, LOW);
	delay(320);
	ack = gpio_get_value(DATA_PIN);
	if (ack != HIGH) {
		perror("ACK Error 1");
		gpio_set_mode(DATA_PIN, OUTPUT);
		gpio_set_value(DATA_PIN, HIGH);
		gpio_set_mode(DATA_PIN, INPUT);
	}
	return 0;
}
/**
 * Function to wait for data ready signal from SHT11
 * See the data sheet for data ready signal from SHT11
 * @param void
 * @return void
 */
void wait_for_result() {
	int ack;
	gpio_set_mode(DATA_PIN, INPUT);
	int i;
	for (i = 0; i < 100; i++) {
		delay(20);
		ack = gpio_get_value(DATA_PIN);
		if (ack == LOW) {
			printf("Sensor Data Ready\n");
			break;
		}
	}
	if (ack == HIGH) {
		perror("ACK error 2");
	}
}
/**
 * Read the result byte/s from SHT11 sensor
 * See the SHT11 datasheet for execution sequence
 * @param void
 * @return result
 */
int retreive_data_SHT11() {
	int16_t value;
	gpio_set_mode(DATA_PIN, INPUT);
	gpio_set_mode(CLOCK_PIN, OUTPUT);
	value = shiftIn(MSBFIRST, 8);
	value *= 256;
	//Send ACK
	gpio_set_mode(DATA_PIN, OUTPUT);
	gpio_set_value(DATA_PIN, HIGH);
	gpio_set_value(DATA_PIN, LOW);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, LOW);
	//Get LSB
	gpio_set_mode(DATA_PIN, INPUT);
	value |= shiftIn(MSBFIRST, 8);
	return value;
}
/**
 * Sequence of bits to skip CRC from Sensor
 * See the SHT11 Datasheet for execution sequence
 * @param void
 * @return void
 */
void skip_crc() {
	gpio_set_mode(DATA_PIN, OUTPUT);
	gpio_set_mode(CLOCK_PIN, OUTPUT);
	gpio_set_value(DATA_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, LOW);
}
/**
 * Function to execute temperature command and calcuate human readable celcius value and store
 * See the SHT11 Datasheet for calculation formulas and coefficients
 * @param void
 * @return void
 */
void read_temperature() {
	// Conversion coefficients from SHT11 datasheet
	send_command_SHT11(measure_temperature);
	wait_for_result();
	int temp = retreive_data_SHT11();
	skip_crc();
	temperature = 0.0;
	temperature = ((float) temp * D2_14bit) + D1_5V;
}
/**
 * Function to execute relative humidity command and calculate human readable realative humidity of environment as percentage
 * See the SHT11 Datasheet for calculation formulas and coefficients
 * @param void
 * @return void
 */
void read_humidity() {
	send_command_SHT11(measure_temperature);
	wait_for_result();
	int hum = retreive_data_SHT11();
	skip_crc();
	humidity = 0.0;
	humidity = C1 + (C2 * (float) hum) + (C3 * (float) hum * (float) hum);
}
