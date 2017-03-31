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
#include <alloc.h>
#include <time.h>
#include <ctype.h>
#include <sys\stat.h>
#include <string.h>
#include <mem.h>
#include <time.h>

#include "toy.h"



static rect *bR[25];
static rect cycleR[2];
static rect autocycleR[3];
static rect palR[4];
static rect monoR[2];
static rect doitR[3];
static rect gridR[2];
static rect fastR[2];
static rect seaR[2];
static rect wrapR[2];
static rect changeR[2];
static rect loadsaveR;

static int ourspinflag;
static int ourgrayflag;
static int ourskipper;
static int ourfastflag;
static int ourseatype;
static int ourwrapflag;
static int ourchangeflag;
static int paled;
static int items;
static int loadshape;

static void push(int n, int inout)
{
   if (n == -1)
      return;

   if (n == items - 1)
		ExtraHilite(bR[n], inout);

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
		PushOrDoublePress(bR[n], inout, n - 11 == ourskipper);
		break;
   case 13:
   case 14:
		PushOrDoublePress(bR[n], inout, n - 13 == ourfastflag);
		break;
   case 15:
   case 16:
		PushOrDoublePress(bR[n], inout, n - 15 == ourseatype);
		break;
   case 17:
   case 18:
		PushOrDoublePress(bR[n], inout, n - 17 == ourwrapflag);
		break;
   case 19:
   case 20:
		PushOrDoublePress(bR[n], inout, n - 19 == ourchangeflag);
		break;

	default:
		PushButton(bR[n], inout);
	}

}


static void _do_view_menu(void)
{

	int height = 11 * (3 * FontHeight / 2) + 2 * FontHeight + 16;
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
   static char *seamsgs[] = {"Blank","Noisy"};
   static char *changemsgs[] = {"Instant","Delayed"};
   static char *loadsavemsg = "Load Shape";
	int current_item;
	int doit = false;
	char *p;
	int savedgrayflag = grayflag;

	static int lefters[25] = {
      1,0,
      4,2,3,
      8,5,6,7,
      10,9,
      12,11,
      14,13,
      16,15,
      18,17,
      20,19,
      21,
      24,22,23
	};

	static int righters[25] = {
		1, 0,
		3, 4, 2,
		6, 7, 8, 5,
		10, 9,
      12,11,
      14,13,
      16,15,
      18,17,
      20,19,
      21,
      23,24,22
	};

	static int uppers[25] = {
      22,24,
      0,0,1,
      2,2,3,4,
      7,8,
      9,10,
      11,12,
      13,14,
      15,16,
      17,18,
      19,
      21,21,21
	};

	static int downers[25] = {
		3, 4,
		5, 6, 8,
		9, 9, 9, 10,
      11,12,
      13,14,
      15,16,
      17,18,
      19,20,
      21,21,
      23,
      0,0,1
	};

	items = 0;



	HideCursor();
	ProtectOff();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Options Menu", BLACK);
	Centers(&tR, &cx, &cy);

	PenColor(BLACK);
	BackColor(LIGHTGRAY);
	MoveTo(tR.Xmin, tR.Ymin + FontHeight + 4);
	LineRel(width - 1, 0);

	row = tR.Ymin + FontHeight + 12;

	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	CreateRadioPanelLabel(&R, &ccmsgs[1], cycleR, 2, -1,"Color Cycle Once: ",BLACK,LIGHTGRAY);
	bR[items++] = &cycleR[0];
	bR[items++] = &cycleR[1];

	row += 3 * FontHeight / 2;

	R.Xmin = tR.Xmin + width / 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	CreateRadioPanelLabel(&R, ccmsgs, autocycleR, 3, ourspinflag = spinflag,"Auto Cycle: ",BLACK,LIGHTGRAY);
	for (i = 0; i < 3; i++)
		bR[items++] = &autocycleR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);

	CreateRadioPanelLabel(&R, palmsgs, palR, 4, -1,"Palette: ",BLACK,LIGHTGRAY);
	for (i = 0; i < 4; i++)
		bR[items++] = &palR[i];

	OffsetRect(&R, 0, 3 * FontHeight / 2);
	row += 3 * FontHeight / 2;

	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 4;
	CreateRadioPanelLabel(&R, offon, monoR, 2, ourgrayflag = grayflag,"Monochrome: ",BLACK,LIGHTGRAY);
	for (i = 0; i < 2; i++)
		bR[items++] = &monoR[i];

	row += 3 * FontHeight / 2;
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	R.Xmin = tR.Xmin + 4;
   R.Xmax = tR.Xmax - 4;

	PenColor(BLACK);
	MoveTo(tR.Xmin + 1, row);
	LineTo(tR.Xmax - 1, row);

   row += FontHeight/2;
   OffsetRect(&R,0,FontHeight/2);
	R.Xmin = cx + 4;
   CreateRadioPanelLabel(&R,offon,gridR,2,ourskipper = 1-skipper,"Grid Lines: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &gridR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,offon,fastR,2,ourfastflag = fastflag,"Fast Mode: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &fastR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,seamsgs,seaR,2,ourseatype = seatype,"Sea Type: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &seaR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,offon,wrapR,2,ourwrapflag = wrapflag,"Wrap: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &wrapR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,changemsgs,changeR,2,ourchangeflag = changeflag,"Changes: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &changeR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = cx - 6*StringWidthX;
   R.Xmax = R.Xmin + 12 * StringWidthX;
   PaintRadioButton(&R,false,false,loadsavemsg);
   loadsaveR = R;
   bR[items++] = &loadsaveR;



	row += 2*FontHeight;
	OffsetRect(&R, 0, 2*FontHeight);
   R.Xmin = tR.Xmin + 4;
   R.Xmax = tR.Xmax - 4;
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
   	current_item = -1;
		for (i = 0; i < items; i++)
		{
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

      if (key == XALTW)
         InfoBox();

		if (key == XF1)
		{
			helptext("toyopts.hlp");
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
            setdefaultpalette();
				break;
			case 6:
            randompalette();
				break;
			case 7:
            changepalette();
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
				usepalette();
				break;
         case 11:
         case 12:
				CheckRadioButtons(X, Y, gridR, 2, &ourskipper, offon);
            break;
         case 13:
         case 14:
				CheckRadioButtons(X, Y, fastR, 2, &ourfastflag, offon);
            break;
         case 15:
         case 16:
				CheckRadioButtons(X, Y, seaR, 2, &ourseatype, seamsgs);
            break;
         case 17:
         case 18:
				CheckRadioButtons(X, Y, wrapR, 2, &ourwrapflag, offon);
            break;
         case 19:
         case 20:
				CheckRadioButtons(X, Y, changeR, 2, &ourchangeflag, changemsgs);
            break;
         case 21:
            loadshape = true;
            break;
         }
		}


		if (paled || loadshape)
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
	else if (loadshape)
      p = loadsavemsg;
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
      spinflag = ourspinflag;
      skipper = 1-ourskipper;
      fastflag = ourfastflag;
      changeflag = ourchangeflag;
      wrapflag = ourwrapflag;
      seatype = ourseatype;
      grayflag = ourgrayflag;
	}
   else
      grayflag = savedgrayflag;

	grayscale();
	usepalette();

}


int options_panel(void)
{

	do
	{
      loadshape = paled = false;
		_do_view_menu();
	} while (paled);
   if (loadshape)
      ShapeLoader();

   return 0;
}
