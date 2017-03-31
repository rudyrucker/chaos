#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "GRconst.h"
#include "GRports.h"
#include "GRextrn.h"

#include "buttons.h"
#include "attrdefs.h"
#include "attrvars.h"
#include "attrexts.h"


/* Dragging a zoombox around, from in which zoom to at. */
double yzoomfactor=2.0;

static void BigFrame(rect *RR)
{
   rect R = *RR;
   RasterOp(zXORz);
   PenColor(WHITE);
   FrameRect(&R);
   InsetRect(&R,1,1);
   PenColor(7);
   FrameRect(&R);
   PenColor(WHITE);
   InsetRect(&R,1,1);
   FrameRect(&R);
   RasterOp(zREPz);
}

void zoombox(void)
{
	rect zoomR;
	point pt;
	int X, Y;
	rect clipR;
	double aspect;
	int key, button;
	int stride = 9;
	int direction = 0;

	/* First time, just drag a deltax sized boxed, wiggle it around */

	int height, width;

	if (curx < minx)
		HideCursor();
	else
		erasecursor();


	clipR = sR;
	clipR.Xmin = sR.Xmax / 8;
	ClipRect(&clipR);

  	width = maxx / zoomfactor;

   if (dimension == LOGISTIC && !tracetype)
      height = maxy / yzoomfactor;
   else
      height = maxy / zoomfactor;

   width = max(width,8);
   height = max(height,8);



	aspect = height / (float) width;


	X = pt.X = curx;
	Y = pt.Y = cury;

	CenterRect(&pt, width, height, &zoomR);
	MoveCursor(pt.X, pt.Y);

	BigFrame(&zoomR);

	while (1)
	{
		event e;
		int xstep = 0, ystep = 0;
		int xd = 0, yd = 0;

		int n = KeyEvent(false, &e);

		key = 0;
		if (n)
		{

			button = (e.State >> 8) & 0x7;


			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else
				key = e.ScanCode << 8;


			if (button == swLeft || key == XINSERT || key == 0x0d)
			{
            key = 0x0d;
				direction = 1;
				break;
			}
			else if (button == swRight || key == XDELETE)
			{
            key = 0x0d;
				direction = -1;
				break;
			}
			else if (key == XALTO)
			{
				direction = -2;
				break;
			}


		}
		else
		{
			if (e.CursorX != X || e.CursorY != Y)
			{
				xstep = e.CursorX - X;
				ystep = e.CursorY - Y;
			}
		}

		if (key == 0x1b || key == ' ' || key == XALTZ)
			break;


		switch (e.ScanCode << 8)
		{
      case XF1:
         ShowCursor();
         helptext((dimension == LOGISTIC) ? "zoomvar.hlp" : "zoomasp.hlp");
         HideCursor();
         break;
		case XHOME:
			xstep = ystep = -stride;
			break;
		case XPGDN:
			xstep = ystep = stride;
			break;
		case XLARROW:
			xstep = -stride;
			break;
		case XRARROW:
			xstep = stride;
			break;
		case XDARROW:
			ystep = stride;
			break;
		case XUARROW:
			ystep = -stride;
			break;
		case XPGUP:
			xstep = stride;
			ystep = -stride;
			break;
		case XEND:
			xstep = -stride;
			ystep = stride;
			break;
		case XCENTER5:
			stride ^= 8;
			break;
		}





		if (xstep || ystep)
		{

         BigFrame(&zoomR);
			if ((e.State & 3) || (dimension == LORENZ && lorenzflyflag))
			{
            rect RR = zoomR;
				InsetRect(&zoomR, xstep, ystep);
				/* pop the aspect ratio */

				if (!(dimension == LOGISTIC && tracetype == 0))
				{
					if (xstep)
					{
						int yc;
						int height;

						yc = zoomR.Ymin + (zoomR.Ymax - zoomR.Ymin) / 2;
						height = aspect * (zoomR.Xmax - zoomR.Xmin);
						zoomR.Ymin = yc - height / 2;
						zoomR.Ymax = zoomR.Ymin + height;
					}
					else
					{
						int xc;
						int width;

						xc = zoomR.Xmin + (zoomR.Xmax - zoomR.Xmin) / 2;
						width = (zoomR.Ymax - zoomR.Ymin) / aspect;
						zoomR.Xmin = xc - width / 2;
						zoomR.Xmax = zoomR.Xmin + width;
					}
				}
            if (zoomR.Xmin > zoomR.Xmax-8 || zoomR.Ymin > zoomR.Ymax-8)
               zoomR = RR;


			}
			else
			{

				OffsetRect(&zoomR, xstep, ystep);
				/* Now force the fucker if it is out of range */
				X += xstep;
				Y += ystep;
			}
			if (zoomR.Xmin < minx && zoomR.Xmax > minx + maxx ||
			    zoomR.Ymin < minx && zoomR.Ymax > minx + maxx)
				InsetRect(&zoomR, 10, 10);




			if (zoomR.Xmin < minx)
				xd = minx - zoomR.Xmin;
			if (zoomR.Xmax > minx + maxx)
				xd = minx + maxx - zoomR.Xmax;
			if (zoomR.Ymin < 0)
				yd = -zoomR.Ymin;
			if (zoomR.Ymax > maxy)
				yd = maxy - zoomR.Ymax;

			if (xd || yd)
			{
				OffsetRect(&zoomR, xd, yd);
				X += xd;
				Y += yd;
			}
			MoveCursor(X, Y);
         BigFrame(&zoomR);
		}
	}
	BigFrame(&zoomR);
	ClipRect(&sR);
	RasterOp(zREPz);
	if (direction != 0)
	{
      double z;

		zoomfactor = 1.0 / ((zoomR.Xmax - zoomR.Xmin) / (float) (maxx));
      if (dimension == LOGISTIC && tracetype == 0)
         z = yzoomfactor = 1.0/((zoomR.Ymax - zoomR.Ymin) / (float) (maxy));
      else
         z = zoomfactor;
		curx = zoomR.Xmin + (zoomR.Xmax - zoomR.Xmin) / 2;
		cury = zoomR.Ymin + (zoomR.Ymax - zoomR.Ymin) / 2;
		_dozoom(direction,zoomfactor,z);
	}
	if (curx < minx)
		ShowCursor();
	else
		drawcursor();
}
