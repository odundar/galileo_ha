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

#define USBCAM 0

int main(void) {
	//Get USB Cam Device
	CvCapture *pCapturedImage = cvCreateCameraCapture(USBCAM);
	//Get Frame from Capture Device
	IplImage *pSaveImg = cvQueryFrame(pCapturedImage);
	//Save Image to Filesystem
	cvSaveImage("capture.jpg", pSaveImg, 0);
	//Release Image Pointer
	cvReleaseImage(&pSaveImg);
	return 0;
}
