#include <stdio.h>
#include "highgui.h"
#include "cv.h"
#include <fftw3.h>
#include <math.h>
#include <cxcore.h>
#include <string.h>
#include <stdlib.h>

#include "DetectSegment.h"


#ifndef RECOGNITION_H
#define RECOGNITION_H



#define CVX_RED     CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN   CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE    CV_RGB(0x00,0x00,0xff)

#define TESTING 1
#define test2_path "../TestinDData/01-01.png"
#define test_path  "../TestingData/01-01.png"
#define train_path "../TrainingData/01-01.png"
#define END 16 // was before void trainDHu

#define SIGN(X) ((X)>0?1:-1)

fftw_complex mean;
//static int Gest[]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'B', 'c', 'C', 'L', 'd', 'D', 'O', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'T', 'm', 'n', 'o', 'P', 'p', 'q', 'r', 'R', ';', 's', 'l', 't', 'u','v', 'w', 'x', 'y', 'z'};
CvMemStorage *storage1;
static int Gest[] = { 'a', 'b', 'c','g','L','p','R','v'}; //L lclk
static int indx[] = {11,12,15,22,28,33,36,42}; //put in 12
/*62.5% for hu moments*/
/*static int Gest[] = {'5', 'a', 'b', 'c','g','L','p','R','v'}; //L lclk
static int indx[] = {6,11,12,15,22,28,33,36,42}; 
*/
/*
static int Gest[] = {'5', 'b', 'c','g','l','R'}; //L lclk
static int indx[] = {6,12,15,22,27,36}; //put in 12
*/
//static int indx[] ={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};

static int compare(fftw_complex *f1,fftw_complex *f2);
void sortContour(fftw_complex *contour,int N);
CvSeq * computeContour(IplImage* segmented,IplImage* img_8uc3);
double * computeFDFeatures(CvSeq * cont, int N);
double * computeHuFeatures(CvSeq * cont);

double  I1(double*,double*);
double  I2(double*,double*);
double  I3(double*,double*);


int Recognize(double * Features,CvArr** eigenVects,CvArr** eigenVals,CvArr** Means,int numGestures,int numFeatures,int use);
int RecognizeHu(double * Features,double** Means,int numGestures,int numFeatures, double (*funcptr)(double*,double*));
void trainMahaFD(CvArr **Means,CvArr **eigenVects,CvArr ** eigenVals,int numGestures,int numTraining,int numFeatures);
void testFDSeg(CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures, int numTesting,int numFeatures);
void testFD(CvArr** Means,CvArr** eigenVects,CvArr** eigenVals,int numGestures, int numTesting,int numFeatures);

#endif /* RECOGNITION_H*/
