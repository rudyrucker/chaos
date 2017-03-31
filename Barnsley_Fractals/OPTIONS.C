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
#include <time.h>

#include "game.h"



static rect *bR[40];
static rect cycleR[2];
static rect autocycleR[3];
static rect palR[4];
static rect monoR[2];
static rect mapR[2];
static rect doitR[3];
static rect flockR[3];
static rect coloringR[9];
static rect traceR[2];

static int ourspinflag;
static int ourgrayflag;
static int ourcoloring_style;
static int ourbarnmapflag;
static int ourflocktype;
static int ourtrace;
static int paled;

static void push(int n, int inout)
{
   if (n == -1)
      return;

	switch (n)
	{
		case 2:
		case 3:
		case 4:
		PushOrDoublePress(bR[n], inout, n - 2 == ourspinflag);
		break;
	case 9:
	case 10:
		PushOrDoublePress(bR[n], inout, n - 9 == ourgrayflag);
		break;
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		PushOrDoublePress(bR[n], inout, n - 11 == ourcoloring_style);
		break;
	case 20:
	case 21:
		PushOrDoublePress(bR[n], inout, n - 20 == ourbarnmapflag);
		break;
	case 22:
	case 23:
	case 24:
		PushOrDoublePress(bR[n], inout, n - 22 == ourflocktype);
		break;
	case 25:
	case 26:
		PushOrDoublePress(bR[n], inout, n - 25 == ourtrace);
		break;

	case 29:
		ExtraHilite(bR[n], inout);
		/* deliberate fallthrough */
	default:
		PushButton(bR[n], inout);
	}

}


void _do_view_menu(void)
{

	int height = 11 * (3 * FontHeight / 2) + 2 * FontHeight + 12;
	int width = 2 * sR.Xmax / 3 + 20;
	rect tR;
	int cx, cy;
	rect R;
	int key;
	int i;
	int row;
	static char *ccmsgs[] = {"None", "Forward", "Reverse"};
	static char *palmsgs[] = {"Default", "Random", "Preset", "Edit"};
	static char *offon[] = {"Off", "On"};
	static char *doits[] = {"F1 for HELP", "ESC to Cancel", "ACCEPT"};
	static char *colorings[] = {"Mono", "One Map", "Pileup", "Two Map",
	"Average", "Sum", "Overlay", "Three Sum", "Product"};
	static char *coloringmsg = "Coloring: ";
	static char *flockers[] = {"Dots", "Rectangle", "Logo"};

	int current_item;
	int doit = false;
	char *p;
	int items = 0;
	int savedgrayflag = grayflag;

	static int lefters[30] = {
		1, 0,
		4, 2, 3,
		8, 5, 6, 7,
		10, 9,
		15, 11, 12, 13, 14,
		19, 16, 17, 18,
		21, 20,
		24, 22, 23,
      26, 25,
      29, 27, 28
	};

	static int righters[30] = {
		1, 0,
		3, 4, 2,
		6, 7, 8, 5,
		10, 9,
		12, 13, 14, 15, 11,
		17, 18, 19, 16,
		21, 20,
		23, 24, 22,
      26, 25,
		28, 29, 27
	};

	static int uppers[30] = {
		27, 29,
		0, 0, 1,
		2, 3, 3, 4,
		5, 7,
		9, 9, 9, 10, 10,
		11, 12, 14, 15,
		17, 18,
		20, 20, 21,
      23, 24,
		25, 25, 26
	};

	static int downers[30] = {
		3, 4,
		5, 6, 8,
		9, 9, 9, 10,
		11, 15,
		16, 17, 17, 18, 19,
		20, 20, 21, 21,
		22, 24,
		25, 25, 26,
      28, 29,
		0, 0, 1
	};




	HideCursor();
	ProtectOff();
	PushCursorType();
	ArrowCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Options Menu", BLACK);
	Centers(&tR, &cx, &cy);

	PenColor(BLACK);
	BackColor(LIGHTGRAY);
	MoveTo(tR.Xmin, tR.Ymin + FontHeight + 4);
	LineRel(width - 1, 0);

	row = tR.Ymin + FontHeight + 12;

	TextAlign(alignRight, alignTop);
	MoveTo(cx - 4, row + 2);
	DrawString("Color Cycle Once: ");
	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	CreateRadioPanel(&R, &ccmsgs[1], cycleR, 2, -1);
	bR[items++] = &cycleR[0];
	bR[items++] = &cycleR[1];

	row += 3 * FontHeight / 2;

	R.Xmin = tR.Xmin + width / 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;

	JString("Auto Cycle: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);

	CreateRadioPanel(&R, ccmsgs, autocycleR, 3, ourspinflag = spinflag);
	for (i = 0; i < 3; i++)
		bR[items++] = &autocycleR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);

	JString("Palette: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);

	CreateRadioPanel(&R, palmsgs, palR, 4, -1);
	for (i = 0; i < 4; i++)
		bR[items++] = &palR[i];

	OffsetRect(&R, 0, 3 * FontHeight / 2);
	row += 3 * FontHeight / 2;

	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 4;
	JString("Monochrome: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, offon, monoR, 2, ourgrayflag = grayflag);
	for (i = 0; i < 2; i++)
		bR[items++] = &monoR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + StringWidth(coloringmsg) + 6;
	R.Xmax = tR.Xmax - 4;
	JString(coloringmsg, R.Xmin - 2, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, colorings, coloringR, 5, ourcoloring_style = coloring_style);
	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	CreateRadioPanel(&R, colorings + 5, coloringR + 5, 4, ourcoloring_style - 5);
	for (i = 0; i < 9; i++)
		bR[items++] = &coloringR[i];

	row += 3 * FontHeight / 2;
	PenColor(BLACK);
	MoveTo(tR.Xmin + 1, row);
	LineTo(tR.Xmax - 1, row);

	row += FontHeight / 2;

	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 8;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;

	JString("Maps: ", cx - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, offon, mapR, 2, ourbarnmapflag = barnmapflag);
	for (i = 0; i < 2; i++)
		bR[items++] = &mapR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + width / 4;

	JString("Flock Type: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, flockers, flockR, 3, ourflocktype = flocktype);
	for (i = 0; i < 3; i++)
		bR[items++] = &flockR[i];


	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = cx + 4;
	JString("Trace: ", cx - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, offon, traceR, 2, ourtrace = !notrace);
	for (i = 0; i < 2; i++)
		bR[items++] = &traceR[i];


	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + 4;

	PenColor(BLACK);
	MoveTo(tR.Xmin + 1, row);
	LineTo(tR.Xmax - 1, row);

	row += FontHeight;
	OffsetRect(&R, 0, FontHeight);
	CreateRadioPanel(&R, doits, doitR, 3, -1);
	for (i = 0; i < 3; i++)
		bR[items++] = &doitR[i];

	ExtraHilite(bR[items - 1], false);





	LimitMouseRect(&tR);

	ShowCursor();

	current_item = items - 1;
	push(current_item, true);
	move_to_corner(bR[current_item]);
	while (1)
	{
		event e;
		int n = KeyEvent(false, &e);
		int X = e.CursorX;
		int Y = e.CursorY;
		int button = (e.State & 0x700) >> 8;
		int last_item = current_item;

		key = 0;
		for (i = 0; i < items; i++)
		{
			current_item = -1;
			if (XYInRect(X, Y, bR[i]))
			{
				current_item = i;
				break;
			}
		}

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else if (e.ScanCode != 0xff)
				key = e.ScanCode << 8;

			if (button == swLeft)
				key = 0x0d;

         if (key == 0x0d && current_item == items - 2)
            key = 0x1b;

			if (button == swRight)
				key = 0x1b;

			if (key == 0x1b)
         {
            current_item = items - 2;
   			push(last_item, false);
            push(current_item,true);
            move_to_corner(bR[current_item]);
				break;
         }

			if (key == XF10)
				GifOutput("mandopt.gif", 1);

		}

		if (key == 0x0d && current_item == items - 2)
			break;

		if (key == 0x0d && current_item == items - 1)
		{
			doit = true;
			break;
		}
		if (key == 0x0d && current_item == items - 3)
			key = XF1;

		if (key == XF1)
		{
			helptext("gameopts.hlp");
			LimitMouseRect(&tR);
		}

		if (key == 0x0d)
		{
			switch (current_item)
			{
			case 0:
				spinpalette();
				break;
			case 1:
				revspinpalette();
				break;
			case 2:
			case 3:
			case 4:
				CheckRadioButtons(X, Y, autocycleR, 3, &ourspinflag, ccmsgs);
				break;
			case 5:
				dodefaultpalette();
				break;
			case 6:
				randompalette();
				break;
			case 7:
				presetpalette();
				break;
			case 8:
				/* memory requires that we exit with this one */
				paled = true;
				break;
			case 9:
			case 10:
				CheckRadioButtons(X, Y, monoR, 3, &ourgrayflag, offon);
				grayflag = ourgrayflag;

				grayscale();
				usepalette();
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				CheckRadioButtons(X, Y, coloringR, 9, &ourcoloring_style, colorings);
				break;
			case 20:
			case 21:
				CheckRadioButtons(X, Y, mapR, 2, &ourbarnmapflag, offon);
				break;
			case 22:
			case 23:
			case 24:
				CheckRadioButtons(X, Y, flockR, 3, &ourflocktype, flockers);
				break;
			case 25:
			case 26:
				CheckRadioButtons(X, Y, traceR, 2, &ourtrace, offon);
				break;

			}
		}


		if (paled)
			break;


		navigate(key, lefters, righters, uppers, downers, items, bR, &current_item);






		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}

	}

	/* Wait while the button is down */
	if (doit)
		p = doits[2];
	else if (paled)
		p = palmsgs[3];
	else
		p = doits[1];

	PaintRadioButtonBase(bR[current_item], true, true, p, DARKGRAY, RED, WHITE);
	if (doit)
		ExtraHilite(bR[current_item], true);
	WaitForNothing();
	PaintRadioButtonBase(bR[current_item], false, false, p, DARKGRAY, RED, WHITE);
	if (doit)
		ExtraHilite(bR[current_item], false);

	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	LimitMouseRect(&sR);
	ShowCursor();

	if (paled)
	{
		palette_tweaker();
		LimitMouseRect(&sR);
	}




	if (doit)
	{
		int uvflag = false;

		spinflag = ourspinflag;

		if (savedgrayflag != ourgrayflag)
			grayflag = ourgrayflag;
		else
			grayflag = savedgrayflag;
		grayscale();
		usepalette();

		if (ourbarnmapflag != barnmapflag)
		{
			barnmapflag = ourbarnmapflag;
			uvflag = 1;
		}

		if (ourflocktype != flocktype)
		{
			flocktype = ourflocktype;
			uvflag = 1;
		}
		if (ourcoloring_style != coloring_style)
		{
			coloring_style = ourcoloring_style;
			uvflag = 1;
		}
		if (ourtrace == notrace)
		{
			notrace = !ourtrace;
			uvflag = 1;
		}

		if (uvflag)
			install_fill_outline();

	}

	PopCursorType();

}


void do_view_menu(void)
{

	do
	{
		paled = 0;
		_do_view_menu();
	} while (paled);
}
