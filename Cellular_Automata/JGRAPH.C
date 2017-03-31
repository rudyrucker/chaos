#include "toy.h"

/* Josh's replacements for Rudy's method of doing stuff...
Since FLI lib code is already here, I use it. */

static int last_color = 0;
int maxcolor;
int mode;
extern char egacolortable[17];

extern int vgaflag;
extern int palettenumber;

extern void rsetmode(int);
extern void getmode(void);
int monoflag = 0;

void setdefaultpalette(void);


/*------------------Graphics Mode and Color Functions----*/

void installmode()
{

   mapArray bm = {0,0,0,0,0,0,0,0};

	SetDisplay(GrafPg0);
	mode = 0x10;
	maxcolor = 15;		/* modeinfo[mode].maxcolor */
	last_color = 0;

	/* Clear the screen */
	PenColor(BLACK);
	PaintRect(&sR);
   GetPort(&thePort);
   aspect = (double)thePort->portBMap->pixResX/
            (double)thePort->portBMap->pixResY;

   CursorMap(bm);
	sR = thePort->portRect;
	FontHeight = thePort->txFont->leading;
	StringWidthX = StringWidth("X");
   TrackCursor(true);
	LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);
   ScaleMouse(24,16);
	EventQueue(true);

}

void restoretextmode()
{
	SetDisplay(TextPg0);
	/* Clear the screen */
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

unsigned char defaultpallette[16][3];

unsigned char defHODGEpal[16*3];
unsigned char defHODGEct[16];
unsigned char defTUBEpal[16*3];
unsigned char defTUBEct[16];
unsigned char defEATpal[16*3];
unsigned char defEATct[16];
unsigned char defNLUKYpal[16*3];
unsigned char defNLUKYct[16];

void setdefaultpalette(void)
{
   unsigned char *p;
   unsigned char *c;

   switch(caotype)
   {
   case CA_HODGE:
      p = defHODGEpal;
      c = defHODGEct;
      break;
   case CA_EAT:
      p = defEATpal;
      c = defEATct;
      break;
   case CA_TUBE:
      p = defTUBEpal;
      c = defTUBEct;
      break;
   case CA_NLUKY:
      p = defNLUKYpal;
      c = defNLUKYct;
      break;
   }
   memcpy(vgacolortable,p,16*3);
   memcpy(egacolortable,c,16);

   usepalette();
}


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
      setdefaultpalette();
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

#define minimum_luminosity 10
int available_colors[] = {2,3,5,6,9,10,11,12,13,14};


void randompalette(void)
{
	int i;

	if (hasVGA)
	{

		for (i = 0; i < 16; i++)
			egacolortable[i] = i;

		/* it's a VGA! */
		for (i = 1; i < 16; i++)
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
#ifdef LIMIT_COLORS
               vgacolortable[available_colors[i]][0] = (r * 63) / 99;
					vgacolortable[available_colors[i]][1] = (g * 63) / 99;
					vgacolortable[available_colors[i]][2] = (b * 63) / 99;
#else
					vgacolortable[i][0] = (r * 63) / 99;
					vgacolortable[i][1] = (g * 63) / 99;
					vgacolortable[i][2] = (b * 63) / 99;
#endif
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
#ifdef OLDWAY
         /*
			 * make sure that we never use the equivalent of
			 * 0,1,4,7,8,15 here
			 */
			static char zeep[] = {0, 1, 7, 8, 14, 15};
			int k;

			for (k = 0; k < 6; k++)
			{
				if (i == egacolortable[zeep[k]])
					break;
			}
			if (k == 6)
#endif

				testbed[j++] = i;
		}

		n = j;

		for (i = 1; i < 16; i++)
		{
			int t = random(n);

#ifdef OLDWAY
         egacolortable[available_colors[i]] = testbed[t];
#else
         egacolortable[i] = testbed[t];
#endif
			/* probably could memcpy(testbed+t,testbed+t+1,n-t) */
			for (; t < n; t++)
				testbed[t] = testbed[t + 1];
			n--;
		}
	}
	usepalette();
}



void changepalette()
{				/* this calls loadVGApalette so we have only
				 * one palette key */
	/*
	 * Randomly load the pallette with 16 of the 64 possible colors,
	 * making sure each one is different. Hm. How do we draw from a deck?
	 * Ah, I know.   This is EGA case.
	 */

	unsigned char colors[64];
	unsigned char used_colors[17];
	int limit;
	union REGS regs;
	struct SREGS sregs;

	int i;

	if (vgaflag)
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

#ifdef OLDWAY
/*----------------------------------------------------------*/
/* This DragRect comes from Island Software, mostly, but
this one works... */

void far DragRect(rect far * R, short RefX, short RefY)
{				/* DragRect */
	short dx, dy, mx, my, but;
	unsigned char done;
	event e;
	int button;
	int MouseX, MouseY;

	button = 0;

	/* Turn off event queueing */
	RasterOp(zXORz);


	HideCursor();
	PenColor(WHITE);
	do
	{
		/* set done if left pressed */


		/* poll mouse position & mouse buttons */
		KeyEvent(false, &e);
		MouseX = e.CursorX;
		MouseY = e.CursorY;
		button = e.State >> 8;

		done = (button == swLeft);

		if (!done)
		{
			dx = MouseX - RefX;
			dy = MouseY - RefY;

			if (R->Xmax > sR.Xmax)
				dx = sR.Xmax - R->Xmax;	/* dx<0 */
			if (R->Ymax > sR.Ymax)
				dy = sR.Ymax - R->Ymax;	/* dy<0 */
			if (160 > R->Xmin)
				dx = 160 - R->Xmin;	/* dx>0 */
			if (0 > R->Ymin)
				dy = -R->Ymin;	/* dy>0 */

			R->Xmin = R->Xmin + dx;
			R->Xmax = R->Xmax + dx;
			R->Ymin = R->Ymin + dy;
			R->Ymax = R->Ymax + dy;


			FrameRect(R);
			/* wait for mouse to move or button to release */
			do
			{
				KeyEvent(false, &e);
				mx = e.CursorX;
				my = e.CursorY;
				but = e.State >> 8;
			}
			while ((mx == MouseX) && (my == MouseY) && (but != 0));


			/* erase old marker by drawing over it */
			FrameRect(R);

			PenColor(WHITE);


			RefX = MouseX;
			RefY = MouseY;
		}		/* if not done */


	}
	while (!done);

	RasterOp(zREPz);
	ShowCursor();

	R->Xmin = R->Xmin + dx;
	R->Xmax = R->Xmax + dx;
	R->Ymin = R->Ymin + dy;
	R->Ymax = R->Ymax + dy;


}				/* DragRect */
#endif

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
	_useEGApalette((unsigned char *)egacolortable);
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
   unsigned char *p;
   unsigned char *c;
	useEGApalette();

	if (hasVGA)
   {
		_useVGApalette((unsigned char *)vgacolortable);

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

   /* and lock the current palette into place? */

   switch(caotype)
   {
   case CA_HODGE:
      p = HODGE_colortable;
      c = HODGE_ct;
      break;
   case CA_EAT:
      p = EAT_colortable;
      c = EAT_ct;
      break;
   case CA_TUBE:
      p = TUBE_colortable;
      c = TUBE_ct;
      break;
   case CA_NLUKY:
      p = NLUKY_colortable;
      c = NLUKY_ct;
      break;
   }
   memcpy(p,vgacolortable,16*3);
   memcpy(c,egacolortable,16);


}

#ifdef LIMIT_COLORS
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
#else
int upspinvals[16] = {
   0,
   2,3,4,5,6,7,8,9,10,11,12,13,14,15,1
   };

int downspinvals[16] = {
   0,
   15,1,2,3,4,5,6,7,8,9,10,11,12,13,14
   };
#endif

void spinpalette(void)
{
	char tmp[16];
	int i;
	memcpy(tmp, egacolortable, 16);
	for (i = 0; i < 16; i++)
		egacolortable[i] = tmp[upspinvals[i]];
	useEGApalette();
}

void revspinpalette(void)
{
	char tmp[16];
	int i;

	memcpy(tmp, egacolortable, 16);
	for (i = 0; i < 16; i++)
		egacolortable[i] = tmp[downspinvals[i]];
	useEGApalette();
}
