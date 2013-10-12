#include <stdio.h>
#include "Recognition.h"
#include "DetectSegment.h"

#define DEBUG 1
#if (DEBUG==1)
int main(int argc, char **argv)
{
//	int indx[] = {6,11,12,14,15,22,27,28,33,36,42,}; 
							//5, a, b, c,caps,g,l,LC,p,RC,v,
	int numGestures=9;
	int numTraining=80;	
	int numTesting=19;
	int numFeatures=8;
	int i,j,k,t;
	char * filename ="../../../test3.png";
	//IplImage * testImag=cvLoadImage("../../../testSh.jpg");
	IplImage * testSegmented;
	CvArr** eigenVects= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** eigenVals=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	trainMahaFD(Means,eigenVects,eigenVals,numGestures,numTraining,numFeatures);
	testFDSeg(Means,eigenVects,eigenVals,numGestures,numTesting,numFeatures);
	//testFD(Means,eigenVects,eigenVals,numGestures,numTesting,numFeatures);
	//cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);	
	//testonImage(Means,eigenVects,eigenVals,numGestures,numFeatures,filename,indx);
	
	cvWaitKey(0);
	return 0;
}
#endif

