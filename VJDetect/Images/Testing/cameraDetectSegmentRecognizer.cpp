#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <fftw3.h>
#include <math.h>
#include<cxcore.h>
#include <string.h>
#include <stdlib.h>


#define CVX_RED     CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN   CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE    CV_RGB(0x00,0x00,0xff)

//#define h_min 0.3*255 /* was -.5*/ /* .425 and .48 worked ok with || */
/*#define h_max 0.5*255
#define s_min 0.20*255
#define s_max 0.51*255
#define l_min 0.28*255
#define l_max 0.55*255
*/
#define b_min 77
#define b_max 127
#define r_min 133
#define r_max 173
#define g_min 
#define g_max 

#define h_min 0*255
#define h_max 0.11*255
#define s_min 0.2*255
#define s_max 0.7*255

// Eugene Belilovsky
// Camera read in
// viola jones detector
// segmentation/binarization

CvHaarClassifierCascade  * cascade;
CvMemStorage* storage;
int HAND=0;

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
			//	fprintf(stderr,"%d %d %d\n",ptr[3*x+0],ptr[3*x+1],ptr[3*x+2]);	
				if(((ptr[3*x+0] > h_min) && (ptr[3*x+0] < h_max))
							&& ((ptr[3*x+1] > s_min)&&(ptr[3*x+1] < s_max)) )
					ptr2[x] =255;
				else
					ptr2[x]=0; 
			/*YCrCb not terrible
				if(((ptr[3*x+0] > 80)) && ((ptr[3*x+2]>85) && (ptr[3*x+2] <135))
							&& ((ptr[3*x+1] > 135)&&(ptr[3*x+1] < 180)) )
					ptr2[x] =0;
				else
					ptr2[x]=255; 
			*/
     }
  }
	return dst;
}

IplImage* binary_threshold_rgb(IplImage* src) {

  	IplImage* src2=cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_BGR2RGB);
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
    	if(  ((ptr[3*x+2] >= b_min) && (ptr[3*x+2] <= b_max)) )
				//		|| ((ptr[3*x+0] >= r_min)&&(ptr[3*x+0] <= r_max)) )
				ptr2[x] = 0;
			else
				ptr2[x]=255; 
     }
  }
  return dst;
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

		cvSetImageROI(img,*r);

		img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);

		cvCopy(img,img1,NULL);
		cvRectangle(
					img,
					cvPoint(r->x,r->y),
					cvPoint(r->x+r->width,r->y+r->height),
					colors[0]
		);
		cvResetImageROI(img);
		HAND=1;

	}
	else
	{
		HAND=0;		
		img1=cvCreateImage(cvSize(100,100),img->depth,img->nChannels);
	}


	cvReleaseImage( &gray);
	cvReleaseImage( &small_img );
	return img1;
}

/*
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
*/

double * computeFDFeatures(IplImage* segmented, int N)
{
	cvNamedWindow( "Edge",1);
	cvMoveWindow("Capture", 100, 10);
	
	IplImage* img_edge = cvCreateImage( cvGetSize(segmented), 8, 1 );
	IplImage* img_8uc3 = cvCreateImage( cvGetSize(segmented), 8, 3 );
	cvThreshold( segmented, img_edge, 128, 255, CV_THRESH_BINARY );
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
	int n2;
	double Scale;
	double * Features;
	Features=(double *)malloc(sizeof(double)*N);
	
	//malloc error checking
	
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
	fprintf(stderr,"best is %d\n",best);
	n=0;
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
		if(n==best && c->total >20){
			cvCvtColor( segmented, img_8uc3, CV_GRAY2BGR );
			cvDrawContours(
				img_8uc3,
				c,
				CVX_RED,
				CVX_BLUE,
				1, // Try different values of max_level, and see what happens
				4,
				4
				);
			printf("Contour #%d\n", n );
			cvShowImage("Edge", img_8uc3 );
		//	cvWaitKey(0);
			printf("%d elements:\n", c->total );
			contour=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(c->total));
			FD=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(c->total));

			for( int i=0; i<c->total; ++i ) {
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
			//	printf("(%d,%d)\n", p->x, p->y );
				//assemble complex representation here
				contour[i][0]=p->x;
				contour[i][1]=p->y;
			}

			plan_forward=fftw_plan_dft_1d(c->total,contour,FD,FFTW_FORWARD,FFTW_ESTIMATE);
			fftw_execute(plan_forward);
			//do fft
			n2=c->total/2;
			Scale=(double)sqrt(pow(FD[1][0],2)+pow(FD[1][1],2));
			//reduce to 10 coefficients
			//normalize
			if(N+2>=c->total)
			{
				fprintf(stderr,"Contour Is too small");
				return 0;
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
			//cvWaitKey(0);
		}
		n++;
	}
	//try downspampling later
	
	printf("Finished all contours.\n");
	//destroy fftw_plan
	cvCvtColor( segmented, img_8uc3, CV_GRAY2BGR );
	cvShowImage( "Edge", img_8uc3 );
	//cvWaitKey(0);
	//cvDestroyWindow( "Edge" );
	cvReleaseImage( &segmented );
	cvReleaseImage( &img_8uc3 );
	cvReleaseImage( &img_edge );
	return Features;
	
}



void ReadInCovMatrix(char* filename,int numFeatures,CvMat * mat)
{
	fprintf(stderr,"1\n");
	FILE *file=NULL;
	int i,j;
	double element;
	double *dp = &element;
	if((file=fopen(filename,"r"))==NULL)fprintf(stderr,"Error: can't open file\n");
	fprintf(stderr,"2\n");
	
	//float element_3_2 = 7.7;
	
	for(j=0;j<numFeatures;j++)
	{
		for(i=0;i<numFeatures-1;i++)
		{	
			fscanf(file,"%lf ",dp);
			*( (double*)CV_MAT_ELEM_PTR( *mat, j,i ) ) = element;
		}

		fscanf(file,"%lf\n",dp);
		*( (double*)CV_MAT_ELEM_PTR( *mat, j,i ) ) = element;
	}

	for(j=0;j<numFeatures;j++)
	{
		for(i=0;i<numFeatures;i++)
		{	
			fprintf(stderr,"%lf ",CV_MAT_ELEM( *mat, double, j, i ));
							
		}
		fprintf(stderr, "\n");
	}
	fclose(file);
}

void ReadInMeans(char* filename,int numFeatures,CvMat ** Means,int numGestures)
{
	fprintf(stderr,"1\n");
	FILE *file=NULL;
	int i,j;
	double element;
	double *dp = &element;
	if((file=fopen(filename,"r"))==NULL)fprintf(stderr,"Error: can't open file\n");
	fprintf(stderr,"2\n");
	
	//float element_3_2 = 7.7;
	
	for(j=0;j<numGestures;j++)
	{
		Means[j]=cvCreateMat(1,numFeatures,CV_64FC1);
		for(i=0;i<numFeatures-1;i++)
		{	
			fscanf(file,"%lf ",dp);
			//fprintf(stderr,"j is :%d, and i is %d\n",j,i);
			*( (double*)CV_MAT_ELEM_PTR( *(Means[j]), 0,i ) ) = element;
			
		}
		fscanf(file,"%lf",dp);
		*( (double*)CV_MAT_ELEM_PTR( *(Means[j]), 0,i ) ) = element;
	}

	for(j=0;j<numGestures;j++)
	{
		for(i=0;i<numFeatures-1;i++)
		{	
			fprintf(stderr,"%lf ",CV_MAT_ELEM( *Means[j], double, 0, i ));
		}
		fprintf(stderr,"%lf ",CV_MAT_ELEM( *Means[j], double, 0, i ));
	}

}


void ReadInData(char *filenamesCov,char * FileMeans,CvArr** invCovMatrices,CvArr** Means,int numGestures,int numFeatures)
{
	//error
	char *cov = (char *)malloc(sizeof(char)*strlen(filenamesCov)+7);
	strcpy(cov,filenamesCov);
	CvMat * mat;
//	strcat("Variance
	
	int i,j;
	char dig[3];
	for(i=0;i<numGestures;i++)
	{
		fprintf(stderr,"%d ",i);
		
		if( (i+1)>9) {
			dig[0]='0'+((i+1)/10)%100;
			dig[1] ='0'+(i+1)%10;
			dig[2]='\0';
		}
		else {
			dig[0] ='0'+(i+1)%10;
			dig[1]='\0';
		}
		strcat(cov,dig);
		strcat(cov,".txt");
		invCovMatrices[i] = cvCreateMat( numFeatures, numFeatures, CV_64FC1 );
		ReadInCovMatrix(cov,numFeatures,invCovMatrices[i]);
		fprintf(stderr,"%s \n",cov);
		cov[strlen(filenamesCov)]='\0';
	}
	ReadInMeans(FileMeans,numFeatures,Means,numGestures);
}


int Recognize(double * Features,CvArr** invCovMatrices,CvArr** Means,int numGestures,int numFeatures)
{
	int i;
	double d=10000;
	double d2;
	int choice=0;
	CvMat* input=cvCreateMat(1,numFeatures,CV_64FC1);
	for(i=0;i<numFeatures;i++)
	{
		*( (double*)CV_MAT_ELEM_PTR( *(input), 0,i ) ) = Features[i];
	}
	for(i=0;i<numGestures;i++)
	{
		if ((d2=cvMahalonobis(input,Means[i],invCovMatrices[i]))<d)
		{
			fprintf(stderr,"for %d the distance is %lf\n",i,d); 
			d=d2;
			choice=i;
		}
	}
	return choice;
}
/*
int main(int argc,char ** argv)
{
	CvMat *mat=cvCreateMat(8,8,CV_64FC1);
	char *filenamesCov = "../../../fourier/fourier/Variances";
	
	char * FileMeans = "../../../fourier/fourier/Means.txt";
	int numGestures;
	numGestures=46;	
	int numFeatures=8;
	CvArr** invCovMatrices= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	if( (Means==NULL) || (invCovMatrices==NULL) ) {
		perror("malloc");
		exit(-1);
	}

	
	fprintf(stderr,"GEORGE SUXORS\n");
	//ReadInCovMatrix("../../../fourier/fourier/Variances1.txt",8,mat);
	ReadInData(filenamesCov,FileMeans, invCovMatrices, Means,numGestures, numFeatures);
//char * filename = "txt";
	//ReadInCovMatrix(filename,8,mat);

	return 0;	
}

*/
int main(int argc,char ** argv){
	int key;
	double *Features;
	//code to record video
	CvVideoWriter *writer=0;
	int isColor=1;
	int fps=10;
	int frameW=640;
	int frameH=480;
	writer=cvCreateVideoWriter("out.avi",CV_FOURCC('D','I','V','X'),fps,cvSize(frameW,frameH),isColor);
	CvMat *mat=cvCreateMat(8,8,CV_64FC1);
	char *filenamesCov = "../../../fourier/fourier/Variances";
	char * FileMeans = "../../../fourier/fourier/Means.txt";
	int numGestures;
	numGestures=46;	
	int numFeatures=8;
	CvArr** invCovMatrices= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	//end record video       

	CvCapture* capture = NULL;
	cascade = (CvHaarClassifierCascade*)cvLoad( "../trainedNew20.xml",
	0, 0, 0 );	
	storage = cvCreateMemStorage(0); 
	IplImage* img;
	IplImage* img1;
	IplImage* img2;
	IplImage* imb;
	if(NULL==(capture = cvCaptureFromCAM(1))){
		printf("\nError on cvCaptureFromCAM");
		return -1;
	}
	ReadInData(filenamesCov,FileMeans,invCovMatrices,Means,numGestures,numFeatures);
	//-----
	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Capture2", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Capture3", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Window",CV_WINDOW_AUTOSIZE);	

	cvMoveWindow("Capture", 550, 250);
	cvMoveWindow("Capture2",850, 50);
	cvMoveWindow("Capture3",100,500);
	cvMoveWindow("Capture4",500,600);

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
		if(HAND==1)
		{
			Features=computeFDFeatures(img2,8);
			fprintf(stderr,"Found a hand. The gesture recognized is : %d\n",1+Recognize(Features,invCovMatrices,Means,numGestures,numFeatures));
			//imb=cvCreateImage("",8,1);
		}
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

