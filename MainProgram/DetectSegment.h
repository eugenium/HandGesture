#ifndef DETECT_SEGMENT_H
#define DETECT_SEGMENT_H

#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <cxcore.h>

#define h_min 0
#define h_max 179
#define s_min 61
#define s_max 227


CvHaarClassifierCascade  *cascade;
CvMemStorage *storage;

IplImage* binary_threshold_hsl(IplImage* src,int DilateNum);
int detect_and_draw(IplImage** img1, IplImage* img, double scale );
#endif
