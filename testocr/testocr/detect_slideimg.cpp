#include "myallheader.h"
#include "myutil.h"

int SetSlideRoi(IplImage *srcImage, MYRECT *roi)
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
   //MYRECT licenseRect[MAXRECT];
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

   // TO DO list : 부추 좌상좌표로 x,y 둘다 정렬
   /*부추 소스*/ /* x좌표순으로 정렬*/
   for(i=0;i<rectCnt;i++){
      for(j=0;j<rectCnt;j++){
         if(contoRect[i].left < contoRect[j].left)
            Swap(&contoRect[i],&contoRect[j]);
      }
   }
#if DRAWCONTORECT
   for(i=0;i<rectCnt;i++){
      cvRectangle(dstImage, cvPoint(contoRect[i].left, contoRect[i].top), cvPoint(contoRect[i].right, contoRect[i].bottom), CV_RGB(255,0,0), 2);
      cvShowImage("rect",dstImage);
      waitKey(0);
   }
#endif

   int count =0, maxCnt=0;
   int dx, dy;
   int first=0,last=0;
   float gradient=0, bGradient=0;

   for(i=0;i<rectCnt-1;i++){
      count=0;
      if(contoRect[i].width > 100) continue;
      if(contoRect[i].height < contoRect[i].width) continue;
      for(j=i+1;j<rectCnt;j++){
         if(abs(contoRect[i].width-contoRect[j].width) > 20) continue;
         if(abs(contoRect[i].height-contoRect[j].height) > 20) continue;
         if(contoRect[j].height < contoRect[j].width) continue;

         dx = contoRect[j].left - contoRect[i].left;
         //if(dx > 100) break;
         dy = abs(contoRect[j].top - contoRect[i].top);
         if(dy > 50) continue;
         //if(IsContain(&contoRect[i],&contoRect[j])) continue;

         if(dx==0) dx=1; if(dy==0) dy=1;
         bGradient = gradient;
         gradient = (float)dy/(float)dx;

         printf("i:%d, j:%d, dx:%d, dy:%d ",i,j,dx,dy);
         printf("gr:%lf bgr:%lf  grad :%lf \n",gradient, bGradient,abs( gradient - bGradient ));
         if(gradient < 0.5 && abs( gradient - bGradient ) < 0.2){
            count+=1; 
            if(count>maxCnt){
               first = i; last=j; maxCnt=count;
               printf("first:%d, last:%d \n",first,last);
            }
         }
      }
   }
   if(first ==0 && last ==0) 
   {
      cvReleaseMemStorage(&storage);
      cvReleaseImage(&dstImage);
      return isDetect;
   }

   isDetect=1;
   roi->top = contoRect[first].top;
   roi->left = contoRect[first].left;
   roi->right = contoRect[last].right;
   roi->bottom = contoRect[last].bottom;
   roi->width = roi->right - roi->left;
   roi->height = roi->bottom - roi->top;

   cvRectangle(dstImage, cvPoint(contoRect[first].left, contoRect[first].top), cvPoint(contoRect[first].right, contoRect[first].bottom), CV_RGB(0,255,0), 2);
   cvRectangle(dstImage, cvPoint(contoRect[last].left, contoRect[last].top), cvPoint(contoRect[last].right, contoRect[last].bottom), CV_RGB(0,255,0), 2);
   cvRectangle(dstImage, cvPoint(contoRect[first].left, contoRect[first].top), cvPoint(contoRect[last].right, contoRect[last].bottom), CV_RGB(255,0,0), 2);
   cvShowImage("bochu",dstImage);
   waitKey(0);
   cvReleaseMemStorage(&storage);
   cvReleaseImage(&dstImage);
   return isDetect;
}