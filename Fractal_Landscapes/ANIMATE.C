#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include "forge.h"
#include "aaflisav.h"
#include <dos.h>
#include <sys\stat.h>

static Vscreen *bs;
static Jfile flifd;
static Fli_head fh;
extern int whichpic;
unsigned char sys_cmap[768];	/* Public VGA colour map */
int DoTweening = 0;

int animatething = 0;
static rect animatethingFrame;
static rect animatethingRects[4];
static char *animatethings[] = {
   "Clouds",
   "Mountains",
   "Planet",
   "Contour"
   };

static char *doitmsgs[] = {
   "F1 for HELP",
   "ESC to Cancel",
   "ANIMATE"
   };
static rect doitFrame;
static rect doitRects[3];

static rect paramFrames[2][7];
static rect paramRects[2][7];
static text_button paramTBS[2][7];

static double params[2][7];

static rect framesFrame,framesRect;
static text_button framesTB;

static int frames=10;

static char *parammsgs[] = {
   "Dimension: ",
   "Height:    ",
   "Power:     ",
   "Elevation: ",
   "Azimuth:   ",
   "Hour:      ",
   "Season:    "
   };

static int paramtypes[] = {
   GS_FLOAT,GS_FLOAT,GS_FLOAT,
   GS_UNSIGNED,GS_INTEGER,GS_INTEGER,GS_INTEGER
   };

struct {
   double lo,hi;
} param_limits[] = {
   2.0,3.0,
   0.0,2.0,
   0.0,2.0,
   0.0,90.0,
   -180.0,180.0,
   -24.0,24.0,
   -360.0,360.0
   };

int lefts[] = {
   3,0,1,2,
   11,12,13,14,15,16,17,
   4,5,6,7,8,9,10,
   18,
   21,19,20
   };

int rights[] = {
   1,2,3,0,
   11,12,13,14,15,16,17,
   4,5,6,7,8,9,10,
   18,
   20,21,19
   };

int uppers[] = {
   19,19,20,21,
   0,4,5,6,7,8,9,
   2,11,12,13,14,15,16,
   10,
   18,18,18
   };

int downers[] = {
   4,4,11,11,
   5,6,7,8,9,10,18,
   12,13,14,15,16,17,18,
   20,
   0,1,3
   };


static int create_fli(char *p, int n)
{
	bs = aa_alloc_mem_screen();

   if (!bs)
      return AA_ERR_NOMEM;

	return (flifd = fli_create(p, &fh, n));
}
static int dontscaleme;
static int save_next_fli(void)
{
   /* check to see if we need to massage these numbers */
   int scalem = 0;
   int i;

   if (!dontscaleme)
   {
      for(i=0;i<256*3;i++)
      {
         if (sys_cmap[i] > 63)
         {
            scalem = 1;
            break;
         }
      }
      if (scalem)
      {
         for(i=0;i<256*3;i++)
            sys_cmap[i] >>= 2;
      }
   }

	memcpy(aa_colors, sys_cmap, 256 * 3);

	i = fli_write_next(flifd, &fh, &aa_screen, bs);
	aa_copy_screen(&aa_screen, bs);
   return i;
}

static void end_fli(void)
{
	fli_end(flifd, &fh, &aa_screen, bs);
	aa_free_mem_screen(bs);
	dos_close(flifd);
}


int jmovie(char *filename)
{
	double fdinc, powinc, hourinc, seasoninc, elevinc;
   double azimuthinc,elevationinc;

   char afilename[MAXPATH];

   double saved_vdown = vdown;
   double saved_vturn = vturn;

	float *a = NULL;
	double rmin = 1e50, rmax = -1e50, rmean, rrange;
   FILE *fd=NULL;
   rect R;
   float *za;

	double fdim = animation_start.dimension;
	double powscale = animation_start.power;
	double season = animation_start.season;
	double hour = animation_start.hour;
	double hgtfac = animation_start.height;
   double azimuth = animation_start.azimuth;
   double elevation = animation_start.elevation;

   double H0,HN,mantissa,exponent,Hmultiplier,H;


   double xpower = log(powscale+1);
   double xendpower = log(animation_end.power+1);
   double xpowinc;
   unsigned long ticker=0;
   unsigned percent;
   unsigned long nexttick;
   unsigned long totalticks;

   int errval = AA_SUCCESS;

   int n = meshsizes[mesh];

   union REGS regs;


	int i, j;
	int frame;
	unsigned int seed = rseed;	/* Current random seed */
	double lastfd = 1e50;	/* Just some big number */
	double lastpower = 1e50;
   double lasthgtfac = 1e50;
   int cleared = 0;

	free(databuffer);
	changed = true;


   i = create_fli(filename,4);

	if (i<0)
      return i;

   if (frames < 2)
		frames = 2;

	fdinc = (animation_end.dimension - animation_start.dimension) / (float) (frames - 1);
	powinc = (animation_end.power - animation_start.power) / (float) (frames - 1);
   xpowinc = (xendpower - xpower)/(float)(frames-1);
	hourinc = (animation_end.hour - animation_start.hour) / (float) (frames - 1);
	seasoninc = (animation_end.season - animation_start.season) / (float) (frames - 1);
	elevinc = (animation_end.height - animation_start.height) / (float) (frames - 1);
	elevationinc = (animation_end.elevation - animation_start.elevation) / (float) (frames - 1);
	azimuthinc = (animation_end.azimuth - animation_start.azimuth) / (float) (frames - 1);


   H0 = 4.0 - animation_start.dimension;
   HN = 4.0 - animation_end.dimension;

   mantissa = HN / H0;
   exponent = 1.0 / (frames - 1);
   Hmultiplier = pow(mantissa,exponent);

   H = H0;

	whichpic = animatething;
   first_mountain_projection = 1;
   if (whichpic == 1)
   {

      PreScale = 1;

      ticking = 1;

      TempFileName(afilename,"FORGE.DAT");
      fd = fopen(afilename,"wb");
      if (!fd)
         putenv("TMP=");
      TempFileName(afilename,"FORGE.DAT");
      fd = fopen(afilename,"wb");

      /* Make sure there is room for the thing */
      if (fd)
      {
         struct stat st;
         struct dfree dt;
         long needed;
         long avail;

         fstat(fileno(fd),&st);
         getdfree(st.st_dev+1,&dt);
         needed = frames * (long)n * (long)n * 2L * sizeof (float);
         avail = (long)dt.df_avail * (long)dt.df_bsec * (long)dt.df_sclus;
         if (needed > avail)
         {
            char tbuf[132];
            sprintf(tbuf,"Not enough disk space for pre-scaling: %ld bytes needed",needed);
            ErrorBox(tbuf);
            fclose(fd);
            remove(afilename);
            fd = NULL;
            PreScale = false;
            goto nodisk;
            
         }
      }

      if (!fd)
      {
         FileError(afilename,NULL);
         PreScale = false;
      }
      else
      {

         for(frame=0;frame<frames && !aborted;frame++)
         {
            rect psR;
            char tbuf[128];


            if (frame == 0)
            {
               int cx,cy;
               Centers(&sR,&cx,&cy);
               R.Xmin = cx + 4;
               R.Xmax = R.Xmin + StringWidthX * 30;
               R.Ymin = cx - FontHeight/2 - 4;
               R.Ymax = R.Ymin + FontHeight + 4;
               psR = R;
               ShadowAndSave(&psR);
               PenColor(LIGHTGRAY);
               PaintRect(&R);
               PenColor(BLACK);
               FrameRect(&R);
            }

            MoveTo(psR.Xmin+2,psR.Ymin + 2);
            TextAlign(alignLeft,alignTop);
            sprintf(tbuf,"Prescaling: Frame %3d of %3d",frame+1,frames);
            PenColor(WHITE);
            BackColor(LIGHTGRAY);
            DrawString(tbuf);

            rseed = seed;
            initgauss(rseed);
            if (a)
            {
               free(a);
               a = NULL;
            }
            PenColor(frame % 15 + 1);

            fdim = 4.0 - H;

			   spectralsynth(&a, n, 3.0-fdim);
            za = a + 1;

            powscale = exp(xpower) - 1.0;

       
      	   if (powscale != 1.0)
			   {
               ticker = 0;

               totalticks = n*n;
               InitTicker("Power scaling",totalticks);
               nexttick = tickertape[percent = 0];


				   for (i = 0; !aborted && i < n; i++)
				   {
                  int ixn = i*n*2;
					   for (j = 0; !aborted && j < n; j++)
					   {
						   double r = za[ixn+j*2];

						   if (r > 0)
                        za[ixn+j*2] = pow(r,powscale);
                     if (++ticker > nexttick)
                     {
                        tick(++percent);
                        nexttick = tickertape[percent];
                        AbortCheck();
                     }
					   }
				   }
			   }
            ticker = 0;
         	totalticks = n*n*2;
         	InitTicker("Scaling to screen", totalticks);
	         nexttick = tickertape[percent = 0];

			   for (i = 0; !aborted && i < n; i++)
            {
               int ixn = i*n*2;
				   for (j = 0; !aborted && j < n; j++)
				   {
					   double r = za[ixn+j*2];

					   rmin = min(rmin, r);
					   rmax = max(rmax, r);
                  if (++ticker > nexttick)
                  {
                     tick(++percent);
                     nexttick = tickertape[percent];
                     AbortCheck();
                  }
				   }
            }

			   rmean = (rmin + rmax) / 2;
			   rrange = (rmax - rmin) / 2;

			   for (i = 0; i < n; i++)
			   {
               int ixn = i*n*2;
				   for (j = 0; j < n; j++)
				   {
                  za[ixn+j*2] = (za[ixn+j*2]-rmean)/rrange;
                  if (++ticker > nexttick)
                  {
                     tick(++percent);
                     nexttick = tickertape[percent];
                  }
				   }
			   }
            /* Write the unscaled cloud data... */

            if (fd)
               fwrite(a,n*n+1,2*sizeof(float),fd);

            vturn = azimuth;
            vdown = 90 - elevation;
            ScaleMountain(a, n, hgtfac);

            /* Now write the scaled data.. */
            if (fd)
               fwrite(a,n*n+1,2*sizeof(float),fd);

            first_mountain_projection = 0;
   		   fdim += fdinc;
            xpower += xpowinc;
   		   hgtfac += elevinc;
            elevation += elevationinc;
            azimuth += azimuthinc;
            H *= Hmultiplier;
		   }
         fclose(fd);
         AbortCheck();
         PopRect(&i);
      }
nodisk:
      fdim = animation_start.dimension;
   	hgtfac = animation_start.height;
      elevation = animation_start.elevation;
      azimuth = animation_start.azimuth;
      xpower = log(animation_start.power+1);
      H = H0;
   }

   if (whichpic == 1 && PreScale && !aborted)
   {
      fd = fopen(afilename,"rb");
      if (!fd)
      {
         perror(afilename);
         fprintf(stderr,"Couldn't read prescale file.\n");
         PreScale = 0;
      }
   }
	first_mountain_projection = 1;



   R.Xmin = 0;
   R.Xmax = 319;
   R.Ymin = 0;
   R.Ymax = 199;
   planetRect = R;

	for (frame = 0; !aborted && frame < frames; frame++)
	{
      if (fd)
         fread(a,n*n+1,2*sizeof(float),fd);
      else
      {
         rseed = seed;
		   initgauss(rseed);


         /* Don't do a new synthesis if fdim unchanged */
		   if (fdim != lastfd || powscale != lastpower || hgtfac != lasthgtfac)
		   {
			   if (a)
            {
				   free(a);
               a = NULL;
            }

            fdim = 4.0 - H;
			   spectralsynth(&a, n, 3.0 - fdim);
            za = a+1;

            powscale = exp(xpower) - 1.0;

			   /* Check for error in A here */
			   if (!aborted && powscale != 1.0)
			   {
				   for (i = 0; i < n; i++)
				   {
                  int ixn = i*n*2;
					   for (j = 0; j < n; j++)
					   {
						   double r = za[ixn+j*2];

						   if (r > 0)
							   za[ixn+j*2] = pow(r,powscale);
					   }
                  AbortCheck();
				   }
			   }
			   for (i = 0; !aborted && i < n; i++)
            {
               int ixn = i*n*2;
				   for (j = 0;j < n; j++)
				   {
					   double r = za[ixn+j*2];

					   rmin = min(rmin, r);
					   rmax = max(rmax, r);
				   }
               AbortCheck();
            }

			   rmean = (rmin + rmax) / 2;
			   rrange = (rmax - rmin) / 2;

			   for (i = 0; !aborted && i < n; i++)
			   {
               int ixn = i*n*2;
				   for (j = 0; j < n; j++)
				   {
                  za[ixn+j*2] = (za[ixn+j*2]-rmean)/rrange;
				   }
               AbortCheck();
			   }
		   }
		   lastpower = powscale;
		   lastfd = fdim;
         lasthgtfac = hgtfac;
      }
      if (aborted)
         break;

      if (frame == 0)
      {
         CloseTicker();
         if (!cleared)
            PushPlanetRects();
         cleared = 1;
         regs.h.ah = 0;
         regs.h.al = 0x13;
         int86(0x10,&regs,&regs);
         switch(animatething)
         {
         case 0:
            initcloudcolors();
            break;
         case 1:
            initcloudcolors();
            break;
         case 2:
            initplanetcolors();
            break;
         case 3:
            initpurecolors();
            break;
         }
      }

      
      ticking = 2;

		switch (whichpic)
		{
		case 2:
			if (DoRotation)
         {
   		 	dPlanetStart = ((float) (frame)) / (float) frames;
            if (DoRotation == 2)
               dPlanetStart = 1.0 - dPlanetStart;
         }

//			if (DitherRotate)
//				DitherRotation++;
         /* regenerate the seed here always, we need it only for stars I think */
         rseed = seed;
		   initgauss(rseed);
         genplanet(a, n, 1.0, -1.0, hour, season,hgtfac);
         
			break;
		case 0:
			elevfac = hgtfac;
         genclouds(a, n);
			break;
		case 1:
         if (DoClouds)
         {
            elevfac = hgtfac;
            ticking = 0;
   			genclouds(a, n);
         }
         else
            ticking = 2;
         if (fd)
            fread(a,n*n+1,2*sizeof(float),fd);
         vturn = azimuth;
         vdown = 90 - elevation;
			genproj(a, n, hgtfac);
         CloseTicker();
			first_mountain_projection = 0;
         break;
      case 3:
         initpurecolors();
         genpures(a,n);
         break;

		}
      if (whichpic == 2 && DoDither)
      {
         dontscaleme = true;
         memcpy(sys_cmap,planet_clut,sizeof sys_cmap);
      }
      else
      {
         dontscaleme = false;
      	memcpy(sys_cmap, clut, sizeof sys_cmap);
      }
      if (parameter_stamping)
         StampParameters(fdim,hgtfac,powscale,mesh,terms,
            azimuth,90-elevation,season,hour);
      if (frame_stamping)
      {
         char tbuf[128];
         sprintf(tbuf,"Frame %d of %d",frame+1,frames);
         DisplayString13(tbuf,0,0);
      }

		i = save_next_fli();
      if (i)
      {
         errval = i;
         break;
      }
      /* Stamp the frame on the screen */
      if (!frame_stamping)
      {
         char tbuf[128];
         sprintf(tbuf,"Frame %d of %d",frame+1,frames);
         DisplayString13(tbuf,0,0);
      }
		fdim += fdinc;

      xpower += xpowinc;
	   powscale += powinc;
      season += seasoninc;
		hour += hourinc;
		hgtfac += elevinc;
      elevation += elevationinc;
      azimuth += azimuthinc;
      H *= Hmultiplier;
      AbortCheck();

	}
	if (a)
		free(a);


	end_fli();
   if (fd)
   {
      fclose(fd);
      remove(afilename);
   }

   if (errval != AA_SUCCESS || aborted)
      remove(filename);


   vdown = saved_vdown;
   vturn = saved_vturn;

   return errval;

}

void do_animation(void)
{

   union REGS regs;
   int doit;
   char tbuf[128];
   rect savedRect = planetRect;
   int cleared = 0;
   int errval;

   doit = select_file("Make FLI","*.FLI",tbuf,"FLI");

   doit = doit && Overwrite(tbuf);

   LimitMouse(0, 0, sR.Xmax, sR.Ymax);
   
   if (!doit)
      return;
   HideCursor();
   RenderingMode = 1;

   /* Save all of the stored images please */
   if (!cleared)
   {
      PushPlanetRects();
      cleared = 1;
   }

   errval = jmovie(tbuf);

   planetRect = savedRect;
   if (cleared)
   {
      regs.h.ah = 0;
      regs.h.al = (hasVGA) ? 0x12 : 0x10;
      int86(0x10,&regs,&regs);
      PaintMainWindow();

      PopPlanetRects();
      cleared = 0;
   }

   ShowCursor();      

   ticking = 1;

   if (errval)
   {
      sprintf(tbuf,"Animation error: %s",fli_error_message(errval));
      ErrorBox(tbuf);
   }

}



static rect *bR[40];
static int items=0;

static void push(int item,int inout)
{
   if (item == -1)
      return;

   if (item <= 3)
   {
      if (item == animatething)
         DoublePress(bR[item],inout,RED);
      else
         PushButton(bR[item],inout);
   }
   else if (item <= items - 4)
      InvertInsides((text_button *)bR[item]);

   else
   {
      PushButton(bR[item],inout);
      if (item == items - 1)
         ExtraHilite(bR[items-1],inout);
   }
}



void animate(void)
{
   rect R,tR;

   int doit = 0;
   int i,j;
   int width = 3*sR.Xmax/4;
   
   int height = 11 * (FontHeight + 4) + FontHeight + 8 + 44 + 6;

   int key,err;
   int current_item=21;
   char tbuf[128];

   int cx,cy,cx2,cy2;
   items = 0;

   if (!memok(64L*1024L*2L))
   {
      ErrorBox("Not enough memory to animate.");
      return;
   }

   params[0][0] = animation_start.dimension;
   params[0][1] = animation_start.height;
   params[0][2] = log(animation_start.power+1);
   params[0][3] = animation_start.elevation;
   params[0][4] = animation_start.azimuth;
   params[0][5] = animation_start.hour/(2*M_PI)*24;
   params[0][6] = animation_start.season/(2*M_PI)*360;

   params[1][0] = animation_end.dimension;
   params[1][1] = animation_end.height;
   params[1][2] = log(animation_end.power+1);
   params[1][3] = animation_end.elevation;
   params[1][4] = animation_end.azimuth;
   params[1][5] = animation_end.hour/(2*M_PI)*24;
   params[1][6] = animation_end.season/(2*M_PI)*360;


   PushCursorPosition();

   Centers(&sR,&cx,&cy);

   tR.Xmin = cx - width/2;
   tR.Xmax = tR.Xmin + width - 1;
   tR.Ymin = cy - height/2;
   tR.Ymax = tR.Ymin + height - 1;
   Centers(&tR,&cx2,&cy2);
   HideCursor();
   if (!ShadowAndSave(&tR))
   {
      ShowCursor();
      return;
   }

   PenColor(LIGHTGRAY);
   PaintRect(&tR);
   PenColor(BLACK);
   FrameRect(&tR);
   LimitMouse(tR.Xmin,tR.Ymin,tR.Xmax,tR.Ymax);

   TextAlign(alignCenter,alignTop);
   MoveTo(cx,tR.Ymin + 2);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Animation");

   R.Xmin = tR.Xmin + 2;
   R.Xmax = tR.Xmax - 2;
   R.Ymin = tR.Ymin + FontHeight + 8;
   R.Ymax = R.Ymin + FontHeight + 4;
   animatethingFrame = R;

   CreateRadioPanel(&animatethingFrame,animatethings,
      animatethingRects,4,animatething);

   for(i=0;i<4;i++)
      bR[items++] = &animatethingRects[i];

   MoveTo(tR.Xmin,animatethingFrame.Ymax + 3);
   PenColor(BUTTONFRAME);
   LineTo(tR.Xmax,animatethingFrame.Ymax + 3);

   TextAlign(alignCenter,alignTop);
   MoveTo(tR.Xmin + (tR.Xmax-tR.Xmin)/4,animatethingFrame.Ymax+4);
   PenColor(BLACK);
   BackColor(LIGHTGRAY);
   DrawString("Start");

   MoveTo(tR.Xmin + 3*(tR.Xmax-tR.Xmin)/4,animatethingFrame.Ymax+4);
   DrawString("End");

   MoveTo(tR.Xmin,animatethingFrame.Ymax + FontHeight + 4);
   PenColor(BUTTONFRAME);
   LineTo(tR.Xmax,animatethingFrame.Ymax + FontHeight + 4);

   for(i=0;i<2;i++)
   {
      for(j=0;j<7;j++)
      {
         if (i==0)
         {
            R.Xmin = tR.Xmin + 4;
            R.Xmax = cx - 4;
         }
         else
         {
            R.Xmin = cx+4;
            R.Xmax = tR.Xmax - 4;
         }

         R.Ymin = animatethingFrame.Ymax + 6 + FontHeight + 4 + j * (FontHeight + 8);
         R.Ymax = R.Ymin + FontHeight + 4;

         paramFrames[i][j] = R;

         PaintNumberBoxBase(&R,&paramTBS[i][j],
            params[i][j],parammsgs[j],paramtypes[j],
            BLACK,LIGHTGRAY,DARKGRAY,WHITE);
         paramRects[i][j] = paramTBS[i][j].nR;
         bR[items++] = (rect *)&paramTBS[i][j];
      }
   }

   MoveTo(tR.Xmin,paramFrames[0][6].Ymax + 4);
   PenColor(BUTTONFRAME);
   LineTo(tR.Xmax,paramFrames[0][6].Ymax + 4);

   MoveTo(cx,animatethingFrame.Ymax+3);
   LineTo(cx,paramFrames[0][6].Ymax + 4);

   R.Xmin = cx - 8 * StringWidthX;
   R.Xmax = cx + 8 * StringWidthX;
   R.Ymin = paramFrames[0][6].Ymax + 8;
   R.Ymax = R.Ymin + FontHeight + 4;

   framesFrame = R;
   PaintNumberBoxBase(&R,&framesTB,frames,"Frames: ",GS_UNSIGNED,
      BLACK,LIGHTGRAY,DARKGRAY,WHITE);
   framesRect = framesTB.nR;
   bR[items++] = (rect *)&framesTB;

   R.Xmin = tR.Xmin + 4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = framesFrame.Ymax + 8;
   R.Ymax = R.Ymin + FontHeight + 4;
   doitFrame = R;
   CreateRadioPanel(&doitFrame,doitmsgs,doitRects,3,-1);
   ExtraHilite(&doitRects[2],false);
   for(i=0;i<3;i++)
      bR[items++] = &doitRects[i];

   move_to_corner(&doitRects[2]);

   ShowCursor();
   while(1)
   {
      event e;
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int button = (e.State & 0x700) >> 8;
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
         }

         else if (button == swRight)
         {
            key = 0x1b;
         }
      }
      if (key == XF1 || (key == 0x0d && XYInRect(X,Y,&doitRects[0])))
      {
         PushCursorPosition();
         PaintRadioButton(&doitRects[0],true,true,doitmsgs[0]);
         WaitForNothing();
         helptext("forganim.hlp");
         PaintRadioButton(&doitRects[0],false,false,doitmsgs[0]);
         LimitMouse(tR.Xmin,tR.Ymin,tR.Xmax,tR.Ymax);
         PopCursorPosition();
         continue;
      }

      if (key == 0x1b || (key == 0x0d && XYInRect(X,Y,&doitRects[1])))
      {
         PaintRadioButton(&doitRects[1],true,true,doitmsgs[1]);
         WaitForNothing();
         PaintRadioButton(&doitRects[1],false,false,doitmsgs[1]);
         break;
      }

      if (key == 0x0d)
      {
         CheckRadioButtons(X,Y,animatethingRects,4,&animatething,animatethings);
         for(i=0;i<2;i++)
         {
            for(j=0;j<7;j++)
            {
               if (XYInRect(X,Y,&paramRects[i][j]))
               {
                  double z = params[i][j];
                  if (GetNumber(&paramTBS[i][j],&z,paramtypes[j],
                     param_limits[j].lo,param_limits[j].hi))
                        params[i][j] = z;
                  InvertInsides(&paramTBS[i][j]);
               }
            }
         }
         if (XYInRect(X,Y,&framesRect))
         {
            double z = frames;
            if (GetNumber(&framesTB,&z,GS_INTEGER,1,999))
               frames = z;
            InvertInsides(&framesTB);
         }
         if (XYInRect(X,Y,&doitRects[2]))
         {
            PaintRadioButton(&doitRects[2],true,true,doitmsgs[2]);
            ExtraHilite(&doitRects[2],true);
            WaitForNothing();
            PaintRadioButton(&doitRects[2],false,false,doitmsgs[2]);
            ExtraHilite(&doitRects[2],false);
            doit = true;
            break;
         }

  
      }
      
      navigate(key,lefts,rights,uppers,downers,items,bR,&current_item);
      switch(key)
      {
      case XF10:
			if (select_file("Save Screen", "*.gif", tbuf, "GIF") && Overwrite(tbuf))
				GifOutput(tbuf,true);
      	LimitMouse(0, 0, sR.Xmax, sR.Ymax);
         break;
      }

      if (last_item != current_item)
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

   animation_start.height = params[0][1];
   animation_start.dimension = params[0][0];
   animation_start.power = exp(params[0][2])-1;
   animation_start.elevation = params[0][3];
   animation_start.azimuth = params[0][4];
   animation_start.hour = params[0][5] * 2 * M_PI/24;
   animation_start.season = params[0][6] * 2 * M_PI / 360;

   animation_end.height = params[1][1];
   animation_end.dimension = params[1][0];
   animation_end.power = exp(params[1][2])-1;
   animation_end.elevation = params[1][3];
   animation_end.azimuth = params[1][4];
   animation_end.hour = params[1][5] * 2 * M_PI/24;
   animation_end.season = params[1][6] * 2 * M_PI / 360;

   if (doit)
      do_animation();

   RenderingMode = 0;

}

int NothingWaiting(void)
{
   return !SomethingWaiting();
}

void view_animation(void)
{
   char filename[128];
   union REGS regs;
   int found = 0;
   int errval;
   char tbuf[128];


   while(select_file("View animation","*.FLI",filename,"FLI"))
   {
      HideCursor();
      if (!found)
         PushPlanetRects();
      found = 1;
      
      regs.h.ah = 0;
      regs.h.al = 0x13;
      int86(0x10,&regs,&regs);

      errval = fli_until(filename,-1,NothingWaiting);

      regs.h.ah = 0;
      regs.h.al = (hasVGA) ? 0x12 : 0x10;
      int86(0x10,&regs,&regs);
      LoadPalette(0,15,brightpal);
      ShowCursor();
      if (errval < 0)
      {
         sprintf(tbuf,"Animation error: %s",fli_error_message(errval));
         ErrorBox(tbuf);
      }

   }

   if (found)
   {
      initgauss(rseed);
      HideCursor();
      PaintMainWindow();
      PopPlanetRects();
      ShowCursor();
   }
}




   
   
