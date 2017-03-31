#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include <ctype.h>

#ifndef INCLUDE_FILE
#define INCLUDE_FILE "toy.h"
#endif
#include INCLUDE_FILE
#ifndef __COLORS
#if defined(FORGE)
#define __COLORS
enum COLORS
{
	BLACK,
	LIGHTRED,
	YELLOW,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTBLUE,
	LIGHTMAGENTA,
	WHITE,
	DARKGRAY,
	RED,
	BROWN,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	LIGHTGRAY,
};

#endif
#endif


#undef getcwd

/* File selection tool, stolen in look and feel from 3d Studio */

#define SFILEMAX 10
#define FILEMAX 128

static char working_directory[MAXPATH]={0};
static char current_directory[MAXPATH]={0};
static rect tR;
static rect sliderRect;
static rect bubbleRect;
static rect uaR;
static rect daR;
static rect fileFrame, filenameRects[SFILEMAX];
static rect driveRects[32];
static rect filenameRect;
static text_button filenameTB;
static rect dirRect;
static text_button dirTB;
static rect wildcardRect;
static text_button wildcardTB;
static rect *nameRects[3] = {&filenameRect, &dirRect, &wildcardRect};
static text_button *nameTBS[3] = {&filenameTB, &dirTB, &wildcardTB};
static int slider_length;
static int filecount;
static char filelist[FILEMAX][14];
static int start_positions[FILEMAX];
static char active_file[14] = "";
static char filespec[128];
static int current_item;
static int working_disk = -1;
static int floppycount;

static int topline = 0;
static char *doitmsgs[] = {
	"F1 for HELP",
	"ESC to cancel",
	"OK"
};

static rect doitFrame;
static rect doitRects[3];

static void invert_item(int item)
{
	rect R = *nameRects[item];
	int n;

	HideCursor();
	InsetRect(&R, 1, 1);

	switch (item)
	{
	case 0:
		n = StringWidth(active_file);
		break;
	case 1:
		n = StringWidth(working_directory);
		break;
	case 2:
		n = StringWidth(filespec);
		break;
	}

	R.Xmax = R.Xmin + n;

	InvertRect(&R);
	ShowCursor();
}


void show_chosen(void)
{
	rect R = filenameRect;

	HideCursor();
	InsetRect(&R, 1, 1);
	PenColor(DARKGRAY);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin, R.Ymin);
	BackColor(DARKGRAY);
	PenColor(WHITE);
	DrawString(active_file);

	R.Xmax = R.Xmin + StringWidth(active_file);

	if (current_item == 0)
		InvertRect(&R);
	ShowCursor();
}

int jstrcmp(const void *z1, const void *z2)
{
	char *s1 = (char *) z1;
	char *s2 = (char *) z2;


	if (*s1 == '\\')	/* a directory */
	{
		if (*s2 == '\\')/* another dir? */
		{
			return strcmp(s1, s2);	/* Just two plain dirs */
		}
		return 1;	/* Oh, he's normal, I come last */
	}
	/* First guy is a regular filename */
	if (*s2 == '\\')	/* Is he weird? */
		return -1;
	return strcmp(s1, s2);
}

static void drawone(int i)
{
	char tbuf[128];

   HideCursor();
	TextAlign(alignLeft, alignTop);
	PenColor(WHITE);
	BackColor(BLUE);
	MoveTo(filenameRects[i].Xmin + 2, filenameRects[i].Ymin);
	sprintf(tbuf, "%-14s", filelist[topline + i]);
	DrawString(tbuf);
   ShowCursor();
}

void repaint_all_entries(void)
{
	rect R;
	int i;

	HideCursor();

	R = fileFrame;
	InsetRect(&R, 1, 1);

	PenColor(BLUE);
	PaintRect(&R);


	for (i = 0; i < SFILEMAX && i < filecount; i++)
		drawone(i);


	ShowCursor();
}


void update_scroll_bar(void)
{
	rect R;

	HideCursor();
	R.Xmin = sliderRect.Xmin + 2;
	R.Xmax = sliderRect.Xmax - 2;
	R.Ymin = sliderRect.Ymin + 2 + start_positions[topline];
	R.Ymax = R.Ymin + slider_length - 1;

	bubbleRect = R;

	R = sliderRect;
	InsetRect(&R, 1, 1);
	PenColor(BLUE);
	PaintRect(&R);

	PenColor(LIGHTGRAY);
	PaintRect(&bubbleRect);
	PushButton(&bubbleRect, false);
	ShowCursor();
}

void slide_scroll_bar(void)
{
	rect R, R1;

	HideCursor();

	R.Xmin = sliderRect.Xmin + 2;
	R.Xmax = sliderRect.Xmax - 2;
	R.Ymin = sliderRect.Ymin + 2 + start_positions[topline];
	R.Ymax = R.Ymin + slider_length - 1;

	R1 = sliderRect;
	InsetRect(&R1, 2, 2);
	ClipRect(&R1);
	BackColor(BLUE);

	ScrollRect(&R1, 0, R.Ymin - bubbleRect.Ymin);
	bubbleRect = R;
	ClipRect(&sR);
	ShowCursor();
}


void initialize_directories(char *filespec)
{
	int i;
	int n;
	struct ffblk ffblk;
	char tbuf[32];
	int l;
	char *msg = "Warning: too many files. Stopping at %d";

	filecount = 0;
	if (working_directory[0])
	{
		BackColor(BLUE);
		/* Make a linked list of all the files we are concerned with. */
		n = findfirst(filespec, &ffblk, 0);
		if (!n)
		{
			if (filecount > FILEMAX - 1)
			{
				sprintf(tbuf, msg, FILEMAX);
				ErrorBox(tbuf);
				goto hell;
			}


			strcpy(filelist[filecount++], ffblk.ff_name);
			while (!n)
			{
				n = findnext(&ffblk);
				if (!n)
				{
					if (filecount > FILEMAX - 1)
					{
						sprintf(tbuf, msg, FILEMAX);
						ErrorBox(tbuf);
						goto hell;
					}
					strcpy(filelist[filecount++], ffblk.ff_name);
				}
			}
		}


		/* Now, we also add an entry for each available directory... */
		n = findfirst("*.*", &ffblk, FA_DIREC);
		if (!n)
		{
			while (!n && (!(ffblk.ff_attrib & FA_DIREC) || !strcmp(".", ffblk.ff_name) || !strcmp("..", ffblk.ff_name)))
				n = findnext(&ffblk);
			if (!n)
			{
				sprintf(tbuf, "\\%s", ffblk.ff_name);
				if (filecount > FILEMAX - 1)
				{
					sprintf(tbuf, msg, FILEMAX);
					ErrorBox(tbuf);
					goto hell;
				}
				strcpy(filelist[filecount++], tbuf);
				while (!n)
				{
					n = findnext(&ffblk);
					if (!n && (ffblk.ff_attrib & FA_DIREC) && strcmp(".", ffblk.ff_name) && strcmp("..", ffblk.ff_name))
					{
						if (filecount > FILEMAX - 1)
						{
							sprintf(tbuf, msg, FILEMAX);
							ErrorBox(tbuf);
							goto hell;
						}
						sprintf(tbuf, "\\%s", ffblk.ff_name);
						strcpy(filelist[filecount++], tbuf);
					}
				}
			}
		}
	}

hell:
	qsort(filelist, filecount, sizeof(filelist[0]), jstrcmp);

	/* Figure out the size of the slider itself */
	if (filecount < SFILEMAX)
		slider_length = (sliderRect.Ymax - sliderRect.Ymin - 2);
	else
		slider_length = (int) ((float) (sliderRect.Ymax - sliderRect.Ymin - 2) *
				((float) SFILEMAX) / (float) filecount - 1);


	l = sliderRect.Ymax - sliderRect.Ymin - 4;


	if (filecount > SFILEMAX)
	{
		for (i = 0; i < filecount - SFILEMAX + 1; i++)
		{
			start_positions[i] = i * (l - slider_length) / (float) (filecount - SFILEMAX);
		}
	}
	else
		start_positions[0] = 0;

	repaint_all_entries();
	update_scroll_bar();

	show_chosen();

}


void PaintDirname(void)
{
	rect R = dirRect;

	HideCursor();
	InsetRect(&R, 1, 1);
	PenColor(DARKGRAY);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin, R.Ymin);
	BackColor(DARKGRAY);
	PenColor(WHITE);
	DrawString(working_directory);
	ShowCursor();
}

void PaintWildname(void)
{
	rect R = wildcardRect;

	HideCursor();
	InsetRect(&R, 1, 1);
	PenColor(DARKGRAY);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin, R.Ymin);
	BackColor(DARKGRAY);
	PenColor(WHITE);
	DrawString(filespec);
	ShowCursor();
}


void change_directory(char *p)
{
	char *dd;
	char tbuf[128];

	HideCursor();
	strupr(p);
	/* Check to see if there is a ":" in the new string. */
	dd = strchr(p, ':');
	if (dd)
	{
		/*
		 * Yup, there's a drive here. Assume it is the first thing in
		 * the string (if not, we are deeply fucked.
		 */
		int new_disk = *p - 'A';
		int ok = true;

		if (floppycount == 1 && new_disk == 1)
			new_disk = 0;
		setdisk(new_disk);
		if (getdisk() == new_disk)
		{

			char *t = getcwd(tbuf, 128);

			if (!t)
				ok = false;
		}
		else
			ok = false;

		if (!ok)
		{
			sprintf(tbuf, "Cannot change to drive %c", *p);
			ErrorBox(tbuf);
			PaintDirname();
			ShowCursor();
			return;
		}

		working_disk = new_disk;
		p = dd + 1;
	}



	if (!chdir(p))
	{
      getcwd(working_directory,MAXPATH);
		topline = 0;
		initialize_directories(filespec);
		active_file[0] = 0;
		show_chosen();
	}
	else
	{
		sprintf(tbuf, "Can't change to directory %s", p);
		ErrorBox(tbuf);
	}

	PaintDirname();
	ShowCursor();
}


static void scrollme(int n)
{
	rect R = fileFrame;

	HideCursor();

	InsetRect(&R, 1, 1);
	R.Ymax -= 1;
	BackColor(BLUE);
	ClipRect(&R);
	ScrollRect(&R, 0, n);
	ClipRect(&sR);
	ShowCursor();
}

void maybe_scrolldown(void)
{

	if (topline + SFILEMAX < filecount)
	{
		scrollme(-FontHeight);
		topline++;
		drawone(SFILEMAX - 1);
		update_scroll_bar();
	}
}
void maybe_scrollup(void)
{
	if (topline)
	{
		scrollme(FontHeight);
		topline--;
		drawone(0);
		update_scroll_bar();
	}
}

void process_arrow_button(rect * aR, void (*p) (void), event * e)
{
	int first = true;
	int lasttime = e->Time;
	int button = swLeft;

	PushMouseRectLimit(aR);
	HideCursor();
	InvertRect(aR);
	(*p) ();
	ShowCursor();
	while (button == swLeft)
	{
		int banzai = false;

		KeyEvent(false, e);
		button = (e->State & 0x700) >> 8;
		if (first)
		{
			if (e->Time - lasttime > 4)
			{
				first = false;
				banzai = true;
			}
		}
		else
			banzai = (e->Time != lasttime);

		if (banzai)
			(*p) ();
	}
	HideCursor();
	InvertRect(aR);
	PopMouseRect();
	ShowCursor();
}


int select_file(char *title, char *_filespec, char *chosen, char *ext)
{
	int height, width;
	int cx, cy;
	int cx1, cy1;
	int key;
	int current_disk;
	rect R;
	int i;
	int drivecount = setdisk(getdisk());
	int buttonwidth;
	char tbuf[128];
	int t1, t2;
	event e, laste;
	int retval = 0;
	char drive[MAXDRIVE];
	char file[MAXFILE];
	char eext[MAXEXT];
	int chosen_item = -1;
	char helpfilename[MAXPATH];
	union REGS regs;
   char *sep;
	if (strcmp(filespec, _filespec))
	{
		active_file[0] = 0;
		topline = 0;
	}

	strcpy(filespec, _filespec);
	int86(0x11, &regs, &regs);
	floppycount = ((regs.x.ax & 0xc0) >> 6) + 1;

	HideCursor();
	PushCursorPosition();
	PushCursorType();
	laste.Time = 0;
	current_item = 0;


	height = FontHeight + 4	/* title */
		+ SFILEMAX * FontHeight	/* filenames */
		+ 16;		/* padding */

	width = 3 * (sR.Xmax + 1) / 4 + 20;


	Centers(&sR, &cx, &cy);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;

	tR = R;

	if (!ShadowAndSave(&tR))
	{
		ShowCursor();
      PopCursorPosition();
		return 0;
	}

	getcwd(current_directory, 128);

   if (current_directory[strlen(current_directory)-1] != '\\')
      sep = "\\";
   else
      sep = "";

	sprintf(helpfilename, "%s%sselect.hlp", current_directory,sep);
	current_disk = getdisk();

	if (working_disk != -1)
		setdisk(working_disk);
	else
		working_disk = current_disk;


	/* save the current directory */


	if (!working_directory[0])
		strcpy(working_directory,current_directory);
	else
		chdir(working_directory);

	/* Build the sorted list of filenames */



	RasterOp(zREPz);
	PushMouseRectLimit(&tR);

	PenColor(LIGHTGRAY);
	PaintRect(&tR);

	PenColor(0);
	FrameRect(&tR);

	TextAlign(alignCenter, alignTop);
	MoveTo(cx, tR.Ymin + 2);
	PenColor(0);
	BackColor(LIGHTGRAY);
	DrawString(title);

	R.Xmin = tR.Xmin + 30;
	R.Xmax = R.Xmin + 15 * StringWidthX;
	R.Ymin = tR.Ymin + FontHeight + 2 + 4;
	R.Ymax = R.Ymin + 4 + SFILEMAX * FontHeight;

	fileFrame = R;

	PenColor(BLUE);
	PaintRect(&R);
	PushButton(&R, true);

	/* create the subrects */
	for (i = 0; i < SFILEMAX; i++)
	{
		R.Xmin = fileFrame.Xmin + 1;
		R.Xmax = fileFrame.Xmax - 1;
		R.Ymin = fileFrame.Ymin + 1 + i * FontHeight;
		R.Ymax = R.Ymin + FontHeight - 1;
		filenameRects[i] = R;
	}

	R.Xmin = tR.Xmin + 4;
	R.Xmax = R.Xmin + 20;
	R.Ymin = fileFrame.Ymin;
	R.Ymax = R.Ymin + (R.Xmax - R.Xmin) / aspect;

	uaR = R;
	PenColor(DARKGRAY);
	PaintRect(&R);
	PushButton(&R, false);
	Centers(&R, &cx1, &cy1);
	TextAlign(alignCenter, alignMiddle);
	MoveTo(cx1 - 2, cy1);
	PenColor(WHITE);
	BackColor(DARKGRAY);
	DrawChar('\x80');

	R.Ymax = fileFrame.Ymax;
	R.Ymin = R.Ymax - (R.Xmax - R.Xmin) / aspect;
	daR = R;
	PenColor(DARKGRAY);
	PaintRect(&R);
	PushButton(&R, false);
	Centers(&R, &cx1, &cy1);
	MoveTo(cx1 - 2, cy1);
	PenColor(WHITE);
	BackColor(DARKGRAY);
	DrawChar('\x81');

	R.Ymin = uaR.Ymax + 2;
	R.Ymax = daR.Ymin - 2;
	sliderRect = R;
	PenColor(BLUE);
	PaintRect(&R);
	PushButton(&R, true);

	buttonwidth = StringWidthX * 2 + 4;
	/* Now the little buttons for the disk drives */

	for (i = 0; i < drivecount + 2; i++)
	{
		R.Xmin = fileFrame.Xmax + 4 + (i % 5) * (buttonwidth + 4);
		R.Xmax = R.Xmin + buttonwidth - 1;
		R.Ymin = fileFrame.Ymin + +(i / 5) * (buttonwidth / aspect + 4);
		R.Ymax = R.Ymin + buttonwidth / aspect;

		switch (i)
		{
		case 0:
			sprintf(tbuf, "\\");
			break;
		case 1:
			sprintf(tbuf, "..");
			break;
		default:
			sprintf(tbuf, "%c", (i - 2) + 'A');
			break;
		}
		driveRects[i] = R;
		PaintRadioButtonBase(&R, false, false, tbuf, DARKGRAY, DARKGRAY, WHITE);
	}


	/* Now the filename etc. box */
	R.Xmin = driveRects[4].Xmax + 8;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = driveRects[0].Ymin;
	R.Ymax = R.Ymin + FontHeight + 4;

	PaintTextBoxBase(&R, &filenameTB, "Filename: ", "", 0, LIGHTGRAY, DARKGRAY, WHITE);
	filenameRect = filenameTB.nR;

	R.Ymin = R.Ymax + 4;
	R.Ymax = R.Ymin + FontHeight + 4;
	PaintTextBoxBase(&R, &dirTB, "Dir: ", working_directory, 0, LIGHTGRAY, DARKGRAY, WHITE);
	dirRect = dirTB.nR;

	R.Ymin = R.Ymax + 4;
	R.Ymax = R.Ymin + FontHeight + 4;
	PaintTextBoxBase(&R, &wildcardTB, "Wildcard: ", filespec, 0, LIGHTGRAY, DARKGRAY, WHITE);
	wildcardRect = wildcardTB.nR;
	/* Now create the magick buttons */

	R.Ymax = fileFrame.Ymax;
	R.Ymin = R.Ymax - FontHeight - 8;
	R.Xmin = driveRects[0].Xmin;
	doitFrame = R;

	t1 = (doitFrame.Xmax - doitFrame.Xmin + 2) / 3;
	t2 = t1 - 8;

	for (i = 0; i < 3; i++)
	{
		R.Xmin = doitFrame.Xmin + 4 + t1 * i;
		R.Xmax = R.Xmin + t2;
		R.Ymax = doitFrame.Ymax - 2;
		R.Ymin = R.Ymax - FontHeight - 4;
		doitRects[i] = R;
		PaintRadioButtonBase(&R, false, false, doitmsgs[i], 8, 8, WHITE);
	}
	ExtraHilite(&doitRects[2], false);
	move_to_corner(&doitRects[2]);
	initialize_directories(filespec);



	ShowCursor();

	while (1)
	{
		int n = KeyEvent(false, &e);
		int X = e.CursorX;
		int Y = e.CursorY;
		int button = (e.State & 0x700) >> 8;

		key = 0;

		if (e.ASCII && e.ASCII != 0xe0)
			key = e.ASCII;
		else
			key = e.ScanCode << 8;


		if (n)
		{
			if (button == swRight ||
			(button == swLeft && XYInRect(X, Y, &doitRects[1])))
				key = 0x1b;


			if (button == swLeft)
			{
				if (XYInRect(X, Y, &doitRects[2]))
				{
					retval = 1;
					break;
				}
				if (XYInRect(X, Y, &doitRects[0]))
					key = XF1;


				/* Is it one of the file boxes? */
				for (i = 0; i < SFILEMAX && i < filecount; i++)
				{
					if (XYInRect(X, Y, &filenameRects[i]))
					{
						char *p = filelist[topline + i];

						if (*p != '\\')
						{
							/*
							 * this one might be
							 * a second of a
							 * double click.
							 */
							if (e.Time < laste.Time + 10 && i == chosen_item)
							{
								key = 0x0d;
								retval = 1;
								break;
							}
							else
							{
								strcpy(active_file, p);
								current_item = 0;
								show_chosen();
								chosen_item = i;
								laste = e;
								continue;
							}
						}
						else
						{
							change_directory(p + 1);
							continue;
						}
					}
				}
				if (XYInRect(X, Y, &driveRects[0]))	/* top level */
					change_directory("\\");
				else if (XYInRect(X, Y, &driveRects[1]))
					change_directory("..");

				/* is it a change-drive thing? */
				for (i = 0; i < drivecount; i++)
				{
					if (XYInRect(X, Y, &driveRects[i + 2]))
					{
						int new_disk = i;
						char *t;

						if (floppycount == 1 && i == 1)
							new_disk = 0;

						setdisk(new_disk);
						if (getdisk() == new_disk)
						{
							HideCursor();
							t = getcwd(NULL, 128);
							ShowCursor();
							if (t)
							{
								strcpy(working_directory,t);
								working_disk = new_disk;
								change_directory(".");
							}
						}

						continue;
					}
				}

				if (XYInRect(X, Y, &daR))
					process_arrow_button(&daR, maybe_scrolldown, &e);
				else if (XYInRect(X, Y, &uaR))
					process_arrow_button(&uaR, maybe_scrollup, &e);
				else if (XYInRect(X, Y, &bubbleRect))
				{
					R = sliderRect;
					InsetRect(&R, 2, 2);
					LimitMouseRect(&R);
					while (button == swLeft)
					{
						int n;

						n = (Y - sliderRect.Ymin) / (float) (sliderRect.Ymax - sliderRect.Ymin) * filecount;
						n = min(n, filecount - SFILEMAX);
						n = max(n, 0);


						if (n < topline)
						{
							topline = n;
							slide_scroll_bar();
						}
						if (n > topline)
						{
							topline = n;
							slide_scroll_bar();
						}
						KeyEvent(false, &e);
						X = e.CursorX;
						Y = e.CursorY;
						button = (e.State & 0x700) >> 8;
					}
					repaint_all_entries();
					LimitMouseRect(&tR);
					continue;
				}


				else if (XYInRect(X, Y, &sliderRect))
				{
					if (filecount > SFILEMAX)
					{
						topline = (Y - sliderRect.Ymin) / (float) (sliderRect.Ymax - sliderRect.Ymin) * filecount;
						topline = min(topline, filecount - SFILEMAX);
						repaint_all_entries();
						update_scroll_bar();
						continue;
					}
				}

				for (i = 0; i < 3; i++)
				{
					if (XYInRect(X, Y, nameRects[i]))
					{
						if (current_item != i)
						{
							invert_item(current_item);
							invert_item(current_item = i);
							continue;
						}
					}
				}






			}

		}
gotkey:
		if (key == 0x0d)
		{
			retval = 1;
			break;
		}

		if (key == 0x1b)
			break;

		if (isprint(key))
		{
			/* The sucker wants to start typing something. */
			char sbuf[128];
			int n;
			text_button *TB;

			sbuf[0] = key;
			sbuf[1] = 0;
			TB = nameTBS[current_item];
			n = GetText(TB, tbuf, sbuf, DARKGRAY, WHITE);
			switch (current_item)
			{
			case 0:
				if (n)
					strcpy(active_file, tbuf);
				show_chosen();
				break;
			case 1:
				/* A new directory */
				/* Maybe also a new disk drive */
				if (n)
					change_directory(tbuf);
				else
					PaintDirname();
				invert_item(current_item);
				break;
			case 2:
				/* A new wildcard */
				if (n)
				{
					strcpy(filespec, tbuf);
					initialize_directories(tbuf);
				}
				PaintWildname();
				invert_item(current_item);
				break;
			}

		}


		switch (key)
		{
		case XF1:
			helptext(helpfilename);
			LimitMouseRect(&tR);
			break;

		case '\t':
			invert_item(current_item);
			current_item++;
			if (current_item > 2)
				current_item = 0;
			invert_item(current_item);
			break;

		case XHOME:
			topline = 0;
			repaint_all_entries();
			slide_scroll_bar();
			break;

		case XEND:
			topline = max(filecount - SFILEMAX, 0);
			repaint_all_entries();
			slide_scroll_bar();
			break;



		case XDARROW:
			maybe_scrolldown();
			break;
		case XUARROW:
			maybe_scrollup();
			break;

		case XPGUP:
			if (topline)
			{
				topline -= SFILEMAX;
				if (topline < 0)
					topline = 0;
				repaint_all_entries();
				update_scroll_bar();
			}
			break;
		case XPGDN:
			topline += SFILEMAX;
			topline = min(topline, filecount - SFILEMAX);
			if (topline < 0)
				topline = 0;
			repaint_all_entries();
			update_scroll_bar();
			break;

		}


	}



	HideCursor();
	PopRect(&i);
	PopMouseRect();
	PopCursorPosition();
	PopCursorType();
	ShowCursor();

	if (active_file[0])
	{
		/* Build the file name into "chosen" */
		fnsplit(active_file, NULL, NULL, file, eext);
		if (eext[0] == 0)
			sprintf(eext, ".%s", ext);

		drive[0] = working_disk + 'A';
		drive[1] = ':';
		drive[2] = 0;
		fnmerge(chosen, drive, working_directory + 2, file, eext);
	}
	else
		retval = 0;

	setdisk(current_disk);
	chdir(current_directory);
	return retval;
}
