#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cv.h"
#include "highgui.h"
#include <cxcore.h>

void ReadInCovMatrix(char* filename,int numFeatures,CvMat * mat);
void ReadInMeans(char* filename,int numFeatures,CvMat ** Means,int numGestures);
void ReadInData(char *filenamesCov,char * FileMeans,CvArr** invCovMatrices,CvArr** Means,int numGestures,int numFeatures);
