#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>


#include "mag.h"

#define NOFLOATS
#define NOTINIES
#define NONUMBERS
#define NOFRAME
#define NOTITLE

#pragma argsused
void reposition_slider_v_base(slider * s, int selected, int floater, int numbers)
{
	/* reposition the slider based on value v */
	double range;
	double foffset;
	int offset;
	double barwidth = s->sR.Xmax - s->sR.Xmin;
	point p;
	rect R;

	s->value = max(s->value, s->min);
	s->value = min(s->value, s->max);
#ifndef NOFLOATS
	if (floater)
	{
		long t;

		if (s->value > 0)
			t = (long) ((s->value + .00005) * 1000);
		else
			t = (long) ((s->value - .00005) * 1000);

		s->value = t / 1000.0;
	}
#endif

	if (s->value == s->old_value)
		return;


	range = s->max - s->min;
	foffset = s->value - s->min;
	offset = foffset / range * barwidth;




	/* Screw this XOR shit, just slide the whole box. */
	BackColor(selected == 2 ? ~DARKGRAY : DARKGRAY);

	p.X = s->sR.Xmin + offset;
	p.X = max(s->sR.Xmin + 5, p.X);
	p.X = min(s->sR.Xmax - 5, p.X);

	R.Xmin = p.X - s->bubblewidth / 2;
	R.Xmax = p.X + s->bubblewidth / 2 - 1;
	R.Ymin = s->sR.Ymin + 2;
	R.Ymax = s->sR.Ymax - 2;
	while (R.Xmin < s->sR.Xmin + 2)
		OffsetRect(&R, 1, 0);
	while (R.Xmax > s->sR.Xmax - 2)
		OffsetRect(&R, -1, 0);


	if (R.Xmin != s->bR.Xmin)
	{
		rect R1 = s->sR;

		ProtectRect(&R1);
		InsetRect(&R1, 1, 1);
		ClipRect(&R1);
		ScrollRect(&R1, -(s->bR.Xmin - R.Xmin), 0);
		ClipRect(&sR);
		ProtectOff();
	}
	s->bR = R;
	/* Now paint the new number in */
#ifndef NONUMBERS
	if (numbers)
	{
		PaintNumberBoxEntry(&s->TB, s->value, (floater) ? GS_FLOAT : GS_UNSIGNED);
	}
#endif
	s->old_value = s->value;

}


void reposition_slider_v(slider * s, int selected)
{
	reposition_slider_v_base(s, selected, true, true);
}

void reposition_slider_X_base(slider * s, int X, int selected, int floater, int numbers)
{
	/*
	 * Figure out the new value based upon X, and then move the fucker.
	 */
	double range;
	int offset;
	double barwidth = s->sR.Xmax - s->sR.Xmin;


	range = s->max - s->min;
	offset = (double) X - s->sR.Xmin;
	s->value = s->min + offset / barwidth * range;
	reposition_slider_v_base(s, selected, floater, numbers);
}

void reposition_slider_X(slider * s, int X, int selected)
{
	reposition_slider_X_base(s, X, selected, true, true);
}




int slider_height(slider * s)
{
	int height;

	height = 2;
	if (s->title)
		height += FontHeight + 4;

	height += 4 * FontHeight / 5;
	height += FontHeight / 2 + 12;

	return height;
}

static void dash_or_plus(int sx, int sy, int width, int plus)
{
	MoveTo(sx, sy);
	LineRel(width - 1, 0);
	if (plus)
	{
		MoveTo(sx + width / 2, sy - width / 2);
		LineRel(0, width - 1);
	}
}

#pragma argsused
void create_slider_base_width(slider * s, rect * mR, int x, int y, int floater,
			       int tinies, int frame, int numbers, int width)
{


	rect R;
	int row;
	double range, foffset;
	int offset;
	point p;
	int zwidth;
	int zepps;
	int i;

	R.Xmin = mR->Xmin + x;
	R.Xmax = mR->Xmax - x;
	R.Ymin = mR->Ymin + y;

	R.Ymax = R.Ymin + slider_height(s);

	s->tR = R;
	s->bubblewidth = width;

#ifndef NOFRAME
	/* Box the whole thing */
	if (frame)
	{
		PenColor(BUTTONFRAME);
		FrameRect(&R);
	}
#endif

	row = R.Ymin + 2;
#ifndef NOTITLE

	if (s->title)
	{
		int cx;

		cx = R.Xmin + (R.Xmax - R.Xmin) / 2;
		PenColor(BLACK);
		BackColor(MENUBACK);
		TextAlign(alignCenter, alignTop);
		MoveTo(cx, row);
		DrawString(s->title);
	}
#else
	row += FontHeight + 4;
#endif

#ifndef NONUMBERS
	if (numbers)
	{
		int w = 8 * 10;

		R.Xmin = s->tR.Xmin + 4;
		R.Xmax = R.Xmin + w - 1;
		R.Ymin = row - FontHeight / 4;
		R.Ymax = R.Ymin + FontHeight + 4;
		s->TB.nR = R;
		PaintNumberBoxEntry(&s->TB, s->value, (floater) ? GS_FLOAT : GS_UNSIGNED);

		s->sR.Xmin = R.Xmax + 8;
	}
	else
	{
		s->sR.Xmin = s->tR.Xmin + 2;
	}
#else
	s->sR.Xmin = s->tR.Xmin + 2;
#endif

	s->sR.Xmax = s->tR.Xmax - 2;
	s->sR.Ymin = row;
	s->sR.Ymax = row + (4 * FontHeight) / 5;


	PenColor(DARKGRAY);
	PaintRect(&s->sR);
	PushButton(&s->sR, true);


	/*
	 * Now figure out where we want the slider piece to be. We XOR the
	 * thing into place, to make it easier to move it. And we just paint
	 * it by drawing a 10-wide vertical line.
	 */


	range = s->max - s->min;
	foffset = s->value - s->min;

	offset = foffset / range * (s->sR.Xmax - s->sR.Xmin);

	p.X = s->sR.Xmin + offset;
	p.X = max(s->sR.Xmin + 5, p.X);
	p.X = min(s->sR.Xmax - 5, p.X);

	R.Xmin = p.X - width / 2;
	R.Xmax = p.X + width / 2 - 1;
	R.Ymin = s->sR.Ymin + 2;
	R.Ymax = s->sR.Ymax - 2;
	while (R.Xmin < s->sR.Xmin + 2)
		OffsetRect(&R, 1, 0);
	while (R.Xmax > s->sR.Xmax - 2)
		OffsetRect(&R, -1, 0);

	s->bR = R;


	PenColor(LIGHTGRAY);
	PaintRect(&s->bR);
	PushButton(&s->bR, false);


	/* Now position and paint the value box. */


	s->old_value = s->value;

	/* create the zed box */

#ifdef NOTINIES
	if (tinies)
	{
		int cx, cy;

		Centers(&s->sR, &cx, &cy);

		zwidth = (s->sR.Xmax - s->sR.Xmin - 10) / 6;
		row = s->tR.Ymax - FontHeight / 2 - 4;
		for (i = 0; i < 3; i++)
		{
			rect R1, R2;

			R1.Xmin = cx + i * zwidth + 2 * (i + 1);
			R1.Xmax = R1.Xmin + zwidth - 1;
			R1.Ymin = row;
			R1.Ymax = row + FontHeight / 2;

			R2.Xmax = cx - i * zwidth - 2 * (i + 1);
			R2.Xmin = R2.Xmax - zwidth + 1;
			R2.Ymin = row;
			R2.Ymax = row + FontHeight / 2;

			s->zR[3 + i] = R1;
			s->zR[2 - i] = R2;
		}


		zepps = -3;
		for (i = 0; i < 6; i++)
		{
			rect R;
			int cy;
			int zwidth3 = zwidth / 3;
			int zstart;
			int dashwidth = zwidth3 - 4;


			R = s->zR[i];
			dashwidth = min(dashwidth, s->zR[i].Ymax - s->zR[i].Ymin - 4);

			Centers(&R, &cx, &cy);
			PenColor(8);
			PaintRect(&R);
			PushButton(&R, false);
			PenColor(7);

			switch (zepps)
			{
			case -3:
			case 3:
				zstart = cx - (3 * dashwidth / 2 + 4);
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 3);

				zstart = cx - dashwidth / 2;
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 3);

				zstart = cx + dashwidth / 2 + 4;
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 3);

				break;
			case -2:
			case 2:
				zstart = R.Xmin + zwidth / 2 - dashwidth - 2;
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 2);

				zstart = R.Xmin + zwidth / 2 + 2;
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 2);
				break;

			case -1:
			case 1:

				zstart = R.Xmin + zwidth / 2 - dashwidth / 2;
				dash_or_plus(zstart, cy + 1, dashwidth, zepps == 1);
				break;

			}
			zepps++;
			if (zepps == 0)
				zepps++;
		}
	}
#endif

}

void create_slider_base(slider * s, rect * mR, int x, int y, int floater,
			 int tinies, int frame, int numbers)
{
	create_slider_base_width(s, mR, x, y, floater, tinies, frame, numbers, 10);
}

void create_slider(slider * s, rect * mR, int x, int y)
{
	/*
	 * Create a slider. Mother rect is mR, x and y positions are relative
	 * to R.
	 */

	create_slider_base(s, mR, x, y, true, true, true, true);
}

#ifndef NOTINIES
static double slider_increments[] = {-.1, -.01, -.001, .001, .01, .1};
int ProcessSlider(slider * s, int current_item)
{
	double d = s->value;
	int i;

	if (current_item == 1)
	{
		if (GetNumber(&s->TB, &d, GS_FLOAT, s->min, s->max))
		{
			s->value = d;
			reposition_slider_v(s, false);
		}
		return 2;
	}
	if (current_item == 0)
		return 0;

	for (i = 0; i < 6; i++)
	{
		if (current_item - 2 == i)
		{
			s->value += slider_increments[i];
			reposition_slider_v(s, false);
			return 1;
		}
	}
	return 0;
}

#endif
