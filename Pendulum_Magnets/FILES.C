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
#include "gif.h"

#include "mag.h"
extern C_struct C;

typedef struct
{
	char *text;
	rect R;
} fmt;

#define tCenterpull  "Centerpull: "
#define tChargeunit  "Charge:     "
#define tFrequency   "Frequency:  "
#define tRadius      "Radius:     "
#define tMagRad      "Magnet Rad: "
#define tFriction    "Friction:   "
#define tReverse     "Reverse:    "
#define tCaprad      "Capture Rad:"
#define tBasin       "Basins:     "
#define tForceType   "Force Type: "
#define tTraceType   "Trace Type: "
#define tCalcType    "Calc Type:  "
#define tMagnets     "MAGNETS:    "
#define tAutoPat     "Pattern:    "
#define tMaxpts      "Limits:     "
#define tDiameter    "Radius:     "
#define tTraceLength "Trace Len:  "
void FSaveLayout(void)
{
	/* Offer a Layout Save with the selection menu. */
	char filename[128];
	int n;
	FILE *fd;
	int i;
   int ok = true;

	configstructure *Cs = &C.config[mymag];

	time_t timex = time(NULL);



	n = select_file("Save Layout", "*.MFG", filename, "MFG");
	if (n && Overwrite(filename))
	{
		fd = fopen(filename, "wt");
		if (fd)
		{


			/*
			 * A layout save saves EVERYTHING in the "saved
			 * layout" arena.
			 */
			/* First, put a header of some sort. */
			fprintf(fd, "CHAOS Magnets Layout\n");
			fprintf(fd, "Created: %s", ctime(&timex));
			/* Now the configuration */

			ok &= EOF != fprintf(fd, "%s%ld\n", tCenterpull, centerpull);
			ok &= EOF != fprintf(fd, "%s%ld\n", tFrequency, freq);
			ok &= EOF != fprintf(fd, "%s%g\n", tRadius, radius);
			ok &= EOF != fprintf(fd, "%s%ld\n", tMagRad, magnetradius);
			ok &= EOF != fprintf(fd, "%s%g\n", tFriction, friction);
			ok &= EOF != fprintf(fd, "%s%d\n", tReverse, reversibleflag);
			ok &= EOF != fprintf(fd, "%s%d\n", tCaprad, xsection);
			ok &= EOF != fprintf(fd, "%s%d\n", tBasin, basinflag);
			ok &= EOF != fprintf(fd, "%s%d\n", tForceType, forcetype);
			ok &= EOF != fprintf(fd, "%s%d\n", tTraceType, tracetype);
			ok &= EOF != fprintf(fd, "%s%d\n", tCalcType, rkflag);
			ok &= EOF != fprintf(fd, "%s%d\n", tAutoPat, Cs->M.autopattern);
			ok &= EOF != fprintf(fd, "%s%d\n", tTraceLength, memorylength);
			ok &= EOF != fprintf(fd, "%s%d\n", tTraceLength, memorylength);

			ok &= EOF != fprintf(fd, "%s%d %d\n", tMaxpts, Cs->M.maxx, Cs->M.maxy);
			/* Now the magnets themselves. */
			for (i = 0; ok && i < M.n; i++)
				ok &= EOF != fprintf(fd, "Magnet %d: %d %d %ld\n",
					i + 1,
					M.magnet[i].x,
					M.magnet[i].y,
					M.magnet[i].charge);

         ok &= EOF != fflush(fd);

			if (ferror(fd) || !ok)
			{
				FileError(filename, fd);
				fclose(fd);
				remove(filename);
			}
			else
				fclose(fd);
		}
	}
}
void FLoadLayout(void)
{
	/* Offer a Layout Save with the selection menu. */
	char tbuf[128];
	int n;
	FILE *fd;
	int maxread = 0;

	n = select_file("Load Layout", "*.MFG", tbuf, "MFG");
	if (n)
	{

		if (access(tbuf, 0))
		{
			FileError(tbuf, NULL);
			return;
		}
		HideCursor();
		fd = fopen(tbuf, "rt");
		while (1)
		{
			long tl;
			double td;
			int ti, ti1, ti2;

			fgets(tbuf, 128, fd);
			if (feof(fd))
				break;

			if (tbuf[0] == '#')
				continue;

			if (sscanf(tbuf, tCenterpull "%ld", &tl))
				centerpull = tl;
			else if (sscanf(tbuf, tRadius "%lg", &td))
            radius = td;
			else if (sscanf(tbuf, tFrequency "%ld", &tl))
				freq = tl;
			else if (sscanf(tbuf, tMagRad "%ld", &tl))
				magnetradius = tl;
			else if (sscanf(tbuf, tFriction "%lg", &td))
				friction = td;
			else if (sscanf(tbuf, tTraceLength "%d", &ti))
				memorylength = ti;
			else if (sscanf(tbuf, tReverse "%d", &ti))
				reversibleflag = ti;
			else if (sscanf(tbuf, tCaprad "%d", &ti))
				xsection = ti;
			else if (sscanf(tbuf, tBasin "%d", &ti))
				basinflag = ti;
			else if (sscanf(tbuf, tForceType "%d", &ti))
				forcetype = ti;
			else if (sscanf(tbuf, tTraceType "%d", &ti))
				tracetype = ti;
			else if (sscanf(tbuf, tCalcType "%d", &ti))
				rkflag = ti;
			else if (sscanf(tbuf, "Magnet %d: %d %d %ld", &ti, &ti1, &ti2, &tl) == 4)
			{
				int ii = ti - 1;

				M.magnet[ii].x = ti1;
				M.magnet[ii].y = ti2;
				M.magnet[ii].charge = tl;
				maxread = max(maxread, ti);
			}
		}
		SelectNothing();
		holenumber = M.n = maxread;
		/* OKAY, now update all the data. */
		saveballflag = 0;
		showclub(clubx, cluby, 0);
		CursorShowing = false;
		startconfig();

		fclose(fd);
		DoAllButtons();
		MoveCursor(clubx + minx, cluby);
		ResetBasins();
		ShowCursor();
	}
}

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

		if (access(chosen_file, 0))
		{
			FileError(chosen_file, NULL);
			continue;
		}
		HideCursor();
		BackColor(BLACK);
		EraseRect(&sR);
		if (GifDisplay(chosen_file) == GIF_COOLMAN)
		{
			int doit = false;
			int GIFspinflag = 0;
			int iterationspeed = 100;
			int iteration = 0;

			sound(110);
			delay(10);
			nosound();
			while (!doit)
			{
				extern void spinGIFpalette(int);
				extern void revspinGIFpalette(int);

				int n = KeyEvent(false, &e);

				if ((e.State >> 8) & 7)
					break;
				if (n)
				{
					int key = 0;

					if (e.ASCII && e.ASCII != 0xe0)
						key = e.ASCII;
					else if (e.ScanCode != 0xff)
						key = e.ScanCode << 8;

					switch (key)
					{
					case 0:
						break;
					case XALTJ:
						spinGIFpalette(OurMode);
						break;
					case XALTK:
						revspinGIFpalette(OurMode);
						break;
					case XALTY:
						GIFspinflag++;
						if (GIFspinflag == 3)
							GIFspinflag = 0;
						break;
					case ',':
						if (iterationspeed > 2)
							iterationspeed /= 2;
						break;
					case '.':
						if (iterationspeed < 16 * 1024)
							iterationspeed *= 2;
						break;
					default:
						doit = true;
					}
				}

				if (iteration++ > iterationspeed)
				{
					if (GIFspinflag == 1)
						spinGIFpalette(OurMode);
					else if (GIFspinflag == 2)
						revspinGIFpalette(OurMode);
					iteration = 0;
				}


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
		usepalette();
		firsttimeflag = 1;
		setuphole1(holenumber);
		setupcontrols();
		setuphole2();
		MoveCursor(ballx + minx, bally);
		CursorShowing = false;
		ResetBasins();
		ShowCursor();
	}
}
static rect *bRp[10];
static int items;

static void push(int item, int inout)
{
	if (item != -1)
	{
		PushButton(bRp[item], inout);
		if (item == items - 1)
			ExtraHilite(bRp[item], inout);
	}
}

void FGifOutput(int n)
{
	char chosen[128];
	char tbuf[128];

	sprintf(tbuf, "Save %s as GIF file", n ? "screen" : "image");
	if (select_file(tbuf, "*.GIF", chosen, "GIF") && Overwrite(chosen))
		GifOutput(chosen, n);
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
	static int filekeys[] = {XALTS, XALTL, XALTG, XALTH, XALTF};

	static char *msgs[] = {
		"Save Parameters    (Alt-S)",
		"Load Parameters    (Alt-L)",
		"Save image as GIF  (Alt-G)",
		"Save screen as GIF (Alt-H)",
		"View GIF image     (Alt-F)",
		"F1 for HELP",
		"ESC to Cancel",
		NULL
	};


	items = 7;
	for (i = 0; i < 7; i++)
		width = max(width, StringWidth(msgs[i]) + 24);

	HideCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "File Menu", BLACK);
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
			push(last_item, false);
			push(current_item, true);
		}

		if (key == 0x1b)
			break;

		if (key == XF1 || (key == 0x0d && current_item == items - 2))
		{
			helptext("magfile.hlp");
			continue;
		}


		if (key == 0x0d && current_item != -1)
		{
			doit = true;
			break;
		}


	}


	PaintRadioButtonBase(bRp[current_item], true, true, msgs[current_item], DARKGRAY, RED, WHITE);
	WaitForNothing();
	PaintRadioButtonBase(bRp[current_item], false, false, msgs[current_item], DARKGRAY, RED, WHITE);


	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	LimitMouseRect(&sR);
	ShowCursor();

	if (doit)
	{
		switch (current_item)
		{
		case 0:
			FSaveLayout();
			break;
		case 1:
			FLoadLayout();
			break;
		case 2:
			FGifOutput(false);
			break;
		case 3:
			FGifOutput(true);
			break;
		case 4:
			gif_viewer();
			break;
		}
	}

}
