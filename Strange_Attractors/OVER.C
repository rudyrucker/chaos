#include "attract.h"
#include <math.h>

/*---------------------Zoom and Pan Functions---------*/
void setwindow(int start)
{
	if (start)
	{
		switch (dimension)
		{
			case LOGISTIC:
			flox = FLOX1START;
			fhix = FHIX1START;
			floy = FLOY1START;
			fhiy = FHIY1START;
			floz = FLOZ1START;
			fhiz = FHIZ1START;
			break;
		case HENON:
			flox = FLOX2START;
			fhix = FHIX2START;
			floy = FLOY2START;
			fhiy = FHIY2START;
			floz = FLOZ2START;
			fhiz = FHIZ2START;
			break;
		case LORENZ:
			flox = FLOX3START;
			fhix = FHIX3START;
			floy = FLOY3START;
			fhiy = FHIY3START;
			floz = FLOZ3START;
			fhiz = FHIZ3START;
			break;
		case YORKE:
			flox = FLOX4START;
			fhix = FHIX4START;
			floy = FLOY4START;
			fhiy = FHIY4START;
			floz = FLOZ4START;
			fhiz = FHIZ4START;
			break;
		}
		startdeltax = fhix - flox;
		startdeltay = fhiy - floy;
		startdeltaz = fhiz - floz;
		startcenterx = flox + startdeltax / 2;
		startcentery = floy + startdeltay / 2;
		startcenterz = floz + startdeltaz / 2;
	}
	fdeltax = fhix - flox;
	fdeltay = fhiy - floy;
	fdeltaz = fhiz - floz;

	fdeltax_div_64K = fdeltax / 0x10000L;
	fdeltay_div_64K = fdeltay / 0x10000L;
	fdeltaz_div_64K = fdeltaz / 0x10000L;

	xscale = maxx / (fhix - flox);
	yscale = maxy / (fhiy - floy);
	zscale = maxy / (fhiz - floz);
	xyscale = maxx / (fhiy - floy);

	fcenterx = flox + fdeltax / 2;
	fcentery = floy + fdeltay / 2;
	fcenterz = floz + fdeltaz / 2;
	if (dimension == LOGISTIC && !tracetype)
	{
		pixelx = minx;
		fx = flox;
		fxstep = (fhix - flox) / maxx;
		fystep = (fhiy - floy) / maxy;
		startlog = standardstartlog;
		stoplog = standardstoplog;
	}
	if (dimension == LORENZ)
	{
		pcentx = min(flox, LENS * FLOX3START);
		pcenty = min(floy, LENS * FLOY3START);
		pcentz = min(floz, LENS * FLOZ3START);
		pcentfly = min(pcentx, pcenty);
		pcanvasz = (FLOZ3START + FHIZ3START) / 2;
	}
   logisticlaunchflag = 1;
   ribbonindex = ribbonfull = 0;
   long_iteration = 0L;

}

void dopan(int panx, int pany)
{
	double templox, temphix, temploy, temphiy;

	if (Stamping)
	{
		preserve_data();
		slide_stamps();
	}

	if (dimension < LORENZ || axis == 'z')
	{
		templox = flox + panx * (fdeltax / 2);
		temphix = fhix + panx * (fdeltax / 2);
		temploy = floy + pany * (fdeltay / 2);
		temphiy = fhiy + pany * (fdeltay / 2);
		if (dimension == LOGISTIC && (templox < FLOX1START ||
			temphix > FHIX1START || temploy < 0 || temphiy > 1))
			return;
		flox = templox;
		fhix = temphix;
		floy = temploy;
		fhiy = temphiy;
		fcenterx = flox + fdeltax / 2;
		fcentery = floy + fdeltay / 2;
	}
	else if (axis == 'x')
	{
		floy = floy + panx * (fdeltay / 2);
		fhiy = fhiy + panx * (fdeltay / 2);
		floz = floz + pany * (fdeltaz / 2);
		fhiz = fhiz + pany * (fdeltaz / 2);
		fcentery = floy + fdeltay / 2;
		fcenterz = floz + fdeltaz / 2;
	}
	else if (axis == 'y')
	{
		flox = flox + panx * (fdeltax / 2);
		fhix = fhix + panx * (fdeltax / 2);
		floz = floz + pany * (fdeltaz / 2);
		fhiz = fhiz + pany * (fdeltaz / 2);
		fcenterx = flox + fdeltax / 2;
		fcenterz = floz + fdeltaz / 2;
	}
	xscale = maxx / (fhix - flox);
	yscale = maxy / (fhiy - floy);
	zscale = maxy / (fhiz - floz);
	xyscale = maxx / (fhiy - floy);
	iteration = 0;
	long_iteration = 0;
	/* use iteration as counter to prevent a double pan */
	installmode();
	setwindow(0);
	ribbonindex = 0;
	ribbonfull = 0;
	trihedronon = 0;
	resetcursor();
}



void _dozoom(int n,double zoomfactor,double yzoomfactor)
{
	if (Stamping)
	{
		preserve_data();
		slide_stamps();
	}

	erasecursor();
	if (n == 0)
	{
		installmode();
		setwindow(1);
		return;
	}

	if (dimension == LOGISTIC && n == -1 && fhix - flox >= 3)
		return;

	if (dimension < LORENZ || axis == 'z')
	{
		fcenterx = flox + fdeltax * ((double) (curx - minx) / (double) maxx);
		fcentery = fhiy - fdeltay * ((double) cury / (double) maxy);
	}
	else if (axis == 'x')
	{
		fcentery = floy + fdeltay * ((double) (curx - minx) / (double) maxx);
		fcenterz = fhiz - fdeltaz * ((double) cury / (double) maxy);
	}
	else if (axis == 'y')
	{
		fcenterx = flox + fdeltax * ((double) (curx - minx) / (double) maxx);
		fcenterz = fhiz - fdeltaz * ((double) cury / (double) maxy);
	}

	if (n == 1)
	{
		if (dimension == LOGISTIC && tracetype == 0)
		{
			fdeltax /= zoomfactor;
			fdeltay /= yzoomfactor;
		}
		else
		{
			fdeltax /= zoomfactor;
			fdeltay /= zoomfactor;
		}
      fdeltaz /= zoomfactor;

      if (dimension == LORENZ)
      {
         fdeltaz = max(fdeltaz,1.0);
         fdeltay = max(fdeltay,1.0);
         fdeltax = max(fdeltax,1.0);
      }
      else
      {
         fdeltaz = max(fdeltaz,1.0e-2);
         fdeltay = max(fdeltay,1.0e-2);
         fdeltax = max(fdeltax,1.0e-2);
      }


		flox = fcenterx - fdeltax / 2;
		fhix = fcenterx + fdeltax / 2;
		floy = fcentery - fdeltay / 2;
		fhiy = fcentery + fdeltay / 2;
		floz = fcenterz - fdeltaz / 2;
		fhiz = fcenterz + fdeltaz / 2;
		if (dimension == LOGISTIC)
		{
			if (flox < FLOX1START)
			{
				fdeltax = zoomfactor * (fcenterx - FLOX1START);
				flox = FLOX1START;
				fhix = FLOX1START + fdeltax;
			}
			if (fhix > FHIX1START)
			{
				fdeltax = zoomfactor * (FHIX1START - fcenterx);
				flox = FHIX1START - fdeltax;
				fhix = FHIX1START;
			}
			if (fhiy > 1)
			{
				fdeltay = yzoomfactor * (1 - fcentery);
				floy = 1 - fdeltay;
				fhiy = 1;
			}
			if (floy < 0)
			{
				fdeltay = yzoomfactor * fcentery;
				floy = 0;
				fhiy = fdeltay;
			}
			fcenterx = flox + fdeltax / 2;
			fcentery = floy + fdeltay / 2;
		}
	}
	else if (n == -1)
	{
		if (dimension != LOGISTIC)
		{
			fdeltax *= zoomfactor;
			fdeltay *= zoomfactor;
			fdeltaz *= zoomfactor;

         if (dimension == LORENZ)
         {
            fdeltaz = min(fdeltaz,500);
            fdeltay = min(fdeltay,500);
            fdeltax = min(fdeltax,500);
         }
         else
         {
            fdeltaz = min(fdeltaz,1.0e3);
            fdeltay = min(fdeltay,1.0e3);
            fdeltax = min(fdeltax,1.0e3);
         }


			flox = fcenterx - fdeltax / 2;
			fhix = fcenterx + fdeltax / 2;
			floy = fcentery - fdeltay / 2;
			fhiy = fcentery + fdeltay / 2;
			floz = fcenterz - fdeltaz / 2;
			fhiz = fcenterz + fdeltaz / 2;
		}
		else
		{
			fdeltax *= zoomfactor;
			fdeltay *= yzoomfactor;
         fdeltax = min(fdeltax,3.99999);
         fdeltay = min(fdeltay,0.99999);

			flox = fcenterx - fdeltax / 2;
			fhix = fcenterx + fdeltax / 2;
			floy = fcentery - fdeltay / 2;
			fhiy = fcentery + fdeltay / 2;
			floz = fcenterz - fdeltaz / 2;
			fhiz = fcenterz + fdeltaz / 2;
			if (flox < FLOX1START)
			{
				flox = FLOX1START;
				fhix = FLOX1START + fdeltax;
			}
			if (fhix > FHIX1START)
			{
				flox = FHIX1START - fdeltax;
				fhix = FHIX1START;
			}
			if (fhiy > 1)
			{
				floy = 1 - fdeltay;
				fhiy = 1;
			}
			if (floy < 0)
			{
				floy = 0;
				fhiy = fdeltay;
			}
			fcenterx = flox + fdeltax / 2;
			fcentery = floy + fdeltay / 2;
		}
	}
	else if (n == -2)	/* actually justa set origin dicky */
	{
		if (dimension != LOGISTIC)
		{
			flox = fcenterx - fdeltax / 2;
			fhix = fcenterx + fdeltax / 2;
			floy = fcentery - fdeltay / 2;
			fhiy = fcentery + fdeltay / 2;
			floz = fcenterz - fdeltaz / 2;
			fhiz = fcenterz + fdeltaz / 2;
         viewspot.x = fcenterx;
         viewspot.y = fcentery;
         viewspot.z = fcenterz;
		}
		else
		{
			flox = fcenterx - fdeltax / 2;
			fhix = fcenterx + fdeltax / 2;
			floy = fcentery - fdeltay / 2;
			fhiy = fcentery + fdeltay / 2;
 			floz = fcenterz - fdeltaz / 2;
 			fhiz = fcenterz + fdeltaz / 2;
			if (flox < FLOX1START)
			{
				flox = FLOX1START;
				fhix = FLOX1START + fdeltax;
			}
			if (fhix > FHIX1START)
			{
				flox = FHIX1START - fdeltax;
				fhix = FHIX1START;
			}
			if (fhiy > 1)
			{
				floy = 1 - fdeltay;
				fhiy = 1;
			}
			if (floy < 0)
			{
				floy = 0;
				fhiy = fdeltay;
			}
			fcenterx = flox + fdeltax / 2;
			fcentery = floy + fdeltay / 2;
		}
   
	}

	fdeltax_div_64K = fdeltax / 0x10000L;
	fdeltay_div_64K = fdeltay / 0x10000L;
	fdeltaz_div_64K = fdeltaz / 0x10000L;
	xscale = maxx / (fhix - flox);
	yscale = maxy / (fhiy - floy);
	zscale = maxy / (fhiz - floz);
	xyscale = maxx / (fhiy - floy);
	if (dimension < LORENZ && dimension != HENON)
		fillflock();
	installmode();
	setwindow(0);
	ribbonindex = 0;
	ribbonfull = 0;
	trihedronon = 0;
   logisticlaunchflag=1;
	resetcursor();
}

void dozoom(int n)
{
   _dozoom(n,zoomfactor,(dimension == LOGISTIC) ? yzoomfactor : zoomfactor);
}


/*-----------------------Flock Functions------------*/

void monoflock()
{
	long_iteration = 0;

	switch (dimension)
	{
	case HENON:
	case YORKE:
		fflock2ptr->atom[0].x = 0.51;
		fflock2ptr->atom[0].y = 0.001;
		fflock2ptr->color[0] = maxcolor;
		fflock2ptr->releasetime[0] = long_iteration;
		fflock2ptr->n = 1;
		break;
	case LORENZ:
		fflock3ptr->atom[0].x = 0.1;
		fflock3ptr->atom[0].y = 1.0;
		fflock3ptr->atom[0].z = 0.1;
		fflock3ptr->color[0] = maxcolor;
		fflock3ptr->releasetime[0] = long_iteration;
		fflock3ptr->n = 1;
		break;
	}
}

void dotflock(double ds, int dots)
{
	int i, j, k, l;
	double fxinc, fyinc, fzinc, fx1, fy1, fz1;
	unsigned char color;

	long_iteration = 0;

	switch (dimension)
	{
	case YORKE:
	case HENON:
		if (!fancyflag)
		{
			fxinc = ds * startdeltax;
			fyinc = ds * startdeltay;
		}
		else
		{
			fxinc = ds * fdeltax;
			fyinc = ds * fdeltay;
		}
		fcurx = flox + (fhix - flox) * (double) curx / maxx;
		fcury = fhiy + (fhiy - floy) * (double) (cury) / maxy;
		i = 0;
		if (fancyflag)
			fy1 = fcury;
		else
			fy1 = startcentery;
		for (k = 0; k < dots; k++)
		{
			if (fancyflag)
				fx1 = fcurx;
			else
				fx1 = startcenterx;
			for (j = 0; j < dots; j++)
			{
				fflock2ptr->atom[i].x = fx1;
				fflock2ptr->atom[i].y = fy1;
				i++;
				fx1 += fxinc;
			}
			fy1 += fyinc;
		}
		fflock2ptr->n = dots * dots;
		for (i = 0; i < fflock2ptr->n; i++)
		{
			color = colorize(i);
			fflock2ptr->color[i] = color;
			fflock2ptr->releasetime[i] = long_iteration;
		}
		break;
	case LORENZ:
		fxinc = ds * fdeltax;
		fyinc = ds * fdeltay;
		fzinc = ds * fdeltaz;
		i = 0;
		fz1 = fcenterz;
		for (l = 0; l < dots; l++)
		{
			fy1 = fcentery;
			for (k = 0; k < dots; k++)
			{
				fx1 = fcenterx;
				for (j = 0; j < dots; j++)
				{
					fflock3ptr->atom[i].x = fx1;
					fflock3ptr->atom[i].y = fy1;
					fflock3ptr->atom[i].z = fz1;
					i++;
					fx1 += fxinc;
				}
				fy1 += fyinc;
			}
			fz1 += fzinc;
		}
		fflock3ptr->n = dots * dots * dots;
		for (i = 0; i < fflock3ptr->n; i++)
		{
			color = colorize(i);
			fflock3ptr->color[i] = color;
			fflock3ptr->releasetime[i] = long_iteration;
		}
		break;
	}
	*saved_fflock3ptr = *fflock3ptr;
}

void fillflock()
{
	int i;


	if (dimension == LOGISTIC)
		for (i = 0; i < XPIXELS; i++)
			fflock1ptr->atom[i] = LOGSTART;

	long_iteration = 0;

	switch (flocktype)
	{
	case 0:
		monoflock();
		break;
	case 1:
		switch (dimension)
		{
		case YORKE:
		case HENON:
			if (fancyflag)
				dotflock(0.03, 2);
			else
			{
				dotflock(0.03, 2);
				fflock2ptr->n = 2;
				fflock2ptr->color[0] = colorize(2);
				fflock2ptr->color[1] = colorize(maxcolor - 2);
			}
			break;
		case LORENZ:
			dotflock(0.03, 3);
			break;
		}
		break;
	case 2:
		switch (dimension)
		{
		case YORKE:
		case HENON:
			if (fancyflag)
				dotflock(0.01, 8);
			else
				dotflock(0.01, 4);
			break;
		case LORENZ:
			dotflock(0.001, 4);
			break;
		}
		break;
	}
	ribbonindex = 0;
	ribbonfull = 0;
	cosa = cos((fha * PI) / 4.0);
	sina = sin((fha * PI) / 4.0);

	if (dimension == LORENZ)
		*saved_fflock3ptr = *fflock3ptr;
	else if (dimension == YORKE || dimension == HENON)
   {
		*saved_fflock2ptr = *fflock2ptr;
      start_shiftregister = shiftregister;
      sixteen_count = 0;
   }
}



void addxyflock(int x, int y)
{
	int n, i;
	pair u;
	ftriple local, templocal;

	int curx = x;
	int cury = y;

	switch (dimension)
	{
	case LOGISTIC:
		break;
	case YORKE:
	case HENON:
		n = fflock2ptr->n;
		fflock2ptr->atom[n].x =
			flox + (fhix - flox) * (double) (curx - minx) / maxx;
		fflock2ptr->atom[n].y =
			fhiy - (fhiy - floy) * (double) (cury) / maxy;
		fflock2ptr->color[n] = fflock2ptr->color[n - 1] + 4;
		fflock2ptr->n++;
		fflock2ptr->releasetime[n] = long_iteration;
		saved_fflock2ptr->atom[n] = fflock2ptr->atom[n];
		saved_fflock2ptr->releasetime[n] = fflock2ptr->releasetime[n];
		saved_fflock2ptr->color[n] = fflock2ptr->color[n];
		break;
	case LORENZ:
		n = fflock3ptr->n;
		if (!lorenzflyflag)
      {
			switch (axis)
			{
			case 'x':
				fflock3ptr->atom[n].x = fcenterx;
				fflock3ptr->atom[n].y =
					floy + (fhiy - floy) * (double) (curx - minx) / maxx;
				fflock3ptr->atom[n].z =
					fhiz - (fhiz - floz) * (double) (cury) / maxy;
				break;
			case 'y':
				fflock3ptr->atom[n].x =
					flox + (fhix - flox) * (double) (curx - minx) / maxx;
				fflock3ptr->atom[n].y = 0;
				fflock3ptr->atom[n].z =
					fhiz - (fhiz - floz) * (double) (cury) / maxy;
				break;
			case 'z':
				fflock3ptr->atom[n].x =
					flox + (fhix - flox) * (double) (curx - minx) / maxx;
				fflock3ptr->atom[n].y =
					fhiy - (fhiy - floy) * (double) (cury) / maxy;
				fflock3ptr->atom[n].z = pcanvasz;
				break;
			}
      }
		else
		{
			templocal.x =
				(fhix - flox) * (-0.5 +
					   ((double) (curx - minx) / maxx));
			templocal.y =
				(fhiy - floy) * (0.5 - ((double) (cury) / maxy));
			local.x = templocal.x * viewnormal.x + templocal.y *
				viewbinormal.x;
			local.y = templocal.x * viewnormal.y + templocal.y *
				viewbinormal.y;
			local.z = templocal.x * viewnormal.z + templocal.y *
				viewbinormal.z;
			fflock3ptr->atom[n].x = viewspot.x + local.x;
			fflock3ptr->atom[n].y = viewspot.y + local.y;
			fflock3ptr->atom[n].z = viewspot.z + local.z;
		}
		fflock3ptr->color[n] =
			colorize(fflock3ptr->color[n - 1] + 4);
		fflock3ptr->releasetime[n] = long_iteration;
		fflock3ptr->n++;
		saved_fflock3ptr->atom[n] = fflock3ptr->atom[n];
		saved_fflock3ptr->releasetime[n] = fflock3ptr->releasetime[n];
		saved_fflock3ptr->color[n] = fflock3ptr->color[n];
		saved_fflock3ptr->n++;
		i = ribbonindex - 1;
/* was i = ribbonindex--, I don't know why. */

		if (i < 0)
			i = ribbonlength - 1;
		/* fill the new guy's ribbon up with copies of his startpoint */
		u = projectpixel(&(fflock3ptr->atom[n]), axis);
		for (i = 0; i < ribbonlength; i++)
			ribbon[i]->atom[n] = u;
		break;
	}
	if ((dimension == YORKE || dimension == HENON) && tracetype == 1)
	{
		u = convertpair(&(fflock2ptr->atom[n]));
		for (i = 0; i < ribbonlength; i++)
			ribbon[i]->atom[n] = u;
	}

}

void addrandomflock(void)
{
	addxyflock(random(maxx), random(maxy));
}

void addflock()
{
	if (dimension == LORENZ && fflock3ptr->n >= MAXFLOCK)
		return;
	if ((dimension == YORKE || dimension == HENON) &&
	    fflock2ptr->n >= MAXFLOCK)
		return;
	if (dimension == LORENZ)
	{
		if (trihedrons_exist)
		{
			if (fflock3ptr->n == 1 && !trihedroncount)
			{
				saved_fflock3ptr->n = fflock3ptr->n = 0;	/* overwrite naked
	            		                     				 * singleton */
				long_iteration = 0;
				starting_trihedroncount = 1;
     
				installmode();
			}
			trihedroncount++;
		}
	}
	addxyflock(curx, cury);
}

void eraseribbon(int n)
{
	pair u, v;
	int i,j,k;

   /* We don't even fucking TRY if ribbon erase is off. */

   if (!tracetype)
      return;


   if (ribbonfull)
   {
      j = ribbonindex;
      for(i=0;i<ribbonlength;i++)
      {
         k = j+1;
         if (k == ribbonlength)
            k = 0;
         u = ribbon[j]->atom[n];
         v = ribbon[k]->atom[n];
			Bres(u.x, u.y, v.x, v.y, 0);
         j = k;
      }
   }
   else
   {
      for(i=0;i<ribbonindex;i++)
      {
         u = ribbon[i]->atom[n];
         v = ribbon[i+1]->atom[n];
			Bres(u.x, u.y, v.x, v.y, 0);
      }
   }
}

void erasetrihedron(int i)
{
	if (trihedrons_exist)
	{
		wBres(antintip[i].x, antintip[i].y,
		      ntip[i].x, ntip[i].y, 0, WIDTH);
		wBres(antibntip[i].x, antibntip[i].y,
		      bntip[i].x, bntip[i].y, 0, WIDTH);
		wBres(antittip[i].x, antittip[i].y,
		      ttip[i].x, ttip[i].y, 0, WIDTH);
	}
}



void restoretextmode()
{
   SetDisplay(TextPg0);
}
