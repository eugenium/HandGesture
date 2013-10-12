#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <math.h>
#include <cxcore.h>
#include <string.h>
#include <stdlib.h>
#include "DetectSegment.h"


/*
	createcvwindow,
	draw box where user places hand
	wait for user to press enter, and capture frame
	
	hsv color space
	create the histogram
	and count 10% of the pixel values in the outside bins
	then check the values at the two inner points, those are the ranges for 
*/

int main(int argc,char ** argv)
{
	int key;
	int w,h;
	IplImage* img,*img1;
	CvCapture* capture = NULL;
	CvCapture* cap2=NULL;
	CvRect  pr;
	CvRect * r;
	w=150;
	h=100;
	int start=100;
	if(argc==1)
	{
		if(NULL==(capture = cvCaptureFromCAM(1))){
			printf("\nError on cvCaptureFromCAM");
			return -1;
		}
	}
	
	if(argc>1)
	{
		if(NULL==(capture = cvCaptureFromAVI(argv[1]))){
			printf("\nError on cvCaptureFromCAM");
			return -1;
		}
	}

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Captured",CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Capture", 550, 250);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
	
	fprintf(stderr,"successful exit\n");


	
	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}
		
// h-0 179 s-61 227
		cvReleaseImage(&img1);
		img1=binary_threshold_hsl(img,9);

		cvShowImage("Capture",img);
		cvShowImage("Captured",img1);

		key = cvWaitKey(50);

		if((key&0x7f)==27)
			break;
	}


	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture");

	cvReleaseImage( &img );

	exit(1);
	return 0;
}
