#include "forge.h"
#include <math.h>
#include <alloc.h>
static int meshsize;
int whichpic;
unsigned long tickertape[100];


unsigned long tickersizes[] =
{
	0x20 + 0x8,
	0xc0 + 0x20,
	0x400 + 0xc0,
	0x1400 + 0x300,
	0x6000 + 0xe00,
	0x1f800L,
};


#pragma warn -sig
void Fourn(float *data, int nn[], int ndim, int isign)
{
	long i1, i2, i3;
	long ip1, ip2;
	long i2rev, i3rev, ip3, ifp1, ifp2;
	long k1, k2;
	long ibit, n;
	long idim;
	unsigned ntot, nprev, nrem;
	float tempi, tempr;
	double theta, wi, wpi, wpr, wr, wtemp;
	unsigned long ticker = 0;
	unsigned percent;
	unsigned long totalticks;
	unsigned long nexttick;


   float t;

	/*
	 * Zero out everything but the first TermsToUse squared items in the
	 * array.
	 */




	ntot = 1;
	for (idim = 1; idim <= ndim; idim++)
		ntot *= nn[idim];
	nprev = 1;
	switch (nn[1])
	{
	case 4:
		totalticks = tickersizes[0] - 1;
		break;
	case 8:
		totalticks = tickersizes[1] - 1;
		break;
	case 16:
		totalticks = tickersizes[2] - 1;
		break;
	case 32:
		totalticks = tickersizes[3] - 1;
		break;
	case 64:
		totalticks = tickersizes[4] - 1;
		break;
	case 128:
		totalticks = tickersizes[5] - 1;
		break;
	}

	InitTicker("Fourier Transform", totalticks);
	nexttick = tickertape[percent = 0];

	for (idim = ndim; idim >= 1; idim--)
	{
		n = nn[idim];
		nrem = ntot / (n * nprev);
		ip1 = nprev << 1;
		ip2 = ip1 * n;
		ip3 = ip2 * nrem;
		i2rev = 1;
		for (i2 = 1; i2 <= ip2; i2 += ip1)
		{
			if (i2 < i2rev)
			{
				for (i1 = i2; i1 <= i2 + ip1 - 2; i1 += 2)
				{
					for (i3 = i1; i3 <= ip3; i3 += ip2)
					{
						if (++ticker > nexttick)
						{
							tick(++percent);
							nexttick = tickertape[percent];
                     AbortCheck();
							if (aborted)
								return;
						}

						i3rev = i2rev + i3 - i2;
                  t = data[i3];
                  data[i3] = data[i3rev];
                  data[i3rev] = t;

                  t = data[i3+1];
                  data[i3+1] = data[i3rev+1];
                  data[i3rev+1] = t;
					}
				}
			}
			ibit = ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit)
			{
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1 = ip1;
		while (ifp1 < ip2)
		{
			ifp2 = ifp1 << 1;
			theta = isign * 6.28318530717959 / (ifp2 / ip1);
			wtemp = sin(0.5 * theta);
			wpr = -2.0 * wtemp * wtemp;
			wpi = sin(theta);
			wr = 1.0;
			wi = 0.0;
			for (i3 = 1; i3 <= ifp1; i3 += ip1)
			{
				for (i1 = i3; i1 <= i3 + ip1 - 2; i1 += 2)
				{
					for (i2 = i1; i2 <= ip3; i2 += ifp2)
					{
						k1 = i2;
						k2 = k1 + ifp1;
						tempr = wr * data[k2] - wi * data[k2+1];
						tempi = wr * data[k2+1] + wi * data[k2];

                  data[k2] = data[k1] - tempr;
                  data[k2+1] = data[k1+1] - tempi;

                  data[k1] += tempr;
                  data[k1+1] += tempi;


						if (++ticker > nexttick)
						{
							tick(++percent);
							nexttick = tickertape[percent];
                     AbortCheck();
							if (aborted)
								return;
						}

					}
				}
				wr = (wtemp = wr) * wpr - wi * wpi + wr;
				wi = wi * wpr + wtemp * wpi + wi;
			}
			ifp1 = ifp2;
		}
		nprev *= n;
	}





}

#pragma warn .sig

#undef SWAP

/*
 * SPECTRALSYNTH  --  Spectrally synthesised fractal motion in two
 * dimensions.  This algorithm is given under the name SpectralSynthesisFM2D
 * on page 108 of Peitgen & Saupe.
 */




void spectralsynth(float **x, int n, double h)
{
	unsigned long bl;
	int i, j, j0, nsize[3];
	double rad, phase;
	float *a;
	unsigned long totalticks = (n * (long) n) / 2;
	unsigned long ticker = 0;
	unsigned percent;
	float radxcosine, radxsine;
	unsigned long nexttick;
   float exponent = -(h+1)/2;
   int exptype;

   int termstouse = termsizes[terms];
   int start = termstouse/2;
   int stop = n - start;

   if (h == 0.0)
      exptype = 2;
   else if (h == 1.0)
      exptype = 1;
   else
      exptype = 0;

	InitTicker("Spectral synthesis", totalticks);
	nexttick = tickertape[percent = 0];
	meshsize = n;
	if (*x)
		free(*x);
	bl = ((((unsigned long) n) * n) + 1) * 2 * sizeof(float);
   safe_alloc = true;
	a = (float *) farcalloc(bl, 1);
	*x = a;
	if (a == NULL)
	{
      char tbuf[128];
      sprintf(tbuf,"Unable to allocate %ld bytes",bl);
      ErrorBox(tbuf);
		return;
	}
   a++;     /* to account for fourier smudging */

	for (i = 0; i <= n / 2; i++)
	{
      int ixn = i*n*2;
      int i0 = (i == 0) ? 0 : n-i;
      int i0xn = i0*n*2;
      int ixi = i*i;

		for (j = 0; j <= n / 2; j++)
		{

         double ggg;
			phase = 2 * M_PI * (rand() / arand);
         ggg = gauss();

         /* we do these OUTSIDE the loop so not to fuck up the randomizer. */
         if (!((i > start && i < stop) || (j > start && j < stop)))
         {
			   if (i != 0 || j != 0)
			   {
               if (exptype == 1)
                  rad = ggg / (double)(ixi+j*j);
               else if (exptype == 2)
                  rad = ggg / sqrt((double)(ixi+j*j));
               else
   				   rad = pow((double) (ixi + j * j), exponent) * ggg;
				   radxcosine = rad * cos(phase);

 				   radxsine = rad * sin(phase);
			   }
			   else
			   {
				   radxsine = radxcosine = 0.0;
			   }
			   j0 = (j == 0) ? 0 : n - j;

            a[i0xn+j0*2] = a[ixn+j*2+1] = radxcosine;

            a[ixn+j*2+1] = radxsine;
            a[i0xn+j0*2+1] = -radxsine;
         }
			if (++ticker > nexttick)
			{
				tick(++percent);
				nexttick = tickertape[percent];
            AbortCheck();
				if (aborted)
					return;
			}
		}
	}
   a[n*n+1] = 0.0;
   a[n + 1] = 0.0;
   a[n*n + n + 1] = 0.0;

	for (i = 1; i <= n / 2 - 1; i++)
	{
      int ixn = i*n*2;
      int i0 = n-i;
      int i0xn = i0*n*2;
      int ixi = i*i;
		for (j = 1; j <= n / 2 - 1; j++)
		{
         double ggg;
         phase = 2 * M_PI * (rand() / arand);
         ggg = gauss();
         if (!((i > start && i < stop) || (j > start && j < stop)))
         {
            if (exptype == 1)
               rad = ggg / (double)(ixi+j*j);
            else if (exptype == 2)
               rad = ggg / sqrt((double)(ixi+j*j));
            else
   				rad = pow((double) (ixi + j * j), exponent) * ggg;
			   radxcosine = rad * cos(phase);
            a[i0xn+j*2] =  a[ixn+(n-j)*2] = radxcosine;

   			radxsine = rad * sin(phase);
            a[ixn+(n-j)*2+1] = radxsine;
            a[i0xn+j*2+1] = -radxsine;
         }

			if (++ticker > nexttick)
			{
				tick(++percent);
				nexttick = tickertape[percent];
            AbortCheck();
				if (aborted)
					return;
			}
		}
	}


	nsize[0] = 0;
	nsize[1] = nsize[2] = n;/* Dimension of frequency domain array */


	Fourn(a-1, nsize, 2, -1);	/* Take inverse 2D Fourier transform */
}

void initialize_forgery(int who, double fd, double powscale, int n, float **a)
{
	double rmin = 1e50, rmax = -1e50, rmean, rrange;
	int i, j;
	unsigned long totalticks = (n * (long) n);
	unsigned long ticker;
	unsigned percent;
	unsigned long nexttick;
   float *aa;

	whichpic = who;
	meshsize = n;
   
	spectralsynth(a, n, 3.0 - fd);
   aa = *a;
   aa++;

	if (aborted)
		goto beatit;

	if (powscale != 1.0)
	{
		InitTicker("Power scaling", totalticks);
		nexttick = tickertape[percent = 0];
		ticker = 0;
		/* Indicate process here */
		for (i = 0; i < n; i++)
		{
         int ixn = i*n*2;

			for (j = 0; j < n; j++)
			{
				double r = aa[ixn+j*2];

				if (r > 0)
               aa[ixn+j*2] = pow(r,powscale);

				if (++ticker > nexttick)
				{
					tick(++percent);
					nexttick = tickertape[percent];
					aborted = SomethingWaiting();
					if (aborted)
						goto beatit;
				}
			}
		}
	}
	ticker = 0;
	totalticks *= 2;
	InitTicker("Scaling to screen", totalticks);
	nexttick = tickertape[percent = 0];
	for (i = 0; i < n; i++)
	{
      int ixn = i*n*2;
		for (j = 0; j < n; j++)
		{
			double r = aa[ixn+j*2];

			rmin = min(rmin, r);
			rmax = max(rmax, r);
			if (++ticker > nexttick)
			{
				tick(++percent);
				nexttick = tickertape[percent];
				aborted = SomethingWaiting();
				if (aborted)
					goto beatit;
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
         aa[ixn+j*2] = (aa[ixn+j*2]-rmean)/rrange;
			if (++ticker > nexttick)
			{
				tick(++percent);
				nexttick = tickertape[percent];
				aborted = SomethingWaiting();
				if (aborted)
					goto beatit;
			}
		}
	}
beatit:
	CloseTicker();
}
