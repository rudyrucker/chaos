#include "menu.h"
#include <math.h>
#include <alloc.h>
#include <dir.h>

typedef struct {double x,y; } pair;

pair p1[] = {
   .5,0.42201835,
   0.75004548,0.0,
   0.25,0
   };
pair p2[] = {
   0.5,0.65137615,
   0.87284244,0.0,
   0.75004548,0.0,
   0.5733945,0.44954128,
   0.4266055,0.44954128,
   0.25,0.0,
   0.12715756,0.0
   };
pair p3[] = {
   .5,0.87155963,
   1.0,0.0,
   0.87284244,0.0,
   0.5733945,0.72477064,
   0.4266055,0.72477064,
   0.12715756,0.0,
   0.0,0.0
   };
pair p4[] = {
   0.57177274,1.0,
   1.0,1.0,
   1.0,0.0
   };
pair p5[] = {
   0.0,1.0,
   0.42822726,1.0,
   0.0,0.0
   };

pair a1[] = {
   .5,0.42201835,
   0.75004548,0.0,
   0.5733945,0.44954128,
   0.4266055,0.44954128,
   0.25,0
   };

pair a2[] = {
   0.87284244,0.0,
   0.5733945,0.72477064,
   0.4266055,0.72477064,
   0.12715756,0.0,
   0.5,0.65137615,
   };

pair a3[] = {
   0.0,0.0,
   0.42822726,1.0,
   0.57177274,1.0,
   1.0,0.0,
   .5,0.87155963,
   };




struct pn {
   pair *p;
   int n;
} logo[]  =
{
   p1,sizeof(p1)/sizeof(pair),
   p2,sizeof(p2)/sizeof(pair),
   p3,sizeof(p3)/sizeof(pair),
   p4,sizeof(p4)/sizeof(pair),
   p5,sizeof(p5)/sizeof(pair),
   NULL,0
   };

struct pn alogo[] = {
   a1,sizeof(a1)/sizeof(pair),
   a2,sizeof(a2)/sizeof(pair),
   a3,sizeof(a3)/sizeof(pair),
   NULL,0
};



point ppoints[40];
polyHead phead[5];
char fontname[128] = "chaos.fnt";
fontRcd *fonter;

void XDrawString(char *msg)
{
   while(*msg && !AbortCheck())
   {
      if (*msg != ' ')
         DrawChar(*msg);
      else
         MoveRel(10,0);
      msg++;
   }
}


void ailogo(int firsttimeflag)
{
   int i;
   int items=0;
   point *pp;
   rect R;
   dirRec di;
   event e;

   for(pp=ppoints,i=0;alogo[i].p;i++)
   {
      int j;
      pair *p = alogo[i].p;
      int x = 640/4 + p[0].x * 320;
      int y = (350/2 + 350*aspect/4) - p[0].y * 175 * aspect;
      int minx,maxx,miny,maxy;

      pp->X = minx = maxx = x;
      pp->Y = miny = maxy = y;
      pp++;
      phead[i].polyBgn = items++;
      
      for(j=1;j<alogo[i].n;j++,pp++,items++)
      {
         int x = 640/4 + p[j].x * 320;
         int y = (350/2 + 350*aspect/4) - p[j].y * 175 * aspect;
         pp->X = x;
         pp->Y = y;
         minx = min(minx,x);
         miny = min(miny,y);
         maxx = max(maxx,x);
         maxy = max(maxy,y);

         
      }
      pp->X = x;
      pp->Y = y;
      pp++;
      phead[i].polyEnd = items++;
      phead[i].polyRect.Xmin = minx;
      phead[i].polyRect.Ymin = miny;
      phead[i].polyRect.Xmax = maxx;
      phead[i].polyRect.Ymax = maxy;
   }
   PenColor(CYAN);
   for(i=0;i<3;i++)
   {
      if (AbortCheck())
         break;
      FramePoly(1,phead+i,ppoints);
   }
#ifdef NOISY
   if (firsttimeflag)
   {
      char *p = searchpath(fontname);
      if (p)
      {
         FileQuery(p,&di,1);
         fonter = (fontRcd *)farmalloc(di.fileSize*2);
         FileLoad(p,(char *)fonter,(int)di.fileSize*2);
         SetFont(fonter);
         TextFace(cProportional);
         MoveTo(0,0);
         PenColor(WHITE);
         RasterOp(zREPz);

         TextAlign(alignLeft,alignTop);
         XDrawString("AUTODESK presents...");
         TextAlign(alignLeft,alignBottom);
         MoveTo(0,349);
         XDrawString("James Gleick's CHAOS: The So\x7cware");
         TextFace(cNormal);
         SystemFont(16);
      }
   }
#else
   if (firsttimeflag)
   {
      PenColor(WHITE);
      BackColor(BLACK);
      TextAlign(alignCenter,alignTop);
      MoveTo(320,4);
      DrawString("James Gleick's CHAOS: The Software");
      MoveTo(320,4+FontHeight + 4);
      DrawString("Please wait while program loads");
      TextAlign(alignCenter,alignBottom);
      MoveTo(320,345);
      DrawString("\x1d Copyright 1990 by Autodesk, Inc.");
   }
#endif

}



















