//#include "stdafx.h"
#include <string.h>
#include <allheaders.h>
#include <baseapi.h>

#include <opencv\cv.h>
#include <opencv2\opencv.hpp>

#pragma comment(lib, "liblept168")
#pragma comment(lib, "libtesseract302")

using namespace std;
using namespace tesseract;
using namespace cv;

typedef struct{
	int top;
	int left;
	int bottom;
	int right;

	int width;
	int height;
}MYRECT;

void InitialFilter(IplImage *myimg, int threshold, int openning, int closing);
int SetRoi(IplImage *srcImage, MYRECT *roi); // Set ROI for Detect License
int SetSlideRoi(IplImage *srcImage, MYRECT *roi);
void DetectOCR(char *img);
int IsContain(MYRECT *x, MYRECT *y);
void Swap(MYRECT *x, MYRECT *y);