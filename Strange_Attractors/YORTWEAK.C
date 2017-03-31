#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>

#include "attract.h"
static char *SML[] = {"Small", "Medium", "Large"};
static rect flocksizeR[3];
static int our_flocksize;
static int our_flocktype;
static char *point_line[] = {"Point", "Line"};
static rect tracetypeR[2];
static int our_tracetype;

char *topologymsg[] = {"Torus", "Klein", "Projective"};
static rect topologyR[3];
static int our_topology;

static int items;
static rect *bR[40];

extern char *standard_button_texts[];

slider yorke_slider = {
	"Chaoticity",
	0.0, 0.0,
	0.0, 4.0
};

slider yorke1_slider = {
	"X Shift",
	0.0, 0.0,
	0.0, 1.0
};

slider yorke2_slider = {
	"Y Shift",
	0.0, 0.0,
	0.0, 1.0
};

static slider *sliders[] = {
	&yorke_slider,
	&yorke1_slider,
	&yorke2_slider
};

char *yorke_buttons[] = {
	"Randomize",
	"Reset"
};
static rect doitR[5];


static void push(int n, int inout)
{
	switch (n)
	{
		case -1:
		break;
	case 9:
	case 17:
	case 25:
		InvertInsides((text_button *) bR[n]);
		break;
	case 0:
	case 1:
	case 2:
		PushOrDoublePress(bR[n], inout, our_flocktype == n);
		break;
	case 3:
	case 4:
		PushOrDoublePress(bR[n], inout, our_tracetype == n - 3);
		break;
	case 5:
	case 6:
	case 7:
		PushOrDoublePress(bR[n], inout, our_topology == n - 5);
		break;
	case 36:
		ExtraHilite(bR[n], inout);
		/* deliberate flow through */
	default:
		PushButton(bR[n], inout);
		break;
	}
}

void yorke_tweaker(void)
{
	int imflag = false, swflag = false, ffflag = false;

	rect tR;		/* the whole thing */
	int i;
	int row;
	int last_time;
	int keyword;
	int centerx;
	int centery = sR.Ymax / 2;
	char chosen_file[128];
	int width;
	int height;
	int current_item;
	rect R;
	char *pc;


	static int lefters[] = {
		2, 0, 1,
		4, 5,
		7, 5, 6,
		8,
		15, 9, 10, 11, 12, 13, 14,
		16,
		23, 17, 18, 19, 20, 21, 22,
		24,
		31, 25, 26, 27, 28, 29, 30,
		33, 32,
		36, 34, 35
	};


	static int righters[] = {
		1, 2, 0,
		4, 3,
		6, 7, 5,
		8,
		10, 11, 12, 13, 14, 15, 9,
		16,
		18, 19, 20, 21, 22, 23, 17,
		24,
		26, 27, 28, 29, 30, 31, 25,
		33, 34,
		35, 36, 34
	};

	static int uppers[] = {
		34, 35, 36,
		1, 2,
		3, 3, 4,

		6,
		6, 8, 8, 8, 8, 8, 8,
		12,
		9, 16, 16, 16, 16, 16, 16,
		20,
		17, 24, 24, 24, 24, 24, 24,
		25, 29,
		32, 32, 33
	};

	static int downers[] = {
		3, 3, 4,
		5, 7,
		9, 8, 8,
		12,
		17, 16, 16, 16, 16, 16, 16,
		20,
		25, 24, 24, 24, 24, 24, 24,
		28,
		32, 32, 32, 32, 33, 33, 33,
		34, 36,
		0, 1, 3
	};

	char *msg1 = "Flock Size: ";
	char *msg2 = "World Shape: ";




	our_flocktype = flocktype;
	our_topology = yorketopologyflag;
	our_tracetype = tracetype;
	yorke_slider.value = epsilon;
	yorke1_slider.value = omega1;
	yorke2_slider.value = omega2;


	items = 0;

	height =
		2 + FontHeight + 8 +
		4 * (3 * FontHeight / 2) +
		2 * FontHeight +
		3 * (slider_height(&yorke_slider) + 4)
		+ 4;

	width = 2 * sR.Xmax / 3;

	HideCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Yorke Tweaks", BLACK);
	Centers(&tR, &centerx, &centery);
	PushMouseRectLimit(&tR);

	R.Xmin = tR.Xmin + StringWidth(msg1) + 12;
	R.Xmax = tR.Xmax - 4;
	//R.Xmax = tR.Xmax - 12 * 8;
	row = R.Ymin = tR.Ymin + FontHeight + 8;
	R.Ymax = R.Ymin + FontHeight + 4;
	JString(msg1, R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, SML, flocksizeR, 3, our_flocktype);
	for (i = 0; i < 3; i++)
		bR[items++] = &flocksizeR[i];


	//R.Xmin = flocksizeR[2].Xmax + 8;
	//R.Xmax = tR.Xmax - 4;
	//OffsetRect(&R, 0, -2);
	//flocksizeTB.nR = R;
	//PaintNumberBoxEntry(&flocksizeTB, our_flocksize, GS_UNSIGNED);
	//bR[items++] = (rect *) & flocksizeTB;

	row += 3 * FontHeight / 2;
	R.Xmin = centerx;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	JString("Trace Type: ", R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, point_line, tracetypeR, 2, our_tracetype);
	for (i = 0; i < 2; i++)
		bR[items++] = &tracetypeR[i];

	row += 3 * FontHeight / 2;
	R.Xmin = tR.Xmin + 4 + StringWidth(msg2) + 12;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	JString(msg2, R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, topologymsg, topologyR, 3, our_topology);
	for (i = 0; i < 3; i++)
		bR[items++] = &topologyR[i];





	row += 3 * FontHeight / 2;
	create_slider(&yorke_slider, &tR, 4, row - tR.Ymin);
	bR[items++] = &yorke_slider.bR;
	bR[items++] = &yorke_slider.TB.nR;
	for (i = 0; i < 6; i++)
		bR[items++] = &yorke_slider.zR[i];


	row = yorke_slider.tR.Ymax + 4;
	create_slider(&yorke1_slider, &tR, 4, row - tR.Ymin);
	bR[items++] = &yorke1_slider.bR;
	bR[items++] = &yorke1_slider.TB.nR;
	for (i = 0; i < 6; i++)
		bR[items++] = &yorke1_slider.zR[i];

	row = yorke1_slider.tR.Ymax + 4;
	create_slider(&yorke2_slider, &tR, 4, row - tR.Ymin);
	bR[items++] = &yorke2_slider.bR;
	bR[items++] = &yorke2_slider.TB.nR;
	for (i = 0; i < 6; i++)
		bR[items++] = &yorke2_slider.zR[i];

	row = yorke2_slider.tR.Ymax + FontHeight / 2;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	CreateRadioPanel(&R, yorke_buttons, doitR, 2, -1);

	OffsetRect(&R, 0, 2 * FontHeight);
	CreateRadioPanel(&R, standard_button_texts, doitR + 2, 3, -1);

	for (i = 0; i < 5; i++)
		bR[items++] = &doitR[i];

	current_item = items - 1;
	push(current_item, true);
	move_to_corner(bR[current_item]);

	ShowCursor();
	while (1)
	{
		event e;
		int n = KeyEvent(false, &e);
		int button = (e.State & 0x700) >> 8;
		int X = e.CursorX;
		int Y = e.CursorY;
		int last_item = current_item;
		slider *current_slider = NULL;
		int slider_n;

		for (i = 0; i < 3; i++)
		{
			if (XYInRect(X, Y, &sliders[i]->tR))
			{
				current_slider = sliders[i];
				slider_n = i;
				break;
			}
		}
		keyword = 0;
		if (n)
			last_time = e.Time;
		else
		{
			if (e.Time - last_time > 5)
			{
				if (button)
					n = 1;
				else
					last_time = e.Time;
			}
		}
		if (n)
		{
			keyword = ShiftArrows(&e);
			if (button == swRight)
				keyword = 0x1b;

			if (!keyword)
			{
				if (e.ASCII && e.ASCII != 0xe0)
					keyword = e.ASCII;
				else
					keyword = e.ScanCode << 8;
			}
			if (button == swLeft)
			{
				keyword = 0x0d;
				if (current_slider)
				{
					if (XYInRect(X, Y, &current_slider->bR))
					{
						PushMouseRectLimit(&current_slider->sR);
						while (button == swLeft)
						{
							reposition_slider_X(current_slider, X, false);
							KeyEvent(false, &e);
							button = (e.State & 0x700) >> 8;
							X = e.CursorX;
							Y = e.CursorY;
						}
						PopMouseRect();
						keyword = 0;
					}
					else if (XYInRect(X, Y, &current_slider->sR))
					{
						reposition_slider_X(current_slider, X, false);
						keyword = 0;
					}
				}
			}

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

		if (keyword == 0x1b)
		{
			keyword = 0x0d;
			current_item = items - 2;
		}

		i = keyword & ~1;

		if ((i == XLARROW || i == XRARROW
		     || i == XCLARROW || i == XCRARROW) && current_slider && XYInRect(X, Y, &current_slider->bR))
		{
			int shifted = (keyword & 1);
			int controlled = (i == XCLARROW || i == XCRARROW);

			double ink;

			if (shifted)
				ink = 0.1;
			else if (controlled)
				ink = 0.001;
			else
				ink = 0.01;

			if (i == XLARROW || i == XCLARROW)
				ink = -ink;
			current_slider->value += ink;
			reposition_slider_v(current_slider, false);
			move_to_corner(&current_slider->bR);
		}
		else
			navigate(keyword, lefters, righters, uppers, downers, items, bR, &current_item);

		if (keyword == XF1)
		{
			current_item = items - 3;
			keyword = 0x0d;
		}

		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}

		if (keyword == XF10)
		{
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF"))
				SaveScreenGif(chosen_file);
			break;
		}

		if (keyword == 0x0d)
		{
			if (current_item == items - 3)
				helptext("attytwk.hlp");
			else if (current_item >= items - 5 && current_item <= items - 1)
				break;
			if (current_slider)
			{
				int n = ProcessSlider(current_slider, current_item - (slider_n * 8 + 8));

				if (n == 2)
					push(current_item, true);
			}
			CheckRadioButtons(X, Y, flocksizeR, 3, &our_flocktype, SML);
			CheckRadioButtons(X, Y, tracetypeR, 2, &our_tracetype, point_line);
			CheckRadioButtons(X, Y, topologyR, 3, &our_topology, topologymsg);

		}
	}


	R = doitR[current_item - (items - 5)];
	if (current_item >= items - 2)
		pc = standard_button_texts[current_item - (items - 3)];
	else
		pc = yorke_buttons[current_item - (items - 5)];

	PaintRadioButton(&R, true, true, pc);
	if (current_item == items - 1)
	{
		ExtraHilite(&R, true);
		DoublePress(&R, true, RED);
	}
	WaitForNothing();
	PaintRadioButton(&R, false, false, pc);
	if (current_item == items - 1)
	{
		ExtraHilite(&R, false);
		DoublePress(&R, false, RED);
	}

	HideCursor();
	PopRect(&i);
	PopMouseRect();
	PopCursorPosition();
	ShowCursor();

	if (current_item == items - 4)
		yorkenumber = -1;
	if (current_item == items - 4 || current_item == items - 5)
	{
		randomize_yorkers();
		imflag = ffflag = swflag = true;
	}


	if (current_item == items - 1)
	{

		if (Stamping && (our_tracetype != tracetype ||
				 our_flocksize != fflock2ptr->n ||
				 our_flocktype != flocktype ||
				 our_topology != yorketopologyflag ||
				 yorke_slider.value != epsilon ||
				 yorke1_slider.value != omega1 ||
				 yorke2_slider.value != omega2))
		{
			preserve_data();
			slide_stamps();
		}


		if (our_flocktype != flocktype)
		{
			flocktype = our_flocktype;
			imflag = 1;
			ffflag = 1;
			starting_trihedroncount = trihedronon = trihedroncount = 0;
		}

		if (our_topology != yorketopologyflag)
		{
			yorketopologyflag = our_topology;
			imflag = /* ffflag = */ 1;
		}

		if (yorke_slider.value != epsilon)
		{
			epsilon = yorke_slider.value;
			epsbar = epsilon / TWOPI;
			imflag = 1;
			ffflag = 1;
		}

		if (yorke1_slider.value != omega1)
		{
			omega1 = yorke1_slider.value;
			imflag = ffflag = 1;
		}

		if (yorke2_slider.value != omega2)
		{
			omega2 = yorke2_slider.value;
			imflag = ffflag = 1;
		}

		if (our_tracetype != tracetype)
		{
			tracetype = our_tracetype;
			if (tracetype == 0)
				imflag = 1;
			ribbonindex = 0;
			ribbonfull = 0;
		}
	}
	if (imflag)
		installmode();
	if (ffflag)
		fillflock();
	if (swflag)
		setwindow(0);

}
