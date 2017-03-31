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

#include<stdio.h>;
#include<stdlib.h>;
#include<conio.h>;
#include<math.h>
#include <dos.h>
#include <dir.h>
#include "scodes.h"
#include <alloc.h>
#include <time.h>
#include <ctype.h>
#include <sys\stat.h>
#include <string.h>
#include <mem.h>
#include <time.h>
#include <io.h>

#include "mand.h"
#include "gif.h"

static char *headertext = "CHAOS Fractal Data\n";
extern double flox, fhix, floy, fhiy, fu, fv, fa, fb;
char *vflagstonames(stampdata * d);

void load_params(char *filename)
{
	FILE *fd;
	char tbuf[512];
	char sbuf[128];
	double ft;
	double centerx;
	double centery;
	double width;
	double tu, tv, ta, tb;
	int n;
	char *p;

	pushstamp(&stampviews[saveptr]);
	fd = fopen(filename, "rt");
	if (!fd)
	{
		sprintf(tbuf, "%s: %s", filename, strerror(errno));
		tbuf[strlen(tbuf) - 1] = 0;
		ErrorBox(tbuf);
		return;
	}
	tu = tv = ta = tb = centerx = centery = width = 0.0;

	while (1)
	{
		fgets(tbuf, 512, fd);
		if (feof(fd))
			break;

		if (strlen(tbuf) == 0)
			continue;

		if (tbuf[0] == '#')
			continue;

		tbuf[strlen(tbuf) - 1] = 0;	/* kill the fucking trailing
						 * \n */

		if (sscanf(tbuf, "Type: %s", sbuf))
			namestoflags(tbuf + 6);
		else if (sscanf(tbuf, "Center X: %lg", &ft))
			centerx = ft;
		else if (sscanf(tbuf, "Center Y: %lg", &ft))
			centery = ft;
		else if (sscanf(tbuf, "Width: %lg", &ft))
		{
			if (ft > 1.0e-13)
				width = ft;
			else
			{
				sprintf(tbuf, "Width of %g encountered; minimum is 1.0e-13", ft);
				ErrorBox(tbuf);
				width = 1.0e-13;
			}
		}
		else if (sscanf(tbuf, "Slicetype: %s", sbuf))
			slicetype = slicenametoslicetype(sbuf);
		else if (sscanf(tbuf, "Filltype: %s", sbuf))
			insideflag = nametofill(sbuf);
		else if (sscanf(tbuf, "U: %lg", &ft))
			tu = ft;
		else if (sscanf(tbuf, "V: %lg", &ft))
			tv = ft;
		else if (sscanf(tbuf, "A: %lg", &ft))
			ta = ft;
		else if (sscanf(tbuf, "B: %lg", &ft))
			tb = ft;
		else if (sscanf(tbuf, "Iterations: %d", &n))
			maxiteration = n;
	}

	fu = tu;
	fv = tv;
	fa = ta;
	fb = tb;

	flox = centerx - width / 2.0;
	fhix = centerx + width / 2.0;

	floy = centery - (4.5 / 5.6) * width / 2.0;
	fhiy = centery + (4.5 / 5.6) * width / 2.0;

	SetUpProperView();
	ParameterDisplayMode = 0;
	useview();
	strcpy(tbuf, filename);
	p = strrchr(tbuf, '\\');
	if (!p)
		p = tbuf;
	else
		p++;

	menutext(p);
}

void save_params(char *filename)
{
	FILE *fd;
	time_t timex;
	int ok = true;

	fd = fopen(filename, "wt");

	if (!fd)
	{
		FileError(filename, NULL);
		return;
	}


	ok &= EOF != fprintf(fd, headertext);
	timex = time(NULL);

	ok &= EOF != fprintf(fd, "Created: %s", ctime(&timex));
	ok &= EOF != fprintf(fd, "Type: %s\n", flagstonames());
	ok &= EOF != fprintf(fd, "Filltype: %s\n", fillname_t[insideflag]);
	ok &= EOF != fprintf(fd, "Slicetype: %s\n", slicename_t[slicetype]);
	ok &= EOF != fprintf(fd, "Center X: %0.10g\n", flox + (fhix - flox) / 2.0);
	ok &= EOF != fprintf(fd, "Center Y: %0.10g\n", floy + (fhiy - floy) / 2.0);
	ok &= EOF != fprintf(fd, "Width: %0.10g\n", fhix - flox);
	ok &= EOF != fprintf(fd, "Iterations: %d\n", maxiteration);
	ok &= EOF != fprintf(fd, "U: %0.10g\n", startfu);
	ok &= EOF != fprintf(fd, "V: %0.10g\n", startfv);
	ok &= EOF != fprintf(fd, "A: %0.10g\n", startfa);
	ok &= EOF != fprintf(fd, "B: %0.10g\n", startfb);

	ok &= EOF != fflush(fd);

	if (!ok || ferror(fd))
	{
		FileError(filename, fd);
		fclose(fd);
		remove(filename);
	}
	else
		fclose(fd);

}
int gif_display_page = 0x800;
void gif_viewer(void)
{
	int found = 0;
	int mem_err = 0;
	char chosen_file[128];
	event e;

	while (!mem_err &&
	       select_file("View GIF image", "*.gif", chosen_file, "GIF"))
	{
		union REGS regs;

		/* make sure the file exists */
		if (access(chosen_file, 0))
		{
			char tbuf[128];

			sprintf(tbuf, "%s: %s", chosen_file, strerror(errno));
			tbuf[strlen(tbuf) - 1] = 0;
			ErrorBox(tbuf);
			continue;
		}


		HideCursor();
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

	if (found)
	{
		/* restore the screen to how we like it */
		union REGS regs;

		HideCursor();
		regs.h.ah = 0;
		regs.h.al = (hasVGA) ? 0x12 : 0x10;
		int86(0x10, &regs, &regs);
		draw_buttons();
		useview();
		resetcursor();
		ShowCursor();
	}
}

void do_files_menu(void)
{
	int width = 0;
	int height = 7 * (3 * FontHeight / 2) + FontHeight * 2 + 8;
	int i;
	int current_item;
	int doit = false;
	rect tR, R;
	rect bR[10];
	rect *bRp[10];
	int items = 7;
	static int filekeys[] = {XALTS, XALTL, XALTG, XALTH, XALTF};

	static char *msgs[] = {
		"Save Parameters    (Alt-S)",
		"Load Parameters    (Alt-L)",
		"Save image as GIF  (Alt-G)",
		"Save screen as GIF (Alt-H)",
		"View GIF image     (Alt-F)",
		"F1 for Help",
		"ESC to Cancel",
		NULL
	};


	for (i = 0; i < 7; i++)
		width = max(width, StringWidth(msgs[i]) + 24);

	HideCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "File Menu", BLACK);
	ArrowCursor();
	ProtectOff();

	for (i = 0; msgs[i]; i++)
	{
		R.Xmin = tR.Xmin + 4;
		R.Xmax = tR.Xmax - 4;
		R.Ymin = tR.Ymin + FontHeight + 4 + i * (3 * FontHeight / 2);
		if (i >= items - 2)
			R.Ymin += 8;
		R.Ymax = R.Ymin + FontHeight + 4;

		bR[i] = R;
		bRp[i] = &bR[i];

		PaintRadioButtonBase(&R, false, false, msgs[i], DARKGRAY, RED, WHITE);
	}

	LimitMouseRect(&tR);
	current_item = items - 1;
	move_to_corner(bRp[current_item]);
	PushButton(bRp[current_item], true);
	ExtraHilite(bRp[current_item], true);
	ShowCursor();
	LimitMouseRect(&tR);

	while (!doit)
	{
		event e;
		int n;
		int X, Y;
		int button;
		int key = 0;
		int last_item = current_item;

		n = KeyEvent(false, &e);
		X = e.CursorX;
		Y = e.CursorY;

		button = (e.State & 0x700) >> 8;

		current_item = -1;
		for (i = 0; i < items; i++)
		{
			if (XYInRect(X, Y, bRp[i]))
			{
				current_item = i;
				break;
			}
		}
		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else if (e.ScanCode != 0xff)
				key = e.ScanCode << 8;

			if (button == swRight)
				key = 0x1b;

			if (button == swLeft)
				key = 0x0d;
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

		navigate(key, NULL, NULL, (int *) -1, (int *) -1, items, bRp, &current_item);
		if (key == 0x1b)
			current_item = items - 1;

		if (last_item != current_item)
		{
			if (last_item != -1)
			{
				PushButton(bRp[last_item], false);
				if (last_item == items - 1)
					ExtraHilite(bRp[last_item], false);
			}
			if (current_item != -1)
			{
				PushButton(bRp[current_item], true);
				if (current_item == items - 1)
					ExtraHilite(bRp[current_item], true);
			}
		}

		if (key == 0x1b)
			break;

		if (key == XF1 || (key == 0x0d && current_item == items - 2))
		{
			PushCursorPosition();
			helptext("MANDFILE.HLP");
			PopCursorPosition();
			LimitMouseRect(&tR);
			continue;
		}


		if (key == 0x0d && current_item != -1)
		{
			doit = true;
			break;
		}


	}


	PaintRadioButtonBase(bRp[current_item], true, true, msgs[current_item], DARKGRAY, RED, WHITE);
	if (current_item == items - 1)
		ExtraHilite(bRp[current_item], true);
	WaitForNothing();
	PaintRadioButtonBase(bRp[current_item], false, false, msgs[current_item], DARKGRAY, RED, WHITE);
	if (current_item == items - 1)
		ExtraHilite(bRp[current_item], false);


	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	LimitMouseRect(&sR);
	ShowCursor();

	if (doit)
	{
		char chosen[128];

		switch (current_item)
		{
		case 0:
			if (select_file("Save Fractal Parameters", "*.FRP", chosen, "FRP") && Overwrite(chosen))
				save_params(chosen);
			break;
		case 1:
			if (select_file("Load Fractal Parameters", "*.FRP", chosen, "FRP"))
				load_params(chosen);
			break;
		case 2:
			if (select_file("Save image as GIF file", "*.GIF", chosen, "GIF") && Overwrite(chosen))
				GifOutput(chosen, 0);
			break;
		case 3:
			if (select_file("Save screen as GIF file", "*.GIF", chosen, "GIF") && Overwrite(chosen))
				GifOutput(chosen, 1);
			break;
		case 4:
			gif_viewer();
			break;
		}
	}

	if (curx > minx)
		BoxCursor();
}
