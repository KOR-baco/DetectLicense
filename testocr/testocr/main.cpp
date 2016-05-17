/**
 ** 2015.5.17 �ż��� ��������
 **/

//#include "stdafx.h"
#include <string.h>
#include <allheaders.h>
#include <baseapi.h>

#include <opencv\cv.h>
#include <opencv2\opencv.hpp>

#pragma comment(lib, "liblept168")
#pragma comment(lib, "libtesseract302")

#define MAX(x,y) ((x) > (y) ? (x) : (y)) 
#define MIN(x,y) ((x) < (y) ? (x) : (y)) 

#define INPUT_IMG "complex2.jpg"//complex1.jpg //"c6692n.jpg"//"c6692.jpg" //"c8119.jpg"//"c8006.jpg"//"c3250.jpg" //c2370 //c7115
#define OCR_IMG "ocrroi.jpg"

#define DEFAULT_SIZE 382800

#define THRESHOLD2 130 // ����ȭ threshold 80? ~ 150, 40, 100, 130 ���

#define OPENNING 1
#define OPENNING_CNT 1
#define CLOSING 0
#define CLOSING_CNT 1

#define MAXRECT 100
#define CONTODRAW 1

#define NOISEPX 4

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

MYRECT SetRoi(IplImage *myimg); // Set ROI for Detect License
void DetectOCR(char *img);

int main()
{
	IplImage    *srcImage, *resizedImage, *contoImage, *ocrImage;
	float resize;
	MYRECT roi;

	//�̹��� �ε� & �̹��� ��������
	if ((srcImage = cvLoadImage(INPUT_IMG, CV_LOAD_IMAGE_GRAYSCALE)) == NULL)
		return -1;
	resize = sqrtf((float)DEFAULT_SIZE/(float)(srcImage->width*srcImage->height));
	resizedImage = cvCreateImage(cvSize((int)(srcImage->width*resize),(int)(srcImage->height*resize)), IPL_DEPTH_8U, 1);
	cvResize(srcImage, resizedImage, CV_INTER_LINEAR);

	cvThreshold(resizedImage, resizedImage, THRESHOLD2, 255, CV_THRESH_BINARY);// �Ӱ�ġ ����ȭ

	if(OPENNING){
		printf("���� ���� ����\n");
		cvErode(resizedImage,resizedImage,NULL,OPENNING_CNT);//ħ��
		cvDilate(resizedImage,resizedImage,NULL,OPENNING_CNT);//��â 
	}
	if(CLOSING){
		printf("���� ���� ����\n");
		cvDilate(resizedImage,resizedImage,NULL,CLOSING_CNT);//��â
		cvErode(resizedImage,resizedImage,NULL,CLOSING_CNT);//ħ�� 
	}

	contoImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 1);

	cvCopy(resizedImage,contoImage);

	roi = SetRoi(contoImage);

	ocrImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 3);
	cvCvtColor(resizedImage, ocrImage, CV_GRAY2BGR); // �÷���ȯ
	//cvRectangle(ocrImage, cvPoint(roi.left, roi.top), cvPoint(roi.right, roi.bottom), CV_RGB(0,200,0), 2);
	//cvShowImage("ocrImage", ocrImage);
	//cvWaitKey(0);
	cvSetImageROI(ocrImage, cvRect(roi.left, roi.top, roi.width, roi.height));
	cvShowImage("ocrImage", ocrImage);
	cvSaveImage(OCR_IMG, ocrImage);
	cvWaitKey(0);
	DetectOCR(OCR_IMG);
	cvResetImageROI(ocrImage);
	cvShowImage("ocrImage", ocrImage);
	cvShowImage("resizedImage", resizedImage);
	cvWaitKey(0);
	//�̹��� �޸� ���� : IplImage    *srcImage, *resizedImage, *contoImage, *ocrImage;
	cvReleaseImage(&srcImage);
	cvReleaseImage(&resizedImage);
	cvReleaseImage(&contoImage);
	return 0;
}

MYRECT SetRoi(IplImage *myimg) // Set ROI for Detect License
{
	int top=myimg->height;
	int left=myimg->width;
	int bottom=0;
	int right=0;
	int width=0;
	int height=0;

	int xMin, xMax, yMin, yMax;
	int i, j, k;
	int contCnt=0, rectCnt=0, licenseCnt=0;
	MYRECT contoRect[MAXRECT];
	MYRECT licenseRect[MAXRECT];
	MYRECT roi = {0,};
	IplImage *dstImage = cvCreateImage(cvGetSize(myimg), IPL_DEPTH_8U, 3);
	cvCvtColor(myimg, dstImage, CV_GRAY2BGR); // �÷���ȯ

	// ���� ���� ���� ����
	int mode = CV_RETR_LIST; //��� �ܰ����� �˻��ϰ� ����Ʈ�� �ִ´�.
	int method = CV_CHAIN_APPROX_SIMPLE; //����, ����, �밢 ������ ������ �����Ѵ�. 
	CvPoint offset = cvPoint(0, 0); // ������
	CvMemStorage* storage = cvCreateMemStorage(0);// �޸� ���丮���� �����ϱ� ���� ���

	//���� ��ĵ
	CvSeq *contour = 0; //������ ����
	CvSeq *first_contour;
	CvContourScanner scanner = cvStartFindContours(myimg, storage,sizeof(CvContour), mode, method, offset); //���� ��ĵ


	while ((contour = cvFindNextContour(scanner)) != 0)
	{
#if CONTODRAW
		cvDrawContours(dstImage, contour, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), 0, 1);// ���並 �׷��ִ� �Լ�
#endif
	}
	first_contour = cvEndFindContours(&scanner); // ��ĳ�� �Ϸ�. �������� ù��° ���ҿ� ���� �����͸� ��ȯ�Ѵ�.


	for (contour = first_contour, k = 0; contour != 0; contour = contour->h_next, k++)
	{
		xMin = 1000; xMax = 0; yMin = 1000; yMax = 0;
		contCnt = 0;

		for (i = 0; i<contour->total; i++)
		{
			CvPoint* pt = (CvPoint*)cvGetSeqElem(contour, i);

			if (xMin > pt->x) xMin = pt->x;
			if (xMax < pt->x) xMax = pt->x;
			if (yMin > pt->y) yMin = pt->y;
			if (yMax < pt->y) yMax = pt->y;
		}
		/** Store Conto Rect point**/
		contoRect[rectCnt].left = xMin;
		contoRect[rectCnt].right = xMax;
		contoRect[rectCnt].top = yMin;
		contoRect[rectCnt].bottom = yMax;
		contoRect[rectCnt].width = xMax - xMin;
		contoRect[rectCnt].height = yMax - yMin;
		if(contoRect[rectCnt].width>NOISEPX && contoRect[rectCnt].height>NOISEPX){
			rectCnt++; contCnt++;
		}
	}

	/**************************************************
	******** TO DO: Find ROI Source Code ***********
	***************************************************/
	for(i=0; i<rectCnt; i++){
		cvRectangle(dstImage, cvPoint(contoRect[i].left, contoRect[i].top), cvPoint(contoRect[i].right, contoRect[i].bottom), CV_RGB(0,200,200), 1);
		int myWidth, myHeight;
		myWidth = contoRect[i].width;
		myHeight = contoRect[i].height;
		printf("conto%d : ����=%d, ����=%d\n", i+1, myWidth, myHeight);
		if(myWidth>myHeight*2 && myWidth<myHeight*5){
			int innerConto=0;
			MYRECT *bigger = &contoRect[i];
			for(j=0; j<rectCnt; j++){
				if(contoRect[j].height>=(myHeight/2)){
					MYRECT *smaller = &contoRect[j];
					if(bigger->top < smaller->top && bigger->bottom > smaller->bottom && bigger->left <= smaller->left && bigger->right >= smaller->right){
						cvRectangle(dstImage, cvPoint(contoRect[j].left, contoRect[j].top), cvPoint(contoRect[j].right, contoRect[j].bottom), CV_RGB(200,0,0), 2);
						innerConto++;
						bottom = MAX(bottom, contoRect[j].bottom);
						right = MAX(right, contoRect[j].right);
						left = MIN(left, contoRect[j].left);
						top = MIN(top, contoRect[j].top);
					}
				}
			}
			if(innerConto>4){
				printf("���� conto ���� : %d\n",innerConto);
				licenseRect[licenseCnt].top = contoRect[i].top;
				licenseRect[licenseCnt].bottom = contoRect[i].bottom;
				licenseRect[licenseCnt].right = contoRect[i].right;
				licenseRect[licenseCnt].left = contoRect[i].left;
				licenseRect[licenseCnt].width = contoRect[i].width;
				licenseRect[licenseCnt].height = contoRect[i].height;
				licenseCnt++;
			}
		}
	}

	for(i=0; i<licenseCnt; i++){
		int myWidth, myHeight;
		myWidth = licenseRect[i].right-licenseRect[i].left;
		myHeight = licenseRect[i].bottom - licenseRect[i].top;
		cvRectangle(dstImage, cvPoint(licenseRect[i].left, licenseRect[i].top), cvPoint(licenseRect[i].right, licenseRect[i].bottom), CV_RGB(0,200,0), 2);
		
		/*
		roi.top = licenseRect[i].top;
		roi.bottom = licenseRect[i].bottom;
		roi.left = licenseRect[i].left;
		roi.right = licenseRect[i].right;
		roi.width = licenseRect[i].width;
		roi.height = licenseRect[i].height;
		*/
	}

	//cvShowImage("contoimg1", myimg);
	cvShowImage("contoimg2", dstImage);
	cvWaitKey(0);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&dstImage);

	width = right - left;
	height = bottom - top;

	roi.top = top;
	roi.bottom = bottom;
	roi.left = left;
	roi.right = right;
	roi.width = width;
	roi.height = height;
	return roi;
}

void DetectOCR(char *img)
{
	char *outText;
	//const char *path = "/tessdate";
		TessBaseAPI *api = new TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	// Open input image with leptonica library
	Pix *image = pixRead(img);
	//Pix *image = pixRead("cleanroi.jpg");
	api->SetImage(image);
	// Get OCR result

	outText = api->GetUTF8Text();

	printf("OCR output:\n%s", outText);

	printf("��ȣ�� : ");
	for(int i=0; i<strlen(outText); i++){
		if(outText[i] >= '0' && outText[i] <= '9'){
			printf("%c ",outText[i]);
		}
	}printf("\n");


	// Destroy used object and release memory
	api->End();
	//delete[] outText;
	//pixDestroy(&image);
}