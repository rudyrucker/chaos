#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>

#include "attract.h"

static rect doitR[3];
extern char *standard_button_texts[];
static rect accuracyR[3];
static int items;
static int our_accuracy;

static char *LMH[] = {"Low", "Medium", "High"};

static slider chaotic_slider = {
	"Chaoticity",
	0.0, 0.0,
	0.001, 3.999
};

static slider humpshift_slider = {
	"Humpspot",
	0.0, 0.0,
	0.1, 0.9
};


static rect *bR[30];

double slider_increments[] = {-.1, -.01, -.001, .001, .01, .1};

static void push(int n, int inout)
{
	if (n == -1)
		return;
	else if (n < 3)
		PushOrDoublePress(bR[n], inout, our_accuracy == n);
	else if (n == 4 || n == 12)
		InvertInsides((text_button *) bR[n]);
	else
		PushButton(bR[n], inout);
	if (n == items - 1)
		ExtraHilite(bR[n], inout);
}

void logistic_tweaker(void)
{
	rect tR;		/* the whole thing */
	int err;
	event e;
	int i;
	int row;
	int last_time;
	double start_chaoticity;
	int keyword;
	char chosen_file[128];
	int width = 2 * sR.Xmax / 3;
	int height;
	int current_item;
	short X, Y;
	rect R;
	int start_accuracy;

	/* since we just have one of them */
	int swflag = 0;
	int imflag = 0;
	double start_humpshift;
	char *pc;

	static int lefters[] = {
		2, 0, 1,
		3,
		4,
		10, 5, 6, 7, 8, 9,
		11,
		12,
		18, 13, 14, 15, 16, 17,
		21, 19, 20,
	};

	static int righters[] = {
		1, 2, 0,
		3,
		4,
		6, 7, 8, 9, 10, 5,
		11,
		12,
		14, 15, 16, 17, 18, 13,
		20, 21, 19
	};

	static int uppers[] = {
		19, 20, 21,
		1,
		3,
		4, 4, 4, 4, 4, 4,
		7,
		11,
		12, 12, 12, 12, 12, 12,
		13, 15, 17,
	};

	static int downers[] = {
		3, 3, 3,
		4,
		7,
		11, 11, 11, 11, 11, 11,
		12,
		15,
		19, 19, 20, 20, 21, 21,
		0, 1, 2
	};


	items = 0;

	height = 2 + FontHeight + 8 +
		3 * FontHeight / 2 + 4 +
		slider_height(&chaotic_slider) + 4 +
		slider_height(&humpshift_slider) + 4 +
		(3 * FontHeight / 2) + FontHeight / 2;

	HideCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Logistic Tweaks", BLACK);
	PushMouseRectLimit(&tR);


	if (standardstartlog < 20)
		our_accuracy = 0;
	else if (standardstartlog < 50)
		our_accuracy = 1;
	else
		our_accuracy = 2;
	start_accuracy = our_accuracy;


	R.Xmin = tR.Xmin + (tR.Xmax - tR.Xmin) / 3;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = tR.Ymin + FontHeight + 8;
	R.Ymax = R.Ymin + FontHeight + 4;
	JString("Accuracy: ", R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, LMH, accuracyR, 3, tracetype ? -1 : our_accuracy);
	for (i = 0; i < 3; i++)
		bR[items++] = &accuracyR[i];

	start_chaoticity = chaotic_slider.value = lvfx;
	create_slider(&chaotic_slider, &tR, 4, accuracyR[0].Ymax + 4 - tR.Ymin);
	bR[items++] = &chaotic_slider.bR;
	bR[items++] = (rect *) & chaotic_slider.TB;
	for (i = 0; i < 6; i++)
		bR[items++] = &chaotic_slider.zR[i];


	start_humpshift = humpshift_slider.value = humpspot;
	create_slider(&humpshift_slider, &tR, 4, chaotic_slider.tR.Ymax + 4 - tR.Ymin);
	bR[items++] = &humpshift_slider.bR;
	bR[items++] = (rect *) & humpshift_slider.TB;
	for (i = 0; i < 6; i++)
		bR[items++] = &humpshift_slider.zR[i];

	current_item = 0;
	row = humpshift_slider.tR.Ymax + FontHeight / 2;

	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;
	CreateRadioPanel(&R, standard_button_texts, doitR, 3, -1);
	for (i = 0; i < 3; i++)
		bR[items++] = &doitR[i];

	/* Now blank out the irrelevant things. */
	if (tracetype != 0)
	{
		for (i = 0; i < 3; i++)
		{
			R = *bR[i];
			InsetRect(&R, 1, 1);
			GrayOut(&R);
		}
	}
	else
	{
		R = chaotic_slider.tR;
		InsetRect(&R, 1, 1);
		GrayOut(&R);
	}


	ShowCursor();
	current_item = items - 1;
	push(current_item, true);
	move_to_corner(bR[current_item]);
	while (1)
	{
		int button;
		int last_item = current_item;

		slider *current_slider = NULL;


		int n = KeyEvent(false, &e);

		button = e.State >> 8;

		X = e.CursorX;
		Y = e.CursorY;

		if (XYInRect(X, Y, &chaotic_slider.tR) && tracetype != 0)
			current_slider = &chaotic_slider;
		else if (XYInRect(X, Y, &humpshift_slider.tR))
			current_slider = &humpshift_slider;


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
				break;
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
					if (tracetype == 0 && i >= 3 && i <= 10)
						current_item = -1;
					if (tracetype != 0 && i < 3)
						current_item = -1;
					break;
				}
			}
		}

		if (keyword == 0x0d)
		{
			if (current_item == 4 && tracetype != 0)
			{
				double d = chaotic_slider.value;

				if (GetNumber(&chaotic_slider.TB, &d, GS_FLOAT, 0.001, 3.999))
				{
					chaotic_slider.value = d;
					reposition_slider_v(&chaotic_slider, false);
				}
				push(current_item, true);
			}
			else if (current_item == 12)
			{
				double d = humpshift_slider.value;

				if (GetNumber(&humpshift_slider.TB, &d, GS_FLOAT, 0.1, 0.9))
				{
					humpshift_slider.value = d;
					reposition_slider_v(&humpshift_slider, false);
				}
				push(current_item, true);
			}
			else if (current_item == items - 3)
				keyword = XF1;
			else if (current_item == items - 2)
				keyword = 0x1b;
			else if (current_item == items - 1)
				break;
		}

		if (keyword == 0x1b)
			break;

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
		{
			while (1)
			{
				navigate(keyword, lefters, righters, uppers, downers, items, bR, &current_item);
				if (tracetype == 0 && !(current_item >= 3 && current_item <= 10))
					break;
				if (tracetype != 0 && !(current_item >= 0 && current_item <= 2))
					break;
			}
		}


		if (keyword == 0x0d)
		{
			if (current_item < 3 && current_item != -1 && tracetype == 0)
				CheckRadioButtons(X, Y, accuracyR, 3, &our_accuracy, LMH);
			else if (current_item >= 5 && current_item <= 18 && current_item != 11 && current_item != 12)
			{
				int offset;

				if (current_item <= 10)
					offset = 5;
				else
					offset = 13;
				current_slider->value += slider_increments[current_item - offset];
				reposition_slider_v(current_slider, false);
			}
		}


		switch (keyword)
		{
		case XF10:
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF"))
				SaveScreenGif(chosen_file);
			break;

		case XF1:
			helptext("attlgtwk.hlp");
			break;
		}

		if (current_item != last_item)
		{
			push(last_item, false);
			push(current_item, true);
		}
	}

	if (keyword == 0x0d && current_item == items - 1)
		i = 2;
	else
		i = 1;



	R = doitR[i];
	pc = standard_button_texts[i];

	PaintRadioButton(&R, true, true, pc);
	if (i == 2)
	{
		ExtraHilite(&R, true);
		DoublePress(&R, true, RED);
	}
	WaitForNothing();
	PaintRadioButton(&R, false, false, pc);
	if (i == 2)
	{
		ExtraHilite(&R, false);
		DoublePress(&R, false, RED);
	}





	HideCursor();
	PopRect(&err);
	PopMouseRect();
	PopCursorPosition();

	if (keyword == 0x0d && current_item == items - 1)
	{
		int sdone = 0;

		if (start_accuracy != our_accuracy)
		{
			if (Stamping && !sdone)
			{
				preserve_data();
				slide_stamps();
				sdone = 1;
			}
			logisticlaunchflag = 1;
			switch (our_accuracy)
			{
			case 0:
				standardstartlog = 1;
				standardstoplog = 16;
				break;
			case 1:
				standardstartlog = 20;
				standardstoplog = 50;
				break;
			case 2:
				standardstartlog = 50;
				standardstoplog = 150;
				break;
			}
			imflag = 1;
			swflag = 1;
		}

		if (start_chaoticity != chaotic_slider.value)
		{
			if (Stamping && !sdone)
			{
				preserve_data();
				slide_stamps();
				sdone = 1;
			}
			lvfx = chaotic_slider.value;
			logisticlaunchflag = 1;
			imflag = 1;
			swflag = 1;
		}

		if (start_humpshift != humpshift_slider.value)
		{
			if (Stamping && !sdone)
			{
				preserve_data();
				slide_stamps();
				sdone = 1;
			}
			humpspot = humpshift_slider.value;
			humpshift = log(0.5) / log(humpspot);
			logisticlaunchflag = 1;
			imflag = 1;
			swflag = 1;
			fancyflag = 1;
		}


		if (imflag)
			installmode();
		if (swflag)
			setwindow(0);
      announceparms();
	}

	ShowCursor();
}
