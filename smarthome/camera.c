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
#include "camera.h"

void capture_frame() {
	//Get Capture Device assign to a memory location
#ifdef IP_CAM
	CvCapture *pCapturedImage = cvCaptureFromFile(IP_CAM);
#else
	CvCapture *pCapturedImage = cvCreateCameraCapture(USBCAM);
#endif
	//Frame to Save
	IplImage *pSaveImg = cvQueryFrame(pCapturedImage);
	//Get TimeStamp
	//Get Current Time
	time_t now;
	struct tm *tm;
	now = time(0);
	if ((tm = localtime(&now)) == NULL) {
		printf("Can't Get Time\n");
		return;
	}
	char buf[256];
	printf("Capturing....\n");
	sprintf(buf, "/home/root/smarthome/captures/capture%d_%d_%d_%d_%d_%d.jpg", tm->tm_year, tm->tm_mon,
			tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	//Save Image to Filesystem
	cvSaveImage(buf, pSaveImg, 0);
	printf("Saved....\n");
	//Release Memory Pointer
	//cvReleaseCapture(&pCapturedImage);
	//cvReleaseImage(&pSaveImg);
	return;
}

void record_video(int sec) {
	//Capture Device and Captured Frame
#ifdef IP_CAM
	CvCapture *pCapture = cvCaptureFromFile(IP_CAM);
#else
	CvCapture *pCapture = cvCreateCameraCapture(USBCAM);
#endif
	int width = (int) cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int) cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT);
	//Get Size of Video
	CvSize S;
	S.height = height;
	S.width = width;

	//Frame Structure
	IplImage *frame;
	//Create VideoWriter
	//Get Current Time
	time_t now;
	struct tm *tm;
	now = time(0);
	//Start Recording for Given Time
	printf("Recording....\n");
	if ((tm = localtime(&now)) == NULL) {
		printf("Can't Get Time\n");
		return;
	}
	char buf[256];
	sprintf(buf, "/home/root/smarthome/captures/record%d_%d_%d_%d_%d_%d.avi", tm->tm_year, tm->tm_mon,
			tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	CvVideoWriter* videoWriter = cvCreateVideoWriter(buf,
			CV_FOURCC('X', 'V', 'I', 'D'), 20, S, 1);

	int start = tm->tm_sec;
	int progress = tm->tm_sec;
	while ((progress - start) <= sec) {
		frame = cvQueryFrame(pCapture);
		cvWriteFrame(videoWriter, frame);
		now = time(0);
		if ((tm = localtime(&now)) == NULL) {
			printf("Can't Get Time\n");
			return;
		}
		progress = tm->tm_sec;
	}
	printf("Saved....\n");
	//Release Pointers from Memory
	cvReleaseCapture(&pCapture);
	//This is needed to make recorded video container to be closed properly
	cvReleaseVideoWriter(&videoWriter);
}
