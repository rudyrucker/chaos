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
#include "sglib.h"
#include "zaster.h"

unsigned char *zblines[640];
zdepth *zd[640];
int allocated_lines;
static int px, py, pn, ppar, piseq, piffle;
static int pimode;
static int zemask[4];
static double xmin, xmax, ymin, ymax, zmin, zmax;
double vdown = 30.0, vturn = 30.0, mscale;
unsigned long fcleft;
static float *za;
static int ourbpline;
double percentage = 0.75;

static int memall(unsigned int ysize, unsigned int bpline, unsigned int zbsize)
{
   /* Notice that we only use HALF of available memory. */

   int i;
   ourbpline = bpline;


   for(i=0;i<ysize;i++)
   {
      if (RenderingMode == 0)
      {
         safe_alloc = true;
         zblines[i] = (unsigned char *)calloc(bpline,1);
         if (!zblines[i])
            break;
      }
      safe_alloc = true;
      zd[i] = (zdepth *)calloc(zbsize,1);
      if (!zd[i])
      {
         if (RenderingMode == 0)
            free(zblines[i]);
         break;
      }
   }

   allocated_lines = i;
   if (ysize > 200)
   {
      for(i=4*allocated_lines/5;i<allocated_lines;i++)
      {
         if (RenderingMode == 0)
            free(zblines[i]);
         free(zd[i]);
      }
      allocated_lines = 4*allocated_lines/5;
   }
	return allocated_lines;

   
}

#pragma argsused
// absline doesn't get used
static void memline(int lineno,int absline,unsigned char **lineb,zdepth **zb)
{
   if (lineb != NULL)
   {
      if (RenderingMode == 0)
         *lineb = zblines[lineno];
      else
         *lineb = MK_FP(0xa000,(199 - absline) * 320);
   }

   if (zb != NULL)
      *zb = zd[lineno];
}

static void memfree(void)
{
   int i;
   for(i=0;i<allocated_lines;i++)
   {
      if (RenderingMode == 0)
         free(zblines[i]);
      free(zd[i]);
   }
}

static int iteration = 0;

static void polyrew(void)
{
   int i;
   px = py = ppar = 0;
	pimode = False;
	piseq = 0;
	piffle = 0;
   if (RenderingMode == 0)
   {
      for(i=0;i<allocated_lines;i++)
         memset(zblines[i],0,ourbpline);
   }
   iteration = 0;
}

/*
 * POLYGET  --  Return next polygon  from  the  terrain  mesh.   This routine
 * is made more complicated than its task would seem to require because it is
 * called as  a  slave  by Zoroaster.   As  a slave, it has to keep track of
 * its position in the mesh and  return  the	next  triangle each	time.
 * In  addition,  it  contains  logic  to generate the edges where the
 * terrain  was  sectioned, and  just  those  visible  from  the  observer's
 * eye position.
 */

int polyget_calls;

static double scale(double x)
{
   if (x > 0)
      x *= elevfac;

   return x;
}

int NoEdges = false;

static psp polyget(void)
{
	int i, n = pn;
	static struct spolygon sp;
	static double pu[4];
	static vector pd[4];
	static point3d pc[4];


	/*
	 * Pressing  any  key terminates mesh  generation (but doesn't suck
	 * up the key).  This lets you get out of	free-run  mode or an
	 * ugly-looking picture much faster.
	 */

   if (aborted || SomethingWaiting())
   {
      aborted = 1;
      return NULL;
   }

   polyget_calls++;

	if (pimode)
	{

		/* Emitting sectioned edges.  */

		if (NoEdges || piseq == 3 && py >= (n-1))
			return NULL;


		switch (piseq)
		{
		case 0:
		case 1:
			vecget(pd[0], ((double) px) / (n-1),
			       ((double) py) / (n-1), 0.0);
			vecget(pd[1], ((double) px + 1) / (n-1),
			       ((double) py) / (n-1), 0.0);
			vecget(pd[2], ((double) px + 1) / (n-1),
			    ((double) py) / (n-1), scale(za[((px+1)*n+py)*2]));
			vecget(pd[3], ((double) px) / (n-1),
			       ((double) py) / (n-1), scale(za[(px*n+py)*2]));
			break;
		case 2:
		case 3:
			vecget(pd[0], ((double) px) / (n-1),
			       ((double) py) / (n-1), 0.0);
			vecget(pd[1], ((double) px) / (n-1),
			       ((double) py + 1) / (n-1), 0.0);
			vecget(pd[2], ((double) px) / (n-1),
			       ((double) py + 1) / (n-1),
                scale(za[(px*n+py+1)*2]));

			vecget(pd[3], ((double) px) / (n-1),
			       ((double) py) / (n-1), scale(za[(px*n+py)*2]));
			break;
		}
	}
	else
	{

		/* Generating terrain mesh.	Build next mesh tile.  */

		if (ppar == 0)
		{
			vecget(pd[0], ((double) px) / (n-1),
			       ((double) py) / (n-1),
                scale(za[(px*n+py)*2]));
			vecget(pd[1], ((double) px + 1) / (n-1),
			       ((double) py) / (n-1),
                scale(za[((px+1)*n+py)*2]));
			vecget(pd[2], ((double) px + 1) / (n-1),
			       ((double) py + 1) / (n-1),
                scale(za[((px+1)*n+py+1)*2]));
			vecget(pd[3], ((double) px) / (n-1),
			       ((double) py + 1) / (n-1),
                scale(za[(px*n+py+1)*2]));
		}
	}

	/*
	 * Project  the  current  tile to the display.	 If any of the tile's
	 * corners are below sea level, snap them to sea level.
	 */

	if (pimode || (ppar == 0))
	{
		for (i = 0; i < 4; i++)
		{
			vector po;

			pu[i] = 0;
			if (pd[i][Z] < 0)
			{
				pu[i] = pd[i][Z];	/* Save depths below
							 * water */
				pd[i][Z] = 0;
			}
			vecxmat(po, pd[i], ct);
			vecput(&pc[i][X], &pc[i][Y], &pc[i][Z], po);
		}

		/*
		 * Assign shade to tile.  Edge section tiles are coloured
		 * with constant dirt colour, different for each edge.
		 * Underwater tiles  are  shaded  by average depth, and
		 * terrain tiles are coloured based on their elevation,  then
		 * shaded	based  on their angle to the observer using
		 * the cosine law.
		 */

		if (pimode)
		{
			sp.pcolour = 70 - piffle;	/* Sectioned edge */
		}
		else
		{
			double elev = 20.0 *
			(pd[0][Z] + pd[1][Z] + pd[2][Z] + pd[3][Z]) / 4.0;
			vector ve1, ve2, vn;

			if (elev == 0.0)
			{

				/* Underwater. */

#ifdef OLD_UNDER
				sp.pcolour = 71;	/* Icky constant colour
							 * water */
#else
				elev = 0.0;	/* Pretty depth-contoured
						 * water */
				for (i = 0; i < 4; i++)
				{
					elev += pu[i];
				}
				sp.pcolour = 240 + -15 * (elev / 4);
#endif
			}
			else
			{

				/*
				 * Terrain tile.  Calculate normal to face
				 * prior to scaling onto screen and into
				 * Z-buffer.
				 */

				vecsub(ve1, pc[0], pc[1]);
				vecsub(ve2, pc[2], pc[1]);
				ve1[X] /= mscale;
				ve1[Y] /= mscale;
				ve1[Z] /= 65000.0 / (zmax - zmin);
				ve2[X] /= mscale;
				ve2[Y] /= mscale;
				ve2[Z] /= 65000.0 / (zmax - zmin);
				veccross(vn, ve1, ve2);
				vecnorm(vn, vn);
				sp.pcolour = 72 + floor(elev) * 8 +
					floor(7 * fabs(vn[Z]) + 0.5);
            sp.pcolour = min(sp.pcolour,231);
            sp.pcolour = max(sp.pcolour,72);

#ifdef DLUT
				dlut[sp.pcolour] = True;
#endif
			}
		}
	}
	sp.npoints = pimode ? 4 : 3;


	/*
	 * Build  Zoroaster  polygon  packet  for   tile.   Note  that
	 * sectioned edges (generated while pimode is True) are placed at
	 * the  maximum  Z buffer value to they obscure everything else.
	 */

	for (i = 0; i < 4; i++)
	{
		sp.pt[i].x = pc[i][X];
		sp.pt[i].y = pc[i][Y];
		sp.pt[i].z = pimode ? 65535U : pc[i][Z];
	}

#ifdef BADPT
	for (i = 0; i < 4; i++)
	{
		if (sp.pt[i].x < 0 || sp.pt[i].x >= SCRX ||
		    sp.pt[i].y < 0 || sp.pt[i].y >= SCRY ||
		    pc[i][Z] < 0 || pc[i][Z] > 65535.0)
		{
			printf("Bad point %d: %d,%d,%g\n", i, sp.pt[i].x, sp.pt[i].y, pc[i][Z]);
			sp.pt[i].x = 0;
			sp.pt[i].y = 0;
			sp.pt[i].z = 0;

		}
	}
#endif

   /* I'm feeling mistrustful of Zaster's ability to eliminate
      duplicates in the middle of a packet. So I'm going to
      try to eliminate them by hand here, but just the one case. */
   if (pimode)
   {
      if (sp.pt[1].x == sp.pt[2].x && sp.pt[1].y == sp.pt[2].y)
      {
         sp.npoints = 3;
         sp.pt[2] = sp.pt[3];
      }
   }


	/*
	 * This  awful mess steps the  counters to the next tile.  All the
	 * tests and gotos in the piseq switch  handle  visibility of
	 * sectioned edges.
	 */

freen:
	if (pimode)
	{
		switch (piseq)
		{

		case 0:
			px++;
			if (px < (n-1))
				break;
			piseq++;
			if (!zemask[1] && !zemask[3])
				goto pseq2;
			piffle++;
			px = 0;
			py = (n-1);
			break;

		case 1:
			px++;
			if (px < (n-1))
				break;
	pseq2:
			piseq++;
			if (!zemask[0] && !zemask[1])
				goto pseq3;
			piffle++;
			py = px = 0;
			break;

		case 2:
			py++;
			if (py < (n-1))
				break;
	pseq3:
			piseq++;
			py = 0;
			px = (n-1);
			if (!zemask[2] && !zemask[3])
				py = (n-1);
			piffle++;
			break;

		case 3:
			py++;
			break;
		}
	}
	else
	{
		if (ppar)
		{
			sp.pt[1] = sp.pt[2];
			sp.pt[2] = sp.pt[3];
		}

		if (ppar)
		{
			px++;
			if (px >= (n-1))
			{
				px = 0;
				py++;
				if (py >= (n-1))
				{
					pimode = True;
					piseq = 0;
					px = py = 0;
					if (!zemask[0] && !zemask[2])
					{
						px = pn;
						goto freen;
					}
				}
			}
			ppar = 0;
		}
		else
		{
			ppar++;   
		}
	}



	rastpext(&sp);
	return &sp;
}


static void lineret(int lineno,unsigned char *linebuf)
{
   /* this actually shoots out a line of rasterized data. */
   int i;
   int k;
   int ir, ig, ib, idith;
   int left,right,bottom;

   left = planetRect.Xmin;
   right = planetRect.Xmax+1;
   bottom = planetRect.Ymax;

   if (planetRect.Xmin != 0)
   {
      left++;
      right--;
      bottom--;
   }

   if (aborted)
      return;

   if (RenderingMode == 0)
   {

      if (planetRect.Ymax-1-lineno == TickerFrame.Ymax + 10 &&
          planetRect.Xmin < TickerFrame.Ymin)
         MoveTicker();
      for(i=0,k=left;k<right;k++,i++)
      {
         unsigned char r = linebuf[i];

         if (r)
         {
            if (!WireFraming)
            {

               ir = clut[r][0];
               ig = clut[r][1];
               ib = clut[r][2];

   	         ir = ((NCOLOURS - 1) * ir) / 255;
   	         ig = ((NCOLOURS - 1) * ig) / 255;
   	         ib = ((NCOLOURS - 1) * ib) / 255;
   	         idith = ditherm[k & (DITHERN - 1)][lineno & (DITHERN - 1)];

   	         PenColor(colmap[((ir > idith) ? 1 : 0) |
   			         ((ig > idith) ? 2 : 0) |
   			         ((ib > idith) ? 4 : 0)]);
            }
            else
               PenColor(r);

            SetPixel(k,bottom-lineno);
         }
      }
   }
}






static void fillbott(void)
{
   unsigned int i, j, lastj = 0;
	unsigned char *screen;
   int ScrY,ScrX;

   if (aborted)
      return;

   /* Shit. How can we fill the bottom when we don't know what the
      colors are? I think we need to know the shape of the baseplate
      and do it from there, neh? */

   if (!DoClouds)
      return;

   if (RenderingMode == 0)
   {
      extern void fillEGAbott(void);

      fillEGAbott();
      return;
   }
   ScrX = 320;
   ScrY = 200;

	for (i = 0; i < ScrX; i++)
	{
		for (j = ScrY - 1; j > 0; j--)
		{
			screen = MK_FP(0xa000,j*320+i);
			if (*screen == 70 || *screen == 69)
				break;

		}
		if (j > 0 || lastj > 0)
		{
			if (j == 0)
				j = lastj;
			lastj = j;
			for (j = lastj + 1; j < ScrY; j++)
				*(char *)(MK_FP(0xa000,j*320+i)) = 0;
		}
	}
}




int PreScale = 1;
int first_mountain_projection = 1;
static double bpmin, sx, sy;
static matrix m, m1, rtm;
static double bpz[4];

void genproj(float *a, int n, double hgtfac)
{
	int i, j, k;
	int ScrY;
   int ScrX;
   unsigned long totalticks = (n*(long)n);
   unsigned long ticker=0;
   unsigned percent;
   unsigned long nexttick;

	za = a+1;
	
   if (RenderingMode == 0)
   {
      if (planetRect.Xmin == 0)
      {
         ScrY = percentage * (planetRect.Ymax-planetRect.Ymin+1);
         ScrX = (planetRect.Xmax-planetRect.Xmin + 1);
      }
      else
      {
         ScrY = percentage * (planetRect.Ymax-planetRect.Ymin-1);
         ScrX = (planetRect.Xmax-planetRect.Xmin - 1);
      }

   }
   else
   {
      ScrX = 320;
      ScrY = percentage * 200;
   }

   if (1)
   {


	   /*
	   * Prescale the grid points into intensities and calculate
	   * transformed extents.
	   */

      InitTicker("Scaling mountain",totalticks);
      nexttick = tickertape[percent = 0];


	   /* Let's try only doing the scaling once, see what happens. */
	   if (first_mountain_projection && !PreScale)
	   {
		   xmin = ymin = zmin = 1E50;
		   xmax = ymax = zmax = -1E50;
      }
      bpmin = -1E50;
  	   matident(ct);
      tran(0.5,0.5,0.0);
   	rot(torad(vdown), X);
   	rot(torad(vturn), Z);

      tran(-.5,-.5,0.0);
	   for (i = 0; i < n; i++)
	   {
         int ixn = i * n * 2;
		   for (j = 0; j < n; j++)
		   {
			   vector vi, vo;
			   double tx, ty, tz;

            tz = za[ixn+j*2];
			   if (tz < 0.0)
			   {
				   tz = 0.0;
			   }
			   else
			   {
//				   za[ixn+j*2] = (tz *= hgtfac);
               tz *= hgtfac;
			   }
			   vecget(vi, ((double) i) / (n - 1),
				         ((double) j) / (n - 1),
				         tz);
			   vecxmat(vo, vi, ct);
			   vecput(&tx, &ty, &tz, vo);
            if (first_mountain_projection && !PreScale)
            {
				   xmin = min(xmin, tx);
				   xmax = max(xmax, tx);
				   ymin = min(ymin, ty);
				   ymax = max(ymax, ty);
				   zmin = min(zmin, tz);
				   zmax = max(zmax, tz);
			   }
            if (++ticker > nexttick)
            {
               tick(++percent);
               nexttick = tickertape[percent];
            }   
		   }
      }

	   /* Include corner points of baseplate in extents */

	   for (i = 0; i < n; i += (n - 1))
	   {
		   for (j = 0; j < n; j += (n - 1))
		   {
			   vector vi, vo;
			   double tx, ty, tz;

			   vecget(vi, ((double) i) / (n - 1),
				         ((double) j) / (n - 1),
				         0.0);
			   vecxmat(vo, vi, ct);
			   vecput(&tx, &ty, &tz, vo);
            if (first_mountain_projection && !PreScale)
            {
		         xmin = min(xmin, tx);
		         xmax = max(xmax, tx);
		         ymin = min(ymin, ty);
		         ymax = max(ymax, ty);
		         zmin = min(zmin, tz);
		         zmax = max(zmax, tz);
            }
		   }
	   }

	   /*
		   * Create transformation and projection matrix.  Note that
		   * we compose  the  transformation  and  projection  matrices
		   * by multiplication so only one vector by matrix
		   * multiplication is needed to transform each point in the
		   * mesh.
		   */

      if (1)
      {
         sx = (ScrX - 1) / (xmax - xmin);
         sy = (ScrY - 1) / (ymax - ymin);

         if (first_mountain_projection)
            mscale = min(sx, sy);

         matscal(m1, sx, sy, 65000.0 / (zmax - zmin));
         mattran(m, -xmin, -ymin, -zmin);
         matmul(rtm, m, m1);
         matmul(m, ct, rtm);
         matcopy(ct, m);

         /* Include corner points of baseplate in extents */

         for (i = 0; i < n; i += (n - 1))
         {
   	      for (j = 0; j < n; j += (n - 1))
   	      {
   		      vector vi, vo;
   		      double tx, ty, tz;

   		      vecget(vi, ((double) i) / (n - 1),
   				         ((double) j) / (n - 1),
   				         0.0);
   		      vecxmat(vo, vi, ct);
   		      vecput(&tx, &ty, &tz, vo);
   		      bpz[((!!i) << 1) + (!!j)] = tz;
   	      }
         }

	   /*
		   * Now find the closest point  of the basepoint.  This is
		   * used to determine which  edges  should  be  overdrawn
		   * with  the section profile.
		   */

         if (1/*first_mountain_projection*/)
         {
 	         for (i = 0; i < 4; i++)
  	         {
  		         zemask[i] = False;
  		         if (bpz[i] > bpmin)
  			         bpmin = bpz[i];
  	         }
         }
         k = -1;
         for (i = j = 0; i < 4; i++)
         {
   	      if (bpz[i] == bpmin)
   	      {
   		      j++;
   		      if (k < 0)
   			      k = i;
   	      }
         }
	      zemask[k] = True;
      }
   }

   pn = n;
   CloseTicker();
   if (RenderingMode == 1 && !DoClouds)
   {
      union REGS regs;
      regs.h.al = 0x13;
      int86(0x10,&regs,&regs);
   }
   if (!DoClouds)
      initmountaincolors();

   polyget_calls = 0;






#pragma warn -pro
	zaster(ScrX,ScrY,256, memall, memline, memfree, polyrew,
	       polyget, lineret);
#pragma warn .pro

	fillbott();
}

unsigned char mountaincolors[] = {
	0,  0,  0,
	0,  0, 63,
	1,  1, 63,
	2,  2, 63,
	3,  3, 63,
	4,  4, 63,
	5,  5, 63,
	6,  6, 63,
	7,  7, 63,
	8,  8, 63,
	9,  9, 63,
	10, 10, 63,
	11, 11, 63,
	12, 12, 63,
	13, 13, 63,
	14, 14, 63,
	15, 15, 63,
	16, 16, 63,
	17, 17, 63,
	18, 18, 63,
	19, 19, 63,
	20, 20, 63,
	21, 21, 63,
	22, 22, 63,
	23, 23, 63,
	24, 24, 63,
	25, 25, 63,
	26, 26, 63,
	27, 27, 63,
	28, 28, 63,
	29, 29, 63,
	30, 30, 63,
	31, 31, 63,
	32, 32, 63,
	33, 33, 63,
	34, 34, 63,
	35, 35, 63,
	36, 36, 63,
	37, 37, 63,
	38, 38, 63,
	39, 39, 63,
	40, 40, 63,
	41, 41, 63,
	42, 42, 63,
	43, 43, 63,
	44, 44, 63,
	45, 45, 63,
	46, 46, 63,
	47, 47, 63,
	48, 48, 63,
	49, 49, 63,
	50, 50, 63,
	51, 51, 63,
	52, 52, 63,
	53, 53, 63,
	54, 54, 63,
	55, 55, 63,
	56, 56, 63,
	57, 57, 63,
	58, 58, 63,
	59, 59, 63,
	60, 60, 63,
	61, 61, 63,
	62, 62, 63,
	63, 63, 63,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	26, 16,  7,
	32, 20,  9,
	10, 15, 45,
	6, 12,  6,
	8, 15,  7,
	9, 19,  8,
	12, 23,  8,
	14, 27,  8,
	15, 30,  8,
	18, 33,  9,
	19, 36,  9,
	11, 14,  2,
	13, 17,  4,
	16, 22,  4,
	17, 26,  4,
	18, 29,  5,
	19, 32,  5,
	21, 36,  5,
	21, 39,  5,
	15, 16,  3,
	15, 19,  5,
	17, 24,  5,
	20, 28,  5,
	21, 33,  5,
	23, 35,  5,
	23, 39,  5,
	27, 42,  5,
	15, 16,  5,
	18, 21,  5,
	19, 26,  5,
	21, 29,  5,
	23, 33,  5,
	25, 35,  5,
	27, 39,  5,
	29, 42,  5,
	17, 14,  5,
	19, 19,  5,
	20, 32,  5,
	23, 36,  5,
	26, 39,  6,
	26, 41,  6,
	29, 45,  6,
	32, 49,  6,
	17, 24,  5,
	20, 29,  5,
	21, 34,  5,
	23, 38,  6,
	27, 40,  6,
	27, 44,  6,
	31, 48,  6,
	31, 51,  6,
	17, 24,  5,
	21, 29,  5,
	23, 34,  5,
	24, 37,  6,
	27, 41,  6,
	28, 44,  6,
	32, 48,  6,
	33, 51,  6,
	15, 22,  4,
	19, 26,  5,
	20, 32,  5,
	24, 37,  5,
	25, 41,  5,
	27, 43,  6,
	31, 46,  6,
	32, 49,  6,
	15, 22,  3,
	19, 27,  4,
	20, 31,  4,
	23, 36,  5,
	25, 41,  5,
	29, 44,  6,
	30, 48,  6,
	33, 50,  7,
	16, 27,  5,
	18, 30,  6,
	20, 35,  6,
	24, 38,  6,
	26, 42,  6,
	31, 46,  6,
	32, 50,  6,
	33, 50,  7,
	16, 26,  5,
	18, 30,  6,
	21, 35,  6,
	23, 39,  6,
	25, 43,  6,
	26, 46,  6,
	28, 49,  7,
	34, 51,  7,
	17, 29,  6,
	18, 34,  6,
	20, 39,  6,
	25, 43,  7,
	25, 47,  7,
	29, 50,  7,
	31, 51,  7,
	35, 52,  7,
	14, 26,  7,
	19, 31,  6,
	20, 35,  6,
	23, 39,  6,
	23, 43,  6,
	26, 47,  6,
	30, 50,  6,
	32, 53,  6,
	13, 23,  6,
	14, 28,  6,
	20, 33,  6,
	23, 37,  6,
	23, 40,  6,
	26, 45,  6,
	30, 47,  6,
	33, 51,  6,
	28, 40, 16,
	26, 41, 17,
	30, 42, 19,
	31, 44, 20,
	35, 44, 24,
	35, 45, 25,
	36, 47, 27,
	42, 48, 28,
	32, 40, 25,
	34, 43, 26,
	37, 44, 28,
	40, 46, 31,
	42, 47, 33,
	47, 49, 36,
	51, 49, 37,
	52, 49, 37,
	42, 31, 26,
	44, 33, 27,
	45, 36, 29,
	47, 38, 31,
	48, 40, 33,
	50, 43, 36,
	52, 45, 38,
	53, 47, 40,
	31, 26, 25,
	36, 29, 28,
	40, 34, 31,
	42, 38, 35,
	45, 40, 38,
	47, 43, 40,
	50, 46, 43,
	51, 47, 45,
	35, 33, 31,
	40, 38, 36,
	45, 43, 40,
	49, 47, 44,
	53, 50, 47,
	56, 54, 50,
	60, 57, 53,
	63, 60, 56,
	35, 35, 35,
	40, 40, 40,
	45, 45, 45,
	49, 49, 49,
	53, 53, 53,
	56, 56, 56,
	60, 60, 60,
	63, 63, 63,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0,  0,  0,
	0, 60, 60,
	0, 57, 59,
	0, 55, 59,
	0, 53, 59,
	0, 50, 58,
	0, 47, 58,
	0, 45, 58,
	0, 42, 57,
	0, 39, 57,
	0, 36, 57,
	0, 33, 56,
	0, 29, 56,
	0, 25, 56,
	0, 20, 55,
	0, 14, 55,
	0,  0, 55,
};


void initmountaincolors(void)
{
   int i;

   for(i=0;i<256;i++)
   {
      Spal(i,
         mountaincolors[i*3],
         mountaincolors[i*3+1],
         mountaincolors[i*3+2]);
   }
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
}



void ScaleMountain(float *a,int n,double hgtfac)
{
   /* Don't draw the picture; just tweak the minima and maxima. */
   int i,j,k;
	int ScrY;
   int ScrX = 320;
   unsigned long totalticks = (n*(long)n);
   unsigned long ticker=0;
   unsigned percent;
   unsigned long nexttick;

   za = a+1;
   ScrY = percentage * 200;
   if (first_mountain_projection)
   {
      xmin = ymin = zmin = 1E50;
   	xmax = ymax = zmax = -1E50;
   }
   InitTicker("Scaling mountain",totalticks);
   nexttick = tickertape[percent = 0];
   bpmin = -1E50;
	matident(ct);
   tran(0.5,0.5,0.0);
	rot(torad(vdown), X);
	rot(torad(vturn), Z);
   tran(-.5,-.5,0.0);
	for (i = 0; i < n; i++)
	{
      int ixn = i*n*2;

		for (j = 0; j < n; j++)
		{
			vector vi, vo;
			double tx, ty, tz;

			tz = za[ixn+j*2];
			if (tz < 0.0)
			{
				tz = 0.0;
			}
			else
			{
//            za[ixn+j*2] = (tz *= hgtfac);
            tz *= hgtfac;

			}
			vecget(vi, ((double) i) / (n - 1),
				      ((double) j) / (n - 1),
				      tz);
			vecxmat(vo, vi, ct);
			vecput(&tx, &ty, &tz, vo);
			xmin = min(xmin, tx);
			xmax = max(xmax, tx);
			ymin = min(ymin, ty);
			ymax = max(ymax, ty);
			zmin = min(zmin, tz);
			zmax = max(zmax, tz);
         if (++ticker > nexttick)
         {
            tick(++percent);
            nexttick = tickertape[percent];
         }   
      }
   }
	/* Include corner points of baseplate in extents */

	for (i = 0; i < n; i += (n - 1))
	{
		for (j = 0; j < n; j += (n - 1))
		{
			vector vi, vo;
			double tx, ty, tz;

			vecget(vi, ((double) i) / (n - 1),
				      ((double) j) / (n - 1),
				      0.0);
			vecxmat(vo, vi, ct);
			vecput(&tx, &ty, &tz, vo);
		   xmin = min(xmin, tx);
		   xmax = max(xmax, tx);
		   ymin = min(ymin, ty);
		   ymax = max(ymax, ty);
		   zmin = min(zmin, tz);
		   zmax = max(zmax, tz);
		}
	}
	/*
		* Create transformation and projection matrix.  Note that
		* we compose  the  transformation  and  projection  matrices
		* by multiplication so only one vector by matrix
		* multiplication is needed to transform each point in the
		* mesh.
		*/

   sx = (ScrX - 1) / (xmax - xmin);
   sy = (ScrY - 1) / (ymax - ymin);

   mscale = min(sx, sy);
   matscal(m1, sx, sy, 65000.0 / (zmax - zmin));
   mattran(m, -xmin, -ymin, -zmin);
   matmul(rtm, m, m1);
   matmul(m, ct, rtm);
   matcopy(ct, m);

   /* Include corner points of baseplate in extents */

   for (i = 0; i < n; i += (n - 1))
   {
   	for (j = 0; j < n; j += (n - 1))
   	{
   		vector vi, vo;
   		double tx, ty, tz;

   		vecget(vi, ((double) i) / (n - 1),
   				   ((double) j) / (n - 1),
   				   0.0);
   		vecxmat(vo, vi, ct);
   		vecput(&tx, &ty, &tz, vo);
   		bpz[((!!i) << 1) + (!!j)] = tz;
   	}
   }

	/*
		* Now find the closest point  of the basepoint.  This is
		* used to determine which  edges  should  be  overdrawn
		* with  the section profile.
		*/
 	for (i = 0; i < 4; i++)
  	{
  		zemask[i] = False;
  		if (bpz[i] > bpmin)
  			bpmin = bpz[i];
  	}
   k = -1;
   for (i = j = 0; i < 4; i++)
   {
   	if (bpz[i] == bpmin)
   	{
   		j++;
   		if (k < 0)
   			k = i;
   	}
   }
	zemask[k] = True;
}
#undef X
#undef Y
#undef Z

static void fillEGAbott(void)
{
   vector v1;
   vector endpoints[3];
   point polypoints[6];
   polyHead Head;
   rect R;
   int left,right,bottom;

   left = planetRect.Xmin;
   right = planetRect.Xmax;
   bottom = planetRect.Ymax;

   if (left)
   {
      left++;
      right--;
      bottom--;
   }

   vecget(v1,0.0,0.0,0.0);
   vecxmat(endpoints[0],v1,ct);

   vecget(v1,1.0,0.0,0.0);
   vecxmat(endpoints[1],v1,ct);

   vecget(v1,1.0,1.0,0.0);
   vecxmat(endpoints[2],v1,ct);

   /* Gee. Now I think we just have a polygon we can fill
      with Meta? */

   polypoints[0].X = left + endpoints[0][0];
   polypoints[0].Y = bottom - endpoints[0][1];

   polypoints[1].X = left + endpoints[1][0];
   polypoints[1].Y = bottom - endpoints[1][1];

   polypoints[2].X = left + endpoints[2][0];
   polypoints[2].Y = bottom - endpoints[2][1];

   polypoints[3].X = right;
   polypoints[3].Y = bottom;

   polypoints[4].X = left;
   polypoints[4].Y = bottom;

   polypoints[5] = polypoints[0];

   Head.polyBgn = 0;
   Head.polyEnd = 5;
   Head.polyRect.Xmin = left;
   Head.polyRect.Ymin = planetRect.Ymin;
   Head.polyRect.Xmax = right;
   Head.polyRect.Ymax = bottom;

   PenColor(BLACK);
   R = planetRect;
   if (left)
      InsetRect(&R,1,1);
   ClipRect(&R);
   PaintPoly(1,&Head,polypoints);
   ClipRect(&sR);
 
}

