/*
 * This program shows chaotic dynamics of particles.
 *
 * The idea is that I have a moving unit mass particle callled the bob, and I
 * have some magents attracting or repelling the bob and I have a central
 * linear restoring "pendulum" force and I can have friction on the motion.
 * The particle is tracked by keeping variables ballx,bally,speedx,and
 * speedy.  The time interval dt between updates is represented as 1/freq,
 * where freq is an integer.
 *
 * My forces can be either proportional to 1/distance^2 or 1/distance. My
 * friction can be either proportional to speed or to speed^2.
 *
 * Since bob is a unit mass, so Newton's equation force=mass*acceleration
 * reduces to acceleration=force. Acceralation is dspeed/dt, thereforeo Force
 * = dspeed/dt,so dspeed = force*dt, or dspeed = force/freq.
 *
 * I use all integers for my positions, speeds, and forces, but use reals called
 * deltaxdebt and deltaydebt to keep track of the roundoff error in 1)
 * speed+=force/freq I use reals called frictiondebt to deep track of
 * roundoff when I frictionally reduce speed with 2) abs(speed) -=
 * abs(speed)*friction/freq OR (speed^2 * friction/100*freq)
 *
 * The program also has a basinflag=1 mode in which the bob is serially released
 * from each screen position, with each start position then being colored
 * according to which magnet the bob alights on.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <dir.h>
#include <time.h>
#include "mag.h"

/* MAGNET STRUCTURE TYPE */

#define M0 0,0,0		/* for spacer in magnet table initialize */

/*
 * the following page or so is a long initialization of a global C which
 * holds 4 preset configstructures and up to 28 more saved configstructures.
 * The loadconfig() function gets things out of C and the saveconfig() puts
 * things in.  JOSH, I'd like you to add a way to get saved configs out of
 * the structure C and onto disk as well, and viceversa.
 */


/*--------------Global Variables-----------*/

magnetstructure M;		/* this global holds the current
				 * magnetstructure */

/*
 * the following page or so is a long initialization of a global C which
 * holds 4 preset configstructures and up to 28 more saved configstructures.
 * The loadconfig() function gets things out of C and the saveconfig() puts
 * things in.  JOSH, I'd like you to add a way to get saved configs out of
 * the structure C and onto disk as well, and viceversa.
 */
metaPort *thePort;
void _showmagnet(int x, int y, unsigned char color, int mode);

C_struct C =
{
	/* highest index number of  present configs */
	6,

	/* 0 is same as start values, a symmetric attractive pattern of 6 */
	/* magnetstructure */
	6,			/* number of mags */
	1,			/* autopatternload positions */
	640, 350,
	0, 0, 2,		/* x,y,charge,type of each mag */
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	/* twelve config parameters */
	8,			/* centerpull */
	2,			/* chargeunit */
	256,			/* freq */
	80,			/* radius */
	24,			/* magnetradius */
	1,			/* friction */
	0,			/* reversibleflag */
	1,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	3,			/* tracetype */
	1,			/* frictiontype */

	/* 1 repulsive reversible symmetric 5 */
	/* magnetstructure */
	5,			/* number of mags */
	1,			/* autopatternload positions */
	640, 350,
	0, 0, -20,		/* x,y,charge,type of each mag */
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,	/* unused magnet slots */
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	/* twelve config parameters */
	128,			/* centerpull */
	-20,			/* chargeunit */
	300,			/* freq */
	80,			/* radius */
	20,			/* magnetradius */
	0,			/* friction */
	1,			/* reversibleflag */
	0,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	1,			/* tracetype */
	0,			/* frictiontype */


	/* 2  attractive grid of 6x4 magnets */
	/* magnetstructure */
	24,			/* number of mags */
	4,			/* autopatternload positions */
	6, 4,
	/* x,y,charge,type of each mag */

	/* We move the whole thing in a bunch! */

	50, 60, 2,
	150, 60, 2,
	250, 60, 2,
	350, 60, 2,
	450, 60, 2,
	550, 60, 2,
	50, 160, 2,
	150, 160, 2,
	250, 160, 2,
	350, 160, 2,
	450, 160, 2,
	550, 160, 2,
	50, 260, 2,
	150, 260, 2,
	250, 260, 2,
	350, 260, 2,
	450, 260, 2,
	550, 260, 2,
	50, 360, 2,
	150, 360, 2,
	250, 360, 2,
	350, 360, 2,
	450, 360, 2,
	550, 360, 2,
	M0, M0, M0, M0, M0, M0, M0, M0,

	/* twelve config parameters */
	0,			/* centerpull */
	2,			/* chargeunit */
	256,			/* freq */
	80,			/* radius */
	20,			/* magnetradius */
	0.5,			/* friction */
	0,			/* reversibleflag */
	0,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	4,			/* tracetype */
	1,			/* frictiontype */

	/* 3  sixteen repellors */
	/* magnetstructure */
	16,			/* number of mags */
	2,			/* randomload positions */
	640, 350,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	0, 0, -20,
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,

	/* twelve config parameters */
	256,			/* centerpull */
	-20,			/* chargeunit */
	600,			/* freq */
	120,			/* radius */
	25,			/* magnetradius */
	1.4,			/* friction */
	0,			/* reversibleflag */
	0,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	3,			/* tracetype */
	2,			/* frictiontype */

	/* 4  hollow shell of 13 repellors */
	/* magnetstructure */
	13,			/* number of mags */
	1,                      /* josh's center symmetry load */
	640, 480,
	239, 197, -2,
	219, 202, -2,
	205, 215, -2,
	198, 234, -2,
	200, 253, -2,
	211, 269, -2,
	229, 278, -2,
	248, 278, -2,
	266, 269, -2,
	277, 253, -2,
	279, 234, -2,
	272, 215, -2,
	258, 202, -2,
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	/* 19 unused magnet slots */
	/* twelve config parameters */
	250,			/* centerpull */
	-2,			/* chargeunit */
	400,			/* freq */
	30,			/* radius */
	16,			/* magnetradius */
	0.53,			/* friction */
	0,			/* reversibleflag */
	0,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	3,			/* tracetype */
	1,			/* frictiontype */

	/* 5 seven dipole pairs */
	/* magnetstructure */
	14,			/* number of mags */
	0,			/* raw load */
	640, 480,
	264, 152, 4,
	252, 142, -4,
	227, 243, -4,
	371, 244, -4,
	248, 270, 4,
	377, 227, 4,
	431, 131, -4,
	435, 149, 4,
	496, 266, -4,
	513, 277, 4,
	421, 363, 4,
	440, 368, -4,
	232, 415, -4,
	249, 400, 4,

	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	/* 18 unused magnet slots */
	/* twelve config parameters */
	10,			/* centerpull */
	4,			/* chargeunit */
	450,			/* freq */
	30,			/* radius */
	16,			/* magnetradius */
	1.8,			/* friction */
	0,			/* reversibleflag */
	0,			/* xsection */
	0,			/* basinflag */
	0,			/* forcetype */
	3,			/* tracetype */
	1,			/* frictiontype */

	/* 6 draw basins of attraction */
	/* magnetstructure */
	7,			/* number of mags */
	3,			/* autopatternload positions */
	640, 350,
	0, 0, 2,		/* x,y,charge,type of each mag */
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	0, 0, 2,
	M0, M0, M0, M0, M0, M0, M0, M0, /* unused magnet slots */
	M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0,
	/* twelve config parameters */
	6,			/* centerpull */
	2,			/* chargeunit */
	128,			/* freq */
	120,			/* radius */
	30,			/* magnetradius */
	1,			/* friction */
	0,			/* reversibleflag */
	16,			/* xsection */
	1,			/* basinflag */
	0,			/* forcetype */
	1,			/* tracetype */
	1,			/* frictiontype */

};



int oldballx, oldbally;
int confignumber = 0;		/* this keeps track of which config in C is
				 * active */

/*
 * You might think the following 10 globals aren't really needed because the
 * values are in M or in C[confignumber]. But if you start tweaking the
 * values and don't necessarily want to lock them in as things saved in C,
 * then you ARE going to need these isolated globals.
 */
long centerpull = 8;
long chargeunit = 2;
long freq = 256;
double radius = 80;
long magnetradius = 24;
double friction = 1.00000000;
int reversibleflag = 0;
int xsection = 1;
int basinflag = 0;
int forcetype = 0;

int ballx, bally, deltax, deltay, location;
int ball2x, ball2y, delta2x, delta2y;
int halfclub = 10;
int oldmagnetx;
int oldmagnety;
unsigned char mfcolor = 0;
int stopped = 0;
int onestep = 0;

/*
 * ballx is position.  deltax is change passed updatespeeds(). location is
 * used for computing the sound to make.
 */
long speedx, speedy;
double fspeedx, fspeedy;
int rkflag = 0;
double deltaxdebt = 0, deltaydebt = 0;	/* correct speed roundoff */
double delta2xdebt = 0, delta2ydebt = 0;	/* correct speed roundoff */
double frictiondebtx = 0, frictiondebty = 0;	/* accumulate small effect */
int centerx, centery;		/* screen center */
int lox, hix, loy, hiy, halfx, halfy;

/* bounds for invisible motion offscreen.  I allow half a screen's worth. */
long magrad2 = 576;		/* magnetradius^2.  need it for linear force */
long magrad3 = 13824;

/* magnetradius^3. convenient to have this for updatespeeds() */
int clubx, cluby, clubxinc, clubyinc;

int memorylength = 300;
char magnetfilename[128];

/*
 * where the club is, how much arrow or mouse moved it, distance from ball
 */

CLUBTYPE clubflag = BALLBOX;

/*
 * 0 is a box around the ball, 1 is a line perpendicular to the line from
 * club to ball, and 2 is a box around a magnet.
 */
int mymag;			/* which magnet is being diddled? */
int saveballflag = 0;		/* to erase screen & keep ball in same
				 * position */
int holenumber = 6;		/* number of magnets in a symmetric pattern. */
int stride = 9; 		/* pixels club moves per arrow key press */
int soundpitch = 2;		/* higher makes higher sound */
int firsttimeflag;		/* to avoid showing the trail ball */
int swingflag = 0;		/* 0 until you press mouse button or enter */
int rollflag;			/* ball is still moving */
int inplayflag; 		/* ball is moving or waiting to be hit */
int reverseflag = 0;		/* in process of reversing (skip updatespeed) */
int frictiontype = 1;		/* if friction is on */
int helpscreenflag = 1; 	/* if you are on helpscreen */
int newholeflag = 1;		/* signals setuphole() to use formulas for
				 * mags */
int soundflag = 0;		/* noise on? */
unsigned int magnetcolor = 0;	/* used when basinflag = 1 */
int basinx = 0, basiny = 0;	/* current start point of basin */
int basinstep = 8;		/* current ste between test ponts */
int oddrowflag = 0;		/* used by basin */
int exitflag = 0;		/* 1 means quit */
int tracetype = 1;
int MagnetIsVisible = 1;
int undoflag = 0;
int savedballx, savedbally;
int savedmagnetx, savedmagnety;
int twoballs = 0;
int rodwidth = 20;
int oldrodcx, oldrodcy;
long iterations = 0L;
long basinlimit = 2000L;
int prog_init = 0;
struct
{
	int n, i, full;
	int x[512];
	int y[512];
} oldballs =

{
	300, 0, 0
};

int delayfactor = 0;		/* put in thousandths of secs to run slower */
int pauseflag = 0;		/* 1 to pause */

int minx;
int GrafixCard, CommPort;

int maxy, maxx, maxcolor;

int basindisplaymode = 0;

int cpair(point * p1, point * p2)
{
	if (p1->X < p2->X)
		return -1;
	else if (p1->X > p2->X)
		return 1;
	else if (p1->Y < p2->Y)
		return -1;
	else if (p1->Y > p2->Y)
		return 1;
	else
		return 0;
}

extern int disk_error_handler(int errval, int ax, int bp, int si);
int mode;
extern char VGAclut[16 * 3];

#define JSetPixel(x,y,z) jpixel(x,y,z)

static void drawblock(int tile, int colorband)
{
   if (!CursorShowing && (abs(basinx-clubx) < 16 && abs(basiny - cluby) < 16))
      showclub(clubx,cluby,0);

	if (tile == 8 || tile == 4)
	{
		JSetPixel(basinx, basiny, colorband);
		JSetPixel(basinx + 1, basiny, colorband);
		JSetPixel(basinx, basiny + 1, colorband);
		JSetPixel(basinx + 1, basiny + 1, colorband);
	}
   else
   {
	   JSetPixel(basinx, basiny, colorband);
	   if (tile == 2)
	   {
		   if (!oddrowflag)
		   {
			   JSetPixel(basinx - 1, basiny, 0);
			   JSetPixel(basinx - 2, basiny + 1, 0);
		   }
		   else
		   {
			   JSetPixel(basinx + 1, basiny - 1, 0);
		   }
	   }
   }
   if (!CursorShowing && (abs(basinx-clubx) < 16 && abs(basiny - cluby) < 16))
      showclub(clubx,cluby,maxcolor - 1);
}

#undef JSetPixel
/*-----------------------Main Function-------------*/
int main(int argc, char **argv)
{
	int i;
	int gif_ok = 1;
	if (!memok(sizeof(bitmap) * 2 +
		   4 * (sizeof(char *) * 9 +
		    ((11 * 1 + 7) / 8) * 9) +
		   4 * (sizeof(char *) * 11 +
		    ((13 * 1 + 7) / 8) * 11)))
   {
                fprintf(stderr, "\n\nSorry, not enough memory to run MAGNETS.");
      exit(-1);
	}
	mode = 0;
	CommPort = 0;
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
			case 'm':
				CommPort = -1;
				break;
			}
		}
		argv++, argc--;
	}

	if (!mode)
		mode = detectmode();

	if (mode != 0x10 && mode != 0x12 && mode != -1)
	{
		printf("Error: This program requires either EGA or VGA. Aborting.\n");
		exit(-1);
	}

	if (mode != -1)
		i = InitGrafix(mode == 0x12 ? -EGA640x480 : -EGA640x350);
	if (i != 0 || mode == -1)
	{
		printf("Error: Metagraphics not installed. Aborting.\n");
		exit(-1);
	}

	/* See if there is enough memory for ball and magnet bitmaps. */


	/* Lets warn the user if there is not enough memory to later do a Gif */

	if (!memok(20712L))	/* Added up mallocs in comprs.c */
		gif_ok = 0;


	harderr(disk_error_handler);

	if (CommPort == -1)
		CommPort = 0;
	else
	{
		CommPort = QueryComm();
		if (CommPort & MsDriver)
			CommPort = MsDriver;
		else if (CommPort & 2)
			CommPort = MsCOM2;
		else if (CommPort & 1)
			CommPort = MsCOM1;
		if (CommPort)
			InitMouse(CommPort);
	}

	EventQueue(true);
	prog_init = 1;		/* fatal alloc errors will now call
				 * StopEvent() and StopMouse()	*/

	SetDisplay(GrafPg0);
	TrackCursor(true);
	randomize();

	LoadDefaultPalette("MAGNETS.PAL");

	while (!exitflag)
	{
		installmode();
		usepalette();
		setuphole1(holenumber);
		setupcontrols();
		confignumber = -1;
		CannedLayout();
		//setuphole2();

		stopped = onestep = false;

		if (!gif_ok)
		        ErrorBox("There may not be enough memory to save or view a Gif.");

		while (!exitflag)
		{
			if (basinflag && basinx == 0 && basiny == 0)
				iterations = 0L;
			checkmouse();
			/*
			 * update speeds also moves ball or captive magnet if
			 * clubflag 0 or 2
			 */

			if (spinflag == 1)
				spinpalette();
			else if (spinflag == 2)
				revspinpalette();

			if (stopped)
			{
				if (!onestep || (CursorShowing && clubflag != CLUB))
					continue;
				else
					onestep = false;
			}

			if ((rollflag || clubflag != CLUB) && !exitflag)
			{
				if (swingflag && clubflag != CLUB)
					showball(ballx, bally, maxcolor);
				updatespeeds();
				/*
				 * next bunch of lines are for doing the
				 * basin.  The length is because you want to
				 * do it in four sweeps 8,4,2,1 distance
				 * between pixels, and you don't want to
				 * repeat any pixels
				 */
				if (basinflag)
				{
					iterations++;
	       magnetcolor = trackball();


					if (magnetcolor || iterations == basinlimit)
					{	/* if stopped... */
		  if (!magnetcolor)
	    			showball(ballx, bally, maxcolor);
		  firsttimeflag = 1;

		  if (basindisplaymode && magnetcolor)
		     magnetcolor = iterations * 15.0/basinlimit + 1;
		  iterations = 0L;


		  drawblock(basinstep,magnetcolor);

//						jpixel(basinx, basiny, magnetcolor);
						iterations = 0L;
						if (basinstep == 8)
						{	/* coarsest step */
							basinx += basinstep;
							if (basinx >= maxx)
							{
								basinx = 0;
								basiny += basinstep;
							}
						}
						else
						{	/* finer steps have 2
							 * kind of line */
							if (oddrowflag)
							{
								basinx += basinstep;
								if (basinx >= maxx)
								{
									basinx = basinstep;
									basiny += basinstep;
									oddrowflag = 0;
								}
							}
							else
							{
								basinx += (basinstep << 1);
								if (basinx >= maxx)
								{
									basinx = 0;
									basiny += basinstep;
									oddrowflag = 1;
								}
							}
						}
						if (basiny >= maxy)
						{
							basinstep /= 2;
							basinx = basinstep;
							basiny = 0;
							oddrowflag = 0;
						}
						if (!basinstep)
						{
		     char tbuf[128];

							for (i = 0; i < M.n; i++)
								showmagnet(M.magnet[i].x, M.magnet[i].y,
			   (locked) ? available_colors[i % 10] :
				      ((i % 15) + 1));

							basinstep = 8;
							rollflag = 0;
							clubflag = CLUB;
							inplayflag = 0;
							/*
							 * Automatically make
							 * a GIF of this
							 * fellow.
							 */
							DoGifOutput(false);
                     sprintf(tbuf,"%s has been created",magnetfilename);
		     ErrorBox(tbuf);
						}
						ballx = basinx;
						bally = basiny;
						speedx = 0;
						speedy = 0;
						deltax = 0;
						deltay = 0;
						deltaxdebt = 0;
						deltaydebt = 0;
						frictiondebtx = 0;
						frictiondebty = 0;	/* clean restart */
		  fspeedx = 0;
		  fspeedy = 0;
					}
				}
				/*
				 * end of the basin part, which you normally
				 * skip
				 */
				else
					moveball();
				delay(delayfactor);
				if (soundflag && clubflag != BALLBOX && !CursorShowing && clubflag != MAGBOX)
				{	/* not boxed ball */
					location = (soundpitch *
					       distance(ballx, bally)) / 70;
					if (location < 3)
						location = 3;

					if (rkflag)
						speedx = fspeedx, speedy = fspeedy;

					sound((unsigned int) (distance(
						  (int) (speedx / location),
					       (int) (speedy / location))));
				}
			}
		}
	}
	nosound();
   grayflag = 0;
	grayscale();
	usepalette();
	SetDisplay(TextPg0);
	ClearText();
	StopMouse();
	StopEvent();
	return exitflag;
}

/*------------------------Load and Save Functions---------*/
void startconfig(void)
{
	saveballflag = 0;	/* start ball in standard position */
	//if (!CursorShowing)
		//showclub(clubx, cluby, 0);
	installmode();
	setuphole(holenumber);
	DrawControlPage();
}
void loadconfig(configstructure * p)
{
	int i;

	M = p->M;

	/* If autopattern is 0, scale the magnets appropriately. */
	if (M.autopattern == 0)
	{
		for (i = 0; i < M.n; i++)
		{
			M.magnet[i].x = ((float) maxx * M.magnet[i].x) / (float) M.maxx;
			M.magnet[i].y = ((float) maxy * M.magnet[i].y) / (float) M.maxy;
		}
	}
	centerpull = p->centerpull;
	chargeunit = p->chargeunit;
	freq = p->freq;
	radius = p->radius;
	magnetradius = p->magnetradius;
	friction = p->friction;
	reversibleflag = p->reversibleflag;
	xsection = p->xsection;
	basinflag = p->basinflag;
	forcetype = p->forcetype;
	tracetype = p->tracetype;
	frictiontype = p->frictiontype;

	holenumber = p->M.n;
	magrad2 = magnetradius * magnetradius;
	magrad3 = magrad2 * magnetradius;

	switch (p->M.autopattern)
	{
	case 0: 	/* use config magnet info */
		newholeflag = 0;
		break;
	case 1: 	/* calculate positions */
		newholeflag = 1;
		break;
	case 2: 	/* randomize positions */
		newholeflag = 0;
		for (i = 0; i < M.n; i++)
		{
			M.magnet[i].x = maxx / 8 + random(6 * maxx / 8);
			M.magnet[i].y = maxy / 8 + random(6 * maxy / 8);
		}
		break;
	case 3: 	/* randomize positions */
		newholeflag = 0;
		for (i = 0; i < M.n; i++)
		{
			M.magnet[i].x = maxx / 8 + random(6 * maxx / 8);
			M.magnet[i].y = maxy / 10 + random(maxy / 3);
		}
		break;
	case 4:
		/*
		 * In autopattern 4, M.maxx and M.maxy are the number of rows
		 * and columns of to create. Scale them so that we fill 2/3
		 * of the screen.
		 */
		for (i = 0; i < M.maxy; i++)
		{
			int row = sR.Ymax / 6 + (2 * i * sR.Ymax) / ((M.maxy - 1) * 3);
			int j;

			for (j = 0; j < M.maxx; j++)
			{
				int col = sR.Xmax / 6 + (2 * j * (sR.Xmax - minx)) / ((M.maxx - 1) * 3);

				M.magnet[i * M.maxx + j].x = col;
				M.magnet[i * M.maxx + j].y = row;
			}
		}
		break;

	}

   startconfig();
}
void saveconfig(configstructure * p)
{
	M.autopattern = 0;
	p->M = M;
	p->M.maxx = maxx;
	p->M.maxy = maxy;
	p->centerpull = centerpull;
	p->chargeunit = chargeunit;
	p->freq = freq;
	p->radius = radius;
	p->magnetradius = magnetradius;
	p->friction = friction;
	p->reversibleflag = reversibleflag;
	p->xsection = xsection;
	p->basinflag = basinflag;
	p->forcetype = forcetype;
	p->tracetype = tracetype;
	p->frictiontype = frictiontype;
}

void add_a_magnet(int X, int Y)
{
	int color;

	if (holenumber < MAXMAGS - 1)
	{
		/* Create a new magnet. */
		M.magnet[holenumber].x = clubx;
		M.magnet[holenumber].y = cluby;
		M.magnet[holenumber].charge = chargeunit;
		if (basinflag)
			color = (locked) ? available_colors[holenumber % 10] :
			    ((holenumber % 15) + 1);
		else if (chargeunit < 0)
			color = RED;
		else
			color = LIGHTBLUE;
		M.n++;
		holenumber++;
		showmagnet(clubx, cluby, color);

		/* And move the cursor away. */
		MoveCursor(minx + X + 10, Y + 10);
	}
}

void delete_a_magnet()
{
	int color, i;

	/* Get rid of the old sucker. */
	if (basinflag)
		color = locked ? available_colors[mymag % 10] : ((mymag % 15) + 1);
	else if (M.magnet[mymag].charge < 0)
		color = RED;
	else
		color = LIGHTBLUE;

	/* Get rid of the old frame */
	showclub(M.magnet[mymag].x, M.magnet[mymag].x, 0);	/* erase box */

	showmagnet(M.magnet[mymag].x, M.magnet[mymag].y, color);
	for (i = 0; i < holenumber - 1 - mymag; i++)
		M.magnet[mymag + i] = M.magnet[mymag + i + 1];
	holenumber--;
	M.n--;
	clubflag = CLUB;
	showclub(clubx, cluby, maxcolor - 1);	/* show it as stick */
}

void DoAllButtons(void)
{

	/* Have to redraw all the number things. */
	drawRadiusButton();
	drawChargeButton();
	drawFrequencyButton();
	drawFrictionButton();
	drawCenterpullButton();
	drawCaptureButton();
	stopped = onestep = false;
	SetStopSign();
}


void CannedLayout(void)
{
	char tbuf[40];

	confignumber++; 	/* panel 2 */
	if (confignumber > C.n)
		confignumber = 0;
	loadconfig(&C.config[confignumber]);
	MoveTo(minx + 4, sR.Ymax);
	TextAlign(alignLeft, alignBottom);
	sprintf(tbuf, "Layout %d", confignumber + 1);
	PenColor(LIGHTGRAY);
	BackColor(BLACK);
	PenMode(zREPz);
	DrawString(tbuf);
   DoAllButtons();
   if (!basinflag)
      MoveCursor(ballx+minx,bally);
   CursorShowing=false;
}

void DoGifOutput(int wholescreen)
{
	/* First, find a possible file name to dick with. */
	struct ffblk ffblk;
	int done;
	int number = -1;
	int mynum;

	done = findfirst("MAGNET??.GIF", &ffblk, 0);
	while (!done)
	{
		sscanf(ffblk.ff_name, "MAGNET%d.GIF", &mynum);
		if (mynum > number)
			number = mynum;
		done = findnext(&ffblk);
	}

	sprintf(magnetfilename, "MAGNET%02d.GIF", number + 1);
	GifOutput(magnetfilename, wholescreen);
}
int oldchargeflag=0;
void NewRadius(void)
{
	newholeflag = 1;
   oldchargeflag = 1;
	installmode();
	setuphole(holenumber);
}
void ResetLayout(void)
{
	installmode();

   if (tracetype != 4 && !(basinflag && firsttimeflag))
   	showball(ballx, bally, maxcolor);
	firsttimeflag = 1;
	showmagnets();
	showclub(clubx, cluby, maxcolor - 1);
	oldballs.i = 0;
	oldballs.full = 0;
	basinx = basiny = 0;
   inptr = outptr = erasing = 0;
	basinstep = 8;
   if (tracetype >= 3)
      donttrace = true;
   iterations = 0L;
}

void ToggleBasins(void)
{
	/* Reset the limits for the basin flag */
	basinflag ^= 1;
	basinx = basiny = 0;
	basinstep = 8;
	rollflag = 0;
   iterations = 0L;
}


/*----------------------Icon Functions---------------*/




void showball(int x, int y, int color)
{
	/* Showball is a bit sneaky. I use bitblits for speed. */
	int offset, width;

	int i, j;
	static bitmap far *theBitmap;
	metaPort lclPort;
	static bitmap *bbm = NULL;

	static unsigned char ballicon[9][11] = {
		{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0},
		{0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
		{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
		{0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
		{0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}
	};
	static rect ballRect = {0, 0, 10, 8};
	rect dropRect;
	point P;



	if (bbm == NULL)
	{
		/* First time. Set up the ball bitmap. */
		GetPort(&thePort);
		bbm = malloc(sizeof(bitmap));
		if (!bbm)
		{
			ErrorBox("ERROR allocating ball bitmap\n");
			if (CommPort)
			    StopMouse();
			StopEvent();
			exit(-1);
		}
		/* Copy the official one first */
		theBitmap = thePort->portBMap;

		*bbm = *theBitmap;

		/* Put in our own info */
		bbm->devType = 0;
		bbm->pixWidth = 11;
		bbm->pixHeight = 9;
		bbm->rowBytes = (bbm->pixWidth * bbm->pixBits + 7) / 8;
		/* Allocate the row tables */
		for (i = 0; i < 4; i++)
		{
			bbm->mapTable[i] = (map *) malloc((sizeof(char *)) * bbm->pixHeight);
			bbm->mapTable[i]->rowTable[0] =
				(unsigned char *) malloc(bbm->rowBytes * bbm->pixHeight);
		}
		InitRowTable(bbm, 0, 0, 0);

		/* Now write the mess into the bitmap... */
		InitPort(&lclPort);
		PortBitmap(bbm);
		RasterOp(zREPz);
		for (j = 0; j < 9; j++)
		{
			for (i = 0; i < 11; i++)
			{
				PenColor(ballicon[j][i] ? color : BLACK);
				SetPixel(i, j);
			}
		}

		SetPort(thePort);

	}
	P.X = x;
	P.Y = y;
	MapPt(&P, &thePort->portVirt, &thePort->portRect);
	x = P.X;
	y = P.Y;

	/* Don't do this at all if we are off the proper area of the screen. */
	/* OK, now place the ball so that its center is at X,Y. */
	if (x > 6)
		width = 11;
	else
		width = 11 - (6 - x);
	offset = 11 - width;

	if (width > 0)
	{
		dropRect.Xmin = ((width < 11) ? 0 : x - 5) + minx;
		dropRect.Ymin = y - 4;
		dropRect.Xmax = ((width < 11) ? width - 1 : x + 5) + minx;
		dropRect.Ymax = y + 4;

		ballRect.Xmin = (width < 11) ? offset : 0;
		ballRect.Xmax = 10;
		ballRect.Ymin = 0;
		ballRect.Ymax = 8;
		CopyBits(bbm, theBitmap, &ballRect, &dropRect, &thePort->portRect, zXORz);
		oldballx = x;
		oldbally = y;
	}
}

void _showmagnet(int x, int y, unsigned char color, int mode)
{
	/*
	 * Magnet works the same way ball does, mostly for speed when we are
	 * dragging it.
	 */



	static unsigned char lastcolor = 0xff;

	int i, j;
	static bitmap *theBitmap;
	static metaPort *thePort;
	metaPort lclPort;
	static bitmap *mbm = NULL;
	static rect magRect = {0, 0, 12, 10};
	rect dropRect;
	pusharea p;
	point P;

	static unsigned char magneticon[11][13] = {
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}};

	if (mbm == NULL)
	{
		GetPort(&thePort);
		RasterOp(zREPz);
		mbm = malloc(sizeof(bitmap));
		if (!mbm)
		{
			ErrorBox("ERROR allocating magnet bitmap\n");
			if (CommPort)
			    StopMouse();
			StopEvent();
			exit(-1);
		}
		theBitmap = thePort->portBMap;
		*mbm = *theBitmap;
		mbm->devType = 0;
		mbm->pixWidth = 13;
		mbm->pixHeight = 11;
		mbm->rowBytes = (mbm->pixWidth * mbm->pixBits + 7) / 8;
		/* Allocate the row tables */
		for (i = 0; i < 4; i++)
		{
			mbm->mapTable[i] = (map *) malloc((sizeof(char *)) * mbm->pixHeight);
			mbm->mapTable[i]->rowTable[0] =
				(unsigned char *) malloc(mbm->rowBytes * mbm->pixHeight);
		}
		InitRowTable(mbm, 0, 0, 0);
	}
	if (color != lastcolor)
	{
		PushGrafix(&p);
		InitPort(&lclPort);
		PortBitmap(mbm);
		for (j = 0; j < 11; j++)
		{
			for (i = 0; i < 13; i++)
			{
				PenColor(magneticon[j][i] ? color : BLACK);
				SetPixel(i, j);
			}
		}
		SetPort(thePort);
		PopGrafix(&p);
	}
	lastcolor = color;
	P.X = x;
	P.Y = y;
	MapPt(&P, &thePort->portVirt, &thePort->portRect);
	x = P.X;
	y = P.Y;
	if (x - halfclub > 0)
	{
		dropRect.Xmin = x - 6 + minx;
		dropRect.Ymin = y - 5;
		dropRect.Xmax = dropRect.Xmin + 12;
		dropRect.Ymax = dropRect.Ymin + 10;
		CopyBits(mbm, theBitmap, &magRect, &dropRect, &thePort->portRect, mode);
		MagnetIsVisible = true;
	}
	else
		MagnetIsVisible = false;
}

void showmagnet(int x, int y, unsigned char color)
{
	_showmagnet(x, y, color, zXORz);
}

void showmagnets()
{
	int i;
	unsigned char color;
	int X, Y;

	for (i = 0; i < M.n; i++)
	{
		if (basinflag)
			color = locked ? available_colors[i % 10] : ((i % 15) + 1);
		else if (M.magnet[i].charge < 0)
			color = RED;
		else
			color = LIGHTBLUE;

		X = M.magnet[i].x;
		Y = M.magnet[i].y;

		showmagnet(X, Y, color);
	}
}


void updateonemagnet(int i, long oldcharge)
{
	int oldcolor = (oldcharge < 0) ? RED : LIGHTBLUE;
	int newcolor = (M.magnet[i].charge < 0) ? RED : LIGHTBLUE;

	if (newcolor != oldcolor)
	{
		showmagnet(M.magnet[i].x, M.magnet[i].y, oldcolor);
		showmagnet(M.magnet[i].x, M.magnet[i].y, newcolor);
	}
}

void updateallmagnets(magnetstructure * oldM)
{
	int i;

	for (i = 0; i < M.n; i++)
		updateonemagnet(i, oldM->magnet[i].charge);
}


void updatemagnetcolor(int i)
{
	/* erase old color with xor write and put in new color */


	if (M.magnet[i].charge < 0)
	{
		showmagnet(M.magnet[i].x, M.magnet[i].y, LIGHTBLUE);
		showmagnet(M.magnet[i].x, M.magnet[i].y, RED);
	}
	else
	{
		showmagnet(M.magnet[i].x, M.magnet[i].y, RED);
		showmagnet(M.magnet[i].x, M.magnet[i].y, LIGHTBLUE);
	}
}



void showclub(int x, int y, int color)
{
	/*
	 * clubflag determines kind of club.  0 is a box around the ball,
	 * which drags the ball, and 1 is a line that points at the ball, and
	 * 2 is a box around a magnet.
	 */
	static int x1, y1, x2, y2, x3, y3;
	double cdx, cdy, hypotenuse;
	int dx, dy, i, clubballx, clubbally, clubmagx, clubmagy;
	static rect boxR;
	pusharea savearea;
	int boxcolor = (maxcolor > 1) ? maxcolor - 1 : 1;

   if (x == -9999)
   {
      x1 = 9999;
      return;
   }
	PushGrafix(&savearea);
	RasterOp(zXORz);

	if (!color)
	{			/* erase club */
      if (x1 != -9999)
      {
         if (clubflag == CLUB)	/* stick case */
   			jxwbresen(x1, y1, x2, y2, boxcolor, 1 /* 3 */ );
   		else
   		{		/* box case */
   			/* Just xor in a box */
   			/* Don't do it if too small! */
   			ClipRect(&displayRect);
     			PenColor(boxcolor);
   			PenSize(1, 1);
   			FrameRect(&boxR);
   			ClipRect(&sR);
   		}
      }
      x1 = -9999;
	}
	else
	{
		if (clubflag == CLUB)
		{		/* check if you should switch to box */
			clubballx = ballx - x;	/* check ball first */
			clubbally = bally - y;
			cdx = (double) clubballx;	/* I use reals here so I can */
			cdy = (double) clubbally;	/* do trig for tilt of club */
			hypotenuse = sqrt(cdx * cdx + cdy * cdy);
			if (hypotenuse < 6 && !basinflag)
			{
				clubflag = BALLBOX;

				savedballx = clubx = ballx;
				savedbally = cluby = bally;	/* lock onto the ball */
				speedx = 0;	/* stop it */
				speedy = 0;
				nosound();
			}
			if (clubflag == CLUB)	/* if not locked on ball, check magnets */
				for (i = 0; i < M.n; i++)
				{
					clubmagx = clubx - M.magnet[i].x;
					clubmagy = cluby - M.magnet[i].y;
					if (distance(clubmagx, clubmagy) < 6)
					{
						clubflag = MAGBOX;	/* integer distance OK
									 * here */
						clubx = M.magnet[i].x;
						cluby = M.magnet[i].y;
						mymag = i;
						sound(300);	/* signal capture of
								 * magnet */
						delay(50);
						nosound();
						savedmagnetx = clubx;
						savedmagnety = cluby;
					}
				}
			if (clubflag == CLUB)
			{
	    /* if still not box draw new stick */
            /* it is indeed possible for the fucker to be right on the
               ball. So: */
            if (hypotenuse == 0)
            {
   				dx = 0;
               dy = halfclub * clubbally;
            }
            else
            {
   				dx = (int) ((double) (halfclub * clubbally) / hypotenuse);
   				dy = (int) ((double) (halfclub * clubballx) / hypotenuse);
            }

				x1 = x - dx;
				y1 = y + dy;
				x2 = x + dx;
				y2 = y - dy;
				jxwbresen(x1, y1, x2, y2, color, 1 /* 3 */ );
			}
		}
		if (clubflag != CLUB)
		{		/* box case */
			boxR.Xmin = minx + (x1 = clubx - halfclub);
			boxR.Ymin = y1 = cluby - halfclub;
			boxR.Xmax = minx + (x3 = clubx + halfclub);
			boxR.Ymax = y3 = cluby + halfclub;

			ClipRect(&displayRect);
			PenColor(boxcolor);
			RasterOp(zXORz);
			PenSize(1, 1);
			FrameRect(&boxR);
	 RasterOp(zREPz);
			ClipRect(&sR);
		}
	}
	/* Always force the cursor to where the club is */
	PopGrafix(&savearea);
	if (!CursorShowing)
		MoveCursor(minx + clubx, cluby);
}

void loadcharges()
{
	int i;

	for (i = 0; i < M.n; i++)
	{
		M.magnet[i].charge = chargeunit;
	}
}

void setbounds()
{

	halfx = maxx / 2;
	halfy = maxy / 2;
	lox = -halfx;
	hix = maxx + halfx;
	loy = -halfy;
	hiy = maxy + halfy;
}

void setuphole1(int n)
{
	int i;
	short resX, resY;
	double ourradius = radius * ((double) maxy) / 349.0;
   static int lastcount=0;

	speedx = 0;
	speedy = 0;
	deltax = 0;
	deltay = 0;
	inplayflag = 1;
	if (basinflag)
		rollflag = 1;
	else
		rollflag = 0;
	frictiondebtx = 0;
	frictiondebty = 0;
	deltaxdebt = 0;
	deltaydebt = 0;
	setbounds();
	firsttimeflag = 1;
	centerx = maxx / 2;
	centery = maxy / 2;
	stride = 9;
   inptr = outptr = erasing = 0;

	QueryRes(&resX, &resY);
	aspect = (float) resX / (float) resY;

	if (newholeflag == 1)
	{
		M.n = n;
		if (n == 1)
		{
			M.n = 1;
			M.magnet[0].x = centerx;
			M.magnet[0].y = centery;
		}

		else
		{
			double offset;

			switch (n)
			{
			case 2:
				offset = 0.0;
				break;
			case 4:
				offset = M_PI / 4.0;
				break;
			default:
				offset = M_PI / 2.0;
			}
			for (i = 0; i < n; i++)
			{
				M.magnet[i].x = centerx +
					aspect * ourradius * cos(((float) i) / (float) n * 2.0 * M_PI + offset);
				M.magnet[i].y = centery -
					ourradius * sin(((float) i) / (float) n * 2.0 * M_PI + offset);
			}
		}
      if (!oldchargeflag)
   		for (i = 0; i < M.n; i++)
   			M.magnet[i].charge = chargeunit;

      lastcount = M.n;
		newholeflag = oldchargeflag = 0;
		M.autopattern = 1;
	}
}

void setuphole2(void)
{
	int i;

	if (basinflag)
	{
		ballx = 0;
		bally = 0;
		basinx = 0;
		basiny = 0;
		for (i = 0; i < M.n; i++)
		{
	 int color = (locked) ? available_colors[i] : ((i % 15) + 1);
			if (newholeflag)
				M.magnet[i].charge = chargeunit;

			showmagnet(M.magnet[i].x, M.magnet[i].y, color);
		}
		if (!CursorShowing)
			showclub(clubx, cluby, maxcolor - 1);
		return;
	}

	if (!saveballflag)
	{
		ballx = maxx / 4;
		bally = (3 * maxy) / 4;
		ball2x = ballx;
		ball2y = bally;
		oldrodcx = ballx;
		oldrodcy = bally;
	}
	saveballflag = 0;

  	clubx = ballx;
  	cluby = bally;
	showball(ballx, bally, maxcolor);
	showclub(clubx, cluby, maxcolor - 1);
	showmagnets();
	/* Set the cursor to be where the ball is */

	if (!CursorShowing)
		MoveCursor(ballx + minx, bally);
}

void ResetBasins(void)
{
   basinx = basiny = 0;
   basinstep = 8;
	speedx = 0;
	speedy = 0;
	deltax = 0;
	deltay = 0;
	deltaxdebt = 0;
	deltaydebt = 0;
	frictiondebtx = 0;
	frictiondebty = 0;	/* clean restart */
   fspeedx = 0.0;
   fspeedy = 0.0;
}

void setuphole(int n)
{
	setuphole1(n);
	setuphole2();
   ResetBasins();
}

/*--------------------Ball Motion-------*/

int intsqrt(unsigned int x)
{

	register unsigned int fact1, fact2;
	static int babyroot[5] = {0, 1, 1, 2, 2};

	if (x < 5)
		return babyroot[x];
	else
	{
		fact1 = 2;
		fact2 = x >> 1;
		while ((fact1 != fact2 - 1) && (fact1 != fact2) &&
		       (fact1 != fact2 + 1))
		{
			fact1 = (fact1 + fact2) >> 1;
			fact2 = x / fact1;
		}
		return ((int) fact1);
	}
}


/*
 * distance is based on fact that square root of 32 K is shade over 181, so
 * sum of two of these is still handleable.
 */


int distance(int dx, int dy)
{
	unsigned int adx, ady;

	adx = abs(dx);
	ady = abs(dy);

	if ((adx < 182) && (ady < 182))
		return (intsqrt(adx * adx + ady * ady));
	else
	{
		adx >>= 1;
		ady >>= 1;
		return (2 * distance(adx, ady));
	}
}

int round(double x)
{
	if (x > 0)
		return ((int) (floor(x)));
	else
		return ((int) (ceil(x)));
}
