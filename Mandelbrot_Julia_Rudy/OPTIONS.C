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



static rect *bR[40];
static rect cycleR[2];
static rect autocycleR[3];
static rect palR[4];
static rect monoR[2];
static text_button bandwidthTB;
static rect bandwidthpmR[2];
static rect soundR[2];
static rect zoomR[2];
static rect ctR[2];
static rect sliceR[6];
static rect lineR[3];
static rect doitR[3];
static int ourspinflag;
static int ourgrayflag;
static double ourbandsize;
static int oursoundflag;
static int ourcursorshape;
static int ourfourdeeflag;
static int ourslicetype;
static int ourPDM;

static void push(int n,int inout)
{
   switch(n)
   {
   case -1:
      return;
   case 2:
   case 3:
   case 4:
      PushOrDoublePress(bR[n],inout,n-2 == ourspinflag);
      break;
   case 9:
   case 10:
      PushOrDoublePress(bR[n],inout,n-9 == ourgrayflag);
      break;
   case 11:
      InvertInsides(&bandwidthTB);
      break;
   case 14:
   case 15:
      PushOrDoublePress(bR[n],inout,n-14 == oursoundflag);
      break;
   case 16:
   case 17:
      PushOrDoublePress(bR[n],inout,n-16 == ourcursorshape);
      break;
   case 18:
   case 19:
      PushOrDoublePress(bR[n],inout,n-18 == ourfourdeeflag);
      break;
   case 20:
   case 21:
   case 22:
   case 23:
   case 24:
   case 25:
      PushOrDoublePress(bR[n],inout,n-20 == ourslicetype);
      break;
   case 26:
   case 27:
   case 28:
      PushOrDoublePress(bR[n],inout,n-26 == ourPDM);
      break;
//   case 12:
//   case 13:
//      PushButton(bR[12],inout);
//      break;
   case 31:
      ExtraHilite(bR[n],inout);
      /* deliberate fallthrough */
   default:
      PushButton(bR[n],inout);
   }
      
}









void do_colors_menu()
{
   int height = 12 * (3 * FontHeight/2) + 2 * FontHeight + 12;
   int width = 2*sR.Xmax / 3;
   rect tR;
   int cx,cy;
   rect R;
   int key;
   int i;
   int row;
   static char *ccmsgs[] = {"None","Forward","Reverse"};
   static char *palmsgs[] = {"Default","Random","Preset","Edit"};
   static char *offon[] = {"Off","On"};
   static char *zoomselect[] = {"Zoom","Select"};
   static char *slicers[] = {"UV","AB","AU","BV","BU","AV"};
   static char *liners[] = {"None","Track","Params"};
   static char *doits[] = {"F1 for HELP","ESC to Cancel","ACCEPT"};
   int current_item;
   int doit = 0;

   int items = 0;
   int savedgrayflag = grayflag;

   static int lefters[32] = {
      1,0,
      4,2,3,
      8,5,6,7,
      10,9,
      13,11,12,
      15,14,
      17,16,
      19,18,
      25,20,21,22,23,24,
      28,26,27,
      31,29,30
      };

   static int righters[32] = {
      1,0,
      3,4,2,
      6,7,8,5,
      10,9,
      12,13,11,
      15,14,
      17,16,
      19,18,
      21,22,23,24,25,20,
      27,28,26,
      30,31,29
      };

   static int uppers[32] = {
      29,31,
      0,0,1,
      2,3,3,4,
      7,8,
      9,10,10,
      11,12,
      14,15,
      16,17,
      18,18,18,18,19,19,
      22,23,24,
      26,26,28
      };

   static int downers[32] = {
      3,4,
      5,6,8,
      9,9,9,10,
      11,12,
      14,15,15,
      16,17,
      18,19,
      22,24,
      26,26,26,27,28,28,
      30,30,31,
      0,0,1
      };




   HideCursor();
   ProtectOff();
   ArrowCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Options Menu",BLACK);
   Centers(&tR,&cx,&cy);

   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   MoveTo(tR.Xmin,tR.Ymin + FontHeight + 4);
   LineRel(width-1,0);

//   MoveTo(cx,tR.Ymin + FontHeight + 8);
//   TextAlign(alignCenter,alignTop);
//   DrawString("Colors");
//
//
//   row = tR.Ymin + FontHeight + 8 + FontHeight + 4;

   row = tR.Ymin + FontHeight + 12;

   TextAlign(alignRight,alignTop);
   MoveTo(cx - 4,row + 2);
   DrawString("Color Cycle Once: ");
   R.Xmin = cx + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = row;
   R.Ymax = row + FontHeight + 4;
   CreateRadioPanel(&R,&ccmsgs[1],cycleR,2,-1);
   bR[items++] = &cycleR[0];
   bR[items++] = &cycleR[1];

   row += 3*FontHeight/2;

   R.Xmin = tR.Xmin + width/4;
   R.Ymin = row;
   R.Ymax = row + FontHeight + 4;

   JString("Auto Cycle: ",R.Xmin - 4,row+2,BLACK,LIGHTGRAY,alignRight,alignTop);

   CreateRadioPanel(&R,ccmsgs,autocycleR,3,ourspinflag = spinflag);
   for(i=0;i<3;i++)
      bR[items++] = &autocycleR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);

   JString("Palette: ",R.Xmin-4,row+2,BLACK,LIGHTGRAY,alignRight,alignTop);

   CreateRadioPanel(&R,palmsgs,palR,4,-1);
   for(i=0;i<4;i++)
      bR[items++] = &palR[i];

   OffsetRect(&R,0,3*FontHeight/2);
   row += 3*FontHeight/2;

   R.Xmin = cx + 4;
   R.Xmax = tR.Xmax - 4;
   JString("Monochrome: ",R.Xmin-4,row+2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,offon,monoR,2,ourgrayflag = grayflag);
   for(i=0;i<2;i++)
      bR[items++] = &monoR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = tR.Xmin + width/3;
   R.Xmax = tR.Xmax - width/4;
   PaintNumberBoxBasePrecision(&R,&bandwidthTB,(double)bandsize,"Bandwidth:  ",GS_FLOAT,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE,6,6);
   ourbandsize = bandsize;
   bR[items++] = (rect *)&bandwidthTB;

   R.Xmin = R.Xmax + 8;
   R.Xmax = R.Xmin + 4 * StringWidthX;
   PlusMinusButtons(&R,bandwidthpmR);
   for(i=0;i<2;i++)
      bR[items++] = &bandwidthpmR[i];

   row += 3*FontHeight/2;
   PenColor(BLACK);
   MoveTo(tR.Xmin + 1,row);
   LineTo(tR.Xmax - 1,row);

   row += FontHeight/2;

   R.Xmin = cx+4;
   R.Xmax = tR.Xmax - 8;
   R.Ymin = row;
   R.Ymax = row + FontHeight + 4;

   JString("Sound: ",cx - 4, row + 2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,offon,soundR,2,oursoundflag = soundflag);
   for(i=0;i<2;i++)
      bR[items++] = &soundR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   JString("Cursor Mode: ",cx - 4,row + 2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,zoomselect,zoomR,2,ourcursorshape = cursorshape);
   for(i=0;i<2;i++)
      bR[items++] = &zoomR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   JString("4D Cursor Tracking: ",cx - 4,row + 2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,offon,ctR,2,ourfourdeeflag = fourdeeflag);
   for(i=0;i<2;i++)
      bR[items++] = &ctR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = tR.Xmin + (tR.Xmax-tR.Xmin)/4;
   JString("4D Slice: ",R.Xmin - 4,row + 2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,slicers,sliceR,6,ourslicetype = slicetype);
   for(i=0;i<6;i++)
      bR[items++] = &sliceR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = cx + 4;
   JString("Status Line: ",cx - 4,row + 2,BLACK,LIGHTGRAY,alignRight,alignTop);
   CreateRadioPanel(&R,liners,lineR,3,ourPDM = ParameterDisplayMode);
   for(i=0;i<3;i++)
      bR[items++] = &lineR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   R.Xmin = tR.Xmin + 4;

   PenColor(BLACK);
   MoveTo(tR.Xmin+1,row);
   LineTo(tR.Xmax-1,row);

   row += FontHeight;
   OffsetRect(&R,0,FontHeight);
   CreateRadioPanel(&R,doits,doitR,3,-1);
   for(i=0;i<3;i++)
      bR[items++] = &doitR[i];

   ExtraHilite(bR[items-1],false);

   



   LimitMouseRect(&tR);

   ShowCursor();

   current_item = items - 1;
   push(current_item,true);
   move_to_corner(bR[current_item]);
   while(1)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int button = (e.State & 0x700) >> 8;
      int last_item = current_item;

      key = 0;

      for(i=0;i<items;i++)
      {
         current_item = -1;
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
         else if (e.ScanCode != 0xff)
            key = e.ScanCode << 8;

         if (button == swLeft)
            key = 0x0d;

         if (button == swRight)
            key = 0x1b;

         if (key == 0x1b)
            break;

         if (key == XF10)
            GifOutput("mandopt.gif",1);

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
         helptext("mandopts.hlp");
         LimitMouseRect(&tR);
      }

      if (key == 0x0d)
      {
         switch(current_item)
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
            CheckRadioButtons(X,Y,autocycleR,3,&ourspinflag,ccmsgs);
            break;
         case 5:
            DefaultPalette();
				break;
         case 6:
            randompalette();
				break;
         case 7:
            bumppalette();
				break;
         case 8:
            palette_tweaker();
         	LimitMouse(tR.Xmin, tR.Ymin, tR.Xmax, tR.Ymax);
            break;
         case 9:
         case 10:
            CheckRadioButtons(X,Y,monoR,3,&ourgrayflag,offon);
            grayflag = ourgrayflag;
            
            grayscale();
            usepalette();
            break;
         case 11:
         {
            double z = ourbandsize;
            if (GetNumberBase(&bandwidthTB,&z,GS_FLOAT,.001,32767.0,6,6))
               ourbandsize = z;
            PaintNumberBoxEntryPrecision(&bandwidthTB,ourbandsize,GS_FLOAT,6,6);
         }
            push(current_item,true);
            break;
         case 12:
				ourbandsize /= 2;
				if (ourbandsize < 0.001)	/* min at .004 digit 8K */
					ourbandsize = 0.001;
            PaintNumberBoxEntryPrecision(&bandwidthTB,ourbandsize,GS_FLOAT,6,5);
            push(current_item,true);
            break;
         case 13:
				ourbandsize *= 2;
				if (ourbandsize >= 32767.0)	/* max at 4 digit 64K */
					ourbandsize = 32767.0;
            PaintNumberBoxEntryPrecision(&bandwidthTB,ourbandsize,GS_FLOAT,6,5);
            push(current_item,true);
            break;
         case 14:
         case 15:
            CheckRadioButtons(X,Y,soundR,2,&oursoundflag,offon);
            break;
         case 16:
         case 17:
            CheckRadioButtons(X,Y,zoomR,2,&ourcursorshape,zoomselect);
            break;
         case 18:
         case 19:
            CheckRadioButtons(X,Y,ctR,2,&ourfourdeeflag,offon);
            break;
         case 20:
         case 21:
         case 22:
         case 23:
         case 24:
         case 25:
            CheckRadioButtons(X,Y,sliceR,6,&ourslicetype,slicers);
            break;
         case 26:
         case 27:
         case 28:
            CheckRadioButtons(X,Y,lineR,3,&ourPDM,liners);
            break;
         }
      }

            
      
            
      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);






      if (current_item != last_item)
      {
         push(last_item,false);
         push(current_item,true);
      }

   }
   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (doit)
   {
      int uvflag = false;

      spinflag = ourspinflag;

      if (ourbandsize != bandsize)
      {
         bandsize = ourbandsize;
         fillcolorval();
         uvflag = 1;
      }

      if (savedgrayflag != ourgrayflag)
         grayflag = ourgrayflag;
      else
         grayflag = savedgrayflag;
      grayscale();
      usepalette();

      soundflag = oursoundflag;
      if (cursorshape != ourcursorshape)
         cursorshape = ourcursorshape;

      if (ourfourdeeflag != fourdeeflag)
      {
         fourdeeflag = ourfourdeeflag;
         uvflag = true;
      }

      if (ourslicetype != slicetype)
      {
         slicetype = ourslicetype;
         uvflag = true;
      }

      if (ourPDM != ParameterDisplayMode)
      {
         HideCursor();
         ParameterDisplayMode = ourPDM;
         PushCursorPosition();
         switch(ParameterDisplayMode)
         {
         case 0:
            menutext("");
            break;
         case 1:
            _updatecursor(true);
            break;
         case 2:
            draw_params();
            break;
         }
         PopCursorPosition();
         ShowCursor();
      }

      if (uvflag)
      {
         resetcursor();
         useview();
      }
   }
   if (curx > minx)
      BoxCursor();
}



