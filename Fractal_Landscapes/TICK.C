#include <math.h>
#include "forge.h"

#define LINETICKS

static rect outerCircle,innerCircle;
rect TickerFrame;
int ticker_active = 0;
static int cx,cy;
static int lastangle;
static point timepoint;
int ticking = 1;

/* If ticking is 1, we do it on the EGA screen. If it is 2,
   we do it on the VGA screen in the corner. */

void InitTicker(char *s,long totalticks)
{
   int i;
   int ticks;
   ticks = 50;

   if (ticking)
   {
      if (!ticker_active)
      {
         int width;
         
         int height;
         point p;

         if (ticking == 1)
         {

            width= sR.Xmax/5 + 20;
            width |= 1;    /* make it odd */
            height = ((int)(width / aspect)) | 1;
            p.X = cx = width/2 + 20;
            p.Y = cy = height/2 + FontHeight + 20;

         }
         else
         {
            width = 39;
            height = 29;
            cx = p.X = width+1;
            cy = p.Y = height+1;
         }

         CenterRect(&p,width,height,&outerCircle);
         innerCircle = outerCircle;
         InsetRect(&innerCircle,4,4);
         TickerFrame = outerCircle;
         InsetRect(&TickerFrame,-4,-4);
   
         
         if (ticking == 1)
         {
            TickerFrame.Ymin -= FontHeight + 4;
            ProtectRect(&TickerFrame);
            ShadowAndSave(&TickerFrame);
         }
         ticker_active = 1;
      }

      lastangle = -1;

      if (ticking == 1)
      {
         PenColor(MENUBACK);
         PaintRect(&TickerFrame);
         PenColor(BUTTONFRAME);
         FrameRect(&TickerFrame);
         TextAlign(alignCenter,alignTop);
         MoveTo(cx,TickerFrame.Ymin + 1);
         PenColor(MENUTEXT);
         BackColor(MENUBACK);
         DrawString(s);
         FrameOval(&outerCircle);
      }
      else
      {
         VGAPaintRect(&TickerFrame,0);
         VGAHLine(TickerFrame.Xmin,TickerFrame.Ymin,TickerFrame.Xmax,63);
         VGAHLine(TickerFrame.Xmin,TickerFrame.Ymax,TickerFrame.Xmax,63);
         VGAVLine(TickerFrame.Xmin,TickerFrame.Ymin,TickerFrame.Ymax,63);
         VGAVLine(TickerFrame.Xmax,TickerFrame.Ymin,TickerFrame.Ymax,63);

//         VGAOval(&outerCircle,63);
      }


   }

   for(i=0;i<ticks;i++)
      tickertape[i] = ((i+1)*totalticks)/(long)ticks;


}

void tick(long percent)
{

   if (ticking)
   {

   
      if (percent > 50L)
         return;

      /* draw a line from the center to the right place... */
      if (lastangle != -1)
      {
         if (ticking == 1)
         {
            PenColor(MENUBACK);
            MoveTo(cx,cy);
            LineTo(timepoint.X,timepoint.Y);
         }
         else
            VGALine(cx,cy,timepoint.X,timepoint.Y,0);

      }

      OvalPt(&innerCircle,lastangle = 900 - (int)(percent * 36 * 2),&timepoint);

      if (ticking == 1)
      {
         PenColor(MENUTEXT);
         MoveTo(cx,cy);
         LineTo(timepoint.X,timepoint.Y);
      }
      else
         VGALine(cx,cy,timepoint.X,timepoint.Y,63);

   }
}

void CloseTicker(void)
{
   if (ticker_active)
   {
      int err;
      
      if (ticking == 1)
      {
         PopRect(&err);
         ProtectOff();
      }
      else if (ticking == 2)
      {
         /* Just black out that top area */
         VGAPaintRect(&TickerFrame,0);
      }
      ticker_active = 0;

   }
}

/* Move the ticker to another part of the screen... */
void MoveTicker(void)
{
   if (ticker_active)
   {
      if (ticking == 1)
      {
         long len;
         image *buffer;
         int dx,dy;
         
         len = ImageSize(&TickerFrame);
         HideCursor();
         /* Save the existing ticker */
         buffer = farmalloc(len);
         ReadImage(&TickerFrame,buffer);
         PopRect(&dx);

         dx = sR.Xmax - 10 - TickerFrame.Xmax;
         dy = sR.Ymax - 10 - TickerFrame.Ymax;
         ProtectRect(&TickerFrame);
         OffsetRect(&TickerFrame,dx,dy);
         OffsetRect(&innerCircle,dx,dy);
         OffsetRect(&outerCircle,dx,dy);
         cx += dx;
         cy += dy;
         timepoint.X += dx;
         timepoint.Y += dy;
         ShadowAndSave(&TickerFrame);
         WriteImage(&TickerFrame,buffer);
         farfree(buffer);
         ShowCursor();
      }
   }
}






