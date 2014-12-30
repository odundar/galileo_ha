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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#include "serial.h"
#include "thermometer.h"
#include "message.h"
#include "device.h"
#include "message_queue.h"
#include "socket_listener.h"
#include "camera.h"

//Named Pipe
#define FIFO_FILE       "SMARTHOMEPIPE"
#define USERBUF			80

//Serial Device
int device_file;
const char* serial_device_path = "/dev/ttyUSB0";

//Read Incoming ZWave Messages
pthread_t zwave_handler;
//Periodically Add the ZWave Update Messages to Message Queue
pthread_t update;
//Read Named Pipe for User Requests
pthread_t pipe_handler;
//Read Requests from Network Socket
pthread_t socket_listener;

/**
 * Available Commands
 */
const char* user_commands[] = { "show home status\n", "switch on 3\n",
		"switch off 3\n", "switch on 4\n", "switch off 4\n", "capture frame\n",
		"record video\n", "help\n" };
const int number_of_commands = 8;
//User Command
const char* quit = "quit\n";
char command[USERBUF] = { 0 };
void execute_command(char* user_request);

/**
 * Handle ZWave Controller for Incoming and Outgoing Messages
 */
void* zwave_message_handler(void* arg) {
	int expecting_reply = 0;
	int count = 0;
	int m_length = 0;
	int length;
	uint8_t callback_id;
	uint8_t* last_message;
	uint8_t ack_buffer[] = { ACK };
	int res;
	while (1) {
		uint8_t data[256];
		m_length = read_from_serial_device(device_file, data);
		if (m_length > 0 && expecting_reply == 0) {
			parse_incoming_mesage(data, m_length);
			write_to_serial_device(device_file, ack_buffer, 1);
			update_device_xml(XML_FILE_NAME);
		} else if (m_length > 0 && expecting_reply == 1) {
			//Check Incoming Message Type and Send ACK When it is Received
			switch (data[0]) {
			case SOF:
				//Parse Incoming Message
				res = parse_incoming_mesage(data, m_length);
				count++;
				if (res == SEND_DATA) {
					write_to_serial_device(device_file, ack_buffer, 1);
				} else if (res == callback_id) {
					write_to_serial_device(device_file, ack_buffer, 1);
				} else if (res == RESPONSE_RECEIVED) {
					//Reset Flags
					count = 0;
					expecting_reply = 0;
					write_to_serial_device(device_file, ack_buffer, 1);
				}
				break;
			case ACK:
				//ACK Received
				count++;
				break;
			case NAK:
				break;
			case CAN:
				//Resend Last Message
				if (length > 0) {
					write_to_serial_device(device_file, last_message, length);
				}
				break;
			default:
				break;
			}
		} else if (m_length == 0 && expecting_reply == 1) {
			//In case no response from corresponding node received,
			expecting_reply = 0;
		} else if (m_length == 0 && expecting_reply == 0
				&& number_of_messages > 0) {
			//Get Message From Queue
			uint8_t buffer[256];
			pop_message(buffer, &length, &callback_id);
			//Write Message to Serial Device
			write_to_serial_device(device_file, buffer, length);
			//Raise Flag to Receive Expected Messages
			expecting_reply = 1;
			last_message = buffer;
		}
	}
	return NULL;
}

/**
 * Handle User Requests from PIPE
 */
void* named_pipe_handler(void* arg) {
	FILE *fp;
	char readbuf[USERBUF];

	/* Create the FIFO if it does not exist */
	umask(0);
	mknod(FIFO_FILE, S_IFIFO | 0666, 0);
	while (1) {
		fp = fopen(FIFO_FILE, "r");
		if (fgets(readbuf, USERBUF, fp) != NULL) {
			snprintf(command, sizeof(readbuf), readbuf);
			memset(readbuf,0,sizeof(readbuf));
		}
		fclose(fp);
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	//Open Serial Device
	device_file = open_serial_device(serial_device_path);
	if (device_file < 0) {
		printf("Can't Open Serial Device %s", serial_device_path);
		return EXIT_FAILURE;
	}
	/**
	 * Create Reader Thread
	 */
	int err = pthread_create(&zwave_handler, NULL, &zwave_message_handler,
	NULL);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Reader Thread\n");
		return -1;
	}
	/**
	 * Create Home Status Updater Thread
	 */
	err = pthread_create(&update, NULL, &update_status, NULL);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Device Thread\n");
		return -1;
	}
	/**
	 * Create Named Pipe Handler Thread Thread
	 */
	err = pthread_create(&pipe_handler, NULL, &named_pipe_handler, NULL);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Pipe Handler Thread\n");
		return -1;
	}
	/**
	 * Create Socket Listener
	 */
	err = pthread_create(&socket_listener, NULL, &socket_worker, command);
	if (err != 0) {
		close_serial_device(device_file);
		printf("Can't create Device Thread\n");
		return -1;
	}
	while (1) {
		if (command[0] != 0 && strcmp(quit, command) != 0) {
			execute_command(command);
			memset(command, 0, sizeof(command));
		}else if(strcmp(quit, command) == 0){
			break;
		}
	}
	printf("Quitting....\n");
	pthread_cancel(zwave_handler);
	pthread_cancel(socket_listener);
	pthread_cancel(pipe_handler);
	pthread_cancel(update);
	close_serial_device(device_file);

	return EXIT_SUCCESS;
}

void execute_command(char* user_request) {

	int i = 0;
	for (; i < number_of_commands; i++) {
		if (strcmp(user_request, user_commands[i]) == 0) {
			break;
		}
	}
	if (i < (number_of_commands - 1)) {
		switch (i) {
		case 0:
			print_device_xml(XML_FILE_NAME);
			break;
		case 1:
			binary_switch_on_off(WallPlugNodeID, ON, 20);
			break;
		case 2:
			binary_switch_on_off(WallPlugNodeID, OFF, 21);
			break;
		case 3:
			binary_switch_on_off(LampHolderNodeID, ON, 22);
			break;
		case 4:
			binary_switch_on_off(LampHolderNodeID, OFF, 20);
			break;
		case 5:
			capture_frame();
			break;
		case 6:
			record_video(3);
			break;
		case 7:
			printf("Available Commands:\n");
			int j = 0;
			for (; j < number_of_commands; j++)
			{
				printf("\t%s", user_commands[j]);
			}
			break;
		default:
			break;
		}
	} else {
		printf("Available Commands:\n");
		int j = 0;
		for (; j < number_of_commands; j++) {
			printf("\t%s", user_commands[j]);
		}
	}
}

