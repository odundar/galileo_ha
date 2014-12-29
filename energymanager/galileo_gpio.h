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

#ifndef GALILEO_GPIO_H_
#define GALILEO_GPIO_H_
/**
 * Library to support GPIO Read/Write operations for Intel Galileo Gen2
 */
//GPIO VALUES
#define HIGH 1
#define LOW 0
#define INPUT "in"
#define OUTPUT "out"
//GPIO BUFFER
#define BUF 8
#define MAX_BUF 256

/**
 * Set GPIO Pin Value
 * @param gpio number
 * @param HIGH/LOW value
 * @return success / failure
 */
int gpio_set_value(int gpio_num, int value);
/**
 * Set GPIO Direction
 * @param gpio number
 * @param IN/OUT value
 * @return success / failure
 */
int gpio_set_mode(int gpio_num, const char* mode);
/**
 * Create GPIO Driver
 * @param gpio number
 * @return success / failure
 */
int gpio_export(int gpio_num);
/**
 * Get current value of GPIO
 * @param gpio number
 * @return gpio value as integer LOW/HIGH
 */
int gpio_get_value(int gpio_num);

#endif /* GALILEO_GPIO_H_ */
