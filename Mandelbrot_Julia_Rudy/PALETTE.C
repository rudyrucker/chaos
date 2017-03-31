#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include <math.h>
#include INCLUDE_FILE
#include <alloc.h>

slider Rslider = {"", 0, 0, 0, 63};
slider Gslider = {"", 0, 0, 0, 63};
slider Bslider = {"", 0, 0, 0, 63};
slider *RGB[] = {
	&Rslider, &Gslider, &Bslider
};


static char ourcolortable[64 * 3];
static unsigned char ourct[17];
unsigned char startct[17];

unsigned char startvgapalette[16 * 3];
unsigned char startegapalette[16];

void rusepalette(void);
void PushRect(rect *, int *);
void PopRect(int *);
void create_slider_base(slider *, rect *, int, int, int, int, int, int);
void reposition_slider_v_base(slider *, int, int, int);

int ega_rectify(int n)
{
	if (n < 11)
		return 0;
	if (n < 31)
		return 21;
	if (n < 52)
		return 42;
	return 63;
}

int EGA_equivalent(char *v)
{
	int j;

	for (j = 0; j < 64; j++)
	{
		if (defaultpalette[j][0] == v[0] &&
		    defaultpalette[j][1] == v[1] &&
		    defaultpalette[j][2] == v[2])
			return j;
	}
	return 0;
}



int LoadOurPalette(char *filename)
{
	FILE *fd = fopen(filename, "rt");
	char tbuf[128];
	int ti, tr, tg, tb;
	int i;

	if (!fd)
		return 0;

	while (1)
	{
		fgets(tbuf, 128, fd);
		if (feof(fd))
			break;

		if (sscanf(tbuf, "%d: %d %d %d", &ti, &tr, &tg, &tb) == 4)
		{
			if (!hasVGA)
			{
				tr = ega_rectify(tr);
				tg = ega_rectify(tg);
				tb = ega_rectify(tb);
			}


			ourcolortable[(ti - 1) * 3 + 0] = tr;
			ourcolortable[(ti - 1) * 3 + 1] = tg;
			ourcolortable[(ti - 1) * 3 + 2] = tb;
		}
	}

	if (!hasVGA)
	{

		/* rectify our colortable to agree with this one */
		for (i = 0; i < 16; i++)
			ourct[i] = EGA_equivalent(&ourcolortable[i * 3]);
	}
	else
	{
		for (i = 0; i < 16; i++)
			ourct[i] = i;
	}






	fclose(fd);
	return 1;
}




void SaveOurPalette(char *filename)
{
	/*
	 * Save the color table as an ascii file... if VGA mode, 16 triplets
	 * if EGA mode, 16 numbers
	 */

	FILE *fd;
	int i;
	time_t timex = time(NULL);


	if (!Overwrite(filename))
		return;

	fd = fopen(filename, "wt");
   if (!fd)
   {
      FileError(filename,NULL);
      return;
   }


	fprintf(fd, "CHAOS Color Table\n");
	fprintf(fd, "Created: %s", ctime(&timex));
	for (i = 0; i < 16; i++)
		fprintf(fd, "%02d: %02d %02d %02d\n", i + 1,
			ourcolortable[ourct[i] * 3 + 0],
			ourcolortable[ourct[i] * 3 + 1],
			ourcolortable[ourct[i] * 3 + 2]);

   if (ferror(fd))
   {
      FileError(filename,fd);
      fclose(fd);
      remove(filename);
   }
   else
   	fclose(fd);
}

static int sliderwidth;
static int current_item;
static char *msgs[] =
{"L to Load", "S to Save", "F1 for HELP", "ESC to Cancel", "ACCEPT"};

extern int palettenumber;

void LoadDefaultPalette(char *filename)
{
	if (LoadOurPalette(filename))
	{
		if (mode == 0x12)
		{
			int i;

			memcpy(vgacolortable, ourcolortable, 16 * 3);
			for (i = 0; i < 16; i++)
				egacolortable[i] = i;
		}
		else
			memcpy(egacolortable, ourct, 16);
	}
	memcpy(startvgapalette, vgacolortable, 16 * 3);
	memcpy(startegapalette, egacolortable, 16);
}

void DefaultPalette(void)
{
	memcpy(vgacolortable, startvgapalette, 16 * 3);
	memcpy(egacolortable, startegapalette, 16);
	memcpy(ourcolortable, startvgapalette, 16 * 3);
	memcpy(ourct, egacolortable, 16);
	_useEGApalette(ourct);
	if (hasVGA)
		_useVGApalette((unsigned char *) ourcolortable);
}

int current_color;




void update_numbers(int current_color)
{
	char tbuf[128];

	HideCursor();
	TextAlign(alignLeft, alignTop);
	MoveTo(Rslider.sR.Xmax + 4, Rslider.sR.Ymin - 2);
	PenColor(BLACK);
	BackColor(LIGHTGRAY);
	sprintf(tbuf, "%2.2d", ourcolortable[ourct[current_color] * 3 + 0]);
	DrawString(tbuf);
	MoveTo(Gslider.sR.Xmax + 4, Gslider.sR.Ymin - 2);
	sprintf(tbuf, "%2.2d", ourcolortable[ourct[current_color] * 3 + 1]);
	DrawString(tbuf);
	MoveTo(Bslider.sR.Xmax + 4, Bslider.sR.Ymin - 2);
	sprintf(tbuf, "%2.2d", ourcolortable[ourct[current_color] * 3 + 2]);
	DrawString(tbuf);
	ShowCursor();
}





void update_slider_values(void)
{
	Rslider.value = ourcolortable[ourct[current_color] * 3 + 0];
	Gslider.value = ourcolortable[ourct[current_color] * 3 + 1];
	Bslider.value = ourcolortable[ourct[current_color] * 3 + 2];
}

void update_sliders(void)
{
	reposition_slider_v_base(&Rslider, 0, false, false);
	reposition_slider_v_base(&Gslider, 0, false, false);
	reposition_slider_v_base(&Bslider, 0, false, false);
}

static void update_everything(slider * s, int current_color, int i)
{
	union REGS regs;

	ourcolortable[ourct[current_color] * 3 + i] = s->value;
	HideCursor();

	if (!hasVGA)
	{
		int ec = EGA_equivalent(&ourcolortable[3 * ourct[current_color]]);

		ourct[current_color] = ec;
		memcpy(&ourcolortable[3 * ourct[current_color]], defaultpalette[ec], 3);
		regs.h.al = 0;
		regs.h.ah = 0x10;
		regs.h.bh = ec;
		regs.h.bl = current_color;
		int86(0x10, &regs, &regs);
	}
	else
	{
		regs.h.ah = 0x10;
		regs.h.al = 0x10;
		regs.x.bx = ourct[current_color];

		regs.h.ch = ourcolortable[ourct[current_color] * 3 + 1];
		regs.h.cl = ourcolortable[ourct[current_color] * 3 + 2];
		regs.h.dh = ourcolortable[ourct[current_color] * 3 + 0];

		int86(0x10, &regs, &regs);
	}
	update_numbers(current_color);
	reposition_slider_v_base(s, 0, false, false);

	ShowCursor();
}


void palette_tweaker(void)
{
	int height, width;
	rect R, tR;
	rect cR[16];
	int boxwidth;
	int i;
	int cx, cy;
	rect doitR[5];
	rect *bR[30];
	int items = 0;
	int bubblewidth;
	static int egastarts[4];
	static int egavals[] = {0, 21, 42, 63};
	int doit = 0;
	char tbuf[128];
	int boxheight;
	int loadfile, savefile, helpme;
   extern void ArrowCursor(void);
	static int lefters[24] = {
		7, 0, 1, 2, 3, 4, 5, 6,
		15, 8, 9, 10, 11, 12, 13, 14,
		16,
		17,
		18,
		20, 19,
		23, 21, 22
	};

	static int righters[24] = {
		1, 2, 3, 4, 5, 6, 7, 0,
		9, 10, 11, 12, 13, 14, 15, 8,
		16,
		17,
		18,
		20, 19,
		22, 23, 21
	};

	static int uppers[24] = {
		21, 21, 21, 22, 22, 23, 23, 23,
		0, 1, 2, 3, 4, 5, 6, 7,
		11,
		16,
		17,
		18, 18,
		19, 19, 20
	};

	static int downers[24] = {
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 16, 16, 16, 16, 16, 16, 16,
		17,
		18,
		19,
		21, 23,
		0, 3, 7
	};



	if (hasVGA)
	{
		memcpy(ourcolortable, vgacolortable, 16 * 3);
		memcpy(ourct, egacolortable, 16);
	}
	else
	{
		/* get the current color table */

		memcpy(ourct, egacolortable, 17);
		memcpy(ourcolortable, defaultpalette, 64 * 3);
	}



top:
	items = 0;
	loadfile = savefile = helpme = false;

	current_color = 0;
	HideCursor();
	ProtectOff();
	PushCursorPosition();
	PushCursorType();
	ArrowCursor();

	Centers(&sR, &cx, &cy);


	width = sR.Xmax / 2 + 40;

	height = 2 * (width - 12) / (8 * aspect) + 5 + 3 * (3 * FontHeight / 2)
		+ 2 * (3 * FontHeight / 2) + FontHeight + 8;

	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Palette Editor", BLACK);
	PushMouseRectLimit(&tR);

	boxwidth = (tR.Xmax - tR.Xmin - 8) / 8;
	boxheight = boxwidth / aspect;

	for (i = 0; i < 16; i++)
	{
		rect S;

		S.Xmin = tR.Xmin + 4 + (i % 8) * boxwidth;
		S.Xmax = S.Xmin + boxwidth - 2;
		S.Ymin = tR.Ymin + FontHeight + 4 + (i / 8) * boxheight;
		S.Ymax = S.Ymin + boxheight - 2;

		RasterOp(zREPz);
		PenColor(i);
		PaintRect(&S);
		PushButton(&S, false);
		cR[i] = S;
		bR[items++] = &cR[i];

	}

	if (hasVGA)
		bubblewidth = (width - 40) / 32;
	else
		bubblewidth = (width - 40) / 4 - 10;


	create_slider_base_width(&Rslider, &tR, 20, 2 * boxheight + 5, false, false, false, false, bubblewidth);
	bR[items++] = &Rslider.bR;
	create_slider_base_width(&Gslider, &tR, 20, 2 * boxheight + 5 + FontHeight + 4, false, false, false, false, bubblewidth);
	bR[items++] = &Gslider.bR;
	create_slider_base_width(&Bslider, &tR, 20, 2 * boxheight + 5 + 2 * (FontHeight + 4), false, false, false, false, bubblewidth);
	bR[items++] = &Bslider.bR;
	PenColor(BLACK);
	BackColor(LIGHTGRAY);
	TextAlign(alignRight, alignTop);
	MoveTo(Rslider.sR.Xmin - 4, Rslider.sR.Ymin - 2);
	DrawString("R");
	MoveTo(Gslider.sR.Xmin - 4, Gslider.sR.Ymin - 2);
	DrawString("G");
	MoveTo(Bslider.sR.Xmin - 4, Bslider.sR.Ymin - 2);
	DrawString("B");

	sliderwidth = Rslider.sR.Xmax - Rslider.sR.Xmin;

	egastarts[0] = sliderwidth / 4;
	egastarts[1] = sliderwidth / 2;
	egastarts[2] = 3 * sliderwidth / 4;

	R.Ymin = Bslider.sR.Ymax + FontHeight / 2;
	R.Ymax = R.Ymin + FontHeight + 4;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;

	CreateRadioPanel(&R, msgs, doitR, 2, -1);
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	CreateRadioPanel(&R, msgs + 2, doitR + 2, 3, -1);
	for (i = 0; i < 5; i++)
		bR[items++] = &doitR[i];

	current_item = items - 1;
	PushButton(bR[current_item], true);
	ExtraHilite(bR[current_item], true);
	move_to_corner(bR[current_item]);
	PushButton(bR[current_color], true);

	update_slider_values();
	update_numbers(current_color);
	ShowCursor();
	while (1)
	{
		event e;
		int n;
		int X;
		int Y;
		int button;
		int key = 0;
		int last_item = current_item;

		n = KeyEvent(false, &e);
		X = e.CursorX;
		Y = e.CursorY;
		button = (e.State & 0x700) >> 8;

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else if (e.ScanCode != 0xff)
				key = e.ScanCode << 8;

			if (button == swRight)
				key = 0x1b;

			if (button == swLeft)
			{
				/*
				 * We might want to deal with the slider
				 * here.
				 */
				for (i = 0; i < 3; i++)
				{
					slider *s = RGB[i];

					if (XYInRect(X, Y, &s->bR))
					{
						PushMouseRectLimit(&s->sR);
						while (button == swLeft)
						{
							double n = s->value;

							if (hasVGA)
							{
								s->value = (X - s->sR.Xmin) * 63.0 / sliderwidth;
								if (floor(n) != floor(s->value))
									update_everything(s, current_color, i);
							}
							else
							{
								int j;
								int z = 0;

								for (j = 0; j < 3; j++)
								{
									if ((X - s->sR.Xmin) > egastarts[j])
										z = egavals[j + 1];
								}

								if (floor(n) != z)
								{
									s->value = z;
									update_everything(s, current_color, i);
								}
							}

							KeyEvent(false, &e);
							button = (e.State & 0x700) >> 8;
							X = e.CursorX;
							Y = e.CursorY;
						}
						PopMouseRect();
						continue;
					}
					else if (XYInRect(X, Y, &s->sR))
					{
						double n = s->value;

						if (hasVGA)
						{
							s->value = (X - s->sR.Xmin) * 63.0 / sliderwidth;
							if (floor(n) != floor(s->value))
								update_everything(s, current_color, i);
						}
						else
						{
							int j;
							int z = 0;

							for (j = 0; j < 3; j++)
							{
								if ((X - s->sR.Xmin) > egastarts[j])
									z = egavals[j + 1];
							}

							if (floor(n) != z)
							{
								s->value = z;
								update_everything(s, current_color, i);
							}
						}

						continue;
					}


				}
				key = 0x0d;
			}

			if (key == 0x1b)
				break;
		}
		else
		{
			current_item = -1;

			for (i = 0; i < items; i++)
			{
				if (XYInRect(X, Y, bR[i]))
				{
					current_item = i;
					break;
				}
			}
		}

		if (key == XALTW)
			InfoBox();


		if (key == XF10)
		{
			char tbuf[128];

			if (select_file("Save screen as GIF file", "*.GIF", tbuf, "GIF"))
				GifOutput(tbuf, 1);
			continue;
		}


		if (key == XF1)
		{
			current_item = items - 3;
			key = 0x0d;
		}
		else if (key == 'L' || key == 'l')
		{
			current_item = items - 5;
			key = 0x0d;
		}
		else if (key == 'S' || key == 's')
		{
			current_item = items - 4;
			key = 0x0d;
		}

		if (key == 0x0d)
		{
			if (current_item < 16 && current_item >= 0 && current_item != current_color)
			{
				PushButton(bR[current_color], false);
				current_color = current_item;
				PushButton(bR[current_color], true);
				DoublePress(bR[current_color], true, current_color);
				update_numbers(current_color);
				update_slider_values();
				update_sliders();
			}
			else if (current_item == items - 1)
			{
				doit = 1;
				break;
			}
			else if (current_item == items - 2)
			{
				doit = false;
				break;
			}
			else if (current_item == items - 3)
			{
				helpme = true;
				break;
			}
			else if (current_item == items - 4)
			{
				savefile = true;
				break;
			}
			else if (current_item == items - 5)
			{
				loadfile = true;
				break;
			}


		}


		if ((key == XLARROW || key == '4')&& current_item >= 16 && current_item <= 18)
		{
			slider *s = RGB[current_item - 16];

			if (hasVGA)
			{
            if (e.State & 0x3)
               s->value -= 10;
            else
   				s->value--;
				if (s->value < 0)
					s->value = 63;
			}
			else
			{
				s->value -= 21;
				if (s->value < 0)
					s->value = 63;
			}
			update_everything(s, current_color, current_item - 16);
			move_to_corner(&s->bR);
			continue;
		}
		if ((key == XRARROW || key == '6') && current_item >= 16 && current_item <= 18)
		{
			slider *s = RGB[current_item - 16];

			if (hasVGA)
			{
            if (e.State & 0x3)
   				s->value += 10;
            else
               s->value++;
				if (s->value > 63)
					s->value = 0;
			}
			else
			{
				s->value += 21;
				if (s->value > 63)
					s->value = 0;
			}
			update_everything(s, current_color, current_item - 16);
			move_to_corner(&s->bR);
			continue;
		}

		if ((key == XHOME || key == XEND) && current_item >= 16 && current_item <= 18)
		{
			slider *s = RGB[current_item - 16];

			if (key == XHOME)
				s->value = 0;
			else
				s->value = 63;
			update_everything(s, current_color, current_item - 16);
			move_to_corner(&s->bR);
			continue;
		}








		navigate(key, lefters, righters, uppers, downers, items, bR, &current_item);

		if (last_item != current_item)
		{
			if (last_item != -1)
			{
				PushButton(bR[last_item], false);
				if (last_item == items - 1)
					ExtraHilite(bR[last_item], false);
				if (last_item == current_color)
				{
					DoublePress(bR[last_item], false, last_item);
					PushButton(bR[last_item], true);
				}
			}
			if (current_item != -1)
			{
				PushButton(bR[current_item], true);
				if (current_item == items - 1)
					ExtraHilite(bR[current_item], true);
				if (current_item == current_color)
				{
					PushButton(bR[current_color], true);
					DoublePress(bR[current_color], true, current_color);
				}
			}
		}

	}


	HideCursor();
	PopRect(&i);
	PopCursorType();
	PopCursorPosition();
	PopMouseRect();
	ShowCursor();

	if (helpme)
	{
		helptext("mandpal.hlp");
		goto top;
	}

	if (loadfile)
	{
		if (select_file("Load Palette", "*.pal", tbuf, "PAL"))
		{
			if (LoadOurPalette(tbuf))
			{
				/*
				 * we ned to rewrite a mess here
				 */
				/*
				 * Now tell the world of our palette
				 */

				_useEGApalette(ourct);
				if (hasVGA)
					_useVGApalette((unsigned char *) ourcolortable);
			}
		}
		goto top;
	}
	if (savefile)
	{
		if (select_file("Save Palette", "*.pal", tbuf, "PAL"))
			SaveOurPalette(tbuf);
		goto top;
	}


	if (!doit)
		usepalette();
	else
	{
		memcpy(egacolortable, ourct, 16);
		if (hasVGA)
			memcpy(vgacolortable, ourcolortable, 16 * 3);
	}
}
