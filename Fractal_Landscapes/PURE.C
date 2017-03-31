#define FASTDOT
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <dos.h>
#include "forge.h"

static double dgamma = SCRGAMMA;/* Display gamma */

int SmoothingPures = 1;
int WrappingPures = 0;

#define Spal(x,ri,gi,bi) clut[x][0] = ri; clut[x][1] = gi; clut[x][2] = bi

short ourpal[16];

int colorcomp(const void *z1,const void *z2)
{
   short *c1 = (short *)z1;
   short *c2 = (short *)z2;

   float l1,l2;
   int r,g,b;

   r = defaultpalette[*c1][0]/63.0;
   g = defaultpalette[*c1][1]/63.0;
   b = defaultpalette[*c1][2]/63.0;


   l1 = .3 * r + .59 * g + .11 * b;

   r = defaultpalette[*c2][0]/63.0;
   g = defaultpalette[*c2][1]/63.0;
   b = defaultpalette[*c2][2]/63.0;

   l2 = .3 * r + .59 * g + .11 * b;

   if (l1 < l2)
      return -1;
   else if (l1 > l2)
      return 1;
   else
      return 0;
}
   





void initpurecolors(void)
{
   int i;
   unsigned ir,ig,ib;

   if (RenderingMode == 1)
   {
      union REGS regs;
      struct SREGS sregs;
      FILE *fd = fopen("contour.col","rb");
      if (fd)
      {
         fread(clut,3,256,fd);
         fclose(fd);
      }
      else
      {
      

         for(i=0;i<256;i++)
         {
            hsv_rgb((i * 24000L) / 255L,10000,10000,&ir,&ig,&ib);
		      ir = 0.5 + SCRSAT * pow(ir / 10000.0, 1.0 / dgamma);
		      ig = 0.5 + SCRSAT * pow(ig / 10000.0, 1.0 / dgamma);
		      ib = 0.5 + SCRSAT * pow(ib / 10000.0, 1.0 / dgamma);
            Spal(i,ir,ig,ib);
         }
      }

      regs.h.ah = 0x10;
      regs.h.al = 0x12;
      regs.x.bx = 1;
      regs.x.cx = 255;
      regs.x.dx = FP_OFF(clut);
      sregs.es = FP_SEG(clut);
      int86x(0x10,&regs,&regs,&sregs);
   }
   else
   {
      /* Sort the 15 colors in order of luminence. */
      memcpy(ourpal,brightpal,sizeof ourpal);
      qsort(ourpal,16,sizeof (short),colorcomp);
   }

}

int disty[640];

void genpures(float *a, int n)
{
	int i, j;
	unsigned char *screen;
	unsigned char *cp=NULL, *ap=NULL;
	int ScrY,ScrX;
	double *u=NULL, *u1=NULL;
	char *bxf=NULL, *bxc=NULL;
   int *xstarts=NULL,*ystarts=NULL;

   float xmin = 1e50;
   float xmax = -1e50;

   int left,top;



   screen = MK_FP(0xa000,0);

   if (RenderingMode == 1)
   {
      ScrX = 320,ScrY = 200;
      left = 0;
      top = 0;
   }

   else
   {
      if (planetRect.Xmin)
      {
         ScrX = planetRect.Xmax - planetRect.Xmin - 1;
         ScrY = planetRect.Ymax - planetRect.Ymin - 1;
         left = planetRect.Xmin + 1;
         top = planetRect.Ymin + 1;

      }
      else
      {
         left = 0;
         top = 0;
         ScrX = sR.Xmax + 1;
         ScrY = sR.Ymax + 1;
      }
   }


	/* Allocate the us and bxs */
   if (SmoothingPures)
   {
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
   }
   else
   {
      safe_alloc = true;
      xstarts = (int *)malloc((n+1)*sizeof(int));
      ystarts = (int *)malloc((n+1)*sizeof(int));

      if (!(xstarts && ystarts))
         goto cleanup;
   }

	/* Prescale the grid points into intensities. */

   safe_alloc = true;
	cp = (unsigned char *) malloc(n * n);
	if (cp == NULL)
		return;
	ap = cp;

   for(i=0;i<n;i++)
   {
      int ixn = i*n*2;

      for(j=0;j<n;j++)
      {
         float beep;

         beep = a[ixn+j*2+1];
         if (beep > xmax)
            xmax = beep;
         if (beep < xmin)
            xmin = beep;
      }
   }
	for (i = 0; i < n; i++)
	{
      int ixn = i*n*2;
		for (j = 0; j < n; j++)
		{
			double r;

         r = a[ixn+j*2+1];

         *ap++ = (r - xmin) / (xmax - xmin) * (RenderingMode ? 254.0 : 14.0);

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


   if (SmoothingPures)
   {
	   for (j = 0; j < ScrX; j++)
	   {
         double bx;

         if (WrappingPures)
   		   bx = (n) * (j / (double)ScrX);
         else
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
         double by;

		   int byf, byc;
         memset(rowbuffer,0,sizeof rowbuffer);

         if (WrappingPures)
            by = (n) * (i / (float)ScrY);
         else
            by = (n - 1) * (i / (float)ScrY);

		   byf = floor(by);

         if (byf > n-1)
            byf = 0;
         byc = byf + 1;
         if (byc > n-1)
            byc = 0;

         byf = byf * n;
		   byc = byc * n;

		   t = by - floor(by);
		   t1 = 1 - t;

		   for (j = 0; j < ScrX; j++)
		   {


            unsigned char r;

			   double d1 = t1 * u1[j] * cp[byf + bxf[j]];
			   double d2 = t * u1[j] * cp[byc + bxf[j]];
			   double d3 = t * u[j] * cp[byc + bxc[j]];
			   double d4 = t1 * u[j] * cp[byf + bxc[j]];
   			r = (unsigned char) floor(d1 + d2 + d3 + d4 + 0.5);

            if (RenderingMode == 0)
               rowbuffer[j] = ourpal[r+1];
            else
               rowbuffer[j] = r+1;
		   }
         if (RenderingMode == 0)
            zap_in_row(rowbuffer,left,top+i,ScrX);
         else
            memcpy(screen+i * 320,rowbuffer,ScrX);
         AbortCheck();
	   }
   }
   else
   {
      int i1,j1;
      if (RenderingMode == 0)
      {
         i1 = top;
         j1 = left;
      }
      else
      {
         i1 = j1 = 0;
      }
      for(i=0;i<=n;i++)
      {
         xstarts[i] = i*(ScrX/(float)n);
         ystarts[i] = i*(ScrY/(float)n);
      }

      for(i=0;i<n && !aborted;i++)
      {
         rect R;

         R.Ymin = i1 + ystarts[i];
         R.Ymax = i1 + ystarts[i+1] - 1;

         for(j=0;j<n;j++)
         {
            unsigned char r;
            R.Xmin = j1 + xstarts[j];
            R.Xmax = j1 + xstarts[j+1]-1;
            r = cp[i*n+j];
            if (RenderingMode == 0)
            {
               
               PenColor(ourpal[r+1]);
               PaintRect(&R);
            }
            else
            {
               VGAPaintRect(&R,r+1);
            }
         }
         AbortCheck();
      }
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
   if (xstarts)
      free(xstarts);
   if (ystarts)
      free(ystarts);
	/* god I wish I had C++ here */

}

void jpures(double fd, int n)
{
	float *a = NULL;
   initialize_forgery(Clouds,fd,power,n,&a);
   initpurecolors();
   genpures(a,n);
}


