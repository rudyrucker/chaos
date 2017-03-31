/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal codes of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.	                                                     *						                                                           *
******************************************************************************/

#include "forge.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

/* All I use is Replace and XOR! others are wrong! */
static int rop_masks[] = {0,0x10,0x18,0x8};
void PaintRect(rect *R)
{
   char color = (char)thePort->pnColor;
   int rasterop = rop_masks[thePort->pnMode];
   int i;

   for(i=R->Ymin;i<=R->Ymax;i++)
      HLine(R->Xmin,i,R->Xmax,color,rasterop);
}

void FrameRect(rect *R)
{
   char color = (char)thePort->pnColor;
   int rasterop = rop_masks[thePort->pnMode];

   int i;

   HLine(R->Xmin,R->Ymin,R->Xmax,color,rasterop);
   for(i=R->Ymin+1;i<R->Ymax;i++)
   {
      SetPixel(R->Xmin,i);
      SetPixel(R->Xmax,i);
   }
   HLine(R->Xmin,R->Ymax,R->Xmax,color,rasterop);
}

void Centers(rect *R,int *x,int *y)
{
   *x = R->Xmin + (R->Xmax-R->Xmin)/2;
   *y = R->Ymin + (R->Ymax-R->Ymin)/2;
}

void SplitRectV(rect *R,rect *R1,rect *R2)
{
   int cx,cy;
   Centers(R,&cx,&cy);

   *R1 = *R2 = *R;

   R1->Xmax = cx - 1;
   R2->Xmin = cx;
}

void SplitRectH(rect *R,rect *R1,rect *R2)
{
   int cx,cy;
   Centers(R,&cx,&cy);

   *R1 = *R2 = *R;

   R1->Ymax = cy - 1;
   R2->Ymin = cy;
}


void hsv_rgb(unsigned long h, unsigned int s, unsigned int v, unsigned int *r, unsigned int *g, unsigned int *b)
{
	unsigned int i;
	unsigned long lh, f, p, q, t;

	if (s == 0)
	{
		*r = *g = *b = v;
	}
	else
	{
		lh = h * 10000L;
		if (lh == 360000000L)
			lh = 0;
		lh = lh / 6000;
	}

	i = (unsigned int)(lh / 10000);
	f = lh % 10000;
	p = (v * (10000L - s)) / 10000L;
	q = (v * (10000L - ((s * f) / 10000L))) / 10000L;
	t = (v * (10000L - ((s * (10000L - f))) / 10000L)) / 10000L;
	switch (i)
	{

	case 0:
		*r = (int)v;
		*g = (int)t;
		*b = (int)p;
		break;

	case 1:
		*r = (int)q;
		*g = (int)v;
		*b = (int)p;
		break;

	case 2:
		*r = (int)p;
		*g = (int)v;
		*b = (int)t;
		break;

	case 3:
		*r = (int)p;
		*g = (int)q;
		*b = (int)v;
		break;

	case 4:
		*r = (int)t;
		*g = (int)p;
		*b = (int)v;
		break;

	case 5:
		*r = (int)v;
		*g = (int)p;
		*b = (int)q;
		break;
	}
}

void zap_in_row(unsigned char *buffer,int x,int y,int width)
{
   /* First do the pieces that are not byte-aligned */
   int bytes;

   while(width && (x & 7))
   {
      SetPixel10(x++,y,*buffer++);
      width--;
   }

   /* Now do the pieces that are byte-aligned */
   bytes = width / 8;
   
   if (bytes)
      blattbytes(buffer,x,y,bytes * 8);

   width -= bytes * 8;
   x += bytes * 8;
   buffer += bytes * 8;

   while(width)
   {
      SetPixel10(x++,y,*buffer++);
      width--;
   }


}

void invert_item_round(rect *R)
{
   PenColor(MENUTEXT);
   RasterOp(zXORz);
   FrameRoundRect(R,8,8);
   RasterOp(zREPz);
}

/* and some handrolls for quick VGA things: mostly the ticker... */
void VGAHLine(int x1,int y,int x2,char color)
{
   memset((char *)MK_FP(0xa000,0)+y*320+x1,color,(size_t)(x2-x1)+1);
}

void VGAPaintRect(rect *R,char color)
{
   int i;
   for(i=R->Ymin;i<=R->Ymax;i++)
      VGAHLine(R->Xmin,i,R->Xmax,color);
}

void VGAVLine(int x,int y1,int y2,char color)
{
   char *screen = MK_FP(0xa000,0);
   int i;
   for(i=y1;i<=y2;i++)
      screen[320*i+x] = color;
}

void VGASet4Pixels(int x,int y,int xc,int yc,char n)
{
   char *screen = MK_FP(0xa000,0);

   int rowaddr;

   rowaddr = 320*(yc+y);
   screen[rowaddr+xc+x] = screen[rowaddr+xc-x] = n;
   rowaddr = 320*(yc-y);
   screen[rowaddr+xc+x] = screen[rowaddr+xc-x] = n;
}



void VGAOval(rect *frame,char color)
{

   int width = frame->Xmax-frame->Xmin+1;
   int height = frame->Ymax - frame->Ymin + 1;

   int xc = frame->Xmin + width/2;
   int yc = frame->Ymin + height/2;

   int b0 = height/2;
   int a0 = width/2;

   int x=0;
   int y=b0;
   long a=a0;
   long b=b0;

   long Asquared = a*a;
   long TwoAsquared = 2*Asquared;
   long Bsquared = b*b;
   long TwoBsquared = 2*Bsquared;

   long d,dx,dy;

   d = Bsquared - Asquared*b + Asquared/4L;
   dx = 0;
   dy = TwoAsquared * b;

   while(dx < dy)
   {
      VGASet4Pixels(x,y,xc,yc,color);
      if (d > 0L)
      {
         --y;
         dy -= TwoAsquared;
         d -= dy;
      }

      ++x;
      dx += TwoBsquared;
      d += Bsquared + dx;
   }

   d += (3L * (Asquared-Bsquared)/2L - (dx+dy))/2L;

   while(y>=0)
   {
      VGASet4Pixels(x,y,xc,yc,color);
      if (d < 0L)
      {
         ++x;
         dx += TwoBsquared;
         d += dx;
      }
     
      --y;
      dy -= TwoAsquared;
      d += Asquared - dy;
   }
}

