#include "Recognition.h"
#include "DetectSegment.h"
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
//These distances defined in opencv
double I1(double * HuA,double *HuB)
{
	double answer=0;
	int i;
	double mA,mB;

	for(i=0;i<7;i++)
	{
		mA=((double)SIGN(HuA[i]))*log10(fabs(HuA[i]));
		mB=((double)SIGN(HuB[i]))*log10(fabs(HuB[i]));
		answer+=fabs(((double)1)/mA-((double)1)/mB);
	}

	return answer;
}

//These distances defined in opencv
double I2(double * HuA,double *HuB)
{
	double answer=0;
	int i;
	double mA,mB;

	for(i=0;i<7;i++)
	{
		mA=((double)SIGN(HuA[i]))*log10(fabs(HuA[i]));
		mB=((double)SIGN(HuB[i]))*log10(fabs(HuB[i]));
		answer+=fabs((double)mA-(double)mB);
	}

	return answer;
}

//These distances defined in opencv
double I3(double * HuA,double *HuB)
{
	double answer=0;
	int i;
	double mA,mB;

	for(i=0;i<7;i++)
	{
		mA=((double)SIGN(HuA[i]))*log10(fabs(HuA[i]));
		mB=((double)SIGN(HuB[i]))*log10(fabs(HuB[i]));
		answer+=fabs((mA-mB)/mA);
	}

	return answer;
}
double * computeHuFeatures(CvSeq * cont)
{
	
	int i;
	int N=7;
	int n=0;
	int n2;
	double Scale;
	double *Features;
	Features=(double *)malloc(sizeof(double)*N);
	CvMoments moments;
    CvHuMoments huMoments;
	
	if(Features==NULL)
	{
		fprintf(stderr,"Malloc failed");
		exit(1);
	}
	
	cvMoments(cont,&moments,0);
	cvGetHuMoments(&moments,&huMoments);
	Features[0]=huMoments.hu1;
	Features[1]=huMoments.hu2;
	Features[2]=huMoments.hu3;
	Features[3]=huMoments.hu4;
	Features[4]=huMoments.hu5;
	Features[5]=huMoments.hu6;
	Features[6]=huMoments.hu7;

#if (DEBUG==1)
	for( i=0; i<c->total; ++i ) 
	{
			printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
	}
#endif
	return Features;
	
}

//this computes the 7 hu moments as described in the opencv book  based on the contour
double * computeHuBinaryFeatures(IplImage * cont)
{
	
	int i;
	int N=7;
	int n=0;
	int n2;
	double Scale;
	double *Features;
	Features=(double *)malloc(sizeof(double)*N);
	CvMoments moments;
    CvHuMoments huMoments;
	
	if(Features==NULL)
	{
		fprintf(stderr,"Malloc failed");
		exit(1);
	}
	
	cvMoments(cont,&moments,0);
	cvGetHuMoments(&moments,&huMoments);
	Features[0]=huMoments.hu1;
	Features[1]=huMoments.hu2;
	Features[2]=huMoments.hu3;
	Features[3]=huMoments.hu4;
	Features[4]=huMoments.hu5;
	Features[5]=huMoments.hu6;
	Features[6]=huMoments.hu7;

#if (DEBUG==1)
	for( i=0; i<c->total; ++i ) 
	{
			printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
	}
#endif
	return Features;
	
}
/*
//Computes the Hu Moments based on the binarized image
double * computeHuFeaturesImage(CvSeq * cont)
{
	
	int i;
	int N=7;
	int n=0;
	int n2;
	double Scale;
	double Features;
	Features=(double *)malloc(sizeof(double)*N);
	CvMoments moments;
    CvHuMoments huMoments;
	
	if(Features==NULL)
	{
		fprintf(stderr,"Malloc failed");
		exit(1);
	}
	
	cvMoments(cont,&moments,0);
	cvGetHuMoments(&moments,&huMoments);
	Features[0]=huMoments.hu1;
	Features[1]=huMoments.hu2;
	Features[2]=huMoments.hu3;
	Features[3]=huMoments.hu4;
	Features[4]=huMoments.hu5;
	Features[5]=huMoments.hu6;
	Features[6]=huMoments.hu7;

#if (DEBUG==1)
	for( i=0; i<c->total; ++i ) 
	{
			printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
	}
#endif
	return Features;
	
}
*/
//Computes the Fourier Descriptor Features
double * computeFDFeatures(CvSeq * cont, int N)
{
	
	int i;
	int n=0;
	int n2;
	double Scale;
	double * Features;
	Features=(double *)malloc(sizeof(double)*N);
	fftw_complex *contour;
	fftw_complex *FD;
	fftw_plan plan_forward;
	fprintf(stderr,"FD-1\n");
	if(Features==NULL)
	{
		fprintf(stderr,"Malloc failed");
		exit(1);
	}

	contour=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(cont->total));
	FD=(fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(cont->total));

	for( i=0; i<cont->total; ++i )
	{
		CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, cont, i );	
		//if(wait)
		//	printf("%d+%d*i;\n", p->x, p->y );
		//assemble complex representation here
		contour[i][0]=p->x;
		contour[i][1]=p->y;
	}
	//sortContour(contour,cont->total);
	//Find max contour
	fprintf(stderr,"FD-1\n");
	plan_forward=fftw_plan_dft_1d(cont->total,contour,FD,FFTW_FORWARD,FFTW_ESTIMATE);
	fftw_execute(plan_forward);
	//do fft
	n2=cont->total/2;
	Scale=(double)sqrt(pow(FD[1][0],2)+pow(FD[1][1],2));
	//reduce to 10 coefficients
	//normalize
	if(N+2>=cont->total)
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
#if (DEBUG==1)
	for( i=0; i<c->total; ++i ) 
	{
			printf("%lf+%lf*i;\n",contour[i][0], contour[i][1]);
	}
#endif
	return Features;
	
}

//This function takes a segmented image and computes the biggest outer cont
//returns CvSeq
CvSeq * computeContour(IplImage* segmented,IplImage* img_8uc3)
{
	IplImage* img_edge = cvCreateImage( cvGetSize(segmented), 8, 1 );
//	IplImage* img_8uc3 = cvCreateImage( cvGetSize(segmented), 8, 3 );
	cvThreshold( segmented, img_edge, 128, 255, CV_THRESH_BINARY );
	//storage1 = cvCreateMemStorage(0);

	/*if( storage1==NULL ) {
		
	} 
	else {
		cvClearMemStorage(storage1);
	}*/
	storage1 = cvCreateMemStorage(0);
	CvSeq* first_contour;
	CvSeq* c;
	int Nc = cvFindContours(
				img_edge,
				storage1,
				&first_contour,
				sizeof(CvContour),
				CV_RETR_EXTERNAL // Try all four values and see what happens
				, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0)				
				);
	int i;
	int n=0;
	int best=0;
	int current=0;
	int n2;
	
//	fprintf(stderr, "Total Contours Detected: %d\n", Nc );
	//if( Nc ) {
	for(c=first_contour; c!=NULL; c=c->h_next ) {
		//fprintf(stderr,"contour is %d and has %d points\n",n,c->total);
		if((c->total)>current)
		{
			best=n;
			current=c->total;
		}		
		n++;
	}
	//fprintf(stderr,"best is %d\n",best);
	n=0;
	for( c=first_contour; c!=NULL; c=c->h_next ) {
		if((n==best && (c->total >20)))
		{  
			fprintf(stderr,"drawing %d\n",n);
			cvCvtColor( segmented, img_8uc3, CV_GRAY2BGR );
			cvDrawContours(
				img_8uc3,
				c,
				CVX_RED,
				CVX_BLUE,
				0, // Try different values of max_level, and see what happens
				4,
				4, cvPoint(0,0)
				);

//			cvShowImage("Edge", img_8uc3 );
			//cvReleaseImage( &segmented );
			//fprintf(stderr,"blahblahblah1\n");
			cvReleaseImage( &img_edge );
			return c;
		}
		n++;
	}
	//}
//	HAND=0;
	
	fprintf(stderr,"blah1.05 Nc=%d\n",Nc);
	//if(Nc)
		//cvReleaseMemStorage(&(c->storage));
	fprintf(stderr,"blahblahblah2\n");
	cvReleaseImage( &img_edge );
	return NULL;
	
}

int RecognizeHu(double * Features,double** Means,int numGestures,int numFeatures, double (*funcptr)(double*,double*))
{
	int i,j,z;
	double d=10000;
	double d2,tmp;
	int choice=0;
	//double mahal=0;
/*

	for(i=0;i<numFeatures;i++)
	{
		*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)input), 0,i ) ) = Features[i];
	}
*/
	for(i=0;i<numGestures;i++)
	{

		d2=funcptr(Means[i],Features);	
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


int RecognizeHuT(double * Features,double*** TFeatures,int numGestures,int numFeatures,int numTraining, double (*funcptr)(double*,double*))
{
	int i,j,z;
	double d=10000;
	double d2,tmp;
	int choice=0;
	//double mahal=0;
/*

	for(i=0;i<numFeatures;i++)
	{
		*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)input), 0,i ) ) = Features[i];
	}
*/
	d2=0;
	for(i=0;i<numGestures;i++)
	{
		for(j=0;j<numTraining;j++)
			d2+=funcptr(TFeatures[i][j],Features);
		d2=d2/((double)numTraining);	
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


//This function performs recognition based on the mahalonbis distance, it uses the first (int use) largest eigenvectors/values
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

int RecognizeEucl(double * Features,CvArr** Means,int numGestures,int numFeatures,double (*funcptr)(double*,double*))
{
	int i,j,z;
	double d=1000000;
	double d2,tmp;
	int choice=0;
	double FeatureRay[numFeatures];

	d2=0;
	for(i=0;i<numGestures;i++)
	{

		for(z=0;z<numFeatures;z++)
		{
			d2+=pow((*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,z ) )-Features[z]),2);
			//FeatureRay[z]= *( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,z ) );
		}
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


//This training function assumes the input images are binarized
//small fix otherwise
//Trains for Hu features

void trainDHu(double **Means,int numGestures,int numTraining,int numFeatures) {
	int two=2;
	char num[] ="01-01";
	double *Features;
	char *fo = ".png";
	char filename[] = train_path;//"../../../TrainingData/01-01.png";
	int i,j,k;
	double avg;
	//CvScalar t;
	//CvArr *tmpCov = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
   //	CvArr *tmpEigVec = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
	//1CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numFeatures); 
	CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numTraining);
	for(k=0; k<numTraining; k++)	//1numFeatures
		tmp[k] = cvCreateMat(1,numFeatures,CV_64FC1); //1tmp[k] = cvCreateMat(1,numTraining,CV_64FC1);

	IplImage* src;
	IplImage* img_8uc3;
	CvSeq *cont;
	
	for(i=0; i< numGestures; i++) {
		//Means[i] = cvCreateMat(1,numFeatures,CV_64FC1);
		Means[i]=(double*)malloc(sizeof(double)*numFeatures);
		for(j=0;j<numTraining; j++) {

			filename[END] = '\0';

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
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			
			img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );

			cont = computeContour(src, img_8uc3);

			Features=computeHuFeatures(cont);

			fprintf(stderr,"numFeatures: %d\n",numFeatures);		
			for(k=0;k<numFeatures;k++)		
					fprintf(stderr," %lf ",Features[k]);
			fprintf(stderr,";\n");

			for(k=0; k < numFeatures; k++)
				*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[j]),0,k ) ) = Features[k]; 
			//fprintf(stderr,"copied values\n");
			free(Features);
			cvReleaseImage( &img_8uc3  );
			//cvClearMemStorage( cont->storage );
				
		}

		for(k=0;k<numFeatures;k++) {
			avg=0;
			for(j=0;j<numTraining;j++)
				avg = avg+CV_MAT_ELEM( *((CvMat*)tmp[j]), double, 0, k );
			avg=avg/(double)numTraining;
			//fprintf(stderr,"%lf\n",t.val[0]);
			Means[i][k]=avg;
		//	*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,k ) ) = avg;
		}
		
		// print the feature vectors
			//for(k=0;k<numTraining;k++) {	
				for(j=0;j<numFeatures;j++)		
					fprintf(stderr," %lf ",Means[i][j]);
				fprintf(stderr,";\n");
		//	}
		fprintf(stderr,"covs now\n");
		//for(i=0;i<numGestures;i++) {
			
	}
	//fprintf(stderr,"found averages\n");

}


void trainDTHu(double ***TFeatures,int numGestures,int numTraining,int numFeatures) {
	int two=2;
	char num[] ="01-01";
	double *Features;
	char *fo = ".png";
	char filename[] = train_path;//"../../../TrainingData/01-01.png";
	int i,j,k;
	double avg;
	//CvScalar t;
	//CvArr *tmpCov = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
   //	CvArr *tmpEigVec = cvCreateMat(numFeatures,numFeatures,CV_64FC1);
	//1CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numFeatures); 
	CvArr **tmp= (CvArr **)malloc(sizeof(CvArr *)*numTraining);
	for(k=0; k<numTraining; k++)	//1numFeatures
		tmp[k] = cvCreateMat(1,numFeatures,CV_64FC1); //1tmp[k] = cvCreateMat(1,numTraining,CV_64FC1);

	IplImage* src;
	IplImage* img_8uc3;
	CvSeq *cont;
	
	for(i=0; i< numGestures; i++) {
		//Means[i] = cvCreateMat(1,numFeatures,CV_64FC1);
		TFeatures[i]=(double**)malloc(sizeof(double*)*numTraining);

		if(TFeatures[i]==NULL)
			perror("malloc");

		for(j=0;j<numTraining;j++){
			TFeatures[i][j]=(double*)malloc(sizeof(double)*numFeatures);
		}

		for(j=0;j<numTraining; j++) {

			filename[END] = '\0';

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
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			
			img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );

			cont = computeContour(src, img_8uc3);

			Features=computeHuBinaryFeatures(src);

			//fprintf(stderr,"got contour\n");		
			for(k=0;k<numFeatures;k++)
			{
				TFeatures[i][j][k]=Features[k];
			}
			//fprintf(stderr,"copied values\n");
			free(Features);
			cvReleaseImage( &img_8uc3  );
			cvClearMemStorage( cont->storage );
				
		}
	/*	for(k=0;k<numFeatures;k++) {
			avg=0;
			for(j=0;j<numTraining;j++)
				avg = avg+CV_MAT_ELEM( *((CvMat*)tmp[j]), double, 0, k );
			avg=avg/(double)numGestures;
			//fprintf(stderr,"%lf\n",t.val[0]);
			Means[i][k]=avg;
		//	*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)Means[i]),0,k ) ) = avg;
		}*/
		
		// print the feature vectors
			/*for(k=0;k<numTraining;k++) {	
				for(j=0;j<numFeatures;j++)		
					fprintf(stderr," %lf ",*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[k]),0,j ) ));
				fprintf(stderr,";\n",i);
			}
		fprintf(stderr,"covs now\n");*/
		//for(i=0;i<numGestures;i++) {
			
	}
	//fprintf(stderr,"found averages\n");
	for(i=0;i<numGestures;i++) {	
		for(k=0;k<numFeatures;k++)		
			fprintf(stderr," %lf ",TFeatures[i][0][k]);
		fprintf(stderr,";\n",i);
	}
}

//This training function assumes the input images are binarized
//small fix otherwise
//Trains for Hu features
#define END 16
void trainMahaHu(CvArr** Means,CvArr** eigenVals,CvArr** eigenVects,int numGestures,int numTraining,int numFeatures) {
	int two=2;
	char num[] ="01-01";
	double *Features;
	char *fo = ".png";
	char filename[] = train_path;//"../../../TrainingData/01-01.png";
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
	IplImage* img_8uc3;
	CvSeq *cont;
	
	for(i=0; i< numGestures; i++) {
		Means[i] = cvCreateMat(1,numFeatures,CV_64FC1);

		for(j=0;j<numTraining; j++) {

			filename[END] = '\0';

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
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			
			img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );

			cont = computeContour(src, img_8uc3);

			Features=computeHuFeatures(cont);
			//fprintf(stderr,"got contour\n");		
			for(k=0; k < numFeatures; k++)
				*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[j]),0,k ) ) = Features[k]; 
			//fprintf(stderr,"copied values\n");
			free(Features);
			cvReleaseImage( &img_8uc3  );
			cvClearMemStorage( cont->storage );
				
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
			cvEigenVV(tmpCov,eigenVects[i],eigenVals[i],DBL_EPSILON,0,0);
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
//This training function assumes the input images are binarized
//small fix otherwise
//Trains for FD features
#define END 16
void trainMahaFD(CvArr **Means,CvArr **eigenVects,CvArr ** eigenVals,int numGestures,int numTraining,int numFeatures) {
	int two=2;
	char num[] ="01-01";
	double *Features;
	char *fo = ".png";
	char filename[] = train_path;//"../../../TrainingData/01-01.png";
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
	IplImage* img_8uc3;
	CvSeq *cont;
	
	for(i=0; i< numGestures; i++) {
		Means[i] = cvCreateMat(1,numFeatures,CV_64FC1);

		for(j=0;j<numTraining; j++) {

			filename[END] = '\0';

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
			src = cvLoadImage( filename,CV_LOAD_IMAGE_GRAYSCALE );
			
			img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );

			cont = computeContour(src, img_8uc3);

			Features=computeFDFeatures(cont,numFeatures);
			//fprintf(stderr,"got contour\n");		
			for(k=0; k < numFeatures; k++)
				*( (double*)CV_MAT_ELEM_PTR( *((CvMat *)tmp[j]),0,k ) ) = Features[k]; 
			//fprintf(stderr,"copied values\n");
			free(Features);
			cvReleaseImage( &img_8uc3  );
			cvClearMemStorage( cont->storage );
				
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
			cvEigenVV(tmpCov,eigenVects[i],eigenVals[i],DBL_EPSILON,0,0);
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



//This function simply compares the hu moment to the 
int simpleHuRecognize(CvSeq * tomatchcont,int numTraining)
{
	//	int indx[] = {6,11,12,14,15,22,27,28,33,36,42,}; 
		int j,choice;
		IplImage * im[10];
		CvSeq * contour[10];
		//CvSeq * tomatchcont;
		double d2,d;
		IplImage * img_8uc3;
		d=10000000;

		im[0]=cvLoadImage("../TrainingData/06-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[1]=cvLoadImage("../TrainingData/11-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[2]=cvLoadImage("../TrainingData/12-1.png",CV_LOAD_IMAGE_GRAYSCALE );
	//	im[3]=cvLoadImage("../TrainingData/14-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[3]=cvLoadImage("../TrainingData/15-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[4]=cvLoadImage("../TrainingData/22-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[5]=cvLoadImage("../TrainingData/27-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[6]=cvLoadImage("../TrainingData/28-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[7]=cvLoadImage("../TrainingData/33-1.png",CV_LOAD_IMAGE_GRAYSCALE );
	    im[8]=cvLoadImage("../TrainingData/36-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		im[9]=cvLoadImage("../TrainingData/42-1.png",CV_LOAD_IMAGE_GRAYSCALE );
		//fprintf(stderr,"debug-1\n");
		for(j=0;j<numTraining;j++)
		{
		//	fprintf(stderr,"debug-1\n");
			img_8uc3 = cvCreateImage( cvGetSize(im[j]), 8, 3 );
			contour[j]=computeContour(im[j],img_8uc3);
			cvReleaseImage(&img_8uc3);
		}
		//cvNamedWindow("poop",CV_WINDOW_AUTOSIZE);
		//cvShowImage("poop",im[0]);
		//fprintf(stderr,"debug0");
		//img_8uc3 = cvCreateImage( cvGetSize(im[j]), 8, 3 );
		//if((tomatchcont=computeContour(ToMatch))==NULL)
			//return 0;
		//fprintf(stderr,"debug1");
		for(j=0;j<numTraining; j++) 
		{				
			d2=cvMatchShapes(contour[j],tomatchcont,CV_CONTOURS_MATCH_I1,0);
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

void testonImage( CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures,int numFeatures,char * filename) 
{
	int i,j,t;
	double *Features;
	IplImage* src;
	IplImage* src2;
	//fprintf(stderr,"i=%d j=%d %s \n",i,j,filename);
	src = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
	src2=binary_threshold_hsl(src,9);
	cvNamedWindow("SRC2",CV_WINDOW_AUTOSIZE);
	cvShowImage("SRC2",src2);
	cvWaitKey(0);
	// compute contour
	CvSeq *cont;
	IplImage * img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );
	cont = computeContour(src2, img_8uc3);
	Features=computeFDFeatures(cont,8);	
	t=Recognize(Features,eigenVects,eigenVals,Means,numGestures,numFeatures,6);
	fprintf(stderr,"Found a hand. The gesture recognized is : %c\n",(char) Gest[t]);
	free(Features);
}

void testFDSeg(CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures, int numTesting,int numFeatures) {
	
	char num[] ="01-01";
	char *fo = ".png";
	char *filename;
	if(TESTING) {
		filename = (char *)malloc((strlen(test2_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,test2_path);
	}
	else {
		filename = (char *)malloc((strlen(train_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,train_path);
	}
	CvSeq *cont;
	int stats[numGestures];
	int confusion[numGestures][numGestures];
	int i,j,t,sumNoFeature;
	double *Features;
	IplImage* src;
	IplImage* img1;
	IplImage *img_8uc3;
	sumNoFeature=0;
	
	cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Contour",CV_WINDOW_AUTOSIZE);

	for(i=0;i<numGestures;i++)
		for(j=0;j<numGestures;j++)
				confusion[i][j]=0;

	for(i=0;i<numGestures;i++)
		stats[i]=0;
	
	for(i=0; i<numGestures; i++) {
		for(j=0;j<numTesting;j++) {
			
			if(TESTING)
				filename[END-1] = '\0';
			else
				filename[END] = '\0';
			
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
			src = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
			fprintf(stderr,"1\n");
			img1=binary_threshold_hsl(src,9);
			fprintf(stderr,"img1=%x ---- 2\n",img1);
			cvShowImage("Edge",img1);               
			//cvWaitKey(10000);
			img_8uc3 = cvCreateImage( cvGetSize(img1), 8, 3 );
			cont = computeContour(img1, img_8uc3);
			
			if(cont!=NULL)
			{
				cvShowImage("Contour",img_8uc3);
			}
			else
			{
			//	cvShowImage("Edge",img1);
				sumNoFeature++;
				continue;
			}

			//cvShowImage("Edge",img_8uc3);
			fprintf(stderr,"cont=%x --- 2.5\n",cont);
			Features=computeFDFeatures(cont,numFeatures);	
			fprintf(stderr,"3\n");
			//t=Recognize(Features,eigenVects,eigenVals,Means,numGestures,numFeatures,6);
			t=RecognizeEucl(Features,Means,numGestures,numFeatures,I1);
			free(Features);
			if(t==i)
				stats[i]++;
			confusion[i][t]++;
			cvReleaseImage( &img_8uc3  );
			cvReleaseImage(&img1);
			cvClearMemStorage( cont->storage );

			cvWaitKey(10000);
		}
	}

	int sum=0;
	for(i=0;i<numGestures;i++) {
		sum = sum+stats[i];
	}
	fprintf(stderr,"Percent Accuracy %f\n, Percent Didn't have contours %f\n",(float)sum/(numGestures*numTesting), (float)sumNoFeature/(numGestures*numTesting));

	for(i=0;i<numGestures;i++) {
		fprintf(stderr,"num_correct for %c=%d\n",Gest[i],stats[i]);
	}
	
	for(i=0;i<numGestures;i++) {
		for(j=0;j<numGestures;j++)
			fprintf(stderr," %2d ",confusion[i][j]);
		fprintf(stderr,"\n");
	}	
}

void testDHuSeg(double** Means,int numGestures, int numTesting,int numFeatures) {
	
	char num[] ="01-01";
	char *fo = ".png";
	char *filename;
	if(TESTING) {
		filename = (char *)malloc((strlen(test2_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,test2_path);
	}
	else {
		filename = (char *)malloc((strlen(train_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,train_path);
	}
	CvSeq *cont;
	int stats[numGestures];
	int stats2[numGestures];
	int confusion[numGestures][numGestures];
	int i,j,t,sumNoFeature;
	double *Features;
	IplImage* src;
	IplImage* img1;
	IplImage *img_8uc3;
	sumNoFeature=0;
	
	cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Contour",CV_WINDOW_AUTOSIZE);
	for(i=0;i<numGestures;i++)
		for(j=0;j<numGestures;j++)
				confusion[i][j]=0;
	for(i=0;i<numGestures;i++)
	{
		stats[i]=0;
		stats2[i]=0;
	}	
	for(i=0; i<numGestures; i++) {
		for(j=0;j<numTesting;j++) {
			
			if(TESTING)
				filename[END-1] = '\0';
			else
				filename[END] = '\0';
			
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
			src = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
			fprintf(stderr,"1\n");
			img1=binary_threshold_hsl(src,9);
		//  	fprintf(stderr,"img1=%x ---- 2\n",img1);
		//	cvShowImage("Edge",img1);               
			//cvWaitKey(10000);
			img_8uc3 = cvCreateImage( cvGetSize(img1), 8, 3 );
			cont = computeContour(img1, img_8uc3);
			
			if(cont!=NULL)
			{
				cvShowImage("Contour",img_8uc3);
			}
			else
			{
			//	cvShowImage("Edge",img1);
				sumNoFeature++;
				continue;
			}

			//cvShowImage("Edge",img_8uc3);
			fprintf(stderr,"cont=%x --- 2.5\n",cont);
			Features=computeHuFeatures(cont);	
			fprintf(stderr,"3\n");
			t=RecognizeHu(Features,Means,numGestures,7,I1);
			//t=simpleHuRecognize(cont,10);
			free(Features);
			if(t==i)
				{
					stats[i]++;
				}
			else
				{
					stats2[i]++;
				}
			confusion[i][t]++;
			cvReleaseImage( &img_8uc3  );
			cvReleaseImage(&img1);
			cvClearMemStorage( cont->storage );

			cvWaitKey(10000);
		}
	}

	int sum=0;
	for(i=0;i<numGestures;i++) {
		sum = sum+stats[i];
	}
	fprintf(stderr,"Percent Accuracy %f\n, Percent Didn't have contours %f\n",(float)sum/(numGestures*numTesting), (float)sumNoFeature/(numGestures*numTesting));

	for(i=0;i<numGestures;i++) {
		fprintf(stderr,"num_correct for %c=%d and this gesture was chosen %d times\n",Gest[i],stats[i],stats2[i]);
	}
	for(i=0;i<numGestures;i++) {
		for(j=0;j<numGestures;j++){
			fprintf(stderr," %2d ",confusion[i][j]);
		}
		fprintf(stderr,"\n");
	}	
	
}


void testDTHuSeg(double*** TFeatures,int numGestures, int numTesting, int numTraining,int numFeatures) {
	
	char num[] ="01-01";
	char *fo = ".png";
	char *filename;
	if(TESTING) {
		filename = (char *)malloc((strlen(test2_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,test2_path);
	}
	else {
		filename = (char *)malloc((strlen(train_path)+1)*sizeof(char *));
		filename[0]='\0'; 
		strcat(filename,train_path);
	}
	CvSeq *cont;
	int stats[numGestures];
	int i,j,t,sumNoFeature;
	double *Features;
	IplImage* src;
	IplImage* img1;
	IplImage *img_8uc3;
	sumNoFeature=0;
	
	cvNamedWindow("Edge",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Contour",CV_WINDOW_AUTOSIZE);
	for(i=0;i<numGestures;i++)
		stats[i]=0;
	
	for(i=0; i<numGestures; i++) {
		for(j=0;j<numTesting;j++) {
			
			if(TESTING)
				filename[END-1] = '\0';
			else
				filename[END] = '\0';
			
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
			src = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
			fprintf(stderr,"1\n");
			img1=binary_threshold_hsl(src,9);
		//  	fprintf(stderr,"img1=%x ---- 2\n",img1);
		//	cvShowImage("Edge",img1);               
			//cvWaitKey(10000);
			img_8uc3 = cvCreateImage( cvGetSize(img1), 8, 3 );
			cont = computeContour(img1, img_8uc3);
			
			if(cont!=NULL)
			{
				cvShowImage("Contour",img_8uc3);
			}
			else
			{
			//	cvShowImage("Edge",img1);
				sumNoFeature++;
				continue;
			}

			//cvShowImage("Edge",img_8uc3);
			fprintf(stderr,"cont=%x --- 2.5\n",cont);
			Features=computeHuFeatures(cont);	
			fprintf(stderr,"3\n");
			t=RecognizeHuT(Features,TFeatures,numGestures,7,numTraining,I1
);
			free(Features);
			if(t==i)
				stats[i]++;
			cvReleaseImage( &img_8uc3  );
			//cvReleaseImage(&img1);
			cvClearMemStorage( cont->storage );

			cvWaitKey(10000);
		}
	}

	int sum=0;
	for(i=0;i<numGestures;i++) {
		sum = sum+stats[i];
	}
	fprintf(stderr,"Percent Accuracy %f\n, Percent Didn't have contours %f\n",(float)sum/(numGestures*numTesting), (float)sumNoFeature/(numGestures*numTesting));

	/*for(i=0;i<numGestures;i++) {
		fprintf(stderr,"num_incorrect for %d=%d\n",i+1,stats[i]);
	}*/
	
}


void testFD(CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures, int numTesting,int numFeatures) {
	
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
	CvSeq *cont;
	int stats[numGestures];
	int i,j,t,sumNoFeature;
	double *Features;
	IplImage* src;
	IplImage* img1;
	IplImage *img_8uc3;
	sumNoFeature=0;
	for(i=0;i<numGestures;i++)
		stats[i]=0;
	
	for(i=0; i<numGestures; i++) {
		for(j=0;j<numTesting;j++) {
			
			if(TESTING)
				filename[END-1] = '\0';
			else
				filename[END] = '\0';
			
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
			src = cvLoadImage(filename,CV_LOAD_IMAGE_GRAYSCALE);
			fprintf(stderr,"1\n");
			//img1=binary_threshold_hsl(src);
			fprintf(stderr,"img1=%x ---- 2\n",img1);
			img_8uc3 = cvCreateImage( cvGetSize(src), 8, 3 );
			cont = computeContour(src, img_8uc3);
			if(cont==NULL)
			{
				sumNoFeature++;
				continue;
			}
			fprintf(stderr,"cont=%x --- 2.5\n",cont);
			Features=computeFDFeatures(cont,numFeatures);	
			fprintf(stderr,"3\n");
			t=Recognize(Features,eigenVects,eigenVals,Means,numGestures,numFeatures,6);
			free(Features);
			if(t==i)
				stats[i]++;
			cvReleaseImage( &img_8uc3  );
			cvClearMemStorage( cont->storage );
		}
	}

	int sum=0;
	for(i=0;i<numGestures;i++) {
		sum = sum+stats[i];
	}
	fprintf(stderr,"Percent Accuracy %f\n, Percent Didn't have contours %f\n",(float)sum/(numGestures*numTesting), (float)sumNoFeature/(numGestures*numTesting));

	/*for(i=0;i<numGestures;i++) {
		fprintf(stderr,"num_incorrect for %d=%d\n",i+1,stats[i]);
	}*/
	
}







