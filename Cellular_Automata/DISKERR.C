#define IsExtern

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <dir.h>
#include <dos.h>

#include "toy.h"

/* Let's see if we can pop up a box to complain about the error. */

int disk_error_handler(int errval, int ax, int bp, int si)
{

	rect R, R2;
	char tbuf[128];
	int err;

	int centerx = sR.Xmax / 2;
	int centery = sR.Ymax / 2;

	int height = 2 * FontHeight + 10;
	int width = sR.Xmax / 2;

	R.Xmin = sR.Xmax / 4;
	R.Xmax = R.Xmin + width;

	R.Ymin = centery - height / 2;
	R.Ymax = R.Ymin + height;

	PushRect(&R, &err);


	if (ax < 0)
		sprintf(tbuf, "Device error %x: %x %p", errval, ax, MK_FP(bp, si));
	else
		sprintf(tbuf, "Disk error on drive %c", 'A' + (ax & 0xff));

	PenColor(MENUBACK);
	PaintRect(&R);
	PenColor(MENUTEXT);
	BackColor(MENUBACK);
	R2 = R;
	InsetRect(&R2, 2, 2);
	FrameRect(&R2);

	TextAlign(alignCenter, alignTop);
	MoveTo(centerx, R.Ymin + 4);
	DrawString(tbuf);
	MoveTo(centerx, R.Ymin + FontHeight + 4);
	DrawString("Hit any key to continue");

	getch();
	PopRect(&err);
	hardretn(-1);
#pragma warn -rvl
}
