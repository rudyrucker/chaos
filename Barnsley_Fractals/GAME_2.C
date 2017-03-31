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



#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "game.h"

double zoomfactor = 2.0;

static int lower_limit = 20;

int available_colors[] = {
	2, 3, 5, 6, 9, 10, 11, 12, 13, 14
};


/*---------------------Zoom and Pan Functions---------*/
void _setfwindow(void)
{
	flox = (double) lox / BLOAT;
	fhix = (double) hix / BLOAT;
	floy = (double) loy / BLOAT;
	fhiy = (double) hiy / BLOAT;
	fdeltay = fhiy - floy;
	fdeltax = fhix - flox;
	fstepx = maxx / fdeltax;
	fstepy = maxy / fdeltay;
}

void setfp(void)
{
	int i;
	/* And fix the pixel coordinates of the B map's fixed points */
	for (i = 0; i < fBptr->n; i++)
	{
		Bptr->fixedpoint[i].x = minx +
			(fBptr->fixedpoint[i].x - flox) * fstepx;
		Bptr->fixedpoint[i].y = miny + (fhiy - fBptr->fixedpoint[i].y) * fstepy;
	}
}


void setfwindow(void)
{

	_setfwindow();
   setfp();
}

void setiwindow()
{				/* if you move from real to integer you might
				 * need this */
	lox = flox * BLOAT;
	hix = fhix * BLOAT;
	loy = floy * BLOAT;
	hiy = fhiy * BLOAT;
	deltay = hiy - loy;
	deltax = hix - lox;
}

void setstartwindow()
{

	flox = fstartlox;
	floy = fstartloy;
	fhix = fstarthix;
	fhiy = fstarthiy;
	fdeltax = fstartdeltax;
	fdeltay = fstartdeltay;
	fcenterx = fstartcenterx;
	fcentery = fstartcentery;

	lox = startlox;
	hix = starthix;
	loy = startloy;
	hiy = starthiy;
	deltax = startdeltax;
	deltay = startdeltay;
	centerx = startcenterx;
	centery = startcentery;
}

void dopan(int panx, int pany)
{
	long int llox, lhix, lloy, lhiy;

	/*
	 * use these longs to check that we don't increment past abs 32K or
	 * 0x0L.  Note that you need the L to signal long constant.  Actually
	 * going right out to the edge can break things, so I'll just go to
	 * 0x7000L
	 */

	llox = (long) lox + panx * (deltax / 2);
	lhix = (long) hix + panx * (deltax / 2);
	lloy = (long) loy + pany * (deltay / 2);
	lhiy = (long) hiy + pany * (deltay / 2);
#pragma warn -sig
	if (llox > -0x7000L && lhix < 0x7000L)
	{			/* need LONG constants */
		lox = llox;
		hix = lhix;
	}
	if (lloy > -0x7000L && lhiy < 0x7000L)
	{
		loy = lloy;
		hiy = lhiy;
	}
#pragma warn .sig
	centerx = lox + deltax / 2;
	centery = loy + deltay / 2;

	iteration = 0;
	/* use iteration as counter to prevent a double pan */
	installmode();
	setfwindow();
	fillflock();
	resetcursor();
	if (tweaking || (triangle_display_mode && triangle_editing_mode))
		OutlineCurrentMap();
}

void recenter(void)
{
	dozoom(-2);
}
void maybe_change_modes(int flocker)
{
	static double minim = 1.0 / (double) BLOAT;
	static double maxim = (double) 0x3FFF / (double) BLOAT;

	if (fdeltax / maxx < minim || fdeltay / maxy < minim ||
	    flox < -maxim || fhix > maxim || floy < -maxim || fhiy > maxim)
	{
		if (!realmode)
		{
			if (flocker)
            flocktofflock();
			sound(200);
			delay(500);
			nosound();
		}
		realmode = 1;
	}
	else
	{
		if (realmode)
		{
			if (flocker)
            fflocktoflock();
			sound(700);
			delay(500);
			nosound();
		}
		realmode = 0;
	}
}

void dozoom(int n)
{
	int i;

	switch (n)
	{
	case 0:
		setstartwindow();
		break;

   case -4:
      break;
   case -3:
		fcenterx = flox + fdeltax * ((double) (curx - minx) / (double) maxx);
		fcentery = fhiy - fdeltay * ((double) (cury - miny) / (double) maxy);
		flox = fcenterx - fdeltax / 2;
		fhix = fcenterx + fdeltax / 2;
		floy = fcentery - fdeltay / 2;
		fhiy = fcentery + fdeltay / 2;
      break;

	case 1:
      if (fdeltax > FDELTAXMINIMUM)
      {
		   fcenterx = flox + fdeltax * ((double) (curx - minx) / (double) maxx);
		   fcentery = fhiy - fdeltay * ((double) (cury - miny) / (double) maxy);
		   fdeltax /= zoomfactor;
		   fdeltay /= zoomfactor;
		   flox = fcenterx - fdeltax / 2;
		   fhix = fcenterx + fdeltax / 2;
		   floy = fcentery - fdeltay / 2;
		   fhiy = fcentery + fdeltay / 2;
      }
      else
         return;
		break;
	case -1:
		if (fdeltax < FDELTAXLIMIT)
		{
			fcenterx = flox + fdeltax * ((double) (curx - minx) / (double) maxx);
			fcentery = fhiy - fdeltay * ((double) (cury - miny) / (double) maxy);
			flox = fcenterx - fdeltax;
			fhix = fcenterx + fdeltax;
			floy = fcentery - fdeltay;
			fhiy = fcentery + fdeltay;
			fdeltax *= zoomfactor;
			fdeltay *= zoomfactor;
		}
		else
			return;
		break;
	case -2:
		fcenterx = fstartcenterx;
		fcentery = fstartcentery;
		flox = fcenterx - fdeltax / 2;
		fhix = fcenterx + fdeltax / 2;
		floy = fcentery - fdeltay / 2;
		fhiy = fcentery + fdeltay / 2;
		break;
	}

   maybe_change_modes(true);

	setiwindow();
	installmode();
	/* I just want PART of setfwindow here, so I copy the part */
	/* Fix the new pixel coordinates of the B map's fixed points */
	fstepx = maxx / fdeltax;
	fstepy = maxy / fdeltay;
   for (i = 0; i < fBptr->n; i++)
   {
      double z1;

         
      z1 = minx + (fBptr->fixedpoint[i].x - flox) * fstepx;
      z1 = max(z1,-32767.0);
      z1 = min(z1,32767.0);
     	Bptr->fixedpoint[i].x = z1;

      z1 = miny + (fhiy - fBptr->fixedpoint[i].y) * fstepy;
      z1 = max(z1,-32767.0);
      z1 = min(z1,32767.0);
   	Bptr->fixedpoint[i].y = z1;
   }
	fillflock();
	resetcursor();
	if (tweaking || (triangle_display_mode && triangle_editing_mode))
		OutlineCurrentMap();
}

/*---Double to Int Conversions and Functionsystem Maintenance------*/

double det(double a, double b, double c, double d)
{
	return (a * d - b * c);
}

void fillfixedpoints(ffunctionsystem * K)
{
	int i;
	fpair fcenter;

	fcenter.x = fstartcenterx;
	fcenter.y = fstartcentery;
	for (i = 0; i < K->n; i++)
		K->fixedpoint[i] = fbarnimage(&K->h[i], &fcenter);

}

void bcopy(ffunctionsystem * K, ffunctionsystem * L)
{
	int i;

	L->n = K->n;
	for (i = 0; i < K->n; i++)
	{
		L->h[i].a = K->h[i].a;
		L->h[i].b = K->h[i].b;
		L->h[i].c = K->h[i].c;
		L->h[i].d = K->h[i].d;
		L->h[i].e = K->h[i].e;
		L->h[i].f = K->h[i].f;
		L->weight[i] = K->weight[i];
		L->fixedpoint[i] = K->fixedpoint[i];


		/*
		 * Gee, rudy! DOn't you know you can go L->h[i] = K->h[i];
		 * 
		 * or for that matter, can't we say L = *K;
		 * 
		 * ???
		 */

	}
}

barnmap fint(fbarnmap * h)
{
	barnmap g;

	g.a = BLOAT * h->a;
	g.b = BLOAT * h->b;
	g.c = BLOAT * h->c;
	g.d = BLOAT * h->d;
	g.e = BLOAT * h->e;
	g.f = BLOAT * h->f;
	return g;
}


void Bfint(ffunctionsystem * K, functionsystem * L)
{
	int i;

	L->n = K->n;
	for (i = 0; i < K->n; i++)
	{
		L->h[i] = fint(&K->h[i]);
	}
	weighttocuts(K, L);
}

void monoflock()
{
	flockptr->n = 1;
	flockptr->atom[0].x = centerx;
	flockptr->atom[0].y = centery;
}

void fourflock()
{
	flockptr->atom[0].x = lox;
	flockptr->atom[0].y = hiy;
	flockptr->atom[1].x = hix;
	flockptr->atom[1].y = hiy;
	flockptr->atom[2].x = lox;
	flockptr->atom[2].y = loy;
	flockptr->atom[3].x = hix;
	flockptr->atom[3].y = loy;
	flockptr->n = 4;
}

void sixteenflock()
{
	int i;

	if (!realmode)
	{
		int third, x1, x2, y1, y2;

		third = deltax / 3;
		x1 = lox + third;
		x2 = x1 + third;
		third = deltay / 3;
		y1 = loy + third;
		y2 = y1 + third;

		flockptr->atom[0].x = lox;
		flockptr->atom[0].y = hiy;
		flockptr->atom[1].x = x1;
		flockptr->atom[1].y = hiy;
		flockptr->atom[2].x = x2;
		flockptr->atom[2].y = hiy;
		flockptr->atom[3].x = hix;
		flockptr->atom[3].y = hiy;
		flockptr->atom[4].x = lox;
		flockptr->atom[4].y = y2;
		flockptr->atom[5].x = x1;
		flockptr->atom[5].y = y2;
		flockptr->atom[6].x = x2;
		flockptr->atom[6].y = y2;
		flockptr->atom[7].x = hix;
		flockptr->atom[7].y = y2;
		flockptr->atom[8].x = lox;
		flockptr->atom[8].y = y1;
		flockptr->atom[9].x = x1;
		flockptr->atom[9].y = y1;
		flockptr->atom[10].x = x2;
		flockptr->atom[10].y = y1;
		flockptr->atom[11].x = hix;
		flockptr->atom[11].y = y1;
		flockptr->atom[12].x = lox;
		flockptr->atom[12].y = loy;
		flockptr->atom[13].x = x1;
		flockptr->atom[13].y = loy;
		flockptr->atom[14].x = x2;
		flockptr->atom[14].y = loy;
		flockptr->atom[15].x = hix;
		flockptr->atom[15].y = loy;
		flockptr->n = 16;
	}
	else
	{
		double third, x1, x2, y1, y2;

		third = fdeltax / 3;
		x1 = flox + third;
		x2 = x1 + third;
		third = fdeltay / 3;
		y1 = floy + third;
		y2 = y1 + third;

		fflockptr->fatom[0].x = flox;
		fflockptr->fatom[0].y = fhiy;
		fflockptr->fatom[1].x = x1;
		fflockptr->fatom[1].y = fhiy;
		fflockptr->fatom[2].x = x2;
		fflockptr->fatom[2].y = fhiy;
		fflockptr->fatom[3].x = fhix;
		fflockptr->fatom[3].y = fhiy;
		fflockptr->fatom[4].x = flox;
		fflockptr->fatom[4].y = y2;
		fflockptr->fatom[5].x = x1;
		fflockptr->fatom[5].y = y2;
		fflockptr->fatom[6].x = x2;
		fflockptr->fatom[6].y = y2;
		fflockptr->fatom[7].x = fhix;
		fflockptr->fatom[7].y = y2;
		fflockptr->fatom[8].x = flox;
		fflockptr->fatom[8].y = y1;
		fflockptr->fatom[9].x = x1;
		fflockptr->fatom[9].y = y1;
		fflockptr->fatom[10].x = x2;
		fflockptr->fatom[10].y = y1;
		fflockptr->fatom[11].x = fhix;
		fflockptr->fatom[11].y = y1;
		fflockptr->fatom[12].x = flox;
		fflockptr->fatom[12].y = floy;
		fflockptr->fatom[13].x = x1;
		fflockptr->fatom[13].y = floy;
		fflockptr->fatom[14].x = x2;
		fflockptr->fatom[14].y = floy;
		fflockptr->fatom[15].x = fhix;
		fflockptr->fatom[15].y = floy;
		fflockptr->n = 16;
	}
/* I always keep my colors in the *flockptr even in realmode */
	for (i = 0; i < flockptr->n; i++)
		flockptr->color[i] = 15;
}
void DrawFrames(void)
{
	int i;
	rect R = display_rect;

	InsetRect(&R, 1, 1);

	ClipRect(&R);

	for (i = 0; i < fBptr->n; i++)
   {
      int c = (locked) ? (available_colors[i % 10]) : ((i%15)+1);
		_fOutlineMap(i, c, i);
   }
	ClipRect(&sR);
}

void frameflock()
{
	int i;

	if (!realmode)
	{
		int xinc, yinc, x = lox, y = hiy;
		pair w;

		xinc = deltax / maxx;
		yinc = deltay / maxy;
		for (i = 0; i < maxx; i++)
		{
			w.x = x;
			w.y = y;
			flockptr->atom[i] = w;
			x += xinc;
		}
		for (i = maxx; i < maxx + maxy; i++)
		{
			w.x = x;
			w.y = y;
			flockptr->atom[i] = w;
			y -= yinc;
		}
		for (i = maxx + maxy; i < 2 * maxx + maxy; i++)
		{
			w.x = x;
			w.y = y;
			flockptr->atom[i] = w;
			x -= xinc;
		}
		for (i = 2 * maxx + maxy; i < 2 * maxx + 2 * maxy; i++)
		{
			w.x = x;
			w.y = y;
			flockptr->atom[i] = w;
			y += yinc;
		}
		flockptr->n = 2 * maxx + 2 * maxy;
	}
	else
	{

		double xinc, yinc, x = flox, y = fhiy;
		fpair w;

		xinc = fdeltax / maxx;
		yinc = fdeltay / maxy;
		for (i = 0; i < maxx; i++)
		{
			w.x = x;
			w.y = y;
			fflockptr->fatom[i] = w;
			x += xinc;
		}
		for (i = maxx; i < maxx + maxy; i++)
		{
			w.x = x;
			w.y = y;
			fflockptr->fatom[i] = w;
			y -= yinc;
		}
		for (i = maxx + maxy; i < 2 * maxx + maxy; i++)
		{
			w.x = x;
			w.y = y;
			fflockptr->fatom[i] = w;
			x -= xinc;
		}
		for (i = 2 * maxx + maxy; i < 2 * maxx + 2 * maxy; i++)
		{
			w.x = x;
			w.y = y;
			fflockptr->fatom[i] = w;
			y += yinc;
		}
		fflockptr->n = 2 * maxx + 2 * maxy;
	}
	for (i = 0; i < flockptr->n; i++)
		flockptr->color[i] = 15;




}

unsigned char acadlogo[50][7] = {
	0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00,
	0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00,
	0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00,
	0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00,
	0x00, 0x03, 0xff, 0x7f, 0xf8, 0x00, 0x00,
	0x00, 0x03, 0xff, 0x7f, 0xf8, 0x00, 0x00,
	0x00, 0x03, 0xfe, 0x3f, 0xf8, 0x00, 0x00,
	0x00, 0x07, 0xfe, 0x3f, 0xfc, 0x00, 0x00,
	0x00, 0x07, 0xfc, 0x1f, 0xfc, 0x00, 0x00,
	0x00, 0x0f, 0xfc, 0x0f, 0xfc, 0x00, 0x00,
	0x00, 0x0f, 0xf8, 0x0f, 0xfe, 0x00, 0x00,
	0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x00,
	0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x00,
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00,
	0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00,
	0x00, 0x7f, 0x7f, 0xff, 0xff, 0x80, 0x00,
	0x00, 0x7f, 0x7f, 0xff, 0xbf, 0xc0, 0x00,
	0x00, 0x7e, 0xff, 0x7f, 0xff, 0xc0, 0x00,
	0x00, 0xfe, 0xff, 0x7f, 0xdf, 0xc0, 0x00,
	0x00, 0xfd, 0xfe, 0x3f, 0xdf, 0xe0, 0x00,
	0x01, 0xfd, 0xfd, 0xff, 0xef, 0xe0, 0x00,
	0x01, 0xf9, 0xff, 0xff, 0xe7, 0xe0, 0x00,
	0x01, 0xf3, 0xfb, 0xff, 0xe7, 0xf0, 0x00,
	0x03, 0xf3, 0xf3, 0xff, 0xf3, 0xf0, 0x00,
	0x03, 0xe3, 0xf3, 0xff, 0xf3, 0xf0, 0x00,
	0x03, 0xe7, 0xe7, 0xfb, 0xf1, 0xf8, 0x00,
	0x07, 0xc7, 0xc7, 0xfd, 0xf9, 0xf8, 0x00,
	0x07, 0xcf, 0xc7, 0xfd, 0xf8, 0xf8, 0x00,
	0x07, 0x8f, 0x87, 0xfc, 0xfc, 0x7c, 0x00,
	0x0f, 0x0f, 0x0f, 0x7e, 0x7c, 0x7c, 0x00,
	0x0f, 0x1f, 0x0f, 0x7e, 0x7c, 0x3c, 0x00,
	0x0e, 0x1e, 0x1e, 0x3e, 0x3e, 0x3e, 0x00,
	0x1e, 0x3e, 0x1c, 0x1e, 0x3e, 0x1e, 0x00,
	0x1c, 0x3c, 0x3c, 0x1f, 0x1e, 0x0e, 0x00,
	0x3c, 0x38, 0x38, 0x0f, 0x0f, 0x0f, 0x00,
	0x38, 0x78, 0x30, 0x07, 0x0f, 0x07, 0x00,
	0x30, 0x70, 0x70, 0x07, 0x87, 0x07, 0x00,
	0x70, 0x60, 0x60, 0x03, 0x83, 0x83, 0x80,
	0x60, 0xe0, 0xe0, 0x03, 0x83, 0x81, 0x80,
	0x60, 0xc0, 0xc0, 0x01, 0x81, 0x81, 0x80,
	0xc1, 0x81, 0x80, 0x00, 0xc0, 0xc0, 0xc0,
	0xc1, 0x81, 0x80, 0x00, 0xc0, 0xc0, 0xc0,
	0x81, 0x01, 0x00, 0x00, 0x40, 0x40, 0x40,
};

unsigned char smallacadlogo[25][3] = {
	0x00, 0xff, 0x80,
	0x01, 0xff, 0xc0,
	0x01, 0xff, 0xc0,
	0x01, 0xff, 0xc0,
	0x03, 0xf7, 0xe0,
	0x03, 0xf7, 0xe0,
	0x03, 0xe3, 0xe0,
	0x07, 0xff, 0xf0,
	0x07, 0xff, 0xf0,
	0x07, 0xff, 0xf0,
	0x0f, 0xff, 0xf8,
	0x0f, 0xff, 0xf8,
	0x0f, 0xf7, 0xf8,
	0x1f, 0xff, 0xfc,
	0x1f, 0xff, 0xfc,
	0x1f, 0xff, 0xfc,
	0x3f, 0xff, 0xfe,
	0x3f, 0xbe, 0xee,
	0x37, 0x77, 0xe6,
	0x76, 0x77, 0x77,
	0x6e, 0xe3, 0x33,
	0x6c, 0xc3, 0x33,
	0xd9, 0x81, 0x99,
	0xd9, 0x81, 0x99,
	0x91, 0x00, 0x88,
};

void acadflock(void)
{
	int i, j, k = 0;
	unsigned char curmask = 0x80;
	int length;

	if (Stamping)
		length = 10;
	else if (tweaking)
		length = 25;
	else
		length = 50;

	if (!realmode)
	{
		int xinc, yinc, x, y;
		int xedge;
		pair w;

		x = lox + (deltax >> 1);
		y = hiy - (deltay >> 2);
		xedge = x;
		xinc = deltax / maxx;
		yinc = deltay / maxy;
		for (i = 0; i < length; i++)
		{
			int col = 0;

			curmask = 0x80;
			x = xedge;
			for (j = 0; j < length; j++)
			{
				w.x = x;
				w.y = y;
				flockptr->atom[k] = w;

				if (Stamping)
					flockptr->color[k] = maxcolor;
				else if (!tweaking)
					flockptr->color[k] =
						(acadlogo[i][col] & curmask) ? maxcolor : 1;
				else
					flockptr->color[k] =
						(smallacadlogo[i][col] & curmask) ? maxcolor : 1;
				k++;
				x += xinc;
				curmask >>= 1;
				if (curmask == 0)
				{
					col++;
					curmask = 0x80;
				}
			}
			y -= yinc;
		}
		flockptr->n = length * length;
	}
	else
	{
		double xinc, yinc, x, y, xedge;
		fpair w;

		x = flox + (fdeltax / 2);
		y = fhiy - (fdeltay / 4);
		xedge = x;
		xinc = fdeltax / maxx;
		yinc = fdeltay / maxy;
		for (i = 0; i < length; i++)
		{
			int col = 0;

			x = xedge;
			curmask = 0x80;
			for (j = 0; j < length; j++)
			{
				w.x = x;
				w.y = y;
				fflockptr->fatom[k] = w;
				if (Stamping)
					flockptr->color[k] = maxcolor;
				else if (!tweaking)
					flockptr->color[k] =
						(acadlogo[i][col] & curmask) ? maxcolor : 1;
				else
					flockptr->color[k] =
						(smallacadlogo[i][col] & curmask) ? maxcolor : 1;

				k++;
				x += xinc;
				curmask >>= 1;
				if (curmask == 0)
				{
					col++;
					curmask = 0x80;
				}
			}
			y -= yinc;
		}
		fflockptr->n = length * length;
	}
}


void iconflock()
{
	int i, j, k, parabolaseg;
	double xinc, yinc, x = fstartlox, y = fstarthiy;
	fpair w;
	pair iw;

	static unsigned char eyeicon[21][23] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

	xinc = fstartdeltax / maxx;
	yinc = fstartdeltay / maxy;
	for (i = 0; i < 2 * maxx; i++)
	{
		w.x = x;
		iw.x = x * BLOAT;
		w.y = fstarthiy;
		iw.y = starthiy;
		fflockptr->fatom[i] = w;
		flockptr->atom[i] = iw;
		i++;
		w.y = fstartloy;
		iw.y = startloy;
		fflockptr->fatom[i] = w;
		flockptr->atom[i] = iw;
		x += xinc;
	}
	y = fstarthiy;
	for (i = 2 * maxx; i < 2 * maxx + 2 * maxy; i++)
	{
		w.y = y;
		iw.y = y * BLOAT;
		w.x = fstartlox;
		iw.x = startlox;
		fflockptr->fatom[i] = w;
		flockptr->atom[i] = iw;
		i++;
		w.x = fstarthix;
		iw.x = starthix;
		fflockptr->fatom[i] = w;
		flockptr->atom[i] = iw;
		y -= yinc;
	}

	y = fstarthiy;		/* put "eye" in NE corner */
	for (k = 0; k < 21; k++)
	{
		x = (fstartlox + (maxx - 21) * xinc);	/* hix might not be
							 * exact edge */
		for (j = 0; j < 23; j++)
		{
			if (eyeicon[k][j])
			{
				w.x = x;
				w.y = y;
				iw.x = BLOAT * x;
				iw.y = BLOAT * y;
				fflockptr->fatom[i] = w;
				flockptr->atom[i] = iw;
				i++;
			}
			x += xinc;
		}
		y -= yinc;
	}

	parabolaseg = 12;
	w.y = fstartloy + (fstartdeltay / 3);	/* put "mouth" E lower center */
	w.x = fstartlox + maxx * xinc;	/* "mouth" is a staircase */
	for (k = 0; k < 25; k++)
	{
		for (j = 0; j < parabolaseg; j++)
		{
			iw.x = BLOAT * w.x;
			iw.y = BLOAT * w.y;
			fflockptr->fatom[i] = w;
			flockptr->atom[i] = iw;
			i++;
			w.x -= xinc;
		}
		w.y += yinc;
		w.x += xinc;
		parabolaseg -= 2;
		if (parabolaseg < 2)
			parabolaseg = 2;
	}
	fflockptr->n = i;
	flockptr->n = i;
}


void DrawRudyFrames(void)
{
	int n;
	int i;
	pair p, w, z;
	fpair fw, fz;
	int color;
	int which;

	HideCursor();
	iconflock();
	n = flockptr->n;
	showfixedpoints(Bptr, fBptr);
	if (!realmode)		/* draw the icons all at once outside of step */
	{
		for (i = 0; i < n; i++)
		{
			w = flockptr->atom[i];
			which = choose();
			z = barnimage(&Bptr->h[which], &w);
			if (which == redmap)
				color = 15;
			else
			{
				color = which;
				JCOLORIZE(color);
			}
			p = pixel(&z);
			egavgapixel(p.x, p.y, color);
		}
	}
	else
	{
		for (i = 0; i < n; i++)
		{
			fw = fflockptr->fatom[i];
			which = choose();
			fz = fbarnimage(&fBptr->h[which], &fw);
			p.x = minx + (fz.x - flox) * fstepx;
			p.y = (fhiy - fz.y) * fstepy;
			color = which;
			JCOLORIZE(color);
			egavgapixel(p.x, p.y, color);
		}
	}
	ShowCursor();
}

void fillflock()
{
	iteration = 0;

	if (barnmapflag)
	{

		//DrawFrames();
		DrawRudyFrames();
	}

	switch (flocktype)
	{
	case 0:
		sixteenflock();	/* sixteenflock gives better brightness */
		break;
	case 1:
		frameflock();
		break;
	case 2:
		acadflock();
		break;
	}
}

/*------------Barnmap Functions--------------*/



void step()
{
	pair w, z, p;
	unsigned int which, i;
	unsigned char color;
	fpair fw, fz;
	int limit = (realmode) ? fflockptr->n : flockptr->n;

	if (!realmode)
   {
		for (i = 0; i < limit; i++)
		{		/* checkkeyboard and spin sometimes */
			if (!Stamping)
			{
				if ((i & 1023) == 0)
				{
					checkkeyboard();
					if (exitflag)
						return;
               if (realmode)
                  return;
					if (spinflag)
					{
						if (spinflag == 1)
							spinpalette();
						else
							revspinpalette();
					}
				}
			}
			w = flockptr->atom[i];
			if (notrace)
				show(&w, 0);
			which = choose();
			z = barnimage(&Bptr->h[which], &w);
			flockptr->atom[i] = z;
         /* Viewable? */
         if (z.x >= lox && z.x <= hix && z.y >= loy && z.y <= hiy)
   			p = pixel(&z);
			color = coloring(&p, which, i);
			if (flocktype || iteration > lower_limit)
				egavgapixel(p.x, p.y, color);
		}
   }
	else
	{
		for (i = 0; i < limit; i++)
		{		/* checkkeyboard and spin sometimes */

			if (!Stamping)
			{
				if ((i & 1023) == 0)
				{
					checkkeyboard();
					if (exitflag)
						return;
               if (!realmode)
                  return;
					if (spinflag)
					{
						if (spinflag == 1)
							spinpalette();
						else
							revspinpalette();
					}
				}
			}
			fw = fflockptr->fatom[i];
			if (notrace)
				fshow(&fw, 0);
			which = choose();
			fz = fbarnimage(&fBptr->h[which], &fw);
			fflockptr->fatom[i] = fz;

			p = fpixel(&fz);

			//fp.x = minx + (fz.x - flox) * fstepx;
			//fp.y = miny + (fhiy - fz.y) * fstepy;
			//
				//if (fp.x > 1 && fp.x < maxx && fp.y > 1 && fp.y < maxy)
				//
			{
				//color = coloring(&p, which, i);
				color = coloring(&p, which, i);
				if (flocktype || iteration > lower_limit)
					//egavgapixel((int) fp.x, (int) fp.y, color);
				egavgapixel(p.x, p.y, color);
				//
			}
		}
	}
}

/*------------------Weight Functions--------------*/
void weighttocuts(ffunctionsystem * K, functionsystem * L)
{
	int i, high = 0;

	for (i = 0; i < L->n; i++)
	{
		high = high + (K->weight[i] * MAXINDEX);
		L->hicut[i] = high;
	}
	L->hicut[(L->n) - 1] = MAXINDEX;
}


void cutstoweight(functionsystem * L, ffunctionsystem * K)
{
	int i, lower = 0;

	for (i = 0; i < L->n; i++)
	{
		K->weight[i] = (double) (L->hicut[i] - lower) / MAXINDEX;
		lower = L->hicut[i];
	}
}

void showfixedpoints(functionsystem * K, ffunctionsystem * L)
{
	int mapnum, i, j, x, y, color;

	static unsigned char mapicon[10][10] = {
		{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 1, 1, 1, 1, 0, 0, 0}
	};

	static unsigned char hollowmapicon[10][10] = {
		{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
		{1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
		{0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 1, 1, 1, 1, 0, 0, 0}
	};

	for (mapnum = 0; mapnum < K->n; mapnum++)
	{
		color = mapnum;
		JCOLORIZE(color);
		y = (K->fixedpoint[mapnum].y) - 5;
		for (j = 0; j < 10; j++)
		{
			x = (K->fixedpoint[mapnum].x) - 5;
			for (i = 0; i < 10; i++)
			{
				if ((L->weight[mapnum] && mapicon[j][i]) ||
				    (!(L->weight[mapnum]) && hollowmapicon[j][i]))
					egavgapixel(x, y, color);
				x++;
			}
			y++;
		}
	}
}

int choose()
{
	int j, index, low, high = 0;

	index = sixteenbitsa() & (MAXINDEX - 1);
	for (j = 0; j < MAXBARNMAPS; j++)
	{
		low = high;
		high = Bptr->hicut[j];
		if ((low <= index) && (index < high))
			return j;
	}
	return 0;
}


/* The weighthere(n) function can 1) add weight, 2) subtract weight,
3) set weight to zero, or 4) start editing a map */

void weighthere(int sign)
{
	int i, best, activecount = 0;
	double dx, dy, distance, bestdistance = 500;
	double weightloss = 0, weightgain = 0;

	PushBarnmap();

	for (i = 0; i < Bptr->n; i++)
	{
		if (fBptr->weight[i] > 0)	/* count the nonzero maps */
			activecount++;
		dx = (curx) - Bptr->fixedpoint[i].x;
		dy = cury - Bptr->fixedpoint[i].y;
		distance = sqrt(dx * dx + dy * dy);
		if (distance < bestdistance)
		{
			best = i;
			bestdistance = distance;
		}
	}
	switch (sign)
	{
	case 1:
		bumpup(best);
		bumpdown(best);
		cutstoweight(Bptr, fBptr);
		break;
	case 2:
		if (fBptr->weight[best] >= WEIGHTINC)
			weightloss = WEIGHTINC;
		else
			weightloss = fBptr->weight[best];
		fBptr->weight[best] -= weightloss;
		activecount--;
		if (activecount <= 0)
		{
			fBptr->weight[0] += weightloss / 2;
			fBptr->weight[Bptr->n - 1] += weightloss / 2;
		}
		else
		{
			weightgain = weightloss / activecount;
			for (i = 0; i < Bptr->n; i++)
			{
				if ((fBptr->weight[i] > 0) && (i != best))
					fBptr->weight[i] += weightgain;
			}
		}
		weighttocuts(fBptr, Bptr);
		break;
	case 3:
		redmap = best;
		stage = 0;
		barnmapflag = 1;
		flocktype = 0;
		break;
	case 4:
		weightloss = fBptr->weight[best];
		fBptr->weight[best] = 0;
		activecount--;
		if (activecount <= 0)
		{
			fBptr->weight[0] += weightloss / 2;
			fBptr->weight[Bptr->n - 1] += weightloss / 2;
		}
		else
		{
			weightgain = weightloss / activecount;
			for (i = 0; i < Bptr->n; i++)
				if ((fBptr->weight[i] > 0) && (i != best))
					fBptr->weight[i] += weightgain;
		}
		weighttocuts(fBptr, Bptr);
		break;
	}
	if (flocktype == 0)
	{			/* keep erasing old stuff */
		installmode();
		fillflock();
	}
	if (tweaking)
	{
		ShowWeights();
		ConvertParams(edmap);
		UpdateMapParam(7);
	}
	if (tweaking || (triangle_display_mode && triangle_editing_mode))
		OutlineCurrentMap();
}

void bumpup(int k)
{
	int temp;

	temp = Bptr->hicut[k] + BUMP;
	if (temp > MAXINDEX)
		temp = MAXINDEX;
	Bptr->hicut[k] = temp;
	if ((k + 1 < Bptr->n) && (temp > Bptr->hicut[k + 1]))
		bumpup(k + 1);
}

void bumpdown(int k)
{
	int temp;

	if (k > 0)
	{
		temp = Bptr->hicut[k - 1] - BUMP;
		if (temp < 0)
			temp = 0;
		Bptr->hicut[k - 1] = temp;
		if ((k > 1) && (temp < Bptr->hicut[k - 2]))
			bumpdown(k - 1);
	}
}

void __activate(ffunctionsystem * K)
{
	fBptr = K;
	Bfint(fBptr, Bptr);	/* set the active maps */
	fillfixedpoints(fBptr);	/* calculate the fixed points of fmaps */
	if (!realmode)
		setfwindow();
	fillflock();
}


void _activate(ffunctionsystem * K)
{
	installmode();
	__activate(K);
}

void activate(ffunctionsystem * K)
{
	fBptr = K;
	Bfint(fBptr, Bptr);	/* set the active maps */
	fillfixedpoints(fBptr);	/* calculate the fixed points of fmaps */
	dozoom(0);		/* setstart, setfwindow, fillflock,
				 * installmode */
}

void startnewmap(ffunctionsystem * L, int i)
{
/* either we add a new high map or overwrite an existing one.  The
new map is a simple center square.*/

	if ((i >= MAXBARNMAPS))
		return;
	L->h[i].a = 0.3;
	L->h[i].b = 0;
	L->h[i].c = 0;
	L->h[i].d = 0.3;
	L->h[i].e = fstartcenterx;
	L->h[i].f = fstartcentery;
   L->weight[i] = 0.0;
	balanceweight(L, i);
	//barnmapflag = 1;
}

void balanceweight(ffunctionsystem * L, int i)
{
/* divide weight up between existing nonzero maps and i */

	int j, livemaps = 0;

	for (j = 0; j < L->n - 1; j++)
		if (L->weight[j])
			livemaps++;
	livemaps++;
	for (j = 0; j < L->n ; j++)
		if (L->weight[j] || j == i)
			L->weight[j] = 1.0 / livemaps;
}

void flocktofflock(void)
{
	int i, n;

	n = flockptr->n;
	fflockptr->n = n;
	for (i = 0; i < n; i++)
	{
		fflockptr->fatom[i].x = (double) (flockptr->atom[i].x) / BLOAT;
		fflockptr->fatom[i].y = (double) (flockptr->atom[i].y) / BLOAT;
	}
}

void fflocktoflock(void)
{
	int i, n;

	n = fflockptr->n;
	flockptr->n = n;
	for (i = 0; i < n; i++)
	{
		/*
		 * If things are out of range, make them max out. This might
		 * cause problems, eh? Max value is 7fff; min value is
		 * 0x8000...
		 */
		fpair zed;

		zed.x = fflockptr->fatom[i].x * BLOAT;
		zed.y = fflockptr->fatom[i].y * BLOAT;

		if (zed.x > 32767.0)
			zed.x = 32767.0;

		if (zed.x < -32768.0)
			zed.x = 32768.0;

		if (zed.y > 32767.0)
			zed.y = 32767.0;

		if (zed.y < -32768.0)
			zed.y = 32768.0;


		flockptr->atom[i].x = zed.x;
		flockptr->atom[i].y = zed.y;
	}
}

int coloring(pair * p, int which, int i)
{
	int color;
	int a, b;

	switch (coloring_style)
	{
	case 0:		/* MONO */
		color = 15;
		break;
	case 1:		/* ONE MAP */
		color = which;
		JCOLORIZE(color);
		/* this is barndefs macro to avoid menu colors */
		break;
	case 2:		/* PILEUP */
		color = egavgagetpixel(p->x, p->y);
		color++;
		switch (color)
		{
		case 0:
		case 1:
		case 14:
		case 15:
		case 16:
			color = 2;
			break;
		case 7:
		case 8:
			color = 9;
			break;
		}
		break;
		/*
		 * remaining cases use the old color as well.  Both int and
		 * real mode save old colors in flock->color[]
		 */
	case 3:		/* TWO MAP */
		color = (flockptr->color[i] + which) * 2;
		JCOLORIZE(color);
		flockptr->color[i] = which;
		break;
	case 4:		/* AVERAGE */
		color = (flockptr->color[i] + which) * 2;
		RCOLORIZE(color);
		flockptr->color[i] = color;
		break;
	case 5:		/* SUM */
		which %= 4;
		which++;
		color = flockptr->color[i] + which;
		RCOLORIZE(color);
		flockptr->color[i] = color;
		break;
	case 6:		/* OVERLAY */
		color = flockptr->color[i];
		a = color & 15;
		b = color >> 4;
		which %= 16;
		color = a ^ b ^ which;
		flockptr->color[i] = (a << 4) | which;
		JCOLORIZE(color);
		break;
	case 7:		/* THREE SUM */
		color = flockptr->color[i];
		a = color & 15;
		b = color >> 4;
		which %= 16;
		color = (a + b + which);
		JCOLORIZE(color);
		flockptr->color[i] = (a << 4) | which;
		break;
	case 8:		/* PRODUCT */
		color = flockptr->color[i];
		a = (color & 15) + 1;
		b = (color >> 4) + 1;
		which++;
		color = (a * b * which);
		JCOLORIZE(color);
		flockptr->color[i] = (a << 4) | color;
		break;

	}
	return color;
}
