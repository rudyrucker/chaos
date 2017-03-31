#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "GRconst.h"
#include "GRports.h"
#include "GRextrn.h"

/* Let's see if we can pop up a box to complain about the error. */
extern rect sR;
extern int FontHeight;
extern void PushRect(rect *,int *);
extern void PopRect(int *);

int disk_error_handler(int errval, int ax, int bp, int si)
{

	rect R, R2;
	char tbuf[512];
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

	PenColor(RED);
	PaintRect(&R);
	PenColor(WHITE);
	BackColor(RED);
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
   return 0;      /* never happens */
}
