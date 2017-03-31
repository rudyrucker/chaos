/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal code of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.							     *
*									     *
******************************************************************************/


#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "game.h"


#define DEGREES(n) (((n)*360.0)/(2*M_PI))
#define RADIANS(n) ((n)*(2*M_PI)/360.0)

static numbertool cmap_tool;
static numbertool param_tools[6];
static numbertool weight_tool;
static rect displaymodeR;
static rect addmapR, deletemapR;
static numbertool increment_tool;


/* Tweaker controller for Barnsley. */

static char *tmsgs[] = {
	"Current Map ",
	"Z-rotation  ",
	"X-rotation  ",
	"X-scale     ",
	"Y-scale     ",
	"X-trans     ",
	"Y-trans     ",
	"Weight      ",
	"Increment   ",
	"Display Mode",
	"Add Map", "Delete Map",
	NULL

};

static char *t0msgs[] = {
	"X-rotation: ",
	"Y-rotation: ",
	"X-scale:    ",
	"Y-scale:    ",
	"X-trans:    ",
	"Y-trans:    ",
	"Weight:     "
};

static char *t1msgs[] = {
	"A:          ",
	"B:          ",
	"C:          ",
	"D:          ",
	"E:          ",
	"F:          ",
	"Weight:     ",
};

static char *t2msgs[] = {
	"Corner 1 X: ",
	"Corner 1 Y: ",
	"Corner 2 X: ",
	"Corner 2 Y: ",
	"Corner 3 X: ",
	"Corner 3 Y: ",
	"Weight:     ",
};


static double increment = 0.1;
double tweakparms[20];

#define OX tweakparms[1]
#define OY tweakparms[2]
#define UX tweakparms[3]
#define UY tweakparms[4]
#define VX tweakparms[5]
#define VY tweakparms[6]

int tweakmode = 1;
text_button tweakRects[20];
rect tweakFrames[20];
static char *modemsgs[] = {"Angles   ", "Matrices ", "Triangles"};
typedef struct
{
	rect R;
	rect mR;
	rect pR;
} pmr;

static pmr clicks[10];

static struct
{
	int n;
	fbarnmap fb;
	int map;
	double weight[MAXBARNMAPS];	/* this fucker is BIG! Eeep! */
} barnmap_stack[100], base_barnmap;

int barnmapsp = 0;

int BaseBarnmap(void)
{
	if (barnmapsp)
	{
		fBptr->n = base_barnmap.n;
		edmap = base_barnmap.map;
		fBptr->h[edmap] = base_barnmap.fb;
		memcpy(fBptr->weight, base_barnmap.weight,
		       fBptr->n * sizeof(double));
		barnmapsp = 0;
		return 1;
	}
	else
		return 0;
}


double round(double n)
{
	return floor(n * 1000.0 + .5) / 1000.0;
}


void PushBarnmap(void)
{
	if (barnmapsp < 99)
	{


		barnmap_stack[barnmapsp].fb = fBptr->h[edmap];
		barnmap_stack[barnmapsp].n = fBptr->n;
		memcpy(barnmap_stack[barnmapsp].weight, fBptr->weight,
		       fBptr->n * sizeof(double));
		barnmap_stack[barnmapsp].map = edmap;

		if (barnmapsp++ == 0)
			base_barnmap = barnmap_stack[0];


	}
	else
	{
		int i;

		for (i = 0; i < 99; i++)
			barnmap_stack[i] = barnmap_stack[i + 1];
		barnmap_stack[barnmapsp].fb = fBptr->h[edmap];
		barnmap_stack[barnmapsp].map = edmap;
		barnmap_stack[barnmapsp].n = fBptr->n;
		memcpy(barnmap_stack[barnmapsp].weight, fBptr->weight,
		       fBptr->n * sizeof(double));
	}

}

int PopBarnmap(void)
{
	if (barnmapsp)
	{
		fBptr->n = barnmap_stack[--barnmapsp].n;
		edmap = barnmap_stack[barnmapsp].map;
		fBptr->h[edmap] = barnmap_stack[barnmapsp].fb;
		memcpy(fBptr->weight, barnmap_stack[barnmapsp].weight,
		       fBptr->n * sizeof(double));
		return 1;
	}
	else
		return 0;
}

void ClearBarnmapStack(void)
{
	barnmapsp = 0;
}







void UpdateMapParam(int i)
{
	numbertool *p;
	double v;

	if (i == CURMAP)
	{
		p = &cmap_tool;
		v = edmap + 1;
	}
	else
	{
		if (i >= ZROT && i < ZROT + 6)
			p = &param_tools[i - ZROT];
		else if (i == WEIGHT)
			p = &weight_tool;
		else if (i == INCREMENT)
			p = &increment_tool;

		v = p->value;
	}

	PaintNumberBoxEntryPrecision(&p->TB, round(v), p->type, p->p1, p->p2);
}

void SetCurrentWeight(void)
{
	weight_tool.value = tweakparms[WEIGHT] = fBptr->weight[edmap];
}

void paintTweakBox(int i)
{
	rect R;
	char *p;
	numbertool *n;

	if (i < 6)
	{
		n = &param_tools[i];
		R = n->tR;
		if (tweakmode == 0 && i <= 1)
		{
			n->p1 = 6;
			n->p2 = 0;
		}
		else
		{
			n->p1 = 3;
			n->p2 = 3;
		}
		switch (tweakmode)
		{
		case 0:
			p = t0msgs[i];
			break;
		case 1:
			p = t1msgs[i];
			break;
		case 2:
			p = t2msgs[i];
			break;
		}
		JString(p, R.Xmin - 2, R.Ymin + 2, WHITE, BLUE, alignRight, alignTop);
		PaintNumberBoxEntryPrecision(&n->TB, n->value, n->type, n->p1, n->p2);
	}
	else
	{
		/* has to be WEIGHT */
		n = &weight_tool;
		/* Don't bother overwriting the "weight" word, not needed */
		PaintNumberBoxEntryPrecision(&n->TB, n->value, n->type, n->p1, n->p2);
	}
}


void paintAllBoxes(void)
{
	int i;

	for (i = 0; i < 7; i++)
		paintTweakBox(i);
}



void parms_to_tools(void)
{
	int i;

	for (i = 0; i < 6; i++)
		param_tools[i].value = tweakparms[ZROT + i];
	increment_tool.value = tweakparms[INCREMENT];
	weight_tool.value = tweakparms[WEIGHT];
	cmap_tool.value = edmap + 1;

}


void ConvertParamsBack(int n)
{
	fbarnmap *fb = &fBptr->h[n];
	double *fptr = &fb->a;
	int i;

	double a, b, c, d, e, f;
	double r, s, theta, phi;

	switch (tweakmode)
	{
	case 0:
		theta = RADIANS(tweakparms[ZROT]);
		phi = RADIANS(tweakparms[XROT]);
		r = tweakparms[XSCALE];
		s = tweakparms[YSCALE];
		e = tweakparms[XTRANS];
		f = tweakparms[YTRANS];

		a = r * cos(theta);
		c = r * sin(theta);

		b = -s * sin(phi);
		d = s * cos(phi);

		fb->a = a;
		fb->b = b;
		fb->c = c;
		fb->d = d;
		fb->e = e;
		fb->f = f;
		break;
	case 1:
		for (i = 0; i < 6; i++)
			fptr[i] = tweakparms[i + ZROT];
		break;
	case 2:
		fb->a = (UX - OX) / (fstarthix - fstartlox);
		fb->b = (VX - OX) / (fstarthiy - fstartloy);
		fb->c = (UY - OY) / (fstarthix - fstartlox);
		fb->d = (VY - OY) / (fstarthiy - fstartloy);
		fb->e = -fstartlox * fb->a - fstartloy * fb->b + OX;
		fb->f = -fstartlox * fb->c - fstartloy * fb->d + OY;
		break;
	}


	increment = tweakparms[INCREMENT];
	fBptr->weight[n] = tweakparms[WEIGHT];

	parms_to_tools();
}



void ConvertParams(int n)
{
	fbarnmap *fb = &fBptr->h[n];
	double r, s;
	double *fpd = &fb->a;
	int i;
	double t;
	double phi;

	switch (tweakmode)
	{
	case 0:

		tweakparms[XSCALE] = r = hypot(fb->a, fb->c);
		tweakparms[YSCALE] = s = hypot(fb->b, fb->d);

		t = (r == 0.0) ? 1.0 : fb->a / r;
		tweakparms[ZROT] = DEGREES(acos(t));
		if (fb->c < 0.0)
			tweakparms[ZROT] = -tweakparms[ZROT];

		t = (s == 0.0) ? 1.0 : -fb->b / s;
		phi = asin(t);
		if (fb->d < 0.0)
		{
			if (phi >= 0)
				phi = M_PI - phi;
			else
				phi = -M_PI - phi;
		}
		tweakparms[XROT] = DEGREES(phi);





		tweakparms[XTRANS] = fb->e;
		tweakparms[YTRANS] = fb->f;
		break;
	case 1:
		for (i = 0; i < 6; i++)
			tweakparms[ZROT + i] = fpd[i];
		break;

	case 2:
		/*
		 * hmmmm... we need to get the float equivalents of the
		 * barnimage of three corners of this fucker. hmmm.
		 */

		OX = fb->e + fb->a * fstartlox + fb->b * fstartloy;
		OY = fb->f + fb->c * fstartlox + fb->d * fstartloy;
		UX = fb->e + fb->a * fstarthix + fb->b * fstartloy;
		UY = fb->f + fb->c * fstarthix + fb->d * fstartloy;
		VX = fb->e + fb->a * fstartlox + fb->b * fstarthiy;
		VY = fb->f + fb->c * fstartlox + fb->d * fstarthiy;
		break;
	}
	tweakparms[INCREMENT] = increment;
	tweakparms[WEIGHT] = fBptr->weight[n];
	parms_to_tools();
}


void ReadjustWeights(void)
{
	/* First determine the excess. */
	double total = 0.0;
	int i;
	int livemaps = 0;
	double othertotals = 0.0;
	double excess;

	for (i = 0; i < fBptr->n; i++)
	{
		if (fBptr->weight[i] != 0.0)
		{
			total += fBptr->weight[i];
			livemaps++;
			if (i != edmap)
				othertotals += fBptr->weight[i];
		}
	}

	excess = total - 1.0;

	/* Now we need to fix 'em */
	for (i = 0; i < fBptr->n; i++)
		if (i != edmap && fBptr->weight[i] != 0.0)
			fBptr->weight[i] -= fBptr->weight[i] / othertotals * excess;

	/* Now create new cuts */
	weighttocuts(fBptr, Bptr);
}



rect bottom_rects[10];
rect left_arrow, right_arrow;

void clear_map_display(void)
{
	int height = 8 * FontHeight + 4;
	int row = sR.Ymax - height;

	blast_n(row * 80, (height + 1) * 40);
}

int FirstMap = 0;

void ShiftRight(void)
{
	if (FirstMap < fBptr->n - 6)
	{
		rect R1, R2;

		HideCursor();
		R1 = bottom_rects[1];
		R1.Xmax = bottom_rects[5].Xmax;
		R2 = bottom_rects[0];
		R2.Xmax = bottom_rects[4].Xmax;
		CopyBits(theBitmap, theBitmap, &R1, &R2, &R2, 0);
		FirstMap++;
		ShowOneValues(FirstMap + 5);
		ShowCursor();
	}
}

void ShiftLeft(void)
{
	if (FirstMap > 0)
	{
		rect R1, R2;

		HideCursor();
		R1 = bottom_rects[1];
		R1.Xmax = bottom_rects[5].Xmax;
		R2 = bottom_rects[0];
		R2.Xmax = bottom_rects[4].Xmax;
		CopyBits(theBitmap, theBitmap, &R2, &R1, &R1, 0);
		FirstMap--;
		ShowOneValues(FirstMap);
		ShowCursor();
	}
}


void ShowWeights(void)
{
	char tbuf[128];
	int height = 8 * FontHeight + 4;
	int row = sR.Ymax - height;

	int i;

	HideCursor();
	for (i = 0; i < 6 && (i + FirstMap) < fBptr->n; i++)
	{
		int col = bottom_rects[i].Xmin + 2;

		PenColor(BLACK);
		BackColor(LIGHTGRAY);
		TextAlign(alignLeft, alignTop);
		MoveTo(col, row + 7 * FontHeight + 1);
		sprintf(tbuf, "% 4.3f", fBptr->weight[i + FirstMap]);
		DrawString(tbuf);
	}
	ShowCursor();
}


void ShowOneValues(int j)
{
	char tbuf[128];
	int col = bottom_rects[j - FirstMap].Xmin + 2;
	double *ddd;
	int height = 8 * FontHeight + 4;
	int row = sR.Ymax - height;
	int i;

	if (j - FirstMap >= 6)
		return;

	if (j < FirstMap)
		return;

	HideCursor();

	ddd = (double *) &fBptr->h[j].a;
	MoveTo(col, row + 1);
	sprintf(tbuf, "Map %d ", j + 1);
	TextAlign(alignLeft, alignTop);
	BackColor(LIGHTGRAY);
	PenColor(BLACK);
	DrawString(tbuf);
	for (i = 0; i < 6; i++)
	{
		MoveTo(col, row + (i + 1) * FontHeight + 1);
		sprintf(tbuf, "% 4.3f", ddd[i]);
		DrawString(tbuf);
	}
	MoveTo(col, row + (i + 1) * FontHeight + 1);
	sprintf(tbuf, "% 4.3f", fBptr->weight[j]);
	DrawString(tbuf);
	ShowCursor();
}

void ShowNoValues(int n)
{
	/* just gray out the unused thingie? */
	rect R = bottom_rects[n];

	HideCursor();
	PenColor(LIGHTGRAY);
	BackColor(BLACK);
	FillRect(&R, 3);
	PenColor(BLACK);
	FrameRect(&R);
	ShowCursor();
}



void ShowValues(void)
{
	int i, j;
	int curmap = 0;


	for (i = 0, j = FirstMap; j < fBptr->n && i < 6; j++, i++)
	{
		ShowOneValues(j);
		curmap++;
	}
	for (; i < 6; i++)
	{
		ShowNoValues(i);
	}
}


void ShowAll(void)
{
	/*
	 * All of the active maps get little boxes on the bottom to show what
	 * their numbers are. What jolly fun. They won't all fit, of course,
	 * if there are more that a certain number. Hmm. We have 640 pixels
	 * wide to work in. How many can we fit across?
	 */

	/* To start, just draw some boxes. */

	int i;
	rect R;
	int height = 8 * FontHeight + 4;
	int row = sR.Ymax - height;
	int width = 616 / 7;
	int cx, cy;




	for (i = 0; i < 6; i++)
	{
		R.Xmin = 80 + i * width + 10;
		R.Xmax = R.Xmin + width - 1;
		R.Ymin = row;
		R.Ymax = sR.Ymax - 2;
		bottom_rects[i] = R;
      mainR[LEFTARROWBOX+1+i] = &bottom_rects[i];
	}
	HideCursor();
	/* Paint the big rect... */
	R = bottom_rects[0];
	R.Xmax = bottom_rects[5].Xmax;
	PenColor(LIGHTGRAY);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	for (i = 0; i < 6; i++)
		FrameRect(&bottom_rects[i]);



	R.Xmin = bottom_rects[0].Xmin;
	R.Xmax = R.Xmin + 12 + 4;
	R.Ymax = bottom_rects[0].Ymin - 2;
	R.Ymin = display_rect.Ymax + 2;


	left_arrow = R;
   mainR[LEFTARROWBOX] = &left_arrow;

	R.Xmax = bottom_rects[5].Xmax;
	R.Xmin = R.Xmax - 12 - 4;

	right_arrow = R;
   mainR[RIGHTARROWBOX] = &right_arrow;

	if (fBptr->n > 6)
	{

		PenColor(BUTTONBACK);
		PaintRect(&left_arrow);
		PaintRect(&right_arrow);
		PushButton(&left_arrow, false);
		PushButton(&right_arrow, false);
		PenColor(WHITE);
		BackColor(BLACK);
		RasterOp(zORz);
		TextAlign(alignLeft, alignMiddle);
		Centers(&left_arrow, &cx, &cy);
		MoveTo(left_arrow.Xmin + 2, cy);
		DrawChar(0x82);
		Centers(&right_arrow, &cx, &cy);
		MoveTo(right_arrow.Xmin + 2, cy);
		DrawChar(0x83);
		RasterOp(zREPz);

	}
	else
	{
		PenColor(BLUE);
		PaintRect(&left_arrow);
		PaintRect(&right_arrow);
	}

	ShowCursor();

	ShowValues();

}

pair corners[4];

typedef union {
   struct {
      unsigned code0: 1;
      unsigned code1: 1;
      unsigned code2: 1;
      unsigned code3: 1;
   } ocs;
   short outcodes;
} Outcode;

typedef struct {
   double Xmin,Ymin,Xmax,Ymax;
   } fRect;

#define X1 ep->Xmin
#define Y1 ep->Ymin
#define X2 ep->Xmax
#define Y2 ep->Ymax

#define XUL r->Xmin
#define YUL r->Ymin
#define XLR r->Xmax
#define YLR r->Ymax

static void SetOutcodes(Outcode *u,fRect *r,double x,double y)
{
   u->outcodes = 0;
   u->ocs.code0 = x < XUL;
   u->ocs.code1 = y < YUL;
   u->ocs.code2 = x > XLR;
   u->ocs.code3 = y > YLR;
}

static void Swap(short *pa,short *pb)
{
   int t = *pa;
   *pa = *pb;
   *pb = t;
}


static void fSwap(double *pa,double *pb)
{
   double t = *pa;
   *pa = *pb;
   *pb = t;
}

int Clip(fRect *eep,fRect *rr)
{
   Outcode ocu1,ocu2;
   int Inside,Outside;
   fRect RR;
   fRect EEEP;
   fRect *r = &RR;
   fRect *ep = &EEEP;

   RR.Xmin = rr->Xmin;
   RR.Xmax = rr->Xmax;
   RR.Ymin = rr->Ymin;
   RR.Ymax = rr->Ymax;

   
   EEEP.Xmin = eep->Xmin;
   EEEP.Xmax = eep->Xmax;
   EEEP.Ymin = eep->Ymin;
   EEEP.Ymax = eep->Ymax;

   SetOutcodes(&ocu1,r,X1,Y1);
   SetOutcodes(&ocu2,r,X2,Y2);

   Inside = ((ocu1.outcodes | ocu2.outcodes) == 0);
   Outside = ((ocu1.outcodes & ocu2.outcodes) != 0);

   while(!Outside && !Inside)
   {
      if (ocu1.outcodes == 0)
      {
         fSwap(&X1,&X2);
         fSwap(&Y1,&Y2);
         Swap(&ocu1.outcodes,&ocu2.outcodes);
      }

      if (ocu1.ocs.code0)
      {
         Y1 += (Y2-Y1)*(XUL-X1) / (X2 - X1);
         X1 = XUL;
      }

      else if (ocu1.ocs.code1)
      {
         X1 += (X2-X1)*(YUL-Y1) / (Y2-Y1);
         Y1 = YUL;
      }
      else if (ocu1.ocs.code2)
      {
         Y1 += (Y2-Y1)*(XLR-X1) / (X2-X1);
         X1 = XLR;
      }
      else if (ocu1.ocs.code3)
      {
         X1 += (X2-X1)*(YLR-Y1) / (Y2 - Y1);
         Y1 = YLR;
      }

      SetOutcodes(&ocu1,r,X1,Y1);
      Inside = ((ocu1.outcodes | ocu2.outcodes) == 0);
      Outside = ((ocu1.outcodes & ocu2.outcodes) != 0);
   }
   rr->Xmin = r->Xmin;
   rr->Xmax = r->Xmax;
   rr->Ymin = r->Ymin;
   rr->Ymax = r->Ymax;

   return Inside;
}

void fClippedLine(double x1,double y1,double x2,double y2,rect *R)
{
   fRect R2;
   fRect R3;
   R3.Xmin = R->Xmin;
   R3.Xmax = R->Xmax;
   R3.Ymin = R->Ymin;
   R3.Ymax = R->Ymax;

   R2.Xmin = x1;
   R2.Ymin = y1;
   R2.Xmax = x2;
   R2.Ymax = y2;
   if (Clip(&R2,&R3))
   {
      MoveTo((int)R2.Xmin,(int)R2.Ymin);
      LineTo((int)R2.Xmax,(int)R2.Ymax);
   }
}
fpair ffpixel(fpair *z)
{
	fpair fp;
	fp.x = minx + (z->x - flox) * maxx / fdeltax;
	fp.y = miny + (fhiy - z->y) * maxy / fdeltay;
   return fp;
}

fpair ffpixel_from_barnimage(fbarnmap *h,fpair *z)
{
   fpair t = fbarnimage(h,z);
   return ffpixel(&t);
}

void _fOutlineMap(int map, int color, int number)
{

	/*
	 * if numbers is negative, print 1 2 3 (maybe 4); otherwise print
	 * just the number.
	 */


	fpair tlc, trc, blc, brc;
	fpair fp, fq, fr;
   pair q;
	int corner;
	int do_corners = number < 0;
	rect R;

	HideCursor();
	tlc.x = fstartlox;
	tlc.y = fstarthiy;

	trc.x = fstarthix;
	trc.y = fstarthiy;

	brc.x = fstarthix;
	brc.y = fstartloy;

	blc.x = fstartlox;
	blc.y = fstartloy;



	corner = 0;
	fq = ffpixel_from_barnimage(&fBptr->h[map], &tlc);
	if (do_corners)
   {

      if (fq.x > 0 && fq.y > 0)
      {
         q.x = fq.x;
         q.y = fq.y;
      }
      else
         q.x = q.y = -1;
 		corners[corner++] = q;
   }
	PenColor(color);

	R = display_rect;
	InsetRect(&R, 1, 1);
	ClipRect(&R);
   fr = fq;

	if (!do_corners || tweakmode != 2)
	{
		fp = ffpixel_from_barnimage(&fBptr->h[map], &trc);
		if (do_corners)
      {

         if (fp.x > 0 && fp.y > 0)
         {
            q.x = fp.x;
            q.y = fp.y;
         }
         else
            q.x = q.y = -1;
 		   corners[corner++] = q;
      }
      fClippedLine(fp.x,fp.y,fr.x,fr.y,&R);
      fr = fp;
	}
	fp = ffpixel_from_barnimage(&fBptr->h[map], &brc);
	if (do_corners)
   {
      if (fp.x > 0 && fp.y > 0)
      {
         q.x = fp.x;
         q.y = fp.y;
      }
      else
         q.x = q.y = -1;
 		corners[corner++] = q;
   }
   fClippedLine(fp.x,fp.y,fr.x,fr.y,&R);
   fr = fp;

	fp = ffpixel_from_barnimage(&fBptr->h[map], &blc);
	if (do_corners)
   {
      if (fp.x > 0 && fp.y > 0)
      {
         q.x = fp.x;
         q.y = fp.y;
      }
      else
         q.x = q.y = -1;
 		corners[corner++] = q;
   }
   fClippedLine(fp.x,fp.y,fr.x,fr.y,&R);
   fClippedLine(fp.x,fp.y,fq.x,fq.y,&R);

	if (!Stamping)
	{
		if (do_corners)
		{
			int n = (tweakmode == 2) ? 3 : 4;
			int i;

			PenColor(WHITE);
			BackColor(BLACK);
			TextAlign(alignLeft, alignTop);
			RasterOp(zORz);
			for (i = 0; i < n; i++)
			{
            int X = corners[i].x;
            int Y = corners[i].y;
            if (XYInRect(X,Y,&R))
            {
   				MoveTo(corners[i].x, corners[i].y);
   				DrawChar('0' + (n - i));
            }
			}
			RasterOp(zREPz);
		}
		else
		{
			if (XYInRect(q.x, q.y, &display_rect))
			{
				char tbuf[10];

				MoveTo(q.x, q.y);
				BackColor(BLACK);
				RasterOp(zORz);
				TextAlign(alignLeft, alignTop);
				sprintf(tbuf, "%d", number + 1);
				DrawString(tbuf);
				RasterOp(zREPz);
			}
		}
	}
	ClipRect(&sR);
	ShowCursor();

}

void fOutlineCurrentMap(void)
{
	_fOutlineMap(edmap, 15, -1);
}


void OutlineCurrentMap(void)
{
	ConvertParams(edmap);
	ClipRect(&display_rect);
	HideCursor();

	if (!realmode)
		_setfwindow();
	fOutlineCurrentMap();
	ShowCursor();
}





int process_minus_key(click)
{
	int changed = 0;
	double mmin, mmax;
	mmin = -cornermax;
	mmax = cornermax;
	if (tweakmode == 0)
	{
		if (click < ZROT+2)
		{
			mmin = -180.0;
			mmax = 180.0;
		}
	}

   if (click == DISPLAYMODE)
   {
      tweakmode -= 2;
      if (tweakmode < 0)
         tweakmode += 3;
      change_display_mode();
      return true;
   }

   if (click == KILLMAP || click == ADDMAP)
      return false;
	if (click == CURMAP)
	{
		edmap--;
		if (edmap < 0)
			edmap = fBptr->n - 1;
   	parms_to_tools();
      return true;
	}
	else if (!tweakmode && (click == XSCALE || click == YSCALE))
	{
		tweakparms[click] -= increment;
		if (tweakparms[click] < 0.0)
			tweakparms[click] = 0.0;
		changed = 1;

	}
	else if (click == INCREMENT)
	{
		if (increment > 0.001)
			increment = increment_tool.value = (tweakparms[click] /= 10.0);
      return true;
	}
	else if (click == WEIGHT)
	{
		changed = 1;
		tweakparms[click] -= increment;
		if (tweakparms[click] < 0)
			tweakparms[click] = 0;
      LimitMouseRect(&weight_tool.mR);
	}
	else if (click >= 0 && (tweakparms[click] > mmin || (tweakmode == 0 && click < ZROT + 2)))
	{
		if (tweakmode == 0 && click < ZROT + 2)
		{
			tweakparms[click] -= 10 * increment;
			if (tweakparms[click] <= -180.0)
				tweakparms[click] += 360.0;
		}
		else
			tweakparms[click] -= increment;
		changed = 1;
      tweakparms[click] = max(tweakparms[click],mmin);
      tweakparms[click] = min(tweakparms[click],mmax);
      LimitMouseRect(&param_tools[click-ZROT].mR);
	}

	parms_to_tools();
	if (changed)
		PushBarnmap();

   return changed;
}
int process_plus_key(int click)
{
	int changed = 0;
	double mmin, mmax;
	mmin = -cornermax;
	mmax = cornermax;
	if (tweakmode == 0)
	{
		if (click < ZROT+2)
		{
			mmin = -180.0;
			mmax = 180.0;
		}
	}

   if (click == DISPLAYMODE)
   {
      change_display_mode();
      return true;
   }
   if (click == KILLMAP || click == ADDMAP)
      return false;


	if (click == CURMAP)
	{
		edmap++;
		if (edmap > fBptr->n - 1)
			edmap = 0;
   	parms_to_tools();
      return true;
	}
	else if (click == INCREMENT)
	{
		if (increment < 1)
			increment = increment_tool.value = (tweakparms[click] *= 10.0);
      return true;
	}
	else if (click == WEIGHT)
	{
		changed = 1;
		tweakparms[click] += increment;
		if (tweakparms[click] > 1.0)
			tweakparms[click] = 1.0;
      LimitMouseRect(&weight_tool.pR);
	}
	else if (click >= 0 && (tweakparms[click] < mmax || (tweakmode == 0 && click < ZROT + 2)))
	{
		changed = 1;
		if (tweakmode == 0 && click < ZROT + 2)
		{
			tweakparms[click] += 10 * increment;
			if (tweakparms[click] > 180.0)
				tweakparms[click] -= 360.0;
		}
		else
			tweakparms[click] += increment;

      tweakparms[click] = max(tweakparms[click],mmin);
      tweakparms[click] = min(tweakparms[click],mmax);
      LimitMouseRect(&param_tools[click-ZROT].pR);
	}
	parms_to_tools();
	if (changed)
		PushBarnmap();
   return changed;
}

void reawaken(int click)
{
	/*
	 * Now convert all the fuckers back to a b c d e f, and repaint the
	 * sonofabitch.
	 */
	ConvertParamsBack(edmap);
	if (click == WEIGHT)
		ReadjustWeights();
	UpdateMapParam(click);
	_activate(fBptr);
	OutlineCurrentMap();

	/* And repaint the dickfor. */
	drawcursor();
}
void awaken(void)
{
	ConvertParams(edmap);
	_activate(fBptr);
	OutlineCurrentMap();
	UpdateAllParams(edmap);
}
double cornermax = 100.0;
void CheckCornerDrags(int X, int Y)
{
	/*
	 * If we are within 20 pixels of a corner, drag it to the new place.
	 * Cool, huh?
	 */

	int i;
	int n = (tweakmode == 2) ? 3 : 4;
	int closest = -1;
	double best_distance = 9999.999;
	fpair newfp;

	if (tweakmode != 2)
		return;

	for (i = 0; i < n; i++)
	{
		double x, y, distance;

		x = (double) corners[i].x;
		y = (double) corners[i].y;

		distance = hypot(x - X, y - Y);
		if (distance < best_distance)
		{
			closest = i;
			best_distance = distance;
		}
	}

	/* Hot shit, we've found a point to move. Hm. How? */
	/* Shit, I dunno.... */
	/* Ah, ok... */
	/*
	 * We need to set the new point, and convert it back to the right
	 * format, then do a ConvertBack.
	 */

	/* hmm? */

	if (closest != -1 && best_distance > 1.0)
	{
		pair P;
		int which = 2 - closest;

		P.x = X;
		P.y = Y;

		PushBarnmap();

		newfp = antifpixel(&P);

		/*
		 * We limit the fuckers. If anything has an abs greater then
		 * cornermax, puke it.
		 */

		if (newfp.x > cornermax)
			newfp.x = cornermax;
		if (newfp.x < -cornermax)
			newfp.x = -cornermax;

		if (newfp.y > cornermax)
			newfp.y = cornermax;
		if (newfp.y < -cornermax)
			newfp.y = -cornermax;


		/* OK, set the new point, and wahootie away! */
		tweakparms[2 * which + 1] = newfp.x;
		tweakparms[2 * which + 2] = newfp.y;

		ConvertParamsBack(edmap);

		_activate(fBptr);
		if (tweaking)
		{
			UpdateMapParam(2 * which + ZROT);
			UpdateMapParam(2 * which + ZROT + 1);
			ShowOneValues(edmap);
		}
		if (tweaking || triangle_display_mode)
			OutlineCurrentMap();
	}

}


void zap_a_map(void)
{
	int i;

	PushCursorPosition();
	invert_main_item(current_main_item);
	if (fBptr->n > 2)
	{
		char tbuf[128];

		sprintf(tbuf, "Kill map %d: Are you sure?", edmap + 1);
		if (cancel_ok_msg(tbuf))
		{
         int equalize = (fBptr->weight[edmap] != 0.0);

			for (i = edmap; i < fBptr->n; i++)
				fBptr->h[i] = fBptr->h[i + 1];
			/* set all the weights to equal... sorry! */
			fBptr->n--;
			if (equalize)
         {
            for (i = 0; i < fBptr->n; i++)
			   	fBptr->weight[i] = 1.0 / fBptr->n;
         }
			_activate(fBptr);
			if (edmap >= fBptr->n)
				edmap = fBptr->n - 1;
			UpdateAllParams(edmap);
			FirstMap = 0;
			ShowAll();
			OutlineCurrentMap();
         while(current_main_item < RIGHTARROWBOX &&
               current_main_item > LEFTARROWBOX + fBptr->n)
                  current_main_item--;
      }
	}
	invert_main_item(current_main_item);
	PopCursorPosition();
}

void PaintDisplayMode(void)
{
	char tbuf[128];

	sprintf(tbuf, "Display mode: %s", modemsgs[tweakmode]);
	PaintRadioButton(&displaymodeR, current_main_item == TWEAKBASE+27, false, tbuf);
}

void change_display_mode(void)
{
	tweakmode += 1;
	if (tweakmode == 3)
		tweakmode = 0;

	PaintDisplayMode();
	if (tweakmode == 2)
		TriangleCursor();
	else
		ArrowCursor();
	ConvertParams(edmap);
	paintAllBoxes();
	_activate(fBptr);
	OutlineCurrentMap();

}

void tw_add_new_barnmap(void)
{
   edmap = min(fBptr->n,MAXBARNMAPS-1);
   add_new_barnmap();
   if (tweaking)
   {
		ShowAll();
		parms_to_tools();
		UpdateAllParams(edmap);
      if (current_main_item >=LEFTARROWBOX && current_main_item <=RIGHTARROWBOX)
         current_main_item = -1;
   }
}

int process_main_click(int click)
{
	/* this is where we do the magick numeric entry shit */

	int changed = 0;
	double td;
	int wfn = 0;
	double mmin, mmax;

	mmin = -cornermax;
	mmax = cornermax;

	if (click <= YTRANS && click >= ZROT)
	{
		if (tweakmode == 0)
		{
			if (click < ZROT+2)
			{
				mmin = -180.0;
				mmax = 180.0;
			}
		}
		td = tweakparms[click];
		if (GetNumber(&tweakRects[click], &td, GS_FLOAT, mmin, mmax))
		{
			tweakparms[click] = td;
			parms_to_tools();
			changed = 1;
		}
		else
      {
			UpdateMapParam(click);
         invert_tweak_item(CURMAP + click*3);
      }

		wfn = 1;
	}
	else if (click == WEIGHT)
   {
		WeightEditor();
//      invert_tweak_item(CURMAP + click * 3);
      wfn = 1;
   }
	else if (click == CURMAP)
	{
		bump_edmap();
		wfn = 1;
	}
	else if (click == ADDMAP)
	{
		tw_add_new_barnmap();
		wfn = 1;
	}
	else if (click == KILLMAP && fBptr->n > 2)
		zap_a_map(), wfn = 1;
	else if (click == DISPLAYMODE)
		change_display_mode(), wfn = 1;

	else if (click == INCREMENT)
	{
		/* just rotate through the things... */
		if (increment == 1.000)
			increment = .001;
		else
			increment *= 10.0;

		increment_tool.value = tweakparms[INCREMENT] = increment;
		UpdateMapParam(INCREMENT);
      invert_tweak_item(CURMAP + click * 3);
		wfn = 1;
	}

	if (changed)
	{
		PushBarnmap();
		ConvertParamsBack(edmap);
		reawaken(click);
      invert_tweak_item(CURMAP + click * 3);
	}
	if (wfn)
		WaitForNothing();

   return changed;
}

static void check_plus_minus(numbertool * n, int X, int Y, int *mc, int *pc, int *c, int i)
{
	if (XYInRect(X, Y, &n->mR))
		*mc = *c = i;
	else if (XYInRect(X, Y, &n->pR))
		*pc = *c = i;
}


int CheckTweakClicks(event * e2)
{
	/*
	 * This always returns 0, but if necessary, the tweak work is done
	 * here.
	 */

	int i;
	int X;
	int Y;
	int mainclick;
	int minusclick;
	int plusclick;
	int click;
   event e;
   int button;

   e = *e2;
   mainclick = minusclick = click = plusclick = -1;
   button = (e.State & 0x700) >> 8;
   X = e.CursorX;
   Y = e.CursorY;

	if ((triangle_editing_mode || tweaking) && XYInRect(X, Y, &display_rect))
		CheckCornerDrags(X, Y);
	else if (tweaking)
	{
      while(button == swLeft)
      {

		   /* check to see if it is one of the bottom rects. */
		   for (i = 0; i < 6 && i < fBptr->n; i++)
		   {
			   if (XYInRect(X, Y, &bottom_rects[i]))
			   {
				   /* change the curmap */
				   edmap = i + FirstMap;
				   awaken();
               WaitForNothing();
			   }
		   }

		   if (fBptr->n > 6)
		   {
			   if (XYInRect(X, Y, &left_arrow))
            {
				   ShiftLeft();
            }
			   else if (XYInRect(X, Y, &right_arrow))
            {
				   ShiftRight();
            }
		   }



		   for (i = 0; tmsgs[i]; i++)
		   {
			   if (XYInRect(X, Y, &tweakRects[i].nR))
			   {
				   mainclick = click = i;
				   break;
			   }
			   if (i == CURMAP)
				   check_plus_minus(&cmap_tool, X, Y, &minusclick, &plusclick, &click, i);
			   else if (i >= ZROT && i < ZROT + 6)
				   check_plus_minus(&param_tools[i - ZROT], X, Y, &minusclick, &plusclick, &click, i);
			   else if (i == INCREMENT)
				   check_plus_minus(&increment_tool, X, Y, &minusclick, &plusclick, &click, i);
			   else if (i == WEIGHT)
				   check_plus_minus(&weight_tool, X, Y, &minusclick, &plusclick, &click, i);

            if (click != -1)
               break;

		   }

		   if (click == -1)
         {
            LimitMouseRect(&sR);
			   return 0;
         }



		   if (mainclick != -1)
			   process_main_click(click);
		   else if (minusclick != -1)
         {
			   if (!process_minus_key(click))
            {
               LimitMouseRect(&sR);
               return 0;
            }
         }
		   else if (plusclick != -1)
         {
			   if (!process_plus_key(click))
            {
               LimitMouseRect(&sR);
               return 0;
            }
         }
		   if (click == CURMAP)
		   {
			   /*
			   * OK, we have a new edmap; we need to read in all of
			   * the new fucker and use him instead.
			   */
			   if (edmap < 0)
				   edmap = fBptr->n - 1;
			   if (edmap > fBptr->n - 1)
				   edmap = 0;
			   awaken();
		   }
		   else if (click == INCREMENT)
		   {
            UpdateMapParam(click);
		   }
		   else if (click != ADDMAP && click != KILLMAP && mainclick == -1)
		   {
			   ConvertParamsBack(edmap);	/* make sure they match? */
			   reawaken(click);
            if (((current_main_item - TWEAKBASE) % 3) == 0)
               invert_tweak_item(current_main_item - TWEAKBASE);

		   }


		   /* Wait for an upclick on some of them. */
		   if (click == INCREMENT || click == CURMAP || click == DISPLAYMODE
		      || click == ADDMAP || click == KILLMAP)
		   {
			   WaitForNothing();
		   }

		   if (click == WEIGHT)
			   ShowValues();
		   else if (click != INCREMENT && click != DISPLAYMODE && click != CURMAP && click != -1)
			   ShowOneValues(edmap);
         PeekEvent(1,&e);
         button = (e.State & 0x700) >> 8;
         X = e.CursorX;
         Y = e.CursorY;
	   }
   }

   LimitMouseRect(&sR);
	return 0;
}


void invert_tweak_item(int n)
{
	int row = n / 3;
	int col = n % 3;
	rect *R = NULL;
	text_button *TB = NULL;

	if (n < 27)
	{
		numbertool *p;

		switch (row)
		{
		case 0:
			p = &cmap_tool;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			p = &param_tools[row - 1];
			break;
		case 7:
			p = &weight_tool;
			break;
		case 8:
			p = &increment_tool;
			break;
		}

		switch (col)
		{
		case 0:
			TB = &p->TB;
			break;
		case 1:
			R = &p->mR;
			break;
		case 2:
			R = &p->pR;
			break;
		}
	}
	if (n == 27)
		R = &displaymodeR;
	else if (n == 28)
		R = &addmapR;
	else if (n == 29)
		R = &deletemapR;

	if (R)
		invert_item_rect(R);
	else if (TB)
		InvertInsides(TB);
}


void UpdateAllParams(int edmap)
{
	int i;

	ConvertParams(edmap);
	PaintNumberBoxEntryPrecision(&cmap_tool.TB, cmap_tool.value, GS_UNSIGNED, 4, 3);

	for (i = 0; i < 6; i++)
	{
		numbertool p = param_tools[i];

		PaintNumberBoxEntryPrecision(&p.TB, round(p.value), p.type, p.p1, p.p2);
	}

	PaintNumberBoxEntryPrecision(&weight_tool.TB, round(weight_tool.value), weight_tool.type,
				     weight_tool.p1, weight_tool.p2);
	PaintNumberBoxEntryPrecision(&increment_tool.TB, round(increment_tool.value), increment_tool.type,
				     increment_tool.p1, increment_tool.p2);

	if (current_main_item >= TWEAKBASE + CURMAP && current_main_item < TWEAKBASE + CURMAP + 27 &&
	    ((current_main_item - (CURMAP + TWEAKBASE)) % 3) == 0)
		invert_tweak_item(current_main_item - TWEAKBASE);
}






void tweak(int n)
{
	int row;
	int i;
	rect R;
	rect R1;
	rect R2;
	int cx, cy;

	HideCursor();

	R.Xmin = display_rect.Xmax + 1;
	R.Xmax = sR.Xmax;
	R.Ymin = 0;
	R.Ymax = display_rect.Ymax;
	BackColor(BLUE);
	RasterOp(zREPz);
	EraseRect(&R);
	R.Xmin = minx;
	R.Ymin = display_rect.Ymax + 1;
	R.Ymax = sR.Ymax;
	EraseRect(&R);


	R.Xmin = display_rect.Xmax + 2;
	R.Xmax = sR.Xmax;

	R.Ymin = 0;
	R.Ymax = 16 * FontHeight;

	RasterOp(zREPz);
	PenColor(MENUBACK);
	PaintRect(&R);

	ConvertParams(n);

	row = 4;

	R1.Xmin = R.Xmin + 2;
	R1.Xmax = R.Xmax - 2;
	R1.Ymin = row;
	R1.Ymax = row + FontHeight + 4;
	Centers(&R1, &cx, &cy);

	cmap_tool.tR = R1;
	cmap_tool.tR.Xmin = cx + 2;
	cmap_tool.value = edmap + 1;
	cmap_tool.type = GS_UNSIGNED;
	CreateNumberTool(&cmap_tool);
	tweakRects[CURMAP] = cmap_tool.TB;
	R2 = cmap_tool.mR;
	R2.Xmax = cmap_tool.pR.Xmax;
	clicks[CURMAP].R = R2;
	clicks[CURMAP].mR = cmap_tool.mR;
	clicks[CURMAP].pR = cmap_tool.pR;

   mainR[TWEAKBASE] = (rect *)&cmap_tool.TB;
   mainR[TWEAKBASE+1] = &cmap_tool.mR;
   mainR[TWEAKBASE+2] = &cmap_tool.pR;




	JString("Current Map:", cx, R1.Ymin + 2, WHITE, BLUE, alignRight, alignTop);

	for (i = 0; i < 6; i++)
	{
		char *p;

		OffsetRect(&R1, 0, 3 * FontHeight / 2);
		switch (tweakmode)
		{
		case 0:
			p = t0msgs[i];
			break;
		case 1:
			p = t1msgs[i];
			break;
		case 2:
			p = t2msgs[i];
			break;
		}
		JString(p, cx, R1.Ymin + 2, WHITE, BLUE, alignRight, alignTop);
		if (tweakmode == 0 && i <= 1)
		{
			param_tools[i].p1 = 6;
			param_tools[i].p2 = 0;
		}
		else
		{
			param_tools[i].p1 = 4;
			param_tools[i].p2 = 3;
		}
		param_tools[i].value = tweakparms[i + 1];
		param_tools[i].type = GS_FLOAT;
		param_tools[i].tR = R1;
		param_tools[i].tR.Xmin = cx + 2;
		CreateNumberTool(&param_tools[i]);
		tweakRects[ZROT + i] = param_tools[i].TB;
		R2 = param_tools[i].mR;
		R2.Xmax = param_tools[i].pR.Xmax;
		clicks[i + ZROT].R = R2;
		clicks[i + ZROT].mR = param_tools[i].mR;
		clicks[i + ZROT].pR = param_tools[i].pR;
      mainR[TWEAKBASE+(i+ZROT)*3] = (rect *)&param_tools[i].TB;
      mainR[TWEAKBASE+(i+ZROT)*3 + 1] = &param_tools[i].mR;
      mainR[TWEAKBASE+(i+ZROT)*3 + 2] = &param_tools[i].pR;
	}

	OffsetRect(&R1, 0, 3 * FontHeight / 2);
	weight_tool.tR = R1;
	weight_tool.tR.Xmin = cx + 2;
	JString(t0msgs[6], cx, R1.Ymin + 2, WHITE, BLUE, alignRight, alignTop);
	weight_tool.p1 = 4;
	weight_tool.p2 = 3;
	weight_tool.value = tweakparms[i + 1];
	weight_tool.type = GS_FLOAT;
	CreateNumberTool(&weight_tool);
	tweakRects[WEIGHT] = weight_tool.TB;
	R2 = weight_tool.mR;
	R2.Xmax = weight_tool.pR.Xmax;
	clicks[WEIGHT].R = R2;
	clicks[WEIGHT].mR = weight_tool.mR;
	clicks[WEIGHT].pR = weight_tool.pR;
   mainR[TWEAKBASE+WEIGHT*3] = (rect *)&weight_tool.TB;
   mainR[TWEAKBASE+WEIGHT*3+1] = &weight_tool.mR;
   mainR[TWEAKBASE+WEIGHT*3+2] = &weight_tool.pR;


	OffsetRect(&R1, 0, 3 * FontHeight / 2);
	increment_tool.tR = R1;
	increment_tool.tR.Xmin = cx + 2;
	JString("Increment:  ", cx, R1.Ymin + 2, WHITE, BLUE, alignRight, alignTop);
	increment_tool.p1 = 4;
	increment_tool.p2 = 3;
	increment_tool.value = increment;
	increment_tool.type = GS_FLOAT;
	CreateNumberTool(&increment_tool);
	tweakRects[INCREMENT] = increment_tool.TB;
	R2 = increment_tool.mR;
	R2.Xmax = increment_tool.pR.Xmax;
	clicks[INCREMENT].R = R2;
	clicks[INCREMENT].mR = weight_tool.mR;
	clicks[INCREMENT].pR = weight_tool.pR;
   mainR[TWEAKBASE+INCREMENT*3] = (rect *)&increment_tool.TB;
   mainR[TWEAKBASE+INCREMENT*3+1] = &increment_tool.mR;
   mainR[TWEAKBASE+INCREMENT*3+2] = &increment_tool.pR;


	OffsetRect(&R1, 0, 3 * FontHeight / 2);
	displaymodeR = R1;
	PaintDisplayMode();
	tweakRects[DISPLAYMODE].nR = displaymodeR;
   mainR[TWEAKBASE+DISPLAYMODE*3] = &displaymodeR;


	OffsetRect(&R1, 0, 3 * FontHeight / 2);
	addmapR = R1;
	addmapR.Xmax = cx - 2;
	PaintRadioButton(&addmapR, false, false, "Add map");
	tweakRects[ADDMAP].nR = addmapR;
   mainR[TWEAKBASE+DISPLAYMODE*3+1] = &addmapR;

	deletemapR = R1;
	deletemapR.Xmin = cx + 2;
	PaintRadioButton(&deletemapR, false, false, "Delete map");
	tweakRects[KILLMAP].nR = deletemapR;
   mainR[TWEAKBASE+DISPLAYMODE*3+2] = &deletemapR;





	FirstMap = 0;
	ShowAll();



	ShowCursor();
}


int check_triplet(int X, int Y, numbertool * nt, int base)
{
	int retval = true;

	if (XYInRect(X, Y, &nt->TB.nR))
		current_main_item = base * 3 + TWEAKBASE;
	else if (XYInRect(X, Y, &nt->mR))
		current_main_item = base * 3 + TWEAKBASE + 1;
	else if (XYInRect(X, Y, &nt->pR))
		current_main_item = base * 3 + TWEAKBASE + 2;
	else
		retval = false;

	return retval;
}



void CheckTweakDrags(event * e)
{
	int i;
	int X = e->CursorX;
	int Y = e->CursorY;

	for (i = 0; i < 6 && i < fBptr->n; i++)
	{
		if (XYInRect(X, Y, &bottom_rects[i]))
		{
			current_main_item = LEFTARROWBOX + 1 + i;
			return;
		}
	}

	if (fBptr->n > 6)
	{
		if (XYInRect(X, Y, &right_arrow))
		{
			current_main_item = RIGHTARROWBOX;
			return;
		}
		if (XYInRect(X, Y, &left_arrow))
		{
			current_main_item = LEFTARROWBOX;
			return;
		}
	}

	if (check_triplet(X, Y, &cmap_tool, CURMAP))
		return;
	for (i = 0; i < 6; i++)
	{
		if (check_triplet(X, Y, &param_tools[i], i + ZROT))
			return;
	}
	if (check_triplet(X, Y, &increment_tool, INCREMENT))
		return;
	if (check_triplet(X, Y, &weight_tool, WEIGHT))
		return;
	if (XYInRect(X, Y, &displaymodeR))
	{
		current_main_item = TWEAKBASE + DISPLAYMODE * 3;
		return;
	}
	if (XYInRect(X, Y, &addmapR))
	{
		current_main_item = TWEAKBASE + DISPLAYMODE * 3 + 1;
		return;
	}
	if (XYInRect(X, Y, &deletemapR))
	{
		current_main_item = TWEAKBASE + DISPLAYMODE * 3 + 2;
		return;
	}




}
