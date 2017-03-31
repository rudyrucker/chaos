/* Part two of ATTRACT. Split up because it was too damned
big to compile. */

#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "attract.h"

#ifdef abs
#undef abs
#endif
#define abs(x) (((x) > 0) ? (x) : (-x))

int available_colors[] = {2, 3, 5, 6, 9, 10, 11, 12, 13, 14};

#define maxcolor 10
int SaveMe;			/* move this later */
char *stamper = NULL;

extern int StampTracking;

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
   short Xmin,Ymin,Xmax,Ymax;
   } longRect;

#define X1 ep->Xmin
#define Y1 ep->Ymin
#define X2 ep->Xmax
#define Y2 ep->Ymax

#define XUL r->Xmin
#define YUL r->Ymin
#define XLR r->Xmax
#define YLR r->Ymax

static void SetOutcodes(Outcode *u,longRect *r,short x,short y)
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


#ifdef LONGCLIP
static void lSwap(long *pa,long *pb)
{
   long t = *pa;
   *pa = *pb;
   *pb = t;
}
#endif

int Clip(rect *eep,rect *rr)
{
   Outcode ocu1,ocu2;
   int Inside,Outside;
   longRect RR;
   longRect EEEP;
   longRect *r = &RR;
   longRect *ep = &EEEP;

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
         Swap(&X1,&X2);
         Swap(&Y1,&Y2);
         Swap(&ocu1.outcodes,&ocu2.outcodes);
      }

      if (ocu1.ocs.code0)
      {
         Y1 += (long)(Y2-Y1)*(XUL-X1) / (X2 - X1);
         X1 = XUL;
      }

      else if (ocu1.ocs.code1)
      {
         X1 += (long)(X2-X1)*(YUL-Y1) / (Y2-Y1);
         Y1 = YUL;
      }
      else if (ocu1.ocs.code2)
      {
         Y1 += (long)(Y2-Y1)*(XLR-X1) / (X2-X1);
         X1 = XLR;
      }
      else if (ocu1.ocs.code3)
      {
         X1 += (long)(X2-X1)*(YLR-Y1) / (Y2 - Y1);
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




void stamp_pix(int x, int y, int color)
{
	pair p, q;

	p.x = x;
	p.y = y;
	q = screentostamp(&p);
	segavgapixel(q.x, q.y, color);
}

void Pix(int x, int y, int color)
{
	egavgapixel(x, y, color);
	if (StampTracking)
      stamp_pix(x, y, color);
}




void stamp_bres(int x1, int y1, int x2, int y2, int color)
{
	pair p, q, r;

	p.x = x1;
	p.y = y1;
	q = screentostamp(&p);
	p.x = x2;
	p.y = y2;
	r = screentostamp(&p);
	sbresen(q.x, q.y, r.x, r.y, color);
}


void Bres(int x1, int y1, int x2, int y2, int color)
{
	bresen(x1, y1, x2, y2, color);
	if (StampTracking)
      stamp_bres(x1, y1, x2, y2, color);
}

void wBres(int x1, int y1, int x2, int y2, int color, int width)
{
	wbresen(x1, y1, x2, y2, color, width);
   if (StampTracking)
   	stamp_bres(x1, y1, x2, y2, color);
}


pair convertpair(fpair * w)
{
	pair temp;

	temp.x = ((w->x - flox) * xscale) + minx;
	temp.y = ((fhiy - w->y) * yscale);
	return temp;
}

int maybe_shutup_cursor(rect *R)
{
   /* R is actually the endpoints of a line. If the endpoints
      "clip" into the cursor rect, we need to hide the cursor. */
   rect Z = *R;
   if (!cursoron)
      return false;

   if (Clip(&Z,&current_cursor))
   {
      erasecursor();
      return true;
   }
   return false;
}


void bresen(int xa, int ya, int xb, int yb, int color)
{
	int dx, dy, uphill = 0, g, inc1, inc2, r, c, f;

   /* We don't want to bother with any of this if the line won't
      cross the screen, now do we? How do we know? */
   /* Well... */
   rect R;
   rect R2 = sR;
   int hidden;

   R.Xmin = xa;
   R.Ymin = ya;
   R.Xmax = xb;
   R.Ymax = yb;
   R2.Xmin = minx;
   R2.Ymax = maxy;

   if (!Clip(&R,&R2))
      return;
   xa = R.Xmin;
   xb = R.Xmax;
   ya = R.Ymin;
   yb = R.Ymax;

   /* We might want to hide the cursor here */
   hidden = maybe_shutup_cursor(&R);


	dx = xb - xa;
	dy = yb - ya;
	if (((dx >= 0) && (dy >= 0)) || ((dx <= 0) && (dy <= 0)))
		uphill = 1;
	if (abs(dx) > abs(dy))
	{
		if (dx > 0)
		{
			c = xa;
			r = ya;
			f = xb;
		}
		else
		{
			c = xb;
			r = yb;
			f = xa;
		}
		inc1 = 2 * abs(dy);
		g = inc1 - abs(dx);
		inc2 = g - abs(dx);
		if (uphill)
		{
			while (c <= f)
			{
				egavgapixel(c, r, color);
				c++;
				if (g >= 0)
				{
					r++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (c <= f)
			{
				egavgapixel(c, r, color);
				c++;
				if (g > 0)
				{
					r--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
	else
	{
		if (dy > 0)
		{
			c = xa;
			r = ya;
			f = yb;
		}
		else
		{
			c = xb;
			r = yb;
			f = ya;
		}
		inc1 = 2 * abs(dx);
		g = inc1 - abs(dy);
		inc2 = g - abs(dy);
		if (uphill)
		{
			while (r <= f)
			{
				egavgapixel(c, r, color);
				r++;
				if (g > 0)
				{
					c++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (r <= f)
			{
				egavgapixel(c, r, color);
				r++;
				if (g > 0)
				{
					c--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
   if (hidden)
      drawcursor();
}

void wbresen(int xa, int ya, int xb, int yb, int color, int width)
{
	int dx, dy, uphill = 0, g, inc1, inc2, r, c, f, w;


   rect R;
   rect R2 = sR;
   int hidden;

   R.Xmin = xa;
   R.Ymin = ya;
   R.Xmax = xb;
   R.Ymax = yb;
   R2.Xmin = minx;
   R2.Ymax = maxy;

   if (!Clip(&R,&R2))
      return;
   xa = R.Xmin;
   xb = R.Xmax;
   ya = R.Ymin;
   yb = R.Ymax;
   hidden = maybe_shutup_cursor(&R);

	dx = xb - xa;
	dy = yb - ya;
	if (dx * dy > 0)
		uphill = 1;
	if (abs(dx) > abs(dy))
	{
		if (dx > 0)
		{
			c = xa;
			r = ya;
			f = xb;
		}
		else
		{
			c = xb;
			r = yb;
			f = xa;
		}
		inc1 = 2 * abs(dy);
		g = inc1 - abs(dx);
		inc2 = g - abs(dx);
		if (uphill)
		{
			while (c <= f)
			{
				for (w = 0; w < width; w++)
					egavgapixel(c, r + w, color);
				c++;
				if (g >= 0)
				{
					r++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (c <= f)
			{
				for (w = 0; w < width; w++)
					egavgapixel(c, r + w, color);
				c++;
				if (g > 0)
				{
					r--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
	else
	{
		if (dy > 0)
		{
			c = xa;
			r = ya;
			f = yb;
		}
		else
		{
			c = xb;
			r = yb;
			f = ya;
		}
		inc1 = 2 * abs(dx);
		g = inc1 - abs(dy);
		inc2 = g - abs(dy);
		if (uphill)
		{
			while (r <= f)
			{
				for (w = 0; w < width; w++)
					egavgapixel(c + w, r, color);
				r++;
				if (g > 0)
				{
					c++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (r <= f)
			{
				for (w = 0; w < width; w++)
					egavgapixel(c + w, r, color);
				r++;
				if (g > 0)
				{
					c--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
   if (hidden)
      drawcursor();
}

void sbresen(int xa, int ya, int xb, int yb, int color)
{
	int dx, dy, uphill = 0, g, inc1, inc2, r, c, f;
   rect R;


   R.Xmin = xa;
   R.Ymin = ya;
   R.Xmax = xb;
   R.Ymax = yb;

   if (!Clip(&R,&stampR))
      return;
   xa = R.Xmin;
   xb = R.Xmax;
   ya = R.Ymin;
   yb = R.Ymax;

	dx = xb - xa;
	dy = yb - ya;
	if (((dx >= 0) && (dy >= 0)) || ((dx <= 0) && (dy <= 0)))
		uphill = 1;
	if (abs(dx) > abs(dy))
	{
		if (dx > 0)
		{
			c = xa;
			r = ya;
			f = xb;
		}
		else
		{
			c = xb;
			r = yb;
			f = xa;
		}
		inc1 = 2 * abs(dy);
		g = inc1 - abs(dx);
		inc2 = g - abs(dx);
		if (uphill)
		{
			while (c <= f)
			{
				segavgapixel(c, r, color);
				c++;
				if (g >= 0)
				{
					r++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (c <= f)
			{
				segavgapixel(c, r, color);
				c++;
				if (g > 0)
				{
					r--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
	else
	{
		if (dy > 0)
		{
			c = xa;
			r = ya;
			f = yb;
		}
		else
		{
			c = xb;
			r = yb;
			f = ya;
		}
		inc1 = 2 * abs(dx);
		g = inc1 - abs(dy);
		inc2 = g - abs(dy);
		if (uphill)
		{
			while (r <= f)
			{
				segavgapixel(c, r, color);
				r++;
				if (g > 0)
				{
					c++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (r <= f)
			{
				segavgapixel(c, r, color);
				r++;
				if (g > 0)
				{
					c--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
}


void xwbresen(int xa, int ya, int xb, int yb, int color, int width)
{
	int dx, dy, uphill = 0, g, inc1, inc2, r, c, f, w;

	dx = xb - xa;
	dy = yb - ya;
	if (dx * dy > 0)
		uphill = 1;
	if (abs(dx) > abs(dy))
	{
		if (dx > 0)
		{
			c = xa;
			r = ya;
			f = xb;
		}
		else
		{
			c = xb;
			r = yb;
			f = xa;
		}
		inc1 = 2 * abs(dy);
		g = inc1 - abs(dx);
		inc2 = g - abs(dx);
		if (uphill)
		{
			while (c <= f)
			{
				for (w = 0; w < width; w++)
					xegavgapixel(c, r + w, color);
				c++;
				if (g >= 0)
				{
					r++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (c <= f)
			{
				for (w = 0; w < width; w++)
					xegavgapixel(c, r + w, color);
				c++;
				if (g > 0)
				{
					r--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
	else
	{
		if (dy > 0)
		{
			c = xa;
			r = ya;
			f = yb;
		}
		else
		{
			c = xb;
			r = yb;
			f = ya;
		}
		inc1 = 2 * abs(dx);
		g = inc1 - abs(dy);
		inc2 = g - abs(dy);
		if (uphill)
		{
			while (r <= f)
			{
				for (w = 0; w < width; w++)
					xegavgapixel(c + w, r, color);
				r++;
				if (g > 0)
				{
					c++;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
		else
		{
			while (r <= f)
			{
				for (w = 0; w < width; w++)
					xegavgapixel(c + w, r, color);
				r++;
				if (g > 0)
				{
					c--;
					g += inc2;
				}
				else
					g += inc1;
			}
		}
	}
}


/* Good candidate for optimization. All of these divides can be done
  just once. */


/*------------Dynamics Functions--------------*/
void lstep()
{
   extern void logistic_step(void);
   extern void henon_step(void);
   extern void lorenz_step(void);
   extern void yorke_step(void);

	switch (dimension)
	{
		case LOGISTIC:
		logistic_step();
		break;

	case HENON:
		henon_step();
		break;

	case LORENZ:
		lorenz_step();
		break;

	case YORKE:
		yorke_step();
		break;
	}
}



void popview(view * w)
{
	flox = w->vlox;
	fhix = w->vhix;
	floy = w->vloy;
	fhiy = w->vhiy;
}

void pushview(view * w)
{
	w->vlox = flox;
	w->vhix = fhix;
	w->vloy = floy;
	w->vhiy = fhiy;
}

double fancyhump(double lvfx, double x, double y)
{
/* this function has its max at pow(0.5, 1/y) and conversely, to
have a max at humpspot, one needs to let y, or humpshift, be
log(0.5)/log(humpmax) */

	double z;

	if (x <= 0)
		return 0;
	z = pow(x, y);
	z = lvfx * z * (1 - z);
	if (z <= 0)
		return 0;

	return pow(z, 1 / y);
}

void randomize_yorkers(void)
{
	yorkenumber++;
	switch (yorkenumber)
	{
	case 0:
		omega1 = 0.48566516831488;
		omega2 = 0.90519373301868;
		epsilon = 0.5;
		epsbar = EPSBAR1;
		break;
	case 1:
		omega1 = 0.42;
		omega2 = 0.3;
		epsilon = 0.6;
		epsbar = epsilon / TWOPI;
		break;
	case 2:
		omega1 = 0.3356;
		omega2 = 0.4160;
		epsilon = 0.6;
		epsbar = epsilon / TWOPI;
		break;
	case 3:
		omega1 = 0.38051254310267;
		omega2 = 0.83337639800084;
		epsilon = 0.75;
		epsbar = epsilon / TWOPI;
		break;
	case 4:
		omega1 = 0.45921779763739;
		omega2 = 0.53968253968254;
		epsilon = 0.5;
		epsbar = EPSBAR1;
		break;
	default:
		omega1 = (double) sixteenbitsa() / 0x10000L +
			(double) sixteenbitsa() / 0xFFFFFFFFL;
		omega2 = (double) sixteenbitsa() / 0x10000L +
			(double) sixteenbitsa() / 0xFFFFFFFFL;
		omega1 = fabs(omega1);
		omega2 = fabs(omega2);
		break;
	}
}

void SplitRectH(rect * tR, rect * d1, rect * d2)
{
	int midpoint = tR->Ymin + (tR->Ymax - tR->Ymin) / 2;

	d1->Xmin = d2->Xmin = tR->Xmin;
	d1->Xmax = d2->Xmax = tR->Xmax;

	d1->Ymin = tR->Ymin;
	d1->Ymax = midpoint;
	d2->Ymin = midpoint + 1;
	d2->Ymax = tR->Ymax;
}

void SplitRectV(rect * tR, rect * d1, rect * d2)
{
	int midpoint = tR->Xmin + (tR->Xmax - tR->Xmin) / 2;

	d1->Ymin = d2->Ymin = tR->Ymin;
	d1->Ymax = d2->Ymax = tR->Ymax;

	d1->Xmin = tR->Xmin;
	d1->Xmax = midpoint;
	d2->Xmin = midpoint + 1;
	d2->Xmax = tR->Xmax;
}

pair screentostamp(pair * p)
{
	pair s;

	s.x = ((p->x - 80) * xscreentostamp) + 5;
	s.y = (p->y * yscreentostamp) + stampR.Ymin;

	return s;
}


#define _sign(x) ( ((x) < 0) ? -1 : 1)

#define round(x) ((int)((x)*1000+_sign(x)*.5))/1000.0

extern int ParameterDisplayMode;/* Status line toggle */

void announceparms(void)
{
	char tbuf[128];
   int i;

	static char *yorke_shapes[] = {
		"Torus",
		"Klein",
		"Projective"
	};
	double td1, td2;
	PenColor(LIGHTGRAY);
   BackColor(BLACK);
   RasterOp(zREPz);
	TextAlign(alignLeft, alignBottom);
	MoveTo(minx + 4, sR.Ymax);
	memset(tbuf, ' ', 80);
	tbuf[80] = '\0';
	DrawString(tbuf);
   tbuf[0] = 0;
   MoveTo(minx + 4, sR.Ymax);
	if (ParameterDisplayMode)
	{			/* Status line is on */

		switch (dimension)
		{
		case LOGISTIC:
			td1 = round(lvfx);
			td2 = round(humpspot);

			switch (tracetype)
			{
			case -1:
			case 0:
				sprintf(tbuf, "Logistic Map: humpspot=%g", td2);
				break;
			case 1:
			case 2:
				sprintf(tbuf, "Logistic %s: humpspot=%g, chaoticity=%g",
					(tracetype == 1) ? "Pulse" : "Hump", td2, td1);
				break;
			}
			break;
		case HENON:
			td1 = round(fha);
			if (fancyflag)
				sprintf(tbuf, "Hnon: chaoticity=%g", td1);
			else
				sprintf(tbuf, "Hnon Horseshoe");
			break;
		case LORENZ:
         i = (int) (1.0/dt + .5);
			if (!lorenzflyflag)
				sprintf(tbuf, "Lorenz: axis=%c,accuracy=%d", axis, i);
			else
				sprintf(tbuf, "Lorenz: fly's eye view,accuracy=%d", i);
			break;
		case YORKE:
			td1 = round(epsilon);
			sprintf(tbuf, "Yorke: topology=%s,chaoticity=%g",
				yorke_shapes[yorketopologyflag], td1);
			break;
		}
	}
   if (stopped)
      strcat(tbuf," [Stopped]");
	DrawString(tbuf);
}

/* To avoid using the TC .BGI files, I keep  my  own  graphics	info.*/
#undef maxcolor
struct graphinfo
{
	int maxx, maxy;
	unsigned char maxcolor;
}

 modeinfo[0x14] =
{
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	319, 199, 3,		/* 4 = CGA */
	0, 0, 0,
	39, 199, 1,		/* 6 = HiCGA */
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	639, 349, 15,		/* 0x10 = EGA */
	0, 0, 0,
	639, 478, 15,		/* 0x12 = VGA */
	319, 199, 255		/* 0x13 = MCGA */
};


void installmode()
{
	int i;
	int height;

	if (curx > minx)
		erasecursor();

	SetDisplay(GrafPg0);

	GetPort(&thePort);
	sR = thePort->portRect;
	FontHeight = thePort->txFont->chHeight;
	aspect = (double) thePort->portBMap->pixResX /
		(double) thePort->portBMap->pixResY;
	StringWidthX = StringWidth("X");
	height = sR.Ymax / 8;

	HideCursor();
	/* Erase the display */
	BackColor(BLACK);



	minx = (modeinfo[mode].maxx + 1) / 8;
	maxx = 7 * minx;
	maxy = modeinfo[mode].maxy - FontHeight;

	dR.Xmin = minx;
	dR.Xmax = sR.Xmax;
	dR.Ymin = 0;
	dR.Ymax = sR.Ymax;
	stamprxmin = stampR.Xmin = 5;
	stamprxmax = stampR.Xmax = stampR.Xmin + 69;
	stamprymax = stampR.Ymax = sR.Ymax - 8;
	stamprymin = stampR.Ymin = stampR.Ymax - height + 1;
	xscreentostamp = (double) (stamprxmax - stamprxmin) / maxx;
	yscreentostamp = (double) (stamprymax - stamprymin) / maxy;
	for (i = 0; i < 3; i++)
	{
		rect S = stampR;

		OffsetRect(&S, 0, -(i + 1) * (height + 4));
		stampingR[i] = S;
	}


	EraseRect(&dR);
	EraseRect(&stampR);
	maxcolor = modeinfo[mode].maxcolor;
	if (colorcycleflag)
		changepalette();/* to keep starting up in new palettes */
	Jusepalette();
	SaveMe = 0;
	announceparms();
	for (i = 0; i < 100; i++)
		ShowCursor();
	if (curx > minx)
	{
		HideCursor();
		drawcursor();
	}
}
