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

#include "attract.h"



static rect *bR[40];
static rect cycleR[2];
static rect autocycleR[3];
static rect palR[4];
static rect monoR[2];
static rect doitR[3];
static rect soundR[2];
static rect stamperR[2];
static rect cursorshapeR[2];
static text_button ribbonlengthTB;
static rect ribbonlengthpmR[2];

static int ourspinflag;
static int ourgrayflag;
static int paled;
static int oursoundflag;
static int ourstamping;
static int ourcursorshape;
static int ourribbonlength;
static char *manual_auto[] = {"Manual", "Auto"};
static char *zoom_select[] = {"Zoom", "Select"};


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
		PushOrDoublePress(bR[n], inout, n - 11 == oursoundflag);
		break;
	case 13:
	case 14:
		PushOrDoublePress(bR[n], inout, n - 13 == ourstamping);
		break;
	case 15:
	case 16:
		PushOrDoublePress(bR[n], inout, n - 15 == ourcursorshape);
		break;
	case 17:
		InvertInsides((text_button *) bR[n]);
		break;
	case 22:
		ExtraHilite(bR[n], inout);
		/* deliberate fallthrough */
	default:
		PushButton(bR[n], inout);
	}

}


void _do_view_menu(void)
{

	int height = 10 * (3 * FontHeight / 2) + 2 * FontHeight + 12;
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

	int current_item;
	int doit = false;
	char *p;
	int items = 0;
	int savedgrayflag = grayflag;

	static int lefters[23] = {
		1, 0,
		4, 2, 3,
		8, 5, 6, 7,
		10, 9,
		12, 11,
		14, 13,
		16, 15,
		19, 17, 18,
		22, 20, 21
	};

	static int righters[23] = {
		1, 0,
		3, 4, 2,
		6, 7, 8, 5,
		10, 9,
		12, 11,
		14, 13,
		16, 15,
		18, 19, 17,
		21, 22, 20
	};

	static int uppers[23] = {
		20, 22,
		0, 0, 1,
		2, 2, 3, 4,
		7, 8,
		9, 10,
		11, 12,
		13, 14,
		15, 16, 16,
		17, 17, 18
	};

	static int downers[23] = {
		3, 4,
		5, 6, 8,
		9, 9, 9, 10,
		11, 12,
		13, 14,
		15, 16,
		17, 18,
		21, 22, 22,
		0, 0, 1
	};




	HideCursor();
	ProtectOff();
	//PushCursorType();
	//ArrowCursor();
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
	PenColor(BLACK);
	MoveTo(tR.Xmin + 1, row);
	LineTo(tR.Xmax - 1, row);

	row += FontHeight / 2;
	OffsetRect(&R, 0, 2 * FontHeight);
	JString("Sound: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, offon, soundR, 2, oursoundflag = soundflag);
	for (i = 0; i < 2; i++)
		bR[items++] = &soundR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	JString("Stamping: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, manual_auto, stamperR, 2, ourstamping = Stamping);
	for (i = 0; i < 2; i++)
		bR[items++] = &stamperR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	JString("Cursor: ", R.Xmin - 4, row + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, zoom_select, cursorshapeR, 2, ourcursorshape = cursorshape);
	for (i = 0; i < 2; i++)
		bR[items++] = &cursorshapeR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + width / 3;
	R.Xmax = tR.Xmax - width / 4;
	PaintNumberBoxBase(&R, &ribbonlengthTB, (double) ribbonlength, "Trace Length:  ", GS_INTEGER,
			   BLACK, LIGHTGRAY, DARKGRAY, WHITE);
	ourribbonlength = ribbonlength;
	bR[items++] = (rect *) & ribbonlengthTB;


	R.Xmin = R.Xmax + 8;
	R.Xmax = R.Xmin + 4 * StringWidthX;
	PlusMinusButtons(&R, ribbonlengthpmR);
	for (i = 0; i < 2; i++)
		bR[items++] = &ribbonlengthpmR[i];


	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;

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
				push(current_item, true);
				move_to_corner(bR[current_item]);
				break;
			}

			if (key == XF10)
				GifOutput("mandopt.gif", 1);

		}
		else
		{
			for (i = 0; i < items; i++)
			{
				current_item = -1;
				if (XYInRect(X, Y, bR[i]))
				{
					current_item = i;
					break;
				}
			}
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
			helptext("attview.hlp");
			LimitMouseRect(&tR);
		}

		if (key == 0x0d)
		{
			switch (current_item)
			{
			case 0:
				Jspinpalette();
				break;
			case 1:
				revJspinpalette();
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
				CheckRadioButtons(X, Y, monoR, 2, &ourgrayflag, offon);
				grayflag = ourgrayflag;

				grayscale();
				Jusepalette();
				break;
			case 11:
			case 12:
				CheckRadioButtons(X, Y, soundR, 2, &oursoundflag, offon);
				break;
			case 13:
			case 14:
				CheckRadioButtons(X, Y, stamperR, 2, &ourstamping, manual_auto);
				break;
			case 15:
			case 16:
				CheckRadioButtons(X, Y, cursorshapeR, 2, &ourcursorshape, zoom_select);
				break;
			case 17:
				{
					double z = ourribbonlength;
					int bot = 4;

					if (dimension == LORENZ && trihedroncount > 1 && tritracetype == 1)
						bot = 16;

					if (GetNumber(&ribbonlengthTB, &z, GS_UNSIGNED, bot, MAXRIBBONLENGTH))
						ourribbonlength = z;
					push(17, true);
					//PaintNumberBoxEntry(&ribbonlengthTB, ourribbonlength, GS_UNSIGNED);
				}
				break;
			case 18:
				ourribbonlength /= 2;
				if (ourribbonlength < 4)
					ourribbonlength = 4;
				if (dimension == LORENZ && trihedroncount > 1 && tritracetype == 1 &&
				    ourribbonlength < 16)
					ourribbonlength = 16;
				PaintNumberBoxEntry(&ribbonlengthTB, ourribbonlength, GS_UNSIGNED);
				break;
			case 19:
				ourribbonlength *= 2;
				if (ourribbonlength > MAXRIBBONLENGTH)
					ourribbonlength = MAXRIBBONLENGTH;
				PaintNumberBoxEntry(&ribbonlengthTB, ourribbonlength, GS_UNSIGNED);
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
		int imflag = 0;

		spinflag = ourspinflag;
		if (savedgrayflag != ourgrayflag)
			grayflag = ourgrayflag;
		else
			grayflag = savedgrayflag;
		grayscale();
		Jusepalette();

		if (ourcursorshape != cursorshape)
			cursorshape = ourcursorshape;

		if (ribbonlength != ourribbonlength)
		{
			ribbonindex = 0;
			ribbonfull = 0;
			trihedronon = 0;
			imflag = 1;
			ribbonlength = ourribbonlength;
		}
		if (soundflag != oursoundflag)
			soundflag = oursoundflag;

		if (Stamping != ourstamping)
			Stamping = ourstamping;


		if (imflag)
			installmode();


	}

	//PopCursorType();

}


void do_view_menu(void)
{

	do
	{
		paled = 0;
		_do_view_menu();
	} while (paled);
}
