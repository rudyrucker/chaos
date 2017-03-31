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


#include "mand.h"
#define ASPECTIZING

/* Dragging a zoombox around, from in which zoom to at. */
static rect clipR;
static void BigFramedRect(rect *tR)
{
   rect R = *tR;
   ClipRect(&clipR);
   PenColor(WHITE);
   RasterOp(zXORz);
   FrameRect(&R);
   InsetRect(&R,1,1);
   FrameRect(&R);
   RasterOp(zREPz);
   ClipRect(&sR);
}


void zoombox(void)
{
   rect zoomR;
   point pt;
   int X,Y;
   static double aspect;
   int key,button;
   int stride = 9;
   char tbuf[128];
   double fcenterx,fcentery,fwidth;



   int height,width;

   HideCursor();
   PushCursorPosition();
   clipR = sR;
   clipR.Xmin = sR.Xmax/8;

   aspect = (clipR.Xmax-clipR.Xmin)/(float)(clipR.Ymax-clipR.Ymin);

   width = max(maxx*zoomfactor,8);

   height = width / aspect;

   



   X = pt.X = curx + minx;
   Y = pt.Y = cury;

   CenterRect(&pt,width,height,&zoomR);
   MoveCursor(pt.X,pt.Y);

   BigFramedRect(&zoomR);
   fcenterx = flox + (X-minx)/(double)maxx * (fhix - flox);
   fcentery = floy + Y/(double)maxy * (fhiy - floy);
   fwidth = (zoomR.Xmax-zoomR.Xmin)/(double)maxx * (fhix-flox);
   sprintf(tbuf,"%g %g %g",fcenterx,fcentery,fwidth);
   menutext(tbuf);


   /* debounce till buttons are up */
   while(1)
   {
      event e;
      KeyEvent(false,&e);
      if (((e.State >> 8) & 0x7) == 0)
         break;
   }


   while(1)
   {
      event e;
      int xstep=0,ystep=0;
      int xd=0,yd=0;

      int n = KeyEvent(false,&e);
   
      button = (e.State >> 8) & 0x7;
      key = 0;
      if (n)
      {
         if (e.ASCII && e.ASCII != 0xe0)
            key = e.ASCII;
         else
            key = e.ScanCode << 8;


      }

      if (e.CursorX != X || e.CursorY != Y)
      {
         xstep = e.CursorX - X;
         ystep = e.CursorY - Y;
      }


      if (button == swLeft)
         key = 0x0d;

      else if (button == swRight)
         key = XDELETE;

      else if (button == swLeft+swRight)
         e.State |= 3;

      if (key == 0x1b || key == ' ' || key == XALTZ)
         break;

      if (key == 0x0d || key == XINSERT || key == XDELETE)
      {
         zoomfactor = ((zoomR.Xmax-zoomR.Xmin)/(float)(maxx));
         break;
      }
      

      switch(e.ScanCode << 8)
      {
      case XF1:
         RasterOp(zREPz);
         ArrowCursor();
         ShowCursor();
         helptext("zoomvar.hlp");
         HideCursor();
         LimitMouseRect(&sR);
         PenColor(WHITE);
         RasterOp(zXORz);
         break;
      case XHOME:
         xstep = ystep = -stride;
         break;
      case XPGDN:
         xstep = ystep = stride;
         break;
      case XLARROW:
         xstep = -stride;
         break;
      case XRARROW:
         xstep = stride;
         break;
      case XDARROW:
         ystep = stride;
         break;
      case XUARROW:
         ystep = -stride;
         break;
      case XPGUP:
         xstep = stride;
         ystep = -stride;
         break;
      case XEND:
         xstep = -stride;
         ystep = stride;
         break;
      case XCENTER5:
         stride ^= 8;
         break;
      }


                     


      if (xstep || ystep)
      {

         BigFramedRect(&zoomR);
         if (e.State & 3)
         {
            rect RR = zoomR;
            InsetRect(&zoomR,xstep,ystep);
      
          
            /* pop the aspect ratio */

#ifdef ASPECTIZING
            if (xstep)
            {
               int yc;
               int height;
               yc = zoomR.Ymin+(zoomR.Ymax-zoomR.Ymin)/2;
               height = (zoomR.Xmax-zoomR.Xmin) / aspect;
               zoomR.Ymin = yc-height/2;
               zoomR.Ymax = zoomR.Ymin + height;
            }
            else
            {
               int xc;
               int width;
               xc = zoomR.Xmin + (zoomR.Xmax-zoomR.Xmin)/2;
               width = (zoomR.Ymax-zoomR.Ymin) * aspect;
               zoomR.Xmin = xc - width/2;
               zoomR.Xmax = zoomR.Xmin + width;
            }
#endif
            if (zoomR.Xmin > zoomR.Xmax-8 || zoomR.Ymin > zoomR.Ymax-8)
               zoomR = RR;


         }
         else
         {

            OffsetRect(&zoomR,xstep,ystep);
            /* Now force the fucker if it is out of range */
            X += xstep;
            Y += ystep;
         }
         if (zoomR.Xmin < minx && zoomR.Xmax > minx+maxx ||
             zoomR.Ymin < minx && zoomR.Ymax > minx+maxx)
            InsetRect(&zoomR,10,10);

             

            
         if (zoomR.Xmin < minx)
            xd = minx - zoomR.Xmin;
         if (zoomR.Xmax > minx+maxx)
            xd = minx+maxx - zoomR.Xmax;
         if (zoomR.Ymin < 0)
            yd = - zoomR.Ymin;
         if (zoomR.Ymax > maxy)
            yd = maxy - zoomR.Ymax;

         if (xd || yd)
         {
            OffsetRect(&zoomR,xd,yd);
            X += xd;
            Y += yd;
         }
         MoveCursor(X,Y);
         BigFramedRect(&zoomR);
         /* and print the bottom line shit */
         fcenterx = flox + (X-minx)/(double)maxx * (fhix - flox);
         fcentery = floy + Y/(double)maxy * (fhiy - floy);
         fwidth = (zoomR.Xmax-zoomR.Xmin)/(double)maxx * (fhix-flox);
         sprintf(tbuf,"%g %g %g",fcenterx,fcentery,fwidth);
      	menutext(tbuf);

    

         
      }
   }
   BigFramedRect(&zoomR);
   RasterOp(zREPz);
   if (key == 0x0d || key == XINSERT || key == XDELETE)
   {
      curx = zoomR.Xmin + (zoomR.Xmax-zoomR.Xmin)/2;
      cury = zoomR.Ymin + (zoomR.Ymax-zoomR.Ymin)/2;
      dozoom((key == XDELETE) ? (1/zoomfactor) : zoomfactor);
   }
   else
      bottomline();

   PopCursorPosition();
   if (curx > minx)
      BoxCursor();
   else
      ArrowCursor();
   ShowCursor();
}

            




