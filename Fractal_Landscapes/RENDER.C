#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include "forge.h"



static char *rendertomsgs[] = {
   "Render to Screen",
   "Render to File"
   };

static char *rendermodemsgs[] = {
   "16 colors",
   "256 colors"
   };

static char *renderthings[] = {
   "Clouds",
   "Mountains",
   "Planet",
   "Contour"
   };

static char *doitmsgs[] = {
   "F1 for HELP",
   "ESC to Cancel",
   "RENDER"
   };

static rect renderthingFrame;
static rect renderthingRects[4];
static rect rendermodeRects[2];
static rect rendermodeFrame;
static rect rendertoRects[2];
static rect rendertoFrame;
static rect doitFrame;
static rect doitRects[3];


enum {
   CLOUDS,MOUNTAINS,PLANET,PURE,
   COLOR16,COLOR256,
   TOSCREEN,TOFILE,
   F1HELP,CANCEL,RENDER
   };

static int lefts[] = {
   PURE,CLOUDS,MOUNTAINS,PLANET,
   COLOR256,COLOR16,
   TOFILE,TOSCREEN,
   RENDER,F1HELP,CANCEL
   };

static int rights[] = {
   MOUNTAINS,PLANET,PURE,CLOUDS,
   COLOR256,COLOR16,
   TOFILE,TOSCREEN,
   CANCEL,RENDER,F1HELP
   };

   

static int downers[] = {
   COLOR16,COLOR16,COLOR256,COLOR256,
   TOSCREEN,TOFILE,
   F1HELP,CANCEL,
   CLOUDS,MOUNTAINS,PLANET
   };

static int uppers[] = {
   F1HELP,CANCEL,CANCEL,RENDER,
   CLOUDS,PLANET,
   COLOR16,COLOR256,
   TOSCREEN,TOSCREEN,TOFILE,
   };   

int rendermode = 0;
int renderto = 0;
int renderthing = 0;
static int current_item;
int parameter_stamping = 0;
int frame_stamping = 0;

extern void DisplayChar13(int,int,int,char,char);

void StampParameters(double fracdim,double elevfac,double power,
   int mesh,int terms,double vturn,double vdown,double siang,double shang)
{
   char tbuf[128];

   switch(renderthing)
   {
   case 0:
   case 3:
      sprintf(tbuf,"D:%4.3f H:%4.3f P:%4.3f M:%d T:%d R:%d",
         round(fracdim),round(elevfac),round(log(power+1)),meshsizes[mesh],
            min(meshsizes[mesh],termsizes[terms]),rseed);
      break;
   case 1:
      sprintf(tbuf,"D:%4.3f H:%4.3f P:%4.3f M:%d T:%d R:%d A:%4.3f E:%4.3f",
         round(fracdim),round(elevfac),round(log(power+1)),meshsizes[mesh],
            min(meshsizes[mesh],termsizes[terms]),rseed,
            vturn,90-vdown);
      break;
   case 2:
      sprintf(tbuf,"D:%4.3f H:%4.3f P:%4.3f M:%d T:%d R:%d S:%d H:%d",
         round(fracdim),round(elevfac),round(log(power+1)),meshsizes[mesh],
            min(meshsizes[mesh],termsizes[terms]),rseed,
            (int)(siang * 360.0 / (2 * M_PI)),
            (int)(shang * 24.0/(2*M_PI)));
      break;
   }

   if (rendermode == 0)
   {
      PenColor(WHITE);
      BackColor(BLACK);
      RasterOp(zORz);
      MoveTo(sR.Xmin+4,sR.Ymax - 4);
      TextAlign(alignLeft,alignBottom);
      DrawString(tbuf);
      RasterOp(zREPz);
   }
   else
   {
      /* A bit touchier. We want to not overwrite. Try it first. */
      char *p;
      int col = 0;
      int row = 23;
      p = strtok(tbuf," ");
      while(1)
      {
         int n;
         char tbuf2[30];

         sprintf(tbuf2,"%s ",p);
         n = strlen(tbuf2);
         if (col + n >= 40)
         {
            row++;
            col = 0;
         }

         DisplayString13(tbuf2,col,row);
         col += n;
         p = strtok(NULL," ");
         if (!p)
            break;

      }
   }

}

void do_rendering(void)
{
   rect savedRect = planetRect;
   char gifname[128];
   rect R;
   int cleared = 0;
   int gifval = true;
   ticking = 1;
   aborted = 0;

   /* Find the name of the file we want to save it as, maybe */
   if (renderto == 1)
   {
      aborted = !select_file("Output file name","*.GIF",gifname,"GIF");
      LimitMouseRect(&sR);

      aborted = aborted || !Overwrite(gifname);
   }

   
   if (!aborted)
   {



   


      initgauss(rseed);
      RenderingMode = rendermode;
      if (changed)
         initialize_forgery(0,fracdim,power,meshsizes[mesh],&databuffer);
      changed = aborted;

      if(!aborted)
      {
         HideCursor();
         PushPlanetRects();
         cleared = 1;
         if (rendermode == 1)
         {
            R.Xmin = 0;
            R.Xmax = 319;
            R.Ymin = 0;
            R.Ymax = 199;
            planetRect = R;
            if (renderthing != 1 || DoClouds)
            {
               union REGS regs;
               regs.h.ah = 0;
               regs.h.al = 0x13;
               int86(0x10,&regs,&regs);
            }
         }
         else
         {
            planetRect = sR;
            PenColor(BLACK);
            PaintRect(&planetRect);
         }

         switch(renderthing)
         {
         case 0:
            /* clouds */
            initcloudcolors();
            genclouds(databuffer,meshsizes[mesh]);
            break;
         case 1:
            /* mountains */
            if (DoClouds)
            {
               float *saviour;

               int n = meshsizes[mesh];
               unsigned bl = ((n * n) + 1) * 2 * sizeof(float);
               /* Save the damned data, since we dick it */
               saviour = malloc(bl);
               memcpy(saviour,databuffer,bl);
               ticking = 2;
               initcloudcolors();
               genclouds(databuffer,meshsizes[mesh]);
               memcpy(databuffer,saviour,bl);
               free(saviour);
               ticking = 0;
            }
            PreScale = 0;
            if (!aborted)
               genproj(databuffer,meshsizes[mesh],elevfac);
            break;
         case 2:
            /* planet */
            initplanetcolors();
            initgauss(rseed);
      	   genplanet(databuffer, meshsizes[mesh], 1.0, -1.0, shang, siang,elevfac);
            break;
         case 3:
            initpurecolors();
            genpures(databuffer,meshsizes[mesh]);
            break;
         }

         if (parameter_stamping)
            StampParameters(fracdim,elevfac,power,mesh,terms,
               vturn,vdown,siang,shang);

         if (renderto == 1 && !aborted)
         {
            if (renderthing == 2)
               memcpy(clut,planet_clut,sizeof clut);
   			gifval = GifOutput(gifname,true);
         }

         while(!aborted && gifval)
         {
            event e;
            int n = KeyEvent(false,&e);
            if (n)
            {
               if (e.ASCII || e.ScanCode || (e.State & 0x700))
                  break;
            }
         }


      }

      if (cleared)
      {
         /* now restore the screen */
         if (rendermode == 1)
         {
            union REGS regs;
            regs.h.ah = 0;
            regs.h.al = mode;
            int86(0x10,&regs,&regs);
         }
         else
         {
            BackColor(BLACK);
            EraseRect(&sR);
         }
         planetRect = savedRect;

         initgauss(rseed);
         PaintMainWindow();
         PopPlanetRects();
         if (gifval == 0 && rendermode == 1)
            FileError(gifname,(FILE *)-1L);

      }
      CloseTicker();    /* possibly redundant */
      ShowCursor();
   }
}
static int items;
static rect *bR[20];

static void push(int n,int inout)
{
   if (n < 0)
      return;

   switch(n)
   {
   case 0:
   case 1:
   case 2:
   case 3:
      PushOrDoublePress(bR[n],inout,n == renderthing);
      break;
   case 4:
   case 5:
      PushOrDoublePress(bR[n],inout,n - 4 == rendermode);
      break;
   case 6:
   case 7:
      PushOrDoublePress(bR[n],inout,n - 6 == renderto);
      break;
   default:
      PushButton(bR[n],inout);
      break;
   }

   if (n == items - 1)
      ExtraHilite(bR[n],inout);
}



void render(void)
{
   rect R,tR;
   int width = 3*sR.Xmax/5;
   int height = 4*(FontHeight+4) + FontHeight + 8 + 16 + 4;
   int key;
   int err;
   int doit = 0;
   int i;
   items = 0;

   HideCursor();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Render an Image",BLACK);
   LimitMouseRect(&tR);

   R.Xmin = tR.Xmin + 2;
   R.Xmax = tR.Xmax - 2;
   R.Ymin = tR.Ymin + FontHeight + 8;
   R.Ymax = R.Ymin + FontHeight + 4;
   renderthingFrame = R;

   CreateRadioPanel(&renderthingFrame,renderthings,
      renderthingRects,4,renderthing);
   for(i=0;i<4;i++)
      bR[items++] = &renderthingRects[i];

   R = renderthingFrame;

   OffsetRect(&R,0,R.Ymax-R.Ymin + 4);
   rendermodeFrame = R;

   CreateRadioPanel(&rendermodeFrame,rendermodemsgs,
      rendermodeRects,2,rendermode);
   for(i=0;i<2;i++)
      bR[items++] = &rendermodeRects[i];

   R = rendermodeFrame;
   OffsetRect(&R,0,R.Ymax-R.Ymin + 4);
   rendertoFrame = R;

   CreateRadioPanel(&rendertoFrame,rendertomsgs,
      rendertoRects,2,renderto);
   for(i=0;i<2;i++)
      bR[items++] = &rendertoRects[i];

   /* Now create the individual buttons */
   R = rendertoFrame;
   OffsetRect(&R,0,R.Ymax-R.Ymin + 8);
   doitFrame = R;
   CreateRadioPanel(&doitFrame,doitmsgs,doitRects,3,-1);
   for(i=0;i<3;i++)
      bR[items++] = &doitRects[i];


   current_item = items - 1;
   push(current_item,true);
   move_to_corner(bR[current_item]);




   ShowCursor();
   while(1)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int button = (e.State & 0x700) >> 8;
      int wait=false;
      int last_item = current_item;

      key = 0;

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

         /* Where is the fucker? */
         if (button == swLeft)
         {
            key = 0x0d;
            wait=true;
         }

         else if (button == swRight)
         {
            key = 0x1b;
            wait=true;
         }

      }

      if (key == XF1 || (key == 0x0d && XYInRect(X,Y,&doitRects[0])))
      {
         PushCursorPosition();
         PaintRadioButton(&doitRects[0],true,true,doitmsgs[0]);
         if (wait)
            KeyEvent(true,&e);
         helptext("forgrend.hlp");
         PaintRadioButton(&doitRects[0],false,false,doitmsgs[0]);
         LimitMouse(tR.Xmin,tR.Ymin,tR.Xmax,tR.Ymax);
         PopCursorPosition();
         continue;
      }



      if (key == 0x1b || (key == 0x0d && XYInRect(X,Y,&doitRects[1])))
      {
         PaintRadioButton(&doitRects[1],true,true,doitmsgs[1]);
         if (wait)
            KeyEvent(true,&e);
         PaintRadioButton(&doitRects[1],false,false,doitmsgs[1]);
         break;
      }

      if (key == 0x0d)
      {
         CheckRadioButtons(X,Y,renderthingRects,4,&renderthing,renderthings);
         if (hasVGA)
            CheckRadioButtons(X,Y,rendermodeRects,2,&rendermode,rendermodemsgs);
         CheckRadioButtons(X,Y,rendertoRects,2,&renderto,rendertomsgs);

         if (XYInRect(X,Y,&doitRects[2]))
         {
            PaintRadioButton(&doitRects[2],true,true,doitmsgs[2]);
            ExtraHilite(&doitRects[2],true);
            /* wait for the key to come up */
            if (wait)
               KeyEvent(true,&e);
            PaintRadioButton(&doitRects[2],false,false,doitmsgs[2]);
            ExtraHilite(&doitRects[2],false);
            doit = true;
            break;
         }

      }
      navigate(key,lefts,rights,uppers,downers,items,bR,&current_item);

      if (current_item != last_item)
      {
         push(last_item,false);
         push(current_item,true);
      }


   }
      
   HideCursor();
   PopRect(&err);
   PopCursorPosition();
   LimitMouse(sR.Xmin,sR.Ymin,sR.Xmax,sR.Ymax);
   ShowCursor();

   if (doit)
      do_rendering();
   RenderingMode = 0;
}

         

         





   

