#include "myallheader.h"
#include "myutil.h"

int SetRoi(IplImage *srcImage, MYRECT *roi) // Set ROI for Detect License
{
	IplImage *myimg = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 1);
	cvCopy(srcImage,myimg);

	int isDetect=0;
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
	//MYRECT roi = {0,};

	IplImage *dstImage = cvCreateImage(cvGetSize(myimg), IPL_DEPTH_8U, 3);
	cvCvtColor(myimg, dstImage, CV_GRAY2BGR); // 컬러변환

	// 콘토 관련 변수 설정
	int mode = CV_RETR_LIST; //모든 외곽선을 검색하고 리스트에 넣는다.
	int method = CV_CHAIN_APPROX_SIMPLE; //수평, 수직, 대각 성분의 끝점만 저장한다. 
	CvPoint offset = cvPoint(0, 0); // 오프셋
	CvMemStorage* storage = cvCreateMemStorage(0);// 메모리 스토리지를 생성하기 위해 사용

	//콘토 스캔
	CvSeq *contour = 0; //시퀀스 변수
	CvSeq *first_contour;
	CvContourScanner scanner = cvStartFindContours(myimg, storage,sizeof(CvContour), mode, method, offset); //콘토 스캔


	while ((contour = cvFindNextContour(scanner)) != 0)
	{
#if CONTODRAW
		cvDrawContours(dstImage, contour, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), 0, 1);// 콘토를 그려주는 함수
#endif
	}
	first_contour = cvEndFindContours(&scanner); // 스캐닝 완료. 시퀀스의 첫번째 원소에 대한 포인터를 반환한다.


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
		printf("conto%d : 가로=%d, 세로=%d\n", i+1, myWidth, myHeight);
		if(myWidth>myHeight*2 && myWidth<myHeight*6){
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
			if(innerConto>=4){
				printf("내부 conto 갯수 : %d\n",innerConto);
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
		isDetect = 1;
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

	roi->top = top;
	roi->bottom = bottom;
	roi->left = left;
	roi->right = right;
	roi->width = width;
	roi->height = height;
	return isDetect;
}