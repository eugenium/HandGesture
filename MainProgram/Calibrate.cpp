#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <math.h>
#include <cxcore.h>
#include <string.h>
#include <stdlib.h>



/*
	createcvwindow,
	draw box where user places hand
	wait for user to press enter, and capture frame
	
	hsv color space
	create the histogram
	and count 10% of the pixel values in the outside bins
	then check the values at the two inner points, those are the ranges for 
*/
IplImage* binary_threshold_hsl(IplImage* src,int h_min,int h_max,int s_min,int s_max) {
  
	IplImage* src2=cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_BGR2HSV);
	IplImage* dst = cvCreateImage( cvGetSize(src), src->depth,1);

	for( int y=0; y<src2->height; y++ ) {
  		uchar* ptr = (uchar*) (
     		src2->imageData + y * src->widthStep
		);
		uchar* ptr2 = (uchar*) (
     		dst->imageData + y * dst->widthStep
  		);

	  	for( int x=0; x<src->width; x++ ) {
				if(((ptr[3*x+0] > h_min) && (ptr[3*x+0] < h_max))
							&& ((ptr[3*x+1] > s_min)&&(ptr[3*x+1] < s_max)) )
					ptr2[x] =255;
				else
					ptr2[x]=0; 
     }
  }
	return dst;
}

void decision(int *range,int a, int b,int c,int d) {

	
	if( (c<b) && (c>a) && (b<d) ) {
		range[0] =a;
		range[1] =c;
		return;
	}
	
	if( (c>a) && (d<b)  ) {
		
		if( (c-a) > (b-d) ) {
			range[0] =a;
			range[1] =c;
			return;
		}
		
		range[0] =d;
		range[1] =b;
		return;
	}
	
	if( (c<a) && (d>a) && (d<b) ) {
		range[0] =d;
		range[1] =b;
		return;
	}	
	
	
	if( (c>b) || (a>d) ) {
		range[0] = a;
		range[1] = b;
		return;
	}

	range[0]=a;
	range[1]=b;	
	
}

void range_finder(int *indx,int min, int max, CvHistogram *hist,int num_pix, float per_lost) {
	
	int i,j,z;
	float temp;
	int sum=0;
	i=min;
	j=max;
	/*for(z=0; i<=j;z++)
    {
		if(z%2==0)
		{
			sum+=cvQueryHistValue_1D(hist,i);
			i++;
		}
		else
		{
			sum+=cvQueryHistValue_1D(hist,j);
			j--;						
		}	
		if((float)sum/(float)num_pix>=(float)per_lost)
				break;	
		//fprintf(stderr,"hskin min=%d max=%d\n",1,i,j);				
	}*/
	for(i=min; i<=j;i++)
    {

		sum+=cvQueryHistValue_1D(hist,i);
		if((float)sum/(float)num_pix>=((float)per_lost/(float)2))
				break;	
		fprintf(stderr,"min=%d max=%d\n",i,j);				
	}
	sum=0;
	for(j=max; i<=j;j--)
    {

		sum+=cvQueryHistValue_1D(hist,j);

		if((float)sum/(float)num_pix>=((float)per_lost/(float)2))
				break;	
		fprintf(stderr,"min=%d max=%d\n",i,j);				
	}

	indx[0]=i;
	indx[1]=j;
}

CvScalar colors[] = {
		{{0,0,255}}, {{0,128,255}},{{0,255,255}},{{0,255,0}},
		{{255,128,0}},{{255,255,0}},{{255,0,0}}, {{255,0,255}}
	}; 
//void calculateRange(CvHistogram *,

int main(int argc,char ** argv){

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
	if(NULL==(capture = cvCaptureFromCAM(0))){
		printf("\nError on cvCaptureFromCAM");
		return -1;
	}
	
	/*if(NULL==(cap2 = cvCaptureFromAVI("../TestingSData/infile.ogv"))){
		printf("\nError on cvCaptureFromCAM");
		return -1;
	}*/
	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Captured",CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Capture", 550, 250);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}
		pr=cvRect(start,start,start+w,start+h);
		r=&pr;
		

		img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);
		cvSetImageROI(img,*r);
		cvCopy(img,img1,NULL);
		cvResetImageROI(img);
		
		cvRectangle(
					img,
					cvPoint(r->x,r->y),
					cvPoint(r->x+r->width,r->y+r->height),
					colors[0]
		);
		cvShowImage("Capture",img);
		cvShowImage("Captured",img1);

		key = cvWaitKey(100);

		if((key&0x7f)==27)
			break;
	}
	fprintf(stderr,"successful exit\n");

	IplImage* skin=cvCreateImage( cvGetSize(img1), img1->depth,3);
	cvCvtColor(img1,skin,CV_BGR2HSV);
	
	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}
		pr=cvRect(start,start,start+w,start+h);
		r=&pr;
		

		img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);
		cvSetImageROI(img,*r);
		cvCopy(img,img1,NULL);
		cvResetImageROI(img);
		
		cvRectangle(
					img,
					cvPoint(r->x,r->y),
					cvPoint(r->x+r->width,r->y+r->height),
					colors[0]
		);
		cvShowImage("Capture",img);
		cvShowImage("Captured",img1);

		key = cvWaitKey(100);

		if((key&0x7f)==27)
			break;
	}
	fprintf(stderr,"successful exit\n");

	IplImage* background=cvCreateImage( cvGetSize(img1), img1->depth,3);
	cvCvtColor(img1,background,CV_BGR2HSV);
	IplImage* h_plane = cvCreateImage( cvGetSize(img1), 8, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(img1), 8, 1 );
	IplImage* s2_plane = cvCreateImage( cvGetSize(background), 8, 1 );
	IplImage* h2_plane = cvCreateImage( cvGetSize(background), 8, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(img1), 8, 1 );
	//IplImage* planes[] = { h_plane, s_plane };
	//IplImage* planes2[] = { h2_plane, s2_plane };
	cvCvtPixToPlane( skin, h_plane, s_plane, v_plane, 0 );
	cvCvtPixToPlane( background, h2_plane, s2_plane, v_plane, 0 );

	CvHistogram *hist_skins,*hist_skinh, *hist_backs,*hist_backh;
	int h_bins = 181, s_bins = 256;

	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 }; // hue is [0,180]
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist_skinh = cvCreateHist(
				1,
				&hist_size[0],
				CV_HIST_ARRAY,
				&ranges[0],
				1
			);
		hist_skins = cvCreateHist(
				1,
				&hist_size[1],
				CV_HIST_ARRAY,
				&ranges[1],
				1
			);
		hist_backh = cvCreateHist(
				1,
				&hist_size[0],
				CV_HIST_ARRAY,
				&ranges[0],
				1
			);
		hist_backs = cvCreateHist(
				1,
				&hist_size[1],
				CV_HIST_ARRAY,
				&ranges[1],
				1
			);
	}	
	cvCalcHist( &h_plane, hist_skinh, 0, 0 );
	cvCalcHist( &s_plane, hist_skins, 0, 0 );
	cvCalcHist( &h2_plane, hist_backh, 0, 0 );
	cvCalcHist( &s2_plane, hist_backs, 0, 0 );

	float min[4],max[4];
	int min_indx[4],max_indx[4];
	/*//cvNormalizeHist( hist_skinh, 1.0 );
	cvGetMinMaxHistValue( hist_skinh, &min[0], &max[0], &min_indx[0], &max_indx[0] );
	//cvNormalizeHist( hist_skins, 1.0 );
	cvGetMinMaxHistValue( hist_skins, &min[1], &max[1], &min_indx[1], &max_indx[1] );
	//cvNormalizeHist( hist_backh, 1.0 );
	cvGetMinMaxHistValue( hist_backh, &min[2], &max[2], &min_indx[2], &max_indx[2] );
	//cvNormalizeHist( hist_backs, 1.0 );
	cvGetMinMaxHistValue( hist_backs, &min[3], &max[3], &min_indx[3], &max_indx[3] );
	*/
	int hskin[2];
	int hback[2];
	int sskin[2];
	int sback[2];

	range_finder(hskin,0, 180,hist_skinh,w*h,0.0001);
	range_finder(sskin,0, 255,hist_skins,w*h,0.01);
	range_finder(hback,0, 180,hist_backh,w*h,0.01);
	range_finder(sback,0, 255,hist_backs,w*h,0.01);

	fprintf(stderr,"hskin min=%d max=%d\n",hskin[0],hskin[1]);
	fprintf(stderr,"sskin min=%d max=%d\n",sskin[0],sskin[1]);
	fprintf(stderr,"hback min=%d max=%d\n",hback[0],hback[1]);
	fprintf(stderr,"sback min=%d max=%d\n",sback[0],sback[1]);

	int hrange[2],srange[2];

	decision(hrange,hskin[0], hskin[1],hback[0],hback[1]);
	decision(srange,sskin[0], sskin[1],sback[0],sback[1]);
	//decide on thresholds
	
	fprintf(stderr,"hmin=%d hmax=%d\n",hrange[0],hrange[1]);
	fprintf(stderr,"smin=%d smax=%d\n",srange[0],srange[1]);

	
	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}
		
// h-0 179 s-61 227
		cvReleaseImage(&img1);
		img1=binary_threshold_hsl(img,0,179,srange[0],srange[1]);

		cvShowImage("Capture",img);
		cvShowImage("Captured",img1);

		key = cvWaitKey(50);

		if((key&0x7f)==27)
			break;
	}

		cvReleaseImage(&img1);
		char * filename = "../TestingSData/11-1.png";
		img = cvLoadImage( filename );
		img1=binary_threshold_hsl(img,0,179,61,227);
		IplImage* img2=cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels);

		IplConvKernel * myKernel= cvCreateStructuringElementEx(8,8,0,0,CV_SHAPE_RECT,NULL);
	//set the top corner to 0;
		cvDilate(img1,img2,myKernel,1); 

		cvShowImage("Capture",img);
		cvShowImage("Captured",img1);
		cvNamedWindow("dilated",CV_WINDOW_AUTOSIZE);
		cvShowImage("dilated",img2);

		key = cvWaitKey(0);

	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture");

	cvReleaseImage( &img );

	exit(1);
	return 0;
}
