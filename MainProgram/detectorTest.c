	
#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include "DetectSegment.h"
#include "Recognition.h"
#define END 16

extern CvHaarClassifierCascade  *cascade;
extern CvMemStorage *storage;

int main (int argc, char **argv) {

	char num[] ="01-01";
	int numGestures=2;
	int numTraining=30;
	char *fo = ".jpg";
	char filename[] = "../DetectTest1/";//"../../../TrainingData/01-01.png";
	int i,j,k,key;
	double avg;
	IplImage* src=NULL;
	IplImage* img1=NULL;
	//src =cvCreateImage( cvSize(640.480), 8, 3 );
	int hands[numGestures][numTraining];
	int indx[] = {1,2};
	cvNamedWindow("DETECTION", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("NonBin", CV_WINDOW_AUTOSIZE);
	cascade=NULL;
		storage = cvCreateMemStorage(0); 
	cascade = (CvHaarClassifierCascade*)cvLoad( "NewNewtrained.xml",
	0, 0, 0 );
		fprintf(stderr,"cascade=%x\n",cascade);	
	cvMoveWindow("DETECTION", 550, 250);
	cvMoveWindow("NonBin",100,500);
	cvWaitKey(1000);

	for(i=0;i<numGestures; i++ ) {
		for(j=0;j<numTraining; j++) {

			filename[END-1] = '\0';

			num[0] = '0'+indx[i]/10 ;
			num[1] = '0'+indx[i]%10;
				
			if((j+1)>9) {
				num[3] = '0'+(j+1)/10;
				num[4] = '0'+(j+1)%10;
				num[5] = '\0';				
			}
			else {
				num[3] = '0'+j+1;
				num[4] = '\0';	
			}
			strcat(filename,num);
			strcat(filename,fo);
			fprintf(stderr,"i=%d j=%d %s \n",i,j,filename);		
			src = cvLoadImage( filename,CV_LOAD_IMAGE_COLOR);
			
			cvShowImage("DETECTION",src);
			fprintf(stderr,"bla %x\n",src);		
			hands[i][j]=detect_and_draw(&img1,src,1);
			fprintf(stderr,"bla2\n");	
			cvShowImage("NonBin",img1);
			cvWaitKey(10000);
			cvReleaseImage(&src);
				
		}
	}
	
	for(i=0;i<numGestures; i++ ) {
		for(j=0;j<numTraining; j++) {
			fprintf(stderr,"i=%d j=%d hand=%d \n",i,j,hands[i][j]);
		}
	}
	cvWaitKey(10000);
			
	fprintf(stderr,"bla3\n");		
	return 0;
}
