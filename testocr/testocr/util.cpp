#include "myallheader.h"
#include "myutil.h"

void Swap(MYRECT *x, MYRECT *y){
   int topTemp,bottomTemp,leftTemp,rightTemp,widthTemp,heightTemp;

   topTemp= x->top;   bottomTemp=x->bottom;   rightTemp=x->right; leftTemp=x->left;   widthTemp=x->width; heightTemp=x->height;
   x->top=y->top;      x->bottom=y->bottom;   x->right=y->right;   x->left=y->left;   x->width=y->width;   x->height=y->height;
   y->top=topTemp;      y->bottom=bottomTemp;   y->right=rightTemp;   y->left=leftTemp;   y->width=widthTemp;   y->height=heightTemp;
}

int IsContain(MYRECT *x, MYRECT *y){
   if (x->left < y->left && x->bottom > y->bottom
      && x->top < y->top && x->bottom > y->bottom)
      return 1;
   else
      return 0;
}