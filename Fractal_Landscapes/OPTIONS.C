#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include "forge.h"


#define ROUND(x) (((x) > 0) ? ((x) + .5) : ((x) - .5))

static char *doitmsgs[] = {
   "F1 for HELP",
   "ESC to Cancel",
   "ACCEPT"
   };


static char *onofftexts[] = {"Off","On"};

void other_options(void)
{
   int i,width,height,items,cx,cy;
   rect R,tR,*bR[20];
   int key,current_item,found = 0;
   int cx1,cy1;
   rect bps[4][3];
   int sx;
   int buttonvals[4];
   rect buttonrects[3];

   int ourParamStamping = frame_stamping;
   int ourFrameStamping = parameter_stamping;
   static int lefters[] = {1,0,3,2,4,5,6};
   static int righters[] = {1,0,3,2,4,5,6};
   static int uppers[] = {6,6,0,1,2,4,5};
   static int downers[] = {2,3,4,4,5,6,0};


   height = (5*FontHeight/2) * 3 + FontHeight + 8;
   width = sR.Xmax/3 + 40;
   items = 0;

   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Other Options",BLACK);
   Centers(&sR,&cx,&cy);

   sx = tR.Xmax - (tR.Xmax-tR.Xmin)/3;
   R.Xmin = sx + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = tR.Ymin + FontHeight + 6;
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   DrawString("Parameter Stamping: ");
   CreateRadioPanel(&R,onofftexts,bps[0],2,buttonvals[0] = ourParamStamping);
   bR[items++] = &bps[0][0];
   bR[items++] = &bps[0][1];

   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Frame Stamping: ");

   CreateRadioPanel(&R,onofftexts,bps[1],2,buttonvals[1] = ourFrameStamping);
   bR[items++] = &bps[1][0];
   bR[items++] = &bps[1][1];


   R.Xmin = tR.Xmin + 4;

   for(i=0;i<3;i++)
   {
      R.Ymin += (3*FontHeight/2);
      R.Ymax = R.Ymin + FontHeight + 3;
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      buttonrects[i] = R;
      bR[items++] = &buttonrects[i];
   }

   current_item = items-1;
   move_to_corner(bR[items-1]);
   PushButton(bR[items-1],true);
   ExtraHilite(bR[items-1],true);
   LimitMouseRect(&tR);
   ShowCursor();
   while(!found)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int last_item = current_item;
      ProcessKey(X,Y,&current_item,&key,bR,items,n,&e);
      if (key == 0x1b || (key == 0x0d && current_item == items - 2))
         break;

      if (key == XF1)
      {
         key = 0x0d;
         current_item = 4;
      }


      if (key == 0x0d)
      {
         switch(current_item)
         {
         case 0:
         case 1:
            CheckRadioButtons(X,Y,bps[0],2,&ourParamStamping,onofftexts);
            buttonvals[0] = ourParamStamping;
            break;
         case 2:
         case 3:
            CheckRadioButtons(X,Y,bps[1],2,&ourFrameStamping,onofftexts);
            buttonvals[1] = ourFrameStamping;
            break;
         case 4:
            helptext("forgoopt.hlp");
            LimitMouseRect(&tR);
            break;
         case 6:
            found = 1;
            break;
         }

      }

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);

      if (last_item != current_item)
      {
         if (last_item != -1)
         {
            if (last_item >= items - 3)
               PushButton(bR[last_item],false);
            else 
            {
               int ii = last_item/2;
               if (buttonvals[ii] ^ (last_item & 1))
                  PushButton(bR[last_item],false);
               else
                  DoublePress(bR[last_item],false,RED);
            }   


            if (last_item == items-1)
               ExtraHilite(bR[items-1],false);
         }
         if (current_item != -1)
         {
            if (current_item >= items - 3)
               PushButton(bR[current_item],true);
            else
            {
               int ii = current_item/2;
               if (buttonvals[ii] ^ (current_item & 1))
                  PushButton(bR[current_item],true);
               else
                  DoublePress(bR[current_item],true,RED);
            }   

            if (current_item == items-1)
               ExtraHilite(bR[items-1],true);

         }
      }

   }

   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (found)
   {
      parameter_stamping = ourParamStamping;
      frame_stamping = ourFrameStamping;
   }
}


static void hilite_planet_item(int item,rect **bR,int items,int *buttonvals,int inout)
{
   if (item != -1)
   {
      if (item < 2)
         InvertInsides((text_button *)bR[item]);
      else if (item >= items - 3)
         PushButton(bR[item],inout);
      else if (item < 8) 
      {
         int ii = (item - 2)/2;
         if (buttonvals[ii] ^ (item & 1))
            PushButton(bR[item],inout);
         else
            DoublePress(bR[item],inout,RED);
      }
      else
      {
         if (buttonvals[3] != item - 8)
            PushButton(bR[item],inout);
         else
            DoublePress(bR[item],inout,RED);
      }

      if (item == items-1)
         ExtraHilite(bR[items-1],inout);
   }
}



void planet_options(void)
{
   int i,width,height,items,cx,cy;
   rect R,tR,*bR[20];
   int key,current_item,found = 0;
   int cx1,cy1;
   rect bps[4][3];
   int sx;
   int buttonvals[4];
   text_button hourbox,seasonbox;
   rect buttonrects[3];
   
   static int uppers[14] =  {13,13, 0, 1, 2, 3, 4, 5, 6, 6, 7, 8,11,12};
   static int downers[14] = { 2, 3, 4, 5, 6, 7, 8,10,11,11,11,12,13, 0};
   static int lefters[14] = { 1, 0, 3, 2, 5, 4, 7, 6,10, 8, 9,11,12,13};
   static int righters[14]= { 1, 0, 3, 2, 5, 4, 7, 6, 9,10, 8,11,12,13};
   static char *projtexts[] = {"Globe","Map"};
   static char *rotations[] = {"None","EW","WE"};



   double hour,season;
   int ourDither = DoDither;
   int ourIce = DoIce;
   int ourWholeScreen = DoWholeScreen;
   int ourRotation = DoRotation;
   
   hour = shang/(2*M_PI)*24;
   season = siang / (2*M_PI)*360;
   items = 0;


   height = (3*FontHeight/2) * 8 + FontHeight + 8;
   width = sR.Xmax/2;


   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Planet Options",BLACK);
   Centers(&sR,&cx,&cy);
   R.Xmin = tR.Xmin + 4;
   R.Xmax = cx - 4;
   R.Ymin = tR.Ymin + FontHeight + 4;
   R.Ymax = R.Ymin + FontHeight + 3;


   PaintNumberBoxBase(&R,&hourbox,hour,"Hour:   ",GS_INTEGER,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   bR[items++] = (rect *)&hourbox;
   R.Xmin = cx + 4;
   R.Xmax = tR.Xmax - 4;
   
   PaintNumberBoxBase(&R,&seasonbox,season,"Season: ",GS_INTEGER,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   bR[items++] = (rect *)&seasonbox;


   sx = cx;
   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   R.Xmin = sx + 4;
   
   Centers(&R,&cx1,&cy1);

   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   DrawString("Projection: ");

   CreateRadioPanel(&R,projtexts,bps[0],2,buttonvals[0] = ourWholeScreen);
   bR[items++] = &bps[0][0];
   bR[items++] = &bps[0][1];

   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Icecaps: ");

   CreateRadioPanel(&R,onofftexts,bps[1],2,buttonvals[1] = ourIce);
   bR[items++] = &bps[1][0];
   bR[items++] = &bps[1][1];


   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Dither: ");

   CreateRadioPanel(&R,onofftexts,bps[2],2,buttonvals[2] = ourDither);
   bR[items++] = &bps[2][0];
   bR[items++] = &bps[2][1];

   sx = tR.Xmin + (tR.Xmax-tR.Xmin)/3;
   R.Xmin = sx + 4;

   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Rotation: ");

   CreateRadioPanel(&R,rotations,bps[3],3,buttonvals[3] = ourRotation);
   bR[items++] = &bps[3][0];
   bR[items++] = &bps[3][1];
   bR[items++] = &bps[3][2];


   R.Xmin = tR.Xmin + 4;
   for(i=0;i<3;i++)
   {
      R.Ymin += (3*FontHeight/2);
      R.Ymax = R.Ymin + FontHeight + 3;
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      buttonrects[i] = R;
      bR[items++] = &buttonrects[i];
   }

   current_item = items-1;
   move_to_corner(bR[items-1]);
   PushButton(bR[items-1],true);
   ExtraHilite(bR[items-1],true);
   LimitMouseRect(&tR);
   ShowCursor();
   while(!found)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int last_item = current_item;

      ProcessKey(X,Y,&current_item,&key,bR,items,n,&e);

      if (key == 0x1b || (key == 0x0d && current_item == items - 2))
         break;

      if (key == XF1)
      {
         key = 0x0d;
         current_item = items - 3;
      }



      if (key == 0x0d)
      {
         switch(current_item)
         {
         case 0:
         {
            double z = hour;
            if (GetNumber(&hourbox,&z,GS_INTEGER,-24.0,24.0))
            {
               hour = z;
            }
            InvertInsides(&hourbox);
            break;
         }
         case 1:
         {
            double z = season;
            if (GetNumber(&seasonbox,&z,GS_INTEGER,-360.0,360.0))
            {
               season = z;
            }
            InvertInsides(&seasonbox);
            break;
         }

         case 2:
         case 3:
            CheckRadioButtons(X,Y,bps[0],2,&ourWholeScreen,projtexts);
            buttonvals[0] = ourWholeScreen;
            break;
         case 4:
         case 5:
            CheckRadioButtons(X,Y,bps[1],2,&ourIce,onofftexts);
            buttonvals[1] = ourIce;
            break;
         case 6:
         case 7:
            CheckRadioButtons(X,Y,bps[2],2,&ourDither,onofftexts);
            buttonvals[2] = ourDither;
            break;
         case 8:
         case 9:
         case 10:
            CheckRadioButtons(X,Y,bps[3],3,&ourRotation,rotations);
            buttonvals[3] = ourRotation;
            break;

         case 11:
            helptext("forgpopt.hlp");
            LimitMouseRect(&tR);
            break;
         case 12:
            /* can't happpen here */
            break;
         case 13:
            found = 1;
            break;
         }

      }

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);

      if (last_item != current_item)
      {

         hilite_planet_item(last_item,bR,items,buttonvals,false);
         hilite_planet_item(current_item,bR,items,buttonvals,true);
      }

   }

   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (found)
   {
      DoDither = ourDither;
      DoIce = ourIce;
      DoWholeScreen = ourWholeScreen;
      DoRotation = ourRotation;
      shang = 2*M_PI*hour/24.0;
      siang = 2*M_PI*season/360.0;
   }
}
        
void contour_options(void)
{
   int i,width,height,items,cx,cy;
   rect R,tR,*bR[20];
   int key,current_item,found = 0;
   int cx1,cy1;
   rect bps[3][2];
   int buttonvals[2];
   int sx;

   int ourSmooth = SmoothingPures;
   int ourWrap = WrappingPures;
   static int lefters[] = {1,0,3,2,4,5,6};
   static int righters[] = {1,0,3,2,4,5,6};
   static int uppers[] = {6,6,0,1,2,4,5};
   static int downers[] = {2,3,4,4,5,6,0};
   rect buttonrects[3];

   items = 0;

   height = (3*FontHeight/2) * 5 + FontHeight + 8;
   width = sR.Xmax/3 + 16;

   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Contour Options",BLACK);
   Centers(&sR,&cx,&cy);

   sx = cx;
   R.Xmin = sx + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = tR.Ymin + FontHeight + 6;
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   DrawString("Smoothing: ");
   CreateRadioPanel(&R,onofftexts,bps[0],2,buttonvals[0] = ourSmooth);
   bR[items++] = &bps[0][0];
   bR[items++] = &bps[0][1];
   
   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   Centers(&R,&cx1,&cy1);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Wrapping: ");

   CreateRadioPanel(&R,onofftexts,bps[1],2,buttonvals[1] = ourWrap);
   bR[items++] = &bps[1][0];
   bR[items++] = &bps[1][1];

   R.Xmin = tR.Xmin + 4;
   for(i=0;i<3;i++)
   {
      R.Ymin += (3*FontHeight/2);
      R.Ymax = R.Ymin + FontHeight + 3;
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      buttonrects[i] = R;
      bR[items++] = &buttonrects[i];
   }

   current_item = items-1;
   move_to_corner(bR[items-1]);
   PushButton(bR[items-1],true);
   ExtraHilite(bR[items-1],true);
   LimitMouseRect(&tR);
   ShowCursor();
   while(!found)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int last_item = current_item;

      ProcessKey(X,Y,&current_item,&key,bR,items,n,&e);

      if (key == 0x1b || (key == 0x0d && current_item == items - 2))
         break;

      if (key == XF1)
      {
         key = 0x0d;
         current_item = items - 3;
      }


      if (key == 0x0d)
      {
         switch(current_item)
         {
         case 0:
         case 1:
            CheckRadioButtons(X,Y,bps[0],2,&ourSmooth,onofftexts);
            buttonvals[0] = ourSmooth;
            break;
         case 2:
         case 3:
            CheckRadioButtons(X,Y,bps[1],2,&ourWrap,onofftexts);
            buttonvals[1] = ourWrap;
            break;
         case 4:
            helptext("forgcopt.hlp");
            LimitMouseRect(&tR);
            break;
         case 9:
            /* can't happpen here */
            break;
         case 6:
            found = 1;
            break;
         }

      }

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);

      if (last_item != current_item)
      {
         if (last_item != -1)
         {
            if (last_item >= items - 3)
               PushButton(bR[last_item],false);
            else 
            {
               int ii = last_item/2;
               if (buttonvals[ii] ^ (last_item & 1))
                  PushButton(bR[last_item],false);
               else
                  DoublePress(bR[last_item],false,RED);
            }   


            if (last_item == items-1)
               ExtraHilite(bR[items-1],false);
         }
         if (current_item != -1)
         {
            if (current_item >= items - 3)
               PushButton(bR[current_item],true);
            else
            {
               int ii = current_item/2;
               if (buttonvals[ii] ^ (current_item & 1))
                  PushButton(bR[current_item],true);
               else
                  DoublePress(bR[current_item],true,RED);
            }   

            if (current_item == items-1)
               ExtraHilite(bR[items-1],true);

         }
      }

   }

   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (found)
   {
      SmoothingPures = ourSmooth;
      WrappingPures = ourWrap;
   }
}





void mountain_options(void)
{
   int i,width,height,items,cx,cy;
   rect R,tR,*bR[20];
   int key,current_item,found = 0;
   int cx1,cy1;
   rect bps[1][2];
   int sx;
   int buttonvals[1];
   text_button azimuthbox,elevationbox,limitbox;

   static int lefters[] =  {0,1,2,4,3,5,6,7};
   static int righters[] = {0,1,2,4,3,5,6,7};
   static int uppers[] =   {7,0,1,2,2,3,5,6};
   static int downers[] =  {1,2,3,5,5,6,7,0};
   rect buttonrects[3];

   double azimuth = vturn;
   double elevation = 90.0 - vdown;
   double limit = percentage * 100.0;
   int ourClouds = DoClouds;

   items = 0;


   height = (3*FontHeight/2) * 7 + FontHeight + 8;
   width = sR.Xmax/3+16;


   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Mountain Options",BLACK);
   Centers(&sR,&cx,&cy);
   R.Xmin = tR.Xmin + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = tR.Ymin + FontHeight + 4;
   R.Ymax = R.Ymin + FontHeight + 3;


   PaintNumberBoxBase(&R,&azimuthbox,azimuth,"Azimuth:   ",GS_INTEGER,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   bR[items++] = (rect *)&azimuthbox;


   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   PaintNumberBoxBase(&R,&elevationbox,elevation,"Elevation: ",GS_INTEGER,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   bR[items++] = (rect *)&elevationbox;


   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   PaintNumberBoxBase(&R,&limitbox,limit,"Screen Limit: ",GS_INTEGER,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   bR[items++] = (rect *)&limitbox;



   sx = cx;
   R.Ymin += (3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 3;
   R.Xmin = sx + 4;
   
   Centers(&R,&cx1,&cy1);

   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   TextAlign(alignRight,alignMiddle);
   MoveTo(sx - 4,cy1);
   DrawString("Clouds: ");

   CreateRadioPanel(&R,onofftexts,bps[0],2,buttonvals[0] = ourClouds);
   bR[items++] = &bps[0][0];
   bR[items++] = &bps[0][1];

   R.Xmin = tR.Xmin + 4;


   for(i=0;i<3;i++)
   {
      R.Ymin += (3*FontHeight/2);
      R.Ymax = R.Ymin + FontHeight + 3;
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      buttonrects[i] = R;
      bR[items++] = &buttonrects[i];
   }

   current_item = items-1;
   move_to_corner(bR[items-1]);
   PushButton(bR[items-1],true);
   ExtraHilite(bR[items-1],true);
   LimitMouseRect(&tR);
   ShowCursor();
   while(!found)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int last_item = current_item;
      ProcessKey(X,Y,&current_item,&key,bR,items,n,&e);

      if (key == 0x1b || (key == 0x0d && current_item == items - 2))
         break;

      if (key == XF1)
      {
         key = 0x0d;
         current_item = items - 3;
      }

      if (key == 0x0d)
      {
         switch(current_item)
         {
         case 0:
         {
            double z = azimuth;
            if (GetNumber(&azimuthbox,&z,GS_INTEGER,-180.0,180.0))
            {
               azimuth = z;
            }
            InvertInsides(&azimuthbox);
            break;
         }
         case 1:
         {
            double z = elevation;
            if (GetNumber(&elevationbox,&z,GS_UNSIGNED,0.0,90.0))
            {
               elevation = z;
            }
            InvertInsides(&elevationbox);
            break;
         }
         case 2:
         {
            double z = limit;
            if (GetNumber(&limitbox,&z,GS_INTEGER,2.0,100.0))
            {
               limit = z;
            }
            InvertInsides(&limitbox);
            break;
         }

         case 3:
         case 4:
            CheckRadioButtons(X,Y,bps[0],2,&ourClouds,onofftexts);
            buttonvals[0] = ourClouds;
            break;
         case 5:
            helptext("forgmopt.hlp");
            LimitMouseRect(&tR);
            break;
         case 6:
            /* can't happpen here */
            break;
         case 7:
            found = 1;
            break;
         }

      }

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);

      if (last_item != current_item)
      {
         if (last_item != -1)
         {
            if (last_item < 3)
               InvertInsides((text_button *)bR[last_item]);
            else if (last_item >= items - 3)
               PushButton(bR[last_item],false);
            else 
            {
               int ii = (last_item - 3)/2;
               if (buttonvals[ii] ^ ((last_item-3) & 1))
                  PushButton(bR[last_item],false);
               else
                  DoublePress(bR[last_item],false,RED);
            }   


            if (last_item == items-1)
               ExtraHilite(bR[items-1],false);
         }
         if (current_item != -1)
         {
            if (current_item < 3)
               InvertInsides((text_button *)bR[current_item]);
            else if (current_item >= items - 3)
               PushButton(bR[current_item],true);
            else
            {
               int ii = (current_item - 3)/2;
               if (buttonvals[ii] ^ ((current_item-3) & 1))
                  PushButton(bR[current_item],true);
               else
                  DoublePress(bR[current_item],true,RED);
            }   

            if (current_item == items-1)
               ExtraHilite(bR[items-1],true);

         }
      }

   }

   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (found)
   {
      vturn = azimuth;
      vdown = 90.0 - elevation;
      percentage = limit / 100.0;
      DoClouds = ourClouds;
   }
}



static char *options_txts[] = {
   "Planet Options   ",
   "Contour Options  ",
   "Mountain Options ",
   "Other Options",
   "F1 for HELP",
   "ESC to Cancel",
   NULL
   };

   

void options(void)
{
   int i;
   int width,height;
   rect R;
   int items;
   int cx,cy;
   rect bR[10];
   rect *pbR[10];
   rect tR;
   int key;
   int current_item;
   int found = 0;

      
   
   width = 0;

   for(i=0;options_txts[i];i++)
      width = max(width,StringWidth(options_txts[i])+14);

   items = i;

   height = items * (3*FontHeight/2) + FontHeight + 8 + FontHeight/2;
   Centers(&sR,&cx,&cy);

   R.Xmin = cx - width/2;
   R.Xmax = R.Xmin + width - 1;
   R.Ymin = cy - height/2;
   R.Ymax = R.Ymin + height - 1;


   tR = R;
   PushCursorPosition();
   HideCursor();
   ShadowAndSave(&tR);

   PenColor(LIGHTGRAY);
   PaintRect(&R);
   PenColor(BLACK);
   FrameRect(&R);

   TextAlign(alignCenter,alignTop);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   MoveTo(cx,tR.Ymin + 2);
   DrawString("Options");

   for(i=0;i<items;i++)
   {
      R.Xmin = tR.Xmin + 4;
      R.Xmax = tR.Xmax - 4;
      R.Ymin = tR.Ymin + 2 + FontHeight + 2 + i * (3*FontHeight/2);
      R.Ymax = R.Ymin + FontHeight + 3;
      if (i >= items - 2)
         OffsetRect(&R,0,FontHeight/2);
      bR[i] = R;
      pbR[i] = &bR[i];
      PaintRadioButtonBase(&R,false,false,options_txts[i],
         DARKGRAY,RED,WHITE);
   }

   move_to_corner(&bR[current_item = items-1]);
   PushButton(&bR[current_item],true);
   ExtraHilite(&bR[current_item],true);

   LimitMouseRect(&tR);
   ShowCursor();

   while(1)
   {
      event e;
      int last_item = current_item;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      ProcessKey(X,Y,&current_item,&key,pbR,items,n,&e);

      if (key == 0x1b)
      {
         key = 0x0d;
         current_item = items - 1;
      }
      if (key == XF1)
      {
         key = 0x0d;
         current_item = items - 2;
      }

      if (key == 0x0d && current_item != -1)
      {
         PaintRadioButtonBase(&bR[current_item],true,true,options_txts[current_item],
            DARKGRAY,RED,WHITE);
         if (current_item == items - 1)
            ExtraHilite(&bR[current_item],true);
         WaitForNothing();
         
         if (current_item == items - 2)
         {
            helptext("forgopts.hlp");
            LimitMouseRect(&tR);
         }
         else
            found = 1;
         PaintRadioButtonBase(&bR[current_item],false,false,options_txts[current_item],
            DARKGRAY,RED,WHITE);
         if (current_item == items - 1)
            ExtraHilite(&bR[current_item],false);

         if (found)
            break;


      }

      navigate(key,NULL,NULL,(int*)-1,(int*)-1,items,pbR,&current_item);

      if (last_item != current_item)
      {
         if (last_item != -1)
         {
            PushButton(&bR[last_item],false);
            if (last_item == items-1)
               ExtraHilite(&bR[last_item],false);
         }
         if (current_item !=-1)
         {
            PushButton(&bR[current_item],true);
            if (current_item == items-1)
               ExtraHilite(&bR[current_item],true);
         }
      }

   }

   HideCursor();
   PopRect(&i);
   LimitMouseRect(&sR);
   PopCursorPosition();
   ShowCursor();


   if (found)
   {
      switch(current_item)
      {
      case 0:
         planet_options();
         break;
      case 1:
         contour_options();
         break;
      case 2:
         mountain_options();
         break;
      case 3:
         other_options();
         break;
      }
   }
}


