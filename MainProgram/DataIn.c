
#include "DataIn.h"

void ReadInCovMatrix(char* filename,int numFeatures,CvMat * mat)
{
	FILE *file=NULL;
	int i,j;
	double element;
	double *dp = &element;
	if((file=fopen(filename,"r"))==NULL)fprintf(stderr,"Error: can't open file\n");

	
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
#if (DEBUG==1)
	for(j=0;j<numFeatures;j++)
	{
		for(i=0;i<numFeatures;i++)
		{	
			fprintf(stderr,"%lf ",CV_MAT_ELEM( *mat, double, j, i ));
							
		}
		fprintf(stderr, "\n");
	}
#endif
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

	
	
	for(j=0;j<numGestures;j++)
	{
		Means[j]=cvCreateMat(1,numFeatures,CV_64FC1);
		for(i=0;i<numFeatures-1;i++)
		{	
			fscanf(file,"%lf ",dp);
			*( (double*)CV_MAT_ELEM_PTR( *(Means[j]), 0,i ) ) = element;
			
		}
		fscanf(file,"%lf",dp);
		*( (double*)CV_MAT_ELEM_PTR( *(Means[j]), 0,i ) ) = element;
	}
#if (DEBUG==1)
	for(j=0;j<numGestures;j++)
	{
		for(i=0;i<numFeatures-1;i++)
		{	
			fprintf(stderr,"%lf ",CV_MAT_ELEM( *Means[j], double, 0, i ));
		}
		fprintf(stderr,"%lf ",CV_MAT_ELEM( *Means[j], double, 0, i ));
	}
#endif
}


void ReadInData(char *filenamesCov,char * FileMeans,CvArr** invCovMatrices,CvArr** Means,int numGestures,int numFeatures)
{
	char *cov = (char *)malloc(sizeof(char)*strlen(filenamesCov)+7);
	strcpy(cov,filenamesCov);
	CvMat * mat;

	int i,j;
	char dig[3];
	for(i=0;i<numGestures;i++)
	{
#if (DEBUG==1)
		fprintf(stderr,"%d ",i);
#endif		
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
#if (DEBUG==1)
		fprintf(stderr,"%s \n",cov);
#endif
		cov[strlen(filenamesCov)]='\0';
	}
	ReadInMeans(FileMeans,numFeatures,Means,numGestures);
}

