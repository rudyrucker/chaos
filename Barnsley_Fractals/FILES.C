#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <sys\stat.h>
#include <dir.h>
#include <time.h>
#include <io.h>

#include "game.h"
#include "gif.h"

int rescaleflag = 0;
static int items;
static char *file_texts[] =
{
	"Save Parameters    (Alt-S)",
	"Load Parameters    (Alt-L)",
	"Save image as GIF  (Alt-G)",
	"Save screen as GIF (Alt-H)",
	"View GIF image     (Alt-F)",
	"F1 for HELP",
	"ESC to Cancel",
	NULL
};

static int file_keys[] = {XALTS, XALTL, XALTG, XALTH, XALTF};


static rect fileR[10];
static rect *pfileR[10];

static void push(int n, int inout)
{
	if (n == -1)
		return;

	PushButton(pfileR[n], inout);
	if (n == items - 1)
		ExtraHilite(pfileR[n], inout);
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

static char *headertext = "The Chaos Game IFS Data\n";
static void invalidIFS(char *reason, char *name)
{
	char tbuf[128];

	sprintf(tbuf, "Invalid IFS file %s: %s", name, reason);
	ErrorBox(tbuf);
}

int LoadParams(char *filename)
{
	FILE *fd;
	char tbuf[256];
	int highest = 0;
	ffunctionsystem fb;
	int used[MAXBARNMAPS];
	double total;
	int i;
	double version = 0.0;

	if ((fd = OpenWithError(filename, "rt")) == NULL)
		return false;
	memset(used, 0, sizeof used);

	while (1)
	{
		double a, b, c, d, e, f, w;
		int t;


		fgets(tbuf, 256, fd);

		if (feof(fd))
			break;
		if (tbuf[0] == '#')
			continue;

		if (sscanf(tbuf, "Version %lg", &a) == 1)
			version = a;


		if (sscanf(tbuf, "Map %d: %lg %lg %lg %lg %lg %lg %lg",
			   &t, &a, &b, &c, &d, &e, &f, &w) == 8)
		{
			if (version != 0.0)
				t--;

			if (t >= MAXBARNMAPS)
			{
				sprintf(tbuf, "map %d found, limit is %d", t, MAXBARNMAPS - 1);
				invalidIFS(tbuf, filename);
				fclose(fd);
				return false;
			}
			if (used[t])
			{
				sprintf(tbuf, "multiple entry %d", t);
				invalidIFS(tbuf, filename);
				fclose(fd);
				return false;
			}

			highest = max(highest, t);
			fb.h[t].a = a;
			fb.h[t].b = b;
			fb.h[t].c = c;
			fb.h[t].d = d;
			fb.h[t].e = e;
			fb.h[t].f = f;
			fb.weight[t] = w;
			used[t] = true;
		}
		else if (sscanf(tbuf, "Window: (%lg %lg) (%lg %lg)",
				&a, &b, &c, &d) == 4)
		{
			if (fabs(a - b) < FDELTAXMINIMUM)
				ErrorBox("Window too small; ignoring");
			else if (fabs(a - b) > FDELTAXLIMIT)
				ErrorBox("Window too large; ignoring");
			else
			{
            extern void maybe_change_modes(int flocker);
				fstartlox     = flox = a;
				fstartloy     = floy = b;
				fstarthix     = fhix = c;
				fstarthiy     = fhiy = d;
				fstartdeltax  = fdeltax = fhix - flox;
				fstartdeltay  = fdeltay = fhiy - floy;
				fstartcenterx = fcenterx = flox + fdeltax / 2;
				fstartcentery = fcentery = floy + fdeltay / 2;

            /* set the float flag here */

			}
		}
		else if (sscanf(tbuf, "Index Window: (%lg %lg) (%lg %lg)",
				&a, &b, &c, &d) == 4)
		{
			if (fabs(a - b) < FDELTAXMINIMUM)
				ErrorBox("Index window too small; ignoring");
			else if (fabs(a - b) > FDELTAXLIMIT)
				ErrorBox("Index window too large; ignoring");
			else
			{
				fstartlox = a;
				fstartloy = b;
				fstarthix = c;
				fstarthiy = d;
				fstartdeltax = fstarthix - fstartlox;
				fstartdeltay = fstarthiy - fstartloy;
				fstartcenterx = fstartlox + fstartdeltax / 2;
				fstartcentery = fstartloy + fstartdeltay / 2;

			}
		}

	}
	fclose(fd);

	if (!highest)
	{
		sprintf(tbuf, "two maps needed, only %d present", highest + 1);
		invalidIFS(tbuf, filename);
		return false;
	}
	else
	{
		char name[30], ext[20];

		total = 0;
		for (i = 0; i <= highest; i++)
		{
			if (!used[i])
			{
				sprintf(tbuf, "missing entry %d", i);
				invalidIFS(tbuf, filename);
				return false;
			}
			total += fb.weight[i];
		}

		if (fabs(total - 1.0) > 1.0e-6)
			ErrorBox("Warning: Total weight is not 1.0; weights normalized.");


		fb.n = highest + 1;

		*fBptr = fb;
		w_normalize(fBptr->weight, fBptr->weight);

      lox = BLOAT * flox;
      hix = BLOAT * fhix;
      loy = BLOAT * floy;
      hiy = BLOAT * fhiy;
      deltax = hix - lox;
      deltay = hiy - loy;
		startlox = lox = BLOAT * flox;
		starthix = hix = BLOAT * fhix;
		starthiy = loy = BLOAT * floy;
		startloy = hiy = BLOAT * fhiy;
      centerx = lox + deltax / 2;
      centery = loy + deltay / 2;

      startlox = BLOAT * fstartlox;
      starthix = BLOAT * fstarthix;
      startloy = BLOAT * fstartloy;
      starthiy = BLOAT * fstarthiy;
      startdeltax = starthix - startlox;
      startdeltay = starthiy - startloy;
      startcenterx = startlox + startdeltax / 2;
      startcentery = startloy + startdeltay / 2;

      /* save the cursor position... */
		fnsplit(filename, NULL, NULL, name, ext);
		sprintf(current_barnmap_name, "%s%s", name, ext);




		return true;
	}

}


void SaveParams(char *filename)
{
	FILE *fd;
	time_t timex;
	int i;
   int ok = true;


	fd = fopen(filename, "wt");	/* Has to be openable and writable
					 * here; we've passed Overwrite. */

   if (!fd)
   {
      FileError(filename,fd);
      return;
   }
	fprintf(fd, headertext);
	timex = time(NULL);
	ok &= EOF != fprintf(fd, "Created: %s", ctime(&timex));
	ok &= EOF != fprintf(fd, "Version %f\n", VERSION);
	for (i = 0; ok && i < fBptr->n; i++)
	{
		double *d = (double *) &fBptr->h[i];
		int j;

		ok &= EOF != fprintf(fd, "Map %d: ", i + 1);
		for (j = 0; j < 6; j++)
			fprintf(fd, "%g ", d[j]);
		ok &= EOF != fprintf(fd, "%g\n", fBptr->weight[i]);
	}

	ok &= EOF != fprintf(fd, "Window: (%.20g %.20g) (%.20g %.20g)\n", flox, floy, fhix, fhiy);
	ok &= EOF != fprintf(fd, "Index Window: (%.20g %.20g) (%.20g %.20g)\n", fstartlox, fstartloy, fstarthix, fstarthiy);


   ok &= EOF != fflush(fd);

   if (!ok)
   {
      FileError(filename,fd);
      fclose(fd);
      remove(filename);
   }
   else
   	fclose(fd);
}
void load_parameters(void)
{
	char chosen_file[128];


	if (select_file("Load Parameters", "*.ifs", chosen_file, "IFS"))
	{
		invert_main_item(current_main_item);
		if (LoadParams(chosen_file))
		{
         maybe_change_modes(false);
         Bfint(fBptr,Bptr);
         fillfixedpoints(fBptr);
         dozoom(-4);

//			_activate(fBptr);
			edmap = 0;
			if (tweaking)
			{
				if (fBptr->n < 6)
				{
					if (current_main_item == LEFTARROWBOX)
						current_main_item++;
					while (current_main_item - LEFTARROWBOX > fBptr->n)
						current_main_item--;
				}
				FirstMap = 0;
				ShowAll();
				UpdateAllParams(edmap);
			}
			ClearBarnmapStack();
		}
		invert_main_item(current_main_item);
	}
}
void gif_viewer(void)
{
	int found = 0;
	int mem_err = 0;
	char chosen_file[128];
	char tbuf[128];
   int i,j;

	event e;

	while (!mem_err &&
	       select_file("View GIF image", "*.gif", chosen_file, "GIF"))
	{

		union REGS regs;

		/* check to make sure the chosen file exists. */
		if (access(chosen_file, 0))
		{
         FileError(tbuf,NULL);
		}
		else
		{
			HideCursor();
         if (!found)
         {
            /* Hide the stamps! */
            for(i=0;i<4;i++)
               PushRect(&stamp_rects[i],&j);
         }

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
			found = 1;
			regs.h.ah = 0;
			regs.h.al = (hasVGA) ? 0x12 : 0x10;
			int86(0x10, &regs, &regs);

			usepalette();

			ShowCursor();
		}
	}

	if (found)
	{
		HideCursor();
		/* restore the screen to how we like it */
		BackColor(BLACK);
		EraseRect(&sR);
		usepalette();
		DrawButtons();
		if (tweaking)
			tweak(edmap);
		install_fill_outline();
      for(i=0;i<4;i++)
         PopRect(&j);
		ShowCursor();
	}
}

void do_files_menu(void)
{

	int i;

	rect tR;
	int key;
	int centerx, centery;
	int height;
	event e;
	int current_item;
	int row;
	int err;
	int width;
	short X, Y;
	int found = 0;
	static int hotkeys[] = {XALTS, XALTL, XALTG, XALTF, 'a', 'v', 'x'};

	PushCursorPosition();
	HideCursor();

	Centers(&sR, &centerx, &centery);

	width = 0;

	for (items = 0; file_texts[items]; items++)
		width = max(width, StringWidth(file_texts[items]) + 16);

	width += 4;

	height = items * (3 * FontHeight / 2) + FontHeight / 2 + FontHeight + 8;

	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "File Menu", BLACK);

	centerx = tR.Xmin + (tR.Xmax - tR.Xmin) / 2;

	PushMouseRectLimit(&tR);

	row = FontHeight + 4 + tR.Ymin;

	for (i = 0; i < items; i++)
	{
		rect R;

		if (i == 5)
			row += FontHeight / 2;

		R.Xmin = tR.Xmin + 4;
		R.Xmax = tR.Xmax - 4;
		R.Ymin = row;
		R.Ymax = R.Ymin + FontHeight + 4;

		fileR[i] = R;
		pfileR[i] = &fileR[i];
		PaintRadioButton(&R, false, false, file_texts[i]);
		row += 3 * FontHeight / 2;
	}
	current_item = items - 1;
	push(current_item, true);
	move_to_corner(pfileR[current_item]);
	ShowCursor();

	while (!found)
	{
		int button;
		int n = KeyEvent(false, &e);
		int last_item = current_item;

		button = e.State >> 8;

		key = 0;
		X = e.CursorX;
		Y = e.CursorY;
		current_item = -1;

		/* drag the cursor along */
		for (i = 0; file_texts[i]; i++)
			if (XYInRect(X, Y, &fileR[i]))
				current_item = i;

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else
				key = e.ScanCode << 8;

			if ((button == swLeft || key == 0x0d) && current_item != -1)
			{
				found = 1;
				key = 0x0d;
			}

			if (key == XF1 || (found && current_item == items - 2))
			{
				PushCursorPosition();
				helptext("gamefile.hlp");
				LimitMouse(tR.Xmin, tR.Ymin, tR.Xmax, tR.Ymax);
				PopCursorPosition();
				found = 0;
			}

			if (button == swRight || key == 0x1b)
			{
				key = 0x0d;
				current_item = items - 1;
			}
		}

		for (i = 0; i < 7; i++)
		{
			if (key == hotkeys[i])
			{
				found = true;
				current_item = i;
				break;
			}
		}

		navigate(key, NULL, NULL, (int *) -1, (int *) -1, items, pfileR, &current_item);

		for (i = 0; i < 5; i++)
		{
			if (key == file_keys[i])
			{
				current_item = i;
				found = 1;
				break;
			}
		}


		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}

		if ((key == 0x0d && current_item == items - 1) || found)
			break;

	}

	PaintRadioButton(pfileR[current_item], true, true, file_texts[current_item]);
	WaitForNothing();
	PaintRadioButton(pfileR[current_item], false, false, file_texts[current_item]);


	HideCursor();
	PopRect(&err);
	ShowCursor();

	if (found)
	{
		char chosen_file[128];

		/* we've got something to do */
		switch (current_item)
		{
		case 0:
			if (select_file("Save Parameters", "*.ifs", chosen_file, "IFS") && Overwrite(chosen_file))
				SaveParams(chosen_file);
			break;

		case 1:

			load_parameters();
			break;

		case 2:
			if (select_file("Save Image", "*.gif", chosen_file, "GIF"))
				if (Overwrite(chosen_file))
					SaveImageGif(chosen_file);
			break;

		case 3:
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF"))
				if (Overwrite(chosen_file))
					SaveScreenGif(chosen_file);
			break;

		case 4:
			gif_viewer();
			break;
		}
	}
	WaitForNothing();
	PopCursorPosition();
	PopMouseRect();
}
