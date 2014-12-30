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
#include "thermometer.h"
#include "galileo_gpio.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//SHT11 Commands
uint8_t measure_temperature = 0b00000011;
uint8_t measure_humidity = 0b00000101;
uint8_t read_status = 0b00000111;
uint8_t write_status = 0b00000110;

//Temperature Measurement Constants as in Datasheet
// T =  d1 + d2 * SOt
float D1_5V = -40.1;
float D2_14bit = 0.01;
float D2_12bit = 0.04;
//Relative Humidity Measurement Constants as in Datasheet
// RH = c1 + c2 * SOrh + c3 * SOrh * SOrh
float C1 = -2.0468;
float C2 = 0.0367;
float C3 = -0.00395484;

void delay(unsigned long ms) {
	usleep((ms * 1000));
	return;
}

void delayMicroseconds(unsigned int us) {
	usleep(us);
	return;
}

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

void skip_crc() {
	gpio_set_mode(DATA_PIN, OUTPUT);
	gpio_set_mode(CLOCK_PIN, OUTPUT);
	gpio_set_value(DATA_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, HIGH);
	gpio_set_value(CLOCK_PIN, LOW);
}

float read_temperature() {
	// Conversion coefficients from SHT11 datasheet
	float temperature = 0.0; //Celcius
	send_command_SHT11(measure_temperature);
	wait_for_result();
	int temp = retreive_data_SHT11();
	skip_crc();
	temperature = 0.0;
	temperature = ((float) temp * D2_14bit) + D1_5V;
	return temperature;
}
float read_humidity() {
	// Conversion coefficients from SHT11 datasheet
	float humidity = 0.0;
	send_command_SHT11(measure_humidity);
	wait_for_result();
	int hum = retreive_data_SHT11();
	skip_crc();
	humidity = 0.0;
	humidity = C1 + (C2 * (float) hum) + (C3 * (float) hum * (float) hum);
	return humidity;
}
int shiftIn(int bit_order, int n_bits) {
	int ret = 0;
	int i;
	gpio_set_value(CLOCK_PIN, LOW);
	for (i = 0; i < n_bits; ++i) {
		gpio_set_value(CLOCK_PIN, HIGH);
		if (bit_order == LSBFIRST) {
			ret |= gpio_get_value(DATA_PIN) << i;
		} else {
			ret |= gpio_get_value(DATA_PIN) << (n_bits - i);
		}
		delayMicroseconds(20);
		gpio_set_value(CLOCK_PIN, LOW);
	}
	return (ret);
}
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
