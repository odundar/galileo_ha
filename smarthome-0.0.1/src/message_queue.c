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

#include "message_queue.h"
#include <stdlib.h>
#include <stdio.h>

void add_message(uint8_t* message, int length, uint8_t callback_id) {
	struct message *temp = (struct message *) malloc(sizeof(struct message));
	temp->length = length;
	temp->callback_id = callback_id;
	temp->zwave_message = (uint8_t *) malloc(sizeof(uint8_t) * length);
	int i = 0;
	for (; i < length; i++) {
		temp->zwave_message[i] = message[i];
	}
	temp->next = NULL;
	if (message_list == NULL) {
		message_list = temp;
		number_of_messages = 1;
	} else {
		struct message* cur = message_list;
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = temp;
		number_of_messages++;
	}
	return;
}

void pop_message(uint8_t* message, int* length, uint8_t* callback_id) {
	if (message_list != NULL) {
		int i = 0;
		*length = message_list->length;
		*callback_id = message_list->callback_id;
		//message = (uint8_t *) malloc(sizeof(uint8_t) * message_list->length);
		for (; i < *length; i++) {
			message[i] = message_list->zwave_message[i];
		}
		message_list = message_list->next;
		number_of_messages--;
	}
	return;
}
