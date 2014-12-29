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

#ifndef SERIAL_H_
#define SERIAL_H_
#include <stdint.h>
/**
 * This Library, Serial.h
 * Defined to Create a Serial Connection with ZWave Serial USB Adapter
 */

/**
 * Open Serial Controller and Settings
 * @param serial device file path "/dev/ttyUSB0"
 * @return device file
 */
int open_serial_device(const char* serial_device_path);
/**
 * Close Controller File
 * @param device file integer
 * @return success
 */
int close_serial_device(int device_file);
/**
 * Read From Serial Controller
 * @param device file integer
 * @param data address pointer
 * @return number of bytes read
 */
int read_from_serial_device(int device_file, uint8_t* data);
/**
 * Write to Serial Controller
 * @param byte buffer to write
 * @param length of buffer
 * @return number of bytes written
 */
int write_to_serial_device(int device_file, uint8_t buffer[], int length);

#endif /* SERIAL_H_ */
