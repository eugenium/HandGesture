#include <stdio.h>
#include "Recognition.h"
#include "DetectSegment.h"

#define DEBUG 1

#if (DEBUG==1)
int main(int argc, char **argv)
{
//	int indx[] = {6,11,12,14,15,22,27,28,33,36,42,}; 
							//5, a, b, c,caps,g,l,LC,p,RC,v,
	int numGestures=8;
	int numTraining=1;	
	int numTesting=19;
	int numFeatures=7;
	int i,j,k,t;
	char * filename ="../../../test3.png";
	//IplImage * testImag=cvLoadImage("../../../testSh.jpg");
	IplImage * testSegmented;
	double** Means=(double **)malloc(sizeof(double *)*numGestures);
	//for(i=0;i<numGestures;i++)
	//{
	//	Means[i]=(double*)malloc(sizeof(double)*numFeatures);	
	//}
	trainDHu(Means,numGestures,numTraining,numFeatures);
	testDHuSeg(Means,numGestures,numTesting,numFeatures);
	//testFD(Means,eigenVects,eigenVals,numGestures,numTesting,numFeatures);
	//cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);	
	//testonImage(Means,eigenVects,eigenVals,numGestures,numFeatures,filename,indx);
	
	cvWaitKey(0);
	return 0;
}
#endif

