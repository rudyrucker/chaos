/* Routines to do stamping, etc. */
#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include "attract.h"
#include <alloc.h>

stamp_structure *stamp_data[4] = {NULL, NULL, NULL, NULL};

int stamp_err = 0;
typedef void (*pfs) (stamp_structure *);
void preserve_data(void);

void lorenz_stamper(stamp_structure * s)
{
	lorenz_str *p = &s->s.lz;

	p->viewbinormal = viewbinormal;
	p->viewnormal = viewnormal;
	p->viewspot = viewspot;
	p->accuracy = dt;
	p->view = axis;
	p->flox = flox;
	p->floy = floy;
	p->floz = floz;
	p->fhix = fhix;
	p->fhiy = fhiy;
	p->fhiz = fhiz;
	p->trihedroncount = starting_trihedroncount;
   p->end_trihedroncount = trihedroncount;
	p->tracetype = tracetype;
	p->ribbonlength = ribbonlength;
	safe_alloc = 1;
	if ((p->fflock3ptr = malloc(sizeof(fflockstruct3))) == NULL)
	{
		ErrorBox("Not enough memory to preserve stamp.");
		stamp_err = 1;
		free(s);
		s = NULL;
		return;
	}
	*(p->fflock3ptr) = *saved_fflock3ptr;
   p->fflock3ptr->n = fflock3ptr->n;
}

void henon_stamper(stamp_structure * s)
{
	henon_str *p = &s->s.h;

	p->fha = fha;
	p->tracetype = tracetype;
	p->ribbonlength = ribbonlength;
	p->flox = flox;
	p->floy = floy;
	p->fhix = fhix;
	p->fhiy = fhiy;
	p->fancyflag = fancyflag;
   safe_alloc = 1;
	if ((p->fflock2ptr = malloc(sizeof(fflockstruct2))) == NULL)
	{
		ErrorBox("Not enough memory to preserve stamp.");
		stamp_err = 1;
		free(s);
      s = NULL;
		return;
	}
   *(p->fflock2ptr) = *saved_fflock2ptr;
   p->fflock2ptr->n = fflock2ptr->n;
   p->shiftregister = start_shiftregister;

}

void yorke_stamper(stamp_structure * s)
{
	yorke_str *p = &s->s.y;

	p->epsilon = epsilon;
	p->yorketopologyflag = yorketopologyflag;
	p->tracetype = tracetype;
	p->ribbonlength = ribbonlength;
	p->omega1 = omega1;
	p->omega2 = omega2;
	p->flox = flox;
	p->floy = floy;
	p->fhix = fhix;
	p->fhiy = fhiy;
   p->flocktype = flocktype;
}

void logistic_stamper(stamp_structure * s)
{
	logistic_str *p = &s->s.lo;

	p->lvfx = lvfx;
	p->humpspot = humpspot;
	p->tracetype = tracetype;
	if (tracetype == 0)
		p->flox = flox;
	p->floy = floy;
	p->fhix = fhix;
	p->fhiy = fhiy;
	{
		if ((p->fflock1ptr = malloc(sizeof(fflockstruct1))) == NULL)
		{
			ErrorBox("Not enough memory to preserve stamp.");
			stamp_err = 1;
			free(s);
			s = NULL;
			return;
		}
		memcpy(p->fflock1ptr, fflock1ptr, sizeof(fflockstruct1));
	}

}

void yorke_restorer(stamp_structure * s)
{
	yorke_str *p = &s->s.y;

   flocktype = p->flocktype;
	epsilon = p->epsilon;
	yorketopologyflag = p->yorketopologyflag;
	tracetype = p->tracetype;
	ribbonlength = p->ribbonlength;
	omega1 = p->omega1;
	omega2 = p->omega2;
	flox = p->flox;
	floy = p->floy;
	fhix = p->fhix;
	fhiy = p->fhiy;
	epsbar = epsilon / TWOPI;
	dimension = YORKE;
   fancyflag = 1;
	setwindow(0);
	installmode();
   fillflock();
}

void henon_restorer(stamp_structure * s)
{
	henon_str *p = &s->s.h;
   int max0;

	tracetype = p->tracetype;
	fha = p->fha;
	flox = p->flox;
	floy = p->floy;
	fhix = p->fhix;
	fhiy = p->fhiy;
	ribbonlength = p->ribbonlength;
	fancyflag = p->fancyflag;
	dimension = HENON;
	installmode();
	cosa = cos((fha * M_PI) / 4.0);
	sina = sin((fha * M_PI) / 4.0);
   *saved_fflock2ptr = *fflock2ptr = *p->fflock2ptr;
   free(p->fflock2ptr);
	setwindow(0);
	ribbonfull = 0;
	ribbonindex = 0;
   for(max0 = 0;max0 < fflock2ptr->n && fflock2ptr->releasetime[max0] == 0;max0++);
   if (max0 == fflock2ptr->n)
      next_release = 0;
   else
   {
      fflock2ptr->n = max0;
      next_release = fflock2ptr->releasetime[max0];
   }
   fflock2ptr->atom[max0].x = -9999;
   long_iteration = 0L;
   shiftregister = start_shiftregister = p->shiftregister;
   sixteen_count = 0;
}

void logistic_restorer(stamp_structure * s)
{
	logistic_str *p = &s->s.lo;

	lvfx = p->lvfx;
	flox = p->flox;
	floy = p->floy;
	fhix = p->fhix;
	fhiy = p->fhiy;
	tracetype = p->tracetype;
	if (tracetype == 0)
	{
		*fflock1ptr = *p->fflock1ptr;
	}
	humpspot = p->humpspot;
	humpshift = log(0.5) / log(humpspot);
	dimension = LOGISTIC;
	logisticlaunchflag = 1;
	installmode();
	free(p->fflock1ptr);
	p->fflock1ptr = NULL;
	setwindow(0);
}




void lorenz_restorer(stamp_structure * s)
{
	lorenz_str *p = &s->s.lz;
	int max0;

	int initial_axis = axis;

	*fflock3ptr = *p->fflock3ptr;
	trihedroncount = p->trihedroncount;
	axis = p->view;
	dt = p->accuracy;
	viewspot = p->viewspot;
	viewbinormal = p->viewbinormal;
	viewnormal = p->viewnormal;
	tracetype = p->tracetype;
	ribbonlength = p->ribbonlength;

	/* now find the maximum at 0 */
	for (max0 = 0; max0 < fflock3ptr->n && fflock3ptr->releasetime[max0] == 0; max0++);
	if (max0 == fflock3ptr->n)
		next_release = 0;
	else
	{
		fflock3ptr->n = max0;
		next_release = fflock3ptr->releasetime[max0];
	}
	fflock3ptr->atom[max0].x = -9999;

	*saved_fflock3ptr = *fflock3ptr;
	starting_trihedroncount = trihedroncount;
	ribbonindex = ribbonfull = 0;

	if (axis != initial_axis)
	{
		switch (axis)
		{
		case 'w':
			lorenzflyflag = 1;
			break;
		case 'x':
		case 'y':
		case 'z':
			lorenzflyflag = 0;
			break;
		}
	}

	flox = p->flox;
	floy = p->floy;
	floz = p->floz;
	fhix = p->fhix;
	fhiy = p->fhiy;
	fhiz = p->fhiz;

	ribbonindex = ribbonfull = 0;
	trihedronon = 0;
	dimension = LORENZ;
	installmode();
	setwindow(0);
	free(p->fflock3ptr);
	p->fflock3ptr = NULL;
	long_iteration = 0;
}

static rect *sssR;

void framelines(rect * R)
{
	RasterOp(zXORz);
	PenColor(WHITE);
	MoveTo(sssR->Xmin, sssR->Ymin);
	LineTo(R->Xmin, R->Ymin);
	MoveTo(sssR->Xmax, sssR->Ymin);
	LineTo(R->Xmax, R->Ymin);
	MoveTo(sssR->Xmin, sssR->Ymax);
	LineTo(R->Xmin, R->Ymax);
	MoveTo(sssR->Xmax, sssR->Ymax);
	LineTo(R->Xmax, R->Ymax);
	RasterOp(zREPz);
}



static void textbox(char **textlines)
{
	int items = 0, i;
	int height, width;
	rect R;
	rect tR;
	int cx, cy;
	char *msg1 = "Click or press any key to continue";

	for (items = width = 0; textlines[items]; items++)
		width = max(width, StringWidth(textlines[items]) + 16);
	width = max(width, StringWidth(msg1) + 16);
	Centers(&sR, &cx, &cy);


	height = FontHeight * (items + 1) + 16;

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;
	framelines(&R);
	BasicCenteredBox(&tR, width, height, DARKGRAY, textlines[0], WHITE);

	TextAlign(alignLeft, alignTop);
	for (i = 1; i < items; i++)
	{
		MoveTo(tR.Xmin + 4, tR.Ymin + 8 + FontHeight * i);
		PenColor(WHITE);
		BackColor(DARKGRAY);
		DrawString(textlines[i]);
	}

	MoveTo(cx, tR.Ymin + 8 + FontHeight * i + 4);
	TextAlign(alignCenter, alignTop);
	DrawString(msg1);


	while (1)
	{
		event e;

		KeyEvent(true, &e);

		if (e.ASCII || e.ScanCode || (e.State & 0x700))
			break;
	}
	WaitForNothing();
	PopRect(&i);
	framelines(&R);
}





void logistic_shower(stamp_structure * s)
{
	logistic_str *p = &s->s.lo;

	char *lines[20];
	char tbuf[128];
	int i, items = 0;

	lines[items++] = strdup("Logistic Parameters");

	sprintf(tbuf, "Window: (%g %g) (%g %g)",
		p->flox, p->floy, p->fhix, p->fhiy);
	lines[items++] = strdup(tbuf);

	sprintf(tbuf, "Tracetype: %d", p->tracetype);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Chaoticity: %g", p->lvfx);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Humpspot: %g", p->humpspot);
	lines[items++] = strdup(tbuf);
	lines[items] = NULL;
	textbox(lines);
	for (i = 0; i < items; i++)
		free(lines[i]);

}

static char *SML[] = {"Small","Medium","Large"};

void henon_shower(stamp_structure * s)
{
	henon_str *p = &s->s.h;
	char tbuf[128];
	char *lines[20];
	int i, items = 0;

	lines[items++] = strdup("Henon Parameters");
	sprintf(tbuf, "Flock size: %d", p->fflock2ptr->n);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Window: (%g %g) (%g %g)",
		p->flox, p->floy, p->fhix, p->fhiy);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Fanciness: %d", p->fancyflag);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Ribbonlength: %d", p->ribbonlength);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Tracetype: %d", p->tracetype);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Chaoticity: %g", p->fha);
   lines[items++] = strdup(tbuf);
	lines[items] = NULL;
	textbox(lines);
	for (i = 0; i < items; i++)
		free(lines[i]);
}


void yorke_shower(stamp_structure * s)
{
	yorke_str *p = &s->s.y;
	char tbuf[128];
	int items = 0, i;
	char *lines[20];
   extern char *topologymsg[];

	lines[items++] = strdup("Yorke Parameters");
	sprintf(tbuf, "Flock type: %s", SML[p->flocktype]);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Window: (%g %g) (%g %g)",
		p->flox, p->floy, p->fhix, p->fhiy);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Omegas: %g %g", p->omega1, p->omega2);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Ribbonlength: %d", p->ribbonlength);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Tracetype: %d", p->tracetype);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Topology: %s", topologymsg[p->yorketopologyflag]);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Epsilon: %g", p->epsilon);
	lines[items] = NULL;
	textbox(lines);
	for (i = 0; i < items; i++)
		free(lines[i]);
}



void lorenz_shower(stamp_structure * s)
{
	lorenz_str *p = &s->s.lz;
	char tbuf[128];
	int items = 0, i;
	char *lines[20];

	lines[items++] = strdup("Lorenz Parameters");
	sprintf(tbuf, "Flock size: %d", p->fflock3ptr->n);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Starting trihedrons: %d", p->trihedroncount);
	lines[items++] = strdup(tbuf);
   sprintf(tbuf, "Ending trihedrons: %d",p->end_trihedroncount);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Window: (%g %g %g)", p->flox, p->floy, p->floz);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "        (%g %g %g)", p->fhix, p->fhiy, p->floz);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Axis: %c", p->view);
	lines[items++] = strdup(tbuf);
	sprintf(tbuf, "Accuracy: %d", (int)(1.0/p->accuracy + .5));
	lines[items++] = strdup(tbuf);
	lines[items] = NULL;
	textbox(lines);
	for (i = 0; i < items; i++)
		free(lines[i]);
}


pfs stampers[] = {
	logistic_stamper,
	henon_stamper,
	lorenz_stamper,
	yorke_stamper
};

pfs restorers[] = {
	logistic_restorer,
	henon_restorer,
	lorenz_restorer,
	yorke_restorer
};

/* restorers have the obligation to free up the fflock memory */

pfs showers[] = {
	logistic_shower,
	henon_shower,
	lorenz_shower,
	yorke_shower
};



void restore_data(int n)
{
	stamp_structure s;
	fflockstruct1 *fs1;
	fflockstruct2 *fs2;
	fflockstruct3 *fs3;


	if (!stamp_data[n])
		return;

	s = *stamp_data[n];	/* copy it, we might loose it */
	switch (s.dimension)
	{
	case LOGISTIC:
		if (!memok(sizeof(fflockstruct1)) ||
		    (fs1 = malloc(sizeof(fflockstruct1))) == NULL)
		{
			ErrorBox("Not enough memory to restore stamp.");
			return;
		}
		memcpy(fs1, s.s.lo.fflock1ptr, sizeof(fflockstruct1));
		s.s.lo.fflock1ptr = fs1;
		break;
	case HENON:
      if (!memok(sizeof(fflockstruct2)) ||
		    (fs2 = malloc(sizeof(fflockstruct2))) == NULL)
		{
			ErrorBox("Not enough memory to restore stamp.");
			return;
		}
		memcpy(fs2, s.s.h.fflock2ptr, sizeof(fflockstruct2));
		s.s.h.fflock2ptr = fs2;
		break;
	case YORKE:
#ifdef OLDWAY
		if (!memok(sizeof(fflockstruct2)) ||
		    (fs2 = malloc(sizeof(fflockstruct2))) == NULL)
		{
			ErrorBox("Not enough memory to restore stamp.");
			return;
		}
		memcpy(fs2, s.s.y.fflock2ptr, sizeof(fflockstruct2));
		s.s.y.fflock2ptr = fs2;
#endif
		break;
	case LORENZ:
		if (!memok(sizeof(fflockstruct3)) ||
		    (fs3 = malloc(sizeof(fflockstruct3))) == NULL)
		{
			ErrorBox("Not enough memory to restore stamp.");
			return;
		}
		memcpy(fs3, s.s.lz.fflock3ptr, sizeof(fflockstruct3));
		s.s.lz.fflock3ptr = fs3;
		break;
	}


	erasecursor();
	if (Stamping)
	{
		preserve_data();
		slide_stamps();
	}
	/* the restorer will free the proper flockptr */
	(*restorers[s.dimension - 1]) (&s);
}

void preserve_data(void)
{
	int i;
	int ok=true;
	stamp_structure *s;

	switch (dimension)
	{
	case LOGISTIC:
		ok = memok((sizeof(fflockstruct1)));
		break;
   case HENON:
		ok = memok((sizeof(fflockstruct2)));
		break;
	case YORKE:
#ifdef OLDWAY
		ok = memok((sizeof(fflockstruct2)));
		break;
#endif
	case LORENZ:
		ok = memok((sizeof(fflockstruct3)));
		break;
	}

	if (!ok)
	{
		stamp_err = 1;
		ErrorBox("Not enough memory to preserve stamp.");
		return;
	}

	/* get rid of the highest */
	if (stamp_data[3])
	{
		s = stamp_data[3];
		switch (s->dimension)
		{
		case LOGISTIC:
			free(s->s.lo.fflock1ptr);
			s->s.lo.fflock1ptr = NULL;
			break;
      case HENON:
			free(s->s.h.fflock2ptr);
			s->s.h.fflock2ptr = NULL;
			break;
#ifdef OLDWAY
		case YORKE:
			free(s->s.y.fflock2ptr);
			s->s.y.fflock2ptr = NULL;
			break;
#endif
		case LORENZ:
			free(s->s.lz.fflock3ptr);
			s->s.lz.fflock3ptr = NULL;
			break;
		}
		free(s);
		s = NULL;
	}

	for (i = 3; i > 0; i--)
		stamp_data[i] = stamp_data[i - 1];

	s = malloc(sizeof(stamp_structure));
	s->dimension = dimension;
	(*stampers[dimension - 1]) (s);
	stamp_data[0] = s;
}

void show_data(int n)
{
	if (!stamp_data[n])
		return;

	sssR = &stampingR[n];

	HideCursor();
	(*showers[stamp_data[n]->dimension - 1]) (stamp_data[n]);
	ShowCursor();
}


void slide_stamps(void)
{
	int i;
	bitmap *b = thePort->portBMap;

	if (stamp_err)
	{
		stamp_err = 0;
		return;
	}

	HideCursor();
	for (i = 2; i > 0; i--)
	{
		CopyBits(b, b, &stampingR[i - 1], &stampingR[i], &stampingR[i], zREPz);
		PenColor(WHITE);
		FrameRect(&stampingR[i]);
	}

	CopyBits(b, b, &stampR, &stampingR[0], &stampingR[0], zREPz);
	FrameRect(&stampingR[0]);
	ShowCursor();
}
