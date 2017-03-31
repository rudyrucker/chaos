#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include "forge.h"

/* dxfout the array. we add a baseplate and side plates also. */


void dxfout(void)
{
   char filename[128];
   FILE *fd;
   int i,j;

	unsigned char *cp, *ap;
	int ScrY,ScrX;
	double *u, *u1;
	char *bxf, *bxc;
   float xmin = 1e50;
   float xmax = -1e50;
   float *za;
   int n = select_file("DXF Out","*.dxf",filename,"DXF");
   unsigned long totalticks;
   unsigned long ticker=0;
   unsigned percent;
   unsigned long nexttick;

	LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);
   
   if (!n || !Overwrite(filename))
      return;


   ticking = 1;
   initgauss(rseed);
   initialize_forgery(Clouds,fracdim,power,meshsizes[mesh],&databuffer);

   n = meshsizes[mesh];
   ScrX = ScrY = n;

   fd = fopen(filename,"wt");
   if (!fd)
      return;
   fprintf(fd,"0\nSECTION\n2\nENTITIES\n0\nPOLYLINE\n8\nChaosMesh\n");
   fprintf(fd,"70\n16\n71\n%d\n72\n%d\n66\n1\n",ScrX,ScrY);





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
   za = databuffer+1;

   for(i=0;i<n;i++)
   {
      int ixn = i*n*2;
      for(j=0;j<n;j++)
      {
         float beep;

         beep = za[ixn+j*2];
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

         r = za[ixn+j*2];

         *ap++ = (r - xmin) / (xmax - xmin) * 255.0;

		}
	}


   totalticks = n*n;
   InitTicker("Creating DXF file",totalticks);
   nexttick = tickertape[percent = 0];

	for (j = 0; j < ScrX; j++)
	{
		double bx = (n - 1) * (j / (double)ScrX);

		bxf[j] = floor(bx);
		bxc[j] = bxf[j] + 1;
		u[j] = bx - bxf[j];
		u1[j] = 1 - u[j];
	}


	for (i = 0; !aborted && i < ScrY; i++)
	{
		double t, t1, by = (n - 1) * (i / (float)ScrY);
		int byf, byc;

      aborted |= SomethingWaiting();
		byf = floor(by) * n;
		byc = byf + n;
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

         /* Create this vertex */
         fprintf(fd,"0\nVERTEX\n8\nChaosMesh\n10\n%g\n20\n%g\n30\n%g\n70\n64\n",
            j/(double)(ScrX-1),i/(double)(ScrY-1),r/255.0 - .5);

         if (++ticker > nexttick)
         {
            tick(++percent);
            nexttick = tickertape[percent];
         }   

		}

	}
   CloseTicker();
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
   fprintf(fd,"0\nSEQEND\n8\nChaosMesh\n0\nENDSEC\n0\nEOF\n");
   fclose(fd);
}
