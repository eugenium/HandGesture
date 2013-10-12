#include <stdio.h>
#include "highgui.h"
#include "cv.h"

// Detect and draw detected object boxes on image
// Presumes 2 Globals:
// Cascade is loaded by:

// AND that storage is allocated:
 
//

CvHaarClassifierCascade  * cascade;
CvMemStorage* storage;

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
//	     fprintf(stderr,"1\n");
	     CvRect* r = (CvRect*)cvGetSeqElem( objects, 0 );
		 cvRectangle(
		    img,
		    cvPoint(r->x,r->y),
		    cvPoint(r->x+r->width,r->y+r->height),
		    colors[0]
		 );
         cvSetImageROI(img,*r);
	//	 fprintf(stderr,"1.9");
	     img1=cvCreateImage(cvSize(r->width,r->height),img->depth,img->nChannels);
		// fprintf(stderr,"1.99");
	     cvCopy(img,img1,NULL);
	//     fprintf(stderr,"2");
	     cvResetImageROI(img);
		 cvRectangle(
		    img,
		    cvPoint(r->x,r->y),
		    cvPoint(r->x+r->width,r->y+r->height),
		    colors[0]
		 );

	}
    else
	{
	     img1=cvCreateImage(cvSize(35,35),img->depth,img->nChannels);
		 fprintf(stderr,"size: %d %d\n",cvGetSize(img1).width,cvGetSize(img1).height);
	}
	fprintf(stderr,"size: %d %d\n",cvGetSize(img1).width,cvGetSize(img1).height);
   /* cvRectangle(
        img,
        cvPoint(r->x,r->y),
        cvPoint(r->x+r->width,r->y+r->height),
        colors[i%8]
     );
 */
  
  cvReleaseImage( &gray);
  cvReleaseImage( &small_img );
  return img1;
}

void vRecord()
{

}

int main(int argc,char ** argv)
{
   int key;
   //code to record video
	CvVideoWriter *writer=0;
   int isColor=1;
   int fps=10;
   int frameW=640;
   int frameH=480;
      writer=cvCreateVideoWriter("out.avi",CV_FOURCC('D','I','V','X'),fps,cvSize(frameW,frameH),isColor);
  //end record video       
  CvCapture* capture = NULL;
  cascade = (CvHaarClassifierCascade*)cvLoad( "../NewTrained.xml",
     0, 0, 0 );	
  storage = cvCreateMemStorage(0); 
  IplImage* img;
  IplImage* img1;
  if(NULL==(capture = cvCaptureFromCAM(1)))
  {
    printf("\nError on cvCaptureFromCAM");
    return -1;
  }
  
  //-----
  cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Capture2", CV_WINDOW_AUTOSIZE);
  
  cvMoveWindow("Capture", 550, 250);
   cvMoveWindow("Capture2",850, 50);
  
  for(;;){
  	 fprintf(stderr,"came back 0");
    if(NULL==(img=cvQueryFrame(capture))){
      printf("\nError on cvQueryFrame");
      break;
    }

	fprintf(stderr,"img size: %d %d\n",cvGetSize(img).width,cvGetSize(img).height);
    img1=detect_and_draw(img,1);
	
    cvShowImage("Capture", img1);
	cvShowImage("Capture2",img);

	
    cvWriteFrame(writer,img1);
    key = cvWaitKey(10);
    
    if(key==0x1b)
      break;
  }
  
  cvReleaseCapture(&capture);
  cvDestroyWindow("Capture");
  //  CvCapture* capture = cvCaptureFromCAM(0);
   
  cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE );
  // cvRectangle(img, cvPoint(100,100), cvPoint(200,200), cvScalar(255,0,0), 8);
 
   
/*    cvReleaseImage( &img );
    cvDestroyWindow( "Example1");
*/

   cvShowImage( "Example1", img );
   cvWaitKey(0);
}
