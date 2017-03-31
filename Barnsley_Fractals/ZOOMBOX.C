#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "game.h"

/* Dragging a zoombox around, from in which zoom to at. */
static void BigFrame(rect *RR)
{
   rect R = *RR;
   RasterOp(zXORz);
   PenColor(WHITE);
   FrameRect(&R);
   InsetRect(&R,1,1);
   PenColor(7);
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

	/* First time, just drag a deltax sized boxed, wiggle it around */

	int height, width;

	HideCursor();
   PushCursorType();
   ArrowCursor();
	clipR = sR;
	clipR.Xmin = sR.Xmax / 8;
	ClipRect(&clipR);

	width = maxx / zoomfactor;
   aspect = (clipR.Xmax-clipR.Xmin)/(float)(clipR.Ymax-clipR.Ymin);
   width = max(maxx/zoomfactor,8);
   height = width / aspect;


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

			if (button == swRight)
				key = XDELETE;

			if (button == swLeft)
				key = XINSERT;


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

		if (key == 0x0d || key == XINSERT || key == XDELETE)
		{
			zoomfactor = 1.0 / ((zoomR.Xmax - zoomR.Xmin) / (float) (maxx));
			break;
		}

		switch (e.ScanCode << 8)
		{
		case XF1:
			ShowCursor();
			helptext("zoomasp.hlp");
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
			if (e.State & 3)
			{
            rect RR = zoomR;

				InsetRect(&zoomR, xstep, ystep);
				/* pop the aspect ratio */

				if (xstep)
				{
					int yc;
					int height;

					yc = zoomR.Ymin + (zoomR.Ymax - zoomR.Ymin) / 2;
					height = (zoomR.Xmax - zoomR.Xmin) / aspect;
             
					zoomR.Ymin = yc - height / 2;
					zoomR.Ymax = zoomR.Ymin + height;
				}
				else
				{
					int xc;
					int width;

					xc = zoomR.Xmin + (zoomR.Xmax - zoomR.Xmin) / 2;
					width = (zoomR.Ymax - zoomR.Ymin) * aspect;
					zoomR.Xmin = xc - width / 2;
					zoomR.Xmax = zoomR.Xmin + width;
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
			BigFrame(&zoomR);
         MoveCursor(X,Y);
		}
	}
	BigFrame(&zoomR);
	ClipRect(&sR);
	RasterOp(zREPz);
   if (key == 0x0d || key == XINSERT || key == XDELETE)
   {
   	curx = zoomR.Xmin + (zoomR.Xmax - zoomR.Xmin) / 2;
   	cury = zoomR.Ymin + (zoomR.Ymax - zoomR.Ymin) / 2;
      dozoom((key == XDELETE) ? -1 : 1);
   }
   PopCursorType();
	ShowCursor();
}
