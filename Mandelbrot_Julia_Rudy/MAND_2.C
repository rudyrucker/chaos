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

#include "mand.h"



void adjustxy(void)
{
   double mybloat = (cubicflag) ? bloat / 8 : bloat;
	x = lox + pixelx * deltax;
	y = loy + (maxy - pixely) * deltay;


	if (floatflag)
	{			/* using floating point algorithm */
		fx = flox + pixelx * deltax;
		fy = floy + (maxy - pixely) * deltay;
	}

	if (ruckerflag)
	{
		if (slicetype < 2)
		{
			/* is a = x, b = y, x = x, y = y */
			if (!floatflag)
			{
				a = x;
				b = y;
				nega = -a;
				negb = -b;
				a3 = (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);

			}
			else
			{
				fa = fx;
				fb = fy;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
			}
		}
		else
		{
			/* a = x, b = x, x = y, y = y */
			if (!floatflag)
			{
				a = x;
				b = x;
				nega = -a;
				negb = -b;
				a3 = (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);

				x = y;
			}
			else
			{
				fa = fx;
				fb = fx;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fy;
			}
		}
		return;
	}

	if (slicetype && !juliaflag && cubicflag)	/* if cubic slice 1-5 */
   {

		switch (slicetype)
		{
			/*
			 * In each of these calls, the x and y which I pass
			 * play the role of	the u and v, respectively.
			 * Therefore, in the standard Muv slicetype 0 we have
			 * a = a, b = b, x = x, y = y
			 */
		case 1:
			/* Mab has a = x, b = y, x = u, y = v */
			if (!floatflag)
			{


				a = x;
				b = y;
				nega = -a;
				negb = -b;
	    a3 = 3 * (b * (b/mybloat) - a * (a/mybloat));
	    b3 = -6 * a * (b/mybloat);



				x = u;
				y = v;
			}
			else
			{
				fa = fx;
				fb = fy;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fu;
				fy = fv;
			}
			break;
		case 2:
			/* Mau has a = x, b = b, x = y, y = v */
			if (!floatflag)
			{
				a = x;
				nega = -a;
				a3 =  (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);
				x = y;
				y = v;
			}
			else
			{
				fa = fx;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fy;
				fy = fv;
			}
			break;
		case 3:
			/* Mbv has a = a, b = x, x = u, y = y */
			if (!floatflag)
			{
				b = x;
				negb = -x;
				a3 = (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);
				x = u;
			}
			else
			{
				fb = fx;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fu;
			}
			break;
		case 4:
			/* Mbu has a = a, b = x, x = y, y = v */
			if (!floatflag)
			{
				b = x;
				negb = -b;
				a3 = (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);
				x = y;
				y = v;
			}
			else
			{
				fb = fx;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fy;
				fy = fv;
			}
			break;
		case 5:
			/* Mav has a = x, b = b, x = u, y = y */
			if (!floatflag)
			{
				a = x;
				nega = -x;
				a3 = (long) (3 * (b * ((double) b / mybloat) -
						  a * ((double) a / mybloat)));
				b3 = (long) (-6 * a * (double) b / mybloat);
				x = u;
			}
			else
			{
				fa = fx;
				fa3 = 3 * (fb * fb - fa * fa);
				fb3 = -6 * (fa * fb);
				fx = fu;
			}
			break;
		}
   }
}



void spiralxy(void)
{
	static char xchanges[4] = {1, 0, -1, 0};
	static char ychanges[4] = {0, 1, 0, -1};
	static char bleep[] = {0, 2, 1, 3};

	do
	{

		if (spiralrun == 0)
		{
			/* Time to change direction */
			if (spiraldirection == -1)
			{
				/* must be the first time */
				spiraldirection = 0;
				spiralrun = 1;
				spiralbaserun = 1;
			}
			else
			{
				spiraldirection = (spiraldirection + 1) & 3;
				if (!(spiraldirection & 1))
					spiralbaserun++;
				spiralrun = spiralbaserun;
			}
		}

		pixelx += xchanges[spiraldirection];
		pixely += ychanges[spiraldirection];

		spiralrun--;

		if (pixelx >= maxx || pixelx < 0 || pixely >= maxy || pixely < 0)
		{
			if (SpiralPass == 3)
			{
				doneflag = 1;
				if (AutoGIF)
				{
					gifsave(AutoGIF == 2);
					if (SpecifiedGifName)
						exitflag = 1;
				}
			}
			else
			{
				pixelx = maxx / 2;
				pixely = maxy / 2;
				spiraldirection = -1;
				SpiralPass++;
			}
		}
	} while (!doneflag && ((pixelx & 3) ^ bleep[pixely & 3]) != SpiralPass);

	adjustxy();

}

void _stepxy(void)
{

	if (!pixelx && !pixely)
	{
		tile = 8;
		oddrowflag = 0;
	}

	if (tile == 8)
	{			/* coarsest step */
		pixelx += tile;
		if (pixelx >= maxx)
		{
			pixelx = 0;
			pixely += tile;
		}
	}
	else
	{			/* finer steps have 2 kind of line */
		if (oddrowflag)
		{
			pixelx += tile;
			if (pixelx >= maxx)
			{
				pixelx = tile;
				pixely += tile;
				oddrowflag = 0;
			}
		}
		else
		{
			pixelx += (tile << 1);
			if (pixelx >= maxx)
			{
				pixelx = 0;
				pixely += tile;
				oddrowflag = 1;
			}
		}
	}


	if (pixely >= maxy)
	{
		OneScreenDone = 1;
		tile >>= 1;
		pixelx = tile;
		pixely = 0;
		oddrowflag = 0;
		if (QuickMode)
			doneflag = 1;
	}
	if (!tile)
	{
		pixelx = 0;
		pixely = 0;
		doneflag = 1;
	}

	if (doneflag && AutoGIF)
	{
		gifsave(AutoGIF == 2);
		if (SpecifiedGifName)
			exitflag = 1;
	}

}
void stepxy(void)
{

   _stepxy();
   adjustxy();

}



/*-------------Floating point fractals------------------*/

int mandelfloat(void)
{

	register int i;
	double ftx, fty, fx2, fy2;

	ftx = fx;
	fty = fy;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 7)
	{
		fty = 2 * ftx * fty + fy;
		ftx = fx2 - fy2 + fx;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	if (i == maxiteration)
		return 0;
	else
		return i + 1;
}

int inmandelfloat(void)
{

	register int i, smallestindex;
	double ftx, fty, fx2, fy2, fdist, smallestval;

	ftx = fx;
	fty = fy;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;
	smallestval = 100;
	smallestindex = 0;

	while (i < maxiteration && fx2 + fy2 < 7)
	{
		fty = 2 * ftx * fty + fy;
		ftx = fx2 - fy2 + fx;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		fdist = fx2 + fy2;
		if (fdist < smallestval)
		{
			smallestval = fdist;
			smallestindex = i;
		}
		i++;
	}

	if (i == maxiteration)
	{
		if (insideflag == 1)
			return (int) (bloat * smallestval);
		else
			return smallestindex;
	}
	else
		return i + 1;
}

int juliafloat(void)
{

	register int i;
	double ftx, fty, fx2, fy2;

	ftx = fx;
	fty = fy;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 7)
	{
		fty = 2 * ftx * fty + fv;
		ftx = fx2 - fy2 + fu;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	if (i == maxiteration)
		return 0;
	else
		return i + 1;
}

int cubicmandelfloat(void)
{
	int poscount;
	int i;
	double ftx, fty, fxa, fyb, fx2, fy2, ftemp;

	ftx = fa;
	fty = fb;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fx;
		fty = fxa * fty + fyb * ftx + fy;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	poscount = i;

	ftx = -fa;
	fty = -fb;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fx;
		fty = fxa * fty + fyb * ftx + fy;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	i = min(poscount, i);
	if (i == maxiteration)
		return 0;
	else
		return i + 1;

}



int incubicmandelfloat(void)
{
	int poscount;
	register int i, smallestindex;
	double ftx, fty, fxa, fyb, fx2, fy2, ftemp, fdist;
	double smallestval;

	ftx = fa;
	fty = fb;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;
	smallestval = 100;
	smallestindex = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fx;
		fty = fxa * fty + fyb * ftx + fy;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		fdist = fx2 + fy2;
		if (fdist < smallestval)
		{
			smallestval = fdist;
			smallestindex = i;
		}
		i++;
	}

	poscount = i;

	ftx = -fa;
	fty = -fb;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fx;
		fty = fxa * fty + fyb * ftx + fy;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		if (fdist < smallestval)
		{
			smallestval = fdist;
			smallestindex = i;
		}
		i++;
	}

	i = min(poscount, i);
	if (i == maxiteration)
	{
		if (insideflag == 1)
			return (int) (bloat * smallestval);
		else
			return smallestindex;
	}
	else
		return i + 1;
}


int cubicjuliafloat(void)
{
	int poscount;
	register int i;
	double ftx, fty, fxa, fyb, fx2, fy2, ftemp;

	ftx = fx;
	fty = fy;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fu;
		fty = fxa * fty + fyb * ftx + fv;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	poscount = i;

	ftx = -fa;
	fty = -fb;
	fx2 = ftx * ftx;
	fy2 = fty * fty;
	i = 0;

	while (i < maxiteration && fx2 + fy2 < 4)
	{
		fxa = fx2 - fy2 + fa3;
		fyb = 2 * ftx * fty + fb3;
		ftemp = fxa * ftx - fyb * fty + fu;
		fty = fxa * fty + fyb * ftx + fv;
		ftx = ftemp;
		fx2 = ftx * ftx;
		fy2 = fty * fty;
		i++;
	}

	i = min(poscount, i);
	if (i == maxiteration)
		return 0;
	else
		return i + 1;
}

void _readcursor(double *ffu,double *ffv,double *ffa,double *ffb,int juliaflag)
{
   *ffa = fa;
   *ffb = fb;
   *ffu = fu;
   *ffv = fv;

	if (juliaflag && mandelflag)
	{
		*ffu = mandelview.vlox +
		(curx - minx) * (mandelview.vhix - mandelview.vlox) / maxx;
		*ffv = mandelview.vloy +
			(maxy - cury) * (mandelview.vhiy - mandelview.vloy) / maxy;
		return;
	}
	if (juliaflag && cubicflag && !ruckerflag)
	{
		switch (slicetype)
		{
		case 0: /* Muv */
			*ffu = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffv = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		case 1: /* Mab */
			*ffa = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffb = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		case 2: /* Mau */
			*ffa = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffu = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		case 3: /* Mbv */
			*ffb = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffv = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		case 4: /* Mbu */
			*ffb = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffu = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		case 5: /* Mav */
			*ffa = cubicview.vlox + (curx - minx) *
				(cubicview.vhix - cubicview.vlox) / maxx;
			*ffv = cubicview.vloy + (maxy - cury) *
				(cubicview.vhiy - cubicview.vloy) / maxy;
			break;
		}
		return;
	}
	if (juliaflag && ruckerflag)
	{
		*ffu = ruckerview.vlox +
			(curx - minx) * (ruckerview.vhix - ruckerview.vlox) / maxx;
		*ffv = ruckerview.vloy +
			(maxy - cury) * (ruckerview.vhiy - ruckerview.vloy) / maxy;
		if (slicetype < 2)
		{
			*ffa = *ffu;
			*ffb = *ffv;
		}
		else
		{
			*ffa = *ffu;
			*ffb = *ffu;
			*ffu = *ffv;
		}
		return;
	}
	if (cubicflag && !juliaflag)
	{
		if (!ruckerflag)
			switch (slicetype)
			{
			case 0:
				*ffa = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffb = 0.6 - ((1.2 * cury) / maxy);
				break;
			case 1:
				*ffu = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffv = 0.6 - ((1.2 * cury) / maxy);
				break;
			case 2:
				*ffb = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffv = 0.6 - ((1.2 * cury) / maxy);
				break;
			case 3:
				*ffa = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffu = 0.6 - ((1.2 * cury) / maxy);
				break;
			case 4:
				*ffa = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffv = 0.6 - ((1.2 * cury) / maxy);
				break;
			case 5:
				*ffb = -0.6 + ((1.2 * (curx - minx)) / maxx);
				*ffu = 0.6 - ((1.2 * cury) / maxy);
				break;
			}
		else
			switch (slicetype)
			{
			case 0:
				*ffa = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffb = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			case 1:
				*ffu = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffv = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			case 2:
				*ffb = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffv = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			case 3:
				*ffa = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffu = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			case 4:
				*ffa = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffv = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			case 5:
				*ffb = ruckerview.vlox + (curx - minx) *
					(ruckerview.vhix - ruckerview.vlox) / maxx;
				*ffu = ruckerview.vloy + (maxy - cury) *
					(ruckerview.vhiy - ruckerview.vloy) / maxy;
				break;
			}
	}
}

void readcursor(void)
{
   _readcursor(&fu,&fv,&fa,&fb,juliaflag);
}

void popview(view * w)
{
	double dx, dy;

	flox = w->vlox;
	fhix = w->vhix;
	floy = w->vloy;
	fhiy = w->vhiy;
	if (fourdeeflag && w == &cubicview)
	{
		dx = (fhix - flox) / 2;
		dy = (fhiy - floy) / 2;
		switch (slicetype)
		{
		case 0: /* Muv */
			flox = fu - dx;
			fhix = fu + dx;
			floy = fv - dy;
			fhiy = fv + dy;
			break;
		case 1: /* Mab */
			flox = fa - dx;
			fhix = fa + dx;
			floy = fb - dy;
			fhiy = fb + dy;
			break;
		case 2: /* Mau */
			flox = fa - dx;
			fhix = fa + dx;
			floy = fu - dy;
			fhiy = fu + dy;
			break;
		case 3: /* Mbv */
			flox = fb - dx;
			fhix = fb + dx;
			floy = fv - dy;
			fhiy = fv + dy;
			break;
		case 4: /* Mbu */
			flox = fb - dx;
			fhix = fb + dx;
			floy = fu - dy;
			fhiy = fu + dy;
			break;
		case 5: /* Mav */
			flox = fa - dx;
			fhix = fa + dx;
			floy = fv - dy;
			fhiy = fv + dy;
			break;
		}
//		centercursorflag = 1;
		pushview(&cubicview);
	}
}

void popstamp(stampdata * w)
{
	popview(&w->v);
	mandelflag = w->mandelflag;
	cubicflag = w->cubicflag;
	insideflag = w->insideflag;
	ruckerflag = w->ruckerflag;
	juliaflag = w->juliaflag;
   maxiteration = w->iterations;
	fa = w->fa;
	fb = w->fb;
	fu = w->fu;
	fv = w->fv;
}


void pushview(view * w)
{
	w->vlox = flox;
	w->vhix = fhix;
	w->vloy = floy;
	w->vhiy = fhiy;
}

void pushstamp(stampdata * w)
{
	pushview(&w->v);
	w->mandelflag = mandelflag;
	w->cubicflag = cubicflag;
	w->insideflag = insideflag;
	w->ruckerflag = ruckerflag;
	w->juliaflag = juliaflag;
	w->fa = startfa;
	w->fb = startfb;
	w->fu = startfu;
	w->fv = startfv;
   w->iterations = maxiteration;
}
void slowdown_warning(void)
{
	sound(500);
	delay(100);
	sound(300);
	delay(300);
	nosound();
}

void speedup_warning(void)
{
	sound(300);	/* signal return to hispeed mode */
	delay(100);
	sound(800);
	delay(300);
	nosound();
}

void useview()
{
	long mybloat;
	int toosmall;
	int i, j, k;
   rect R;
   static int VeryFirstTime = 1;

   lastrowpainted = maxy;

	starttime = clock();
	BoxActive = 0;
	if (SpiralMode)
	{
		pixelx = maxx / 2;
		pixely = maxy / 2;
		spiraldirection = -1;
		spiralrun = 0;
		SpiralPass = 0;
	}
	else
	{
		pixelx = 0;
		pixely = 0;
	}

	if (juliaflag)
		start_julia();
	else if (mandelflag)
		start_mandel();
	else if (cubicflag && !ruckerflag)
		start_cubic();
	else
		start_CCM();

	if (cubicflag)
		mybloat = bloat / 8;
	else
		mybloat = bloat;
	u = fu * mybloat;
	v = fv * mybloat;
	a = fa * mybloat;
	b = fb * mybloat;
	nega = -a;
	negb = -b;
	fa3 = 3 * (fb * fb - fa * fa);
	fb3 = -6 * fa * fb;
	a3 = fa3 * mybloat;
	b3 = fb3 * mybloat;
	lox = flox * mybloat;
	hix = fhix * mybloat;
	loy = floy * mybloat;
	hiy = fhiy * mybloat;
	fx = flox;
	fy = fhiy;

	deltax = (double) (hix - lox) / maxx;
	deltay = (double) (hiy - loy) / maxy;
	toosmall = 0;
	if (deltax <= 0.5 || deltay <= 0.5 || fractal == incubicmandelfloat)
		toosmall = 1;
	if (toosmall || forcefloat)
	{			/* go to floating point */
		if (!floatflag)
		{		/* warn of slowdown */
	 slowdown_warning();

	 integer_to_float();
	   	floatflag = 1;
		}

		if (mandelflag)
		{
			if (juliaflag)
				fractal = juliafloat;
			else if (!insideflag)
				fractal = mandelfloat;
			else
				fractal = inmandelfloat;
		}
		else
		{		/* cubic case */
			if (juliaflag)
				fractal = cubicjuliafloat;
			else if (!insideflag)
				fractal = cubicmandelfloat;
			else
				fractal = incubicmandelfloat;
		}
		deltax = (fhix - flox) / maxx;
		deltay = (fhiy - floy) / maxy;
	}
	if (!forcefloat && !toosmall && floatflag)
	{			/* getting out of floating point */
		floatflag = 0;
      float_to_integer();
		if (mandelflag)
		{
			if (juliaflag)
			{
				if (!chip)
					fractal = julia32;
				else
					fractal = julia16;
			}
			else if (!insideflag)
			{
				if (!chip)
 					fractal = mandel32;
				else
					fractal = mandel16;
			}
			else
			{
				if (!chip)
				{
					if (insideflag == 1)
						fractal = in2cubicmandel32;
					else
						fractal = in1cubicmandel32;
				}
				else
				{
					if (insideflag == 1)
						fractal = in1cubmand16;
					else
					{
						fractal = incubicmandelfloat;
						deltax = (fhix - flox) / maxx;
						deltay = (fhiy - floy) / maxy;
					}
				}
			}
		}
		else
		{
			if (juliaflag)
			{
				if (!chip)
					fractal = cubicjulia32;
				else
					fractal = cubicjulia16;
			}
			else if (!insideflag)
			{
				if (!chip)
					fractal = cubicmandel32;
				else
					fractal = cubicmandel16;
			}
			else
			{
				if (!chip)
				{
					if (insideflag == 1)
						fractal = in2cubicmandel32;
					else
						fractal = in1cubicmandel32;
				}
				else
				{
					if (insideflag == 1)
						fractal = in1cubmand16;
					else
					{
						fractal = incubicmandelfloat;
						deltax = (fhix - flox) / maxx;
						deltay = (fhiy - floy) / maxy;
					}
				}
			}
		}
      speedup_warning();

	}

   starta=a,startb=b,startu=u,startv=v;
   startfa=fa,startfb=fb,startfu=fu,startfv=fv;

	adjustxy();
	HideCursor();
	if (SaveMe && (OneScreenDone || VeryFirstTime))
	{
		/*
		 * Steal 1/8 of the pixels in the old screen before we delete
		 * everything...
		 */

      int savesize = (int)(72L * ((maxy+1)/8L));

//		erasecursor();
		if (!savearray[saveptr])
		{
			safe_alloc = 1;
   			savearray[saveptr] = farcalloc((long)savesize,1L);
		}
		if (savearray[saveptr] == NULL)
		{
		        ErrorBox("Not enough memory to preserve stamp.");
			SaveMe = 0;
		}
		else
		{

      if (!OneScreenDone && VeryFirstTime)
      {
	 int row;
	 for(row=0;row < (maxy+1)/8;row++)
	 {
	    if (mode == 0x10)
	       memcpy(savearray[saveptr]+72*row,
	       mand10_img+70*row,70);
	    else
	       memcpy(savearray[saveptr]+72*row,
	       mand12_img+70*row,70);
	 }
      }
      else
      {

		   for (i = 0; i < (maxy + 1) / 8; i++)
	 {
	    char rowbuffer[320];
	    int start = minx/8;
	    int startx2 = 80 + start;
	    int startx3 = 160 + start;
	    int startx4 = 240 + start;


	    blast_in_row(rowbuffer,i*8);
	    for(j=0;j<70;j++)
	    {
	       char t;
	       t = (rowbuffer[start+j] & 0x80) >> 7;
	       t |= (rowbuffer[startx2+j] & 0x80) >> 6;
	       t |= (rowbuffer[startx3+j] & 0x80) >> 5;
	       t |= (rowbuffer[startx4+j] & 0x80) >> 4;
				   savearray[saveptr][i * 72 + j] = t;
	    }
	 }
      }
      VeryFirstTime = 0;

		if (DrawZoomBox)
		{
			/* draw the zoom box in the right place for this... */
			double z1 = zoomfactor;
			int boxwidth = z1 * 70;
			int boxheight = z1 * maxy / 8;
			int startx = ((curx - minx) / 8) - boxwidth / 2;
			int endx = startx + boxwidth;
			int starty = (cury / 8) - (boxheight / 2);
			int endy = starty + boxheight;

			if (startx < 0)
				startx = 0;
			if (endx >= 70)
				endx = 69;
			if (starty < 0)
				starty = 0;
			if (endy >= (maxy / 8) - 1)
				endy = maxy / 8 - 1;
			for (i = starty + 1; i < endy; i++)
			{
				savearray[saveptr][i * 72 + startx] = 15;
				savearray[saveptr][i * 72 + endx] = 15;
			}
			for (i = startx; i <= endx; i++)
			{
				savearray[saveptr][starty * 72 + i] = 15;
				savearray[saveptr][endy * 72 + i] = 15;
			}
		}

		last_saved = saveptr;
		saveptr++;
		if (saveptr >= STAMPCOUNT)
			saveptr = 0;
		SaveMe = 0;
		DrawZoomBox = 0;
		}
	}

   /*  just erase the right areas of the screen */
   R = sR;
   R.Xmin = minx;
   BackColor(BLACK);
   EraseRect(&R);

   /* and the stamping areas */
//   R.Xmin = 0;
//   R.Xmax = minx - 1;
//   R.Ymin = stampstarts[STAMPCOUNT-1];
//   EraseRect(&R);

   PenColor(BLACK);
   for(i=0;i<STAMPCOUNT;i++)
      PaintRect(&stamprects[i]);

	usepalette();		/* reset to active ega or vga palettenumber */
	if (centercursorflag)
      resetcursor();

	/* Always display the most recent one on the bottom. */
	if (last_saved != -1)
	{
		for (k = 0; k < STAMPCOUNT; k++)
		{
			int ll = last_saved - k;

			if (ll < 0)
				ll += STAMPCOUNT;

			if (savearray[ll])
	 {
				for (i = 0; i < (maxy + 1) / 8; i++)
					rowblast(&savearray[ll][i * 72], 8, stampstarts[k] + i, 72);
	 }


		}
	}
   for(i=0;i<STAMPCOUNT;i++)
   {
      rect R;
      R = stamprects[i];
      PenColor(0xf);
      FrameRect(&R);
      InsetRect(&R,1,1);
      PenColor(1);
      FrameRect(&R);
   }
   PenColor(BLUE);
   MoveTo(stamprects[0].Xmax+2,stamprects[0].Ymax);
   LineTo(stamprects[0].Xmax+2,stamprects[STAMPCOUNT-1].Ymin);


	testcenterflag = 1;	/* be watchful of bad spiral start */
	testcentercount = 0;
	oldcolorband = 0;	/* for sound */
	OneScreenDone = 0;
   bottomline();
	doneflag = 0;		/* to restart calculation */
   newflag = 1;


//   setup_calcmand();
   tile = 8;
   ShowCursor();


}

void Pan(void)
{
	double fcurx, fcury;
   double dx = fhix - flox;
   double dy = fhiy - floy;

	pushstamp(&stampviews[saveptr]);

	fcurx = (flox * (maxx - (curx - minx)) + fhix * (curx - minx)) / maxx;
	fcury = (fhiy * (maxy - cury) + floy * cury) / maxy;

   fcurx = max(-2.0,fcurx);
   fcurx = min(2.0,fcurx);
   fcury = max(-2.0,fcury);
   fcury = min(2.0,fcury);

   flox = fcurx - dx/2;
   fhix = fcurx + dx/2;
   floy = fcury - dy/2;
   fhiy = fcury + dy/2;

	if (mandelflag && !juliaflag)
		pushview(&mandelview);
	if (cubicflag && !ruckerflag && !juliaflag)
		pushview(&cubicview);
	if (ruckerflag)
		pushview(&ruckerview);
	SaveMe = 1;
	centercursorflag = 1;
   HideCursor();
	useview();
   ShowCursor();
   doneflag = 0;
}






void dozoom(double zoom)
{
	double fcurx, fcury;
	double xavg, yavg, dx, dy;
   double oflox = flox;
   double ofloy = floy;
   double ofhix = fhix;
   double ofhiy = fhiy;
   int omax = maxiteration;

	dx = (fhix - flox) * zoom;
	dy = (fhiy - floy) * zoom;

   dx = max(dx,1.0e-13);
   dy = max(dy,1.0e-13);
   dx = min(dx,5.6);
   dy = min(dy,4.5);


	fcurx = (flox * (maxx - (curx - minx)) + fhix * (curx - minx)) / maxx;
	fcury = (fhiy * (maxy - cury) + floy * cury) / maxy;


	xavg = fcurx;
	yavg = fcury;

	pushstamp(&stampviews[saveptr]);
	if (!cubicflag)
		maxiteration = max(50, -log10(dx) * 120);
	else
		maxiteration = max(20, -log10(dx) * 60);

	flox = xavg - (dx / 2);
	fhix = xavg + (dx / 2);
	floy = yavg - (dy / 2);
	fhiy = yavg + (dy / 2);

   if (flox == fhix || floy == fhiy)
   {
      flox = oflox;
      fhix = ofhix;
      floy = ofloy;
      fhiy = ofhiy;
      maxiteration = omax;
      sound(55);
      delay(10);
      nosound();
      return;
   }


	if (mandelflag && !juliaflag)
		pushview(&mandelview);
	if (cubicflag && !ruckerflag && !juliaflag)
		pushview(&cubicview);
	if (ruckerflag)
		pushview(&ruckerview);
	if (zoom < 1)
		DrawZoomBox = 1;
	SaveMe = 1;
	centercursorflag = 1;
   HideCursor();
	useview();
   ShowCursor();
   doneflag = 0;
}

/*----------Color Functions-----------------------------------------*/
/*
 * My strategy here will be to say that iteration j will get a color between
 * 1 and 15 determined by the high four bits to the right of the leftmost
 * nonzero bit of j.
 */


int acceptable_colors[10] = {
   2,3,5,6,9,10,11,12,13,14
   };

void fillcolorval(void)
{
	int j, k, color;

	colorval[0] = 0;
	for (j = 1; j < ITERATIONCAP; j++)
	{
		k = (int) (bandsize * j);
		if (k < 16)
			color = k;
		else if (k < 32)
			color = k;
		else if (k < 64)
			color = k / 2;
		else if (k < 256)
			color = k / 4;
		else if (k < 512)
			color = k / 8;
		else if (k < 1024)
			color = k / 16;
		else if (k < 2048)
			color = k / 32;
		else if (k < 4096)
			color = k / 32;
		else if (k < 8000)
			color = k / 64;
		else if (k < 16000)
			color = k / 128;
		else if (k < 32000)
			color = k / 128;
		else
			k = j;
      if (locked)
      {
	 color %= 10;
   	   colorval[j] = acceptable_colors[color];
      }
      else
      {
	 color %= 15;	/* gives val from 0 to 14 */
	 color++;	/* gives number from 1 to 15 */
   	   colorval[j] = color;
      }
	}
}

void float_to_integer(void)
{
   long mybloat;

   mybloat = (cubicflag) ? bloat/8 : bloat;

   u = fu * mybloat;
   v = fv * mybloat;
   a = fa * mybloat;
   b = fb * mybloat;
   lox = flox * mybloat;
   loy = floy * mybloat;
   hix = fhix * mybloat;
   hiy = fhiy * mybloat;
}

void integer_to_float(void)
{
   double mybloat;

   mybloat = (cubicflag) ? bloat/8.0 : bloat;

   fu = u / mybloat;
   fv = v / mybloat;
   fa = a / mybloat;
   fb = b / mybloat;
   flox = lox / mybloat;
   fhix = hix / mybloat;
   floy = loy / mybloat;
   fhiy = hiy / mybloat;
}

void bottomline(void)
{
   switch(ParameterDisplayMode)
   {
   case 0:
      menutext("");
      break;
   case 1:
      _updatecursor(true);
      break;
   case 2:
      draw_params();
      break;
   }
}

