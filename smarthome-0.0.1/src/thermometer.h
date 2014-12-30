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

#ifndef THERMOMETER_H_
#define THERMOMETER_H_
/**
 * Library to support SHT11 Sensors
 */
#include <stdint.h>
#define LSBFIRST 0
#define MSBFIRST 1

#define DATA_PIN 38  //IO 7
#define CLOCK_PIN 40 //IO 8

/**
 * Read in bits from GPIO Pins with a Clock Control
 * @param bit order least significant bit first or most significant bit first (LSBFIRST/MSBFIRST)
 * @param number of bits
 * @return read bits
 */
int shiftIn(int bit_order, int n_bits);
/**
 * Write out bits to GPIO Pins with a Clock Control
 * @param bit order LSBFIRST/MSBFIRST
 * @param byte to write
 * @return void
 */
void shiftOut(uint8_t bitOrder, uint8_t val);
/**
 * Wrapping usleep function
 * @param miliseconds
 * @return void
 */
void delay(unsigned long us);
/**
 * Wrapping usleep function with converting miliseconds to microseconds
 * @param microseconds
 * @return void
 */
void delayMicroseconds(unsigned int us);
/**
 * Send Built-in Humidity Command to Sensor and Calculate Relative Humidity
 * @param void
 * @return relative humidity float
 */
float read_humidity(void);
/**
 * Send Built-in Temperature Command to Sensor and Calculate Temperature in Celcius
 * @param void
 * @return temperature float
 */
float read_temperature(void);
/**
 * Skip Checksum operation, check datasheet
 * @param void
 * @return void
 */
void skip_crc(void);

#endif /* THERMOMETER_H_ */
