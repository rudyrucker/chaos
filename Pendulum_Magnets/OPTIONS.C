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



static rect *bR[40];
static rect cycleR[2];
static rect autocycleR[3];
static rect palR[4];
static rect monoR[2];
static rect doitR[3];

static rect soundR[2];
static rect basinR[2];
static rect tracetypeR[3];
static rect eraseR[2];
static rect forcetypeR[2];
static int our_forcetype;
static rect rkR[2];
static int our_rkflag;
static rect reverseR[2];
static int our_reversibleflag;

static numbertool tracelenNT;
static int our_tracelen;

static numbertool circleradNT;
static double our_radius;



static int ourspinflag;
static int ourgrayflag;
static int items;
static int our_basinflag;
static int our_soundflag;
static double our_radius;
static int our_tracetype;
static int our_eraseflag;

static int paled;


static void push(int n, int inout)
{
   rect *R = bR[n];

   if (n == -1)
      return;


   if (n == items - 1)
		ExtraHilite(bR[n], inout);

	switch (n)
	{
	case 2:
	case 3:
	case 4:
		PushOrDoublePress(R, inout, n - 2 == ourspinflag);
		break;
	case 9:
	case 10:
		PushOrDoublePress(R, inout, n - 9 == ourgrayflag);
		break;
   case 11:
   case 12:
      PushOrDoublePress(R, inout, n - 11 == our_soundflag);
      break;
   case 13:
   case 14:
      PushOrDoublePress(R, inout, n - 13 == our_basinflag);
      break;
   case 15:
   case 16:
      PushOrDoublePress(R,inout,n-15 == our_reversibleflag);
      break;
   case 17:
   case 18:
      PushOrDoublePress(R,inout,n-17 == our_forcetype);
      break;
   case 19:
   case 20:
      PushOrDoublePress(R,inout,n-19 == our_rkflag);
      break;
   case 21:
   case 22:
   case 23:
      PushOrDoublePress(R, inout, n - 21 == our_tracetype);
      break;
   case 24:
   case 25:
      PushOrDoublePress(bR[n], inout, n - 24 == our_eraseflag);
      break;
   case 26:
      InvertInsides(&tracelenNT.TB);
      break;
   case 29:
      InvertInsides(&circleradNT.TB);
      break;
	default:
		PushButton(bR[n], inout);
      break;
	}

}


static void _do_view_menu(void)
{

	int height = 14 * (3 * FontHeight / 2) + 2 * FontHeight + 16;
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
	static char *noyes[] = {"No","Yes"};
	static char *doits[] = {"F1 for HELP", "ESC to Cancel", "ACCEPT"};
   static char *tracetypemsgs[] = {"None","Lines Only","Lines & Bobs"};
   static char *forcetypemsgs[] = {"Inverse Square","Inverse Linear"};
   static char *comptypemsgs[] = {"Lively","Physical"};
  

	int current_item;
	int doit = false;
	char *p;
	int savedgrayflag = grayflag;
   int start_tracetype;
   int start_eraseflag;
   double d;

	static int lefters[] = {
      1,0,
      4,2,3,
      8,5,6,7,
      10,9,
      12,11,
      14,13,
      16,15,
      18,17,
      20,19,
      23,21,22,
      25,24,
      28,26,27,
      31,29,30,
      34,32,33
	};

	static int righters[] = {
		1, 0,
		3, 4, 2,
		6, 7, 8, 5,
		10, 9,
      12,11,
      14,13,
      16,15,
      18,17,
      20,19,
      22,23,21,
      25,24,
      27,28,26,
      30,31,29,
      33,34,32
	};

	static int uppers[] = {
      32,34,
      0,0,1,
      2,2,3,4,
      7,8,
      9,10,
      11,12,
      13,14,
      15,16,
      17,18,
      19,19,20,
      22,23,
      24,25,25,
      27,27,28,
      29,29,30
	};

	static int downers[] = {
		3, 4,
		5, 6, 8,
		9, 9, 9, 10,
      11,12,
      13,14,
      15,16,
      17,18,
      19,20,
      21,23,
      24,24,25,
      26,26,
      29,30,31,
      32,34,34,
      0,0,1,
	};

	items = 0;

   if (tracetype > 2)
      our_eraseflag = true;
   else
      our_eraseflag = false;
   if (tracetype == 0)
      our_tracetype = 0;
   else
      our_tracetype = 1 + (tracetype - 1) % 2;

   start_tracetype = our_tracetype;
   start_eraseflag = our_eraseflag;



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
   CreateRadioPanelLabel(&R,offon,soundR,2,our_soundflag = soundflag,"Sound: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &soundR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,offon,basinR,2,our_basinflag = basinflag,"Basins: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &basinR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,noyes,reverseR,2,our_reversibleflag = reversibleflag,"Reversible: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &reverseR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   i = R.Xmin;
   R.Xmin = tR.Xmin + (tR.Xmax-tR.Xmin)/4;
   CreateRadioPanelLabel(&R,forcetypemsgs,forcetypeR,2,our_forcetype = forcetype,"Force Type: ",BLACK,LIGHTGRAY);
   R.Xmin = i;
   for(i=0;i<2;i++)
      bR[items++] = &forcetypeR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   i = R.Xmin;
   R.Xmin = tR.Xmin + (tR.Xmax-tR.Xmin)/4;
   CreateRadioPanelLabel(&R,comptypemsgs,rkR,2,our_rkflag = rkflag,"Computation: ",BLACK,LIGHTGRAY);
   R.Xmin = i;
   for(i=0;i<2;i++)
      bR[items++] = &rkR[i];

   row += 3*FontHeight/2;
   i = R.Xmin;
   R.Xmin = tR.Xmin + (tR.Xmax-tR.Xmin)/4;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,tracetypemsgs,tracetypeR,3,our_tracetype,"Trace: ",BLACK,LIGHTGRAY);
   R.Xmin = i;
   for(i=0;i<3;i++)
      bR[items++] = &tracetypeR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
	R.Xmin = cx + 4;
   R.Xmax = tR.Xmax - 4;
   CreateRadioPanelLabel(&R,offon,eraseR,2,our_eraseflag,"Trace Erase: ",BLACK,LIGHTGRAY);
   for(i=0;i<2;i++)
      bR[items++] = &eraseR[i];


   OffsetRect(&R,0,3*FontHeight/2);
   row += 3*FontHeight/2;
   R.Xmin = cx+4;
   R.Xmax = R.Xmin + width/4;
   tracelenNT.tR = R;
   tracelenNT.value = our_tracelen = memorylength;
   tracelenNT.type = GS_UNSIGNED;
   CreateNumberToolTitle(&tracelenNT,"Length: ",BLACK,LIGHTGRAY);
   bR[items++] = &tracelenNT.TB.nR;
   bR[items++] = &tracelenNT.mR;
   bR[items++] = &tracelenNT.pR;

   OffsetRect(&R,0,3*FontHeight/2);
   row += 3*FontHeight/2;
   R.Xmin = cx+4;
   R.Xmax = R.Xmin + width/4;

   circleradNT.tR = R;
   circleradNT.value = our_radius = radius;
   circleradNT.type = GS_FLOAT;
   circleradNT.p1 = 4;
   circleradNT.p2 = 2;
   CreateNumberToolTitle(&circleradNT,"Circle Radius: ",BLACK,LIGHTGRAY);
   bR[items++] = &circleradNT.TB.nR;
   bR[items++] = &circleradNT.mR;
   bR[items++] = &circleradNT.pR;

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

      if (key == XALTW)
         InfoBox();

		if (key == XF1)
		{
			helptext("magopts.hlp");
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
            CheckRadioButtons(X,Y,soundR,2,&our_soundflag,offon);
            break;
         case 13:
         case 14:
            CheckRadioButtons(X,Y,basinR,2,&our_basinflag,offon);
            break;
         case 15:
         case 16:
            CheckRadioButtons(X,Y,reverseR,2,&our_reversibleflag,noyes);
            break;
         case 17:
         case 18:
            CheckRadioButtons(X,Y,forcetypeR,2,&our_forcetype,forcetypemsgs);
            break;
         case 19:
         case 20:
            CheckRadioButtons(X,Y,rkR,2,&our_rkflag,comptypemsgs);
            break;
         case 21:
         case 22:
         case 23:
            CheckRadioButtons(X,Y,tracetypeR,3,&our_tracetype,tracetypemsgs);
            break;
         case 24:
         case 25:
            CheckRadioButtons(X,Y,eraseR,3,&our_eraseflag,offon);
            break;
         case 26:
            d = our_tracelen;
            if (GetNumber(&tracelenNT.TB,&d,GS_UNSIGNED,2,512))
               our_tracelen = d;
            push(current_item,true);
            break;
         case 27:
            if (our_tracelen > 2)
               our_tracelen--;
            PaintNumberBoxEntry(&tracelenNT.TB,our_tracelen,GS_UNSIGNED);
            break;
         case 28:
            if (our_tracelen < 512)
               our_tracelen++;
            PaintNumberBoxEntry(&tracelenNT.TB,our_tracelen,GS_UNSIGNED);
            break;
         case 29:
            d = our_radius;
            if (GetNumber(&circleradNT.TB,&d,GS_UNSIGNED,1,135))
               our_radius = d;
            push(current_item,true);
            break;
         case 30:
            our_radius /= 1.5;
            if (our_radius < 1.0)
               our_radius = 1.0;
            PaintNumberBoxEntry(&circleradNT.TB,our_radius,GS_FLOAT);
            break;
         case 31:
            our_radius *= 1.5;
            if (our_radius > 135)
               our_radius = 135;
            PaintNumberBoxEntry(&circleradNT.TB,our_radius,GS_FLOAT);
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
      reversibleflag = our_reversibleflag;
      rkflag = our_rkflag;
      if (rkflag)
      {
         reversibleflag = 0;
         fspeedx = speedx;
         fspeedy = speedy;
      }

      forcetype = our_forcetype;


      if (our_tracetype != start_tracetype || our_eraseflag != start_eraseflag)
      {
         tracetype = our_tracetype;
         if (tracetype && our_eraseflag)
            tracetype += 2;
			resetLength();
         if (CursorShowing)
         {
            CursorShowing = false;
            MoveCursor(ballx+minx,bally);
         }
      }
      if (our_basinflag != basinflag)
      {
         basinflag = our_basinflag;
            /* Yes it looks odd, but we need to do it so it can
               be erased later. */
         if (CursorShowing)
            ShowCursor();
         MasterReset();
         if (basinflag)
         {
            firsttimeflag = 0;
            showball(ballx,bally,maxcolor);
         }
      }

      if (our_radius != radius)
      {
         radius = our_radius;
         NewRadius();
         if (CursorShowing)
         {
            CursorShowing = false;
            MoveCursor(ballx+minx,bally);
         }
      }

      if (our_tracelen != memorylength)
      {
         memorylength = our_tracelen;
         resetLength();
         if (CursorShowing)
         {
            CursorShowing = false;
            MoveCursor(ballx+minx,bally);
         }
      }

      soundflag = our_soundflag;
      spinflag = ourspinflag;
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
      paled = false;
		_do_view_menu();
	} while (paled);

   return 0;
}
