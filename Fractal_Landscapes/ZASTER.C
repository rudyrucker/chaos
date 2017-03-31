/*

	Zoroaster

	Software Z-Buffer and Polygon Fill

	Designed and implemented by Kelvin R. Throop  in  November  of
   1988.   Based  on  Rasta, which was in turn based on AutoCAD's
	polygon fill code, designed and implemented by John Walker  in
	January of 1984.

		    (C) Copyright 1988	Autodesk, Inc.

	This   program	is  copyrighted  by  Autodesk,	Inc.   and  is
	furnished pursuant  to	a  software  license  agreement.   The
	source	code  of  this	program  may  not  be  distributed  or
	published in any form by the licensee.	Licensee  is  free  to
	incorporate  this  code  in  object  form  in derivative works
	providing such derivative works are solely  intended  to  work
	with   Autodesk,   Inc.   products.   Said  derivative	works,
	incorporating  object  versions  of  this   program   may   be
	distributed without royalty at the discretion of the licensee.

	Autodesk  makes  no  warranties, express or implied, as to the
	correctness  of  this  code  or  any  derivative  works  which
   incorporate  it.   Autodesk provides this code on an ``as-is''
	basis and  explicitly  disclaims  any  liability,  express  or
	implied,  for  errors,	omissions,  and  other problems in the
	code, including consequential and incidental damages.

*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "forge.h"
#include "zaster.h"
#include "sglib.h"

/*LINTLIBRARY*/

struct edg
{				/* Edge list item */
	scrcoord ymax, ymin, xpos, xend;
	double slope;
	zdepth zpos, zend;
};
static void lflood(struct spolygon * poly);

/* Screen vector */

struct svector
{
	struct spoint f;	/* From point */
	struct spoint t;	/* To point */
};

/*  Forward procedures	*/

void lflood(), vpcont(), vp256();

/*  Local storage  */

#define vertex poly->pt

static struct svector svec;
static void (*vectaker) (void);	/* Where to send vectors */
//static void (*meml) ();	/* Get memory line buffer routine */
static void (*meml) (int, int, unsigned char **, zdepth **);
static short pcol;		/* Polygon colour description */
static int bpline;		/* Bytes per line */
static int bpcolour;		/* Bytes per colour (for continuous) */
static int bandtop;		/* Top of band line */
static int bandbot;		/* Bottom of band line */
static double pqa, pqb, pqc, pqd;	/* Equation of current polygon plane */
static double pqstep;		/* X step for incremental depth */
static int pqinc;		/* True if incremental Z possible */

#ifdef ISTATS
static int ispasses;		/* Statistic counters */
static long ispolys, ispixels, isinc, ishard, iscorr, isingpixel, iszeronorm, isedgeon;

#endif

#ifdef abs
#undef abs
#endif
#define abs(x)	    (((x) < 0) ? (-(x)) : (x))
#ifdef min
#undef min
#endif
#define min(a, b)   ((a) < (b) ? (a) : (b))
#ifdef max
#undef max
#endif
#define max(a, b)   ((a) > (b) ? (a) : (b))

#define Fp(x)	       ((double) (x))
#define Fix(x)	       ((int) (x))
#define CurrentZ(x, y) ((-(pqd + pqa * (x) + pqb * (y))) / pqc)
#define V	       (void)

/*  ZASTER  --	Master control routine.  This is called by the user program
		to rasterise a set of polygons.  It remains in control until
		the rasterisation is complete, calling the user program to
		return polygons and passing completed rasterised lines
		back to the program.  */

void zaster(int xsize,
	     int ysize,
	     int ncolours,
	     int (*memall) (unsigned int, unsigned int, unsigned int),
	     void (*memline) (int, int, unsigned char **, zdepth **),
	     void (*memfree) (void),
	     void (*polyrew) (void),
	     struct spolygon * (*polyget) (void),
	     void (*lineret) (int, unsigned char *))
{
	int i, j;
	int nlines,		/* Number of lines in a band */
	 passes,		/* Number of passes over polygons */
	 singpass;		/* Single pass flag for optimisation */
	struct spolygon *poly;	/* Pointer to current polygon */
	unsigned char *mlret;	/* Memory line buffer pointer */
	zdepth *mlretz;		/* Memory line Z-buffer pointer */

   unsigned long totalticks;
   unsigned percent;
   unsigned long ticker = 0;
   unsigned long nexttick;


	switch (ncolours)
	{

	case 0:		/* Continuous colour */
		bpline = xsize * 3;
		bpcolour = xsize;
		vectaker = vpcont;
		break;

	case 256:		/* 256 colour map */
		bpline = xsize;
		vectaker = vp256;
		break;


	default:
		fprintf(stderr, "\nZASTER - Invalid ncolours argument: %d\n",
			  ncolours);
		abort();
	}

/* Request memory for rasterisation buffer.  The user is passed
	   the number of lines and the bytes required per line.  The
	   user should return the number of lines that could be provided
	   in memory, from 1 to YSIZE. */

	nlines = (*memall) (ysize, bpline, xsize * (sizeof(zdepth)));
	meml = memline;		/* Make available to line routines */

	if (nlines == 0)
	{
		fprintf(stderr, "\nZASTER - MEMALL returned no lines.\n");

		abort();
	}
	else if (nlines > ysize)
	{
		fprintf(stderr,
			   "\nZASTER - MEMALL returned too many (%d) lines.\n", nlines);

		abort();
	}

/* Now calculate the number of passes required over the
	   polygons.  We are forced to make multiple passes if
	   MEMALL was not able to supply as many line buffers as
	   there are vertical lines. */

	passes = (ysize + (nlines - 1)) / nlines;
	assert(passes > 0);
	singpass = passes == 1;

   /* OK, we know how many polygons we are going to process totally.
      There will be something like (n*n + 2*n) * passes. */

   i = meshsizes[mesh];
   totalticks = (i*(long)i + (i-1L)*(i-1L)) * (long)passes + ysize;
   if (RenderingMode == 0)
     ticking = 1;
   else if (DoClouds)
      ticking = 0;
   else if (percentage <= .75)
      ticking = 2;
   else
      ticking = 0;

   InitTicker("Z-buffering",totalticks);
   nexttick = tickertape[percent = 0];

	bandtop = 0;
#ifdef ISTATS
	ispolys = ispixels = isinc = ishard = iscorr = isingpixel =
		iszeronorm = isedgeon = 0L;
	ispasses = passes;
#endif

	/* This is the main polygon processing loop. */

	while (!aborted && passes--)
	{
		(*polyrew) ();	/* Start polygon read, clear line buffer */
		bandbot = min(ysize, bandtop + nlines);

		/* Clear the Z-buffer to the maximum value */

		for (i = bandtop; i < bandbot; i++)
		{
			(*memline) (i - bandtop, i, NULL, &mlretz);
			for (j = 0; j < xsize; j++)
				*mlretz++ = HIGHZ;
		}

		/* Loop over polygons */

		while ((poly = (*polyget) ()) != NULL)
		{

         if (++ticker > nexttick)
         {
            tick(++percent);
            nexttick = tickertape[percent];
         }


			/*
			 * If POLYGET returns a zero vertex count, bail out.
			 * This permits the program to respond to a
			 * termination request or other contingency.
			 */

			if (poly->npoints == 0)
			{
				(*memfree) ();	/* Release allocated memory */
				return;	/* Return immediately */
			}
#ifdef ISTATS
			if (bandtop == 0)
				ispolys++;
#endif

			/*
			 * If this is a multi-pass rasterisation, test the
			 * extents of the polygon and see if the polygon can
			 * generate any fill in this band.
			 */

			if (singpass || !((poly->pymax < bandtop) ||
					  (poly->pymin >= bandbot)))
			{
				pcol = poly->pcolour;
				lflood(poly);
			}
		}

		/* Return lines rasterised in this band. */

		for (i = bandtop; !aborted && i < bandbot; i++)
		{
         if (++ticker > nexttick)
         {
            tick(++percent);
            nexttick = tickertape[percent];
         }
			(*memline) (i - bandtop, i, &mlret, NULL);
			(*lineret) (i, mlret);
		}
		bandtop = bandbot;
	}
	(*memfree) ();
   CloseTicker();
}

/*  ZSTATS  --	Print rasterisation statistics.  Valid only if this
		module is compiled with ISTATS defined.  */

#ifdef ISTATS
void zstats()
{
	fprintf(stderr, "Polygons processed:       %9ld\n", ispolys);
	fprintf(stderr, "Passes over polygons:     %9d\n\n", ispasses);

	fprintf(stderr, "Single pixel polygons:    %9d\n", isingpixel);
	fprintf(stderr, "Zero normal polygons:     %9d\n", iszeronorm);
	fprintf(stderr, "Edge-on polygons:         %9d\n\n", isedgeon);

	fprintf(stderr, "Incremental calculation:  %9ld\n", isinc);
	fprintf(stderr, "Full calculation:       + %9ld\n", ishard);
	fprintf(stderr, "                          ---------\n", isinc);
	fprintf(stderr, "Total pixels stored:      %9ld\n\n", ispixels);

	fprintf(stderr, "Round-off corrections:    %9ld\n", iscorr);

	assert(ispixels == (ishard + isinc));
}

#endif

/*  VPCONT  --	Vector processing routine for continuous colour  */

static void vpcont(void)
{
#ifdef USEVPCONT
   register int i, r, g, b;
	int valdep;
	unsigned char *rline, *gline, *bline;
	zdepth *mlretz;		/* Z-buffer pointer */
	zdepth czd;
	double rczd;

	/* This is guaranteed to be a horizontal line from left to right */
	assert(svec.f.y == svec.t.y);	/* or my name...    */
	assert(svec.f.x <= svec.t.x);	/* isn't Joe Izusu. */

	r = pcol[0];
	g = pcol[1];
	b = pcol[2];
	(*meml) (svec.f.y - bandtop, svec.f.y, &rline, &mlretz);
	rline += svec.f.x;
	bline = ((gline = rline + bpcolour) + bpcolour);

	valdep = FALSE;
	for (i = svec.f.x; i <= svec.t.x; i++)
	{
#ifdef ISTATS
		ispixels++;
#endif
		if (!(valdep && pqinc))
		{
			rczd = 65535.0 - CurrentZ(i, svec.f.y);
			valdep = TRUE;
#ifdef ISTATS
			ishard++;
#endif
		}
		else
		{
			rczd += pqstep;
#ifdef ISTATS
			isinc++;
#endif
		}
		if (rczd < 0)
		{
			czd = 0;
			valdep = FALSE;
#ifdef ISTATS
			iscorr++;
#endif
		}
		else if (rczd > 65535.0)
		{
			czd = 65535L;
			valdep = FALSE;
#ifdef ISTATS
			iscorr++;
#endif
		}
		else
		{
			czd = rczd;
		}
		if (czd <= mlretz[i])
		{
			mlretz[i] = czd;
			*rline = r;
			*gline = g;
			*bline = b;
		}
		rline++;
		gline++;
		bline++;
	}
#endif
}

/*  VP256  --  Vector processing routine for 256 colour devices  */

static void vp256()
{
	register int i, colour;
	int valdep;
	unsigned char *lbuf;
	zdepth *mlretz;		/* Z-buffer pointer */
	zdepth czd;
	double rczd;

	/* This is guaranteed to be a horizontal line from left to right */
	assert(svec.f.y == svec.t.y);	/* or my name...    */
	assert(svec.f.x <= svec.t.x);	/* isn't Joe Izusu. */

	colour = pcol;
	(*meml) (svec.f.y - bandtop, svec.f.y, &lbuf, &mlretz);
	lbuf += svec.f.x;

	valdep = FALSE;
	for (i = svec.f.x; i <= svec.t.x; i++)
	{
#ifdef ISTATS
		ispixels++;
#endif
		if (!(valdep && pqinc))
		{
			rczd = 65535.0 - CurrentZ(i, svec.f.y);
			valdep = TRUE;
#ifdef ISTATS
			ishard++;
#endif
		}
		else
		{
			rczd += pqstep;
#ifdef ISTATS
			isinc++;
#endif
		}
		if (rczd < 0)
		{
			czd = 0;
			valdep = FALSE;
#ifdef ISTATS
			iscorr++;
#endif
		}
		else if (rczd > 65535.0)
		{
			czd = 65535L;
			valdep = FALSE;
#ifdef ISTATS
			iscorr++;
#endif
		}
		else
		{
			czd = rczd;
		}
		if (czd <= mlretz[i])
		{
			mlretz[i] = czd;
			*lbuf = colour;
		}
		lbuf++;
	}
}





/*  RASTPEXT  --  Calculate extents fields in polygon item  */

void rastpext(poly)
	struct spolygon *poly;
{
	int i, y;
	int ymax, ymin;

	ymin = HIGHSCRC;
	ymax = -1;
	for (i = 0; i < poly->npoints; i++)
	{
		ymin = min(ymin, (y = poly->pt[i].y));
		ymax = max(ymax, y);
	}
	poly->pymin = ymin;
	poly->pymax = ymax;
}

/* Take a polygon structure as input containing a clipped polygon in pixel
   coordinate space and output vectors to the subroutine address stored
   in VECTAKER. */

static void lflood(poly)
	struct spolygon *poly;
{
	scrcoord minint, maxint, thisint;
	struct edg edge[MAXVERT];
	scrcoord inter[MAXVERT];
	int i, j, k, l, m, n, nvert, nedge, nint, bymax, bymin, edgeon, horedge;
	scrcoord s, t, u, y;
	double msl, cin;
	point3d v1, v2, v3, vn;


	nvert = poly->npoints;

	assert(nvert <= MAXVERT);

	horedge = FALSE;	/* No horizontal edges yet */
	edgeon = FALSE;		/* Assume not edge-on */
	pqinc = FALSE;

	if (nvert == 1)
	{
#ifdef ISTATS
		isingpixel++;
#endif
		svec.f.y = svec.t.y = poly->pt[0].y;
		svec.f.x = svec.t.x = poly->pt[0].x;

		/*
		 * Fudge plane equation values to yield correct Z for this
		 * point.
		 */

		pqa = pqb = 0.0;
		pqc = 1.0;
		pqd = -Fp(poly->pt[0].z);
		assert(abs(CurrentZ(svec.f.x, svec.t.y) - poly->pt[0].z) < 1E-10);
		(*vectaker) ();
		return;
	};

/* Determine equation for plane containing the polygon.
	   Note that if we are passed a line segment (in other
           words a "polygon" with 2 vertices (nvert == 2)), the
	   following loop will never be executed, leaving pqa
	   set to zero.  This will set edgeon == TRUE in the
	   test below the loop, which will trigger scan conversion
           of the line as the only edge of the "polygon". */

	i = 0;
	pqa = 0.0;
	while (i < (nvert - 2))
	{
		pointget(v1, (double) poly->pt[i + 0].x - poly->pt[i + 1].x,
			 (double) poly->pt[i + 0].y - poly->pt[i + 1].y,
			 (double) poly->pt[i + 0].z - poly->pt[i + 1].z);
		pointget(v2, (double) poly->pt[i + 2].x - poly->pt[i + 1].x,
			 (double) poly->pt[i + 2].y - poly->pt[i + 1].y,
			 (double) poly->pt[i + 2].z - poly->pt[i + 1].z);
		veccross(vn, v1, v2);
		pointget(v3, (double) poly->pt[i + 1].x,
			 (double) poly->pt[i + 1].y,
			 (double) poly->pt[i + 1].z);

		pqa = vecmag(vn);
		if (pqa != 0.0)
			break;
		i++;
	}

	if (pqa == 0.0)
	{
/* Normal vector is undefined.  This usually indicates that the
	      polygon is degenerate as a result of being so small when
	      projected that different model vertices project onto the
	      same point.  If we totally failed to determine a normal
	      above, render the polygon in wire frame (which for a tiny
	      polygon generally does the right thing. */

/* fprintf(stderr, "Zero normal vector.\n"); */
#ifdef ISTATS
		iszeronorm++;
#endif
		edgeon = TRUE;
	}
	else
	{
#ifdef NORMIT
		vecscal(vn, vn, 1.0 / pqa);	/* Normalise normal vector */
#endif
		pqa = vn[0];
		pqb = vn[1];
		pqc = vn[2];
		pqd = -vecdot(v3, vn);
	}

	edgeon = edgeon || (pqc == 0.0);
	/* edgeon = TRUE; *//**************/

	/*
	 * Scan vertex table and calculate maximum and minimum Y extent of
	 * each edge.  Build edge summary table.
	 */

	for (i = 0, j = 0; i < ((nvert == 2) ? 1 : nvert); i++)
	{
		k = (i + 1) % nvert;
		if (vertex[i].y != vertex[k].y)
		{		/* Ignore horizontal lines */
			edge[j].ymax = max(vertex[i].y, vertex[k].y);
			edge[j].ymin = min(vertex[i].y, vertex[k].y);
			if (edge[j].ymax == vertex[i].y)
			{
				l = i;
				m = k;
			}
			else
			{
				l = k;
				m = i;
			}
			edge[j].xpos = vertex[l].x;
			edge[j].xend = vertex[m].x;
			edge[j].zpos = vertex[l].z;
			edge[j].zend = vertex[m].z;
			edge[j].slope = ((double) vertex[m].x - vertex[l].x) /
				((double) vertex[m].y - vertex[l].y);
			j++;
		}
		else
		{

			/*
			 * Horizontal edge.  Ignore unless we're scan
			 * converting edges.  Remember that we saw a
			 * horizontal edge so that we make the horizontal
			 * edge suppression test below.
			 */

			horedge = TRUE;	/* Remember horizontal edge */
			if (edgeon)
			{
				edge[j].ymax = edge[j].ymin = vertex[i].y;
				if (vertex[i].x < vertex[k].x)
				{
					edge[j].xpos = vertex[i].x;
					edge[j].xend = vertex[k].x;
					edge[j].zpos = vertex[i].z;
					edge[j].zend = vertex[k].z;
				}
				else
				{
					edge[j].xpos = vertex[k].x;
					edge[j].xend = vertex[i].x;
					edge[j].zpos = vertex[k].z;
					edge[j].zend = vertex[i].z;
				}
				j++;
			}
		}
	}
	nedge = j;

#ifdef DEBUG
	printf("\nVertices:");
	for (i = 0; i < nvert; i++)
		printf("\n  %d,%d  ", vertex[i].x, vertex[i].y);
	printf("\nEdges:");
	for (i = 0; i < nedge; i++)
		printf(
		"\n  ymax=%d  ymin=%d  xpos=%d  slope=%9.4f  slantwid=%d\n",
		   edge[i].ymax, edge[i].ymin, edge[i].xpos, edge[i].slope);
	printf("\nY range:  %d - %d", gymin, gymax);
#endif

	bymax = min(poly->pymax, bandbot - 1);
	bymin = max(poly->pymin, bandtop);

	/* Loop over Y indices of lines in this band. */

	for (y = bymax; y >= bymin; y--)
	{

		assert(y < bandbot);


		/*
		 * If the polygon is edge-on with respect to the screen, scan
		 * convert it as the series of lines traced out by its edges.
		 */

		if (edgeon)
		{
#ifdef ISTATS
			isedgeon++;
#endif
			svec.f.y = svec.t.y = y;
			for (j = 0; j < nedge; j++)
			{

				/*
				 * Does this edge intersect the current scan
				 * line?
				 */

				if (y <= edge[j].ymax && y >= edge[j].ymin)
				{

					/*
					 * Yes.  Is this edge more horizontal
					 * than vertical.
					 */

					if ((edge[j].ymax - edge[j].ymin) <=
					    abs(edge[j].xpos - edge[j].xend))
					{

						/*
						 * Yes, it's more horizontal.
						 * Derive coefficients to
						 * express its Z depth in
						 * terms of the X co-ordinate
						 * along the scan line.
						 */

						svec.f.x = edge[j].xpos;
						svec.t.x = edge[j].xend;
						if (svec.f.x == svec.t.x)
						{
							/*
							 * Just a single dot.
							 * Force Z depth to
							 * minimum value of
							 * vector parallel to
							 * Z axis.
							 */
							pqa = 0.0;
							pqd = -Fp(min(edge[j].zpos, edge[j].zend));
						}
						else
						{
							pqa = (Fp(edge[j].zend) - Fp(edge[j].zpos)) /
								(edge[j].xend - edge[j].xpos);
							pqd = -(Fp(edge[j].zpos) + edge[j].xpos * pqa);
						}
						pqb = 0.0;
						pqc = 1.0;
						if (edge[j].ymax == edge[j].ymin)
						{

							/*
							 * Line is stone
							 * horizontal.
							 * Dispose of it in
							 * one swell foop.
							 */

							svec.f.x = edge[j].xpos;
							svec.t.x = edge[j].xend;
						}
						else
						{


							msl = Fp(edge[j].xpos - edge[j].xend) /
								Fp(edge[j].ymax - edge[j].ymin);
							cin = edge[j].xpos - msl * edge[j].ymax;


							svec.f.x = msl *
								((y == edge[j].ymin) ? y : (y - 0.5)) +
								cin;
							svec.t.x = msl *
								((y == edge[j].ymax) ? y : (y + 0.5)) +
								cin;
							if (svec.f.x > svec.t.x)
							{
								t = svec.f.x;
								svec.f.x = svec.t.x;
								svec.t.x = t;
							}
						}
					}
					else
					{

						/*
						 * No, the vector has
						 * vertical extent.  Derive
						 * coefficients to generate
						 * the Z depth along the edge
						 * from the scan line
						 * co-ordinate.
						 */

						if (y == edge[j].ymin)
							svec.f.x = edge[j].xend;
						else
							svec.f.x = edge[j].xpos -
								Fix(edge[j].slope * (edge[j].ymax - y));
						svec.t.x = svec.f.x;
						pqa = 0.0;
						pqb = (Fp(edge[j].zend) - Fp(edge[j].zpos)) /
							(edge[j].ymin - edge[j].ymax);
						pqc = 1.0;
						pqd = -(Fp(edge[j].zpos) + edge[j].ymax * pqb);
					}
					(*vectaker) ();
				}
			}
			continue;
		}


		pqinc = TRUE;
		pqstep = pqa / pqc;

		nint = 0;


		minint = HIGHSCRC;
		maxint = 0;
		for (j = 0; j < nedge; j++)
			if (y <= edge[j].ymax && y >= edge[j].ymin)
			{
				if (y == edge[j].ymin)
					thisint = inter[nint] = edge[j].xend;
				else
					thisint = inter[nint] =
						edge[j].xpos - Fix(edge[j].slope * (edge[j].ymax - y));
				if (thisint <= minint)
				{
					minint = thisint;
				}
				if (thisint >= maxint)
				{
					maxint = thisint;
				}
				nint++;
			}

		if (nint > 1)
		{		/* Skip if < 2 intersections  */

			/* Sort intersections  */

			for (i = 0; i < (nint - 1); i++)
				for (j = i + 1; j < nint; j++)
					if (inter[i] >= inter[j])
					{
						s = inter[i];
						inter[i] = inter[j];
						inter[j] = s;
					}

#ifdef DEBUG
			printf("\nIntersections at y=%d: ", y);
			for (i = 0; i < nint; i++)
				printf("  %d", inter[i]);
#endif


			if (nint > 2)
				for (i = 0; i < (nint - 1); i++)
					if (inter[i] == inter[i + 1])
					{
						for (j = 0; j < nvert; j++)	/* Find vertex which did
										 * it */
							if (vertex[j].y == y && vertex[j].x == inter[i])
								break;
						s = vertex[(j == 0) ? nvert - 1 : j - 1].y;
						t = vertex[(j + 1) % nvert].y;
						u = vertex[j].y;
						if (!((u > s && u > t) || (u < s && u < t)))
						{
							for (j = i + 1; j < (nint - 1); j++)
								inter[j] = inter[j + 1];
							nint--;
#ifdef DEBUG
							printf("\nDuplicate removal:  ");
							for (l = 0; l < nint; l++)
								printf("  %d", inter[l]);
#endif
						}
					}


			if (horedge && (nint > 2))
			{
				for (i = 0; i < nvert; i++)
				{
					if ((y == vertex[i].y) &&
					    (y == vertex[(i + 1) % nvert].y))
					{


						l = vertex[i].x;
						m = vertex[(i + 1) % nvert].x;


						for (k = 0; k < nint - 1; k++)
						{
							if (((l == inter[k]) && (m == inter[k + 1])) ||
							    ((m == inter[k]) && (l == inter[k + 1])))
							{
								if ((k & 1) == 0)
								{

									s = vertex[((i + 2) % nvert)].y;
									t = vertex[((i + nvert) - 1) % nvert].y;
									if (((s <= y) && (t >= y)) ||
									    ((s >= y) && (t <= y)))
									{

										for (n = k + 1; n < (nint - 1); n++)
											inter[n] = inter[n + 1];
										nint--;
#ifdef DEBUG
										printf(
										       "\nHorizontal edge case 1%c: ",
										       s < y ? 'a' : 'b');
										for (l = 0; l < nint; l++)
											printf("  %d", inter[l]);
#endif
									}
								}
								else
								{	/* ((k & 1) == 1) */

									for (n = k; n < (nint - 1); n++)
										inter[n] = inter[n + 1];
									nint--;
#ifdef DEBUG
									printf(
									       "\nHorizontal edge case 2: ");
									for (l = 0; l < nint; l++)
										printf("  %d", inter[l]);
#endif
								}
							}
						}
					}
				}
			}

			/* Draw lines between pairs of intersections  */
			for (i = 0; i < (nint - 1); i += 2)
			{
				svec.f.y = svec.t.y = y;
				svec.f.x = inter[i];
				svec.t.x = inter[i + 1];
				(*vectaker) ();
			}
		}
	}
}
