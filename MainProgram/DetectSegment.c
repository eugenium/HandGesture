#include "DetectSegment.h"
extern CvHaarClassifierCascade  *cascade;
extern CvMemStorage *storage;

IplImage* binary_threshold_hsl(IplImage* src,int DilateNum) {
  
	int y,x;
	int y1[5],x1[2];
	
	IplImage* src2=cvCreateImage( cvGetSize(src), src->depth,3);
	cvCvtColor(src,src2,CV_BGR2HSV);
	IplImage* dst = cvCreateImage( cvGetSize(src), src->depth,1);
	IplConvKernel * myKernel= cvCreateStructuringElementEx(DilateNum,DilateNum,0,0,CV_SHAPE_RECT,NULL);
	for( y=0; y<src2->height; y++ ) {
  		uchar* ptr = (uchar*) (
     		src2->imageData + y * src->widthStep
		);
		uchar* ptr2 = (uchar*) (
     		dst->imageData + y * dst->widthStep
  		);

	  	for( x=0; x<src->width; x++ ) {
				if(((ptr[3*x+0] > h_min) && (ptr[3*x+0] < h_max))
							&& ((ptr[3*x+1] > s_min)&&(ptr[3*x+1] < s_max)) )
					ptr2[x] =255;
				else
					ptr2[x]=0; 
     }
 	}
	//set the outer perimter to black;
	/*y1[0]=0;
	y1[1]=1;
	y1[2]=src2->height-1;
	y1[3]=src2->height-2;
	y1[4]=3;
	x1[0]=0;
	y1[1]=src2->height-1;
	x1[1]=src2->width-1;
	for( y=0; y<300; y++ ) {
		uchar* ptr2 = (uchar*) (
     		dst->imageData + y * dst->widthStep
  		);

	  	for( x=0; x<src->width; x++ ) {
					ptr2[x]=255; 
     }

 	}*/

	IplImage* img2=cvCreateImage(cvSize(dst->width,dst->height),dst->depth,dst->nChannels);
	cvDilate(dst,img2,myKernel,1); 
	cvReleaseImage( &dst );
	cvReleaseImage(&src2);
	cvReleaseStructuringElement(&myKernel);
	return img2;
}

  
int detect_and_draw(IplImage** img1, IplImage* img, double scale)
{
	//extern CvHaarClassifierCascade  *cascade;
	//extern CvMemStorage *storage;
	int HAND;
	char * str;
	CvScalar colors[] = {
		{{0,0,255}}, {{0,128,255}},{{0,255,255}},{{0,255,0}},
		{{255,128,0}},{{255,255,0}},{{255,0,0}}, {{255,0,255}}
	}; //Just some pretty colors to draw with
//	fprintf(stderr,"f1 %x\n",cascade);
//	fprintf(stderr,"f1 %x\n",storage);
	// IMAGE PREPARATION:
	IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	IplImage* small_img = cvCreateImage(
			cvSize( cvRound(img->width/scale), cvRound(img->height/scale)), 8, 1);
			cvCvtColor( img, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );
	
	// DETECT OBJECTS IF ANY
	if(storage!=NULL)
	cvClearMemStorage( storage );
	CvSeq* objects = cvHaarDetectObjects(small_img,cascade,storage,1.1,2,0,cvSize(0,0),cvSize(35, 35));
	// PICK THE FIRST FOUND OBJECT 

	int tmp;
	if (objects)
		tmp=objects->total;
	else
		tmp=0;
	//fprintf(stderr,"size: %d %d\n",cvGetSize(small_img).width,cvGetSize(small_img).height);
	if( 0< tmp/*(objects ? objects->total : 0)*/)
	{

		CvRect* r = (CvRect*)cvGetSeqElem( objects, 0 );
		*img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);
		if(img1==NULL)
			fprintf(stderr,"Problem\n");
		cvSetImageROI(img,*r);
		cvCopy(img,*img1,NULL);
		cvResetImageROI(img);

		cvRectangle(
					img,
					cvPoint(r->x,r->y),
					cvPoint(r->x+r->width,r->y+r->height),
					colors[0],
					3, 8,0
		);
		
		HAND=1;

	}
	else
	{
		HAND=0;
					
		*img1=cvCreateImage(cvSize(100,100),img->depth,img->nChannels);	
	if(img1==NULL)
			fprintf(stderr,"Problem2\n");
	
	}


	cvReleaseImage( &gray);
	cvReleaseImage( &small_img );
	if(img1==NULL)
			fprintf(stderr,"Problem3\n");
	return HAND;
}