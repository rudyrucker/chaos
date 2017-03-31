/*
 * Fractal mountain cloud and landscape generator
 * 
 * Designed and implemented in November of 1989 by John Walker
 * 
 * This program was developed using Borland Turbo C 2.0  for  the IBM  PC  and
 * compatibles.  It does not use any of the Borland BGI graphics facilities,
 * but it does rely heavily on the other Borland DOS support libraries.
 * 
 * References cited in the comments are:
 * 
 * Foley, J. D., and Van Dam, A., Fundamentals of Interactive Computer
 * Graphics,  Reading,  Massachusetts:  Addison Wesley, 1984.
 * 
 * Peitgen, H.-O., and Saupe, D. eds., The Science Of Fractal Images, New York:
 * Springer Verlag, 1988.
 * 
 * Press, W. H., Flannery, B. P., Teukolsky, S. A., Vetterling, W. T., Numerical
 * Recipes In C, New Rochelle: Cambridge University Press, 1988.
 * 
 * 
 */
/*
 * REVISION NOTES: Starting Week of Rest, 1989: Severe twiddles and changes
 * done by Josh Gordon, to make it work nicely with Metawindows.
 * 
 * This program now depends on MetaWINDOW by Metagraphics Software
 * 
 * 
 * WARNING. I've set #pragma warn -sig, since we were getting lots of sig
 * warnings. Shouldn't cause problems. Might.
 * 
 */
#include "forge.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#define EXPORT
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <io.h>

#include "sglib.h"
int HiResMode = 1;
int DoEgaMode = 1;
unsigned char clut[256][3];	/* Colour lookup table */
short ditherm[DITHERN][DITHERN] = {
#if DITHERN == 2
	{0, 2},
	{3, 1}
#elif DITHERN == 4
	{0, 8, 2, 10},
	{12, 4, 14, 6},
	{3, 11, 1, 9},
	{15, 7, 13, 5}
#elif DITHERN == 8
	{0, 32, 8, 40, 2, 34, 10, 42},
	{48, 16, 56, 24, 50, 18, 58, 26},
	{12, 44, 4, 36, 14, 46, 6, 38},
	{60, 28, 52, 20, 62, 30, 54, 22},
	{3, 35, 11, 43, 1, 33, 9, 41},
	{51, 19, 59, 27, 49, 17, 57, 25},
	{15, 47, 7, 39, 13, 45, 5, 37},
	{63, 31, 55, 23, 61, 29, 53, 21}
#endif
};
short colmap[8] = {0, 1, 3, 2, 5, 6, 4, 7};
static double dgamma = SCRGAMMA;/* Display gamma */



#define Cast(low, high) ((low) + ((high) - (low)) * (rand() / arand))
int ditherdiv[256];	/* Dither divide table */
int dithermod[256];	/* Dither modulus table */
int dithersq[16][16];	/* Dither intensity table */

double PlanetAmbient = 0.0;
int DoIce = 1;
int DoRotation = 0;
int DoSnapshot = 0;
int DoClouds=0;

int DitherRotation = 0;
int DoDither = 1;
int DoWholeScreen = 0;

int TermsToUse = 8;



typedef struct {
   int XX;
   int YY;
} ppoint;

int pcompare(const void *z1,const void *z2)
{
   int X1 = ((ppoint *)z1)->XX;
   int Y1 = ((ppoint *)z1)->YY;
   int X2 = ((ppoint *)z2)->XX;
   int Y2 = ((ppoint *)z2)->YY;

   if (X1-X2)
      return X1-X2;
   else
      return Y1-Y2;
}


unsigned char rowbuffer[640];

unsigned char planet_clut[256][3];	/* Colour lookup table */

double dPlanetStart = 0.0;


void genplanet(float *a, int n, double rmax,
   double rmin, double shang, double siang,double elevfac)
{
	int i, j;
	unsigned char *screen;
	unsigned char *cp, *ap;
	double *u, *u1;
	unsigned char *bxf, *bxc;
	int ScrX, ScrY;
#define 	 CloudPower  (elevfac)
   int sealevel;
	int rgbmap[256][3];

#define Atthick 1.03		/* Atmosphere thickness as a percentage of
				 * planet's diameter */
	double athfac = sqrt(Atthick * Atthick - 1.0);
	point3d sunvec;

   int ir, ig, ib, idith;
   int landrange;
   float *za = a + 1;
   char *ppp;

   sealevel = (2.0-elevfac) * 128.0;
   landrange = 255 - sealevel;
   landrange = max(landrange,2);
   sealevel = max(sealevel,2);


   if (DoDither || RenderingMode == 0)
   {
		makedtab(6, dgamma, rgbmap, ditherdiv, dithermod, dithersq);
		for (i = 0; i < 255; i++)
		{
			int ir = (rgbmap[i][0] * SCRSAT) / 255;
         int ig = (rgbmap[i][1] * SCRSAT) / 255;
         int ib = (rgbmap[i][2] * SCRSAT) / 255;

			Spal(i, ir, ig, ib);
		}
		for (i = 0; i <= 5; i++)
		{
			Spal(250 + i, (SCRSAT * i) / 5,
				   (SCRSAT * i) / 5, (SCRSAT * i) / 5);
		}
      if (RenderingMode == 1)
      {
         union REGS regs;
         struct SREGS sregs;


         for(i=0;i<256;i++)
          for(j=0;j<3;j++)
               planet_clut[i][j] = clut[i][j] /*>> 2*/;


         regs.h.ah = 0x10;
         regs.h.al = 0x12;
         regs.x.bx = 0;
         regs.x.cx = 256;
         regs.x.dx = FP_OFF(planet_clut);
         sregs.es = FP_SEG(planet_clut);
         int86x(0x10,&regs,&regs,&sregs);
      }
   }
   ppp = getenv("PLANET_COLORS");
   if (ppp && !access(ppp,0))
   {
      FILE *fd = fopen(ppp,"rb");
      int i,j;

      fread(clut,256,3,fd);
      /* stretch the palette now */
      for(i=0;i<256;i++)
         for(j=0;j<3;j++)
            clut[i][j] *= 255.0/63.0;

      fclose(fd);
   }
   else for(i=0;i<256;i++)
   {

      if (i >= sealevel)
      {
         int landval = i - sealevel;
         
         /* Quickly taper down the blue and red for 1/8th the range. */
         if (landval < landrange / 8)
         {
            ib = 255 - ((float)landval)/(landrange/8.0)*255.0;
            ig = 255 - ((float)landval)/(landrange/8.0)*128.0;
            ir = 255 - ((float)landval)/(landrange/8.0)*192.0;
         }
         else if (landval < landrange/2)
         {
            ib = 0;
            ig = 128;
            ir = (landval)/((float)landrange/2) * 255.0;
         }
         else
         {
            ib = 0;
            ig = 128 - (landval-landrange/2)/(float)(landrange/2) * (128.0 - 78.0);
            ir = 255 - (landval-landrange/2)/(float)(landrange/2) * (255.0 - 129.0);
         }

		}
		else
		{

			/*
				* Water.  Generate clouds above water based
				* on elevation.

            * Let's use the elevfac for this! */

         /* Just for a moment, lets just use water depth, see
            what it does. */
         ir = ig = 0;
         if (sealevel == 0.0)
            ib = 255;
         else
            ib = 64 + (i/(float)(sealevel))*(255.0-64.0);
         /* make half of the water cloudy */
         if (i > sealevel/2)
         {
            ig = ir = (float)((i-sealevel/2))/(float)((sealevel/2)) * 255;
            /* Drop down the blue as we approach sealevel */
         }

		}
      clut[i][0] = ir;
      clut[i][1] = ig;
      clut[i][2] = ib;
   }
   if (RenderingMode == 1)
   {
      if (DoDither)
      {
      }
      else
      {
         union REGS regs;
         struct SREGS sregs;

         for(i=0;i<256;i++)
            for(j=0;j<3;j++)
               planet_clut[i][j] = clut[i][j] >> 2;

         regs.h.ah = 0x10;
         regs.h.al = 0x12;
         regs.x.bx = 1;
         regs.x.cx = 255;
         regs.x.dx = FP_OFF(planet_clut);
         sregs.es = FP_SEG(planet_clut);
         int86x(0x10,&regs,&regs,&sregs);
      }
   }


	sunvec[X] = sin(shang) * cos(siang);
//	sunvec[Y] = sin(siang);
	sunvec[Y] = sin(shang) * sin(siang);
	sunvec[Z] = cos(shang) * cos(siang);


   ScrX = planetRect.Xmax - planetRect.Xmin + 1;
   ScrY = planetRect.Ymax - planetRect.Ymin + 1;
   if (planetRect.Xmin != 0)
      ScrX -= 2;
   if (planetRect.Ymin != 0)
      ScrY -= 2;

   /* Unsafe mallocs, but tested for ahead of time */
   u = (double *) malloc(ScrX * sizeof(double));
	u1 = (double *) malloc(ScrX * sizeof(double));
	bxf = (unsigned char *) malloc(ScrX);
	bxc = (unsigned char *) malloc(ScrX);

	screen = (unsigned char *) MK_FP(0xa000, 0);


	/* Prescale the grid points into intensities. */

	cp = (unsigned char *) malloc(n * n);
	if (cp == NULL)
		return;
	ap = cp;
	for (i = 0; i < n; i++)
	{
      int ixn = i * n*2;

		for (j = 0; j < n; j++)
		{
			double r;

         r = za[ixn+j*2];
			*ap++ = 1.0 + (254.0 * (r - rmin)) / (rmax - rmin);
		}
	}

	/*
	 * Fill the screen from the computed intensity grid by mapping screen
	 * points  onto  the grid, then calculating each pixel value by
	 * bilinear interpolation from the  surrounding  grid points.   (N.b.
	 * the pictures would  undoubtedly look better when generated with
	 * small  grids  if  a  more  well-behaved interpolation were used.)
	 * 
	 * Before   we	 get   started,   precompute   the  line-level
	 * interpolation parameters and store them in an array	so  we don't
	 * have to do this every time around the inner loop.
	 */


#ifdef OLDUPRJ
#define UPRJ(a,size) (0.5 + sin(((a)/((size)-1.0))*M_PI + ((3*M_PI)/2))/2)
#else
#define UPRJ(a,size) ((a)/((size)-1.0))
#endif

	for (j = 0; j < ScrX; j++)
	{

		double bx;

      if (DoRotation)
      {
//			bx = dPlanetStart * (n - 1) + (n) * UPRJ(j, ScrX) / 2.0;

         bx = dPlanetStart*(n-1) + n * acos((j-ScrX/2)/(ScrX-1.0))/M_PI;
			if (bx > n)
				bx -= n;
      }
      else if (!DoWholeScreen)
			bx = (n) * UPRJ(j, ScrX * aspect);
      else
//			bx = (n) * UPRJ(j, ScrX);
   	   bx = (n - 1) * (j / (double)ScrX);


		bxf[j] = floor(bx);
      if (bxf[j] > n-1)
         bxf[j] = 0;

		bxc[j] = bxf[j] + 1;
      if (bxc[j] > n-1)
         bxc[j] = 0;

		u[j] = bx - bxf[j];
		u1[j] = 1 - u[j];
	}


	for (i = 0; i < ScrY && !aborted; i++)
	{
		double t, t1;
      double by = (n - 1) * UPRJ(i, ScrY);
      double dy = 2 * (((ScrY / 2) - i) / ((double) ScrY));
      double dysq = dy * dy;
      double sqomdysq = sqrt(1.0 - dysq);
      double icet;
#ifdef FASTDOT	 
      double svx = sunvec[X],
         svy = sunvec[Y] * dy,
         svz = sunvec[Z] * sqomdysq;
#endif		 
      double azimuth;
		int byf, byc, lcos;
      int minj,maxj;
      /* Check to see if we need to quit. */
//      if (EscapePending())
//         break;


      memset(rowbuffer,0,sizeof rowbuffer);

		icet = 15 * (fabs((i / ((double) (ScrY - 1))) - 0.5) - 0.4);
		azimuth = asin(((((double) i) / (ScrY - 1)) * 2) - 1);
		lcos = (ScrY / 2) * fabs(cos(azimuth)) * aspect;
		byf = floor(by);
      byc = byf + 1;
      byf *= n;
      byc *= n;

		t = by - floor(by);
		t1 = 1 - t;
      minj = (DoWholeScreen) ? 0 : (ScrX/2-lcos);
      maxj = (DoWholeScreen) ? ScrX : ((ScrX/2+lcos)+1);
      maxj = min(maxj,ScrX);
      minj = max(minj,0);


		for (j = minj; j < maxj; j++)
		{
			double r = t1 * u1[j] * cp[byf + bxf[j]] +
	   		t * u1[j] * cp[byc + bxf[j]] +
   			t * u[j] * cp[byc + bxc[j]] +
   			t1 * u[j] * cp[byf + bxc[j]];
         double ice;

         ir = clut[r][0];
         ig = clut[r][1];
         ib = clut[r][2];

			/* Generate polar ice caps. */


#define IceLevel 0.866		/* sqrt(0.75) */
			if (DoIce)
			{
            if (DoIce == 1)
               ice = 1.0 - max(0.0, (icet + ((r - sealevel) / (float)(sealevel))));
            else
               ice = 1.0 - max(0.0, (icet + ((r - sealevel) / (float)(255-sealevel))));
				if (ice < IceLevel)
					ir = ig = ib = 255;
			}

			/* Apply limb darkening by cosine rule. */
         if (!DoWholeScreen)
			{
				double dx = 2 * (((ScrX / 2) - j) / (ScrY * aspect)), dxsq = dx * dx, ds, di, inx;
				double dsq, dsat;

				di = svx * dx + svy + svz * sqrt(1.0 - dxsq);

				if (di < 0)
            {
               if (!(DoDither || RenderingMode == 0))
                  r = 0;
					continue;
            }

				
            if (DoDither || RenderingMode == 0)
            {
               di = min(1.0, di);
            

				   ds = sqrt(dxsq + dysq);
				   ds = min(1.0, ds);



				   /*
				   * Calculate atmospheric absorption based on
				   * the thickness of atmosphere traversed by
				   * light on its way to the surface.
				   */

#define 	 AtSatFac 1.0
				   dsq = ds * ds;
				   dsat = AtSatFac * ((sqrt(Atthick * Atthick - dsq) -
					      sqrt(1.0 * 1.0 - dsq)) / athfac);
#define 	 AtSat(x,y) x = ((x)*(1.0-dsat))+(y)*dsat
				   AtSat(ir, 127);
				   AtSat(ig, 127);
				   AtSat(ib, 255);


				   inx = PlanetAmbient + (1.0 - PlanetAmbient) * di;
				   ir *= inx;
				   ig *= inx;
				   ib *= inx;
            }
			}

#define Dither(v) (dithermod[v] > idith ? ditherdiv[v] + 1 : ditherdiv[v])
         
         if (RenderingMode == 1)
			{
            if (DoDither || RenderingMode == 0)
            {
   				idith = dithersq[j & 15][i & 15];
	   			rowbuffer[j] = Dither(ir) + Dither(ig) * 6 + Dither(ib) * 6 * 6;
            }
            else
               rowbuffer[j] = r;
			}
			else
			{
            char z;

            if (DoDither || RenderingMode == 0)
            {
   				ir = ((NCOLOURS - 1) * ir) / 255;
   				ig = ((NCOLOURS - 1) * ig) / 255;
   				ib = ((NCOLOURS - 1) * ib) / 255;
   				idith = ditherm[j & (DITHERN - 1)][i & (DITHERN - 1)];
   				z = colmap[((ir > idith) ? 1 : 0) |
   						((ig > idith) ? 2 : 0) |
   						((ib > idith) ? 4 : 0)];

            }
            else if (RenderingMode ==1)
               z = r;
            else
               z = ((int)r) >> 4;

            rowbuffer[j] = z;
			}

		}

      if (!DoWholeScreen && (DoDither || RenderingMode == 0))
      {
            /* Left stars */

#define StarFraction   3
#define StarClose      10

		   for (j = 0; j < (ScrX / 2) - (lcos + StarClose); j++)
		   {
			   if (random(1000) < StarFraction)
            {
               if (RenderingMode == 1)
					   rowbuffer[j] = 250 + (rand() % 6);
				   else
                  rowbuffer[j] = 7;
            }
		   }

		   /* Right stars */

		   for (j = (ScrX / 2) + (lcos + StarClose); j < ScrX; j++)
		   {
			   if (random(1000) < StarFraction)
            {
				   if (RenderingMode == 1)
					   rowbuffer[j] = 250 + (rand() % 6);
				   else
                  rowbuffer[j] = 7;
            }
		   }
	   }

      if (RenderingMode == 0)
         zap_in_row(rowbuffer,planetRect.Xmin + 1,planetRect.Ymin + i+1,ScrX);
      else
         memcpy(screen+(planetRect.Ymin + i) * 320 + planetRect.Xmin,
                rowbuffer,
                ScrX);

      AbortCheck();                
   }

	free((char *) cp);
	free(u);
	free(u1);
	free(bxf);
	free(bxc);

}

void initplanetcolors(void)
{
   if (RenderingMode == 1)
      aspect = .75 * 320.0 / 200.0;
}





