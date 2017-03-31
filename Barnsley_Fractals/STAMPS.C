
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "scodes.h"

#include "game.h"



/* Our method of stamping is different entirely in this program.
We just run the fucking fractal! */

/* Eeep! */
int Stamping = 0;
int StampingIterations = 400;

rect stamp_rects[4];

/* and we need to save all the mappers as well! */

barnmap_set *stamp_barnmap_sets[4] = {NULL, NULL, NULL, NULL};

void InitializeStamps(void)
{
	int i;

	int height;


	if (mode == 0x12)
		height = (6 * 76) / 7 - 1;
	else
		height = (35 * 76) / 56 - 1;

	for (i = 0; i < 4; i++)
	{
		rect R;

		R.Xmin = 2;
		R.Xmax = 77;
		R.Ymin = sR.Ymax - (i + 1) * (height + 4);
		R.Ymax = R.Ymin + height;

		stamp_rects[i] = R;
	}
}



void MakeStamp(void)
{


	int i;
	barnmap_set *bs;
	flockstruct *tflockptr;
	fflockstruct *tfflockptr;
	int savedbarnmapflag = barnmapflag;
	int savedflocktype = flocktype;

	if (realmode)
	{
		if (!memok(sizeof(barnmap_set) + sizeof(mapper) * fBptr->n
			     + sizeof(fflockstruct)))
		{
	                ErrorBox("Not enough memory to do a stamp.");
			return;
		}
	}
	else
	{
		if (!memok(sizeof(barnmap_set) + sizeof(mapper) * fBptr->n
			     + sizeof(flockstruct)))
		{
	                ErrorBox("Not enough memory to do a stamp.");
			return;
		}
	}

	barnmapflag = 0;
	flocktype = 0;

	HideCursor();

	Stamping = 1;

	/* slide the stamps... */

	if ((bs = stamp_barnmap_sets[3]) != NULL)
	{
		/* clear out this one... */
		free(bs->maps);

		free(bs);
		stamp_barnmap_sets[3] = NULL;
	}





	for (i = 3; i > 0; i--)
	{
		rect R1, R2;

		R1 = stamp_rects[i - 1];
		R2 = stamp_rects[i];

		CopyBits(theBitmap, theBitmap, &R1, &R2, &R2, 0);

		/* and copy the barnmap */
		stamp_barnmap_sets[i] = stamp_barnmap_sets[i - 1];
	}

	/* and create the new set */
	bs = (barnmap_set *) malloc(sizeof(barnmap_set));
	bs->n = fBptr->n;
	bs->lox = flox;
	bs->loy = floy;
	bs->hix = fhix;
	bs->hiy = fhiy;
   bs->dirty = barnmapsp;

	bs->maps = (mapper *) malloc(sizeof(mapper) * bs->n);

	for (i = 0; i < fBptr->n; i++)
	{
		bs->maps[i].h = fBptr->h[i];
		bs->maps[i].weight = fBptr->weight[i];
	}
   strcpy(bs->name,current_barnmap_name);

	stamp_barnmap_sets[0] = bs;




	/* Save the entire flock here snoid! hope we can... */
	if (realmode)
	{
		tfflockptr = calloc(sizeof(fflockstruct), 1);
		*tfflockptr = *fflockptr;
	}
	else
	{
		tflockptr = calloc(sizeof(flockstruct), 1);
		*tflockptr = *flockptr;
	}


	installmode();
	fillflock();
	iteration = 0;
	for (i = 0; i < StampingIterations; i++, iteration++)
		step();
	Stamping = 0;

	if (realmode)
	{
		*fflockptr = *tfflockptr;
		free(tfflockptr);
	}
	else
	{
		*flockptr = *tflockptr;
		free(tflockptr);
	}

	barnmapflag = savedbarnmapflag;
	flocktype = savedflocktype;
	_installmode();
	ShowCursor();

}

void restore_stamp(int n)
{
	int i;


	barnmap_set *bs = stamp_barnmap_sets[n];

	if (!bs)
		return;

	fBptr->n = bs->n;
   dirty_bit = bs->dirty;
	fstartlox = flox = bs->lox;
	fstartloy = floy = bs->loy;
	fstarthix = fhix = bs->hix;
	fstarthiy = fhiy = bs->hiy;
	fstartdeltax = fdeltax = fhix - flox;
	fstartdeltay = fdeltay = fhiy - floy;
	fstartcenterx = fcenterx = flox + fdeltax / 2;
	fstartcentery = fcentery = floy + fdeltay / 2;

	startlox = lox = BLOAT * flox;
	starthix = hix = BLOAT * fhix;
	starthiy = loy = BLOAT * floy;
	startloy = hiy = BLOAT * fhiy;

	startdeltax = deltax = hix - lox;
	startdeltay = deltay = hiy - loy;
	startcenterx = centerx = lox + deltax / 2;
	startcentery = centery = loy + deltay / 2;

   strcpy(current_barnmap_name,bs->name);
	for (i = 0; i < fBptr->n; i++)
	{
		fBptr->h[i] = bs->maps[i].h;
		fBptr->weight[i] = bs->maps[i].weight;
	}
	ClearBarnmapStack();
	_activate(fBptr);
	edmap = 0;
	if (tweaking)
	{
		FirstMap = 0;
		ShowAll();
		UpdateAllParams(edmap);
	}
}
