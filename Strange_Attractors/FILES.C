#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>

#include "gif.h"
#include "attract.h"

static char *msgs[] =
{
	"Save Parameters     (Alt-S)",
	"Load Parameters     (Alt-L)",
	"Save image as GIF   (Alt-G)",
	"Save screen as GIF  (Alt-H)",
	"View GIF image      (Alt-F)",
	"F1 for Help",
	"ESC to Cancel",
	NULL
};

static rect fileR[10];
static rect *pfileR[10];

static void push(int n, int inout)
{
	if (n != -1)
	{
		PushButton(pfileR[n], inout);
		if (n == 6)
			ExtraHilite(pfileR[n], inout);
	}
}



void SaveImageGif(char *filename)
{
	GifOutput(filename, 0);
}

void SaveScreenGif(char *filename)
{
	GifOutput(filename, 1);
}

void ViewGif(char *filename)
{
	GifDisplay(filename);
}

void gif_viewer(void)
{
	image *saved_stamps;
	rect R;
	int found = 0;
	int mem_err = 0;
	event e;
	char chosen_file[128];
	extern int access(char *);
   union REGS regs;

	while (!mem_err &&
	       select_file("View GIF image", "*.gif", chosen_file, "GIF"))
	{
		if (access(chosen_file))
		{
         FileError(chosen_file,NULL);
         continue;
		}
		HideCursor();
		if (!found)
		{
			/* save the stamps */
			R.Xmin = stampingR[0].Xmin;
			R.Xmax = stampingR[0].Xmax;
			R.Ymin = stampingR[3].Ymin;
			R.Ymax = stampingR[0].Ymax;

			saved_stamps = malloc((int) (ImageSize(&R)));
			if (saved_stamps == NULL)
			{
				mem_err = 1;
				ErrorBox("Not enough memory to save stamps, Gif aborted.");
			}
			else
				ReadImage(&R, saved_stamps);
		}
		if (!mem_err)
		{
			BackColor(BLACK);
			EraseRect(&sR);

			if (GifDisplay(chosen_file) == GIF_COOLMAN)
			{
				sound(110);
				delay(10);
				nosound();
				while (1)
				{
					KeyEvent(true, &e);
					if ((e.State >> 8) & 7)
						break;
					if (e.ASCII || e.ScanCode)
						break;
				}
			}
			else
				mem_err = 1;

         regs.h.ah = 0;
         regs.h.al = (hasVGA) ? 0x12 : 0x10;
         int86(0x10,&regs,&regs);
			found = 1;
			ShowCursor();
			Jusepalette();

		}
	}
	if (found)
	{
		HideCursor();
      regs.h.ah = 0;
      regs.h.al = (hasVGA) ? 0x12 : 0x10;
      int86(0x10,&regs,&regs);
		EraseRect(&sR);
		installmode();
		PenColor(BLACK);
		Jusepalette();
		DrawButtons();
		if (saved_stamps)
		{
			WriteImage(&R, saved_stamps);
			free(saved_stamps);
			saved_stamps = NULL;
		}
		ShowCursor();
	}
}

static char *headertext = "CHAOS Attractor Data\n";

static char *typenames[] =
{
	"LOGISTIC",
	"HENON",
	"LORENZ",
	"YORKE"
};


void logistic_saver(FILE * fd)
{
	int i;

	fprintf(fd, "Trace type: %d\n", tracetype);
	fprintf(fd, "Start log: %d\n", startlog);
	fprintf(fd, "Stop log: %d\n", stoplog);
	fprintf(fd, "Humpspot: %g\n", humpspot);
	fprintf(fd, "Chaoticity: %g\n", lvfx);
	fprintf(fd, "Window: (%g %g) (%g %g)\n",
		flox, floy, fhix, fhiy);

	if (!tracetype)
		for (i = 0; i < XPIXELS; i++)
			fprintf(fd, "Atom %d: %g\n", i + 1, fflock1ptr->atom[i]);


}

void henon_saver(FILE * fd)
{
	/*
	 * We need the flock2 stuff here, as well as a tracetype, a ribbon
	 * length, and ha. hb is a constant.
	 */
   int i;


	fprintf(fd, "Chaoticity: %g\n", fha);
	fprintf(fd, "Trace type: %d\n", tracetype);
	fprintf(fd, "Ribbon length: %d\n", ribbonlength);
	fprintf(fd, "Window: (%g %g) (%g %g)\n",
		flox, floy, fhix, fhiy);
	fprintf(fd, "Fanciness: %d\n", fancyflag);
   fprintf(fd, "Random seed: %d\n",start_shiftregister);
   for(i=0;i<fflock2ptr->n;i++)
   {
      fpair atom = saved_fflock2ptr->atom[i];
      fprintf(fd,"Atom %d: %g %g %d %lu\n",i+1,atom.x,atom.y,
         fflock2ptr->color[i],fflock2ptr->releasetime[i]);
   }

}

void yorke_saver(FILE * fd)
{
	/*
	 * A yorke needs the contents of fflock2ptr, the tracetype, the
	 * ribbonlength, omega1, and epsilon, and a shape.
	 */

	fprintf(fd, "Epsilon: %g\n", epsilon);
	fprintf(fd, "Topology: %d\n", yorketopologyflag);
	fprintf(fd, "Trace type: %d\n", tracetype);
	fprintf(fd, "Ribbon length: %d\n", ribbonlength);
	fprintf(fd, "Omegas: %g %g\n", omega1, omega2);
	fprintf(fd, "Window: (%g %g) (%g %g)\n",
		flox, floy, fhix, fhiy);
   fprintf(fd, "Flock type: %d\n",flocktype);

}

void lorenz_saver(FILE * fd)
{
	/* Save all the particles. */
	int i;

	for (i = 0; i < fflock3ptr->n; i++)
	{
		fprintf(fd, "Atom %d: %g %g %g %d %lu\n",
			i + 1,
			saved_fflock3ptr->atom[i].x,
			saved_fflock3ptr->atom[i].y,
			saved_fflock3ptr->atom[i].z,
			saved_fflock3ptr->color[i],
			saved_fflock3ptr->releasetime[i]);
	}

	/* We also need everything that setwindow needs. */
	fprintf(fd, "Flies: %d\n", starting_trihedroncount);
	fprintf(fd, "Window: (%g %g %g) (%g %g %g)\n",
		flox, floy, floz, fhix, fhiy, fhiz);
	fprintf(fd, "View: %c\n", axis);
	fprintf(fd, "Accuracy: %g\n", dt);

   /* We really need to calculate a fresh flyspot here. */
#ifdef HMM
   if (view == 'w')
   {
      if (trihedroncount == 0)
      {
         ftriple fz,fw;
         int color;
         fw = fflock3ptr->atom[fflock3ptr->n-1];
   		color = (int) (fflock3ptr->color[i]);
	   	fz = rk4lorenzimage(&fw, &color);
#endif

         

	fprintf(fd, "Fly spot: %g %g %g\n",
		viewspot.x, viewspot.y, viewspot.z);
	fprintf(fd, "Fly normal: %g %g %g\n",
		viewnormal.x, viewnormal.y, viewnormal.z);
	fprintf(fd, "Fly binormal: %g %g %g\n",
		viewbinormal.x, viewbinormal.y, viewbinormal.z);
   fprintf(fd, "Fly spot: %g %g %g\n",viewspot.x,viewspot.y,viewspot.z);
}



void logistic_loader(FILE * fd)
{
	double tx;
	double tlox, thix, tloy, thiy;
	unsigned ti;
	char tbuf[256];

	while (1)
	{
		fgets(tbuf, 256, fd);
		if (feof(fd))
			break;
      if (strlen(tbuf) == 0)
         continue;
      if (tbuf[0] == '#')
         continue;

		if (sscanf(tbuf, "Atom %d: %lg", &ti, &tx) == 2)
		{
			ti--;
			fflock1ptr->atom[ti] = tx;
		}
		else if (sscanf(tbuf, "Trace type: %d", &ti))
			tracetype = ti;
		else if (sscanf(tbuf, "Start log: %d", &ti))
			startlog = ti;
		else if (sscanf(tbuf, "Stop log: %d", &ti))
			stoplog = ti;
		else if (sscanf(tbuf, "Humpspot: %lg", &tx))
		{
			humpspot = tx;
			humpshift = log(0.5) / log(humpspot);
		}
		else if (sscanf(tbuf, "Chaoticity: %lg", &tx))
			lvfx = tx;
		else if (sscanf(tbuf, "Window: (%lg %lg) (%lg %lg)",
				&tlox, &tloy, &thix, &thiy) == 4)
		{
         if ((fabs(thix-tlox) < 1.0e-6) ||
             (fabs(thiy-tlox) < 1.0e-6) ||
             (thix <= tlox) ||
             (thiy <= tloy))
         {
            ErrorBox("Window values out of range; using old window");
            continue;
         }
			flox = tlox;
			floy = tloy;
			fhix = thix;
			fhiy = thiy;

		}
	}

	dimension = LOGISTIC;
	iteration = 0;
	installmode();
	logisticlaunchflag = 1;

	/* Can't use setwindow, fucks with fx etc. */
	fdeltax = fhix - flox;
	fdeltay = fhiy - floy;
	fdeltaz = fhiz - floz;

	fdeltax_div_64K = fdeltax / 0x10000L;
	fdeltay_div_64K = fdeltay / 0x10000L;
	fdeltaz_div_64K = fdeltaz / 0x10000L;

	xscale = maxx / (fhix - flox);
	yscale = maxy / (fhiy - floy);


	fcenterx = flox + fdeltax / 2;
	fcentery = floy + fdeltay / 2;
	fcenterz = floz + fdeltaz / 2;
	pixelx = 0;
	fxstep = (fhix - flox) / maxx;
	fystep = (fhiy - floy) / maxy;
}

void henon_loader(FILE * fd)
{
	double tx,ty;
	double tlox, thix, tloy, thiy;
   unsigned tc;
   unsigned long tl;
	unsigned ti;
	char tbuf[256];
   int my_flocktype = -1;
   int maxread = 0;
   int max0=0;

	while (1)
	{
		fgets(tbuf, 256, fd);
		if (feof(fd))
			break;
      if (strlen(tbuf) == 0)
         continue;
      if (tbuf[0] == '#')
         continue;

      if (sscanf(tbuf, "Flock type: %d",&ti))
         my_flocktype = ti;

		else if (sscanf(tbuf, "Trace type: %d", &ti))
			tracetype = ti;
		else if (sscanf(tbuf, "Ribbon length: %d", &ti))
			ribbonlength = ti;
		else if (sscanf(tbuf, "Window: (%lg %lg) (%lg %lg)",
				&tlox, &tloy, &thix, &thiy) == 4)
		{
         if ((fabs(thix-tlox) < 1.0e-6) ||
             (fabs(thiy-tlox) < 1.0e-6) ||
             (thix <= tlox) ||
             (thiy <= tloy))
         {
            ErrorBox("Window values out of range; using old window");
            continue;
         }
			flox = tlox;
			floy = tloy;
			fhix = thix;
			fhiy = thiy;
		}
		else if (sscanf(tbuf, "Fanciness: %d", &ti))
			fancyflag = ti;
      else if (sscanf(tbuf, "Chaoticity: %lg",&tx))
         fha = tx;
      else if (sscanf(tbuf, "Random seed: %d",&ti))
         shiftregister = ti;
      else if (sscanf(tbuf, "Atom %d: %lg %lg %d %lu",&ti,&tx,&ty,&tc,&tl) == 5)
      {
         int n = ti - 1;
         if (n < MAXFLOCK)
         {
            fflock2ptr->atom[n].x = tx;
            fflock2ptr->atom[n].y = ty;
			   fflock2ptr->color[n] = tc;
			   fflock2ptr->releasetime[n] = tl;
            maxread = max(maxread,ti);
			   if (tl == 0)
				   max0 = max(n, max0);
         }
      }


	}

	/* Start up the henonizer... */
	dimension = HENON;
	installmode();
	cosa = cos((fha * M_PI) / 4.0);
	sina = sin((fha * M_PI) / 4.0);
   next_release = 0L;
   long_iteration = 0L;
   start_shiftregister = shiftregister;
   sixteen_count = 0;
   if (maxread < 1 || my_flocktype != -1)
      fillflock();
   else
   {
      fflock2ptr->atom[maxread+1].x = -9999;
      fflock2ptr->n = max0 + 1;
      if (max0 != maxread)
         next_release = fflock2ptr->releasetime[max0 + 1];
   }
	setwindow(0);
	ribbonfull = 0;
	ribbonindex = 0;

	*saved_fflock2ptr = *fflock2ptr;



}

void yorke_loader(FILE * fd)
{
	double tx, ty;
	double tlox, thix, tloy, thiy;
	unsigned ti;
	char tbuf[256];

	while (1)
	{
		fgets(tbuf, 256, fd);
		if (feof(fd))
			break;
      if (strlen(tbuf) == 0)
         continue;
      if (tbuf[0] == '#')
         continue;

		if (sscanf(tbuf, "Flock type: %d",&ti))
         flocktype = ti;
		else if (sscanf(tbuf, "Epsilon: %lg", &tx))
			epsilon = tx;
		else if (sscanf(tbuf, "Topology: %d", &ti))
			yorketopologyflag = ti;
		else if (sscanf(tbuf, "Trace type: %d", &ti))
			tracetype = ti;
		else if (sscanf(tbuf, "Ribbon length: %d", &ti))
			ribbonlength = ti;
		else if (sscanf(tbuf, "Omegas: %lg %lg", &tx, &ty) == 2)
		{
			omega1 = tx;
			omega2 = ty;
		}
		else if (sscanf(tbuf, "Window: (%lg %lg) (%lg %lg)",
				&tlox, &tloy, &thix, &thiy) == 4)
		{
         if ((fabs(thix-tlox) < 1.0e-6) ||
             (fabs(thiy-tlox) < 1.0e-6) ||
             (thix <= tlox) ||
             (thiy <= tloy))
         {
            ErrorBox("Window values out of range; using old window");
            continue;
         }
			flox = tlox;
			floy = tloy;
			fhix = thix;
			fhiy = thiy;
		}


	}
	dimension = YORKE;
	installmode();
	setwindow(0);
	epsbar = epsilon / TWOPI;
   fancyflag = 1;
   fillflock();
	*saved_fflock2ptr = *fflock2ptr;

}

void lorenz_loader(FILE * fd)
{
	double tx, ty, tz;
	double tlox, thix, tloy, thiy, tloz, thiz;
	unsigned long tl;
	unsigned tc;
	unsigned ti;
	char tbuf[256];
	int initial_axis = axis;
	int maxread = 0;
	int max0 = 0;

	while (1)
	{
		fgets(tbuf, 256, fd);
		if (feof(fd))
			break;
      if (strlen(tbuf) == 0)
         continue;
      if (tbuf[0] == '#')
         continue;

		if (sscanf(tbuf, "Atom %d: %lg %lg %lg %d %lu",
			   &ti, &tx, &ty, &tz, &tc, &tl) == 6)
		{
			ti--;
         if (ti < MAXFLOCK)
         {
			   maxread = max(ti, maxread);
			   if (tl == 0)
				   max0 = max(ti, max0);
			   fflock3ptr->atom[ti].x = tx;
			   fflock3ptr->atom[ti].y = ty;
			   fflock3ptr->atom[ti].z = tz;
			   fflock3ptr->color[ti] = tc;
			   fflock3ptr->releasetime[ti] = tl;
         }
		}
		else if (sscanf(tbuf, "Window: (%lg %lg %lg) (%lg %lg %lg)",
			     &tlox, &tloy, &tloz, &thix, &thiy, &thiz) == 6)
		{
         if ((fabs(thix-tlox) < 1.0e-6) ||
             (fabs(thiy-tlox) < 1.0e-6) ||
             (fabs(thiz-tloz) < 1.0e-6) ||
             (thix <= tlox) ||
             (thiz <= tloz) ||
             (thiy <= tloy))
         {
            ErrorBox("Window values out of range; using old window");
            continue;
         }
			flox = tlox;
			floy = tloy;
			floz = tloz;
			fhix = thix;
			fhiy = thiy;
			fhiz = thiz;
		}
		else if (sscanf(tbuf, "Flies: %d", &ti))
			trihedroncount = ti;
		else if (sscanf(tbuf, "View: %c", &tc))
			axis = tc;
		else if (sscanf(tbuf, "Accuracy: %lg", &tx))
			dt = tx;
		else if (sscanf(tbuf, "Fly spot: %lg %lg %lg", &tx, &ty, &tz) == 3)
			viewspot.x = tx, viewspot.y = ty, viewspot.z = tz;
		else if (sscanf(tbuf, "Fly normal: %lg %lg %lg", &tx, &ty, &tz) == 3)
			viewnormal.x = tx, viewnormal.y = ty, viewnormal.z = tz;
		else if (sscanf(tbuf, "Fly binormal: %lg %lg %lg", &tx, &ty, &tz) == 3)
			viewbinormal.x = tx, viewbinormal.y = ty, viewbinormal.z = tz;
	}

	fflock3ptr->atom[maxread + 1].x = 9999;
	fflock3ptr->n = max0 + 1;
	long_iteration = 0;
	if (max0 == maxread)
		next_release = 0;
	else
		next_release = fflock3ptr->releasetime[max0 + 1];

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
	dimension = LORENZ;
	installmode();
	setwindow(0);


}

typedef void (*pff) (FILE *);

pff savers[] =
{
	logistic_saver,
	henon_saver,
	lorenz_saver,
	yorke_saver
};

pff loaders[] =
{
	logistic_loader,
	henon_loader,
	lorenz_loader,
	yorke_loader
};

void LoadParams(char *filename)
{
	FILE *fd;
	char tbuf[256];
	char sbuf[256];
	int i;

	fd = fopen(filename, "rt");
	if (!fd)
	{
      FileError(filename,NULL);
		return;
	}

	while (1)
	{
		fgets(tbuf, 256, fd);
		if (feof(fd))
			break;
      if (strlen(tbuf) == 0)
         continue;
      if (tbuf[0] == '#')
         continue;

		if (sscanf(tbuf, "Type: %s", sbuf) == 1)
		{
			for (i = 0; i < 4; i++)
			{
				if (!strcmp(sbuf, typenames[i]))
				{
					(*loaders[i]) (fd);
               fclose(fd);
               return;
				}
			}
		}
	}
}


void SaveParams(char *filename)
{
	FILE *fd;
	time_t timex;

	fd = fopen(filename, "wt");
	if (!fd)
	{
      FileError(filename,NULL);
		return;
	}

	fprintf(fd, headertext);
	timex = time(NULL);
	fprintf(fd, "Created: %s", ctime(&timex));
	fprintf(fd, "Type: %s\n", typenames[dimension - 1]);

	(*savers[dimension - 1]) (fd);



   if (fflush(fd) == EOF || ferror(fd))
   {
      FileError(filename,fd);
      fclose(fd);
      remove(filename);
   }
   else
      fclose(fd);

}

void do_files_menu(void)
{

	int i;

	rect tR;
	int key;
	int height;
	event e;
	int items;
	int current_item;
	int row;
	int err;
	int width;
	short X, Y;
	int found = 0;
	char chosen_file[128];
	static int filekeys[] = {XALTS, XALTL, XALTG, XALTH, XALTF};

	HideCursor();

	height = FontHeight + 4;
	width = 0;

	for (items = 0; msgs[items]; items++)
		width = max(width, StringWidth(msgs[items]) + 16);

	height = items * (3 * FontHeight / 2) + FontHeight / 2 + FontHeight + 8;

	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "File Menu", BLACK);

	PushMouseRectLimit(&tR);
	PushCursorPosition();

	row = FontHeight + 4 + tR.Ymin;

	for (i = 0; i < items; i++)
	{
		rect R;

		if (i == items - 2)
			row += FontHeight / 2;
		R.Xmin = tR.Xmin + 4;
		R.Xmax = tR.Xmax - 4;
		R.Ymin = row;
		R.Ymax = row + FontHeight + 4;

		fileR[i] = R;
		pfileR[i] = &fileR[i];

		PaintRadioButton(&R, false, false, msgs[i]);
		row += 3 * FontHeight / 2;
	}

	current_item = items - 1;
	push(current_item, true);
	move_to_corner(&fileR[current_item]);
	ShowCursor();

	while (1)
	{
		int button;
		int n = KeyEvent(false, &e);
		int last_item = current_item;

		button = e.State >> 8;

		key = 0;
		X = e.CursorX;
		Y = e.CursorY;

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else
				key = e.ScanCode << 8;

			if (button == swLeft)
				key = 0x0d;
			if (button == swRight)
				key = 0x1b;
		}
		else
		{
			current_item = -1;
			/* drag the cursor along */
			for (i = 0; msgs[i]; i++)
				if (XYInRect(X, Y, &fileR[i]))
					current_item = i;
		}

		for (i = 0; i < 5; i++)
		{
			if (filekeys[i] == key)
			{
				current_item = i;
				key = 0x0d;
				break;
			}
		}

		navigate(key, NULL, NULL, (int *) -1, (int *) -1, items, pfileR, &current_item);
		if (key == 0x1b)
			current_item = items - 1;

		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}
		if (key == 0x1b)
			break;

		if (key == XF1 || (key == 0x0d && current_item == items - 2))
		{
			helptext("ATTFILE.HLP");
			continue;
		}
		if (key == 0x0d && current_item != -1)
		{
			found = true;
			break;
		}

	}
	PaintRadioButtonBase(pfileR[current_item], true, true, msgs[current_item], DARKGRAY, RED, WHITE);
	if (current_item == items - 1)
		ExtraHilite(pfileR[items - 1], true);
	DoublePress(pfileR[current_item], true, RED);
	WaitForNothing();
	PaintRadioButtonBase(pfileR[current_item], false, false, msgs[current_item], DARKGRAY, RED, WHITE);
	if (current_item == items - 1)
		ExtraHilite(pfileR[items - 1], false);
                               
	HideCursor();
	PopRect(&err);
	ShowCursor();
	PopCursorPosition();
	PopMouseRect();

	if (found)
	{

		/* we've got something to do */
		switch (current_item)
		{
		case 0:
			if (select_file("Save Parameters", "*.sap", chosen_file, "SAP") && Overwrite(chosen_file))
				SaveParams(chosen_file);
			break;

		case 1:
			if (select_file("Load Parameters", "*.sap", chosen_file, "SAP"))
				LoadParams(chosen_file);
			ribbonindex = 0;
			ribbonfull = 0;
			installmode();
			setwindow(0);
			break;

		case 2:
			if (select_file("Save Image", "*.gif", chosen_file, "GIF") && Overwrite(chosen_file))
				SaveImageGif(chosen_file);
			break;

		case 3:
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF") && Overwrite(chosen_file))
				SaveScreenGif(chosen_file);
			break;

		case 4:
			gif_viewer();
			break;

		}
	}

}
