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
#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "game.h"

char *wmsgs[] = {"Normalize", "Equalize", "F1 for HELP", "ESC to Cancel", "ACCEPT"};

#define NORMALIZE fBptr->n
#define EQUALIZE (NORMALIZE+1)
#define F1HELP (EQUALIZE+1)
#define ESCCANCEL (F1HELP+1)
#define ACCEPT (ESCCANCEL+1)

static rect *boxR[32];
rect doitRects[5];
static text_button weightTB[32];

static int current_item;


static double ourweights[MAXBARNMAPS];

static void push(int n,int inout)
{
   if (n != -1)
   {

      if (n < fBptr->n)
         InvertInsides(&weightTB[n]);
      else
      {
         PushButton(boxR[n],inout);
         if (n == ACCEPT)
            ExtraHilite(boxR[n],inout);
      }
   }
}
void draw_weight_entry(int current_item)
{
	char tbuf[128];
	rect R = *boxR[current_item];

   HideCursor();

	InsetRect(&R, 1, 1);
	PenColor(BUTTONBACK);
	PaintRect(&R);

	sprintf(tbuf, "% 4.3f", ourweights[current_item]);
	MoveTo(R.Xmin + 1, R.Ymin + 1);
	TextAlign(alignLeft, alignTop);
	PenColor(MENUTEXT);
	BackColor(BUTTONBACK);
	DrawString(tbuf);
   ShowCursor();
}

void draw_all_weight_entries(void)
{
	int i;

	HideCursor();
	for (i = 0; i < fBptr->n; i++)
		draw_weight_entry(i);
	ShowCursor();
}




void w_equalize(double *dest)
{
	int i;

	for (i = 0; i < fBptr->n; i++)
		dest[i] = 1.0 / fBptr->n;
}

void w_normalize(double *dest, double *source)
{
	int i;
	double td;

	for (td = 0.0, i = 0; i < fBptr->n; i++)
		td += source[i];
	if (td < .001)
		w_equalize(dest);
	else
		for (i = 0; i < fBptr->n; i++)
			dest[i] = source[i] / td;
}


void WeightEditor(void)
{
	/*
	 * This one is sneaky. There are fBptr->n plus-minus buttons, a
	 * Balance, a Help, a Cancel, an Accept key.
	 */


	int centerx, centery;
	int height;
	int width;
	rect tR;
	rect R;
	char tbuf[128];
	int i;
	int key = 0;
	int wait = 0;
	int items = fBptr->n;

   int u[40],d[40],l[40],r[40];

	int rows = (items + 1) / 2;

	int err;
   int oddcount;

	Centers(&sR, &centerx, &centery);

	height = rows * (FontHeight + 8) + FontHeight + 4 + (FontHeight + 8) * 2 + 12;
	width = 2 * sR.Xmax / 3;


	memcpy(ourweights, fBptr->weight, sizeof fBptr->weight);
	PushCursorPosition();

	HideCursor();

	invert_main_item(current_main_item);

	R.Xmin = centerx - width / 2;
	R.Xmax = R.Xmin + width;
	R.Ymin = centery - height / 2;
	R.Ymax = R.Ymin + height;

	tR = R;

	if (!ShadowAndSave(&tR))
	{
		ShowCursor();
		return;
	}



	PenColor(LIGHTGRAY);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	FrameRect(&R);





	TextAlign(alignCenter, alignTop);
	MoveTo(centerx, tR.Ymin + 1);
	PenColor(BLACK);
	BackColor(LIGHTGRAY);
	DrawString("Weight Editor");

	TextAlign(alignLeft, alignTop);

	PenColor(MENUTEXT);
	R.Xmin = tR.Xmin + 2;
	R.Xmax = tR.Xmax - 2;

	for (i = 0; i < items; i++)
	{
		R.Ymin = tR.Ymin + (i / 2 + 1) * (FontHeight + 8) + 3;
		R.Ymax = R.Ymin + FontHeight + 4;

		if ((i & 1) == 0)
		{
			R.Xmin = tR.Xmin + 4;
			R.Xmax = centerx - 4;
		}
		else
		{
			R.Xmax = tR.Xmax - 4;
			R.Xmin = centerx + 4;
		}

		sprintf(tbuf, "%2d: ", i + 1);
		PaintNumberBoxBase(&R, &weightTB[i], ourweights[i], tbuf, GS_FLOAT,
				   BLACK, LIGHTGRAY, DARKGRAY, WHITE);
		boxR[i] = (rect *) & weightTB[i];
	}
	TextAlign(alignCenter, alignMiddle);

	PenColor(BUTTONFRAME);

	MoveTo(tR.Xmin, boxR[items - 1]->Ymax + 3);
	LineTo(tR.Xmax, boxR[items - 1]->Ymax + 3);
	MoveTo(centerx, boxR[items - 1]->Ymax + 3);
	LineTo(centerx, boxR[0]->Ymin - 3);
	MoveTo(tR.Xmin, boxR[0]->Ymin - 3);
	LineTo(tR.Xmax, boxR[0]->Ymin - 3);


	R.Ymin = tR.Ymin + (rows + 1) * (FontHeight + 8) + 6;
	R.Ymax = R.Ymin + FontHeight + 4;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = centerx - 4;
	doitRects[0] = R;
	boxR[items] = &doitRects[0];
	PaintRadioButton(&R, false, false, wmsgs[0]);

	R.Xmin = centerx + 4;
	R.Xmax = tR.Xmax - 4;
	doitRects[1] = R;
	boxR[items + 1] = &doitRects[1];
	PaintRadioButton(&R, false, false, wmsgs[1]);

	R.Ymin += FontHeight + 10;
	R.Ymax = R.Ymin + FontHeight + 4;
	R.Xmin = tR.Xmin;
	R.Xmax = tR.Xmax;
	CreateRadioPanel(&R, &wmsgs[2], &doitRects[2], 3, -1);
	for (i = 0; i < 3; i++)
		boxR[items + 2 + i] = &doitRects[2 + i];
	ExtraHilite(boxR[items + 4], false);

   /* set up the uldr boxes */

   for(i=0;i<fBptr->n;i++)
   {
      if ((i & 1) == 0)
      {
         /* left ones */
         if (i == 0)
            u[i] = F1HELP;
         else
            u[i] = i - 2;

         if (i == fBptr->n)
            l[i] = r[i] = i;
         else
            l[i] = r[i] = i+1;

         if (i >= fBptr->n - 2)
            d[i] = NORMALIZE;
         else
            d[i] = i+2;
      }
      else
      {
         /* right ones */
         if (i == 1)
            u[i] = ACCEPT;
         else
            u[i] = i - 2;

         l[i] = r[i] = i-1;

         if (i >= fBptr->n - 1)
            d[i] = EQUALIZE;
         else
            d[i] = i+2;
      }
   }

   oddcount = (fBptr->n) & 1;

   if (oddcount)
   {
      u[NORMALIZE] = fBptr->n - 1;
      u[EQUALIZE] = fBptr->n - 2;
   }
   else
   {
      u[NORMALIZE] = fBptr->n - 2;
      u[EQUALIZE] = fBptr->n - 1;
   }
   d[NORMALIZE] = F1HELP;
   r[NORMALIZE] = l[NORMALIZE] = EQUALIZE;

   d[EQUALIZE] = ACCEPT;
   r[EQUALIZE] = l[EQUALIZE] = NORMALIZE;

   u[F1HELP] = NORMALIZE;
   r[F1HELP] = ESCCANCEL;
   l[F1HELP] = ACCEPT;
   d[F1HELP] = 0;

   u[ESCCANCEL]  = NORMALIZE;
   d[ESCCANCEL] = 0;
   l[ESCCANCEL] = F1HELP;
   r[ESCCANCEL] = ACCEPT;

   u[ACCEPT] = EQUALIZE;
   d[ACCEPT] = 1;
   l[ACCEPT] = ESCCANCEL;
   r[ACCEPT] = F1HELP;


	current_item = items + 4;
	LimitMouse(tR.Xmin, tR.Ymin, tR.Xmax, tR.Ymax);
	move_to_corner(boxR[current_item]);

	ShowCursor();

	while (1)
	{
		event e;

		int n = KeyEvent(false, &e);
		int button = (e.State & 0x700) >> 8;
		int X = e.CursorX;
		int Y = e.CursorY;

      int last_item = current_item;

		key = wait = 0;
		for (i = 0; i < items + 5; i++)
		{
         current_item = -1;
			if (XYInRect(X, Y, boxR[i]))
			{
				current_item = i;
				break;
			}
		}

		if (n)
		{

			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else
				key = e.ScanCode << 8;

			if (button == swRight)
				key = 0x1b;

			if (button == swLeft && current_item != -1)
			{
				wait = true;
				key = 0x0d;
			}

			if (key == 0x1b)
				break;
		}

		if (key == 0x0d)
		{
			if (current_item == ESCCANCEL)
			{
				key = 0x1b;
				break;
			}

			if (current_item == F1HELP)
				key = XF1;

			else if (current_item == ACCEPT)
				break;

			else if (current_item == NORMALIZE)
				key = 'N';

			else if (current_item == EQUALIZE)
				key = 'E';

         else if (current_item != -1)
         {
            double td = ourweights[current_item];
            int n;
   
            n = GetNumber(&weightTB[current_item],&td,GS_FLOAT,0.0,1.0);
            if (n)
               ourweights[current_item] = td;

   			draw_weight_entry(current_item);
            push(current_item,true);
         }
    
		}

      navigate(key,l,r,u,d,fBptr->n + 5,boxR,&current_item);

		switch (key)
		{

		case 'E':
		case 'e':
			/* equalize them easy! */
			PaintRadioButton(boxR[items + 1], true, true, wmsgs[1]);
			if (wait)
				WaitForNothing();

			w_equalize(ourweights);
			draw_all_weight_entries();
			PaintRadioButton(boxR[items + 1], false, false, wmsgs[1]);
			break;

		case 'N':
		case 'n':
			/* normalize em */
			PaintRadioButton(boxR[items], true, true, wmsgs[0]);
			if (wait)
				WaitForNothing();
			w_normalize(ourweights, ourweights);
			draw_all_weight_entries();
			PaintRadioButton(boxR[items], false, false, wmsgs[0]);
			break;



		case XF1:
			PaintRadioButton(boxR[items + 2], true, true, wmsgs[2]);
			if (wait)
				WaitForNothing();
			helptext("gamewght.hlp");
			LimitMouse(tR.Xmin, tR.Ymin, tR.Xmax, tR.Ymax);
			PaintRadioButton(boxR[items + 2], false, false, wmsgs[2]);
			break;

		}

      if (current_item != last_item)
      {
         push(last_item,false);
         push(current_item,true);
      }

	}

	if (key == 0x1b)
	{
		PaintRadioButton(boxR[items + 3], true, true, wmsgs[3]);
		if (wait)
			WaitForNothing();
		PaintRadioButton(boxR[items + 3], false, false, wmsgs[3]);
	}
	else if (key == 0x0d && current_item == ACCEPT)
	{
		PaintRadioButton(boxR[items + 4], true, true, wmsgs[4]);
		ExtraHilite(boxR[items + 4], true);
		if (wait)
			WaitForNothing();
		PaintRadioButton(boxR[items + 4], false, false, wmsgs[4]);
		ExtraHilite(boxR[items + 4], false);
	}


	HideCursor();
	PopRect(&err);

	PopCursorPosition();
	LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);

	if (key == 0x0d && current_item == ACCEPT)
	{
		PushBarnmap();
		w_normalize(fBptr->weight, ourweights);
      SetCurrentWeight();
		_activate(fBptr);
		ConvertParams(edmap);

		if (tweaking)
		{
			UpdateMapParam(7);
			ShowWeights();
			OutlineCurrentMap();
		}
		else if (triangle_editing_mode && triangle_display_mode)
			OutlineCurrentMap();
	}

	invert_main_item(current_main_item);
	ShowCursor();
}
