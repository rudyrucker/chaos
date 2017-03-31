#include <string.h>
#include <alloc.h>

#ifndef INCLUDE_FILE
#define INCLUDE_FILE "toy.h"
#endif

#include INCLUDE_FILE

#define BOXLEN 15

extern metaPort *thePort;

void push_alignment(void);
void pop_alignment(void);
static rect sliderRect, uaR, daR;
static rect bubbleRect;

static int topline;
static int slider_length;
static int trackheight;
static int linecount;
extern int display_page;
extern rect sR;
extern int FontHeight;
extern int StringWidthX;

long int *linepointers;
int linepointer_size;

static int start_positions[2048];


static void slide_scroll_bar(void)
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
	BackColor(LIGHTGREEN);

	ScrollRect(&R1, 0, R.Ymin - bubbleRect.Ymin);
	bubbleRect = R;
	ClipRect(&sR);
	ShowCursor();
}




static void update_scroll_bar(void)
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
	PenColor(LIGHTGREEN);
	PaintRect(&R);
	PenColor(LIGHTGRAY);
	PaintRect(&bubbleRect);
	PushButton(&bubbleRect, false);
	ShowCursor();

}

#define usb update_scroll_bar

static rect insetR;
static point starts[15];
void draw_one(int linenumber, FILE * fd)
{
	char tbuf[128];

	fseek(fd, linepointers[linenumber], 0);
	fgets(tbuf, 128, fd);
	tbuf[strlen(tbuf) - 1] = 0;
	TextAlign(alignLeft, alignTop);
	PenColor(WHITE);
	BackColor(BLUE);
	DrawString(tbuf);
}
static int boxlen;

void repaint_em_all(FILE * fd)
{
	int i;

	RasterOp(zREPz);
	HideCursor();
	PenColor(BLUE);
	PaintRect(&insetR);
	for (i = 0; i < boxlen && topline + i < linecount; i++)
	{
		MoveTo(starts[i].X, starts[i].Y);
		draw_one(topline + i, fd);
	}
	ShowCursor();
}

char *helpmsg = "ESC to Cancel";
static rect exitR;
static text_button searchR;

static char lastfilename[128];

int compare_line(FILE * fd, int i, char *matchbuf)
{
	char sbuf[128];

	fseek(fd, linepointers[i], 0);
	fgets(sbuf, 128, fd);
	strupr(sbuf);
	return (strstr(sbuf, matchbuf) != NULL);
}

static void scrollto(int oldpos, int newpos, FILE * fd)
{
	/*
	 * Figure out how many lines we want to scroll to so that the topline
	 * is at the top of the screen.
	 */
	int i;
	rect R = insetR;

	R.Ymin = starts[0].Y;
	R.Ymax = starts[boxlen - 1].Y + FontHeight - 1;

	topline = newpos;
	if (newpos == oldpos)
		return;
	HideCursor();
	ClipRect(&R);
	if (abs(newpos - oldpos) > boxlen / 2)
		repaint_em_all(fd);
	else
	{
		BackColor(BLUE);
		ScrollRect(&R, 0, FontHeight * (oldpos - newpos));
		if (newpos < oldpos)
		{
			for (i = 0; i < oldpos - newpos; i++)
			{
				MoveTo(starts[i].X, starts[i].Y);
				draw_one(topline + i, fd);
			}
		}
		else
		{
			for (i = 0; i < newpos - oldpos; i++)
			{
				int ii = boxlen - (newpos - oldpos) + i;

				MoveTo(starts[ii].X, starts[ii].Y);
				draw_one(topline + ii, fd);
			}
		}
	}
	ClipRect(&sR);
	ShowCursor();
}




void helptext(char *filename)
{
	FILE *fd;

	int m, i;
	char tbuf[128];
	int maxlen = strlen(helpmsg);
	rect helpR;
	int err;
	int key;
	int topX, topY;
	event e;
	int highlighted_line = -1;
	rect R, R1, R2;

	/* Try to read the whole thing in. */
	/* Screw the saves, though! */
	rect clipper;
	int cx, cy;
	int cx1, cy1;
	int height, width;

	char matchbuf[40];
	char lastmatch[40];
	int l;

	lastmatch[0] = 0;
	fd = fopen(filename, "rt");

	if (!fd)
   {
      FileError(filename,NULL);
		return;
	}

	TextAlign(alignLeft, alignTop);

	HideCursor();
	PushCursorType();
	PushCursorPosition();

	clipper = thePort->portClip;

	PenSize(1, 1);
	RasterOp(zREPz);
	ClipRect(&sR);


	linecount = 0;
	linepointer_size = 256;
	safe_alloc = 1;
	linepointers = malloc(linepointer_size * sizeof(long int));
	if (!linepointers)
	{
		ErrorBox("Not enough memory for help.");
		goto cleanup;
	}

	linepointers[0] = 0L;

	while (1)
	{
		fgets(tbuf, 128, fd);
		if (feof(fd))
			break;

		linecount++;

		if (linecount > linepointer_size)
		{
			linepointer_size *= 2;
			linepointers = realloc(linepointers, linepointer_size * sizeof(long int));
			if (!linepointers)
			{
				ErrorBox("Not enough memory for help.");
				goto cleanup;
			}
		}
		linepointers[linecount] = ftell(fd);

		/* Strip the tailing \n */
		tbuf[m = (strlen(tbuf) - 1)] = 0;
		maxlen = max(maxlen, m);
	}

	/* Hokay, we have the whole thing in. */
	/*
	 * We want a box that can fit "boxlen" lines in. Let DrawTextRect do
	 * it.
	 */

	boxlen = min(linecount, BOXLEN);
	maxlen = min(maxlen, (sR.Xmax - 40) / StringWidthX);

	/* OK, let's do it a new way. */
	height = boxlen * FontHeight + FontHeight + 24;
	width = maxlen * StringWidthX + StringWidthX * 5;

	Centers(&sR, &cx, &cy);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;

	if (!ShadowAndSave(&R))
	{
		ShowCursor();
		ClipRect(&sR);
		fclose(fd);
		free(linepointers);
		linepointers = NULL;
		LimitMouseRect(&sR);
		return;
	}

	insetR = helpR = R;
	insetR.Xmin += StringWidthX;
	insetR.Xmax -= FontHeight + 8;
	insetR.Ymin += StringWidthX;
	insetR.Ymax = insetR.Ymin + 2 + FontHeight * boxlen + 2;


	MoveTo(insetR.Xmin + 1, insetR.Ymin + 1);
	topX = QueryX();
	topY = QueryY();

	/* To speed this up, just paint the edges. It will look odd? */
	R1 = helpR;
	R2 = insetR;
	R1.Ymax = R2.Ymin - 1;
	PenColor(LIGHTGRAY);
	PaintRect(&R1);
	R1 = helpR;
	R1.Xmax = R2.Xmin - 1;
	R1.Ymin = R2.Ymin;
	PaintRect(&R1);
	R1 = helpR;
	R1.Xmin = R2.Xmax + 1;
	R1.Ymin = R2.Ymin;
	PaintRect(&R1);
	R1 = helpR;
	R1.Xmin = R2.Xmin;
	R1.Xmax = R2.Xmax;
	R1.Ymin = R2.Ymax + 1;
	PaintRect(&R1);
	PenColor(BLACK);
	FrameRect(&helpR);

	R = insetR;
	InsetRect(&R, -1, -1);
	PushButton(&R, true);

	R.Xmin = insetR.Xmax + 2;
	R.Xmax = helpR.Xmax - 2;
	R.Ymin = insetR.Ymin;
	R.Ymax = R.Ymin + (R.Xmax - R.Xmin) / aspect;
	uaR = R;
	PenColor(DARKGRAY);
	PaintRect(&R);
	PushButton(&R, false);
	Centers(&R, &cx1, &cy1);
	TextAlign(alignCenter, alignMiddle);
	MoveTo(cx1 - 2, cy1);
	BackColor(DARKGRAY);
	PenColor(WHITE);
	DrawChar('\x80');

	R.Ymax = insetR.Ymax;
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
	PenColor(LIGHTGREEN);
	PaintRect(&R);
	PushButton(&R, true);

	PushMouseRectLimit(&helpR);
	move_to_corner(&helpR);

	trackheight = sliderRect.Ymax - sliderRect.Ymin;
	/* Now wait for keys. */

	if (strcmp(filename, lastfilename))
		topline = 0;


	if (linecount < boxlen)
		slider_length = (sliderRect.Ymax - sliderRect.Ymin - 4);
	else
		slider_length = (int) ((float) (sliderRect.Ymax - sliderRect.Ymin) *
				  ((float) boxlen) / (float) linecount - 1);

	l = sliderRect.Ymax - sliderRect.Ymin - 4;

	if (linecount > BOXLEN)
	{
		for (i = 0; i < linecount; i++)
		{
			start_positions[i] =
				i * ((l - slider_length) / (float) (linecount - BOXLEN));
		}
	}
	else
		start_positions[i] = 0;


	MoveCursor(helpR.Xmin, helpR.Ymin);
	PenColor(BLACK);
	BackColor(BLUE);
	RasterOp(zREPz);
	strcpy(lastfilename, filename);
	update_scroll_bar();

	fseek(fd, 0L, 0);
	MoveTo(topX + 1, topY + 1);

	for (i = 0; i < boxlen; i++)
	{
		starts[i].X = insetR.Xmin + 2;
		starts[i].Y = insetR.Ymin + i * FontHeight + 2;
	}

	repaint_em_all(fd);
	usb();

	R.Xmax = insetR.Xmax;
	R.Xmin = R.Xmax - StringWidth(helpmsg) - 4;
	R.Ymin = insetR.Ymax + 4;
	R.Ymax = helpR.Ymax - 4;
	exitR = R;

	PaintRadioButton(&R, false, false, helpmsg);
	ExtraHilite(&R, false);



	R.Xmin = insetR.Xmin + 4;
	R.Xmax = exitR.Xmin - 8;
	PaintTextBox(&R, &searchR, "Search: ", "");

	move_to_corner(&exitR);
	LimitMouseRect(&helpR);

	ShowCursor();
	while (1)
	{
		int X, Y;
		int n;
		int button;


		key = 0;

		n = KeyEvent(false, &e);
		X = e.CursorX;
		Y = e.CursorY;
		button = (e.State >> 8) & 7;

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else if (e.ScanCode)
				key = e.ScanCode << 8;
			else
			{

				if (button == swRight)
					key = 0x1b;
				else if (button == swLeft)
				{
					if (XYInRect(X, Y, &searchR.nR))
						key = '/';

					else if (XYInRect(X, Y, &exitR))
					{
						PaintRadioButton(&exitR, true, true, helpmsg);
						ExtraHilite(&exitR, true);
						key = 0x1b;
						WaitForNothing();
						PaintRadioButton(&exitR, false, false, helpmsg);
						ExtraHilite(&exitR, false);
					}


					else if (XYInRect(X, Y, &bubbleRect))
					{
						int tl = topline;

						R = sliderRect;
						InsetRect(&R, 2, 2);
						LimitMouseRect(&R);
						while (button == swLeft)
						{
							int n;

							n = (Y - sliderRect.Ymin) / (float) (sliderRect.Ymax - sliderRect.Ymin) * linecount;
							n = min(n, linecount - BOXLEN);

							if (n != topline)
							{
								topline = n;
								slide_scroll_bar();
							}
							KeyEvent(false, &e);
							X = e.CursorX;
							Y = e.CursorY;
							button = (e.State & 0x700) >> 8;
						}
						scrollto(tl, topline, fd);
						LimitMouseRect(&helpR);
						continue;
					}
					else if (XYInRect(X, Y, &sliderRect))
					{
						int tl = topline;

						topline = (Y - sliderRect.Ymin) / (float) (sliderRect.Ymax - sliderRect.Ymin) * linecount;
						topline = min(topline, linecount - BOXLEN);
						update_scroll_bar();
						scrollto(tl, topline, fd);
						continue;
					}
					if (XYInRect(X, Y, &uaR))
					{
						int first = true;
						int lasttime = e.Time;


						LimitMouseRect(&uaR);

						HideCursor();
						InvertRect(&uaR);
						if (topline != 0)
						{
							scrollto(topline, topline - 1, fd);
							slide_scroll_bar();
						}
						ShowCursor();
						while (button == swLeft)
						{
							int banzai = false;

							KeyEvent(false, &e);
							X = e.CursorX;
							Y = e.CursorY;
							button = (e.State & 0x700) >> 8;
							if (first)
							{
								if (e.Time - lasttime > 4)
								{
									first = false;
									banzai = true;
								}
							}
							else
								banzai = (e.Time != lasttime);

							if (banzai)
							{
								if (topline != 0)
								{
									scrollto(topline, topline - 1, fd);
									slide_scroll_bar();
								}
							}
						}
						HideCursor();
						InvertRect(&uaR);
						ShowCursor();
						LimitMouseRect(&helpR);
					}
					else if (XYInRect(X, Y, &daR))
					{
						int first = true;
						int lasttime = e.Time;


						LimitMouseRect(&daR);

						HideCursor();
						InvertRect(&daR);
						if (topline < linecount - boxlen)
						{
							scrollto(topline, topline + 1, fd);
							slide_scroll_bar();
						}
						ShowCursor();
						while (button == swLeft)
						{
							int banzai = false;

							KeyEvent(false, &e);
							X = e.CursorX;
							Y = e.CursorY;
							button = (e.State & 0x700) >> 8;
							if (first)
							{
								if (e.Time - lasttime > 4)
								{
									first = false;
									banzai = true;
								}
							}
							else
								banzai = (e.Time != lasttime);

							if (banzai)
							{
								if (topline < linecount - boxlen)
								{
									scrollto(topline, topline + 1, fd);
									slide_scroll_bar();
								}
							}
						}
						HideCursor();
						InvertRect(&daR);
						ShowCursor();
						LimitMouseRect(&helpR);
					}
				}
			}
		}

		if (!key)
			continue;

		if (key == XF10)
		{
			char tbuf[128];

			if (select_file("Save screen as GIF file", "*.GIF", tbuf, "GIF"))
				GifOutput(tbuf, 1);
			LimitMouseRect(&helpR);
			continue;
		}


		if (key == 0x1b || key == 3 || key == 'q' || key == 'Q')
			break;

		if (key == '/' || key == '?')
		{
			/*
			 * This person is searching for a string. Let him, if
			 * he really wants to...
			 */
			char sbuf[128];

			int n;


			sbuf[0] = 0;
         matchbuf[0] = 0;
			n = GetText(&searchR, matchbuf, sbuf, BLACK, WHITE);

			PaintTextBoxEntry(&searchR, DARKGRAY, WHITE, matchbuf);
			if (n)
			{
				for (i = strlen(matchbuf) - 1; i >= 0 && matchbuf[i] == ' '; i--)
					matchbuf[i] = 0;
				if (!matchbuf[0])
				{
					strcpy(matchbuf, lastmatch);
					PaintTextBoxEntry(&searchR, DARKGRAY, WHITE, matchbuf);
				}
				strcpy(lastmatch, matchbuf);
				strupr(matchbuf);
				if (matchbuf[0])
				{
					int start;
					int found = 0;

					if (key == '/')
					{
						start = (highlighted_line != -1) ?
							highlighted_line + 1 :
							topline + 1;

						for (i = start; i < linecount; i++)
						{
							if (compare_line(fd, i, matchbuf))
							{
								found = 1;
								break;
							}
						}
					}
					else
					{
						start = (highlighted_line != -1) ?
							highlighted_line - 1 :
							topline - 1;
						for (i = start; i >= 0; i--)
						{
							if (compare_line(fd, i, matchbuf))
							{
								found = 1;
								break;
							}
						}
					}

					if (found)
					{
						topline = min(i, linecount - boxlen - 1);
					}
					else
						highlighted_line = -1;
					repaint_em_all(fd);
					update_scroll_bar();
					if (found)
					{
						rect hR;

						/*
						 * and highlight the selected
						 * line
						 */
						hR.Xmin = insetR.Xmin + 2;
						hR.Xmax = insetR.Xmax - 2;
						highlighted_line = i;
						hR.Ymin = starts[highlighted_line - topline].Y + 1;
						hR.Ymax = hR.Ymin + FontHeight - 2;
						InvertRect(&hR);
					}
				}
			}
			continue;
		}






		switch (key)
		{
#ifdef ZZZ
		case XF20:	/* alt-F10 */
			if (select_file("Save Screen", "*.gif", tbuf, "GIF"))
				SaveScreenGif(tbuf);
			break;
#endif
		case XUARROW:
			HideCursor();
			InvertRect(&uaR);
			if (topline != 0)
			{
				scrollto(topline, topline - 1, fd);
				slide_scroll_bar();
			}
			InvertRect(&uaR);
			ShowCursor();
			break;
		case XDARROW:
			HideCursor();
			InvertRect(&daR);
			if (topline < linecount - boxlen)
			{
				scrollto(topline, topline + 1, fd);
				slide_scroll_bar();
			}
			InvertRect(&daR);
			ShowCursor();

			break;
		case XHOME:
			/* Just repaint the whole thing */
			scrollto(topline, 0, fd);
			slide_scroll_bar();
			break;
		case XEND:
			scrollto(topline, linecount - boxlen, fd);
			slide_scroll_bar();
			break;
		case XPGUP:
			/* Go up boxlen lines, or to the top. */
			i = max(topline - 15, 0);
			scrollto(topline, i, fd);
			slide_scroll_bar();
			break;
		case XPGDN:
			i = min(topline + 15, linecount - boxlen);
			scrollto(topline, i, fd);
			slide_scroll_bar();
			break;
		}
		ProtectOff();
	}




	HideCursor();
	ClipRect(&sR);
cleanup1:
	PopRect(&err);

	ClipRect(&clipper);
cleanup0:
cleanup:
	if (linepointers)
	    free(linepointers);
	fclose(fd);

	WaitForNothing();
	PopMouseRect();
	PopCursorType();
	PopCursorPosition();
	ShowCursor();
}
