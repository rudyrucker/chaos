#include <dos.h>
#include "attract.h"
/*------------------Graphics Mode and Color Functions----*/

int hasVGA;
void _useEGApalette(char *p)
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


void _useVGApalette(char *p)
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


char zombies[16][3] = {
	/*
	 * Yellow to blue, but let's leave LIGHTGREEN and CYAN for the 1 and
	 * 2, and YELLOW and BROWN for 14 and 15
	 */
	0, 0, 0,
	0, 63, 0,
	10, 10, 63,

	63, 63, 0,		/* we need 11 steps */
	57, 57, 7,
	51, 51, 13,
	45, 45, 20,
	39, 39, 27,
	33, 33, 33,
	27, 27, 39,
	20, 20, 45,
	13, 13, 51,
	7, 7, 57,
	0, 0, 63,

	63, 63, 0,
	42, 21, 0
};





void zombietones(char *clut)
{

	memcpy(clut, zombies, 3 * 16);
}


void fourtones(char *clut)
{
	int i;
	static int _4tones[] =
	{
		30, 41, 52, 63
	};

	for (i = 0; i < 3; i++)
		clut[0 * 3 + i] = 0;

	for (i = 0; i < 4; i++)
	{
		clut[1 + i * 3 + 0] = _4tones[i];
		clut[1 + i * 3 + 1] = 0;
		clut[1 + i * 3 + 2] = 0;
	}

	for (i = 0; i < 4; i++)
	{
		clut[5 + i * 3 + 1] = _4tones[i];
		clut[5 + i * 3 + 0] = 0;
		clut[5 + i * 3 + 2] = 0;
	}

	for (i = 0; i < 4; i++)
	{
		clut[9 + i * 3 + 2] = _4tones[i];
		clut[9 + i * 3 + 1] = 0;
		clut[9 + i * 3 + 0] = 0;
	}

	for (i = 0; i < 3; i++)
	{
		clut[13 + i * 3 + 1] = clut[13 + i * 3 + 0] = clut[13 + i * 3 + 2] = _4tones[i + 1];
	}
}


void puretones1(char *clut)
{
	int i;

	for (i = 0; i < 3; i++)
		clut[0 * 3 + i] = 0;

	for (i = 0; i < 3; i++)
	{
		clut[1 + i * 3 + 0] = 31 + i * 16;
		clut[1 + i * 3 + 1] = 0;
		clut[1 + i * 3 + 2] = 0;
	}

	for (i = 0; i < 3; i++)
	{
		clut[4 + i * 3 + 1] = 31 + i * 16;
		clut[4 + i * 3 + 0] = 0;
		clut[4 + i * 3 + 2] = 0;
	}

	for (i = 0; i < 3; i++)
	{
		clut[7 + i * 3 + 2] = 31 + i * 16;
		clut[7 + i * 3 + 1] = 0;
		clut[7 + i * 3 + 0] = 0;
	}

	for (i = 0; i < 3; i++)
	{
		clut[10 + i * 3 + 1] =
			clut[10 + i * 3 + 0] = 31 + i * 16;
		clut[10 + i * 3 + 2] = 0;
	}

	for (i = 0; i < 3; i++)
	{
		clut[13 + i * 3 + 2] =
			clut[13 + i * 3 + 0] = 31 + i * 16;
		clut[13 + i * 3 + 1] = 0;
	}
}

static char brightclut[16][3] = {
	0, 0, 0,
	0, 0, 42,
	0, 48, 0,
	48, 48, 0,
	42, 0, 0,
	48, 0, 48,
	0, 48, 48,
	42, 42, 42,
	21, 21, 21,
	63, 63, 0,
	0, 0, 63,
	63, 0, 63,
	0, 63, 63,
	63, 48, 0,
	48, 48, 48,
	63, 63, 63
};

static char inverseclut[16][3] = {
	0, 0, 0,
	0x00, 0x00, 0x2a,
	63, 0, 0,
	0, 63, 63,
	0x2a, 0x00, 0x00,
	63, 0, 63,
	0, 0, 63,
	42, 42, 42,

	21, 21, 21,
	31, 63, 63,
	31, 63, 31,
	63, 31, 63,
	31, 31, 63,
	63, 63, 31,
	31, 31, 31,
	63, 63, 63
};











void puretones2(char *clut)
{
	memcpy(clut, brightclut, 3 * 16);
}

void puretones3(char *clut)
{
	memcpy(clut, inverseclut, 3 * 16);
}



extern char vgacolortable[64 * 3];
char ct[17];

int bigupspinvals[16] = {
	0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1
};
int bigdownspinvals[16] = {
	0, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
};

int upspinvals[16] = {
	0, 1,
	3, 5,
	4,
	6, 9,
	7, 8,
	10, 11, 12, 13, 14, 2,
	15
};

int downspinvals[16] = {
	0, 1,
	14, 2,
	4,
	3, 5,
	7, 8,
	6, 9, 10, 11, 12, 13,
	15
};

void Jspinpalette(void)
{
	char tmp[16];
	int i;

	memcpy(tmp, egacolortable, 16);
	for (i = 0; i < 16; i++)
		egacolortable[i] = tmp[locked ? upspinvals[i] : bigupspinvals[i]];
	useEGApalette();
}

void revJspinpalette(void)
{
	char tmp[16];
	int i;

	memcpy(tmp, egacolortable, 16);
	for (i = 0; i < 16; i++)
		egacolortable[i] = tmp[locked ? downspinvals[i] : bigdownspinvals[i]];
	useEGApalette();
}



void Jsetct(char *ct)
{
	union REGS regs;
	struct SREGS sregs;

	regs.h.ah = 0x10;
	regs.h.al = 0x02;
	regs.x.dx = FP_OFF(ct);
	sregs.es = FP_SEG(ct);
	int86x(0x10, &regs, &regs, &sregs);
}


void Jsetpal(char *clut, char *ct)
{
	union REGS regs;
	struct SREGS sregs;

	/* Now tell the world of our palette */
	regs.h.ah = 0x10;
	regs.h.al = 0x12;
	regs.x.bx = 0;
	regs.x.cx = 16;
	regs.x.dx = FP_OFF(clut);
	sregs.es = FP_SEG(clut);
	int86x(0x10, &regs, &regs, &sregs);

	Jsetct(ct);
}

void Jusepalette(void)
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

#define STANDARDBLUE 0x00,0x00,0x2a
#define STANDARDRED  0x2a,0x00,0x00
#define STANDARDLG   0x2a,0x2a,0x2a
#define STANDARDDG   0x15,0x15,0x15
#define STANDARDWHITE 0x3f,0x3f,0x3f

void presetpalette(void)
{
	/* Well hello hello. Blast some nice palette into the mess. */
	int i;

/* put rudy's palettes in here, too */
	static int rudypal1[16 * 3] = {0, 0, 0,
		STANDARDBLUE,
		63, 0, 0,
		63, 0, 20,
		STANDARDRED,
		63, 0, 30,
		63, 0, 50,
		STANDARDLG,
		STANDARDDG,
		50, 0, 63,
		20, 0, 63,
		10, 0, 63,
		63, 45, 0,
		63, 52, 0,
		63, 55, 0,
		STANDARDWHITE
	};

	static int rudypal2[16 * 3] = {
		0, 0, 0,
		STANDARDBLUE,
		63, 0, 10,
		63, 0, 20,
		STANDARDRED,
		63, 0, 40,
		0, 63, 0,
		STANDARDLG,
		STANDARDDG,
		0, 63, 30,
		0, 63, 40,
		0, 63, 0,
		63, 63, 10,
		63, 63, 20,
		63, 63, 30,
		STANDARDWHITE
	};

	static int rudypal3[16 * 3] = {
		0, 0, 0,
		STANDARDBLUE,
		0, 42, 42,
		21, 22, 42,
		STANDARDRED,
		42, 42, 63,
		42, 63, 63,
		STANDARDLG,
		STANDARDDG,
		63, 42, 42,
		63, 42, 21,
		63, 21, 21,
		42, 21, 21,
		21, 21, 21,
		0, 0, 42,
		STANDARDWHITE
	};

	static int rudypal4[16 * 3] = {
		0, 0, 0,
		STANDARDBLUE,
		0, 21, 42,
		0, 32, 42,
		STANDARDRED,
		30, 42, 42,
		37, 43, 34,
		STANDARDLG,
		STANDARDDG,
		63, 57, 20,
		63, 46, 20,
		63, 40, 18,
		62, 31, 12,
		63, 21, 3,
		63, 13, 0,
		STANDARDWHITE
	};


	for (i = 0; i < 16; i++)
		ct[i] = i;
	ct[i] = 0;

	palettenumber++;
	palettenumber %= 13;

	switch (palettenumber)
	{
	case 0:
		memcpy(vgacolortable, defaultcolortable, 16 * 3);
		break;
	case 1:
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i * 3 + 0] = rudypal1[3 * i];
			vgacolortable[i * 3 + 1] = rudypal1[3 * i + 1];
			vgacolortable[i * 3 + 2] = rudypal1[3 * i + 2];
		}
		break;
	case 2:
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i * 3 + 0] = rudypal2[3 * i];
			vgacolortable[i * 3 + 1] = rudypal2[3 * i + 1];
			vgacolortable[i * 3 + 2] = rudypal2[3 * i + 2];
		}
		break;
	case 3:
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i * 3 + 0] = rudypal3[3 * i];
			vgacolortable[i * 3 + 1] = rudypal3[3 * i + 1];
			vgacolortable[i * 3 + 2] = rudypal3[3 * i + 2];
		}
		break;
	case 4:
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i * 3 + 0] = rudypal4[3 * i];
			vgacolortable[i * 3 + 1] = rudypal4[3 * i + 1];
			vgacolortable[i * 3 + 2] = rudypal4[3 * i + 2];
		}
		break;
	case 5:
		fourtones(vgacolortable);
		break;

	case 6:
		/* Ramp the values in 16 steps of gray. */
		for (i = 1; i < 16; i++)
			vgacolortable[i * 3 + 0] = vgacolortable[i * 3 + 1] = vgacolortable[i * 3 + 2] = 16 + i * 3 - 1;
		break;

	case 7:
		/* Red fades into green */
		for (i = 1; i < 16; i++)
		{
			vgacolortable[i * 3 + 0] = i * 4 - 1;
			vgacolortable[i * 3 + 1] = 64 - vgacolortable[i * 3 + 0];
		}
		break;

	case 8:
		/* Three grays */
		vgacolortable[1 * 3 + 0] = vgacolortable[1 * 3 + 1] = vgacolortable[1 * 3 + 2] = 21;
		vgacolortable[2 * 3 + 0] = vgacolortable[2 * 3 + 1] = vgacolortable[2 * 3 + 2] = 42;
		vgacolortable[3 * 3 + 0] = vgacolortable[3 * 3 + 1] = vgacolortable[3 * 3 + 2] = 63;

		/* Three reds */
		vgacolortable[4 * 3 + 0] = 21;
		vgacolortable[5 * 3 + 0] = 42;
		vgacolortable[6 * 3 + 0] = 63;

		/* Three greens */
		vgacolortable[7 * 3 + 1] = 21;
		vgacolortable[8 * 3 + 1] = 42;
		vgacolortable[9 * 3 + 1] = 63;

		/* Three blues */
		vgacolortable[10 * 3 + 2] = 21;
		vgacolortable[11 * 3 + 2] = 42;
		vgacolortable[12 * 3 + 2] = 63;

		/* Three yellows */
		vgacolortable[13 * 3 + 0] = vgacolortable[13 * 3 + 1] = 21;
		vgacolortable[14 * 3 + 0] = vgacolortable[14 * 3 + 1] = 42;
		vgacolortable[15 * 3 + 0] = vgacolortable[15 * 3 + 1] = 63;

		break;

	case 9:
		puretones1(vgacolortable);
		break;

	case 10:
		puretones3(vgacolortable);
		break;

	case 11:
		puretones2(vgacolortable);
		break;

	case 12:
		zombietones(vgacolortable);
		break;
	}

	/* Now for all of these, force the colors we like. */
	for (i = 0; i < 3; i++)
	{
		vgacolortable[i] = defaultpalette[0][i];
		vgacolortable[3 * 1 + i] = defaultpalette[1][i];
		vgacolortable[3 * 4 + i] = defaultpalette[4][i];
		vgacolortable[3 * 7 + i] = defaultpalette[7][i];
		vgacolortable[3 * 8 + i] = defaultpalette[56][i];
		vgacolortable[3 * 15 + i] = defaultpalette[63][i];
	}



	Jsetpal((char *) vgacolortable, (char *) ct);
}

#define minimum_luminosity 20


void randompalette(void)
{
	int i;
	int limit = (locked ? 10 : 15);

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

			for (k = 0; locked && k < 6; k++)
			{
				if (i == egacolortable[zeep[k]])
					break;
			}
			if (k == 6 || !locked)
				testbed[j++] = i;
		}

		n = j;

		for (i = 0; i < limit; i++)
		{
			int t = random(n);

			egacolortable[locked ? available_colors[i] : i] = testbed[t];
			/* probably could memcpy(testbed+t,testbed+t+1,n-t) */
			for (; t < n; t++)
				testbed[t] = testbed[t + 1];
			n--;
		}
	}
	Jusepalette();
}

void dodefaultpalette(void)
{
	extern unsigned char startvgapalette[16 * 3];
	extern unsigned char startegapalette[16];

	memcpy(egacolortable, startegapalette, 16);
	if (mode == 0x12)
		memcpy(vgacolortable, startvgapalette, 16 * 3);

	Jusepalette();
}

rect current_cursor;

void _drawcursor(rect *RR)
{
   /* Doing it by hand, see if it is horribly slow. Probably is. */
   rect R = *RR;
   int i;
   RasterOp(zREPz);
   for(i=0;i<8;i++)
   {
      PenColor(WHITE);
      MoveTo(R.Xmin,R.Ymin+i);
      LineTo((!cursorshape) ? R.Xmax : (R.Xmax - i),R.Ymin + i);
      if (cursorshape)
      {
         PenColor(BLACK);
         LineTo(R.Xmax,R.Ymin+i);
      }
   }
}


static image *cursorpatch=NULL;
void erasecursor(void)
{
   if (!cursoron)
      return;
   WriteImage(&current_cursor,cursorpatch);
   cursoron = false;

}   

void drawcursor(void)
{
   rect R;
   if (cursoron)
      erasecursor();
   
   R.Xmin = curx;
   R.Xmax = curx + 7;
   R.Ymin = cury;
   R.Ymax = cury + 7;
   current_cursor = R;
   if (!cursorpatch)
   {
      int n = (int) ImageSize(&R);
      cursorpatch = (image *)malloc(n);
   }
   ReadImage(&R,cursorpatch);
   _drawcursor(&current_cursor);
   cursoron = true;
}




 


