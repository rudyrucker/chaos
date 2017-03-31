#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
//#define IsExtern
//#include "usual.h"
#include "mag.h"

#define MAXMAGS 32
#define MAXCONFIGS 32
#define M0 0,0,0		/* for spacer in magnet table initialize */
/*-------------Types------------------*/
extern magnetstructure M;
extern int locked,available_colors[10];
typedef struct
{
	double x, y;
} fpair;

extern long centerpull;
extern long chargeunit;
extern long freq;
extern double radius;
extern long magnetradius;
extern double friction;
extern int reversibleflag;
extern int xsection;
extern int basinflag;
extern int forcetype;

extern int ballx, bally, deltax, deltay, location;
extern int ball2x, ball2y, delta2x, delta2y;
extern int halfclub;
extern int oldmagnetx;
extern int oldmagnety;
extern unsigned char mfcolor;
unsigned char lastmfcolor;
extern int oldballx, oldbally;

/*
 * ballx is position.  deltax is change passed updatespeeds(). location is
 * used for computing the sound to make.
 */
extern long speedx, speedy;
extern double fspeedx, fspeedy;
extern double deltaxdebt, deltaydebt;	/* correct speed roundoff */
extern double delta2xdebt, delta2ydebt;	/* correct speed roundoff */
extern double frictiondebtx, frictiondebty;	/* accumulate small effect */
extern int centerx, centery;	/* screen center */
extern int lox, hix, loy, hiy, halfx, halfy;

/* bounds for invisible motion offscreen.  I allow half a screen's worth. */
extern long magrad2;		/* magnetradius^2.  need it for linear force */
extern long magrad3;

/* magnetradius^3. convenient to have this for updatespeeds() */
extern int clubx, cluby, clubxinc, clubyinc;

/*
 * where the club is, how much arrow or mouse moved it, distance from ball
 */

/*
 * 0 is a box around the ball, 1 is a line perpendicular to the line from
 * club to ball, and 2 is a box around a magnet.
 */
extern int mymag;		/* which magnet is being diddled? */
extern int saveballflag;	/* to erase screen & keep ball in same
				 * position */
extern int holenumber;		/* number of magnets in a symmetric pattern. */
extern int stride;		/* pixels club moves per arrow key press */
extern int soundpitch;		/* higher makes higher sound */
extern int firsttimeflag;	/* to avoid showing the trail ball */
extern int swingflag;		/* 0 until you press mouse button or enter */
extern int rollflag;		/* ball is still moving */
extern int inplayflag;		/* ball is moving or waiting to be hit */
extern int reverseflag;		/* in process of reversing (skip updatespeed) */
extern int frictiontype;	/* if friction is on */
extern int helpscreenflag;	/* if you are on helpscreen */
extern int newholeflag;		/* signals setuphole() to use formulas for
				 * mags */
extern int soundflag;		/* noise on? */
extern unsigned int magnetcolor;/* used when basinflag = 1 */
extern int basinx, basiny;	/* current start point of basin */
extern int basinstep;		/* current ste between test ponts */
extern int oddrowflag;		/* used by basin */
extern int exitflag;		/* 1 means quit */
extern int tracetype;
extern int BallIsVisible;
extern int MagnetIsVisible;
extern int undoflag;
extern int savedballx, savedbally;
extern int savedmagnetx, savedmagnety;
extern int CursorShowing;
extern int twoballs;
extern int rodwidth;
extern int memorylength;

extern struct 
{
	int n, i, full;
	int x[512];
	int y[512];
} oldballs;

point old_balls[512];
int inptr=0;
int outptr = 0;
int erasing = false;
char erasures[512];


extern int delayfactor;		/* put in thousandths of secs to run slower */
extern int pauseflag;		/* 1 to pause */

extern int minx;
extern int oldrodcy, oldrodcx;
extern int GrafixCard, CommPort;
extern int rkflag;		/* means use runge kutte integration */

/*-----------------------Function Declarations--------------*/
fpair derivs(fpair *);
fpair rkmove(int, int);

void DrawControlPage(void);

#define sign(x)		((x)<0 ? -1 : 1)

extern rect controlRect;
extern rect displayRect;

/*
 * The inverse square force of the magnets obeys: force = charge / distance ^
 * 2, and forcex / force = dx / distance, so forcex = charge * dx / distance
 * ^ 3
 *
 *
 * If we allow the magnet to be an extended disk of radius K, rather than a
 * point charge, then if r is less than K, we have an effective charge of
 * (r/K)**2 * C.  So the force, inside the magnet is this over r**2 which,
 * surprise!, is a constant C/ K**2.
 *
 * On the other hand if the magnet is a SPHERE, then effective charge is (r/K)^3 *
 * C, so force is r * C / K^3, so with forcex/force = dx/r, forcex =
 * dx*C/K^3.
 *
 * Inverse linear force obeys force = charge / distance , and forcex / force =
 * dx / distance, so forcex = charge * dx / distance ^ 2
 *
 * Inisde a magnet we should get distance times what we got with inverse square
 * so we ought to have forcex = r*dx*C/K^3, but this goes batshit, is too
 * strong, so fuck the r and copy what you do with the inverse square case.
 *
 *
 * The linear restoring force of the "pendulum" obeys: force = k * distance, and
 * forcex/ force = dx / distance, so forcex = k * distance * dx / distance,
 * forcex = k * dx.
 *
 * The linear friction fource is the same story, frictionx/netfriction =
 * speedx/speed and netfriction = speed * friction,so frictionx =
 * (speedx/speed)* friction*speed = speedx * friction.
 *
 * Square friction has netfriction= friction * speed*speed, so frictionx =
 * (speedx/speed) * (friction * speed * speed) = speedx*friction*speed.
 */
int donterase = -1;
#pragma warn -sig
void updatespeeds()
{
	int dx, dy, clubball, hitforce, i;
	long r, rdiv, force, forcex, forcey, speed;
	fpair fdr;
	double rounddown, frictiondebt, fspeed;
	unsigned char color;


	/*
	 * If I've just reversed direction by the 'v' key, then i don't do
	 * this update
	 */




	if (reverseflag)
	{
		reverseflag = 0;
		return;
	}
	if (basinflag)
		color = locked ? available_colors[mymag % 10] : ((mymag % 15) + 1);
	else if (M.magnet[mymag].charge < 0)
		color = RED;
	else
		color = LIGHTBLUE;


	if (swingflag)
	{
		swingflag = 0;
		if (clubflag == CLUB)
		{
			/*
			 * Hit linearly harder the closer you are. hitforce
			 * is k(maxdist - distance), and we use Fx = F *
			 * (dx/dist) as customary. I rescale so a hit counts
			 * the same for all freqs.
			 */
			dx = ballx - clubx;
			dy = bally - cluby;
			clubball = distance(dx, dy);	/* can't be 0 here,
							 * because clubflag=1 */
			hitforce = (306 - clubball) / 15;	/* biggest is 20 */
			if (hitforce < 0)
				hitforce = 0;
			if (!rkflag)
			{
				speedx += (long) (freq * dx);	/* so hit is same at */
				speedy += (long) (freq * dy);	/* at different freqs */
				deltax = speedx / freq;
				deltay = speedy / freq;
			}
			else
			{
				fspeedx += (double) freq *dx;
				fspeedy += (double) freq *dy;

				deltax = fspeedx / freq;
				deltay = fspeedy / freq;
			}
			if (pauseflag)
			{	/* if pause, don't do it */
				deltax = 0;
				deltay = 0;
			}
			return;
		}
		else
		{		/* box case of swingflag */
			i = clubflag;
			showclub(clubx, cluby, 0);	/* erase the box */
			showball(ballx, bally, maxcolor);	/* and the ball */

			if (clubflag == BALLBOX)
			{	/* 0 means around ball */
				clubx = ballx - 10;	/* move it away from
							 * ball */
				cluby = bally - 10;
			}
			else
			{	/* move it away from magnet */
				clubx = M.magnet[mymag].x - 10;
				cluby = M.magnet[mymag].y - 10;
			}
			MoveCursor(minx + clubx, cluby);
         if (!basinflag && clubflag != MAGBOX)
            jxpixel(ballx,bally,GREEN);
			clubflag = 1;	/* change to normal club */
			showclub(clubx, cluby, maxcolor - 1);	/* show it as stick */
         donterase = inptr;
			if (tracetype == 4 && !basinflag)
            showball(ballx, bally, maxcolor);	/* and the ball */
		}
	}
	if (undoflag)
	{
		/*
		 * Oops, we didn't really want to move whatever we have, if
		 * we really have one anyway.
		 */

		/* Turn off the frame */
		showclub(clubx, cluby, 0);	/* show it as stick */
		if (!clubflag)
		{
			ballx = savedballx;
			bally = savedbally;
		}
		else
		{
			showmagnet(M.magnet[mymag].x, M.magnet[mymag].y, color);
			M.magnet[mymag].x = savedmagnetx;
			M.magnet[mymag].y = savedmagnety;
			showmagnet(M.magnet[mymag].x, M.magnet[mymag].y, color);

		}
		clubflag = 1;
		showclub(clubx, cluby, maxcolor - 1);	/* show it as stick */
		undoflag = 0;
	}
	if (!(clubflag == CLUB))
	{			/* box case of updatespeeds() */
		if (!clubflag)
		{		/* 0 is box on ball.  move ball to club */
			deltax = clubx - ballx;
			deltay = cluby - bally;
			return;	/* done with ball for now */
		}
		else
		{		/* if box on magnet move the magnet to club */
			/* erase magnet with xor write, move, redraw */
			showmagnet(M.magnet[mymag].x, M.magnet[mymag].y, color);
			M.magnet[mymag].x = clubx;
			M.magnet[mymag].y = cluby;
			showmagnet(M.magnet[mymag].x, M.magnet[mymag].y, color);
			/*
			 * no return means I let ball keep moving all the
			 * while
			 */
		}
	}
	if (pauseflag)
	{			/* if pause freeze ball and skip the rest */
		deltax = 0;
		deltay = 0;
	}
	else
	{
		if (!rkflag)
		{
			forcex = 0;
			forcey = 0;
			switch (forcetype)
			{
			case 0:/* inverse square force */
				for (i = 0; i < M.n; i++)
				{
					dx = M.magnet[i].x - ballx;
					dy = M.magnet[i].y - bally;
					r = (long) distance(dx, dy);
					if (r > magnetradius)
					{
						rdiv = 50000000L / (long) (r * r * r);
						/*
						 * if freq 100, charge 10,
						 * and dx is 300, term is
						 * about (20,000,000 /
						 * 27,000,000 ) * (10 * 300 /
						 * 100) about 30. with no net
						 * deltax Keep in mind that
						 * long ints range from - 2
						 * billion to +2 billion. If
						 * dx is 20, you get
						 * (50,000,000 / 8000 ) * (10 *
						 * 20 / 100), round 8000 to
						 * 10,000 and get 10,000.
						 * with deltax = speedx/freq
						 * = 100, which is fine.
						 */
						force = rdiv * M.magnet[i].charge;
						forcex += (force * (long) dx) / freq;
						forcey += (force * (long) dy) / freq;
					}
					else
					{
						/*
						 * if magrad is 20, and dx is
						 * 20, term is ( 20 * 10 *
						 * (50,000,000/( 100 * 8,000)
						 * ), round 8,000 to 10,000,
						 * and get 10,000, matching
						 * the value from the other
						 * method.
						 */
						rdiv = 50000000L / (freq * magrad3);
						force = rdiv * M.magnet[i].charge;
						forcex += dx * force;
						forcey += dy * force;
					}
				}
				break;
			case 1:/* inverse linear force */
				for (i = 0; i < M.n; i++)
				{
					dx = M.magnet[i].x - ballx;
					dy = M.magnet[i].y - bally;
					rdiv = ((long) (dx) * (long) (dx)) +
						((long) (dy) * (long) (dy));
					if (rdiv > magrad2)
					{
						/*
						 * with the assumptions as
						 * before, and dx 300, this
						 * term is (400,000/ 100) *
						 * 10 * 300) / 90,000 ) =
						 * about 120. If dx = 20 this
						 * term is (400,000/100) * 10 *
						 * 20 / 400 = 2,000, which is
						 * a deltax of 20.
						 */
						force = (400000L / freq) * M.magnet[i].charge;
						forcex += (force * (long) dx) / rdiv;
						forcey += (force * (long) dy) / rdiv;
					}
					else
					{
						/** if magrad is 20, and dx is 20, term is
					        ( 20 * 10 * (8,000,000/( 100 * 8,000) ) = 2000 to match.*/
						rdiv = 8000000L / (freq * magrad3);
						force = rdiv * M.magnet[i].charge;
						forcex += dx * force;
						forcey += dy * force;
					}
				}
				break;
			}	/* end switch */
			dx = centerx - ballx;
			dy = centery - bally;

			/*
			 * if dx is 300 and centerpull is 16, term is 10 * 16 *
			 * 300 / 100 = 480
			 */
			forcex += (10 * centerpull * (long) dx) / freq;
			forcey += (10 * centerpull * (long) dy) / freq;
			speedx += forcex;
			speedy += forcey;
		}
		/* end if (!rkflag) */
		else
		{
			fdr = rkmove(ballx, bally);
			fspeedx = fdr.x * freq;
			fspeedy = fdr.y * freq;
			deltaxdebt += fdr.x;
			deltaydebt += fdr.y;
		}		/* end rkflag case */
	}

	if (frictiontype)
	{
		if (!rkflag)
		{
			if (frictiontype == 1)
			{	/* linear friction */
				frictiondebt = friction / freq;
				frictiondebtx += abs(speedx) * frictiondebt;
				frictiondebty += abs(speedy) * frictiondebt;
			}
			else
			{	/* square friction. Divide by extra 100 to
				 * control. */
				speed = (long) distance(speedx, speedy) / 100;
				frictiondebt = speed * friction;
				frictiondebtx += (speedx / freq) * frictiondebt;
				frictiondebty += (speedy / freq) * frictiondebt;
			}
			if (frictiondebtx >= 1)
			{
				speedx = sign(speedx) * (abs(speedx) - floor(frictiondebtx));
				frictiondebtx = frictiondebtx - floor(frictiondebtx);
			}
			if (frictiondebty >= 1)
			{
				speedy = sign(speedy) * (abs(speedy) - floor(frictiondebty));
				frictiondebty = frictiondebty - floor(frictiondebty);
			}
		}
		else
		{
			if (frictiontype == 1)
			{	/* linear friction */
				frictiondebt = friction / freq;
				frictiondebtx += abs(fspeedx) * frictiondebt;
				frictiondebty += abs(fspeedy) * frictiondebt;
			}
			else
			{	/* square friction. Divide by extra 100 to
				 * control. */
				fspeed = sqrt(fspeedx * fspeedx + fspeedy * fspeedy) / 100;
				frictiondebt = fspeed * friction;
				frictiondebtx += (fspeedx / freq) * frictiondebt;
				frictiondebty += (fspeedy / freq) * frictiondebt;
			}
			if (frictiondebtx >= 1)
			{
				fspeedx = sign(fspeedx) * (abs(fspeedx) - floor(frictiondebtx));
				frictiondebtx = frictiondebtx - floor(frictiondebtx);
			}
			if (frictiondebty >= 1)
			{
				fspeedy = sign(fspeedy) * (abs(fspeedy) - floor(frictiondebty));
				frictiondebty = frictiondebty - floor(frictiondebty);
			}
		}
	}
	/*
	 * this next move allows increased accuracy but ruins reversibility
	 */
	if (!reversibleflag)
	{
		if (!rkflag)
		{
			deltaxdebt += (double) (speedx) / freq;
			deltaydebt += (double) (speedy) / freq;
		}
		if (deltaxdebt >= 0)
			rounddown = floor(deltaxdebt);
		else
			rounddown = ceil(deltaxdebt);
		deltax = (int) rounddown;
		deltaxdebt -= rounddown;
		if (deltaydebt >= 0)
			rounddown = floor(deltaydebt);
		else
			rounddown = ceil(deltaydebt);
		deltay = (int) rounddown;
		deltaydebt -= rounddown;
	}
	else
	{
		deltax = speedx / freq;
		deltay = speedy / freq;
	}

	/*
	 * don't go any further than out to edge of screen, more or less
	 */
	if (abs(deltax) > halfx)
		deltax = sign(deltax) * halfx;
	if (abs(deltay) > halfy)
		deltay = sign(deltay) * halfy;

}
#pragma warn .sig

void resetLength(void)
{
   extern void ResetLayout(void);
	/* Erase all the phantom balls and lines */

   inptr = outptr = erasing = 0;
	/* and a Length Reset always calls for a full clear. */
	ResetLayout();
}
int donttrace = false;
void moveball()
{
   static int oldballx,oldbally;
	int newdx, newdy, i;
   int captured = 0;
   int captor;

	if (!deltax && !deltay)
		return;

	if (clubflag == 0)
	{

		oldballx = ballx;
		oldbally = bally;
		if (!stopped)
      {
         showball(ballx, bally, maxcolor);
   		showball(ballx += deltax, bally += deltay, maxcolor);
      }
		firsttimeflag = 0;
      erasures[(inptr+511) & 0x1ff] = false;
		return;
	}
   
   erasures[inptr] = true;

	if (tracetype != 2 && tracetype != 4)
		showball(ballx, bally, maxcolor);	/* xor show erases */

	oldballx = ballx;
	oldbally = bally;
	firsttimeflag = 0;
	ballx += deltax;
	bally += deltay;
	if ((ballx > hix) || (ballx < lox))
	{
		speedx *= -0.5;
		speedy *= 0.5;
		ballx = oldballx;
	}
	if ((bally > hiy) || (bally < loy))
	{
		speedy *= -0.5;
		speedx *= 0.5;
		bally = oldbally;
	}
   if (xsection)
   {
      for(i=0;i<M.n;i++)
      {
         if (M.magnet[i].charge > 0)
         {
			   newdx = abs(ballx - M.magnet[i].x);
			   newdy = abs(bally - M.magnet[i].y);
			   if (!firsttimeflag && (newdx < xsection) &&
			      (newdy < xsection))
            {
               captured = 1;
               captor = i;
               break;
            }
         }
      }
   }
	if (captured)
   {
		speedx = 0;
		speedy = 0;
		ballx = M.magnet[captor].x;
		bally = M.magnet[captor].y;
		rollflag = 0;
		firsttimeflag = 1;

		/* if we are making noise... */
		if (soundflag && clubflag != MAGBOX)
		{
			sound(440);
			delay(40);
			sound(22);
			delay(100);
			nosound();
		}
   }

	showball(ballx, bally, maxcolor);

	if (tracetype && !donttrace)
	{
		/* don't trace if box around ball */
		jxpixel(ballx, bally, GREEN);	/* because line ends get writ
						 * 2ce */
		jxbresen(oldballx, oldbally, ballx, bally, GREEN);
	}
   donttrace = false;

   if (tracetype == 3 || tracetype == 4)
   {
      if (erasing)
      {
         int n1 = outptr;
         int n2 = (outptr + 1) & 0x1ff;

         if (erasures[n1])
         {
       	  	jxpixel(old_balls[n2].X, old_balls[n2].Y, GREEN);
            jxbresen(old_balls[n1].X,old_balls[n1].Y,
              old_balls[n2].X,old_balls[n2].Y,GREEN);

         }

         if (tracetype == 4)
      		showball(old_balls[n1].X,old_balls[n1].Y,maxcolor);
         outptr = (++outptr) & 0x1ff;
      }
      old_balls[inptr].X = ballx;
      old_balls[inptr].Y = bally;
      inptr = ++inptr & 0x1ff;
      if (inptr == memorylength)
         erasing = true;
   }
}



unsigned char trackball()
{
	int oldballx, oldbally;
	int newdx, newdy, i;


   if (!deltax && !deltay)
      return 0;


	if (!firsttimeflag)
		showball(ballx, bally, maxcolor);	/* xor show erases */
	firsttimeflag = 0;


	oldballx = ballx;
	oldbally = bally;
	ballx += deltax;
	bally += deltay;
	if ((ballx > hix) || (ballx < lox))
	{
		speedx *= -0.5;
		speedy *= 0.5;
		ballx = oldballx;
	}
	if ((bally > hiy) || (bally < loy))
	{
		speedy *= -0.5;
		speedx *= 0.5;
		bally = oldbally;
	}
	showball(ballx, bally, maxcolor);
	for (i = 0; i < M.n; i++)
	{
      if (M.magnet[i].charge > 0)
      {
		   newdx = abs(ballx - M.magnet[i].x);
		   newdy = abs(bally - M.magnet[i].y);
		   if ((newdx < xsection) && (newdy < xsection))
		   {
			   showball(ballx, bally, maxcolor);	/* xor show erases */
			   frictiondebtx = 0;
			   frictiondebty = 0;
			   if (!reversibleflag)
			   {
				   deltaxdebt = 0;
				   deltaydebt = 0;
			   }
			   firsttimeflag = 1;
			   return locked ? available_colors[(i % 10)] : ((i%15)+1);
		   }
      }
	}

	return 0;
}
fpair rkmove(int x, int y)
{
	/* see Numerical Recipes in C p. 572 */

	fpair newdr, temp1, temp2, temp3, derivs1, derivs2, derivs3, derivs4;
	double freq2, freq6;


	freq2 = freq + freq;
	freq6 = freq2 + freq2 + freq2;
	temp1.x = x + deltaxdebt;
	temp1.y = y + deltaydebt;
	derivs1 = derivs(&temp1);
	temp1.x = x + derivs1.x / freq2;
	temp1.y = y + derivs1.y / freq2;
	derivs2 = derivs(&temp1);
	temp2.x = x + derivs2.x / freq2;
	temp2.y = y + derivs2.y / freq2;
	derivs3 = derivs(&temp2);
	temp3.x = x + derivs3.x / freq;
	temp3.y = y + derivs3.y / freq;
	derivs4 = derivs(&temp3);
	newdr.x = (derivs1.x + 2.0 * (derivs2.x + derivs3.x) + derivs4.x) /
		freq6;
	newdr.y = (derivs1.y + 2.0 * (derivs2.y + derivs3.y) + derivs4.y) /
		freq6;
	return newdr;
}


fpair derivs(fpair * w)
{
	fpair temp;
	int dx, dy, i;
	double fdx, fdy, r, rdiv, force, forcex, forcey;

	forcex = 0;
	forcey = 0;
	switch (forcetype)
	{
	case 0:		/* inverse square force */
		for (i = 0; i < M.n; i++)
		{
			dx = M.magnet[i].x - w->x;
			dy = M.magnet[i].y - w->y;
			fdx = dx;
			fdy = dy;
			r = sqrt(fdx * fdx + fdy * fdy);
			if (r > magnetradius)
			{
				rdiv = 50000000.0 / (r * r * r);
				/*
				 * if freq 100, charge 10, and dx is 300,
				 * term is about (20,000,000 / 27,000,000 ) *
				 * (10 * 300 / 100) about 30. with no net
				 * deltax Keep in mind that long ints range
				 * from - 2 billion to +2 billion. If dx is
				 * 20, you get (50,000,000 / 8000 ) * (10 *
				 * 20 / 100), round 8000 to 10,000 and get
				 * 10,000. with deltax = speedx/freq = 100,
				 * which is fine.
				 */
				force = rdiv * M.magnet[i].charge;
				forcex += force * dx;
				forcey += force * dy;
			}
			else
			{
				/*
				 * if magrad is 20, and dx is 20, term is (
				 * 20 * 10 * (50,000,000/( 100 * 8,000) ),
				 * round 8,000 to 10,000, and get 10,000,
				 * matching the value from the other method.
				 */
				rdiv = 50000000.0 / (magrad3);
				force = rdiv * M.magnet[i].charge;
				forcex += dx * force;
				forcey += dy * force;
			}
		}
		break;
	case 1:		/* inverse linear force */
		for (i = 0; i < M.n; i++)
		{
			dx = M.magnet[i].x - w->x;
			dy = M.magnet[i].y - w->y;
			rdiv = (double) (dx * dx + dy * dy);
			if (rdiv > magrad2)
			{
				/*
				 * with the assumptions as before, and dx
				 * 300, this term is (400,000/ 100) * 10 *
				 * 300) / 90,000 ) = about 120. If dx = 20
				 * this term is (400,000/100) * 10 * 20 / 400
				 * = 2,000, which is a deltax of 20.
				 */
				force = 400000.0 * M.magnet[i].charge;
				forcex += (force * dx) / rdiv;
				forcey += (force * dy) / rdiv;
			}
			else
			{
				/** if magrad is 20, and dx is 20, term is
				  20 * 10 * (8,000,000/( 100 * 8,000) ) = 2000 to match.*/
				rdiv = 8000000.0 / magrad3;
				force = rdiv * M.magnet[i].charge;
				forcex += dx * force;
				forcey += dy * force;
			}
		}
		break;
	}			/* end switch */

	dx = centerx - w->x;
	dy = centery - w->y;
	forcex += 10 * centerpull * dx;
	forcey += 10 * centerpull * dy;

	temp.x = fspeedx + forcex / freq;
	temp.y = fspeedy + forcey / freq;
	return temp;
}

