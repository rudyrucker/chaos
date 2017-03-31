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

int MyTypeFlag;

static rect *bR[12];
static int my_type;
static int my_fill;
static int items;

static void push(int item,int inout)
{
   if (item == -1)
      return;

   if (item < 5)
   {
      if (item != my_type)
         PushButton(bR[item],inout);
      else
         DoublePress(bR[item],inout,RED);
   }
   else if (item < 8)
   {
      if (item - 5 != my_fill)
         PushButton(bR[item],inout);
      else
         DoublePress(bR[item],inout,RED);
   }
   else
   {
      PushButton(bR[item],inout);
      if (item == items - 1)
         ExtraHilite(bR[item],inout);
   }
}


void do_type_menu(void)
{
   rect R;
   int width;
   int height;
   rect tR;
   int cx,cy;
   char *msg1 = "Fractal: ";
   char *msg2 = "Fill: ";
   int row;
   static char *typemsgs[] = {
      "Mandelbrot",
      "Julia",
      "Rudy",
      "Cubic Mandelbrot",
      "Cubic Julia",
      };
   static char *doitmsgs[] = {"F1 for HELP","ESC to Cancel","ACCEPT"};
   int i;
   int current_item;

   static char *fillmsgs[] = {"Blank","Bullseye","Feathers"};
   static rect typeRect[5];
   static rect fillRect[3];
   static rect doitRect[3];



   static int my_order[] = {1,4,0,3,2};
   static int invert_my_order[] = {2,0,4,3,1};
   int doit = false;

   static int downers[] = {3,3,4,5,7,8,9,10,0,1,2};
   static int lefters[] = {2,0,1,4,3,7,5,6,10,8,9};
   static int righters[] = {1,2,0,4,3,6,7,5,9,10,8};
   static int uppers[] = {8,9,10,0,2,3,3,4,5,6,7};


   items = 0;
   width = 2*sR.Xmax/3;
   height = 4 * (3*FontHeight/2) + FontHeight * 2;

   flagstonames();
   my_type = my_order[MyTypeFlag];
   my_fill = insideflag;

   HideCursor();
   PushCursorPosition();
   ProtectOff();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Types Menu",BLACK);
   Centers(&tR,&cx,&cy);

   row = tR.Ymin + FontHeight + 6;

   JString(msg1,tR.Xmin + 2,row,BLACK,LIGHTGRAY,alignLeft,alignTop);
   R.Xmin = tR.Xmin + 2 + StringWidth(msg1) + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = row;
   R.Ymax = row + FontHeight + 4;

   CreateRadioPanel(&R,typemsgs,typeRect,3,my_type);
   row += 3 * FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanel(&R,&typemsgs[3],&typeRect[3],2,my_type-3);
   for(i=0;i<5;i++)
      bR[items++] = &typeRect[i];


   row += 3 * FontHeight/2 + 4;
   OffsetRect(&R,0,3*FontHeight/2 + 4);
   JString(msg2,tR.Xmin + 2,row,BLACK,LIGHTGRAY,alignLeft,alignTop);
   R.Xmin = tR.Xmin + 2 + StringWidth(msg1) + 4;
   CreateRadioPanel(&R,fillmsgs,fillRect,3,my_fill);
   for(i=0;i<3;i++)
      bR[items++] = &fillRect[i];

   row += 2 * FontHeight;
   OffsetRect(&R,0,2*FontHeight);
   R.Xmin = tR.Xmin + 4;
   CreateRadioPanel(&R,doitmsgs,doitRect,3,-1);
   for(i=0;i<3;i++)
      bR[items++] = &doitRect[i];

   current_item = items - 1;
   move_to_corner(bR[items-1]);
   PushButton(bR[items-1],true);
   ExtraHilite(bR[items-1],true);
   LimitMouseRect(&tR);

   ArrowCursor();  
   ShowCursor();
   while(1)
   {
      event e;
      int n;
      int X,Y,button;
      int key=0;
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

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);

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
            helptext("mandtype.hlp");
            LimitMouseRect(&tR);
         }
         CheckRadioButtons(X,Y,typeRect,5,&my_type,typemsgs);
         CheckRadioButtons(X,Y,fillRect,3,&my_fill,fillmsgs);
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
      if (my_type != my_order[MyTypeFlag] || my_fill != insideflag)
      {
         static int iterators[] = {50,20,50,20,20};

         pushstamp(&stampviews[saveptr]);
         insideflag = my_fill;
         MyTypeFlag = invert_my_order[my_type];
         MyTypetoFlags();
         if (my_type != my_order[MyTypeFlag])
            maxiteration = iterators[MyTypeFlag];
         switch(MyTypeFlag)
         {
         case 2:
				if (cubicflag)
					mandelview = startview;
            break;
			case 3:
				if (!cubicflag)
				{
					fa = fb = fu = fv = 0;	/* coming from mandel */
					cubicview = startview;
				}
				break;
         }
         SaveMe = 1;
         resetcursor();
         useview();
      }

   }
   if (curx > minx)
      BoxCursor();
   else
      ArrowCursor();


}

