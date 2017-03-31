#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "toy.h"


/* Dragging a zoombox around, from in which zoom to at. */


#pragma argsused
#define minx 160
#define maxx 479
#define maxy HIYCOUNT

void FastFrameRect(rect *sR)
{
   rect R = *sR;
   RasterOp(zXORz);
   PenColor(15);
   FrameRect(&R);
   InsetRect(&R,1,1);
   PenColor(7);
   FrameRect(&R);
   InsetRect(&R,1,1);
   PenColor(15);
   FrameRect(&R);
}

int DragRect(rect *R,int x1,int y1)
{
	rect zoomR;
	rect clipR;
	int key, button;
	int stride = 9;
   int X,Y;


   zoomR = *R;
   OffsetRect(&zoomR,x1,y1);
   Centers(&zoomR,&X,&Y);
	clipR = sR;
	clipR.Xmin = sR.Xmax / 8;
	ClipRect(&clipR);
   HideCursor();

	PenColor(WHITE);
	RasterOp(zXORz);
	FastFrameRect(&zoomR);

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
				key = 0x0d;


		}
		else
		{
			if (e.CursorX != X || e.CursorY != Y)
			{
				xstep = e.CursorX - X;
				ystep = e.CursorY - Y;
			}
		}
      if (key == XINSERT)
         key = 0x0d;
      if (key == XDELETE)
         break;

		if (key == 0x1b || key == XALTZ)
			break;

      if (key == XF1)
      {
         ShowCursor();
         helptext("zoomfix.hlp");
         HideCursor();
         continue;
      }

		if (key == 0x0d)
			break;

		switch (e.ScanCode << 8)
		{
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

			FastFrameRect(&zoomR);
			OffsetRect(&zoomR, xstep, ystep);
			/* Now force the fucker if it is out of range */
			X += xstep;
			Y += ystep;
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
			FastFrameRect(&zoomR);
		}
	}
	FastFrameRect(&zoomR);
	ClipRect(&sR);
	RasterOp(zREPz);
   *R = zoomR;
   ShowCursor();
	if (key == 0x0d)
      return 1;
   else if (key == XDELETE)
      return -1;
   else
      return 0;
}
