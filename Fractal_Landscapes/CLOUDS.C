#include "forge.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <dos.h>
#include "sglib.h"
#include <time.h>


static unsigned char cloudpal[256][3] = {
	 0, 0, 0,
	 0, 0,63,
	 1, 1,63,
	 2, 2,63,
	 3, 3,63,
	 4, 4,63,
	 5, 5,63,
	 6, 6,63,
	 7, 7,63,
	 8, 8,63,
	 9, 9,63,
	10,10,63,
	11,11,63,
	12,12,63,
	13,13,63,
	14,14,63,
	15,15,63,
	16,16,63,
	17,17,63,
	18,18,63,
	19,19,63,
	20,20,63,
	21,21,63,
	22,22,63,
	23,23,63,
	24,24,63,
	25,25,63,
	26,26,63,
	27,27,63,
	28,28,63,
	29,29,63,
	30,30,63,
	31,31,63,
	32,32,63,
	33,33,63,
	34,34,63,
	35,35,63,
	36,36,63,
	37,37,63,
	38,38,63,
	39,39,63,
	40,40,63,
	41,41,63,
	42,42,63,
	43,43,63,
	44,44,63,
	45,45,63,
	46,46,63,
	47,47,63,
	48,48,63,
	49,49,63,
	50,50,63,
	51,51,63,
	52,52,63,
	53,53,63,
	54,54,63,
	55,55,63,
	56,56,63,
	57,57,63,
	58,58,63,
	59,59,63,
	60,60,63,
	61,61,63,
	62,62,63,
	63,63,63,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	26,16, 7,
	32,20, 9,
	10,15,45,
	 0,32, 0,
	 0,37, 0,
	 0,42, 0,
	 0,47, 0,
	 0,51, 0,
	 0,55, 0,
	 0,59, 0,
	 0,63, 0,
	 8,32, 0,
	 9,37, 0,
	10,42, 0,
	11,47, 0,
	12,51, 0,
	13,55, 0,
	14,59, 0,
	15,63, 0,
	11,32, 0,
	13,37, 0,
	15,42, 0,
	17,47, 0,
	18,51, 0,
	20,55, 0,
	21,59, 0,
	23,63, 0,
	14,32, 0,
	17,37, 0,
	19,42, 0,
	21,47, 0,
	23,51, 0,
	25,55, 0,
	27,59, 0,
	29,63, 0,
	17,32, 0,
	20,37, 0,
	23,42, 0,
	25,47, 0,
	27,51, 0,
	30,55, 0,
	32,59, 0,
	34,63, 0,
	19,32, 0,
	23,37, 0,
	26,42, 0,
	29,47, 0,
	31,51, 0,
	34,55, 0,
	36,59, 0,
	38,63, 0,
	21,32, 0,
	25,37, 0,
	29,42, 0,
	32,47, 0,
	35,51, 0,
	37,55, 0,
	40,59, 0,
	42,63, 0,
	23,32, 0,
	27,37, 0,
	31,42, 0,
	35,47, 0,
	38,51, 0,
	41,55, 0,
	44,59, 0,
	46,63, 0,
	25,32, 0,
	30,37, 0,
	34,42, 0,
	37,47, 0,
	41,51, 0,
	44,55, 0,
	47,59, 0,
	50,63, 0,
	27,32, 0,
	32,37, 0,
	36,42, 0,
	40,47, 0,
	44,51, 0,
	47,55, 0,
	50,59, 0,
	53,63, 0,
	29,32, 0,
	34,37, 0,
	38,42, 0,
	42,47, 0,
	46,51, 0,
	50,55, 0,
	53,59, 0,
	57,63, 0,
	30,32, 0,
	36,37, 0,
	40,42, 0,
	45,47, 0,
	49,51, 0,
	53,55, 0,
	56,59, 0,
	60,63, 0,
	32,32, 0,
	37,37, 0,
	42,42, 0,
	47,47, 0,
	51,51, 0,
	55,55, 0,
	59,59, 0,
	63,63, 0,
	32,30, 0,
	37,36, 0,
	42,40, 0,
	47,45, 0,
	51,49, 0,
	55,53, 0,
	59,56, 0,
	63,60, 0,
	32,29, 0,
	37,34, 0,
	42,38, 0,
	47,42, 0,
	51,46, 0,
	55,50, 0,
	59,53, 0,
	63,57, 0,
	32,27, 0,
	37,32, 0,
	42,36, 0,
	47,40, 0,
	51,44, 0,
	55,47, 0,
	59,50, 0,
	63,53, 0,
	32,26, 8,
	37,30,10,
	42,35,11,
	47,38,13,
	51,42,14,
	55,45,15,
	59,48,16,
	63,51,17,
	32,27,13,
	37,31,15,
	42,36,17,
	47,39,19,
	51,43,20,
	55,46,22,
	59,50,24,
	63,53,25,
	32,30,28,
	37,36,33,
	42,40,37,
	47,45,41,
	51,49,45,
	55,53,49,
	59,56,52,
	63,60,55,
	32,32,32,
	37,37,37,
	42,42,42,
	47,47,47,
	51,51,51,
	55,55,55,
	59,59,59,
	63,63,63,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0, 0, 0,
	 0,59,59,
	 0,57,59,
	 0,54,59,
	 0,51,58,
	 0,48,58,
	 0,46,57,
	 0,43,57,
	 0,40,57,
	 0,36,56,
	 0,33,56,
	 0,30,55,
	 0,26,55,
	 0,22,55,
	 0,17,54,
	 0,11,54,
	 0, 0,53,
};



void initcloudcolors(void)
{
#ifdef OLDWAY
   int i,j;

	static struct
	{
		int hue;
		int sat;
	}
	 hslist[] =
	{
		{120, 100},
		/* Lowest elevation colour */
		{115, 100},
		{110, 100},
		{105, 100},
		{100, 100},
		{95, 100},
		{90, 100},
		{85, 100},
		{80, 100},
		{75, 100},
		{70, 100},
		{65, 100},
		{60, 100},
		{55, 100},
		{50, 100},
		{45, 100},
		{40, 90},
		{40, 80},
		{35, 20},
		{0, 0}		/* Highest elevation colour */
      };

	Spal(0, 0, 0, 0);
   /*
	   * Generate colours for  clouds.	 These	form
	   * a  ramp  of decreasing  saturation  from blue to
	   * white, as suggested on page 113 of Peitgen &
	   * Saupe.  These  shades  are  not gamma corrected.
	   */


	for (i = 0; i < 64; i++)
	{
		Spal(i + 1, i, i, 63);
	}

	/* Generate colours for terrain. */

	Spal(69, 26, 16, 7);	/* First underground shade */
	Spal(70, 32, 20, 9);	/* Second underground shade */
	Spal(71, 10, 15, 45);	/* Canned water colour */

	/*
		* Elaborate  the  base shades  in the terrain colour
		* table into 8 intensities per basic shade.  The
		* intensities are used to display cosine law shading
		* based on the angle of the  face with regard to the
		* observer.  These shades are gamma corrected for
		* better appearance.
		*/

	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 8; j++)
		{
			unsigned int r, g, b;

			hsv_rgb(100L * hslist[i].hue, hslist[i].sat * 100,
			((int) (10000.0 * ((1.0 - diffuse) +
						diffuse * (j / 7.0)))),
				&r, &g, &b);
			r = 0.5 + SCRSAT * pow(r / 10000.0, 1.0 / dgamma);
			g = 0.5 + SCRSAT * pow(g / 10000.0, 1.0 / dgamma);
			b = 0.5 + SCRSAT * pow(b / 10000.0, 1.0 / dgamma);
			Spal((i * 8) + j + 72, r, g, b);
		}
	}

	/* Generate shades to represent water depth */

	for (i = 0; i < 16; i++)
	{
		unsigned int r, g, b;

		hsv_rgb(24000L - 400 * (15 - i),
			10000, 9000 - 100 * i,
			&r, &g, &b);
		r = 0.5 + SCRSAT * pow(r / 10000.0, 1.0 / dgamma);
		g = 0.5 + SCRSAT * pow(g / 10000.0, 1.0 / dgamma);
		b = 0.5 + SCRSAT * pow(b / 10000.0, 1.0 / dgamma);
		Spal(240 + i, r, g, b);
	}

#else
   FILE *fd = fopen("clouds.col","rb");
   if (fd)
   {
      int i,j;
      fread(clut,256,3,fd);
      fclose(fd);
      for(i=0;i<256;i++)
         for(j=0;j<3;j++)
            clut[i][j] *= 255.0/63.0;
   }
   else
      memcpy(clut,cloudpal,3*256);

#endif

   if (RenderingMode == 1)
   {
      union REGS regs;
      struct SREGS sregs;

      regs.h.ah = 0x10;
      regs.h.al = 0x12;
      regs.x.bx = 0;
      regs.x.cx = 256;
      regs.x.dx = FP_OFF(clut);
      sregs.es = FP_SEG(clut);
      int86x(0x10,&regs,&regs,&sregs);
   }

#ifdef TESTING
   {
   FILE *fd = fopen("cloudx.c","wt");
   int i;
   fprintf(fd,"int cloudpal[256][3] = {\n");
   for(i=0;i<256;i++)
      fprintf(fd,"\t%02d,%02d,%02d,\n",clut[i][0],clut[i][1],clut[i][2]);
   fprintf(fd,"};\n");
   fclose(fd);
}
#endif
}



void genclouds(float *a, int n)
{
   double rmax=1.0;
   double rmin=-1.0;
	int i, j;
	unsigned char *cp, *ap;
   double hv = rmin + (1.0-elevfac/2.0) * (rmax-rmin);
   double rg = rmax - hv;

//	double hv = (rmin + rmax) / 2, rg = rmax - hv;
	int ScrY,ScrX;
	double *u, *u1;
	char *bxf, *bxc;
   int ir, ig, ib, idith;
   unsigned char *screen = MK_FP(0xa000,0);
   unsigned char *rowbuffer=NULL;

   
   if (RenderingMode == 0)
   {
      if (planetRect.Xmin == 0)
      {
         ScrX = sR.Xmax + 1;
         ScrY = sR.Ymax + 1;
         rowbuffer = malloc(640);
      }
      else
      {
         ScrX = planetRect.Xmax - planetRect.Xmin - 1;
         ScrY = planetRect.Ymax - planetRect.Ymin - 1;
      }
   }
   else
   {
      ScrX = 320;
      ScrY = 200;
   }

      

	/* Allocate the us and bxs */
   safe_alloc = true;
	u = (double *) malloc(ScrX * sizeof(double));
	if (!u)
		goto cleanup;

   safe_alloc = true;
	u1 = (double *) malloc(ScrX * sizeof(double));
	if (!u1)
		goto cleanup;

   safe_alloc = true;
	bxf = malloc(ScrX);
	if (!bxf)
		goto cleanup;
   safe_alloc = true;
	bxc = malloc(ScrX);
	if (!bxc)
		goto cleanup;

	/* Prescale the grid points into intensities. */

   safe_alloc = true;
	cp = (unsigned char *) malloc(n * n);
	if (cp == NULL)
		return;
	ap = cp;
	for (i = 0; !aborted && i < n; i++)
	{
      int ixn = i * n * 2;

		for (j = 0; j < n; j++)
		{
			double r;

         r = a[ixn+j*2+1];
			if (r < hv)
				*ap++ = 1;
			else if (rg != 0.0)
				*ap++ = 2 + 62.0 * ((r - hv) / rg);
         else
            *ap++ = 63.0;
		}
      AbortCheck();
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


	for (j = 0; j < ScrX; j++)
	{
		double bx = (n - 1) * (j / (double)ScrX);

		bxf[j] = floor(bx);
		bxc[j] = bxf[j] + 1;
		u[j] = bx - bxf[j];
		u1[j] = 1 - u[j];
	}

	/*
	 * If  there  is  already   a  graphics  image	on the screen,
	 * generate the clouds into a auxiliary screen buffer  (unless we're
	 * unable  to  allocate  one),  then blast copy the new clouds into
	 * the frame buffer.  If we're painting the  first cloud  image,
	 * draw  as we go since there's nothing else to watch anyway.
	 */

	for (i = 0; i < ScrY && !aborted; i++)
	{
		double t, t1, by = (n - 1) * (i / (float)ScrY);
		int byf, byc;

		byf = floor(by) * n;
		byc = byf + n;
		t = by - floor(by);
		t1 = 1 - t;
		for (j = 0; j < ScrX; j++)
		{
			double d1 = t1 * u1[j] * cp[byf + bxf[j]];
			double d2 = t * u1[j] * cp[byc + bxf[j]];
			double d3 = t * u[j] * cp[byc + bxc[j]];
			double d4 = t1 * u[j] * cp[byf + bxc[j]];

         unsigned char r;


			r = (unsigned char) floor(d1 + d2 + d3 + d4 + 0.5);

         if (RenderingMode == 0)
         {
            int col;

            ir = clut[r][0];
            ig = clut[r][1];
            ib = clut[r][2];

   		   ir = ((NCOLOURS - 1) * ir) / 255;
   		   ig = ((NCOLOURS - 1) * ig) / 255;
   		   ib = ((NCOLOURS - 1) * ib) / 255;
   		   idith = ditherm[j & (DITHERN - 1)][i & (DITHERN - 1)];
   		   col = colmap[((ir > idith) ? 1 : 0) |
   				   ((ig > idith) ? 2 : 0) |
   				   ((ib > idith) ? 4 : 0)];

            if (rowbuffer)
               rowbuffer[j] = col;
            else
               SetPixel10(planetRect.Xmin + 1 + j,planetRect.Ymin+1+i,col);
         }
         else
            *screen++ = r;

		}
      AbortCheck();
      if (rowbuffer)
         rowblast(rowbuffer,0,i,640);


	}
cleanup:
	if (cp)
		free((char *) cp);
	if (u)
		free(u);
	if (u1)
		free(u1);
	if (bxf)
		free(bxf);
	if (bxc)
		free(bxc);
   if (rowbuffer)
      free(rowbuffer);

}

