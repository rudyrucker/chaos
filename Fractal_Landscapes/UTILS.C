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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>
#include <sys\stat.h>
#include <time.h>
#include <dir.h>
#include "scodes.h"
#include "forge.h"
#include <io.h>

double arand, gaussadd, gaussfac;	/* Gaussian random parameters */
int nrand;

void initgauss(unsigned int seed)
{
	nrand = 4;		/* Terms to sum for gaussian noise */
	arand = 32767.0;
	gaussadd = sqrt(3.0 * nrand);
	gaussfac = 2 * gaussadd / (nrand * arand);
	srand(seed);
}

double gauss(void)
{
	int i;
	double sum = 0.0;

	for (i = 1; i <= nrand; i++)
		sum += rand();
	return gaussfac * sum - gaussadd;
}

void initseed(void)
{
	if (!seeded)
	{
		long i;

		i = clock() * 0xF37C;
		srand((int)i);
		for (i = 0; i < 7; i++)
			rand();

		rseed = rand();
		seeded = true;
	}
}

int ShadowAndSave(rect *tR)
{

   rect shR = *tR;
   rect uR;
   rect r1,r2,r3;
   int err;
   int i;

   OffsetRect(&shR,6,6);
   r3 = shR;
   ShiftRect(&r3,-6,-6,&r1,&r2);


   UnionRect(&shR,tR,&uR);

   PushRect(&uR, &err);
   if (err)
      return 0;

   RasterOp(zXORz);
   PenColor(8);
   PaintRect(&r1);
   PaintRect(&r2);
   /* and get the corners hee hee? */
   for(i=1;i<6;i++)
   {
      MoveTo(tR->Xmin+i,tR->Ymax);
      LineTo(tR->Xmin+i,tR->Ymax+i);
      MoveTo(tR->Xmax,tR->Ymin+i);
      LineTo(tR->Xmax+i,tR->Ymin+i);
   }
   MoveTo(tR->Xmax,tR->Ymax);
   LineTo(uR.Xmax,uR.Ymax);
   RasterOp(zREPz);
   return 1;

}


static struct {int x,y;} cursorstack[20];
static int cursorsp=0;

void PushCursorPosition(void)
{
   short cx,cy,cl,cb;
   QueryCursor(&cx,&cy,&cl,&cb);
   cursorstack[cursorsp].x = cx;
   cursorstack[cursorsp].y = cy;

   if (cursorsp < 20)
      cursorsp++;

}

void PopCursorPosition(void)
{
   int curx,cury;
   if (cursorsp)
   {
      cursorsp--;
      curx = cursorstack[cursorsp].x;
      cury = cursorstack[cursorsp].y;
      MoveCursor(curx,cury);

   }
}

void move_to_corner(rect *R)
{
   int cx,cy;
   Centers(R,&cx,&cy);
   MoveCursor(cx,cy);
   //R->Xmin+1,R->Ymin+1);
}

void WaitForNothing(void)
{
   event e;

   while(1)
   {
      KeyEvent(false,&e);
      if (!(e.State & 0x700))
         return;
   }
}



int SomethingWaiting(void)
{
   int n;


   event e;
   n = KeyEvent(false,&e);

   if (n)
   {
      if (!(e.ASCII || e.ScanCode || (e.State & 0x700)))
         return 0;
      else
         return 1;
   }
   else
      return 0;
}

void AbortCheck(void)
{
   aborted = aborted || SomethingWaiting();
}


void TempFileName(char *buf,char *name)
{
   char *pathname = getenv("TMP");
   char *sep;
   char tbuf[128];

   sep = "";
   if (pathname && access(pathname,0))
      pathname = NULL;
   if (!pathname)
   {
      getcwd(tbuf,128);
      pathname = tbuf;
   }
   if (pathname[strlen(pathname)-1] != '\\')
      sep = "\\";

   sprintf(buf,"%s%s%s",pathname,sep,name);


}

void RangeError(char *msg)
{
   rect R;
   int cx,cy;
   int width = 0;
   int i;
   int height = 3 * FontHeight + 4;

   char tbuf[3][128];

   Centers(&sR,&cx,&cy);


   strcpy(tbuf[0],"Range Error! Acceptable values are");
   strcpy(tbuf[1],msg);
   strcpy(tbuf[2],"Press any key or click to continue");

   for(i=0;i<3;i++)
      width = max(width, StringWidth(tbuf[i]) + 4);

   R.Xmin = cx - width/2;
   R.Xmax = R.Xmin + width;
   R.Ymin = cy - height/2;
   R.Ymax = R.Ymin + height;

   HideCursor();

   if (!ShadowAndSave(&R))
   {
      ShowCursor();
      return;
   }

   PenColor(9);
   PaintRect(&R);
   PenColor(0);
   FrameRect(&R);

   PenColor(MENUTEXT);
   BackColor(9);
   TextAlign(alignCenter,alignTop);

   for(i=0;i<3;i++)
   {
      MoveTo(cx,R.Ymin + 2 + FontHeight * i);
      DrawString(tbuf[i]);
   }
   ShowCursor();
   while(1)
   {
      event e;

      KeyEvent(true,&e);

      if ((e.State & 0x700) || e.ASCII || e.ScanCode)
         break;
   }

   HideCursor();
   PopRect(&i);
   ShowCursor();
}

int cancel_ok_msg(char *msg)
{
	int current_item = 1;
	int centerx = sR.Xmax / 2;
	int centery = sR.Ymax / 2;
	rect okRect, cancelRect, R;
	int err;
	int retval = 0;

	int height = FontHeight + FontHeight + 14;
	int width = StringWidth(msg) + 12;

	R.Xmin = centerx - width / 2;
	R.Xmax = R.Xmin + width;
	R.Ymin = centery - height / 2;
	R.Ymax = R.Ymin + height;

	RasterOp(zREPz);

	LimitMouse(R.Xmin, R.Ymin, R.Xmax, R.Ymax);
   HideCursor();
   WaitForNothing();

   if (!ShadowAndSave(&R))
   {
      ShowCursor();
      LimitMouseRect(&sR);
      return 1;
   }
	PenColor(MENUBACK);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	FrameRect(&R);
	PenColor(MENUTEXT);
	BackColor(MENUBACK);
	TextAlign(alignCenter, alignTop);
	MoveTo(centerx, R.Ymin + 1);
	DrawString(msg);

	okRect.Xmin = R.Xmin + 4;
	okRect.Xmax = centerx - 2;
	okRect.Ymax = R.Ymax - 4;
	okRect.Ymin = okRect.Ymax - FontHeight - 4;
   PaintRadioButton(&okRect,false,false,"Yes");
   ExtraHilite(&okRect,false);

	cancelRect.Xmax = R.Xmax - 4;
	cancelRect.Xmin = centerx + 2;
	cancelRect.Ymax = R.Ymax - 4;
	cancelRect.Ymin = cancelRect.Ymax - FontHeight - 4;
   PaintRadioButton(&cancelRect,false,false,"No");
   move_to_corner(&cancelRect);
   PushButton(&cancelRect,true);
   current_item = 1;
	ShowCursor();

	while (1)
	{
		event e;
		int key = 0;
		int button;
      int X,Y;
      int n;
      int last_item = current_item;



		n = KeyEvent(false, &e);
      X = e.CursorX;
      Y = e.CursorY;
      if (XYInRect(X,Y,&okRect))
         current_item = 0;
      else if (XYInRect(X,Y,&cancelRect))
         current_item = 1;
      else
         current_item = -1;

      if (n)
      {

		   if (e.ASCII && e.ASCII != 0xe0)
			   key = e.ASCII;
		   else
			   key = e.ScanCode << 8;
		   button = (e.State >> 8) & 0x7;
		   if (button == swRight)
			   break;
		   if (button == swLeft)
			   key = 0x0d;

		   if (key == 0x0d)
		   {
            if (XYInRect(X,Y,&okRect))
               retval = 1;
            else if (XYInRect(X,Y,&cancelRect))
               retval = 0;
			   break;
		   }

		   if (key == 'y' || key == 'Y')
		   {
			   retval = 1;
			   break;
		   }

		   if (key == 0x1b || key == 'n' || key == 'N')
			   break;

		   if (key == XRARROW || key == XLARROW || key == ' ')
		   {
			   current_item ^= 1;
            move_to_corner(current_item ? &cancelRect : &okRect);
		   }
      }

      if (current_item != last_item)
      {
         switch(last_item) {
         case 0:
            PushButton(&okRect,false);
            ExtraHilite(&okRect,false);
            break;
         case 1:
            PushButton(&cancelRect,false);
            break;
         }
         switch(current_item) {
         case 0:
            PushButton(&okRect,true);
            ExtraHilite(&okRect,true);
            break;
         case 1:
            PushButton(&cancelRect,true);
            break;
         }
      }
 


	}

   /* Depress the appropriate button */
   if (retval == 1)
   {
      PaintRadioButton(&okRect,true,true,"Yes");
      ExtraHilite(&okRect,true);
   }
   else
      PaintRadioButton(&cancelRect,true,true,"No");


	/* Wait for the key to be lifted */
   WaitForNothing();

   if (retval == 1)
   {
      PaintRadioButton(&okRect,false,false,"Yes");
      ExtraHilite(&okRect,false);
   }
   else
      PaintRadioButton(&cancelRect,false,false,"No");

	HideCursor();
	PopRect(&err);
	LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);
   ShowCursor();

	return retval;
}

void ErrorBox(char *s)
{
   int width = StringWidth(s) + 8;
   int height = 2*FontHeight + 4;
   rect R;
   int cx,cy;
   char *msg = "Press any key or click to continue.";
   int n;
   width = max(width,StringWidth(msg)+8);
   Centers(&sR,&cx,&cy);

   R.Xmin = cx - width/2;
   R.Xmax = R.Xmin + width - 1;
   R.Ymin = cy - height / 2;
   R.Ymax = R.Ymin + height - 1;
   HideCursor();
   if (!ShadowAndSave(&R))
   {
      ShowCursor();
      return;
   }


   PenColor(9);
   PaintRect(&R);
   PenColor(0);
   FrameRect(&R);
   PenColor(MENUTEXT);
   BackColor(9);
   TextAlign(alignCenter,alignTop);
   MoveTo(cx,R.Ymin + 2);
   DrawString(s);
   MoveTo(cx,R.Ymin + FontHeight + 2);
   DrawString(msg);
   ShowCursor();
   while(1)
   {
      event e;

      KeyEvent(true,&e);

      if ((e.State & 0x700) || e.ASCII || e.ScanCode)
         break;
   }
   HideCursor();
   PopRect(&n);
   ShowCursor();
}

int Overwrite(char *f)
{
	struct stat statbuf;
	int i;
	char tbuf[128];
	char name[30];
	char ext[20];

	i = stat(f, &statbuf);

	if (i)
		return 1;	/* ok to write, file doesn't exist */

   /* It might be read only, in which case we stop it here */
   if (statbuf.st_mode & S_IFDIR)
   {
      sprintf(tbuf,"Error: %s is a directory.",f);
      ErrorBox(tbuf);
      return 0;
   }

   if (!(statbuf.st_mode & S_IWRITE))
   {
      sprintf(tbuf,"Error: %s is read-only.",f);
      ErrorBox(tbuf);
      return 0;
   }



	fnsplit(f, NULL, NULL, name, ext);
	sprintf(tbuf, "Overwrite %s%s?", name, ext);
	return cancel_ok_msg(tbuf);
}


void DrawString(char *s)
{
   static bitmap *theBitmap;
   static fontRcd *theFont;
   static char *fbase;
   static bitmap *pixBitmap;
   static int firsttime=1;
   static int myheight;
   static int halfheight;
   static int mymc;
   static int offsets[256];

   short X,Y;
   int i;
   rect R2;
   static rect R1;

   if (firsttime)
   {
      theBitmap = thePort->portBMap;
      theFont = thePort->txFont;
      fbase = (char *)theFont;
#pragma warn -sig
      pixBitmap = (bitmap *)(fbase + theFont->grafMapTbl);
#pragma warn .sig
      firsttime = 0;
      myheight = theFont->chHeight;
      halfheight = myheight/2;
      R1.Ymin = 0;
      R1.Ymax = myheight - 1;
      mymc = theFont->minChar;
      for(i=0;i<256;i++)
         offsets[i] = (i-mymc) * 8; /* of course this fills some extras */



   }

   X = thePort->pnLoc.X;
   Y = thePort->pnLoc.Y;
   i = thePort->txAlign.X;
   if (i == alignRight)
      X -= strlen(s) << 3;
   else if (i == alignCenter)
      X -= strlen(s) << 2;

   i = thePort->txAlign.Y;

   if (i == alignMiddle)
      Y -= halfheight;
   else if (i == alignBottom)
      Y -= myheight - 1;
   else if (i == alignBaseline)
      Y -= myheight - theFont->descent;


   R2.Ymin = Y;
   R2.Ymax = R2.Ymin + myheight - 1;
   R2.Xmin = X;
   R2.Xmax = X + 7;

   R1.Ymin = 0;
   R1.Ymax = myheight - 1;
   for(i=0;*s;i++,s++,R2.Xmin+=8,R2.Xmax+=8)
   {
      R1.Xmin = offsets[*s];
      R1.Xmax = R1.Xmin + 7;

      CopyBits(pixBitmap,theBitmap,&R1,&R2,&thePort->portClip,0);
   }
   MoveTo(thePort->pnLoc.X+i*8,thePort->pnLoc.Y);

}


static rect MouseRectStack[20];
static int MRptr = 0;
static rect CurrentLimits;

void LimitMouseRect(rect * R)
{
	CurrentLimits = *R;
	LimitMouse(R->Xmin, R->Ymin, R->Xmax, R->Ymax);
}

void PushMouseRect(void)
{
	if (MRptr == 0)
		CurrentLimits = sR;
	if (MRptr < 19)
		MouseRectStack[MRptr++] = CurrentLimits;
}

void PushMouseRectLimit(rect * R)
{
	PushMouseRect();
	LimitMouseRect(R);
}

void PopMouseRect(void)
{
	if (MRptr > 0)
		LimitMouseRect(&MouseRectStack[--MRptr]);
}


double round(double n)
{
	return floor(n * 1000.0 + .5) / 1000.0;
}

void DisplayString13(char *t,int col,int row)
{
   int i;
   int c = col;
   extern void DisplayChar13(char,int,int,int,int);
   for(i=0;t[i];i++)
      DisplayChar13(t[i],(c++*8),row*8,255,0);
}

void PushCursorType(void){}
void PopCursorType(void){}


unsigned long realfarcoreleft(void)
{
   unsigned long l1 = farcoreleft();
   struct farheapinfo hi;

   hi.ptr = NULL;

   while(farheapwalk(&hi) == _HEAPOK)
   {
      if (!hi.in_use)
         l1 += hi.size;
   }
   return l1;
}

void FileError(char *t,FILE *fd)
{
   char tbuf[128];
   if (fd)
      sprintf(tbuf,"%s: Disk full",t);
   else
   {
      sprintf(tbuf,"%s: %s",t,strerror(errno));
      tbuf[strlen(tbuf)-1] = 0;
   }
   ErrorBox(tbuf);
}

void ProcessKey(int X,int Y,int *ci,int *key,rect *bR[],int items,int n,event *e)
{
   int i;
   int button = (e->State & 0x700) >> 8;
   *ci = -1;
   *key = 0;
   for(i=0;i<items;i++)
   {
      if (XYInRect(X,Y,bR[i]))
      {
         *ci = i;
         break;
      }
   }
   if (n)
   {
      if (e->ASCII && e->ASCII != 0xe0)
         *key = e->ASCII;
      else if (e->ScanCode && e->ScanCode != 0xff)
         *key = e->ScanCode << 8;

      if (button == swRight)
         *key = 0x1b;
      if (button == swLeft)
         *key = 0x0d;
   }
}




