/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal codes of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.							     *													   *
******************************************************************************/

/*	barn24.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "game.h"

/*  The  program  works  by tracing the orbits of a flock of atoms.  I
   store an atom's color for use in determining its next color.  I use
   several  different  kinds  of  flocks of atoms, the largest two being a
   square of 50 x 50 atoms and the ~3000 atom picture of smiling A Square.
.  Note that a flockstruct uses 2 + ( 5 * MAXFLOCK) bytes of memory.  */

/*--------------Global Variables, General-----------*/

double flox, fhix, floy, fhiy;
double fdeltax, fdeltay;
double fstepx, fstepy;		/* the useful quantity maxx / (fhix - flox) */
double fcenterx, fcentery;

/* View window is given as double. We maintain it during pans and zooms.*/

/* From now on, we use only 16-bit integers, and arrays thereof. */

int mode, maxx, maxy;
int minx, miny;
int hasVGA;
int dirty_bit = false;
unsigned char maxcolor;
int lox, hix, loy, hiy, deltax, deltay, centerx, centery;
int startlox, starthix, startloy, starthiy, startdeltax, startdeltay;
double fstartlox, fstarthix, fstartloy, fstarthiy;
double fstartdeltax, fstartdeltay, fstartcenterx, fstartcentery;
int startcenterx, startcentery;
int cursoron;			/* 0 no cursor on screen, 1 cursor  on screen */
int curxinc, curyinc, curx, cury;
unsigned int cursoroffset;
unsigned char cursorpatch[64];
int mouseflag = 1;		/* assume mouse present, set flag to 0 if
				 * none */
int barnmapflag = 0;		/* 1 means draw the barnmap pictures */
int colorcycleflag = 0; 	/* 1 means change palette with each refresh */
int curspeed = 8;
int openflag = 0;		/* 1 allows pans, 0 disables them */
int grayflag = 0;
int coloring_style = 5;
int joshcolor[16] = {2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 6, 10, 13, 5, 6, 9};
int cursorcolor = 0x100;
int spinflag = 0;
int monoflag = 0;
int redmap = -1;
int edmap = 0;
int stage = 0;
int triangle_editing_mode = 0;
int triangle_display_mode = 0;
char current_barnmap_name[128];
int locked = true;


/*--------------Int Global Variables, Specific-----------*/
ffunctionsystem *fBptr, *fUptr;
functionsystem *Bptr;

/* fBptr points to the active floating point maps.  This will be either
cannedmaps or the userdefined maps which are pointed to by fUptr.  So
fBptr will always be equal to either &cannedbarnmaps or fUptr.	Bptr points
to the active integer defined maps and is always derived from fBptr. */
flockstruct *flockptr;
fflockstruct *fflockptr;
int flocktype = 0;		/* 0 is 16 atoms, 1 is frame rect, 2 is solid
				 * square */
unsigned int iteration = 0;

 /*
  * counter used to time keyboard check and to pause cursor input after a pan
  */
int exitflag = 0;
int notrace = 0;		/* 0 is show all steps, 1 is erase them */

int current_barnmap = 0;
extern barnmap_set *barnmap_table[];
int checking_limits = 0;
rect current_limit;

int realmode = 0;		/* integer mode if zero... */

rect display_rect;

/*------Function Declarations, General----*/

void detectmode();
void installmode();
void _installmode(void);
void restoretextmode();
void instruct();
void helpscreen();
void useview();
void dozoom(int n);		/* 0 centers, 1 zooms in, -1 zooms out */
void dopan(int panx, int pany); /* move panx halfscreens right, and or pany
				 * halfscreens up.  Negative is opposite way. */
void resetcursor();
void updatecursor();
void message(char s[]);
void mousereset();
int mousebutton();
pair mousemotion();

/*  mousereset() invokes mouse function 0 to reset the mouse for current
   graphics mode.  The first time resetmouse fails, it delivers a warning.
   mousebutton() uses mouse function 3 to return the depressed button bits
   (bit #0 is L, #1 is R).
   mousemotion() uses mouse function 11 to return the pair (xmickeys,
   ymickeys) of the number of mickeys mouse has moved in the two
   directions.	A mickey is 1/200 of an inch, and the mickey count
   can be a full 16 bit positive or negative number..*/


/*------Function Declarations,Specific----*/
void bcopy(ffunctionsystem * K, ffunctionsystem * L);	/* copy the structure */
barnmap fint(fbarnmap h);	/* fint function bloats double barnmap into
				 * int barnmap */
void weighttocuts(ffunctionsystem * K, functionsystem * L);
double det(double, double, double, double);
void cutstoweight(functionsystem * L, ffunctionsystem * K);
void Bfint(ffunctionsystem * K, functionsystem * L);
void setstartwindow();		/* set bloated integer lo, hi, delta, center */
void fillfixedpoints(ffunctionsystem * K);
void setfwindow();
void setiwindow();

 /*
  * setfwindow sets flo and fhi values on the basis of the bloated lo and hi
  * values (altered by zoom and pan).  Also, setfwindow figures out the pixel
  * coordinates of the *Bptr fixed pts; note that although the float valued
  * fixed points of *fBptr are invariant, the pixel coords of these points
  * will indeed depend on my floating point window of view.
  */
void showfixedpoints(functionsystem * K, ffunctionsystem * L);
int choose();
void step();
void checkkeyboard();
void frameflock();
void iconflock();
void fourflock();
void squareflock();
void fillflock();
void weighthere(int sign);
void bumpup(int n);
void bumpdown(int n);
void activate(ffunctionsystem * K);	/* combines all the startup functions */
void flocktofflock(void);
void fflocktoflock(void);
void startnewmap(ffunctionsystem * L, int i);
void setup_barnmap(void);
void randomizeifs(void);

 /* start a new map at postion L->i and balances weight */
void balanceweight(ffunctionsystem * L, int i);

 /* divides up weight equally between i and nonzero maps */
//void edit(int button, int keyword);
fpair realspot(int x, int y);	/* gives fwindow coords of cursor */
int fixheart(int i);

/*-----External Function Declarations, General-------*/

extern int sixteenbitsa();

/*  Uses 16 bit rule 30 shifts to give a fast random integer*/

extern void setmode(int m);
extern int getmode(void);

/*  setmode and getmode use int 10h functions 0 and F*/

extern void egavgapixel(int i, int j, int color);

/*  This is a direct video write, with (i,j) pixel coordinates.
I've put the clipping minx,639 and 0 to maxy in it.*/


/*extern pair image(int a, int b, int c, int d, int e, int f, int x, int y);
 This computes
	w.x = intmul(h.a, z.x) + intmul(h.b, z.y) + h.e;
	w.y = intmul(h.c, z.x) + intmul(h.d, z.y) + h.f;
and returns w .  By intmul(m,n), I mean (m * n) >> SHIFT. */


/*  Uses intmul style multiplication to compute pixel pair, with
   pixel.x=(z.x-lox)*maxx/deltax and pixel.y=(hix-z.y)*maxy/deltay */




extern void mouse(int a, int b, int c, int d);

/*  mouse(a,b,c,d) loads a,b,c,d into ax,bx,cx,dx and invokes int 33h
   to call the mouse function number a, which may return info to the
   ax,bx,cx, and dx registers.*/

extern void changepalette();
extern void usepalette();
extern void spinpalette();
extern void revspinpalette();

/*  The assembly part of the program maintains	an
   egacolortable, and a vgacolortable.	changepalette alters what is
   in these data regions, and usepalette uses int 10h functions to
   invoke the new palettes.*/

extern void grayscale();

/*  In VGA mode, grayscale uses int 10h function 12h to make all DAC
   registers gray, by equally weighting RBG. */

/* Metawindows stuff */
int CommPort = 0;
metaPort *thePort;
bitmap *theBitmap;
rect sR;
int FontHeight;
int StringWidthX;
int MenuBackColor = BLUE;
int MenuTextColor = WHITE;
int ButtonBackColor = DARKGRAY;
int ButtonFrameColor = BLACK;
int ButtonTextColor = YELLOW;
int palettenumber = 0;
int tweaking = 0;
int prog_init = 0;
void install_fill_outline(void);
double aspect;

/*-----------------------Main Function-------------*/



int main(int argc, char **argv)
{
	int i;
	extern int disk_error_handler(int errval, int ax, int bp, int si);
	char *startpal = "GAME.PAL";
	int gif_ok = 1;

   /* main.c plus possible stamps.c allocs require about 130K */

   if (!memok((long) sizeof(barnmap_set) + sizeof(mapper) * MAXBARNMAPS
	     + sizeof(fflockstruct) + sizeof(ffunctionsystem)
	     + sizeof(functionsystem) + sizeof(flockstruct)
	     + sizeof(fflockstruct)))
   {
		fprintf(stderr, "\n\nSorry, not enough memory to run The Chaos Game.");
      exit(-1);
   }
       /* Lets see if there is enough for a later gif under worst conditions. */
	if (!memok(20712L +		 /* Added up mallocs in comprs.c */
	    sizeof(barnmap_set) + sizeof(mapper) * MAXBARNMAPS
	     + sizeof(fflockstruct) + sizeof(ffunctionsystem)
	     + sizeof(functionsystem) + sizeof(flockstruct)
	     + sizeof(fflockstruct)))
		gif_ok = 0;


	mode = 0;
	harderr(disk_error_handler);

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
		detectmode();

	if (mode == 0x12)
		hasVGA = true;


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
	prog_init = 1;
	TrackCursor(true);
	LoadDefaultPalette(startpal);
	GetPort(&thePort);
	sR = thePort->portRect;
	SetDisplay(GrafPg0);
	theBitmap = thePort->portBMap;
	aspect = theBitmap->pixResX / (float) theBitmap->pixResY;

	InitializeCursors();

	FontHeight = thePort->txFont->chHeight;
	StringWidthX = StringWidth("X");
	_installmode(); 	/* need to do this here to set maxx maxy */

	LimitMouse(0, 0, sR.Xmax, sR.Ymax);


	fBptr = calloc(sizeof(ffunctionsystem), 1);
	Bptr = calloc(sizeof(functionsystem), 1);	/* 16*MAXBARNMAPS +2 */
	flockptr = calloc(sizeof(flockstruct), 1);	/* 5*MAXFLOCKSIZE + 2 */
	fflockptr = calloc(sizeof(fflockstruct), 1);	/* 5*MAXFLOCKSIZE + 2 */

	setup_barnmap();
	activate(fBptr);
	resetcursor();		/* center the cursor and turn it on. */
	DrawButtons();

	if (!gif_ok)
		ErrorBox("There may not be enough memory to save or view a Gif.");

	while (exitflag == 0)
	{
		step();
		if ((iteration & 63) == 0)
		{		/* if I have a small sixteen atom flock, I
				 * don't want to be hitting the keyboard and
				 * mouse interrupts too often.  Also I don't
				 * want to autospin too often. */
			checkkeyboard();

			if (spinflag && ((iteration & 0xfff) == 0))
			{
				if (spinflag == 1)
					spinpalette();
				else
					revspinpalette();
			}
		}
		iteration++;
	}

	/* Hokay, bounce the button... */

   grayflag = 0;
   grayscale();
   usepalette();

   restoretextmode();
	StopEvent();
	StopMouse();
	return exitflag;
}

void install_fill_outline(void)
{
	installmode();
	fillflock();
	if (tweaking || (triangle_display_mode && triangle_editing_mode))
		OutlineCurrentMap();
}

void detectmode()
{
	int n = QueryGrafix();

	if (n == -1)
	{
		printf("Error: Metagraphics not installed. Aborting.\n");
		exit(-1);
	}
	if (!(n & 0x200))
	{
		printf("This program needs at least EGA graphics to run.");
		exit(-1);
	}
	if ((n & 0x300) == 0x300)
		mode = 0x12;
	else
		mode = 0x10;
}

void _installmode(void)
{
	/* just sets the default things */

	if (Stamping)
	{
		minx = stamp_rects[0].Xmin;
		maxx = stamp_rects[0].Xmax - stamp_rects[0].Xmin;
		miny = stamp_rects[0].Ymin;
		maxy = stamp_rects[0].Ymax - stamp_rects[0].Ymin;
	}
	else if (!tweaking)
	{
		minx = 80;
		maxy = sR.Ymax;
		maxx = sR.Xmax - minx;
		miny = 0;
	}
	else
	{
		minx = 80;
		maxx = 360;
		miny = 0;
	}
	//keep a constant aspect ratio of 560 across, 480 down for VGA,
   //560 across, 350 down for EGA

	if (mode == 0x12)
		maxy = (6 * maxx) / 7 - 1;
	else
		maxy = (35 * maxx) / 56 - 1;



	display_rect.Xmin = minx;
	display_rect.Xmax = minx + maxx;
	display_rect.Ymin = miny;;
	display_rect.Ymax = miny + maxy;
	maxcolor = 15;
}

void installmode()
{

	_installmode();

	HideCursor();

	PenColor(BLACK);
	RasterOp(zREPz);
	BackColor(BLACK);

	EraseRect(&display_rect);
	PenColor(WHITE);
	FrameRect(&display_rect);

	if (colorcycleflag)
		changepalette();/* to keep starting up in new palettes */
	usepalette();

	/* and show the name of the current barnmap set */
	/* but NOT if we are in the little corner! */
	if (!Stamping)
	{
		char tbuf[128];

		PenColor(LIGHTGRAY);
		TextAlign(alignRight, alignBottom);
		MoveTo(display_rect.Xmax - 2, display_rect.Ymax - 2);
		sprintf(tbuf, "%s%s", (dirty_bit || barnmapsp) ? "*" : "", current_barnmap_name);
		DrawString(tbuf);
	}
	ShowCursor();
}

void restoretextmode()
{
	SetDisplay(TextPg0);
}

pair antipixel(pair * p)
{
	pair op;

	op.x = lox + (p->x - minx) * (double) deltax / maxx;
	op.y = hiy - (p->y - miny) * (double) deltay / maxy;
	return op;
}

fpair antifpixel(pair * p)
{
	fpair fp;

	fp.x = flox + (p->x - minx) * fdeltax / maxx;
	fp.y = fhiy - (p->y - miny) * fdeltay / maxy;
	return fp;
}

pair fpixel(fpair * z)
{
	pair p;
	fpair fp;

	/* warning watch out for overflow here! */

	fp.x = minx + (z->x - flox) * maxx / fdeltax;
	fp.y = miny + (fhiy - z->y) * maxy / fdeltay;


	if (fp.x > 32767)
		fp.x = 32767;
	if (fp.x < -32767)
		fp.x = -32767;
	if (fp.y > 32767)
		fp.y = 32767;
	if (fp.y < -32767)
		fp.y = -32767;

	p.x = fp.x;
	p.y = fp.y;
	return p;
}


void fshow(fpair * z, unsigned char color)
{
	pair p = fpixel(z);

	egavgapixel(p.x, p.y, (int) color);
}

void show(pair * z, unsigned char color)
{

	pair p;


	p = pixel(z);
	egavgapixel(p.x, p.y, (int) color);
}


/*------------------Text Output Functions-----------------*/

void instruct()
{
}
void message(char s[])
{
	restoretextmode();
	printf(s);
	printf("\n\n");
	printf("Press any key to continue.");
	getch();
	setmode(mode);
}

void helpscreen()
{
}

/*------------------Mouse and Keyboard Functions-------------*/

void mousereset()
{
	static int firsttime = 1;

	mouse(0, 0, 0, 0);
	/* This returns  ax = -1 if yes mouse, ax = 0 if no mouse */
	if (firsttime && !_AX)
	{
		message("No mouse is currently active, use arrow keys instead.");
		mouseflag = 0;
		firsttime = 0;
	}
}

int mousebutton()
{
	mouse(3, 0, 0, 0);
	return _BX & 3;
}

pair mousemotion()
{
/*  In order to keep my cursor aligned with the EGA blocks, the minimal horizontal motion is 8 pixels.	I use a minimal vertical motion of 6.*/

	pair temp;
	static int blockx = 8, blocky = 6;

	mouse(11, 0, 0, 0);
	temp.x = _CX;
	temp.y = _DX;
	if (temp.x)
	{
		if (abs(temp.x) <= blockx)
			temp.x = sign(temp.x) * blockx;
		else
			temp.x = blockx * (temp.x / blockx);
	}
	if (temp.y)
	{
		if (abs(temp.y) <= blocky)
			temp.y = sign(temp.y) * blocky;
		else
			temp.y = blocky * (temp.y / blocky);
	}
	return temp;
}

int ShiftArrows(event * e)
{
	int retval = 0;
	int scancode = e->ScanCode << 8;

	if ((scancode == XLARROW ||
	     scancode == XDARROW ||
	     scancode == XUARROW ||
	     scancode == XRARROW ||
	     scancode == XPGUP ||
	     scancode == XPGDN ||
	     scancode == XEND ||
	     scancode == XHOME) && (e->State & 3))
		retval = scancode + 1;

	else if (scancode == XCENTER5)
		retval = XCENTER5;

	return retval;
}

void bump_edmap(void)
{
	edmap++;
	if (edmap >= fBptr->n)
		edmap = 0;
	ConvertParams(edmap);
}


int checkmouse(void)
{
	int button;

	event e;
	int n;
	int wbutton;
	int X, Y;


	int keyword = 0;

	n = KeyEvent(false, &e);
	X = e.CursorX;
	Y = e.CursorY;
	button = (e.State & 0x700) >> 8;

	curxinc = X - curx;
	curyinc = Y - cury;

	if (curxinc || curyinc)
		updatecursor();

	if (n)
	{
		/* check first for shift-arrow keys */
		keyword = ShiftArrows(&e);

		if (!keyword)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				keyword = e.ASCII;
			else if (e.ScanCode)
				keyword = e.ScanCode << 8;
			else
			{

				if ((triangle_editing_mode || (tweaking && tweakmode == 2)) && button == swRight)
				{
					bump_edmap();
					install_fill_outline();
					if (tweaking)
						UpdateAllParams(edmap);
				}
				else if (button && X >= minx && X < maxx + minx && Y <= maxy + miny)
				{
					if (button == swLeft)
						wbutton = 1;
					else if (button == swRight)
						wbutton = 2;
	       else
		  wbutton = 0;
	       if (wbutton)
	       {

					   if (!tweaking)
					   {
						   if (!triangle_editing_mode)
							   weighthere(wbutton);
					   }
					   else
					   {
						   if (tweakmode != 2)
							   weighthere(wbutton);
					   }
	       }
				}
				else
				{
					if (button == swLeft)
						keyword = CheckLeftClicks(&e);
					else if (button == swRight)
						keyword = CheckRightClicks(&e);
				}

			}
		}
	}
	else
	{
		/* if the left key is down, we might want it anyway */
		if (button == swLeft)
			keyword = CheckLeftClicks(&e);
		else
			CheckDrags(&e);
	}


	return keyword;
}

void setup_barnmap(void)
{
	barnmap_set *b;
	int i;

	/* Drop the new shit into fBptr... */
	b = barnmap_table[current_barnmap];

	strcpy(current_barnmap_name, b->name);

	fBptr->n = b->n;

	for (i = 0; i < b->n; i++)
	{
		fBptr->h[i].a = b->maps[i].h.a;
		fBptr->h[i].b = b->maps[i].h.b;
		fBptr->h[i].c = b->maps[i].h.c;
		fBptr->h[i].d = b->maps[i].h.d;
		fBptr->h[i].e = b->maps[i].h.e;
		fBptr->h[i].f = b->maps[i].h.f;
		fBptr->weight[i] = b->maps[i].weight;
	}
/* set window, set startvals as well since these are used for iconflock to
show maps on the big screen */

	startlox = lox = BLOAT * b->lox;
	startloy = loy = BLOAT * b->loy;
	starthix = hix = BLOAT * b->hix;
	starthiy = hiy = BLOAT * b->hiy;

	fstartlox = flox = b->lox;
	fstartloy = floy = b->loy;
	fstarthix = fhix = b->hix;
	fstarthiy = fhiy = b->hiy;
	fstartdeltax = fstarthix - fstartlox;
	fstartdeltay = fstarthiy - fstartloy;
	fstartcenterx = (fstartlox + fstarthix) / 2;
	fstartcentery = (fstartloy + fstarthiy) / 2;

	startdeltax = deltax = hix - lox;
	startdeltay = deltay = hiy - loy;
	startcenterx = lox + deltax / 2;
	startcentery = loy + deltay / 2;

	fdeltax = b->hix - b->lox;
	fdeltay = b->hiy - b->loy;

	redmap = -1;
	stage = 0;
	edmap = 0;
}
void add_new_barnmap(void)
{
	if (fBptr->n < MAXBARNMAPS)
	{
		fBptr->n++;
		startnewmap(fBptr, fBptr->n - 1);
		redmap = fBptr->n - 1;
		activate(fBptr);
	}
	else
	{
//		startnewmap(fBptr, 0);
//		redmap = 0;
//		activate(fBptr);
      ErrorBox("Sorry, a maximum of 20 maps is allowed.");
	}
}

void checkkeyboard()
{
	int keyword;
	char chosen_file[128];

	keyword = 0;
	if (1)
	{
		keyword = checkmouse();

		keyword = CheckMenuKeys(keyword);

		switch (keyword)
		{
		case XALTA:
			presetpalette();
			break;

		case XALTB:
			randompalette();
			break;

		case XALTC:
			dozoom(0);
			break;

		case XALTD:
			dodefaultpalette();
			break;

      case XALTE:
	 locked ^= 1;
	 break;


		case XALTF:
			gif_viewer();
			break;

      case XALTG:
			if (select_file("Save Image", "*.gif", chosen_file, "GIF"))
	    if (Overwrite(chosen_file))
	   			SaveImageGif(chosen_file);
			break;

		case XALTH:
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF"))
	    if (Overwrite(chosen_file))
   				SaveScreenGif(chosen_file);
			break;

      case XALTJ:
	 spinflag = 0;
			spinpalette();
			break;

      case XALTK:
			spinflag = 0;
			revspinpalette();
			break;

		case XALTL:
			load_parameters();
			break;

      case XALTM:
	 grayflag ^= 1;
	 grayscale();
	 usepalette();
	 break;

      /* altN--see under F3 */

		case XALTO:
	 dozoom(-3);
			break;

		case XALTP:
			palette_tweaker();
			LimitMouseRect(&sR);
			break;

		case XALTQ:
			PushCursorPosition();
			PaintQuitButton(&mainbuttonR[5], true);
			WaitForNothing();
			if (cancel_ok_msg("QUIT to DOS: Are you sure?"))
				exitflag = 2;
			PaintQuitButton(&mainbuttonR[5], false);
			if (current_main_item == 5)
				invert_main_item(5);
			PopCursorPosition();
			break;

      case XALTR:
			installmode();
			if (tweaking || (triangle_display_mode && triangle_editing_mode))
				OutlineCurrentMap();
			break;

      case XALTS:
			if (select_file("Save Parameters", "*.ifs", chosen_file, "IFS") && Overwrite(chosen_file))
				SaveParams(chosen_file);
			break;

      case XALTT:
			MakeStamp();
			break;

		case XALTX:

			PushCursorPosition();
			PaintQuitButton(&mainbuttonR[5], true);
			WaitForNothing();
			if (cancel_ok_msg("EXIT: Are you sure?"))
				exitflag = 1;
			PaintQuitButton(&mainbuttonR[5], false);
			if (current_main_item == 5)
				invert_main_item(5);

			PopCursorPosition();
			break;

      case XALTY:
	 spinflag++;
	 if (spinflag > 2)
	    spinflag = 0;
	 break;

      case XALTW:
	 InfoBox();
	 break;


		case ' ':
      case XALTZ:
			zoombox();
			break;


		case XALT1:
		case XALT2:
		case XALT3:
		case XALT4:
			restore_stamp((keyword - XALT1) >> 8);
			break;



		case XF1:
			PressButton(0, true);
			WaitForNothing();
			helptext(tweaking ? "gametwk.hlp" : "gamemain.hlp");
			PressButton(0, false);
			if (current_main_item == 0)
				invert_main_item(current_main_item);
			break;

		case XF2:
			PressButton(1, true);
			WaitForNothing();
			do_files_menu();
			PressButton(1, false);
			if (current_main_item == 1)
				invert_main_item(current_main_item);
			break;

		case 'N':
		case 'n':
		case XF3:
      case XALTN:
         realmode = false;
         dirty_bit = false;
			PressButton(2, true);
			WaitForNothing();
			if (keyword == 'N')
			{
				if (current_barnmap == 0)
				{
					int i;

					for (i = 0; barnmap_table[i + 1]; i++);
					current_barnmap = i;
				}
				else
					current_barnmap--;
			}
			else
			{
				current_barnmap++;
				if (barnmap_table[current_barnmap] == NULL)
					current_barnmap = 0;
			}

			setup_barnmap();
			ClearBarnmapStack();
			_activate(fBptr);
			ConvertParams(edmap);
			if (tweaking)
			{
				edmap = 0;
				FirstMap = 0;
				ShowAll();
				UpdateAllParams(edmap);
			}
			if (tweaking || (triangle_display_mode && triangle_editing_mode))
				OutlineCurrentMap();
			if (tweaking)
			{

				if (fBptr->n < 6)
				{
					if (current_main_item == LEFTARROWBOX)
						current_main_item++;
					while (current_main_item - LEFTARROWBOX > fBptr->n)
						current_main_item--;
				}
			}
			PressButton(2, false);
			if (current_main_item == 2)
				invert_main_item(current_main_item);


			break;

		case XF4:
      case 0x1b:

	 if (keyword == 0x1b && !tweaking)
	    break;

			PushCursorPosition();
			PressButton(3, true);
			WaitForNothing();
			tweaking ^= 1;

			recenter();
			if (tweaking)
			{
				HideCursor();
				if (tweakmode == 2)
					TriangleCursor();
				else
					ArrowCursor();
				ShowCursor();
				tweak(edmap);
			}
			else
			{
				if (triangle_editing_mode)
					TriangleCursor();
				else
					ArrowCursor();
			}
			PressButton(3, false);
			if (current_main_item == 3)
				invert_main_item(current_main_item);

			if (!tweaking && current_main_item > ALTXEXIT)
				current_main_item = -1;

			PopCursorPosition();
			break;

		case XF5:
			PressButton(4, true);
			WaitForNothing();
         PushCursorPosition();
			do_view_menu();
			PressButton(4, false);
			if (current_main_item == 4)
				invert_main_item(4);
         PopCursorPosition();
			break;




		case 'a':
      case 'A':
			tw_add_new_barnmap();
			break;

		case 'c':
      case 'C':
			if (triangle_editing_mode || tweaking)
			{
				bump_edmap();
				install_fill_outline();
				if (tweaking)
					UpdateAllParams(edmap);
			}
			break;

      case 'e':
      case 'E':
			WeightEditor();
			break;

      case 'f':
	 flocktype -= 1;
	 if (flocktype < 0)
	    flocktype = 2;
	 install_fill_outline();
	 break;

      case 'F':
			flocktype += 1;
			if (flocktype > 2)
				flocktype = 0;
			install_fill_outline();
			break;

		case 'I':       /* increase barnmap weight at cursor */
		case XCENTER5:
			if (!triangle_editing_mode && !(tweaking && tweakmode == 2))
				weighthere(1);
			else
				CheckCornerDrags(curx, cury);
			break;

		case 'i':       /* decrease barnmap weight at cursor */
			if (!triangle_editing_mode && !(tweaking && tweakmode == 2))
				weighthere(2);
			else
				CheckCornerDrags(curx, cury);
			break;

		case 'J':       /* change type of color map */
			coloring_style++;
			if (coloring_style == COLORINGSTYLECOUNT)
				coloring_style = 0;
			install_fill_outline();
			break;

		case 'j':       /* change type of color map */
			coloring_style--;
			if (coloring_style == -1)
				coloring_style = COLORINGSTYLECOUNT - 1;

			install_fill_outline();
			break;

		case 'k':       /* kill the nearest map's weight */
      case 'K':
			weighthere(4);
			break;

		case 'l':
      case 'L':
			colorcycleflag ^= 1;
			break;


      case 'M':
      case 'm':
			barnmapflag ^= 1;
			install_fill_outline();
			break;

	 /* n is under F3 */
		case 'R':
      case 'r':
			install_fill_outline();
			break;

		case 't':       /* toggle erase old points */
      case 'T':
			notrace ^= 1;
			break;

		case 'u':
      case 'U':


			if (PopBarnmap())
			{
				_activate(fBptr);
				ConvertParams(edmap);
				if (tweaking)
				{
					UpdateAllParams(edmap);
					ShowOneValues(edmap);
					ShowWeights();
				}
				if (tweaking || (triangle_display_mode && triangle_editing_mode))
					OutlineCurrentMap();
			}
			break;

      case 'V':
		case 'v':       /* Undo change if possible */
			if (!cancel_ok_msg("Undo all changes: Are you sure?"))
				return;
			if (BaseBarnmap())
			{
				_activate(fBptr);
				ConvertParams(edmap);
				if (tweaking)
				{
					UpdateAllParams(edmap);
					ShowOneValues(edmap);
					ShowWeights();
				}
				if (tweaking || (triangle_display_mode && triangle_editing_mode))
					OutlineCurrentMap();
			}
			break;

		case 'w':
      case 'W':

			if (!tweaking)
			{
				triangle_editing_mode ^= 1;
				triangle_display_mode = triangle_editing_mode;
				installmode();
				fillflock();
				if (triangle_editing_mode)
					TriangleCursor();
				else
					ArrowCursor();

				if (triangle_editing_mode)
				{
					tweakmode = 2;
					ConvertParams(edmap);
					if (triangle_display_mode)
						OutlineCurrentMap();
				}

			}
			break;

		case 'z':
      case 'Z':
			randomizeifs();
			ClearBarnmapStack();
			sprintf(current_barnmap_name, "Random (%d maps)", fBptr->n);
			_activate(fBptr);
			ConvertParams(edmap);
			if (tweaking)
			{
				edmap = 0;
				FirstMap = 0;
				ShowAll();
				UpdateAllParams(edmap);
	    if (current_main_item > LEFTARROWBOX && current_main_item < RIGHTARROWBOX)
	       invert_main_item(current_main_item);
			}
			if (tweaking || (triangle_display_mode && triangle_editing_mode))
				OutlineCurrentMap();
			if (tweaking)
			{

				if (fBptr->n < 6)
				{
					if (current_main_item == LEFTARROWBOX)
						current_main_item++;
					while (current_main_item - LEFTARROWBOX > fBptr->n)
						current_main_item--;
				}
			}
			break;

		case SHOME:
			curxinc = -curspeed;
			curyinc = -curspeed;
			break;

		case SUARROW:
			curxinc = 0;
			curyinc = -curspeed;
			break;

		case SPGUP:
			curxinc = curspeed;
			curyinc = -curspeed;
			break;

		case SLARROW:
			curxinc = -curspeed;
			curyinc = 0;
			break;

		case SRARROW:
			curxinc = curspeed;
			curyinc = 0;
			break;

		case SEND:
			curxinc = -curspeed;
			curyinc = curspeed;
			break;

		case SDARROW:
			curxinc = 0;
			curyinc = curspeed;
			break;

		case SPGDN:
			curxinc = curspeed;
			curyinc = curspeed;
			break;

		case XINSERT:
			dozoom(1);
			break;

		case XDELETE:
			dozoom(-1);
			break;
		}
	}
	if (curxinc || curyinc)
		updatecursor();
}


/*----------------------Cursor Functions---------------*/

void resetcursor()
{
	if (!tweaking)
	{
		curx = minx + (maxx + 1) / 2;
		cury = miny + (maxy + 1) / 2;
		MoveCursor(curx, cury);
	}
	ForceCursorOn();
}

void updatecursor()
{
	int oldcurx, oldcury, panx = 0, pany = 0;

	if (iteration < 10)
	{			/* dopan sets iteration to 0 */
		curxinc = 0;
		curyinc = 0;
		return;
	}
	oldcurx = curx;
	oldcury = cury;
	curx += curxinc;
	cury += curyinc;
	curxinc = 0;
	curyinc = 0;
	MoveCursor(curx, cury);

	/* No panning in tweak mode right now */
	if (!tweaking)
	{
		if (curx < 7)
			panx = -1;
		if (curx > (minx + maxx - 7))
			panx = 1;
		if (cury < 7)
			pany = 1;
		if (cury > (maxy - 7))
			pany = -1;
		if (panx | pany)
		{
			if (openflag)
			{
				dopan(panx, pany);
				return;
			}
			else
			{	/* if screen not open, reject offscreen move */
				if (panx)
					curx = oldcurx;
				if (pany)
					cury = oldcury;
			}
		}
	}
}



/*---------Map Manipulation--------------*/



/*-----------------------Map Editing--------------*/
fpair realspot(int x, int y)
{				/* gives fwindow coords of pixel */
	fpair w;

	w.x = flox + ((double) (x - minx) / maxx) * (fhix - flox);
	w.y = floy + ((double) (maxy - y) / maxy) * (fhiy - floy);
	return w;
}

fpair fbarnimage(fbarnmap * h, fpair * w)
{

	fpair z;

	z.x = h->a * w->x + h->b * w->y + h->e;
	z.y = h->c * w->x + h->d * w->y + h->f;
	return z;
}

void randomizeifs(void)
{				/* change params but keep the same weights
				 * and window */
	/* I do it this funny way so that corner angle is between pi/4, 3pi/4 */
	int i;
	double r, s, theta, phi;

	for (i = 0; i < fBptr->n; i++)
	{
		r = (double) sixteenbitsa() / (double) 0x7FFF;
		if ((0 <= r) && (r < 0.2))
			r += 0.2;
		if ((r > -0.2) && (r <= 0))
			r -= 0.2;
		s = (double) sixteenbitsa() / (double) 0x7FFF;
		if ((0 <= s) && (s < 0.2))
			s += 0.2;
		if ((s > -0.2) && (s <= 0))
			s -= -0.2;
		theta = M_PI * (double) sixteenbitsa() / (double) 0x7FFF;
		phi = (2 + (double) sixteenbitsa() / (double) 0x7FFF) * M_PI / 4;
		fBptr->h[i].a = r * cos(theta);
		fBptr->h[i].b = s * (cos(theta) * cos(phi) - sin(theta));
		fBptr->h[i].c = r * sin(theta);
		fBptr->h[i].d = s * (sin(theta) * cos(phi) + cos(theta));
		fBptr->h[i].e = fstartcenterx +
			0.3 * fstartdeltax * (double) sixteenbitsa() / (double) 0x7FFF;
		fBptr->h[i].f = fstartcentery +
			0.3 * fstartdeltay * (double) sixteenbitsa() / (double) 0x7FFF;
	}

	/*
	 * Now randomly distribute the weights. What's a good way to do this?
	 */
	for (i = 0; i < fBptr->n; i++)
		fBptr->weight[i] = 0.0;

	for (i = 0; i < 100; i++)
	{
		int which = random(fBptr->n);

		fBptr->weight[which] += .01;
	}
}
