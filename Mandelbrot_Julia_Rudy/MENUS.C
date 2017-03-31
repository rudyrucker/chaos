/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal code of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.							     *
*									     *
******************************************************************************/




#include<stdio.h>;
#include<stdlib.h>;
#include<conio.h>;
#include<math.h>
#include <dos.h>
#include <dir.h>
#include "scodes.h"
#include <alloc.h>
#include <time.h>
#include <ctype.h>
#include <sys\stat.h>
#include <string.h>
#include <mem.h>
#include <time.h>

#include "mand.h"

/* Buttons and things */

char *typename_t[] = {
	"Julia",
	"Cubic Julia",
	"Mandelbrot",
	"Cubic Mandelbrot",
	"Rudy"
};
char *fillname_t[] = {
	"Blank",
	"Bullseyes",
	"Feathers"
};



button HelpButton = {XF1};
button FilesButton = {XF2};
button TypeButton = {XF3};
button ParamsButton = {XF4};
button ViewButton = {XF5};
button QuitButton = {XALTX};

button *buttonlist[] = {
	&HelpButton,
	&FilesButton,
	&TypeButton,
	&ParamsButton,
	&ViewButton,
   &QuitButton,
	NULL
};


void CheckButtons(int X,int Y)
{
   int i;
   current_main_item = -1;

   /* See if we want to hilite any buttons? */
   for(i=0;i<6;i++)
   {
      if (XYInRect(X,Y,&buttonlist[i]->R))
      {
	 current_main_item = i;
	 return;
      }
   }
   for(i=0;i<4;i++)
   {
      if (XYInRect(X,Y,&stamprects[i]))
	 current_main_item = 6 + (3-i);
   }
}
#pragma argsused
int ClosestButton(int X,int Y)
{
   /* Figure out which button is closest (Y-wise) to
      where we are. */

   int best;
   int distance = 10000;
   int cx,cy;
   int d;
   int i;

   for(i=0;i<6;i++)
   {
      Centers(&buttonlist[i]->R,&cx,&cy);
      d = abs(Y-cy);
      if (d < distance)
      {

	 distance = d;
	 best = i;
      }
   }

   for(i=0;i<4;i++)
   {
      Centers(&stamprects[i],&cx,&cy);
      d = abs(Y-cy);
      if (d < distance)
      {

	 distance = d;
	 best = 6 + 3 - i;
      }
   }

   return best;
}






void SplitRectH(rect * tR, rect * d1, rect * d2)
{
	int midpoint = tR->Ymin + (tR->Ymax - tR->Ymin) / 2;

	d1->Xmin = d2->Xmin = tR->Xmin;
	d1->Xmax = d2->Xmax = tR->Xmax;

	d1->Ymin = tR->Ymin;
	d1->Ymax = midpoint;
	d2->Ymin = midpoint + 1;
	d2->Ymax = tR->Ymax;
}


void make_button(button * b, int x, int y, char *text)
{
	int height = thePort->txFont->chHeight + 2;
   rect R;
   R.Xmin = x;
   R.Xmax = 76;
   R.Ymin = y;
   R.Ymax = y + height;
   b->R = R;

   PaintRadioButtonBase(&b->R,false,false,text,
      DARKGRAY,RED,WHITE);
   PushButton(&b->R,false);
}

static image *saved_button_image=NULL;
static rect mR;

void PaintQuitButton(int inout)
{
   rect R;
   int cx,cy;

   HideCursor();
   R.Xmin = 8;
   R.Xmax = 76;
   R.Ymin = ViewButton.R.Ymax + 6;
   R.Ymax = R.Ymin + 2*FontHeight + 4;
   QuitButton.R = R;
   PenColor(inout ? RED : DARKGRAY);
   PaintRect(&R);
   PenColor(WHITE);
   BackColor(inout ? RED : DARKGRAY);
   TextAlign(alignCenter,alignTop);
   Centers(&R,&cx,&cy);
   cx = R.Xmin + (R.Xmax-R.Xmin)/2;

   MoveTo(cx,R.Ymin+2);
   DrawString("Alt-X");
   MoveTo(cx,R.Ymin+2+FontHeight);
   DrawString("to Exit");
   PushButton(&R,inout);
   ExtraHilite(&R,inout);
   ShowCursor();
}
char *buttonmsgs[] = {
   "F1 HELP ",
   "F2 File ",
   "F3 Types",
   "F4 Tweak",
   "F5 Opts "
   };

void hilite_main_button(int n,int inout)
{
   if (n < 0)
      return;

   if (n < 5)
      PushButton(&buttonlist[n]->R,inout);
   else if (n == 5)
   {
      PushButton(&QuitButton.R,inout);
      ExtraHilite(&QuitButton.R,inout);
   }
}

void push_main_button(int n,int inout)
{
   button *b = buttonlist[n];

   PaintRadioButtonBase(&b->R,inout,inout,buttonmsgs[n],
      DARKGRAY,RED,WHITE);
   PushButton(&b->R,inout);
}

void main_corner(int n)
{
   if (n < 0)
      return;

   if (n < 6)
      move_to_corner(&buttonlist[n]->R);
   else
      move_to_corner(&stamprects[3 - (n-6)]);
}




void draw_buttons(void)
{
   rect R;
   int centerx,cy;

   R.Xmin = 0;
   R.Xmax = minx - 4;
   R.Ymin = 0;
   R.Ymax = maxy-1;
   PenColor(BLUE);
   PaintRect(&R);
   PenColor(WHITE);
   FrameRect(&R);

   if (saved_button_image == NULL)
   {
	   make_button(&HelpButton, 8, 4, "F1 HELP ");
	   make_button(&FilesButton, 8, HelpButton.R.Ymax + 6,  "F2 File ");
	   make_button(&TypeButton, 8, FilesButton.R.Ymax + 6,  "F3 Types");
	   make_button(&ParamsButton, 8, TypeButton.R.Ymax + 6, "F4 Tweak");
	   make_button(&ViewButton, 8, ParamsButton.R.Ymax + 6, "F5 Opts ");
      /* and the quit button is special */

      PaintQuitButton(false);

      Centers(&QuitButton.R,&centerx,&cy);

      mR.Xmin = 0;
      mR.Xmax = minx - 1;
      mR.Ymin = 0;
      mR.Ymax = QuitButton.R.Ymax + 4 + FontHeight;

      safe_alloc = 1;
      saved_button_image = (image *)farmalloc(ImageSize(&mR));
      if (saved_button_image != NULL) {
	      ReadImage(&mR,saved_button_image);
      }
   }
   else
   {
      RasterOp(zREPz);
      WriteImage(&mR,saved_button_image);
   }



}

int jgets(char *tbuf)
{
	int i = 0;

	while (1)
	{
		unsigned char c = getch();

		if (c == 0x0d)
			break;
		if (c == 0x1b)
			return 1;
		if (c == 8)
		{
			if (i > 0)
			{
				putchar(0x08);
				putchar(' ');
				putchar(0x08);
				tbuf[--i] = 0;
			}
		}
		else
			putchar(tbuf[i++] = c);
	}
	return 0;
}

void gifsave(int wholescreen)
{
	/* First, find a possible file name to dick with. */
	struct ffblk ffblk;
	int done;
	int number = -1;
	int mynum;
	char tbuf[128];

	if (!SpecifiedGifName)
	{
		done = findfirst("MAND??.GIF", &ffblk, 0);
		while (!done)
		{
			sscanf(ffblk.ff_name, "MAND%d.GIF", &mynum);
			if (mynum > number)
				number = mynum;
			done = findnext(&ffblk);
		}
		sprintf(tbuf, "MAND%02d.GIF", number + 1);
	}
	else
		strcpy(tbuf, SpecifiedGifName);
	GifOutput(tbuf, wholescreen);
}
#pragma argsused
void stampinfo(char *t, stampdata * d,int n)
{
	rect R;
   rect tR;
	char tbuf[128];
	int err;
   double fwidth;
   int centerx;

	R.Xmin = sR.Xmax / 4;
	R.Xmax = R.Xmin + sR.Xmax / 2;
	R.Ymin = sR.Ymax / 5;
	R.Ymax = R.Ymin + 8 * FontHeight;

   centerx = R.Xmin + (R.Xmax - R.Xmin)/2;

   /* Draw some xor lines to the box first! */

   HideCursor();
   tR = stamprects[n];
   PenColor(MENUTEXT);
   RasterOp(zXORz);
   MoveTo(tR.Xmin,tR.Ymin);
   LineTo(R.Xmin,R.Ymin);
   MoveTo(tR.Xmax,tR.Ymin);
   LineTo(R.Xmax,R.Ymin);
   MoveTo(tR.Xmin,tR.Ymax);
   LineTo(R.Xmin,R.Ymax);
   MoveTo(tR.Xmax,tR.Ymax);
   LineTo(R.Xmax,R.Ymax);
   RasterOp(zREPz);

   ShadowAndSave(&R);

	PenColor(MENUBACK);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	FrameRect(&R);


   /* Center the first one, make it a different color */
   TextAlign(alignCenter,alignTop);
   sprintf(tbuf,"%s parameters",vflagstonames(d));
   PenColor(MENUTEXT);
   BackColor(MENUBACK);
   MoveTo(centerx,R.Ymin + 1);
	DrawString(tbuf);

   fwidth = d->v.vhix - d->v.vlox;

   sprintf(tbuf,"Center: (%g %g)",
      d->v.vlox + fwidth/2.0,
      d->v.vloy + (d->v.vhiy - d->v.vloy)/2.0);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 1);
   TextAlign(alignLeft,alignTop);
   PenColor(BUTTONTEXT);
	DrawString(tbuf);

   sprintf(tbuf,"Width: %g",fwidth);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 2);
	DrawString(tbuf);

	sprintf(tbuf, "u=%g,v=%g", d->fu, d->fv);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 3);
	DrawString(tbuf);

	sprintf(tbuf, "a=%g,b=%g", d->fa, d->fb);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 4);
	DrawString(tbuf);

   sprintf(tbuf, "i=%d",d->iterations);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 5);
	DrawString(tbuf);



	MoveTo(centerx, R.Ymin + 1 + FontHeight * 7);
   TextAlign(alignCenter,alignTop);
   PenColor(MENUTEXT);
   DrawString("Click or press any key to continue");

   ShowCursor();
   while(1)
   {
      event e;

      int n = KeyEvent(false,&e);
      if (n && (e.ASCII || e.ScanCode || (e.State & 0x700)))
	 break;
   }
   HideCursor();

	PopRect(&err);

   PenColor(MENUTEXT);
   RasterOp(zXORz);
   MoveTo(tR.Xmin,tR.Ymin);
   LineTo(R.Xmin,R.Ymin);
   MoveTo(tR.Xmax,tR.Ymin);
   LineTo(R.Xmax,R.Ymin);
   MoveTo(tR.Xmin,tR.Ymax);
   LineTo(R.Xmin,R.Ymax);
   MoveTo(tR.Xmax,tR.Ymax);
   LineTo(R.Xmax,R.Ymax);
   RasterOp(zREPz);

   ShowCursor();


}

void infobox(char *t)
{
	rect R;
	char tbuf[128];
	int err;

	R.Xmin = sR.Xmax / 4;
	R.Xmax = R.Xmin + sR.Xmax / 2;
	R.Ymin = sR.Ymax / 4;
	R.Ymax = R.Ymin + 10 * FontHeight;

   HideCursor();
   ShadowAndSave(&R);

	PenColor(MENUBACK);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	FrameRect(&R);

	TextAlign(alignLeft, alignTop);
	PenColor(BUTTONTEXT);
	BackColor(MENUBACK);

	MoveTo(R.Xmin + 1, R.Ymin + 1);
	DrawString(t);

	sprintf(tbuf, "flox=%g", flox);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight);
	DrawString(tbuf);

	sprintf(tbuf, "fhix=%g", fhix);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 2);
	DrawString(tbuf);

	sprintf(tbuf, "floy=%g", floy);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 3);
	DrawString(tbuf);

	sprintf(tbuf, "fhiy=%g", fhiy);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 4);
	DrawString(tbuf);

	sprintf(tbuf, "mand:%d,cub:%d,ruck:%d,julia:%d,inside:%d",
		mandelflag, cubicflag, ruckerflag, juliaflag, insideflag);
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 5);
	DrawString(tbuf);

	sprintf(tbuf, "Type name is %s", flagstonames());
	MoveTo(R.Xmin + 1, R.Ymin + 1 + FontHeight * 6);
	DrawString(tbuf);


	getch();

	PopRect(&err);
   ShowCursor();
}





void MyTypetoFlags(void)
{
	switch (MyTypeFlag)
	{
		case 0:
		newjuliaflag = 1;
		juliaflag = 1;
		mandelflag = 1;
		cubicflag = 0;
		ruckerflag = 0;
		break;
	case 1:
		newjuliaflag = 1;
		juliaflag = 1;
		mandelflag = 0;
		cubicflag = 1;
		ruckerflag = 0;
		break;
	case 2:
		newjuliaflag = 0;
		juliaflag = 0;
		mandelflag = 1;
		cubicflag = 0;
		ruckerflag = 0;
		break;
	case 3:
		newjuliaflag = 0;
		juliaflag = 0;
		mandelflag = 0;
		cubicflag = 1;
		ruckerflag = 0;
		break;
	case 4:
		newjuliaflag = 0;
		juliaflag = 0;
		mandelflag = 0;
		cubicflag = 1;
		ruckerflag = 1;
	}
}


void namestoflags(char *typename)
{
	int i;


	for (i = 0; i < 5; i++)
		if (!strcmp(typename, typename_t[i]))
			break;

	MyTypeFlag = i;

	MyTypetoFlags();
}


int nametofill(char *fillname)
{
	int i;

	for (i = 0; i < 3; i++)
		if (!strcmp(fillname_t[i], fillname))
			return i;

   return -1;
}

char *vflagstonames(stampdata * d)
{
	int MyTypeFlag;

	if (d->juliaflag && d->mandelflag)
		MyTypeFlag = 0;
	else if (d->juliaflag && d->cubicflag)
		MyTypeFlag = 1;
	else if (d->mandelflag)
		MyTypeFlag = 2;
	else if (d->cubicflag && !d->ruckerflag)
		MyTypeFlag = 3;
	else
		MyTypeFlag = 4;

	return typename_t[MyTypeFlag];
}




char *flagstonames(void)
{
	if (juliaflag && mandelflag)
		MyTypeFlag = 0;
	else if (juliaflag && cubicflag)
		MyTypeFlag = 1;
	else if (mandelflag)
		MyTypeFlag = 2;
	else if (cubicflag && !ruckerflag)
		MyTypeFlag = 3;
	else
		MyTypeFlag = 4;

	return typename_t[MyTypeFlag];
}


char *slicename_t[] = {
	"uv-plane",
	"ab-plane",
	"au-plane",
	"bv-plane",
	"bu-plane",
	"av-plane",
	NULL
};

int slicenametoslicetype(char *slicename)
{
	int i;

	for (i = 0; slicename_t[i]; i++)
		if (!strcmp(slicename, slicename_t[i]))
			return i;

	return 0;
}

char *slicetypetoslicename(void)
{
	return slicename_t[slicetype];
}

void range_error(double bottom,double top,char *text)
{
   rect R;
   int err;

   int centerx = sR.Xmax/2;
   int centery = sR.Ymax/2;
   int height = 4 * FontHeight + 4;
   event e;
   char tbuf[128];

   R.Xmin = centerx - sR.Xmax/4;
   R.Xmax = centerx + sR.Xmax/4;

   R.Ymin = centery - height/2;
   R.Ymax = R.Ymin + height;

   HideCursor();
   ShadowAndSave(&R);

   PenColor(BUTTONBACK);
   PaintRect(&R);
   PenColor(MENUTEXT);
   FrameRect(&R);
   TextAlign(alignCenter,alignTop);

   MoveTo(centerx,R.Ymin + 2);
   PenColor(MENUTEXT);
   BackColor(BUTTONBACK);
   DrawString("Range Error!");
   sprintf(tbuf,"%s must be between",text);
   MoveTo(centerx,R.Ymin + 2 + FontHeight);
   DrawString(tbuf);
   MoveTo(centerx,R.Ymin + 2 + FontHeight*2);
   sprintf(tbuf,"%g and %g",bottom,top);
   DrawString(tbuf);
   MoveTo(centerx,R.Ymin + 2 + FontHeight*3);
   DrawString("Click or hit any key to continue");
   ShowCursor();
   KeyEvent(true,&e);
   HideCursor();
   PopRect(&err);
   ShowCursor();
}
