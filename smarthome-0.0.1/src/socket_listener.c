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
 * This module is used to receive and send messages from network socket
 */
#include "socket_listener.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

void* socket_worker(void* arg) {
	char buffer[MAXBUF + 1];
	char* msg = "ACK\n";
	struct sockaddr_in dest; /* socket info about the machine connecting to us */
	struct sockaddr_in serv; /* socket info about our server */
	int homesocket; /* socket used to listen for incoming connections */
	socklen_t socksize = sizeof(struct sockaddr_in);

	memset(&serv, 0, sizeof(serv)); /* zero the struct before filling the fields */
	serv.sin_family = AF_INET; /* set the type of connection to TCP/IP */
	serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
	serv.sin_port = htons(SOCKET); /* set the server port number */

	homesocket = socket(AF_INET, SOCK_STREAM, 0);

	/* bind serv information to homesocket */
	bind(homesocket, (struct sockaddr *) &serv, sizeof(struct sockaddr));

	/* start listening, allowing a queue of up to 1 pending connection */
	listen(homesocket, 1);
	int consocket = accept(homesocket, (struct sockaddr *) &dest, &socksize);
	while (consocket) {
		send(consocket, msg, strlen(msg), 0);
		int received = -1;
		/* Receive message */
		if ((received = recv(consocket, buffer, MAXBUF, 0)) >= 0) {
			//printf("Received %s from %s \n", buffer, inet_ntoa(dest.sin_addr));
			snprintf(arg, sizeof(buffer), buffer);
			memset(buffer,0,sizeof(buffer));
		}
		close(consocket);
		consocket = accept(homesocket, (struct sockaddr *) &dest, &socksize);
	}
	return NULL;
}
