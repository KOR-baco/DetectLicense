#include "myallheader.h"
#include "myutil.h"

void InitialFilter(IplImage *myimg, int threshold, int openning, int closing)
{
	cvThreshold(myimg, myimg, threshold, 255, CV_THRESH_BINARY);// 임계치 이진화

	if(openning){
		printf("열림 연산 수행\n");
		cvErode(myimg,myimg,NULL,OPENNING_CNT);//침식
		cvDilate(myimg,myimg,NULL,OPENNING_CNT);//팽창 
	}
	if(closing){
		printf("열림 연산 수행\n");
		cvDilate(myimg,myimg,NULL,CLOSING_CNT);//팽창
		cvErode(myimg,myimg,NULL,CLOSING_CNT);//침식 
	}
}