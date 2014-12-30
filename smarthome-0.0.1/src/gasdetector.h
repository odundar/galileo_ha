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

#ifndef GASDETECTOR_H_
#define GASDETECTOR_H_

/**
 * Intel Galileo Pins
 */
#define PINMUX 37
#define ANALOGPIN 0
#define GAS_THRESHOLD 4500

/**
 * Gas Sensor Values
 */
float voltage_scale;
int gas_voltage;

/**
 * Open Analog Device File for Reading
 * @param analog pin number
 * @return device file
 */
int open_analog_device(int pin_number);
/**
 * Read Raw Value
 * @param analog pin device file
 * @return digital value read
 */
int read_analog_device_value(int device_file);
/**
 * Read Voltage Scale Value
 * One time read
 * @param pin number integer
 * @return scale float
 */
float read_voltage_scale(int pin_number);
/**
 * Read Gas Sensor Value from MQ-9 Sensor
 * @param void
 * @return read value
 */
int read_gas_sensor();

#endif /* GASDETECTOR_H_ */
