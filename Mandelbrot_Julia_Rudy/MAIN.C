/* mand39.c */
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

#include "mand.h"

/*---------Global Variables-----------------*/
int mode, maxcolor, maxx, maxy, maxiteration = 50;
int minx;
double flox, fhix, floy, fhiy, fu = 0.3, fv = 0.2, fa, fb, fa3, fb3;
double deltax, deltay, fx, fy;
long lox, hix, loy, hiy;
long x, y, u, v, a, b, nega, negb, a3, b3, bloat;
long xinc, yinc, xinc2, yinc2, xinc4, yinc4, xinc8, yinc8;
int stackpointer;
int returnoffset, returnrow, returnbyte, toprow;
unsigned char *colorval;
int cursoron;			/* 0 no cursor on screen, 1 cursor  on screen */
int cursorshape = 0;		/* 0 hollow square for zoom, 1 triangle for
				 * val select */
int pixelx, pixely, curx, cury, curxinc, curyinc;
unsigned int cursoroffset;
view mandelview, cubicview, ruckerview;
view startview = {-2.8, 2.8, -2.25, 2.25};
int tile;
int basecolor = 0, palettenumber = 3, spinflag = 0;
unsigned char iteration;  /* why a uchar??? */
int autozoomflag = 0, exitflag = 0, doneflag = 0, openflag = 0;
int newjuliaflag = 0;
int fractaltype = 1;
int prog_init = 0;

/* 0 is quadratic julia, 1 is quadratic mandel, 2 is cubic julia,
3 is cubic mandel, 4 is rudy's set */
int mandelflag = 1, cubicflag = 0, insideflag = 0, oddrowflag;
int ruckerflag = 0, juliaflag = 0, grayflag = 0, mouseflag = 0;
int floatflag = 0, slicetype = 0, newsliceflag = 0, fourdeeflag = 0;

 /*
  * I have six kinds of slices depending on which parameters I display. The
  * standard slice is where I set a and b and show u and v.  In the past,
  * called this the Mab set, meaning that a and b are what I set, but from
  * now on I will call it the Muv set meaning that uv is the plane of values
  * which I test by letting xy range over them.  In the same vein, the
  * slicetypes 0 thru 5 can be assigned to the sets Muv, Mab, Mau, Mbv, Mbu,
  * Mav, respectively.	In each case, the indexed params are set equal to x
  * and y respectively as x and y range, and the non-indexed parameters are
  * set equal to the current global constant value of a,b,u or v.
  */
int testcenterflag, testcentercount;
int centercursorflag;
int soundflag;
int oldcolorband = 0;		/* for use in sound */
double bandsize = 1;
int chip = 1;			/* 0 is 386, 1 is 286 */
int curspeed = 8;
int DrawZoomBox = 0;
int SpiralMode = 0;
int SpiralPass = 0;
int mini_menu = 0;
int spiraldirection, spiralrun, spiralbaserun;
int BoxActive = 0;
int cycle_delay = 128;
int cycler = 0;

char *TraversalFileName = NULL;

int (*fractal) (void);
int SaveMe = 0;
int OneScreenDone = 0;
int Frozen = 0;
double zoomfactor = ZOOMFACTOR;
time_t starttime;
stampdata stampviews[STAMPCOUNT];
unsigned char *savearray[STAMPCOUNT] = {NULL, NULL, NULL, NULL};
int saveptr = 0;
int last_saved = -1;
char *SpecifiedGifName = NULL;
int AutoGIF = 0;
extern button *buttonlist[];
bitmap *theBitmap;
fontRec *theFont;
metaPort *thePort;
double aspect;
int forcefloat = 0;

rect sR;
int StringWidthX;
int FontHeight;
int current_main_item=-1;

int ParameterDisplayMode = 0;
   /* 2 means display the x y w a b u v i params
      1 means display the cursor position as appropriate:
	 x y w for the zoombox in both directions
	 u v for julia&&mandel
	    etc for slice types
      0 means don't display anything */

double startfu,startfv,startfa,startfb;
long starta,startb,startu,startv;


/*
 * fractal is a pointer to a function which depends on the fractal type and
 * on the chip type and on the size of the view
 */
int bitshift=28;
long fudge = 1L << 28;
long lm = 1L << 31;

/*--------------Initialized Global Variables	*/

int CommPort = 0;
int QuickMode = 0;
int QuitWhenDone = 0;


int fractintmode = 0;
int lastrowpainted;
int symcheck = 0;
int newflag = 0;
int singlepassmode = 0;
int hasVGA;
/*---------------------Main Function------------------*/
int main(int argc, char **argv)
{
	int numcount = 0;
	int i;
	view myview = startview;
	int gif_ok = 1;

	extern int disk_error_handler(int errval, int ax, int bp, int si);

   /* Make sure there is room for the stamps. */

   if (!memok(4320L * STAMPCOUNT + ITERATIONCAP))
   {
		fprintf(stderr, "\n\nSorry, not enough memory to run Mandel.\n");
      exit(-1);
   }
       /* Lets see if there is enough for a later gif under worst conditions. */
	if (!memok(20712L +		 /* Added up mallocs in comprs.c */
		  4320L * STAMPCOUNT + ITERATIONCAP))
		gif_ok = 0;


   harderr(disk_error_handler);

	mode = 0;

	colorval = malloc(ITERATIONCAP);

	/* Eat the args */
	chip = -1;
	while (argc > 1)
	{
		switch (argv[1][0])
		{
		case '-':
			/* proper flags */
			switch (argv[1][1])
			{
			case 'e':
				mode = 0x10;
				break;
         case 'f':
	    forcefloat = 1;
	    break;
         case '2':
	    chip = 286;
	    break;
			}
      }
		argc--;
		argv++;
	}


	if (!mode)
		mode = detectmode();

   if (mode != 0x10 && mode != 0x12)
   {
      printf("Error: This program needs at least EGA graphics. Aborting.\n");
      return -1;
   }
   else
   {
      if (mode != -1)
   	   i = InitGrafix(mode == 0x12 ? -EGA640x480 : -EGA640x350);
	   if (mode == -1 || i != 0)
	   {
		   printf("Error: Metagraphics not installed. Aborting.\n");
	 return -1;
	   }
   }
   hasVGA = mode == 0x12;

	CommPort = QueryComm();

	if (CommPort & MsDriver)
		CommPort = MsDriver;
	else if (CommPort & 2)
		CommPort = MsCOM2;
	else if (CommPort & 1)
		CommPort = MsCOM1;
	if (CommPort)
		InitMouse(CommPort);

   EventQueue(true);
   TrackCursor(true);
	installmode();




	InitGlobals();
   draw_buttons();

   LimitMouse(0,0,sR.Xmax,sR.Ymax);



	if (chip == -1)
		cpu_type();	/* sets chip */
	if (chip == 0)
		bloat = 0x10000000L;	/* 28 binary zeroes */
	else
		bloat = 0x1000; /* 12 binary zeroes */
	if (chip)
		fractal = mandel16;
   else
		fractal = mandel32;

	ruckerview = mandelview = cubicview = (numcount) ? myview : startview;
	popview(&mandelview);

   LoadDefaultPalette("MANDEL.PAL");
	prog_init = 1;
	useview();

	fillcolorval();
	resetcursor();
   MakeOurCursor();
   MoveCursor(curx,cury);
   ShowCursor();
   updatecursor();

   if (!gif_ok)
		ErrorBox("There may not be enough memory to save or view a Gif.");

	while (!exitflag)
	{
      ProtectCurrentRow();
      if (doneflag)
	 nosound();


		if (!doneflag && !Frozen)
		{
			int col, colorband;

			if (SpiralMode)
				JSetPixel(pixelx + minx, pixely, 15);


			col = (*fractal) ();
			colorband = (int) colorval[col];

			if (soundflag && colorband != oldcolorband)
			{
				if (!col)
					sound((unsigned) NULLTONE);
				else
				{
					if (col && col < MINIMUMAUDIBLE)
						nosound();
					else
						sound(BASETONE + (unsigned) ((float) (col *
						TONERANGE) / maxiteration));
				}
				oldcolorband = colorband;
			}
	 if (singlepassmode)
	 {
      		JSetPixel(pixelx + minx, pixely, colorband);
	    pixelx++;
	    if (pixelx >= maxx)
	    {
	       pixelx=0;
	       if (++pixely >= maxy)
		  doneflag = 1;
	    }
	    adjustxy();
	 }


			else if (!SpiralMode)
			{


				drawblock(tile, colorband);

//	      /* is there symmetry here? */
//	      if (loy == -hiy && mandelflag && !juliaflag)
//	      {
//		 int spy = pixely;
//		 pixely = maxy - pixely;
//				drawblock_reversed(tile, colorband);
//		 lastrowpainted = pixely;
//		 pixely = spy;
//	      }



				stepxy();


			}
			else
			{
				JSetPixel(pixelx + minx, pixely, colorband);
				if (testcenterflag)
					testcentercount += col;
				spiralxy();
				if (iteration == SPIRALBAILCOUNT)
				{
					testcenterflag = 0;
					if (!testcentercount)
					{
						SpiralMode = 0;
						useview();
					}
				}
			}
			iteration++;
		}
		if (doneflag && QuitWhenDone)
			exitflag = 1;



skipit:
		if (Frozen || !(iteration & 7) || doneflag)
		{
			checkkeyboard();
			if (spinflag && !Frozen)
			{
	    if (cycler > cycle_delay)
	    {
   				if (spinflag == 1)
   					spinpalette();
   				else
   					revspinpalette();
	       cycler = 0;
	    }
	    cycler++;
			}

      }
	}
	grayflag = 0;
	grayscale();
   StopEvent();
   StopMouse();

	restoretextmode();
//   printf("Elapsed time: %g seconds",(clock() - starttime)/18.2);
   return exitflag;
}


/* ---- Utilities called by keyboard and menu checkers ----- */
void start_mandel(void)
{
	popview(&mandelview);
	if (!insideflag)
	{
		if (chip)
			fractal = mandel16;
		else
			fractal = mandel32;
	}
	else
	{
		if (!chip)
		{
			if (insideflag == 1)
				fractal = in2man32;
			else
				fractal = in1man32;
		}
		else
			fractal = in1man16;
	}
}

void start_julia(void)
{
	if (newjuliaflag)
		popview(&startview);
	newjuliaflag = 0;
	if (mandelflag)
	{
		if (chip)
			fractal = julia16;
		else
			fractal = julia32;
	}
	else
	{
		if (!chip)
			fractal = cubicjulia32;
		else
			fractal = cubicjulia16;
	}
}

void start_cubic(void)
{
	popview(&cubicview);
	if (!insideflag)
	{
		if (chip)
			fractal = cubicmandel16;
		else
			fractal = cubicmandel32;
	}
	else
	{
		if (!chip)
		{
			if (insideflag == 1)
				fractal = in2cubicmandel32;
			else
				fractal = in1cubicmandel32;
		}
		else
		{
			if (insideflag == 1)
				fractal = in1cubmand16;
			else
			{
				fractal = incubicmandelfloat;
				deltax = (fhix - flox) / maxx;
				deltay = (fhiy - floy) / maxy;
			}
		}
	}
}

void start_CCM(void)
{
	popview(&ruckerview);
	if (!insideflag)
	{
		if (!chip)
			fractal = cubicmandel32;
		else
			fractal = cubicmandel16;
	}
	else
	{
		if (!chip)
		{
			if (insideflag == 1)
				fractal = in2cubicmandel32;
			else
				fractal = in1cubicmandel32;
		}
		else
		{
			if (insideflag == 1)
				fractal = in1cubmand16;
			else
			{
				fractal = incubicmandelfloat;
				deltax = (fhix - flox) / maxx;
				deltay = (fhiy - floy) / maxy;
			}
		}
	}
}

/*---------Graphics Functions---------*/

void InitGlobals(void)
{
	GetPort(&thePort);
   theBitmap = thePort->portBMap;
	sR.Xmin = thePort->portRect.Xmin;
	sR.Xmax = thePort->portRect.Xmax;
	sR.Ymin = thePort->portRect.Ymin;
	sR.Ymax = thePort->portRect.Ymax;
	StringWidthX = StringWidth("X");
	FontHeight = thePort->txFont->chHeight;
   aspect = theBitmap->pixResX / (float)theBitmap->pixResY;
}

int detectmode(void)
{
   int n = QueryGrafix();
   int n2 = n & 0x300;


   if (n == -1)
      return n;

   /* We want either 0x12 for a VGA or 0x10 for an EGA. */

   if (n2 == 0x200)
      return 0x10;
   if (n2 == 0x300)
      return 0x12;

   return n;
}

int stampstarts[8];
rect stamprects[8];

void installmode()
{
	int stampheight;
	int i;

	SetDisplay(GrafPg0);
   InitGlobals();

   minx = 88;
   maxx = 639 - minx;

	maxy = (mode == 0x12) ? 479 : 349;
	/*
	 * We actually want to keep one line available for text; that's 14
	 * rows.
	 */
	maxy -= FontHeight;

   /* let's make sure that the maximum height is divisible by
      4? */
   maxy = ((maxy + 1) & ~3) - 1;

	stampheight = (maxy + 1) / 8 + 4;

	maxcolor = 15;
	usepalette();

	for (i = 0; i < STAMPCOUNT; i++)
   {
      rect R;
      R.Ymin = stampstarts[i] = maxy - (i + 1) * stampheight;
      R.Ymax = R.Ymin + (maxy+1)/8 - 1;
      R.Xmin = 8;
      R.Xmax = R.Xmin + 69;
      stamprects[i] = R;
   }


}

void restoretextmode(void)
{
	SetDisplay(TextPg0);
}

/*
 * The point of the drawblock function is to a) keep the color density up,
 * but b) never fill the screen in totally before its all truly computed. I
 * do it by putting in 2x2 blocks for tilesize 8 and 4, and putting in single
 * pixels for tilesize 2 and 1.  To keep from getting odd triangle shapes, I
 * erase the fase parts of the existing 2x2 blocks when I fill in at tilesize
 * 2.  I do this in a row sensitive way to avoid getting a momentarily
 * jarring black line along the update advance. Don't change this, as I've
 * spent hours looking at alternatives!
 */
/* Gee. Hours. */


void drawblock_reversed(int tile, int colorband)
{
	if (tile == 8 || tile == 4)
	{
		JSetPixel(pixelx + minx, pixely, colorband);
		JSetPixel(pixelx + minx + 1, pixely, colorband);
		JSetPixel(pixelx + minx, pixely - 1, colorband);
		JSetPixel(pixelx + minx + 1, pixely - 1, colorband);
	}
   else
   {
	   JSetPixel(pixelx + minx, pixely, colorband);
	   if (tile == 2)
	   {
		   if (!oddrowflag)
		   {
			   JSetPixel(pixelx + minx - 1, pixely, 0);
			   JSetPixel(pixelx + minx - 2, pixely - 1, 0);
		   }
		   else
		   {
			   JSetPixel(pixelx + minx + 1, pixely + 1, 0);
		   }
	   }
   }
}


void drawblock(int tile, int colorband)
{
	if (tile == 8 || tile == 4)
	{
		JSetPixel(pixelx + minx, pixely, colorband);
		JSetPixel(pixelx + minx + 1, pixely, colorband);
		JSetPixel(pixelx + minx, pixely + 1, colorband);
		JSetPixel(pixelx + minx + 1, pixely + 1, colorband);
	}
   else
   {
	   JSetPixel(pixelx + minx, pixely, colorband);
	   if (tile == 2)
	   {
		   if (!oddrowflag)
		   {
			   JSetPixel(pixelx + minx - 1, pixely, 0);
			   JSetPixel(pixelx + minx - 2, pixely + 1, 0);
		   }
		   else
		   {
			   JSetPixel(pixelx + minx + 1, pixely - 1, 0);
		   }
	   }
   }
}

/*------------------------Cursor Functions------------------*/
unsigned char JGetPixel(int x, int y)
{
   return egavgagetpixel(x,y);
}

void JSetPixel(int x, int y, char c)
{
	union REGS regs;

	if (c & 0x80)
	{
		regs.h.ah = 0x0c;
		regs.h.al = c;
		regs.h.bh = 0;
		regs.x.cx = x;
		regs.x.dx = y;
		int86(0x10, &regs, &regs);
	}
	else
		egavgapixel(x, y, c);
}

void resetcursor()
{
	cursoron = 0;
   curx = minx + maxx/2;
   cury = maxy/2;
   MoveCursor(curx,cury);
}


void menutext(char *buf)
{
	char tbuf[128];


	blast_bottom((maxy+1) * 80,(sR.Ymax - maxy)*80);
	strcpy(tbuf, buf);
	tbuf[79] = 0;
   TextAlign(alignLeft,alignBottom);
   PenColor(LIGHTGRAY);
   BackColor(BLACK);
   RasterOp(zREPz);
   MoveTo(0,sR.Ymax);
   DrawString(tbuf);
//	printf(tbuf);

}


void draw_params(void)
{
	char tbuf[128];

 	double centerx = flox + (fhix - flox) / 2.0;
  	double centery = floy + (fhiy - floy) / 2.0;
  	double width = fhix - flox;
   double ta,tb,tu,tv;

   if (floatflag)
   {
      ta = startfa;
      tb = startfb;
      tu = startfu;
      tv = startfv;
   }
   else
   {
      double mybloat;
      if (cubicflag)
	 mybloat = bloat / 8.0;
      else
	 mybloat = bloat;

      ta = starta / mybloat;
      tb = startb / mybloat;
      tu = startu / mybloat;
      tv = startv / mybloat;
   }



  	sprintf(tbuf, "%s (%0.8g %0.8g) %0.8g a=%0.6g b=%0.6g u=%0.6g v=%0.6g",
	flagstonames(),
  		centerx, centery, width, ta, tb, tu, tv);


	menutext(tbuf);
}

void showstats(void)
{
   char tbuf[128];

   if (ParameterDisplayMode == 1)
   {
      if (cursorshape == 0)
      {
	 double fcenterx = flox + (curx-minx)/(double)maxx * (fhix - flox);
	 double fcentery = fhiy - cury/(double)maxy * (fhiy - floy);
	 double fwidth1 = (fhix-flox) * zoomfactor;
	 double fwidth2 = (fhix-flox) / zoomfactor;

         sprintf(tbuf,"(%g,%g) [%g %g]",fcenterx,fcentery,fwidth1,fwidth2);
	 menutext(tbuf);
      }
      else
      {
	 double tu,tv,ta,tb;
	 _readcursor(&tu,&tv,&ta,&tb,1);
         sprintf(tbuf,"A=%g B=%g U=%g V=%g",ta,tb,tu,tv);
	 menutext(tbuf);
      }
   }
}

void _updatecursor(int force)
{
   if (curx > minx)
      BoxCursor();
   else
      ArrowCursor();
   if (curxinc || curyinc || force)
   {
      MoveCursor(curx+=curxinc,cury+=curyinc);

      if (curx > minx || force)
	 showstats();
   }

}

void updatecursor(void)
{
   _updatecursor(false);
}

/*-----------------------Mouse and Keyboard Functions------*/

void do_stamp(i)
{
	int ll = last_saved - i;

	if (ll < 0)
		ll += STAMPCOUNT;
	if (savearray[ll])
	{
		stampdata tdata;
		pushstamp(&tdata);
		popstamp(&stampviews[ll]);
		stampviews[saveptr] = tdata;
		SetUpProperView();
		useview();
		Frozen = 0;
	}
}

int trystamps(void)
{
   /*
		* Check to see if we are in one of the
		* active stamps. If so, we need to use that
		* view. Return non-zero if we found a stamp to use.
		*/
	if (curx < minx && last_saved != -1)
	{
		int stampheight = (maxy + 1) / 8 + 4;
		int i;

		for (i = 0; i < STAMPCOUNT; i++)
		{
			if (cury > stampstarts[i] &&
					cury < stampstarts[i] + stampheight)
	 {
	    do_stamp(i);
	    return 1;
	 }
      }
   }
   return 0;
}


int checkmouse(void)
{
   int button;
   event e;
   int n;
   int X,Y;

   int keyword = 0;

   n = KeyEvent(false,&e);

   button = (e.State & 0x700) >> 8;

   X = e.CursorX;
   Y = e.CursorY;


   curxinc = e.CursorX - curx;
   curyinc = e.CursorY - cury;
   if (n)
   {

      if (e.ASCII && e.ASCII != 0xe0)
	 keyword = e.ASCII;
      else
	 keyword = e.ScanCode << 8;

   }

   if (n && !keyword && !button)
      return 0; 	/* ignore upclicks */

   curx = e.CursorX;
   cury = e.CursorY;


   if (curxinc || curyinc)
      updatecursor();


   if (n)
   {

      switch(button)
      {
      case swLeft:
		   if (curx < minx)
		   {
			   int i;

			   /* Check if its on any of the buttons */
			   for (i = 0; !keyword && buttonlist[i] != NULL; i++)
			   {
				   rect *R = &buttonlist[i]->R;

				   if (XYInRect(curx, cury, R))
				   {
					   keyword = buttonlist[i]->key;
					   break;
				   }
			   }

			   if (keyword)
				   break;

	    trystamps();
	 }
	 else
	    keyword = XINSERT;
		   break;
      case swRight:
		   /*
			   * If it was a right click on a stamp, just put up
			   * the data for that stamp; if it was a right click
			   * in the menu area, but not on the stamp, put up the
			   * data for the whole screen. I don't anticipate this
			   * being a permanent sort of thing, but it is for
			   * stamp debuggering.
			   */
		   if (curx < minx)
		   {
			   if (last_saved != -1)
			   {
				   int stampheight = (maxy + 1) / 8 + 4;
				   int i;

				   for (i = 0; i < STAMPCOUNT; i++)
				   {
					   if (cury > stampstarts[i] &&
						      cury < stampstarts[i] + stampheight)
					   {
						   int ll = last_saved - i;

						   if (ll < 0)
							   ll += STAMPCOUNT;
						   if (savearray[ll])
						   {
							   /*
								   * Yow.
								   * Display
								   * the data
								   * for this
								   * fucker.
								   */
							   char tbuf[128];

							   sprintf(tbuf, "Stamp info for stamp %d", ll+1);
							   stampinfo(tbuf, &stampviews[ll],i);
						   }
						   break;
					   }
				   }
			   }
		   }
		   else
			   keyword = XDELETE;
		   break;
	   }
   }
   else
      CheckButtons(X,Y);


	return keyword;
}


int checkkeyboard(void)
{
	int keyword;
	int uvflag = 0;

   int last_item = current_main_item;


	keyword = checkmouse();
	if (keyword)
	{
		nosound();
//	ProtectOff();
//	ShowCursor();
      uvflag = ParseKey(keyword);
      _ProtectCurrentRow();
		if (curxinc || curyinc)
			updatecursor();
	}
	if (uvflag)
		useview();

   if (last_item != current_main_item)
   {
      if (last_item != -1 && last_item < 6)
	 hilite_main_button(last_item,false);
      if (current_main_item != -1 && current_main_item < 6)
	 hilite_main_button(current_main_item,true);
   }


   return keyword;
}


