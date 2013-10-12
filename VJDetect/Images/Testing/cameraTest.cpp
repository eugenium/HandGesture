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

void detect_and_draw(IplImage* img, double scale = 1.3)
{
   static CvScalar colors[] = {
      {{0,0,255}}, {{0,128,255}},{{0,255,255}},{{0,255,0}},
      {{255,128,0}},{{255,255,0}},{{255,0,0}}, {{255,0,255}}
   }; //Just some pretty colors to draw with
   // IMAGE PREPARATION:
   //
   IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
   IplImage* small_img = cvCreateImage(
      cvSize( cvRound(img->width/scale), cvRound(img->height/scale)), 8, 1
   );
   cvCvtColor( img, gray, CV_BGR2GRAY );
   cvResize( gray, small_img, CV_INTER_LINEAR );
   cvEqualizeHist( small_img, small_img );
   // DETECT OBJECTS IF ANY
   //
   cvClearMemStorage( storage );
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
  for(int i = 0; i<(objects ? objects->total : 0); i++ )
  {
     CvRect* r = (CvRect*)cvGetSeqElem( objects, i );
     cvRectangle(
        img,
        cvPoint(r->x,r->y),
        cvPoint(r->x+r->width,r->y+r->height),
        colors[i%8]
     );
  }
  
  cvReleaseImage( &gray);
  cvReleaseImage( &small_img );
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
  if(NULL==(capture = cvCaptureFromCAM(1)))
  {
    printf("\nError on cvCaptureFromCAM");
    return -1;
  }
  
  //-----
  cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
 // cvNamedWindow("Capture2", CV_WINDOW_AUTOSIZE);
  
  cvMoveWindow("Capture", 550, 250);
//  cvMoveWindow("Capture2", 320, 50);
  
  for(;;){
  
    if(NULL==(img=cvQueryFrame(capture))){
      printf("\nError on cvQueryFrame");
      break;
    }

    detect_and_draw(img,1);
    cvShowImage("Capture", img);
    cvWriteFrame(writer,img);
    
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
