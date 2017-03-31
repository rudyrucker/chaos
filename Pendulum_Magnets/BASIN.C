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

#include "mag.h"

static int our_mode;
static rect modeR[2];
static numbertool limitNT;
static long our_limit;
static int items;
static rect *bR[10];
static rect doitR[3];

static void push(int n,int inout)
{
   rect *R = bR[n];
   if (n == -1)
      return;

   if (n == items - 1)
		ExtraHilite(R, inout);

   switch(n)
   {
   case 0:
   case 1:
		PushOrDoublePress(R, inout, n == our_mode);
      break;
   case 2:
      InvertInsides(&limitNT.TB);
      break;
   default:
      PushButton(R,inout);
      break;
   }
}



      

void basin_menu(void)
{
	int height = 3 * (3 * FontHeight / 2) + 2 * FontHeight + 16;
	int width = 2*sR.Xmax / 3;
   rect tR;
   int cx,cy;
   rect R;
   int key,i,row;
	static char *doits[] = {"F1 for HELP", "ESC to Cancel", "ACCEPT"};
   static char *modes[] = {"Magnet","Time"};
   int current_item;
   int doit=false;
   double d;
   char *p;

   static int lefters[] = {1,0,4,2,3,7,5,6};
   static int righters[] = {1,0,3,4,2,6,7,5};
   static int uppers[] = {5,7,0,1,1,2,2,3};
   static int downers[] = {2,3,5,7,7,0,0,1};


   our_mode = basindisplaymode;
   items = 0;
   HideCursor();
   ProtectOff();
   PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Basins Menu", BLACK);
	Centers(&tR, &cx, &cy);

	row = tR.Ymin + FontHeight + 8;
	R.Xmin = cx + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	CreateRadioPanelLabel(&R, modes, modeR, 2, our_mode = basindisplaymode,"Display Mode: ",BLACK,LIGHTGRAY);
	bR[items++] = &modeR[0];
	bR[items++] = &modeR[1];

	row += 3 * FontHeight / 2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = cx+4;
   R.Xmax = R.Xmin + width/4;
   limitNT.tR = R;
   limitNT.value = our_limit = basinlimit;
   limitNT.type = GS_UNSIGNED;
   CreateNumberToolTitle(&limitNT,"Limit: ",BLACK,LIGHTGRAY);
   bR[items++] = &limitNT.TB.nR;
   bR[items++] = &limitNT.mR;
   bR[items++] = &limitNT.pR;


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
//			helptext("magbasin.hlp");
//			LimitMouseRect(&tR);
		}

		if (key == 0x0d)
		{
			switch (current_item)
			{
         case 0:
         case 1:
				CheckRadioButtons(X, Y, modeR, 2, &our_mode, modes);
				break;
         case 2:
            d = our_limit;
            if (GetNumber(&limitNT.TB,&d,GS_UNSIGNED,0,32767))
               our_limit = d;
            push(current_item,true);
            break;
         case 3:
            if (our_limit > 1)
               our_limit /= 2;
            PaintNumberBoxEntry(&limitNT.TB,our_limit,GS_UNSIGNED);
            break;
         case 4:
            if (our_limit < 0x4000)
               our_limit *= 2;
            PaintNumberBoxEntry(&limitNT.TB,our_limit,GS_UNSIGNED);
            break;
         }
		}

		navigate(key, lefters, righters, uppers, downers, items, bR, &current_item);

		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}

	}

   p = doits[doit ? 3 : 2];

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

   if (doit)
   {
      basinlimit = our_limit;
      basindisplaymode = our_mode;
   }
}


