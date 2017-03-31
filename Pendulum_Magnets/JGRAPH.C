#include "mag.h"

static int lastcolor = -1;
static int lastmode = -1;
extern int maxx, maxy, maxcolor, mode, minx;
extern double radius;
extern rect displayRect;
extern int mode;
unsigned char egacolortable[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};
unsigned char vgacolortable[16][3] = {
   00,00,00, 
   00,00,42, 
   00,42,00,
   00,42,42,
   42,00,00,
   42,00,42,
   42,21,00,
   42,42,42,
   21,21,21,
   21,21,63,
   21,63,21,
   21,63,63,
   63,21,21,
   63,21,63,
   63,63,21,
   63,63,63,
   };
rect sR;
int FontHeight;
int FirstLine;
int StringWidthX;
int grayflag = 0;
int spinflag = 0;

void JPenColor(int color)
{
	if (color != lastcolor)
		PenColor(lastcolor = color);
}
int hasVGA;

int detectmode(void)
{
	int n = QueryGrafix();
	int n2 = n & 0x300;


	if (n == -1)
		return n;

	/* We want either 0x12 for a VGA or 0x10 for an EGA. */
	hasVGA = (n2 & 0x100);

	if (n2 == 0x200)
		return 0x10;
	if (n2 == 0x300)
		return 0x12;

	return n;
}





static void pix(int i, int j, unsigned char color, int mode)
{
	ClipRect(&displayRect);
	RasterOp(lastmode = mode);
	PenColor(lastcolor = color);
	SetPixel(i, j);
	ClipRect(&sR);
}


void pixel(int i, int j, unsigned char color)
{
	pix(i, j, color, zREPz);
}

void xpixel(int i, int j, unsigned char color)
{
	pix(i, j, color, zXORz);
}

void bres(int x1, int y1, int x2, int y2, unsigned char color, int width, int mode)
{
	ClipRect(&displayRect);
	RasterOp(lastmode = mode);
	PenColor(lastcolor = color);
	PenSize(width, width);

	MoveTo(x1, y1);
	LineTo(x2, y2);
	ClipRect(&sR);
}

void wbresen(int x1, int y1, int x2, int y2, unsigned char color, int width)
{
	bres(x1, y1, x2, y2, color, width, zREPz);
}

void xwbresen(int x1, int y1, int x2, int y2, unsigned char color, int width)
{
	bres(x1, y1, x2, y2, color, width, zXORz);
}

void bresen(int x1, int y1, int x2, int y2, unsigned char color)
{
	bres(x1, y1, x2, y2, color, 1, zREPz);
}

void xbresen(int x1, int y1, int x2, int y2, unsigned char color)
{
  	bres(x1, y1, x2, y2, color, 1, zXORz);
}
rect DisplayR;

void installmode(void)
{
	rect R;

	GetPort(&thePort);
	sR = thePort->portRect;
	LimitMouseRect(&sR);
	ScaleMouse(24, 16);

	FontHeight = thePort->txFont->leading;
	ScreenRect(&sR);
	StringWidthX = StringWidth("X");
	RasterOp(zREPz);
	FirstLine = FontHeight;

	maxx = (3 * thePort->portRect.Xmax) / 4;
	maxy = thePort->portRect.Ymax;
	minx = (thePort->portRect.Xmax + 1) / 4;
	maxcolor = 15;

	BackColor(BLACK);
	SetRect(&DisplayR, minx, 0, minx + maxx, maxy);

	ClipRect(&sR);

	/*
	 * There seems to be a flaw in clipping, so I'm erasing actually one
	 * to the left.
	 */

	R = DisplayR;
	R.Xmin--;

	EraseRect(&R);
/*	ClipRect(&DisplayR); */
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

unsigned char defaultpalette[64][3] = {
	0x00, 0x00, 0x00,
	0x00, 0x00, 0x2a,
	0x00, 0x2a, 0x00,
	0x00, 0x2a, 0x2a,
	0x2a, 0x00, 0x00,
	0x2a, 0x00, 0x2a,
	0x2a, 0x2a, 0x00,
	0x2a, 0x2a, 0x2a,
	0x00, 0x00, 0x15,
	0x00, 0x00, 0x3f,
	0x00, 0x2a, 0x15,
	0x00, 0x2a, 0x3f,
	0x2a, 0x00, 0x15,
	0x2a, 0x00, 0x3f,
	0x2a, 0x2a, 0x15,
	0x2a, 0x2a, 0x3f,
	0x00, 0x15, 0x00,
	0x00, 0x15, 0x2a,
	0x00, 0x3f, 0x00,
	0x00, 0x3f, 0x2a,
	0x2a, 0x15, 0x00,
	0x2a, 0x15, 0x2a,
	0x2a, 0x3f, 0x00,
	0x2a, 0x3f, 0x2a,
	0x00, 0x15, 0x15,
	0x00, 0x15, 0x3f,
	0x00, 0x3f, 0x15,
	0x00, 0x3f, 0x3f,
	0x2a, 0x15, 0x15,
	0x2a, 0x15, 0x3f,
	0x2a, 0x3f, 0x15,
	0x2a, 0x3f, 0x3f,
	0x15, 0x00, 0x00,
	0x15, 0x00, 0x2a,
	0x15, 0x2a, 0x00,
	0x15, 0x2a, 0x2a,
	0x3f, 0x00, 0x00,
	0x3f, 0x00, 0x2a,
	0x3f, 0x2a, 0x00,
	0x3f, 0x2a, 0x2a,
	0x15, 0x00, 0x15,
	0x15, 0x00, 0x3f,
	0x15, 0x2a, 0x15,
	0x15, 0x2a, 0x3f,
	0x3f, 0x00, 0x15,
	0x3f, 0x00, 0x3f,
	0x3f, 0x2a, 0x15,
	0x3f, 0x2a, 0x3f,
	0x15, 0x15, 0x00,
	0x15, 0x15, 0x2a,
	0x15, 0x3f, 0x00,
	0x15, 0x3f, 0x2a,
	0x3f, 0x15, 0x00,
	0x3f, 0x15, 0x2a,
	0x3f, 0x3f, 0x00,
	0x3f, 0x3f, 0x2a,
	0x15, 0x15, 0x15,
	0x15, 0x15, 0x3f,
	0x15, 0x3f, 0x15,
	0x15, 0x3f, 0x3f,
	0x3f, 0x15, 0x15,
	0x3f, 0x15, 0x3f,
	0x3f, 0x3f, 0x15,
	0x3f, 0x3f, 0x3f,
};

static int palettenumber = 0;

void changeVGApalette(void)
{
	/* Well hello hello. Blast some nice palette into the mess. */
	int i;


	memset(vgacolortable, 0, sizeof vgacolortable);
   palettenumber = (palettenumber + 1) % 9;

	switch (palettenumber)
	{
   case 0:
      setdefaultpalette();
      break;
	case 1:
		/* Ramp the values in 16 steps of gray. */
		for (i = 1; i < 16; i++)
			vgacolortable[i][0] = vgacolortable[i][1] = vgacolortable[i][2] = 16 + i * 3 - 1;
		break;
	case 2:
		/* Reverse of the previous */
		for (i = 1; i < 16; i++)
			vgacolortable[i][0] = vgacolortable[i][1] = vgacolortable[i][2] = 66 - i * 3;
		break;

	case 3:
		/* Red fades into green */
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i][0] = i * 4 - 1;
			vgacolortable[i][1] = 64 - vgacolortable[i][0];
		}
		break;

	case 4:
		/* Three grays */
		vgacolortable[1][0] = vgacolortable[1][1] = vgacolortable[1][2] = 21;
		vgacolortable[2][0] = vgacolortable[2][1] = vgacolortable[2][2] = 42;
		vgacolortable[3][0] = vgacolortable[3][1] = vgacolortable[3][2] = 63;

		/* Three reds */
		vgacolortable[4][0] = 21;
		vgacolortable[5][0] = 42;
		vgacolortable[6][0] = 63;

		/* Three greens */
		vgacolortable[7][1] = 21;
		vgacolortable[8][1] = 42;
		vgacolortable[9][1] = 63;

		/* Three blues */
		vgacolortable[10][2] = 21;
		vgacolortable[11][2] = 42;
		vgacolortable[12][2] = 63;

		/* Three yellows */
		vgacolortable[13][0] = vgacolortable[13][1] = 21;
		vgacolortable[14][0] = vgacolortable[14][1] = 42;
		vgacolortable[15][0] = vgacolortable[15][1] = 63;

		break;

	case 5:
		/* Rudy likes this one */
		memcpy(vgacolortable, hodgepal, sizeof vgacolortable);
		break;
	case 6:
		/*
		 * Even and odd get different. Even: red to purple; odd:
		 * purple to blue.
		 */
		even_odd_gradient(vgacolortable, 0, 2);
		break;
	case 7:
		even_odd_gradient(vgacolortable, 1, 2);
		break;
	case 8:
		even_odd_gradient(vgacolortable, 1, 0);
		break;
	}

	usepalette();

}

unsigned char EGApal[] = {
	0,
	0x01, 0x08, 0x39,	/* blues */
	0x02, 0x10, 0x3a,	/* greens */
	0x04, 0x20, 0x3c,	/* reds */
	0x06, 0x30, 0x3e,	/* yellow */
	0x38, 0x07, 0x3f,	/* darkgray white hiwhite */
};



void changepalette(void)
{
	/*
	 * Randomly load the palette with 16 of the 64 possible colors,
	 * making sure each one is different. Hm. How do we draw from a deck?
	 * Ah, I know.
	 */


   if (hasVGA)
      changeVGApalette();
   else
   {
	   unsigned char colors[64];
	   int limit;

	   int i;


	   /* Color 0 is always black, I think. Yes? No? */
	   for (i = 0; i < 64; i++)
		   colors[i] = i;

	   limit = 64;
	   egacolortable[0] = egacolortable[16] = 0;
	   switch (random(10))
	   {
	   case 0:
		   /* Primaries palette */
		   memcpy(egacolortable, EGApal, 16);
		   break;
	   case 1:
		   /* Reverse primary palette */
		   for (i = 1; i < 16; i++)
			   egacolortable[i] = EGApal[16 - i];
		   break;
	   default:
		   for (i = 1; i < 16; i++)
		   {
			   int n = random(limit);

			   egacolortable[i] = n;
			   colors[n] = colors[limit - 1];
			   limit--;
		   }
	   }

      usepalette();
   }
}

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
	_useEGApalette((unsigned char *) egacolortable);
}


void _useVGApalette(unsigned char *p)
{
	union REGS regs;
	struct SREGS sregs;

   /* wait for retrace... */
   while(inportb(0x3da) & 8);
   while(!(inportb(0x3da) & 8));

	regs.x.dx = FP_OFF(p);
	sregs.es = FP_SEG(p);
	regs.x.bx = 0;
	regs.x.cx = 256;
	regs.x.ax = 0x1012;
	int86x(0x10, &regs, &regs, &sregs);
}

double zoomfactor = 1.0;
void DoSomeNastyShit(double factor)
{

	rect R;
	point P;
	int width;
	int height;
	metaPort *thePort;

	GetPort(&thePort);
	R = thePort->portRect;
	width = R.Xmax - R.Xmin + 1;
	height = R.Ymax - R.Ymin + 1;

	P.X = R.Xmin + width / 2;
	P.Y = R.Ymin + height / 2;
	zoomfactor *= factor;

	if (zoomfactor > 16)
		zoomfactor = 16;
	if (zoomfactor < 1 / 16.0)
		zoomfactor = 1 / 16.0;
	CenterRect(&P, zoomfactor * width, zoomfactor * height, &R);
	startconfig();
	VirtualRect(&R);
}
void jpixel(int x, int y, int color)
{
	if (x >= 0)
		pixel(x + minx, y, color);
}

void jxpixel(int x, int y, int color)
{
	if (x >= 0)
		xpixel(x + minx, y, color);
}

void jxbresen(int x1, int y1, int x2, int y2, int color)
{
	xbresen(x1 + minx, y1, x2 + minx, y2, color);
}

void jbresen(int x1, int y1, int x2, int y2, int color)
{
	bresen(x1 + minx, y1, x2 + minx, y2, color);
}

void jxwbresen(int x1, int y1, int x2, int y2, unsigned char color, int width)
{
	xwbresen(x1 + minx, y1, x2 + minx, y2, color, width);
}

void grayscale(void)
{
   union REGS regs;

   regs.h.al = grayflag ? 0 : 1;
   regs.h.ah = 0x12;
   regs.h.bl = 0x33;
   int86(0x10,&regs,&regs);
}

#define minimum_luminosity 30
int available_colors[] = {2,3,5,6,9,10,11,12,13,14};
int locked = true;

void randompalette(void)
{
   int i;
   int limit = (locked) ? 10 : 15;

   if (mode == 0x12)
   {
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
   					vgacolortable[available_colors[i]][0] = (r * 63) / 99;
   					vgacolortable[available_colors[i]][1] = (g * 63) / 99;
   					vgacolortable[available_colors[i]][2] = (b * 63) / 99;
               }
               else
               {
   					vgacolortable[i+1][0] = (r * 63) / 99;
   					vgacolortable[i+1][1] = (g * 63) / 99;
   					vgacolortable[i+1][2] = (b * 63) / 99;
               }

					break;
				}
			}
      }
   }
   else
   {
      /* Pick, at random, 15 numbers out of the 64. What fun. */
      /* fill the 64 first */
      char testbed[64];
      int n = 64;

      for(i=0;i<64;i++)
         testbed[i] = i;

      for(i=1;i<limit;i++)
      {
         int t = random(n);
         egacolortable[i] = testbed[t];
         /* probably could memcpy(testbed+t,testbed+t+1,n-t) */
         for(;t<n;t++)
            testbed[t] = testbed[t+1];
         n--;
      }
   }
   usepalette();
}

int upspinvals[16] = {
	0, 1,
   3,5,
   4,
   6,9,
   7,8,
   10,11,12,13,14,2,
   15
};

int downspinvals[16] = {
   0,1,
   14,2,
   4,
   3,5,
   7,8,
   6,9,10,11,12,13,
   15
};


int bigupspinvals[16] = {
   0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,1
   };
int bigdownspinvals[16] = {
   0,15,1,2,3,4,5,6,7,8,9,10,11,12,13,14
   };

void _spinpalette(char *pal)
{
	char tmp[16];
	int i;
	memcpy(tmp, pal, 16);
	for (i = 0; i < 16; i++)
		pal[i] = tmp[locked ? upspinvals[i] : bigupspinvals[i]];
	_useEGApalette((unsigned char *)pal);
}


void spinpalette(void)
{
   _spinpalette((char *)egacolortable);
}

void spinGIFpalette(int mode)
{
   int locker = locked;
   locked = false;
   if (mode != 0x13)
      _spinpalette((char *)GIFctab);
   else
   {
      unsigned char poop[3];
      int i,j;
      for(j=0;j<3;j++)
         poop[j] = GIFcmap[255*3+j];

      for(i=254;i>=1;i--)
         for(j=0;j<3;j++)
            GIFcmap[(i+1)*3+j] = GIFcmap[i*3+j];

      for(j=0;j<3;j++)
         GIFcmap[3+j] = poop[j];
      _useVGApalette(GIFcmap);
   }
   locked = locker;
}

void _revspinpalette(char *p)
{
	char tmp[16];
	int i;

	memcpy(tmp, p, 16);
	for (i = 0; i < 16; i++)
		p[i] = tmp[locked ? downspinvals[i] : bigdownspinvals[i]];
	_useEGApalette((unsigned char *)p);
}


void revspinpalette(void)
{
   _revspinpalette((char *)egacolortable);
}

void revspinGIFpalette(int mode)
{
   int locker = locked;
   locked = false;
   if (mode != 0x13)
      _revspinpalette((char *)GIFctab);
   else
   {
      unsigned char poop[3];
      int i,j;
      for(j=0;j<3;j++)
         poop[j] = GIFcmap[3+j];

      for(i=1;i<255;i++)
         for(j=0;j<3;j++)
            GIFcmap[i*3+j] = GIFcmap[(i+1)*3+j];

      for(j=0;j<3;j++)
         GIFcmap[255*3+j] = poop[j];
      _useVGApalette(GIFcmap);
   }
   locked = locker;
}
void usepalette()
{
	useEGApalette();

	if (hasVGA)
   {
		_useVGApalette((unsigned char *)vgacolortable);

		if (grayflag)
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

void setdefaultpalette(void)
{
   memcpy(vgacolortable,startvgapalette,16*3);
   memcpy(egacolortable,startegapalette,16);
   usepalette();
}
