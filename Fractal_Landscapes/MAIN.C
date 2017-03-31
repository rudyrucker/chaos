/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal codes of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.	                                                     *						                                                           *
******************************************************************************/

#include "forge.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

/**/


double shang=0.785398,siang=0.331613;

slider fractal_slider = {
   "Fractal Dimension",
   0.0,
   0.0,
   2.0,3.0
   };

slider power_slider = {
   "Power Factor",
   0.0,0.0,
   0.0,2.0
   };

slider elevation_slider = {
   "Height",
   0.0,0.0,
   0.0,2.0
   };

slider *main_sliders[] = {
   &fractal_slider,
   &elevation_slider,
   &power_slider,
   };

double power_slider_value;
double *slider_vals[] = {
   &fracdim,&elevfac,&power_slider_value
   };


float *databuffer=NULL;
char *meshsizelabels[] = {"8","16","32","64","128"};
char *termlabels[] = {"2","4","8","16","32","64","128"};

animation_parameters animation_start,animation_end;

/* Metawindows stuff */
int CommPort = 0;
metaPort *thePort;
bitmap *theBitmap;
rect sR;
int FontHeight;
int StringWidthX;
int MenuBackColor = 13;
int MenuTextColor = 7;
int ButtonBackColor = 8;
int ButtonFrameColor = 0;
int ButtonTextColor = 2;
int mode=0;
unsigned int rseed,seed;
int RenderingMode=0;
int histogrammer = 0;

short brightpal[] = {
		0	,	//Black		 0
		0x3C,	//Red		 1
		0x3E,	//Yellow 	 2
		0x3A,	//Green		 3
		0x3B,	//Cyan		 4
		0x39,	//Blue		 5
		0x3D,	//Magenta	 6
		0x3F,	//White (intense) 7
		0x38,	//Dark grey	 8
		0x4,	//Light red	 9
      0x06,  //yellow also		10
		0x02,	//Light green	11
		0x03,	//Light cyan	12
		0x01,	//Light blue	13
		0x05,	//Light magenta	14
		0x07,	//Light grey	15
      };


rect menuRect;
rect tweakRect;
rect clockRect;
rect planetRect;
rect lineRect;
rect bigPlanetRect;
rect littlePlanetRects[4];
rect planetRect;
rect wholescreenRect;

rect meshsizeframeRect;
rect meshsizeRects[10];
rect termsframeRect;
rect termsRects[10];
int terms=3;

text_button randomRect;
rect randomizeRect;


rect quitRect;

rect animationframeRect;
rect animationRects[3];
char *animationlabels[] = {
   "Start","End"
   };



int hasVGA = 0;
int changed = 1;
int aborted = 0;
rect *mainRects[50];
int togglers[50];

static int items;
static int meshbase;
static int termsbase;
static int animationbase;
int prog_init = 0;



rect mainButtonRects[10];

char *mainButtonTexts[] = {
   "F1 for HELP",
   "F2 File    ",
   "F3 Options ",
   "F4 Planet  ",
   "F5 Clouds  ",
   "F6 Mountain",
   "F7 Contour ",
   "F8 Render  ",
   NULL
   };

enum {F1HELP,F2FILES,F3OPTIONS,F4PLANET,F5CLOUDS,F6MOUNTAIN,
      F7CRATERS, F8RENDER};

double aspect;
int meshsizes[] = {8,16,32,64,128};
int termsizes[] = {2,4,8,16,32,64,128};
int mesh = 3;
int WireFraming = 0;
int currentlittlerect = 0;
int currentmainitem = 0;

struct lrp {
   double fracdim;
   double elevfac;
   double power;
   double vdown;
   double vturn;
   double shang;
   double siang;
   int rseed;
   int mesh;
   int terms;
} littleRectParams[4] = {
   {2.6,1,0.822119,30,30,0.785398,0.331613,25712,3,3},
   {2.6,1,0.822119,30,30,0.785398,0.331613,25712,3,3},
   {2.6,1,0.822119,30,30,0.785398,0.331613,25712,3,3},
   {2.6,1,0.822119,30,30,0.785398,0.331613,25712,3,3},
};

int hasVGA;
int detectmode(void)
{
	int n = QueryGrafix();
	int n2 = n & 0x300;


	if (n == -1)
		return n;

	/* We want either 0x12 for a VGA or 0x10 for an EGA. */
	hasVGA = (n2 & 0x100);

	if (n2 == 0x200)
		return 0x10;
	if (n2 == 0x300)
		return 0x12;

	return n;
}

char planetsavefile[128];

void PushPlanetRects(void)
{
   int i;
   image *im;
   FILE *fd;
   int ok = true;

   HideCursor();
   TempFileName(planetsavefile,"FORGESAV.TMP");
   fd = fopen(planetsavefile,"wb");
   if (fd)
   {

      for(i=0;ok && i<4;i++)
      {
         rect R = littlePlanetRects[i];
         int savesize = (int)ImageSize(&R);
         im = malloc(savesize);
         ReadImage(&R,im);
         ok &= savesize == fwrite(im,1,savesize,fd);
         free(im);
      }
      fclose(fd);
      if (!ok)
         remove(planetsavefile);
   }
   ShowCursor();
}


void _PopPlanetRects(char *filename)
{
   int i;
   image *im;
   FILE *fd;
   HideCursor();
   fd = fopen(filename,"rb");
   if (fd)
   {
   
      for(i=0;i<4;i++)
      {
         rect R = littlePlanetRects[i];
         int savesize = (int)ImageSize(&R);
         im = malloc(savesize);
         fread(im,savesize,1,fd);
         WriteImage(&R,im);
         free(im);
      }
      fclose(fd);
   }
   ShowCursor();
}

void PopPlanetRects(void)
{
   _PopPlanetRects(planetsavefile);
   remove(planetsavefile);
}


void DisplayRseed(void)
{
   PaintNumberBoxEntry(&randomRect,(double)rseed,GS_UNSIGNED);
}
   

void saveLittleRectParams(int n)
{
   struct lrp *z = &littleRectParams[n];

   z->fracdim = fracdim;
   z->elevfac = elevfac;
   z->power = power;
   z->vdown = vdown;
   z->vturn = vturn;
   z->rseed = rseed;
   z->mesh = mesh;
   z->siang = siang;
   z->shang = shang;
   z->terms = terms;
}

void getLittleRectParams(int n)
{
   struct lrp *z = &littleRectParams[n];

   fracdim = z->fracdim;
   power = z->power;
   elevfac = z->elevfac;
   vdown = z->vdown;
   vturn = z->vturn;
   siang = z->siang;
   shang = z->shang;
   terms = z->terms;


   rseed = z->rseed;
   mesh = z->mesh;
}


void restoreLittleRectParams(int n)
{
   int i;
   double vals[7];

   getLittleRectParams(n);
   vals[0] = fracdim;
   vals[2] = log((power) + 1);
   vals[1] = elevfac;
   vals[3] = vdown;
   vals[4] = vturn;
   vals[5] = siang;
   vals[6] = shang;

   SetMeshButton();
   SetTermsButton();
   DisplayRseed();

   for(i=0;i<3;i++)
   {
      slider *s = main_sliders[i];
      if (s->value != vals[i])
      {
         s->value = vals[i];
         reposition_slider_v(main_sliders[i],false);
      }
   }
}

void SetAllSliders(void)
{
   int i;
   for(i=0;i<3;i++)
   {
      slider *s = main_sliders[i];
      if (s->value != *slider_vals[i])
      {
         s->value = *slider_vals[i];
         reposition_slider_v(main_sliders[i],false);
      }
   }
}

void SetMeshButton()
{
   int i;

   for(i=0;i<4;i++)
   {
      PaintRadioButton(&meshsizeRects[i],
         i==mesh,i==mesh,meshsizelabels[i]);
      togglers[meshbase+i] = (i!=mesh);
   }
}

void SetTermsButton()
{
   int i;
   for(i=0;i<6;i++)
   {
      PaintRadioButton(&termsRects[i],
         i==terms,i==terms,termlabels[i]);
      togglers[termsbase+i] = (i != terms);
   }
}


static void push(int n,int inout)
{
   if (n == -1)
      return;

   switch(togglers[n])
   {
   case 0:        /* a radio button */
      DoublePress(mainRects[n],inout,RED);
      break;
   case 1:
      PushButton(mainRects[n],inout);
      break;
   case 2:
      InvertInsides((text_button *)mainRects[n]);
      break;
   case 3:
      PushButton(mainRects[n],inout);
      ExtraHilite(mainRects[n],inout);
      break;
   }
}
      





void PaintQuitRect(int hilite)
{
   int cx,cy;

   rect R = quitRect;
   HideCursor();
   PenColor(hilite ? RED : DARKGRAY);
   PaintRect(&R);
   PushButton(&R,hilite);
   ExtraHilite(&R,hilite);
   PenColor(WHITE);
   BackColor(hilite ? RED : DARKGRAY);
   Centers(&R,&cx,&cy);
   TextAlign(alignCenter,alignTop);
   MoveTo(cx,R.Ymin + 2);
   DrawString("Alt-X");
   MoveTo(cx,R.Ymin + 2 + FontHeight);
   DrawString("to Exit");
   ShowCursor();
}

void PaintButtons(void)
{
   rect R;
   int i;

   R.Xmin = 0;
   R.Xmax = 99;
   R.Ymin = 0;
   R.Ymax = sR.Ymax;
   HideCursor();
   menuRect = R;
   PenColor(MENUBACK);
   PaintRect(&R);

   items = 0;

   for(i=0;mainButtonTexts[i];i++)
   {

      R.Xmin = 3;
      R.Xmax = menuRect.Xmax - 3;
      R.Ymin = i * (3*FontHeight/2) + 4;
      R.Ymax = R.Ymin + FontHeight + 3;

      mainButtonRects[i] = R;
      togglers[items] = 1;
      mainRects[items++] = &mainButtonRects[i];

      PaintRadioButtonBase(&R,false,false,mainButtonTexts[i],
         8,6,7);

   }

   /* And now we have the quit button. */
   R.Xmin = 3;
   R.Xmax = menuRect.Xmax - 3;
   R.Ymin = i * (3*FontHeight/2) + 4;
   R.Ymax = R.Ymin + 2*FontHeight + 3;
   quitRect = R;
   togglers[items] = 3;
   mainRects[items++] = &quitRect;

   PaintQuitRect(false);
   ShowCursor();

}

int elevation_slider_base;
int power_slider_base;
int dimension_slider_base;
void PaintEverythingElse(void)
{
   rect R,R1,R2;
   int i;
   int cx,cy;
   char *msize = "Mesh Size";
   int width;

   R.Xmin = 100;
   R.Xmax = sR.Xmax;
   R.Ymin = 0;
   R.Ymax = sR.Ymax/4;
   lineRect = R;
   xdots = R.Xmax + 1 - R.Xmin;
   ydots = R.Ymax + 1 - R.Ymin;


   PenColor(8);
   PaintRect(&R);

   R.Xmax = sR.Xmax;

   R.Ymin = lineRect.Ymax + 1;
   R.Ymax = sR.Ymax;
   R.Xmin = R.Xmax - (R.Ymax-R.Ymin + 1) * aspect;
   bigPlanetRect = R;

   SplitRectH(&R,&R1,&R2);
   SplitRectV(&R1,&littlePlanetRects[0],&littlePlanetRects[1]);
   SplitRectV(&R2,&littlePlanetRects[2],&littlePlanetRects[3]);

   R = planetRect = littlePlanetRects[currentlittlerect];

   lineRect.Xmin = bigPlanetRect.Xmin;


   PenColor(8);
   for(i=0;i<4;i++)
      FrameRect(&littlePlanetRects[i]);
   PenColor(MENUTEXT);
   FrameRect(&planetRect);


   R.Xmin = 100;
   R.Xmax = littlePlanetRects[0].Xmin - 1;
   R.Ymin = 0;
   R.Ymax = sR.Ymax; 
   tweakRect = R;
   PenColor(MENUBACK);
   PaintRect(&R);


   /* sliders... */
   fractal_slider.value = fracdim;
   create_slider(&fractal_slider,&tweakRect,1,1);
   dimension_slider_base = items;
   togglers[items] = 1;
   mainRects[items++] = &fractal_slider.bR;

   for(i=0;i<6;i++)
   {
      togglers[items] = 1;
      mainRects[items++] = &fractal_slider.zR[i];
   }

   elevation_slider.value = elevfac;
   create_slider(&elevation_slider,&tweakRect,1,
      fractal_slider.tR.Ymax + 4 - tweakRect.Ymin);
   elevation_slider_base = items;

   togglers[items] = 1;
   mainRects[items++] = &elevation_slider.bR;
   for(i=0;i<6;i++)
   {
      togglers[items] = 1;
      mainRects[items++] = &elevation_slider.zR[i];
   }

   power_slider_value = power_slider.value = log(power+1);

   create_slider(&power_slider,&tweakRect,1,1 +
      elevation_slider.tR.Ymax + 4 - tweakRect.Ymin);
   power_slider_base = items;
   togglers[items] = 1;
   mainRects[items++] = &power_slider.bR;
   for(i=0;i<6;i++)
   {
      togglers[items] = 1;
      mainRects[items++] = &power_slider.zR[i];
   }

   /* now create the box with the pushbuttoons */
   meshsizeframeRect = power_slider.tR;
   OffsetRect(&meshsizeframeRect,0,power_slider.tR.Ymax -
      power_slider.tR.Ymin + 4);

   meshsizeframeRect.Ymax = meshsizeframeRect.Ymin +
      FontHeight + 4 + 3*FontHeight/2;

   PenColor(0);
   FrameRect(&meshsizeframeRect);
   TextAlign(alignCenter,alignTop);
   Centers(&meshsizeframeRect,&cx,&cy);
   MoveTo(cx,meshsizeframeRect.Ymin + 2);
   PenColor(BUTTONTEXT);
   BackColor(MENUBACK);
   DrawString(msize);

   CreateRadioPanel(&meshsizeframeRect,
      meshsizelabels,meshsizeRects,4,mesh);
   meshbase = items;
   for(i=0;i<4;i++)
   {
      togglers[items] = (i != mesh);
      mainRects[items++] = &meshsizeRects[i];
   }

   termsframeRect = meshsizeframeRect;
   OffsetRect(&termsframeRect,0,meshsizeframeRect.Ymax -
      meshsizeframeRect.Ymin + 4);


   PenColor(0);
   FrameRect(&termsframeRect);
   TextAlign(alignCenter,alignTop);
   Centers(&termsframeRect,&cx,&cy);
   MoveTo(cx,termsframeRect.Ymin + 2);
   PenColor(BUTTONTEXT);
   BackColor(MENUBACK);
   DrawString("Terms");

   CreateRadioPanel(&termsframeRect,
      termlabels,termsRects,6,terms);
   termsbase = items;
   for(i=0;i<6;i++)
   {
      togglers[items] = (i != terms);
      mainRects[items++] = &termsRects[i];
   }

   animationframeRect = termsframeRect;
   OffsetRect(&animationframeRect,0,termsframeRect.Ymax -
      termsframeRect.Ymin + 4);

   PenColor(0);
   FrameRect(&animationframeRect);
   TextAlign(alignCenter,alignTop);
   Centers(&animationframeRect,&cx,&cy);
   MoveTo(cx,animationframeRect.Ymin + 2);
   PenColor(BUTTONTEXT);
   BackColor(MENUBACK);
   DrawString("Animation");

   CreateRadioPanel(&animationframeRect,
      animationlabels,animationRects,2,-1);
   animationbase = items;
   for(i=0;i<2;i++)
   {
      togglers[items] = 1;
      mainRects[items++] = &animationRects[i];
   }

   R = animationframeRect;
   OffsetRect(&R,0,animationframeRect.Ymax -
      animationframeRect.Ymin + 4);

   R.Ymax = R.Ymin + FontHeight + 4;
   Centers(&R,&cx,&cy);

   R.Xmax = cx - 4;
   R.Xmin = animationframeRect.Xmin + 4;

   randomizeRect = R;   
   PaintRadioButton(&R,false,false,"Randomize");
   togglers[items] = 1;
   mainRects[items++] = &randomizeRect;


   width = StringWidthX * 8;
   R.Xmin = cx + (animationframeRect.Xmax - cx)/2 - width/2;
   R.Xmax = R.Xmin + width - 1;

   randomRect.nR = R;
   PenColor(DARKGRAY);
   PaintRect(&R);
   PushButton(&R,true);
   togglers[items] = 2;
   mainRects[items++] = (rect *)&randomRect;

   DisplayRseed();
   clear_uparams();
   uparam(meshsizes[mesh]);


}

void PaintMainWindow(void)
{
   aspect = theBitmap->pixResX/(double)theBitmap->pixResY;

   LoadPalette(0,15,brightpal);

   PaintButtons();
   PaintEverythingElse();

}

mapArray buttonmap = {0,0,0,0,0,0,0,0};
extern int NoEdges;

int main(int argc, char **argv)
{
	int i;
   event e;
   int exitval = 0;
   char tbuf[128];
   int current_item = 0;
   extern int rendermode;
   int gif_ok = true;


	extern int disk_error_handler(int errval, int ax, int bp, int si);
	if (!memok(20712L))	/* Added up mallocs in comprs.c */
		gif_ok = 0;

   if (!memok(64L*65L*2*sizeof(float)+640L*(2*sizeof(double)+3L) + 64L*64L))
   {
		fprintf(stderr, "\n\nSorry, Not enough memory to run FORGE. Press any key to continue...\n");
      exit(-1);
   }
	harderr(disk_error_handler);

   rseed = 25712;

   initgauss(rseed);
   for(i=0;i<4;i++)
      littleRectParams[i].rseed = rseed;

   animation_start.dimension = animation_end.dimension = fracdim;
   animation_start.height = animation_end.height = elevfac;
   animation_start.power = animation_end.power = power;
   animation_start.elevation = animation_end.elevation = vdown;
   animation_start.azimuth = animation_end.azimuth = vturn;
   animation_start.hour = animation_end.hour = shang;
   animation_start.season = animation_end.season = siang;
   animation_start.terms = animation_end.terms = terms;


	while (argc > 1)
	{
		if (argv[1][0] == '-')
		{
			switch (argv[1][1])
			{
			case 'e':
				mode = 0x10;
				break;
         case 'v':
            mode = 0x12;
            break;


         }   
		}
		argv++;
		argc--;
	}


	if (!mode)
		mode = detectmode();

   if (mode == 0x10)
      hasVGA = 0;
   else
      hasVGA = 1;

   if(hasVGA)
      rendermode = 1;
   else
      rendermode = 0;

	i = InitGrafix(mode == 0x12 ? -EGA640x480 : -EGA640x350);
	if (i != 0)
	{
		printf("Error: Metagraphics not installed. Aborting.\n");
		exit(-1);
	}



	CommPort = QueryComm();

	if (CommPort & MsDriver)
		CommPort = MsDriver;
	else if (CommPort & 2)
		CommPort = MsCOM2;
	else if (CommPort & 1)
		CommPort = MsCOM1;
	if (CommPort)
		InitMouse(CommPort);
	/* We're going to event-drive the whole thing */
	EventQueue(true);
	TrackCursor(true);
//	LoadDefaultPalette(startpal);
	GetPort(&thePort);
	sR = thePort->portRect;
	SetDisplay(GrafPg0);
   theBitmap = thePort->portBMap;
	FontHeight = thePort->txFont->chHeight;
	StringWidthX = StringWidth("X");
	LimitMouseRect(&sR);
   CursorMap(buttonmap);

   getLittleRectParams(0);
   PaintMainWindow();
   _PopPlanetRects((mode == 0x12) ? "forge480.dat" : "forge350.dat");

   if (!gif_ok)
		ErrorBox("There may not be enough memory to save or view a Gif.");

   if (hasVGA && !memok(2*64L*1024L))
      ErrorBox("There may not be enough memory to animate.");


   ShowCursor();
   
   while(1)
   {
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int button = (e.State & 0x700) >> 8;
      int key=0;
      int newvals=0;
      int last_item = current_item;

      aborted = 0;
      
      LimitMouseRect(&sR);
      for(i=0;i<items;i++)
      {
         current_item = -1;

         if (XYInRect(X,Y,mainRects[i]))
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

         if (key == '4' && ((e.ScanCode << 8) == XLARROW))
            key = '4' + XLARROW;
         else if (key == '6' && ((e.ScanCode << 8) == XRARROW))
            key = '6' + XRARROW;
         else if (key == XLARROW && (e.State & 3))
            key = '4' + XLARROW;
         else if (key == XRARROW && (e.State & 3))
            key = '6' + XRARROW;


      
         
         if (button == swRight)
         {
            for(i=0;i<4;i++)
            {
               if (XYInRect(X,Y,&littlePlanetRects[i]))
               {
                  key = XALT1 + 0x100 * i;
                  goto gotkey;
               }
            }
         }

         if (key == 0x0d)
            button = swLeft;

         if (button == swLeft)
         {
            if (XYInRect(X,Y,&quitRect))
            {
               key = XALTX;
               goto gotkey;
            }

            for(i=0;mainButtonTexts[i];i++)
            {
               if (XYInRect(X,Y,&mainButtonRects[i]))
               {
                  key = XF1 + 0x100 * i;
                  goto gotkey;
               }
            }

            if (XYInRect(X,Y,&randomizeRect))
            {
               key = 'z';
               goto gotkey;
            }
            for(i=0;i<4;i++)
            {
               if (XYInRect(X,Y,&littlePlanetRects[i]))
               {
                  key = '1' + i;
                  goto gotkey;
               }
            }

            for(i=0;i<4;i++)
            {
               if (XYInRect(X,Y,&meshsizeRects[i]))
               {
                  if (i != mesh)
                  {
                     PaintRadioButton(&meshsizeRects[mesh],
                        false,false,meshsizelabels[mesh]);
                     togglers[meshbase+mesh] = 1;
                     mesh = i;
                     togglers[meshbase+mesh] = 0;
                     PaintRadioButton(&meshsizeRects[mesh],
                        true,true,meshsizelabels[mesh]);
                     DoublePress(&meshsizeRects[mesh],true,RED);
                     initgauss(rseed);
                     uparam(meshsizes[mesh]);
                     changed = 1;
                     goto gotkey;
                  }
               }
            }

            for(i=0;i<6;i++)
            {
               if (XYInRect(X,Y,&termsRects[i]))
               {
                  if (i != terms)
                  {
                     PaintRadioButton(&termsRects[terms],
                        false,false,termlabels[terms]);
                     togglers[termsbase+terms] = 1;
                     terms = i;
                     togglers[termsbase+terms] = 0;
                     PaintRadioButton(&termsRects[terms],
                        true,true,termlabels[terms]);
                     DoublePress(&termsRects[terms],true,RED);
                     initgauss(rseed);
                     uparam(meshsizes[mesh]);
                     changed = 1;
                     goto gotkey;
                  }
               }
            }
            for(i=0;i<3;i++)
            {
               slider *s = main_sliders[i];

               if (XYInRect(X,Y,&s->bR))
               {
                  LimitMouseRect(&s->sR);
                  while(button == swLeft)
                  {
                     
                     reposition_slider_X_base(s,X,0,true,true);
                     KeyEvent(false,&e);
                     button = (e.State & 0x700) >> 8;
                     X = e.CursorX;
                     Y = e.CursorY;
                     fracdim = fractal_slider.value;
                     elevfac = elevation_slider.value;
                     power = exp(power_slider_value = power_slider.value)-1;
                     initgauss(rseed);
                     uparam(meshsizes[mesh]);

                  }
                  changed = 1;
                  newvals = 1;
                  LimitMouseRect(&sR);
                  goto gotkey;
               }
               else if (XYInRect(X,Y,&s->sR))
               {
                  reposition_slider_X_base(s,X,0,true,true);
                  currentmainitem = i;
                  changed = 1;
                  newvals = 1;
                  goto gotkey;
               }
               else
               {
                  int j;
                  static double incs1[] = {-1,-.1,-.01,.01,.1,1};

                  for(j=0;j<6;j++)
                  {
                     if (XYInRect(X,Y,&s->zR[j]))
                     {
                        short lasttime = e.Time;
                        int first = true;
                        currentmainitem = i;
                        s->value += incs1[j];
                        reposition_slider_v(s,false);
                        changed = 1;
                        newvals = 1;
                        PushButton(&s->zR[j],true);
                        while(button == swLeft && XYInRect(X,Y,&s->zR[j]))
                        {
                           KeyEvent(false,&e);
                           button = (e.State & 0x700) >> 8;
                           X = e.CursorX;
                           Y = e.CursorY;
                           if ((first && (e.Time > lasttime + 4)) ||
                                 (!first && (e.Time > lasttime + 2)))
                           {
                              s->value += incs1[j];
                              reposition_slider_v(s,false);
                              first = false;
                              fracdim = fractal_slider.value;
                              elevfac = elevation_slider.value;
                              power = exp(power_slider_value = power_slider.value)-1;
                              initgauss(rseed);
                              uparam(meshsizes[mesh]);
                              newvals=0;
                           }
                        }
                        PushButton(&s->zR[j],false);
                        goto gotkey;
                                 
                           

                     }
                  }
               }
            }

            if (XYInRect(X,Y,&randomRect.nR))
            {
               double z = rseed;
               if (GetNumber(&randomRect,&z,GS_INTEGER,0.0,32767.0))
               {
                  rseed = z;
                  seed = rseed;
                  newvals = 1;
                  changed = 1;
                  DisplayRseed();
               }
               InvertInsides(&randomRect);
               goto gotkey;
            }



            if (XYInRect(X,Y,&animationRects[0]))
            {
               key = 'S';
               goto gotkey;
            }
            if (XYInRect(X,Y,&animationRects[1]))
            {
               key = 'E';
               goto gotkey;
            }


         }

      }


gotkey:
      if (key == XALTX)
      {
         PaintQuitRect(true);
         WaitForNothing();
         if (cancel_ok_msg("EXIT: Are you sure?"))
            exitval = 1;
         PaintQuitRect(false);
         if (exitval)
            break;
      }

      if (key == XALTQ)
      {
         PaintQuitRect(true);
         WaitForNothing();
         if (cancel_ok_msg("QUIT to DOS: Are you sure?"))
            exitval = 2;
         PaintQuitRect(false);
         if (exitval)
            break;
      }


      if (key >= XF1 && key <= XF8)
      {
         int n = (key - XF1) >> 8;
         PaintRadioButtonBase(&mainButtonRects[n],true,true,mainButtonTexts[n],
            DARKGRAY,RED,WHITE);
         WaitForNothing();
      }

      navigate(key,NULL,NULL,(int *)-1,(int *)-1,items,mainRects,&current_item);
      switch(key)
      {
      case XF1:
         helptext("forge.hlp");
         break;

      case XALTS:
         if (select_file("Save Forgery Parameters","*.FOR",tbuf,"FOR") && Overwrite(tbuf))
            SaveParameters(tbuf);
         break;
      case XALTL:
         if (select_file("Load Forgery Parameters","*.FOR",tbuf,"FOR"))
            LoadParameters(tbuf);
         break;
      case XALTG:
      case XALTH:
         if (select_file("Save Screen as GIF image","*.GIF",tbuf,"GIF") && Overwrite(tbuf))
            GifOutput(tbuf,true);
         break;
      case XALTF:
         gif_viewer();
         break;
      case XALTW:
         InfoBox();
         break;
      case 'a':
      case 'A':
         if (hasVGA)
            animate();
         else
            ErrorBox("Sorry, you need a VGA to animate");
         break;
      case 'v':
      case 'V':
         if (hasVGA)
            view_animation();
         else
            ErrorBox("Sorry, you need a VGA to view animations.");
         break;
      case 'x':
      case 'X':
         dxfout();
         break;

      case XF2:
         files();
         break;
      case 'r':
      case 'R':
         /* This is a shortcut to RENDER with the last settings. */
         PaintRadioButton(&mainButtonRects[7],true,true,mainButtonTexts[7]);
         do_rendering();
         PaintRadioButton(&mainButtonRects[7],false,false,mainButtonTexts[7]);
         RenderingMode = 0;
         push(current_item,true);
         break;

      case 'S':
      case 's':
         PaintRadioButton(&animationRects[0],true,true,animationlabels[0]);
         WaitForNothing();
         animation_start.dimension = fracdim;
         animation_start.height = elevfac;
         animation_start.power = power;
         animation_start.elevation = vturn;
         animation_start.azimuth = vdown;
         animation_start.hour = shang;
         animation_start.season = siang;
         animation_start.terms = terms;
         sound(1760);
         delay(10);
         nosound();
         PaintRadioButton(&animationRects[0],false,false,animationlabels[0]);
         if(current_item == animationbase)
            PushButton(&animationRects[0],true);

         break;

      case 'e':
      case 'E':
         PaintRadioButton(&animationRects[1],true,true,animationlabels[1]);
         WaitForNothing();
         animation_end.dimension = fracdim;
         animation_end.height = elevfac;
         animation_end.power = power;
         animation_end.elevation = vturn;
         animation_end.azimuth = vdown;
         animation_end.hour = shang;
         animation_end.season = siang;
         animation_end.terms = terms;
         sound(1760);
         delay(10);
         nosound();
         PaintRadioButton(&animationRects[1],false,false,animationlabels[1]);
         if(current_item == animationbase+1)
            PushButton(&animationRects[1],true);
         break;

      case XLARROW:
      case XRARROW:
      case XLARROW+'4':
      case XRARROW+'6':
         if (current_item >= 9 && current_item <= 29)
         {
            int currentmainitem = (current_item - 9) / 7;
            slider *s = main_sliders[currentmainitem];
            int inme = XYInRect(X,Y,&s->bR);
            double inc;
            switch(key)
            {
            case XLARROW:
               inc = -.01;
               break;
            case XLARROW+'4':
               inc = -.1;
               break;
            case XRARROW:
               inc = .01;
               break;
            case XRARROW+'6':
               inc = .1;
               break;
            }
        

            s->value += inc;
            reposition_slider_v(s,false);
            if (inme)
               move_to_corner(&s->bR);
            current_item = -1;
            newvals = 1;
         }
         break;

      case XHOME:
      case XEND:
         if (current_item >= 9 && current_item <= 29)
         {
            int currentmainitem = (current_item - 9) / 7;
            slider *s = main_sliders[currentmainitem];
            s->value = (key == XHOME) ? s->min : s->max;
            reposition_slider_v(s,false);
            newvals = 1;
         }
         break;



      case 'M':
      case 'm':

         PaintRadioButton(&meshsizeRects[mesh],
            false,false,meshsizelabels[mesh]);
         togglers[meshbase+mesh] = 1;
         if (key == 'M')
         {
            mesh++;
            if (mesh == 4)
               mesh = 0;
         }
         else
         {
            mesh--;
            if (mesh < 0)
               mesh = 3;
         }
         togglers[meshbase+mesh] = 0;
         PaintRadioButton(&meshsizeRects[mesh],
            true,true,meshsizelabels[mesh]);
         newvals = 1;
         changed = 1;

         break;

      case 'T':
      case 't':

         PaintRadioButton(&termsRects[terms],
            false,false,termlabels[terms]);
         togglers[termsbase+terms] = 1;
         if (key == 'T')
         {
            terms++;
            if (terms == 6)
               terms = 0;
         }
         else
         {
            terms--;
            if (terms < 0)
               terms = 5;
         }
         togglers[termsbase+terms] = 0;
         PaintRadioButton(&termsRects[terms],
            true,true,termlabels[terms]);
         changed = 1;
         newvals = 1;
         break;

      case 'P':
      case 'p':
         current_item = 23;
         move_to_corner(mainRects[current_item]);
         break;
      case 'h':
      case 'H':
         current_item = 16;
         move_to_corner(mainRects[current_item]);
         break;
      case 'F':
      case 'f':
         current_item = 9;
         move_to_corner(mainRects[current_item]);
         break;

      case XF3:
         i = rseed;
         options();
         if (rseed != i)
            newvals = 1;
         break;

      case 'z':
      case 'Z':
         PaintRadioButtonBase(&randomizeRect,true,true,"Randomize",
            DARKGRAY,RED,WHITE);
         WaitForNothing();
         seeded=0;
         initseed();
         seed = rseed;
         newvals = 1;
         changed = 1;
         DisplayRseed();
         PaintRadioButtonBase(&randomizeRect,false,false,"Randomize",
            DARKGRAY,RED,WHITE);
         break;
      case XF4:
         ticking = 1;
         HideCursor();
         initgauss(rseed);
         PenColor(BLACK);
         PaintRect(&planetRect);
         PenColor(MENUTEXT);
         FrameRect(&planetRect);
         saveLittleRectParams(currentlittlerect);
         if (changed)
         	initialize_forgery(Planet, fracdim, power, meshsizes[mesh], &databuffer);
         changed = aborted;
         if (!aborted)
         {
            initplanetcolors();
            initgauss(rseed);
         	genplanet(databuffer, meshsizes[mesh], 1.0, -1.0, shang, siang,elevfac);
         }
         ShowCursor();
         break;
      case XF5:
         ticking = 1;
         initgauss(rseed);
         HideCursor();
         PenColor(BLACK);
         PaintRect(&planetRect);
         PenColor(MENUTEXT);
         FrameRect(&planetRect);
         saveLittleRectParams(currentlittlerect);
         if (changed)
            initialize_forgery(Clouds,fracdim,power,meshsizes[mesh],&databuffer);
         changed = aborted;
         if (!aborted)
         {
            initcloudcolors();
            genclouds(databuffer,meshsizes[mesh]);
         }
         ShowCursor();
         break;
      case XF6:
         ticking = 1;
         initgauss(rseed);
         HideCursor();
         PenColor(BLACK);
         PaintRect(&planetRect);
         PenColor(MENUTEXT);
         FrameRect(&planetRect);
         saveLittleRectParams(currentlittlerect);
         PreScale = 0;
         if (changed)
         	initialize_forgery(Mountains, fracdim, power, meshsizes[mesh], &databuffer);
         if (DoClouds && !aborted)
         {
            initcloudcolors();
            genclouds(databuffer,meshsizes[mesh]);
         }

         if (!aborted)
         	genproj(databuffer, meshsizes[mesh],elevfac);
         changed = aborted;
         ShowCursor();
         break;


      case XF7:
         ticking = 1;
         initgauss(rseed);
         HideCursor();
         PenColor(BLACK);
         PaintRect(&planetRect);
         PenColor(MENUTEXT);
         FrameRect(&planetRect);
         saveLittleRectParams(currentlittlerect);
         if (changed)
            initialize_forgery(Clouds,fracdim,power,meshsizes[mesh],&databuffer);
         changed = aborted;
         if (!aborted)
         {
            initpurecolors();
            genpures(databuffer,meshsizes[mesh]);
         }
         ShowCursor();
         break;

      case '1':
      case '2':
      case '3':
      case '4':
         HideCursor();
         PenColor(8);
         FrameRect(&planetRect);
         PenColor(MENUTEXT);
         currentlittlerect = key - '1';

         planetRect = littlePlanetRects[currentlittlerect];
         FrameRect(&planetRect);
         ShowCursor();
         break;

      case XALT1:
      case XALT2:
      case XALT3:
      case XALT4:
         i = (key - XALT1) >> 8;
         HideCursor();
         PenColor(8);
         FrameRect(&planetRect);
         PenColor(MENUTEXT);
         currentlittlerect = i;
         planetRect = littlePlanetRects[i];
         FrameRect(&planetRect);
         restoreLittleRectParams(i);
         ShowCursor();
         changed = 1;
         newvals = 1;
         break;


      case XF8:
         PushCursorPosition();
         render();
         RenderingMode = 0;
         PopCursorPosition();
         break;

      case 'E' - 'A' + 1:
         NoEdges ^= 1;
         break;

      }

      if (ticker_active)
         CloseTicker();    /* really only needed if aborted */

      if (key >= XF1 && key <= XF8)
      {
         int n = (key - XF1) >> 8;
         PaintRadioButtonBase(&mainButtonRects[n],false,false,mainButtonTexts[n],
            DARKGRAY,RED,WHITE);
         if (current_item == n)
            push(current_item,true);
         current_item = n;
      }


      if (newvals)
      {
         fracdim = fractal_slider.value;
         elevfac = elevation_slider.value;
         power = exp(power_slider_value = power_slider.value)-1;
         initgauss(rseed);
         uparam(meshsizes[mesh]);
         newvals=0;
      }

      if (last_item != current_item)
      {
         push(last_item,false);
         push(current_item,true);
      }
   }



   HideCursor();
   SetDisplay(TextPg0);
   StopEvent();
   StopMouse();

   return exitval;
}

