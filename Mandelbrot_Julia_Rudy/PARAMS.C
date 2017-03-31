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

#include "mand.h"

static double tu, tv, ta, tb, tx, ty, tw, ti;


void SetUpProperView(void)
{
	SaveMe = 1;
	flagstonames();		/* just sets MyTypeFlag as side effect */
	switch (MyTypeFlag)
	{
	case 0:
		pushview(&startview);
		break;
	case 1:
		pushview(&startview);
		break;
	case 2:
		pushview(&mandelview);
		break;
	case 3:
		pushview(&cubicview);
		break;
	case 4:
		pushview(&ruckerview);
		break;
	}
}

static rect *bR[11];
static char *doitmsgs[] = {"F1 for HELP","ESC to Cancel","ACCEPT"};
static rect doitrects[3];
static text_button pTB[8];
static double ourparms[8];
static int items;
static char *prompts[] = {"X: ","Y: ","W: ","A: ","B: ","U: ","V: ","I: "};

static void push(int i,int inout)
{
   if(i == -1)
      return;

   if (i < items - 3)
      InvertInsides(&pTB[i]);
   else
   {
      PushButton(bR[i],inout);
      if (i == items - 1)
         ExtraHilite(bR[i],inout);
   }
}

static struct {
   double lo,hi;
} limits[] = {
   -2.0,2.0,
   -2.0,2.0,
   1e-13,5.6,
   -2.0,2.0,
   -2.0,2.0,
   -2.0,2.0,
   -2.0,2.0,
   10.0,30000.0
   };
      

void do_params_menu(void)
{
   int height,width;
   rect R,tR;
   int i;
   int current_item;
   int doit = false;
   int row;

   if (!floatflag)
      integer_to_float();


	ourparms[3] = ta = startfa;
	ourparms[4] = tb = startfb;
	ourparms[5] = tu = startfu;
	ourparms[6] = tv = startfv;
	ourparms[7] = ti = maxiteration;

	ourparms[0] = tx = flox + (fhix - flox) / 2.0;
	ourparms[1] = ty = floy + (fhiy - floy) / 2.0;
	ourparms[2] = tw = fhix - flox;
   items = 0;

   height = 11 * (3*FontHeight/2) + FontHeight + FontHeight/2 + 12;
   width = sR.Xmax / 3;

   PushCursorPosition();
   HideCursor();
   ProtectOff();

   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Tweak Menu",BLACK);
   row = tR.Ymin + FontHeight + 6;

   R.Xmin = tR.Xmin + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = row;
   R.Ymax = R.Ymin + FontHeight + 4;

   for(i=0;i<8;i++)
   {
      PaintNumberBoxBasePrecision(&R,&pTB[i],ourparms[i],prompts[i],
         (i == 7) ? GS_UNSIGNED : GS_FLOAT,
         BLACK,LIGHTGRAY,DARKGRAY,WHITE,10,10);
      OffsetRect(&R,0,3*FontHeight/2);
      row += 3*FontHeight/2;
      bR[items++] = (rect *)&pTB[i];
   }

   OffsetRect(&R,0,FontHeight/2);
   row += FontHeight/2;

   for(i=0;i<3;i++)
   {
      doitrects[i] = R;
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      OffsetRect(&R,0,3*FontHeight/2);
      row += 3*FontHeight/2;
      bR[items++] = &doitrects[i];
   }

   current_item = items - 2;
   LimitMouseRect(&tR);
   push(current_item,true);
   move_to_corner(bR[current_item]);
   ArrowCursor();
   ShowCursor();
   while(1)
   {
      event e;
      int n;
      int X;
      int Y;
      int key = 0;
      int button;
      int last_item = current_item;

      n = KeyEvent(false,&e);
      X = e.CursorX;
      Y = e.CursorY;
      button = (e.State & 0x700) >> 8;
      current_item = -1;
      for(i=0;i<items;i++)
      {
         if (XYInRect(X,Y,bR[i]))
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

         if (button == swLeft)
            key = 0x0d;

      }

      navigate(key,NULL,NULL,(int *)-1,(int *)-1,items,bR,&current_item);

      if (key == XF1)
      {
         current_item = items - 3;
         key = 0x0d;
      }

      if (key == 0x1b)
         current_item = items - 2;
         


      if (last_item != current_item)
      {
         push(last_item,false);
         push(current_item,true);
      }


      if (key == 0x0d)
      {
         if (current_item == items - 1)
         {
            doit = true;
            break;
         }
         if (current_item == items - 2)
         {
            doit = false;
            break;
         }
         if (current_item == items - 3)
         {
            helptext("mndtweak.hlp");
            LimitMouseRect(&tR);
         }
         else if (current_item != -1)
         {
            double z = ourparms[current_item];
            if (GetNumberBase(&pTB[current_item],&z,
               (current_item == 7) ? GS_UNSIGNED : GS_FLOAT,
               limits[current_item].lo,limits[current_item].hi,10,10))
                  ourparms[current_item] = z;
            PaintNumberBoxEntryPrecision(&pTB[current_item],
               ourparms[current_item],
               (current_item == 7) ? GS_UNSIGNED : GS_FLOAT,10,10);
            push(current_item,true);
         }


      }
      if (key == 0x1b)
         break;
   }
   if (doit)
      PaintRadioButton(bR[items-1],true,true,doitmsgs[2]);
   else
      PaintRadioButton(bR[items-2],true,true,doitmsgs[1]);
   WaitForNothing();
   if (doit)
      PaintRadioButton(bR[items-1],false,false,doitmsgs[2]);
   else
      PaintRadioButton(bR[items-2],false,false,doitmsgs[1]);
   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (doit)
   {
      double dx,dy;

      tx = ourparms[0];
      ty = ourparms[1];
      tw = ourparms[2];
      ta = ourparms[3];
      tb = ourparms[4];
      tu = ourparms[5];
      tv = ourparms[6];
      ti = ourparms[7];
   	pushstamp(&stampviews[saveptr]);

		fu = tu;
		fv = tv;
		fa = ta;
		fb = tb;

		flox = tx - tw / 2.0;
		fhix = tx + tw / 2.0;
		floy = ty - (4.5 / 5.6) * tw / 2.0;
		fhiy = ty + (4.5 / 5.6) * tw / 2.0;
		maxiteration = ti;
      HideCursor();
      /* can we be floating point here? */

      dx = (double)(fhix-flox)/maxx;
      dy = (double)(fhiy-floy)/maxy;

      if (dx <= 1.0e-6 || dy <= 1.0e-6 || fractal == incubicmandelfloat)
      {
         if (!floatflag)
            slowdown_warning();
         floatflag = 1;
      }
      else if (floatflag)
      {
         speedup_warning();
         floatflag = 0;
         float_to_integer();
      }


      SetUpProperView();
		resetcursor();
		useview();
      ShowCursor();
   }

   if (curx >= minx)
      BoxCursor();

}




