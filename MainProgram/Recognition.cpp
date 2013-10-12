#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <fftw3.h>
#include <math.h>
#include <cxcore.h>
#include <string.h>
#include <stdlib.h>

// Eugene Belilovsky
// George Todorov
// Recognition library

static int compare(fftw_complex *f1,fftw_complex *f2)
{
	fftw_complex f1_temp,f2_temp;

	f1_temp[0]=((fftw_complex *)f1)[0][0]-mean[0];
	f1_temp[1]=((fftw_complex *)f1)[0][1]-mean[1];
	f2_temp[0]=((fftw_complex *)f2)[0][0]-mean[0];
	f2_temp[1]=((fftw_complex *)f2)[0][1]-mean[1];

	if (atan2(f1_temp[1],f1_temp[0])>atan2(f2_temp[1],f2_temp[0]))
		return 1;
 	else if(atan2(f1_temp[1],f1_temp[0])==atan2(f2_temp[1],f2_temp[0])) //optimize possibly
		return 0;
	else 
		return -1;
}

void sortContour(fftw_complex *contour,int N)
{
	int i;
	
	mean[0]=0;
	mean[1]=0;
	for(i=0;i<N;i++)
	{
		mean[0]+=contour[i][0];
		mean[1]+=contour[i][1];
	}
	mean[0] = mean[0]/N;
	mean[1] = mean[1]/N;

	//sort 
	qsort(contour,N,sizeof(fftw_complex),compare);

	
}

double * computeFDFeatures(CvSeq * cont, int N,int wait=0)
{
	
	int i;
	int n=0;
	int best=0;
	int current=0;
	int n2;
	double Scale;
	double * Features;
	Features=(double *)malloc(sizeof(double)*N);
	fftw_complex *contour;
	fftw_complex *FD;
	fftw_plan plan_forward;

	if(Features==NULL)
	{
		fprintf(stderr,"Malloc failed");
		exit(1);
	}

	contour=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(cont->total));
	FD=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(cont->total));

	for( int i=0; i<cont->total; ++i )
	{
		CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, cont, i );	
		//if(wait)
		//	printf("%d+%d*i;\n", p->x, p->y );
		//assemble complex representation here
		contour[i][0]=p->x;
		contour[i][1]=p->y;
	}
	sortContour(contour,c->total);
	//Find max contour
	for( int i=0; i<c->total; ++i ) 
	{
		if(wait)
			printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
	}

	cvReleaseImage( &segmented );
	cvReleaseImage( &img_8uc3 );
	cvReleaseImage( &img_edge );
	return Features;
	
}

//This function takes a segmented image and computes the biggest outer contour
//returns CvSeq
CvSeq * computeContour(IplImage* segmented,int wait=0)
{
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
	
	//printf( "Total Contours Detected: %d\n", Nc );
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
		if(c->total>current);
			best=n;
		n++;
	}
	//fprintf(stderr,"best is %d\n",best);
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

			cvShowImage("Edge", img_8uc3 );
			return c;
		}
		n++;
	}
	HAND=0;

	cvReleaseImage( &segmented );
	cvReleaseImage( &img_8uc3 );
	cvReleaseImage( &img_edge );
	return NULL;
	
}

int Recognize(double * Features,CvArr** eigenVects,CvArr** eigenVals,CvArr** Means,int numGestures,int numFeatures,int use)
{
	int i,j,z;
	double d=10000;
	double d2,tmp;
	int choice=0;
	//double mahal=0;
	CvArr* input=cvCreateMat(1,numFeatures,CV_64FC1);
	CvArr* tempSub=cvCreateMat(1,numFeatures,CV_64FC1);
	CvArr* tempEV=cvCreateMat(1,numFeatures,CV_64FC1);

	for(i=0;i<numFeatures;i++)
	{
		*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)input), 0,i ) ) = Features[i];
	}
	for(i=0;i<numGestures;i++)
	{
		for(j=0;j<use;j++)
		{
			cvSub(input,Means[i],tempSub,NULL);
			//cvTranspose(tempSub,tempTran);
			for(z=0;z<numFeatures;z++)
			{
				//tempEV=eigenVects[i]
				*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tempEV), 0,z ) )=CV_MAT_ELEM( *((CvMat *)eigenVects[i]), double, j, z );
			}
			tmp=(double)(cvDotProduct(tempSub,tempEV));//ut(x-u)/lambda
			d2+=(tmp*tmp)/(double)(CV_MAT_ELEM( *((CvMat *)eigenVals[i]), double, 0, j ));
		}
		d2=sqrt(d2);
		fprintf(stderr,"for %c the distance is %lf\n",Gest[i],d2); 

		if (d2<d)
		{
			//fprintf(stderr,"for %d the distance is %lf\n",i,d2); 
			d=d2;
			choice=i;
		}
		d2=0;
	}
	return choice;
}


void train(CvArr **Means,CvArr **eigenVects,CvArr ** eigenVals,int numGestures,int numTraining,int numFeatures, int indx[]) {
	
	int two=2;
	char num[] ="01-01";
	double *Features;
	char *fo = ".png";
	char filename[] = "../../../TrainingData/01-01.png";
	int i,j,k;
	double avg;
	CvScalar t;
	CvArr *tmpCov = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
   //	CvArr *tmpEigVec = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
	//1CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numFeatures); 
	CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numTraining);
	for(k=0; k<numTraining; k++)	//1numFeatures
		tmp[k] = cvCreateMat(1,numFeatures,CV_64FC1); //1tmp[k] = cvCreateMat(1,numTraining,CV_64FC1);

	IplImage* src;
	
	for(i=0; i< numGestures; i++) {
		Means[i] = cvCreateMat(1,numFeatures,CV_64FC1);

		for(j=0;j<numTraining; j++) {

			filename[22] = '\0';

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
			//fprintf(stderr,"i=%d j=%d %s \n",i,j,filename);		
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			Features=computeFDFeatures(src,numFeatures);
			//fprintf(stderr,"got contour\n");		
			for(k=0; k < numFeatures; k++)
				*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[j]),0,k ) ) = Features[k]; //1*((CvMat *)tmp[k]),0,j
			//fprintf(stderr,"copied values\n");
			free(Features);
			//cvReleaseImage( &src );				
		}

		/*for(k=0;k<numFeatures;k++) {
			avg=0;
			for(j=0;j<numGestures;j++)
				avg = avg+CV_MAT_ELEM( *((CvMat*)tmp[k]), double, 0, j );
			avg=avg/(double)numGestures;
			//fprintf(stderr,"%lf\n",t.val[0]);
			*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,k ) ) = avg;
		}*/
		
		// print the feature vectors
			/*for(k=0;k<numTraining;k++) {	
				for(j=0;j<numFeatures;j++)		
					fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[k]),0,j ) ));
				fprintf(stderr,";\n",i);
			}
		fprintf(stderr,"covs now\n");*/
		//for(i=0;i<numGestures;i++) {
			
			cvCalcCovarMatrix( tmp, numTraining, tmpCov, Means[i],CV_COVAR_SCALE|CV_COVAR_NORMAL); //Means[i] , |2
			fprintf(stderr,"%d\n",i);			
			for(k=0;k<numFeatures;k++) {	
				for(j=0;j<numFeatures;j++)		
					fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmpCov),k,j ) ));
				fprintf(stderr,";\n",i);
			}
			eigenVects[i]=cvCreateMat(numFeatures,numFeatures,CV_64FC1);
			eigenVals[i]=cvCreateMat(1,numFeatures,CV_64FC1);
			cvEigenVV(tmpCov,eigenVects[i],eigenVals[i],0);
			fprintf(stderr,"Eigenvalues:\n");
			for(k=0;k<numFeatures;k++)
			{
				fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)eigenVals[i]),0,k ) ));
			}
			fprintf(stderr,";\n",i);
			for(k=0;k<numFeatures;k++) {	
				for(j=0;j<numFeatures;j++)		
					fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)eigenVects[i]),k,j ) ));
				fprintf(stderr,";\n",i);
			}
			//invCovMat[i] = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
			//cvInvert(tmpCov,invCovMat[i],CV_SVD);
		

		//}
	}
	//fprintf(stderr,"found averages\n");
	/*for(i=0;i<numGestures;i++) {
		fprintf(stderr,"i=%d ",i);
		for(j=0;j<numFeatures;j++)		
			fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,j ) ));
		fprintf(stderr,"\n",i);
	}*/

}

#define TESTING 1
#define test2_path "../../../TestingDat1/01-01.png"
#define test_path "../../../TestingData/01-01.png"
#define train_path "../../../TrainingData/01-01.png"

//This function simply compares the hu moment to the 
int simpleHuRecognize(IplImage * ToMatch,int numTraining)
{
		int indx[] = {6,11,12,14,15,22,27,28,33,36,42,}; 
		int j,choice;
		IplImage * im[10];
		CvSeq * contour[10];
		CvSeq * tomatchcont;
		double d2,d;
		d=10000000;

		im[0]=cvLoadImage("../../../TrainingData/06-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[1]=cvLoadImage("../../../TrainingData/11-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[2]=cvLoadImage("../../../TrainingData/12-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[3]=cvLoadImage("../../../TrainingData/14-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[4]=cvLoadImage("../../../TrainingData/15-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[5]=cvLoadImage("../../../TrainingData/22-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[6]=cvLoadImage("../../../TrainingData/27-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[7]=cvLoadImage("../../../TrainingData/28-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[8]=cvLoadImage("../../../TrainingData/33-1.png",CV_LOAD_IMAGE_GRAYSCALE );
	    im[9]=cvLoadImage("../../../TrainingData/36-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[10]=cvLoadImage("../../../TrainingData/42-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		fprintf(stderr,"debug-1\n");
		for(j=0;j<numTraining;j++)
		{
			fprintf(stderr,"debug-1\n");
			contour[j]=computeContour(im[j]);
		}
		//cvNamedWindow("poop",CV_WINDOW_AUTOSIZE);
		//cvShowImage("poop",im[0]);
		fprintf(stderr,"debug0");
		if((tomatchcont=computeContour(ToMatch))==NULL)
			return 0;
		fprintf(stderr,"debug1");
		for(j=0;j<numTraining; j++) 
		{				
			d2=cvMatchShapes(contour[j],tomatchcont,CV_CONTOURS_MATCH_I1);
			fprintf(stderr,"for %d the distance is %lf\n",j,d2);
			if (d2<d)
			{
				//fprintf(stderr,"for %d the distance is %lf\n",i,d2); 
				d=d2;
				choice=j;
			}
			d2=0;					
		}
		return choice;
}

void testonImage( CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures,int numFeatures,char * filename,int *indx) 
{
	int i,j,t;
	double *Features;
	IplImage* src;
	IplImage* src2;
	//fprintf(stderr,"i=%d j=%d %s \n",i,j,filename);
	src = cvLoadImage( filename);
	src2=binary_threshold_hsl(src);
	cvNamedWindow("SRC2",CV_WINDOW_AUTOSIZE);
	cvShowImage("SRC2",src2);
	cvWaitKey(0);
	Features=computeFDFeatures(src2,8,1);	
	t=Recognize(Features,eigenVects,eigenVals,Means,numGestures,numFeatures,6);
	fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
	free(Features);
}

void test( CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures, int numTesting,int numFeatures,int *indx) {
	
	char num[] ="01-01";
	char *fo = ".png";
	char *filename;
	if(TESTING) {
		filename = (char *)malloc((strlen(test_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,test_path);
	}
	else {
		filename = (char *)malloc((strlen(train_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,train_path);
	}
	
	int stats[numGestures];
	int i,j,t;
	double *Features;
	IplImage* src;
	for(i=0;i<numGestures;i++)
		stats[i]=0;
	
	for(i=0; i<numGestures; i++) {
		for(j=0;j<numTesting;j++) {
			
			if(TESTING)
				filename[21] = '\0';
			else
				filename[22] = '\0';
			
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
			//fprintf(stderr,"i=%d j=%d %s \n",i,j,filename);
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			Features=computeFDFeatures(src,8);	
			t=Recognize(Features,eigenVects,eigenVals,Means,numGestures,numFeatures,5);
			free(Features);
			if(t==i)
				stats[i]++;
		}
	}

	int sum=0;
	for(i=0;i<numGestures;i++) {
		sum = sum+stats[i];
	}
	fprintf(stderr,"Percent Accuracy %f\n",(float)sum/(numGestures*numTesting));

	/*for(i=0;i<numGestures;i++) {
		fprintf(stderr,"num_incorrect for %d=%d\n",i+1,stats[i]);
	}*/
	
}




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
