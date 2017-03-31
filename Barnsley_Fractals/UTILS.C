#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include <dir.h>
#include <sys\stat.h>
#include <alloc.h>
#include <io.h>

#include "game.h"

char raster_op;

#pragma argsused
void PaintRoundRect(rect * R, short n1, short n2)
{
	/*
	 * Since all of our round rects are always the same rounding, I
	 * wanted something much faster. How nice for me.
	 */
	int y1 = R->Ymin;
	int y2 = R->Ymax;
	int x1 = R->Xmin;
	int x2 = R->Xmax;

#pragma warn -sig
	int CurrentColor = thePort->pnColor;

#pragma warn .sig
	raster_op = thePort->pnMode;

	/* First line is cut off by 3 on each end */
	HLine(x1 + 3, y1++, x2 - 3, CurrentColor);
	/* Next line is cut off by 1 */
	HLine(x1 + 2, y1++, x2 - 2, CurrentColor);
	HLine(x1 + 1, y1++, x2 - 1, CurrentColor);

	for (; y1 < y2 - 2; y1++)
		HLine(x1, y1, x2, CurrentColor);

	HLine(x1 + 1, y1++, x2 - 1, CurrentColor);
	HLine(x1 + 3, y1++, x2 - 3, CurrentColor);
	FrameRoundRect(R, 8, 8);
}

void DrawString(char *s)
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
	for (i = 0; *s; i++, s++, R2.Xmin += 8, R2.Xmax += 8)
	{
		R1.Xmin = offsets[*s];
		R1.Xmax = R1.Xmin + 7;
		CopyBits(pixBitmap, theBitmap, &R1, &R2, &thePort->portClip, 0);
	}
	MoveTo(thePort->pnLoc.X + i * 8, thePort->pnLoc.Y);

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

void SplitRectV(rect * tR, rect * d1, rect * d2)
{
	int midpoint = tR->Xmin + (tR->Xmax - tR->Xmin) / 2;

	d1->Ymin = d2->Ymin = tR->Ymin;
	d1->Ymax = d2->Ymax = tR->Ymax;

	d1->Xmin = tR->Xmin;
	d1->Xmax = midpoint;
	d2->Xmin = midpoint + 1;
	d2->Xmax = tR->Xmax;
}

void Centers(rect * R, int *centerx, int *centery)
{
	*centerx = R->Xmin + (R->Xmax - R->Xmin) / 2;
	*centery = R->Ymin + (R->Ymax - R->Ymin) / 2;
}

void move_to_corner(rect * R)
{
	int cx, cy;

   /* Make sure this shit is valid. */
	Centers(R, &cx, &cy);

	MoveCursor(curx = cx, cury = cy);

}

void erasecursor(void)
{
	if (cursoron)
		HideCursor();
	cursoron = false;
	/* not always balanced, so... */

}

void drawcursor(void)
{
	short cx, cy, cl, cb;

	if (cursoron)
		erasecursor();

	MoveCursor(curx, cury);

	while (1)
	{
		ShowCursor();
		QueryCursor(&cx, &cy, &cl, &cb);
		if (cl >= 0)
			break;
	}

	cursoron = true;
}

void ForceCursorOn(void)
{
	short cx, cy, cl, cb;

	while (1)
	{
		ShowCursor();
		QueryCursor(&cx, &cy, &cl, &cb);
		if (cl >= 0)
			break;
	}
}

pair pixel_from_barnimage(barnmap * h, pair * z)
{
	pair t = barnimage(h, z);

	return pixel(&t);
}
pair fpixel_from_barnimage(fbarnmap * h, fpair * z)
{
	fpair t = fbarnimage(h, z);

	return fpixel(&t);
}

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
	if (cursorsp)
	{
		cursorsp--;
		curx = cursorstack[cursorsp].x;
		cury = cursorstack[cursorsp].y;
		MoveCursor(curx, cury);

	}
}

void RangeError(char *msg)
{
	rect R;
	int cx, cy;
	int width = 0;
	int i;
	int height = 3 * FontHeight + 4;

	char tbuf[3][128];


	Centers(&sR, &cx, &cy);


	strcpy(tbuf[0], "Range Error! Acceptable values are");
	strcpy(tbuf[1], msg);
	strcpy(tbuf[2], "Press any key or click to continue");

	for (i = 0; i < 3; i++)
		width = max(width, StringWidth(tbuf[i]) + 4);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height;

	HideCursor();

	if (!ShadowAndSave(&R))
	{
		ShowCursor();
		return;
	}

	PenColor(RED);
	PaintRect(&R);
	PenColor(WHITE);
	FrameRect(&R);

	PenColor(WHITE);
	BackColor(RED);
	TextAlign(alignCenter, alignTop);

	for (i = 0; i < 3; i++)
	{
		MoveTo(cx, R.Ymin + 2 + FontHeight * i);
		DrawString(tbuf[i]);
	}
	ShowCursor();
	while (1)
	{
		event e;

		KeyEvent(true, &e);

		if ((e.State & 0x700) || e.ASCII || e.ScanCode)
			break;
	}

	HideCursor();
	PopRect(&i);
	ShowCursor();
}

void ErrorBox(char *s)
{
	int width = StringWidth(s) + 10;
	int height = 2 * FontHeight + 4;
	rect R;
	int cx, cy;
	char *msg = "Press any key or click to continue.";
	int n;

	width = max(width, StringWidth(msg));
	Centers(&sR, &cx, &cy);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;

   HideCursor();
	if (!ShadowAndSave(&R))
   {
      ShowCursor();
		return;
   }

	PenColor(RED);
	PaintRect(&R);
	PenColor(0);
	FrameRect(&R);
	PenColor(MENUTEXT);
	BackColor(RED);
	TextAlign(alignCenter, alignTop);
	MoveTo(cx, R.Ymin + 2);
	DrawString(s);
	MoveTo(cx, R.Ymin + FontHeight + 2);
	DrawString(msg);

	while (1)
	{
		event e;

		KeyEvent(true, &e);

		if ((e.State & 0x700) || e.ASCII || e.ScanCode)
			break;
	}

   WaitForNothing();

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
		return 1;       /* ok to write, file doesn't exist */

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

void WaitForNothing(void)
{

//   event e;
//
//	while (1)
//	{
//		KeyEvent(false, &e);
//
//		if ((e.State & 0x700) == 0)
//			break;
//	}
   short cx,cy,cl,cb;
   do
   {
      QueryCursor(&cx,&cy,&cl,&cb);
   } while(cb);


}


int ShadowAndSave(rect * tR)
{

	rect shR = *tR;
	rect uR;
	rect r1, r2, r3;
	int err;
	int i;

	OffsetRect(&shR, 6, 6);
	r3 = shR;
	ShiftRect(&r3, -6, -6, &r1, &r2);


	UnionRect(&shR, tR, &uR);

	PushRect(&uR, &err);
	if (err)
		return 0;

	RasterOp(zXORz);
	PenColor(8);
	PaintRect(&r1);
	PaintRect(&r2);
	/* and get the corners hee hee? */
	for (i = 1; i < 6; i++)
	{
		MoveTo(tR->Xmin + i, tR->Ymax);
		LineTo(tR->Xmin + i, tR->Ymax + i);
		MoveTo(tR->Xmax, tR->Ymin + i);
		LineTo(tR->Xmax + i, tR->Ymin + i);
	}
	MoveTo(tR->Xmax, tR->Ymax);
	LineTo(uR.Xmax, uR.Ymax);
	RasterOp(zREPz);
	return 1;
}

unsigned char egacolortable[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
unsigned char vgacolortable[64 * 3] = {
   00, 00, 00,
   00, 00, 42,
   00, 42, 00,
   00, 42, 42,
   42, 00, 00,
   42, 00, 42,
   42, 21, 00,
   42, 42, 42,
   21, 21, 21,
   21, 21, 63,
   21, 63, 21,
   21, 63, 63,
   63, 21, 21,
   63, 21, 63,
   63, 63, 21,
   63, 63, 63,
   };


void _useEGApalette(unsigned char *p)
{
	union REGS regs;
	struct SREGS sregs;

	regs.x.dx = FP_OFF(p);
	sregs.es = FP_SEG(p);
	regs.x.ax = 0x1002;
	int86x(0x10, &regs, &regs, &sregs);
}


void useEGApalette(void)
{
	_useEGApalette(egacolortable);
}


void _useVGApalette(unsigned char *p)
{
	union REGS regs;
	struct SREGS sregs;

	regs.x.dx = FP_OFF(p);
	sregs.es = FP_SEG(p);
	regs.x.bx = 0;
	regs.x.cx = 16;
	regs.x.ax = 0x1012;
	int86x(0x10, &regs, &regs, &sregs);
}

void usepalette(void)
{
	useEGApalette();

	if (mode == 0x12)
		_useVGApalette(vgacolortable);

	if (hasVGA)
	{
		if (monoflag)
		{
			union REGS regs;

			regs.h.ah = 0x10;
			regs.h.al = 0x1b;
			regs.x.bx = 0;
			regs.x.cx = 16;
			int86(0x10, &regs, &regs);
		}
	}

}



int upspin[16] = {
	0, 1,
	3, 4, 5, 6, 9,
	7, 8,
	10, 11, 12, 13, 2,
	14, 15
};

int downspin[16] = {
	0, 1,
	13, 2, 3, 4, 5,
	7, 8,
	6, 9, 10, 11, 12,
	14, 15
};
static int bigupspin[16] = {
   0,
   2,3,4,5,6,7,8,9,10,11,12,13,14,15,1
   };
static int bigdownspin[16] = {
   0,
   15,1,2,3,4,5,6,7,8,9,10,11,12,13,14
   };


void spinpalette(void)
{
	char spinners[16];
	int i;

	memcpy(spinners, egacolortable, 16);
	for (i = 0; i < 16; i++)
     egacolortable[i] = spinners[locked ? upspin[i] : bigupspin[i]];
	useEGApalette();
}

void revspinpalette(void)
{
	char spinners[16];
	int i;

	memcpy(spinners, egacolortable, 16);
	for (i = 0; i < 16; i++)
     egacolortable[i] = spinners[locked ? downspin[i] : bigdownspin[i]];
	useEGApalette();
}

void dodefaultpalette(void)
{
	memcpy(egacolortable, startegapalette, 16);
	if (mode == 0x12)
		memcpy(vgacolortable, startvgapalette, 16 * 3);

	usepalette();
}



void changepalette(void)
{
	randompalette();
}

#define minimum_luminosity 10

void randompalette(void)
{
	int i;
   int limit = (locked) ? 10 : 15;

	if (mode == 0x12)
	{

		for (i = 0; i < 16; i++)
			egacolortable[i] = i;

		/* it's a VGA! */
		for (i = 0; i < limit; i++)
		{

			while (1)
			{
				int r, g, b;
				double luminosity;

				r = random(100);
				g = random(100);
				b = random(100);


				luminosity = 0.3 * r + 0.59 * g + 0.11 * b;
				if (luminosity > minimum_luminosity)
				{
	       if (locked)
	       {
					   vgacolortable[available_colors[i] * 3] = (r * 63) / 99;
					   vgacolortable[available_colors[i] * 3 + 1] = (g * 63) / 99;
					   vgacolortable[available_colors[i] * 3 + 2] = (b * 63) / 99;
	       }
	       else
	       {
					   vgacolortable[i * 3 + 3] = (r * 63) / 99;
					   vgacolortable[i * 3 + 4] = (g * 63) / 99;
					   vgacolortable[i * 3 + 5] = (b * 63) / 99;
	       }

					break;
				}
			}
		}
	}
	else
	{
		char testbed[64];
		int n = 64;
		int j;


		for (i = j = 0; i < 64; i++)
		{
			/*
			 * make sure that we never use the equivalent of
			 * 0,1,7,8,14,15 here
			 */
			static char zeep[] = {0, 1, 7, 8, 14, 15};
			int k;

			for (k = 0; !locked && k < 6; k++)
			{
				if (i == egacolortable[zeep[k]])
					break;
			}
			if (k == 6 || locked)
				testbed[j++] = i;
		}

		n = j;

		for (i = 0; i < limit; i++)
		{
			int t = random(n);

	 if (locked)
   			egacolortable[available_colors[i]] = testbed[t];
	 else
   			egacolortable[i+1] = testbed[t];

			/* probably could memcpy(testbed+t,testbed+t+1,n-t) */
			for (; t < n; t++)
				testbed[t] = testbed[t + 1];
			n--;
		}
	}
	usepalette();
}


void even_odd_gradient(unsigned char clut[16][3], int n, int m)
{
	int i;

	for (i = 1; i < 16; i += 2)
	{
		clut[i][n] = 63;
		clut[i][m] = (63 * (i - 1)) / 14;
	}
	for (i = 2; i < 16; i += 2)
	{
		clut[i][n] = ((14 - i) * 63) / 12;
		clut[i][m] = 63;
	}
}

unsigned char hodgepal[16][3] = {
	0, 0, 0,
	63, 3, 0,
	63, 9, 0,
	63, 15, 0,
	63, 21, 0,
	63, 27, 0,
	0, 63, 0,
	0, 43, 20,
	0, 23, 40,
	63, 0, 63,
	57, 0, 63,
	51, 0, 63,
	45, 0, 63,
	39, 0, 63,
	0, 32, 0,
	0, 16, 0,
};

void changeVGApalette(void)
{
	/* Well hello hello. Blast some nice palette into the mess. */
	unsigned char clut[16][3];
	unsigned char ct[17];
	int i;

	memset(clut, 0, sizeof clut);
	for (i = 0; i < 16; i++)
		ct[i] = i;
	ct[i] = 0;
	palettenumber = (palettenumber + 1) % 9;

	switch (palettenumber)
	{
   case 0:
      dodefaultpalette();
      return;
	case 1:
		/* Ramp the values in 16 steps of gray. */
		for (i = 1; i < 16; i++)
			clut[i][0] = clut[i][1] = clut[i][2] = 16 + i * 3 - 1;
		break;
	case 2:
		/* Reverse of the previous */
		for (i = 1; i < 16; i++)
			clut[i][0] = clut[i][1] = clut[i][2] = 66 - i * 3;
		break;

	case 3:
		/* Red fades into green */
		for (i = 1; i < 16; i++)
		{
			clut[i][0] = i * 4 - 1;
			clut[i][1] = 64 - clut[i][0];
		}
		break;

	case 4:
		/* Three grays */
		clut[1][0] = clut[1][1] = clut[1][2] = 21;
		clut[2][0] = clut[2][1] = clut[2][2] = 42;
		clut[3][0] = clut[3][1] = clut[3][2] = 63;

		/* Three reds */
		clut[4][0] = 21;
		clut[5][0] = 42;
		clut[6][0] = 63;

		/* Three greens */
		clut[7][1] = 21;
		clut[8][1] = 42;
		clut[9][1] = 63;

		/* Three blues */
		clut[10][2] = 21;
		clut[11][2] = 42;
		clut[12][2] = 63;

		/* Three yellows */
		clut[13][0] = clut[13][1] = 21;
		clut[14][0] = clut[14][1] = 42;
		clut[15][0] = clut[15][1] = 63;

		break;

	case 5:
		/* Rudy likes this one */
		memcpy(clut, hodgepal, sizeof clut);
		break;
	case 6:
		/*
		 * Even and odd get different. Even: red to purple; odd:
		 * purple to blue.
		 */
		even_odd_gradient(clut, 0, 2);
		break;
	case 7:
		even_odd_gradient(clut, 1, 2);
		break;
	case 8:
		even_odd_gradient(clut, 1, 0);
		break;

	}

   memcpy(vgacolortable,clut,sizeof clut);
   memcpy(egacolortable,ct,sizeof ct);

   usepalette();
}


unsigned char pal1[] = {
	0,
	0x01, 0x08, 0x39,	/* blues */
	0x02, 0x10, 0x3a,	/* greens */
	0x04, 0x20, 0x3c,	/* reds */
	0x06, 0x30, 0x3e,	/* yellow */
	0x38, 0x07, 0x3f,	/* darkgray white hiwhite */
};

void presetpalette()
{

	unsigned char colors[64];
	unsigned char used_colors[17];
	int limit;
	union REGS regs;
	struct SREGS sregs;

	int i;

	if (hasVGA)
	{
		changeVGApalette();
		return;
	}


	/* Color 0 is always black, I think. Yes? No? */
	for (i = 0; i < 64; i++)
		colors[i] = i;

	limit = 64;
	used_colors[0] = used_colors[16] = 0;
	switch (random(10))
	{
	case 0:
		/* Primaries pallette */
		memcpy(used_colors, pal1, 16);
		break;
	case 1:
		/* Reverse primary palette */
		for (i = 1; i < 16; i++)
			used_colors[i] = pal1[16 - i];
		break;
	default:
		for (i = 1; i < 16; i++)
		{
			int n = random(limit);

			used_colors[i] = n;
			memcpy(&colors[n], &colors[n + 1], limit - n - 1);
			limit--;
		}
	}

	/* OK, now set the palette */
	regs.h.ah = 0x10;
	regs.h.al = 0x02;
	regs.x.dx = FP_OFF(used_colors);
	sregs.es = FP_SEG(used_colors);
	int86x(0x10, &regs, &regs, &sregs);
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
	PaintRadioButton(&okRect, false, false, "Yes");
	ExtraHilite(&okRect, false);

	cancelRect.Xmax = R.Xmax - 4;
	cancelRect.Xmin = centerx + 2;
	cancelRect.Ymax = R.Ymax - 4;
	cancelRect.Ymin = cancelRect.Ymax - FontHeight - 4;
	PaintRadioButton(&cancelRect, false, false, "No");
	move_to_corner(&cancelRect);
	PushButton(&cancelRect, true);
	current_item = 1;
	PushCursorType();
	ArrowCursor();
	ProtectOff();
	ShowCursor();

	while (1)
	{
		event e;
		int key = 0;
		int button;
		int X, Y;
		int n;
		int last_item = current_item;



		n = KeyEvent(false, &e);
		X = e.CursorX;
		Y = e.CursorY;

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
				if (XYInRect(X, Y, &okRect))
					retval = 1;
				else if (XYInRect(X, Y, &cancelRect))
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
		else
		{
			if (XYInRect(X, Y, &okRect))
				current_item = 0;
			else if (XYInRect(X, Y, &cancelRect))
				current_item = 1;
			else
				current_item = -1;
		}

		if (current_item != last_item)
		{
			switch (last_item)
			{
			case 0:
				PushButton(&okRect, false);
				ExtraHilite(&okRect, false);
				break;
			case 1:
				PushButton(&cancelRect, false);
				break;
			}
			switch (current_item)
			{
			case 0:
				PushButton(&okRect, true);
				ExtraHilite(&okRect, true);
				break;
			case 1:
				PushButton(&cancelRect, true);
				break;
			}
		}



	}

	/* Depress the appropriate button */
	if (retval == 1)
	{
		PaintRadioButton(&okRect, true, true, "Yes");
		ExtraHilite(&okRect, true);
	}
	else
		PaintRadioButton(&cancelRect, true, true, "No");


	/* Wait for the key to be lifted */
	WaitForNothing();

	if (retval == 1)
	{
		PaintRadioButton(&okRect, false, false, "Yes");
		ExtraHilite(&okRect, false);
	}
	else
		PaintRadioButton(&cancelRect, false, false, "No");

	HideCursor();
	PopRect(&err);
	LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);
	PopCursorType();

	ShowCursor();

	return retval;
}

void JString(char *text, int x, int y, int fg, int bg, int a1, int a2)
{
	TextAlign(a1, a2);
	MoveTo(x, y);
	PenColor(fg);
	BackColor(bg);
	DrawString(text);
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
   else
      LimitMouseRect(&sR);
}

unsigned long realfarcoreleft(void)
{
   unsigned long l1 = farcoreleft();
   struct farheapinfo hi;

   hi.ptr = NULL;

   while(farheapwalk(&hi) == _HEAPOK)
   {
      if (hi.in_use)
	 l1 += hi.size;
   }
   return l1;
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

FILE *OpenWithError(char *name,char *mode)
{
   FILE *n = fopen(name,mode);

   if (!n)
      FileError(name,NULL);

   return n;
}


