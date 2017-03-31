#include "attract.h"


int everinMap = false;

int *logsaviour = NULL;
static void vwipe(int x)
{
	pair p, top, bottom;
	int i;

	p.x = x;
	p.y = 0;
	top = screentostamp(&p);
	p.y = maxy;
	bottom = screentostamp(&p);

	for (i = 0; i < maxy; i++)
		egavgapixel(x, i, 0);
	for (i = 0; i < bottom.y - top.y; i++)
		egavgapixel(x, i + top.y, 0);
}

static void pulsenoise(double y)
{
//				      (abs(1000 * y - 800)));
   if (soundflag)
      sound(100+y*y*1000);
}


void logistic_step(void)
{
	static int pixely, newpixelx, newpixely;
   static int color=14;
	static int logstrip;
	static double y;
	int i, j;
	double x, newy;
	static int mysweep;
	int lasty;
   static int bout;


	if (!tracetype)
	{
      int lastwipe;
      everinMap = true;
      if (standardstartlog < 20)
	 lastwipe = 0;
      else if (standardstartlog < 50)
	 lastwipe = 3;
      else
	 lastwipe = 10;
		if (logisticlaunchflag)
		{
			setwindow(0);
			logisticlaunchflag = 0;
			mysweep = 0;
			pixelx = minx;
	 fillflock();	/* ?? */
			delayfactor = 0;
			if (humpspot - 0.5 < 0.01 && humpspot - 0.5 > -0.01)
			{
				fancyflag = 0;
				humpspot = 0.5;
			}

		}
		if (pixelx < 639)
		{		/* do a column */

			if (startlog - stoplog <= 30)
			{
				if (mysweep > 0 && mysweep < lastwipe)
				{
					int last = -1;
					int base = (pixelx - minx) * 30;

					for (j = 0; j < stoplog - startlog && j < 30; j++)
					{
						int y = logsaviour[base++];

						if (y != last && y >= 0 && y <= maxy)
							Pix(pixelx, y, 0);
						last = y;
					}
				}
			}
			else
			{
				if (mysweep && mysweep < lastwipe)
					vwipe(pixelx);
			}
			y = fflock1ptr->atom[pixelx];
			j = 0;
			while (j++ < startlog)
			{
				if (fancyflag == 0)
					y = fx * y * (1 - y);
				else
					y = fancyhump(fx, y, humpshift);
			}
			i = 0;
			lasty = -1;
			while (j++ < stoplog)
			{
				if (fancyflag == 0)
					y = fx * y * (1 - y);
				else
					y = fancyhump(fx, y, humpshift);
				pixely = maxy - ((y - floy) / fystep);
				if (pixely != lasty && pixely <= maxy && pixely >= 0)
				{
					if (!monoflag)
						color = colorize(j);
					else
						color = available_colors[maxcolor - 1];
					Pix(pixelx, pixely, color);
					if (soundflag)
						sound((int) (((maxy - (float) pixely) * (maxy - (float) pixely)) / 100.0 + 20));
				}
				lasty = pixely;
				if (i < 30)
					logsaviour[(pixelx - minx) * 30 + i++] = pixely;
			}
			fflock1ptr->atom[pixelx] = y;
			fx += fxstep;
			pixelx++;
		}
		else
		{
			startlog = 0;
			stoplog = max(20, standardstartlog);
			pixelx = minx;
			fx = flox;
			mysweep++;
			nosound();
		}
	}
	else if (tracetype == 1)
	{
		if (logisticlaunchflag)
		{
			pixelx = minx;
			y = logstartval;
			pixely = (maxy * (1 - y)) / 3;
			installmode();
			bout = colorize(((unsigned) sixteenbitsa()));
			logstrip = 1;
			logisticlaunchflag = 0;

		}
		if (logstrip == 1)
		{
			newpixelx = pixelx + 7;
			for (i = pixelx + 1; i <= newpixelx; i++)
				Bres(i, 0, i, maxy / 3, 0);	/* sweep */
			if (fancyflag == 0)
				newy = lvfx * y * (1 - y);
			else
				newy = fancyhump(lvfx, y, humpshift);
			newpixely = (maxy * (1 - newy)) / 3;
			Bres(pixelx, pixely, newpixelx,
			     newpixely, color);
			pixelx = newpixelx;
			y = newy;
	 pulsenoise(y);
			pixely = newpixely;
			if (pixelx >= 639 - 7)
			{
				logstrip = 2;
				pixelx = minx;
				pixely = maxy / 3 + maxy * (1 - y) / 3;
				Bres(0, maxy / 3, 0, (2 * maxy) / 3, 0);
			}
		}
		else if (logstrip == 2)
		{
			newpixelx = pixelx + 7;
			for (i = pixelx + 1; i <= newpixelx; i++)
				Bres(i, maxy / 3, i, (2 * maxy) / 3, 0);	/* sweep */
			if (fancyflag == 0)
				newy = lvfx * y * (1 - y);
			else
				newy = fancyhump(lvfx, y, humpshift);
			newpixely = maxy / 3 + maxy * (1 - newy) / 3;
			Bres(pixelx, pixely, newpixelx,
			     newpixely, color);
			pixelx = newpixelx;
			y = newy;
	 pulsenoise(y);
			pixely = newpixely;
			if (pixelx >= 639 - 7)
			{
				logstrip = 3;
				pixelx = minx;
				pixely = (2 * maxy) / 3 + maxy * (1 - y) / 3;
				Bres(0, (2 * maxy) / 3, 0, maxy, 0);
			}
		}
		else
		{
			newpixelx = pixelx + 7;
			for (i = pixelx + 1; i <= newpixelx; i++)
				Bres(i, (2 * maxy) / 3, i, maxy, 0);	/* sweep */
			if (fancyflag == 0)
				newy = lvfx * y * (1 - y);
			else
				newy = fancyhump(lvfx, y, humpshift);
			newpixely = (2 * maxy) / 3 + maxy *
				(1 - newy) / 3;
			Bres(pixelx, pixely, newpixelx,
			     newpixely, color);
			pixelx = newpixelx;
			y = newy;
	 pulsenoise(y);
			pixely = newpixely;
			if (pixelx >= 639 - 7)
			{
				pixelx = minx;
				pixely = maxy * (1 - y) / 3;
				logstrip = 1;
				Bres(0, 0, 0, maxy / 3, 0);
			}
		}
	}
	else
	{
		if (logisticlaunchflag)
		{
			installmode();
			pixelx = minx;
			while (pixelx <= 639)	/* draw parabola & diag */
			{
				x = (double) (pixelx - minx) / maxx;
				if (fancyflag == 0)
					y = lvfx * x * (1 - x);
				else
					y = fancyhump(lvfx, x, humpshift);
				pixely = maxy * (1 - y);
				hump[pixelx] = pixely;
				Pix(pixelx, pixely, 1);
				pixely = maxy * (1 - x);
				line[pixelx] = pixely;
				Pix(pixelx, pixely, 7);
				pixelx++;
			}
			y = logstartval;
			pixelx = (y * maxx) + minx;
			pixely = maxy;
			color = colorize(((unsigned) sixteenbitsa()));
			iteration = 0;
			logisticlaunchflag = 0;
		}
		if (loghumpselectflag == 1)
		{
			y = logstartval;
			pixelx = (y * maxx) + minx;
			pixely = maxy;
			color = colorize(((unsigned) sixteenbitsa()));
			iteration = 0;
			loghumpselectflag = 0;
		}
		if (cleanflag)	/* turned on only by r key & XINSERT */
		{
			for (i = minx; i < 639; i++)
			{
				Pix(i, hump[i], 1);
				Pix(i, line[i], 7);
			}
			cleanflag = 0;
		}

		if (fancyflag == 0)
			y = lvfx * y * (1 - y);
		else
			y = fancyhump(lvfx, y, humpshift);
		newpixely = maxy * (1 - y);
      ++bout;
		color = colorize(bout);
		Bres(pixelx, pixely, pixelx, newpixely, color);
		newpixelx = minx + (y * maxx);
      bout++;
		color = colorize(bout);
		Bres(pixelx, newpixely,
		     newpixelx, newpixely, color);
		pixelx = newpixelx;
		pixely = newpixely;
		if (soundflag)
			sound(100 + (unsigned)
			      (abs(1000 * y - 800)));
	}
}

