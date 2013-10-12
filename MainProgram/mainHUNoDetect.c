#include <stdio.h>
#include "Recognition.h"
#include "DetectSegment.h"

#define DEBUG 0
#if (DEBUG==0)

extern CvHaarClassifierCascade  *cascade;
extern CvMemStorage *storage;

int main(int argc,char ** argv){
	int key,t;
	int HAND;
	double *Features;
	//code to record video
	CvVideoWriter *writer=0;
	int isColor=1;
	int fps=10;
	int frameW=640;
	int frameH=480;
	writer=cvCreateVideoWriter("DemoDetector.avi",CV_FOURCC('D','I','V','X'),fps,cvSize(frameW,frameH),isColor);
	CvMat *mat=cvCreateMat(8,8,CV_64FC1);
	char *filenamesCov = "../../../fourier/fourier/Variances";
	char * FileMeans = "../../../fourier/fourier/Means.txt";
	int numGestures;
	numGestures=8;
	int numTraining=1;	
	int numFeatures=7;   
	double** Means=(double **)malloc(sizeof(double *)*numGestures);

	CvCapture* capture = NULL;
	cascade = (CvHaarClassifierCascade*)cvLoad( "NewNewtrained.xml",
	0, 0, 0 );	
	storage = cvCreateMemStorage(0); 
	
	fprintf(stderr,"f2 %x\n",cascade);
	fprintf(stderr,"f2 %x\n",storage);
	cvWaitKey(1000);
	IplImage* img;
	IplImage* img1=NULL;
	IplImage* img2;
	IplImage* imb;
	IplImage* img_8uc3;
	//CvSeq * cont;
	if(NULL==(capture = cvCaptureFromCAM(0))){
		printf("\nError on cvCaptureFromCAM");
		return -1;
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, frameW);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, frameH);
	CvSeq * cont;
	CvSeq * first_contour;
	trainDHu(Means,numGestures,numTraining,numFeatures);
	//fprintf(stderr,"blahblah\n");
	//ReadInData(filenamesCov,FileMeans,invCovMatrices,Means,numGestures,numFeatures);
	//-----
	cvNamedWindow("DETECTION", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Bin", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("NonBin", CV_WINDOW_AUTOSIZE);

	cvMoveWindow("DETECTION", 550, 250);

	cvMoveWindow("NonBin",100,500);
	cvWaitKey(10);
	HAND=1;
	for(;;){
		if(NULL==(img=cvQueryFrame(capture))){
			printf("\nError on cvQueryFrame");
		break;
		}
		cvShowImage("DETECTION",img);
	//	cvWaitKey(10);
		//fprintf(stderr,"blahblahblah\n");
		//HAND=detect_and_draw(&img1,img,1);
	//	fprintf(stderr,"blahblahblah1.05\n");
	//	fprintf(stderr,"a---img1 = %x\n",img1);
	//	fprintf(stderr,"img1 = %x\n",img1);
	    img2=binary_threshold_hsl(img,4); // for smaller images
		cvShowImage("Bin",img2);
		//fprintf(stderr,"blahblah1.1\n");
		if(HAND==1)
		{
			//fd features
			img_8uc3 = cvCreateImage( cvGetSize(img2), 8, 3 );
			
			cont = computeContour(img2, img_8uc3);
			if(cont==NULL)
			{
				//cvReleaseMemStorage(&(cont->storage));
				//cvReleaseMemStorage(&(storage1));
				continue;
			}
			Features=computeHuFeatures(cont);	
			t=RecognizeHu(Features,Means,numGestures,7,I1);
			fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
		//imb=cvCreateImage("",8,1);
			cvReleaseImage(&img_8uc3);
			//free(cont);
			cvReleaseMemStorage(&(cont->storage));
			//cvReleaseMemStorage(&(cont->storage));
			//cvReleaseMemStorage(&storage1);
			free(Features);
		}

		key = cvWaitKey(3);
	//	fprintf(stderr,"blahblah4\n");
		if(key==0x1b)
			break;
		cvReleaseImage( &img2 );
		//cvReleaseImage(&img);
		

	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("DETECTION");
	cvDestroyWindow("Bin");
	cvDestroyWindow("NonBin");

	cvReleaseImage( &img );
	cvReleaseImage(&img1);

	cvWaitKey(0);
}
#endif
