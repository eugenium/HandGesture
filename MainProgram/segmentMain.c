#include "Recognition.h"

int main(int argc,int argv[]) {

	int numGestures=10;
	int numTraining=80;	
	int numTesting=19;
	int numFeatures=8;
	char num[] ="01-01";
	char *fo = ".png";
	char *filename;
	int p[3];
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

			cvShowImage("Edge",img1);               
			//cvWaitKey(10000);
			fprintf(stderr,"%s\n",filename);
			filename[9] = 'S';
			
		
			p[0] = CV_IMWRITE_JPEG_QUALITY;
    			p[1] = 100;
    			p[2] = 0;
			cvSaveImage(filename, img1,p);
			fprintf(stderr,"%s\n",filename);
			filename[9] = 'D';
			cvReleaseImage(&img1);


			cvWaitKey(10000);
		}
	}



}