#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <fftw3.h>
#include <math.h>
#include <cxcore.h>
#include <string.h>
#include <stdlib.h>


#define CVX_RED     CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN   CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE    CV_RGB(0x00,0x00,0xff)


#define b_min 77
#define b_max 127
#define r_min 133
#define r_max 173
#define g_min 
#define g_max 

#define h_min 0
#define h_max 179
#define s_min 48
#define s_max 158

// Eugene Belilovsky
// Camera read in
// viola jones detector
// segmentation/binarization

CvHaarClassifierCascade  * cascade;
CvMemStorage* storage;
fftw_complex mean;
int HAND=1;
int Gesture[]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'b'+'s', 'c', 'c'+'a', 'c'+'l', 'd', 'd'+'c', 'd'+'o', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'l'+'c', 'm', 'n', 'o', 'o'+'p', 'p', 'q', 'r', 'r'+'c', 'r'+'e', 's', 's'+'p', 't', 'u','v', 'w', 'x', 'y', 'z'};

int Gest[] = {'5', 'a', 'b', 'c','C','g','l','L','p','R','v'};

IplImage* binary_threshold_hsl(IplImage* src) {
  
	IplImage* src2=cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_RGB2HSV);
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
static int compare(fftw_complex *f1,fftw_complex *f2)
{
	fftw_complex f1_temp,f2_temp;
	//fprintf(stderr,"What up dawg!\n");

	f1_temp[0]=((fftw_complex *)f1)[0][0]-mean[0];
	f1_temp[1]=((fftw_complex *)f1)[0][1]-mean[1];
	f2_temp[0]=((fftw_complex *)f2)[0][0]-mean[0];
	f2_temp[1]=((fftw_complex *)f2)[0][1]-mean[1];
	//fprintf(stderr,"What up dawgoe!\n");
	if (atan2(f1_temp[1],f1_temp[0])>atan2(f2_temp[1],f2_temp[0]))
		return 1;
 	else if(atan2(f1_temp[1],f1_temp[0])==atan2(f2_temp[1],f2_temp[0])) //optimize possibly
		return 0;
	else 
		return -1;
}

void sortContour(fftw_complex *contour,int N)
{
	//fftw_complex* temp=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(N));
	//fftw_complex mean;
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
	//copy everything into contour
//	fftw_free(temp);
	
}
double * computeFDFeatures(IplImage* segmented, int N,int wait=0)
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
	//printf( "Total Contours Detected: %d\n", Nc );
	//Find max contour
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
			//printf("Contour #%d\n", n );
			cvShowImage("Edge", img_8uc3 );
		//	cvWaitKey(0);
		//	printf("%d elements:\n", c->total );
			contour=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(c->total));
			FD=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(c->total));

			for( int i=0; i<c->total; ++i ) {
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
			//	
				//if(wait)
				//	printf("%d+%d*i;\n", p->x, p->y );
				//assemble complex representation here
				contour[i][0]=p->x;
				contour[i][1]=p->y;
			}
			sortContour(contour,c->total);
			
			for( int i=0; i<c->total; ++i ) {
				if(wait)
					printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
			}


			if(wait)
				cvWaitKey(0);
		}
		n++;
	}
	//try downspampling later
	
	//printf("Finished all contours.\n");
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

CvSeq * computeContour(IplImage* segmented,int wait=0)
{
	cvNamedWindow( "Edge",1);

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
	
	//malloc error checking
	
	//printf( "Total Contours Detected: %d\n", Nc );
	//Find max contour
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
			//printf("Contour #%d\n", n );
			cvShowImage("Edge", img_8uc3 );
		//	cvWaitKey(0);
		//	printf("%d elements:\n", c->total );

		/*	for( int i=0; i<c->total; ++i ) {
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
			//	
				//if(wait)
				//	printf("%d+%d*i;\n", p->x, p->y );
				//assemble complex representation here
				contour[i][0]=p->x;
				contour[i][1]=p->y;
			}*/
			


			if(wait)
				cvWaitKey(0);
			HAND=1;
			return c;
		}
		n++;
	}
	HAND=0;

	//printf("Finished all contours.\n");
	//destroy fftw_plan
	cvCvtColor( segmented, img_8uc3, CV_GRAY2BGR );
	cvShowImage( "Edge", img_8uc3 );
	//cvWaitKey(0);
	//cvDestroyWindow( "Edge" );
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




#define DEBUG 0

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
	char * filename ="../../../TestingSData/test6.jpg";
	IplImage * testImag=cvLoadImage("../../../testNoShad.jpg");
	IplImage * testSegmented;
	CvArr** eigenVects= (CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** eigenVals=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	CvArr** Means=(CvArr **)malloc(sizeof(CvArr *)*numGestures);
	train(Means,eigenVects,eigenVals,numGestures,numTraining,numFeatures,indx);
	//test(Means,eigenVects,eigenVals,numGestures,numTesting,numFeatures,indx);
	//cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);	
	testonImage(Means,eigenVects,eigenVals,numGestures,numFeatures,filename,indx);
	
	//testSegmented=binary_threshold_hsl(testImag);

	
	//t=simpleHuRecognize(testSegmented,11);
	
	//fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
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
	cascade = (CvHaarClassifierCascade*)cvLoad( "../NewNewtrained.xml",
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
