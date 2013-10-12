#include <cv.h>
#include <highgui.h>
#include<cxcore.h>
#include <stdio.h>
#include <fftw3.h>
#include <math.h>

#define CVX_RED          CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN        CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE        CV_RGB(0x00,0x00,0xff)

int main(int argc, char* argv[]) {
	cvNamedWindow( "capture", 1 );
	IplImage* img_8uc1 = cvLoadImage( "/home/eugene/hand_gest/TestingData/10-2.png", CV_LOAD_IMAGE_GRAYSCALE );
	IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
	IplImage* img_8uc3 = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
	cvThreshold( img_8uc1, img_edge, 128, 255, CV_THRESH_BINARY );
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;
	int Nc = cvFindContours(
				img_edge,
				storage,
				&first_contour,
				sizeof(CvContour),
				CV_RETR_EXTERNAL // Try all four values and see what happens
				);
	int i;
	int n=0;
	int best=0;
	int current=0;
	int N=8;
	int n2;
	double Scale;
	double Features[N];
	fftw_complex *contour;
	fftw_complex *FD;
	fftw_plan plan_forward;
	printf( "Total Contours Detected: %d\n", Nc );
	//Find max contour
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
		if(c->total>current);
			best=n;
		n++;
	}
	fprintf(stderr,"best is %d",best);
	n=0;
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
		if(n==best){
			cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
			cvDrawContours(
				img_8uc3,
				c,
				CVX_RED,
				CVX_BLUE,
				0, // Try different values of max_level, and see what happens
				2,
				8
				);
			printf("Contour #%d\n", n );
			cvShowImage("capture", img_8uc3 );
			printf("%d elements:\n", c->total );
			contour= fftw_malloc(sizeof(fftw_complex)*c->total);

			for( int i=0; i<c->total; ++i ) {
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
			//	printf("(%d,%d)\n", p->x, p->y );
				//assemble complex representation here
				contour[i][0]=p->x;
				contour[i][1]=p->y;
			}
			//do fft
			//cvCvtSeqToArray(c
			cvWaitKey(0);
		}
		n++;
	}
	//try downspampling later
	FD=fftw_malloc(sizeof(fftw_complex)*c->total);
	plan_forward=fftw_plan_dft_1d(c->total,contour,FD,FFTW_FORWARD,FFTW_ESTIMATE);
	fftw_execute(plan_forward);

	n2=c->total/2;
	Scale=(double)sqrt(pow(FD[1][0],2)+pow(FD[1][1],2));
	//reduce to 10 coefficients
	//normalize
	if(N+2>=c->total)
	{
		fprintf(stderr,"Contour Is too small");
		exit(1);
	}
	//positive frequency components
	for(i=0;i<N/2;i++)
	{
		//fftshift stuff
		Features[i]=(double)sqrt(pow(FD[i+2][0],2)+pow(FD[i+2][1],2))/Scale;
	}
	for(i=0;i<N/2;i++)
	{
		Features[i+N/2]=(double)sqrt(pow(FD[N-1-i][0],2)+pow(FD[N-1-i][1],2))/Scale;
	}

	printf("Finished all contours.\n");
	cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
	cvShowImage( "capture", img_8uc3 );
	cvWaitKey(0);
	cvDestroyWindow( "capture" );
	cvReleaseImage( &img_8uc1 );
	cvReleaseImage( &img_8uc3 );
	cvReleaseImage( &img_edge );
	return 0;
}
