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
#include "toy.h"
#include <io.h>

static struct
{
	int x, y;
} cursorstack[20];
static int cursorsp = 0;

void PushCursorPosition(void)
{
	short cx, cy, cl, cb;

	QueryCursor(&cx, &cy, &cl, &cb);
	cursorstack[cursorsp].x = cx;
	cursorstack[cursorsp].y = cy;

	if (cursorsp < 20)
		cursorsp++;

}

void PopCursorPosition(void)
{
	int x, y;

	if (cursorsp)
	{
		cursorsp--;
		x = cursorstack[cursorsp].x;
		y = cursorstack[cursorsp].y;
		MoveCursor(x, y);

	}
}

void move_to_corner(rect * R)
{
	int cx, cy;

	Centers(R, &cx, &cy);
	MoveCursor(cx, cy);
}

void WaitForNothing(void)
{
	event e;

	while (1)
	{
		KeyEvent(false, &e);
		if (!(e.State & 0x700))
			return;
	}
}



int SomethingWaiting(void)
{
	int n;


	event e;

	n = KeyEvent(false, &e);

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


#ifdef DRAWSTRING

void DrawString(char *ss)
{
	static bitmap *theBitmap;
	static fontRcd *theFont;
	static char *fbase;
	static bitmap *pixBitmap;
	static int firsttime = 1;
	static int myheight;
	static int halfheight;
	static int mymc;
	static int offsets[256];
	unsigned char *s = (unsigned char *) ss;

	short X, Y;
	int i;
	rect R2;
	static rect R1;

	if (firsttime)
	{
		theBitmap = thePort->portBMap;
		theFont = thePort->txFont;
		fbase = (char *) theFont;
#pragma warn -sig
		pixBitmap = (bitmap *) (fbase + theFont->grafMapTbl);
#pragma warn .sig
		firsttime = 0;
		myheight = theFont->chHeight;
		halfheight = myheight / 2;
		R1.Ymin = 0;
		R1.Ymax = myheight - 1;
		mymc = theFont->minChar;
		for (i = 0; i < 256; i++)
			offsets[i] = (i - mymc) * 8;	/* of course this fills
							 * some extras */
	}

	X = thePort->pnLoc.X;
	Y = thePort->pnLoc.Y;
	i = thePort->txAlign.X;
	if (i == alignRight)
		X -= strlen(ss) << 3;
	else if (i == alignCenter)
		X -= strlen(ss) << 2;

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
	for (i = 0; *s; i++, s++, R2.Xmin += 8, R2.Xmax += 8)
	{
		R1.Xmin = offsets[*s];
		R1.Xmax = R1.Xmin + 7;
		CopyBits(pixBitmap, theBitmap, &R1, &R2, &thePort->portClip, 0);
	}
	MoveTo(thePort->pnLoc.X + i * 8, thePort->pnLoc.Y);

}
#endif

void Centers(rect * R, int *cx, int *cy)
{
	*cx = R->Xmin + (R->Xmax - R->Xmin) / 2;
	*cy = R->Ymin + (R->Ymax - R->Ymin) / 2;
}


void JString(char *text, int x, int y, int fg, int bg, int a1, int a2)
{
	TextAlign(a1, a2);
	MoveTo(x, y);
	PenColor(fg);
	BackColor(bg);
	DrawString(text);
}

#ifdef NEEDED
typedef enum
{
	ARROW, BOX
} Curtype;
static int cursortypesp = 0;
static Curtype cursortypestack[20];
#endif

void PushCursorType(void)
{
#ifdef NEEDED
	Curtype n;

	if (cursoron)
		n = BOX;
	else
		n = ARROW;

	if (cursortypesp < 19)
		cursortypestack[cursortypesp++] = n;
#endif
}
void PopCursorType(void)
{
#ifdef NEEDED
	if (cursortypesp)
	{
		Curtype n = cursortypestack[--cursortypesp];

		if (n == BOX)
		{
			HideCursor();
			drawcursor();
		}
		else
		{
			erasecursor();
			ShowCursor();
		}
	}
#endif
}

void ArrowCursor(void)
{
#ifdef NEEDED
	erasecursor();
	ShowCursor();
#endif
}

/* WARNING BULLSHIT */
extern char egacolortable[17];
double aspect = 1.0;

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
	else
		LimitMouseRect(&sR);
}

int ShiftArrows(event * e)
{
	int retval = 0;
	int scancode = e->ScanCode << 8;

	if ((scancode == XLARROW ||
	     scancode == XDARROW ||
	     scancode == XUARROW ||
	     scancode == XRARROW ||
	     scancode == XPGUP ||
	     scancode == XPGDN ||
	     scancode == XEND ||
	     scancode == XHOME))
	{
		if (e->State & 3)
			retval = scancode + 1;
	}

	else if (scancode == XCENTER5)
		retval = XCENTER5;

	return retval;
}
void GrayOut(rect * R)
{
	RasterOp(zORz);
	BackColor(BLACK);
	PenColor(LIGHTGRAY);
	PenPattern(3);
	PaintRect(R);
	PenPattern(1);
	RasterOp(zREPz);
}

void TempFileName(char *buf, char *name)
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

	sprintf(buf, "%s%s%s", pathname, sep, name);
}
