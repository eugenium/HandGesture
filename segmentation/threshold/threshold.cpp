#include <stdio.h>
#include <cv.h>
#include <highgui.h>

 

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

/*
	Load the image
	convert it from rgb to hsl
	split it
	threshold each component
	and the three components
	
*/
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

void binary_threshold_hsl(IplImage* src, IplImage* dst) {
  
  /*IplImage* h = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* l = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* s = cvCreateImage(  cvGetSize(src), IPL_DEPTH_8U, 1 );
	cvSplit( src, h, l, s, NULL );*/

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
  

int main(int argc, char** argv)
{
  // Create a named window with the name of the file.
  cvNamedWindow( argv[1], 1 );
  // Load the image from the given file name.
  IplImage* src = cvLoadImage( argv[1] );
	IplImage* src2 = cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_RGB2HLS);
  IplImage* dst = cvCreateImage( cvGetSize(src), src->depth,1);
	cvShowImage( argv[1], src );
	while( 1 ) { if( (cvWaitKey( 10 )&0x7f) == 27 ) break; }
  
	binary_threshold_hsl(src2,dst);
  // Show the image in the named window
  cvShowImage( argv[1], dst );
  // Idle until the user hits the “Esc” key.
  while( 1 ) { if( (cvWaitKey( 10 )&0x7f) == 27 ) break; }
	
	binary_threshold_rgb(src,dst);
  // Show the image in the named window
  cvShowImage( argv[1], dst );
  // Idle until the user hits the “Esc” key.
  while( 1 ) { if( (cvWaitKey( 10 )&0x7f) == 27 ) break; }
  // Clean up and don’t be piggies
  cvDestroyWindow( argv[1] );
  cvReleaseImage( &src );
  cvReleaseImage( &dst );
}

