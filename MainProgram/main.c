#define DEBUG 1

#if (DEBUG==1)
int main(int argc, char **argv)
{
	int indx[] = {6,11,12,14,15,22,27,28,33,36,42,}; 
							//5, a, b, c,caps,g,l,LC,p,RC,v,
	int numGestures=11;
	int numTraining=80;	
	int numTesting=19;
	int numFeatures=8;
	int i,j,k,t;
	char * filename ="../../../test3.png";
	IplImage * testImag=cvLoadImage("../../../testSh.jpg");
	IplImage * testSegmented;
	CvArr** eigenVects= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** eigenVals=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	//train(Means,eigenVects,eigenVals,numGestures,numTraining,numFeatures,indx);
	//test(Means,eigenVects,eigenVals,numGestures,numTesting,numFeatures,indx);
	//cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);	
	//testonImage(Means,eigenVects,eigenVals,numGestures,numFeatures,filename,indx);
	
	testSegmented=binary_threshold_hsl(testImag);
	cvNamedWindow("Binarize",CV_WINDOW_AUTOSIZE);
	cvShowImage("Binarize",testSegmented);
	
	t=simpleHuRecognize(testSegmented,11);
	
	fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
	/*for(i=0;i<numGestures;i++) {
		fprintf(stderr,"i=%d ",i);
		for(j=0;j<numFeatures;j++)		
			fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,j ) ));
		fprintf(stderr,"\n",i);
	}
	
	
	for(i=0;i<numGestures;i++) {
		fprintf(stderr,"%d\n",i);
		for(k=0;k<numFeatures;k++) {	
			for(j=0;j<numFeatures;j++)		
				fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)invCovMatrices[i]),k,j ) ));
			fprintf(stderr,"\n",i);
		}
	}*/
	cvWaitKey(0);
	return 0;
}
#endif

#if (DEBUG==0)
int main(int argc,char ** argv){
	int key,t;
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
	numGestures=11;
	int numTraining=80;	
	int numFeatures=8;
	//CvArr** invCovMatrices= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** eigenVects= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** eigenVals=(CvArr **)malloc(sizeof(CvArr*)*numGestures);
	//end record video       
	int indx[] = {6,11,12,14,15,22,27,28,33,36,42}; 
	//5, a, b, c,caps,g,l,LC,p,RC,v,

	CvCapture* capture = NULL;
	cascade = (CvHaarClassifierCascade*)cvLoad( "../NewTrained.xml",
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
	
	train(Means,eigenVects,eigenVals,numGestures,19,numFeatures,indx);
	fprintf(stderr,"blahblah\n");
	//ReadInData(filenamesCov,FileMeans,invCovMatrices,Means,numGestures,numFeatures);
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
			
			t=simpleHuRecognize(img2,11);
		}
		if(HAND==1)
		{
			fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
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
#endif
