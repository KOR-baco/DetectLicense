#include "myallheader.h"
#include "myutil.h"

void InitialFilter(IplImage *myimg, int threshold, int openning, int closing)
{
	cvThreshold(myimg, myimg, threshold, 255, CV_THRESH_BINARY);// �Ӱ�ġ ����ȭ

	if(openning){
		printf("���� ���� ����\n");
		cvErode(myimg,myimg,NULL,OPENNING_CNT);//ħ��
		cvDilate(myimg,myimg,NULL,OPENNING_CNT);//��â 
	}
	if(closing){
		printf("���� ���� ����\n");
		cvDilate(myimg,myimg,NULL,CLOSING_CNT);//��â
		cvErode(myimg,myimg,NULL,CLOSING_CNT);//ħ�� 
	}
}