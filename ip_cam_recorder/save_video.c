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

#include <stdlib.h>
#include <stdio.h>
#include "opencv/cxcore.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <time.h>

#define IP_CAM "http://admin:123@192.168.2.141:80/mjpeg.cgi?user=admin&password=123&channel=0&.mjpg"
#define FPS 15
#define DURATION 5 //seconds

int main(void) {
	//Capture Device and Captured Frame
	CvCapture *pCapture = cvCaptureFromFile(IP_CAM);
	int width = (int) cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int) cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT);
	//Get Size of Video
	CvSize S;
	S.height = height;
	S.width = width;

	//Frame Structure
	IplImage *frame;
	//Create VideoWriter
	CvVideoWriter* videoWriter = cvCreateVideoWriter("record.avi",
			CV_FOURCC('X', 'V', 'I', 'D'), FPS, S, 1);

	//Get Current Time
	time_t now;
	struct tm *tm;
	now = time(0);
	//Start Recording for Given Time
	printf("Recording....\n");
	if ((tm = localtime(&now)) == NULL) {
		printf("Can't Get Time\n");
		return -1;
	}
	int start = tm->tm_sec;
	int progress = tm->tm_sec;
	while ((progress - start) <= DURATION) {
		frame = cvQueryFrame(pCapture);
		cvWriteFrame(videoWriter, frame);
		now = time(0);
		if ((tm = localtime(&now)) == NULL) {
			printf("Can't Get Time\n");
			return -1;
		}
		progress = tm->tm_sec;
	}
	printf("Saved....\n");
	//Release Pointers from Memory
	cvReleaseCapture(&pCapture);
	//This is needed to make recorded video container to be closed properly
	cvReleaseVideoWriter(&videoWriter);
	return 0;
}
