#include <stdio.h>
#include "highgui.h"
#include "cv.h"

#define h_min 0.3*255 /* was -.5*/ /* .425 and .48 worked ok with || */
#define h_max 0.5*255
#define s_min 0.20*255
#define s_max 0.51*255
#define l_min 0.28*255
#define l_max 0.55*255

#define b_min 77
#define b_max 127
#define r_min 133
#define r_max 173

// Eugene Belilovsky
// Camera read in
// viola jones detector
// segmentation/binarization

CvHaarClassifierCascade  * cascade;
CvMemStorage* storage;


void sum_rgb( IplImage* src, IplImage* dst ) {
  // Allocate individual image  planes.
  IplImage* r = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* g = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* b = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  // Split image onto the color planes.
  cvSplit( src, r, g, b, NULL );
  // Temporary storage.
  IplImage* s = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
  // Add equally weighted rgb values.
  cvAddWeighted( r, 1./3., g, 1./3., 0.0, s );
  cvAddWeighted( s, 2./3., b, 1./3., 0.0, s );
  // Truncate values above 100.
  cvThreshold( s, dst, 100, 100, CV_THRESH_TRUNC );
  cvReleaseImage(  &r );
  cvReleaseImage(  &g );
  cvReleaseImage(  &b );
  cvReleaseImage(  &s );
}

IplImage* binary_threshold_hsl(IplImage* src) {
  
  /*IplImage* h = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* l = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* s = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
	cvSplit( src, h, l, s, NULL );*/
	IplImage* src2=cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_RGB2HLS);
	IplImage* dst = cvCreateImage( cvGetSize(src), src->depth,1);

	for( int y=0; y<src->height; y++ ) {
  	uchar* ptr = (uchar*) (
     	src->imageData + y * src->widthStep
		);
		uchar* ptr2 = (uchar*) (
     	dst->imageData + y * dst->widthStep
  	);
  	for( int x=0; x<src->width; x++ ) {
			/*fprintf(stderr,"%d %d %d\n",ptr[3*x+0],ptr[3*x+1],ptr[3*x+2]);*/
    	if(  ((ptr[3*x+0] > h_min) && (ptr[3*x+0] < h_max)) 
						&& ((ptr[3*x+2] > s_min)&&(ptr[3*x+2] < s_max)) )
				ptr2[x] = 0;
			else
				ptr2[x]=255; 
     }
  }
	return dst;
}

void binary_threshold_rgb(IplImage* src, IplImage* dst) {
  

	for( int y=0; y<src->height; y++ ) {
  	uchar* ptr = (uchar*) (
     	src->imageData + y * src->widthStep
		);
		uchar* ptr2 = (uchar*) (
     	dst->imageData + y * dst->widthStep
  	);
  	for( int x=0; x<src->width; x++ ) {
			/*fprintf(stderr,"%d %d %d\n",ptr[3*x+0],ptr[3*x+1],ptr[3*x+2]);*/
    	if(  ((ptr[3*x+2] >= b_min) && (ptr[3*x+2] <= b_max)) 
						|| ((ptr[3*x+0] >= r_min)&&(ptr[3*x+0] <= r_max)) )
				ptr2[x] = 0;
			else
				ptr2[x]=255; 
     }
  }
}
  

IplImage* detect_and_draw(IplImage* img, double scale = 1.3)
{
	IplImage* img1;
	char * str;
	static CvScalar colors[] = {
		{{0,0,255}}, {{0,128,255}},{{0,255,255}},{{0,255,0}},
		{{255,128,0}},{{255,255,0}},{{255,0,0}}, {{255,0,255}}
	}; //Just some pretty colors to draw with
	// IMAGE PREPARATION:
	//
	IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	IplImage* small_img = cvCreateImage(
	cvSize( cvRound(img->width/scale), cvRound(img->height/scale)), 8, 1);
	cvCvtColor( img, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );
	// DETECT OBJECTS IF ANY
	//
	cvClearMemStorage( storage );
	fprintf(stderr,"size: %d %d\n",cvGetSize(small_img).width,cvGetSize(small_img).height);
	CvSeq* objects = cvHaarDetectObjects(
														small_img,
														cascade,
														storage,
														1.1,
														2,
														0 ,
														cvSize(35, 35)
														);
	// LOOP THROUGH FOUND OBJECTS AND DRAW BOXES AROUND THEM
	//
	// for(int i = 0; i<(objects ? objects->total : 0); i++ )
	fprintf(stderr,"size: %d %d\n",cvGetSize(small_img).width,cvGetSize(small_img).height);
	if( 0<(objects ? objects->total : 0))
	{

		CvRect* r = (CvRect*)cvGetSeqElem( objects, 0 );
		cvRectangle(
			img,
			cvPoint(r->x,r->y),
			cvPoint(r->x+r->width,r->y+r->height),
			colors[0]
			);
		 cvSetImageROI(img,*r);

		img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);

		cvCopy(img,img1,NULL);

		cvResetImageROI(img);

	}
	else
	{
		img1=cvCreateImage(cvSize(100,100),img->depth,img->nChannels);
	}


	cvReleaseImage( &gray);
	cvReleaseImage( &small_img );
	return img1;
}

void obtainU(IplImage* img2){
	IplImage * dst;
	IplImage * dstE;
	int i;

	IplConvKernel * myKernel= cvCreateStructuringElementEx(12,12,0,0,CV_SHAPE_RECT,NULL);
	//set the top corner to 0;
	cvDilate(img2,dst,myKernel,1); //tune parameters
	cvErode(img2,dstE,NULL,1);
	cvSub(dst,dstE,dst,NULL);
	
	
	
	cvReleaseImage(&dst);
	cvReleaseImage(&dstE);
	
}


int main(int argc,char ** argv){
	int key;
	//code to record video
	CvVideoWriter *writer=0;
	int isColor=1;
	int fps=10;
	int frameW=640;
	int frameH=480;
	writer=cvCreateVideoWriter("out.avi",CV_FOURCC('D','I','V','X'),fps,cvSize(frameW,frameH),isColor);
	//end record video       

	CvCapture* capture = NULL;
	cascade = (CvHaarClassifierCascade*)cvLoad( "../NewTrained.xml",
	0, 0, 0 );	
	storage = cvCreateMemStorage(0); 
	IplImage* img;
	IplImage* img1;
	IplImage* img2;

	if(NULL==(capture = cvCaptureFromCAM(1))){
		printf("\nError on cvCaptureFromCAM");
		return -1;
	}

	//-----
	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Capture2", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Capture3", CV_WINDOW_AUTOSIZE);

	cvMoveWindow("Capture", 550, 250);
	cvMoveWindow("Capture2",850, 50);
	cvMoveWindow("Capture3",100,500);

	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}

	
		img1=detect_and_draw(img,1);
	    img2=binary_threshold_hsl(img1);

		cvShowImage("Capture3",img2);
		cvShowImage("Capture", img1);
		cvShowImage("Capture2",img);
		
	

		cvWriteFrame(writer,img1);
		key = cvWaitKey(10);

		if(key==0x1b)
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture");
	cvDestroyWindow("Capture2");
	cvDestroyWindow("Capture3");

	cvReleaseImage( &img );
	cvReleaseImage(&img1);

	cvWaitKey(0);
}
