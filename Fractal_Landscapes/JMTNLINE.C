/*
 * Fractal mountain cloud and landscape generator
 * 
 * Designed and implemented in November of 1989 by John Walker
 * 
 */

#include "forge.h"

#include <stdio.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <ctype.h>




/* Data types	 */

#undef False
#undef True

typedef enum
{False = 0, True = 1} Boolean;

#define V     (void)

/* Globals imported  */

extern unsigned _stklen = 8192 * 4;	/* Need larger stack for
					 * interpolation */
extern long time();		/* Unix-like time function */
int xdots, ydots;

/* Local variables  */

extern double arand, gaussadd, gaussfac;	/* Gaussian random parameters */
extern unsigned rseed;		/* Current random seed */
int seeded = false;		/* Initial seed computed ? */
//static int *parr = NULL;	/* Point array vector */
static int parrn = 0;		/* Point array length */
double fracdim = 2.6;		/* Fractal dimension */
double power = 1.2;		/* Power law scaling exponent */
double elevfac = 1.0;		/* Elevation factor */


/*
 * FOUR1  --  One dimensional fast Fourier transform.
 * 
 */

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr

static void four1(double *data, int nn, int isign)
{
	int n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	n = nn << 1;
	j = 1;
	for (i = 1; i < n; i += 2)
	{
		if (j > i)
		{
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = n >> 1;
		while (m >= 2 && j > m)
		{
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax)
	{
		istep = 2 * mmax;
		theta = 6.28318530717959 / (isign * mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2)
		{
			for (i = m; i <= n; i += istep)
			{
				j = i + mmax;
				tempr = wr * data[j] - wi * data[j + 1];
				tempi = wr * data[j + 1] + wi * data[j];
				data[j] = data[i] - tempr;
				data[j + 1] = data[i + 1] - tempi;
				data[i] += tempr;
				data[i + 1] += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
}

#undef SWAP

#define drawpoly(n,p) PolyLine((n),(point *)(p));

/*
 * SPECTRAL1D	--  One dimensional fractal profile synthesis by spectral
 * synthesis from Brownian motion.
 */

static void spectral1d(double **x, int n, double h)
{
	int i, j = 1;
	double beta = 2 * h + 1, rad, phase;
	double *a;
   int t = termsizes[terms]/2;

	/* We always reseed here */

   safe_alloc = true;
	*x = a = (double *) calloc((n * 2 + 20), sizeof(double));
	if (a == NULL)
		return;
	for (i = 0; i < (n / 2); i++)
	{
      double ggg;
      
		phase = 2 * M_PI * (rand() / arand);
      ggg = gauss();
      if (i < t)
      {
   		rad = pow((double) (i + 1), -beta / 2) * ggg;
   		a[j++] = rad * cos(phase);
	   	a[j++] = rad * sin(phase);
      }
	}
	four1(a, n, -1);
}

/* UPARAM  --	Update generation parameters.  */

/* rather than allocing, just keep 'em around. */

static int parr[200*2];
static double aa[200*2];
static double ymin,ymax;
static int parr_active = 0;
void clear_uparams(void)
{
   parr_active = 0;
}
void uparam(int np)
{
	int i;
	double *a, *r;
//   int minuses,pluses;

   ClipRect(&lineRect);

   spectral1d(&a, np, 3.0 - fracdim);
	ymin = 1E50, ymax = -1E50;



#ifdef OLDWAY
   minuses=pluses=0;
   
   /* now see if we need to invert them for fun */
   for(r=a+1,i=0;i<np;i++,r += 2)
   {
      double cr = *r;
      if (cr < 0)
         minuses++;
      else
         pluses++;
   }

   if (minuses > pluses)
   {
#endif
      for(r=a+1,i=0;i<np;i++,r+=2)
         *r = -*r;
//   }


	r = a + 1;

	for (i = 0; i < np; i++)
	{
		double cr = *r;

		if ((power != 1.0) && (cr > 0.0))
		{
			*r = cr = pow(cr, power);
		}

		ymin = min(ymin, cr);
		ymax = max(ymax, cr);
		r += 2;
	}


   memcpy(aa,a,sizeof aa);

	RasterOp(zREPz);

	if (parr_active)
	{
		PenColor(8);
		drawpoly(parrn, parr);
	}
   parr_active = 1;
	parrn = np;
	r = a + 1;


	for (i = 0; i < np; i++)
	{
		double cr = *r;
#pragma warn -sig
		parr[i * 2] = lineRect.Xmin + 1 + (((long)i) * (xdots - 3)) / (np - 1);
#pragma warn .sig
		cr = cr < 0 ? 0 : ((ydots / 2) - 3) * (cr / ymax);
		cr *= elevfac;

		parr[i * 2 + 1] = (ydots  - 4) - cr;
		r += 2;
	}
	PenColor(GREEN);
	drawpoly(parrn, parr);

	free(a);
   ClipRect(&sR);
}

/* Don't worry about unused things now. */
