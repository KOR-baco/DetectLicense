#include "myallheader.h"
#include "myutil.h"

#define INPUT_IMG "s6348.jpg"//"x6110.jpg"//"x5553.jpg"//"s7217.jpg"//"s6348.jpg"//"s4507.jpg"//"d2370.jpg"//"c8006.jpg"//"c7115.jpg"//"c6692s.jpg"//"c6692.jpg" //"c4422.jpg" //"c3250.jpg" //"c2943.jpg"
#define OCR_IMG "ocrroi.jpg"

int main()
{
	IplImage    *srcImage, *resizedImage, *filteredImage, *contoImage, *ocrImage;
	float resize;
	MYRECT roi;

	//이미지 로드 & 이미지 리사이즈
	if ((srcImage = cvLoadImage(INPUT_IMG, CV_LOAD_IMAGE_GRAYSCALE)) == NULL)
		return -1;
	resize = sqrtf((float)DEFAULT_SIZE/(float)(srcImage->width*srcImage->height));
	resizedImage = cvCreateImage(cvSize((int)(srcImage->width*resize),(int)(srcImage->height*resize)), IPL_DEPTH_8U, 1);
	cvResize(srcImage, resizedImage, CV_INTER_LINEAR);//이미지 리사이즈
	
	filteredImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 1);
	cvCopy(resizedImage,filteredImage);
	InitialFilter(filteredImage, THRESHOLD2, OPENNING, CLOSING);

	contoImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 1);
	cvCopy(filteredImage,contoImage);
	
	if(SetRoi(contoImage,&roi)){
		cvCopy(filteredImage,contoImage);
		ocrImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 3);
		cvCvtColor(contoImage, ocrImage, CV_GRAY2BGR); // 컬러변환

		cvSetImageROI(ocrImage, cvRect(roi.left, roi.top, roi.width, roi.height));
		cvShowImage("ocrImage", ocrImage);
		cvSaveImage(OCR_IMG, ocrImage);
		cvWaitKey(0);
		DetectOCR(OCR_IMG);
		//cvResetImageROI(ocrImage);
		//cvShowImage("ocrImage", ocrImage);
	}
	else if(SetSlideRoi(contoImage,&roi)){
		cvCopy(filteredImage,contoImage);
		ocrImage = cvCreateImage(cvGetSize(resizedImage), IPL_DEPTH_8U, 3);
		cvCvtColor(contoImage, ocrImage, CV_GRAY2BGR); // 컬러변환

		cvSetImageROI(ocrImage, cvRect(roi.left, roi.top, roi.width, roi.height));
		cvShowImage("ocrImage", ocrImage);
		cvSaveImage(OCR_IMG, ocrImage);
		cvWaitKey(0);
		DetectOCR(OCR_IMG);
		//cvResetImageROI(ocrImage);
		//cvShowImage("ocrImage", ocrImage);
	}

	cvShowImage("resizedImage", resizedImage);
	cvWaitKey(0);
	//이미지 메모리 해제 : IplImage    *srcImage, *resizedImage, *contoImage, *ocrImage;
	cvReleaseImage(&srcImage);
	cvReleaseImage(&resizedImage);
	cvReleaseImage(&contoImage);
	
	return 0;
}