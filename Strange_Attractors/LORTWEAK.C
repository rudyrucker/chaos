#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>

#include "attract.h"

static int items;
static int our_flock_value;
static int our_flocksize;
static int our_view;
static int our_accuracy_value;
static int our_accuracy;
static int our_trace;
static rect traceR[2];
static rect doitR[3];

static char *SML[] = {"Small", "Medium", "Large"};
static rect flocksizeR[3];
static text_button flocksizeTB;
static rect viewR[4];
static rect accuracyR[3];
static text_button accuracyTB;

static char *XYZF[] = {"X", "Y", "Z", "Fly"};
static char *offon[] = {"Off", "On"};
static rect *bR[30];
static char *LMH[] = {"Low", "Medium", "High"};

static rect doitR[3];
#define _sign(x) ( ((x) < 0) ? -1 : 1)
#define round(x) ((int)((x)*1000+_sign(x)*.5))/1000.0


static void push(int n, int inout)
{
	rect *R = bR[n];

	switch (n)
	{
	case -1:
		break;
	case 0:
	case 1:
	case 2:
		PushOrDoublePress(R, inout, our_flock_value == n);
		break;
	case 3:
	case 11:
		InvertInsides((text_button *) R);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		PushOrDoublePress(R, inout, our_view == n - 4);
		break;
	case 8:
	case 9:
	case 10:
		PushOrDoublePress(R, inout, our_accuracy_value == n - 8);
		break;
	case 12:
	case 13:
		PushOrDoublePress(R, inout, our_trace == n - 12);
		break;
	default:
		PushButton(R, inout);
		if (n == items - 1)
			ExtraHilite(R, inout);
	}
}





extern char *standard_button_texts[];

void lorenz_tweaker(void)
{
	rect tR;		/* the whole thing */
	int i;
	int row;
	int keyword;
	int centerx;
	int centery = sR.Ymax / 2;
	int width = sR.Xmax / 2;
	int height;
	int current_item;
	rect R, R1;
	int start_accuracy;

	static int lefters[] = {
		3, 0, 1, 2,
		7, 4, 5, 6,
		11, 8, 9, 10,
		13, 12,
		16, 14, 15
	};

	static int righters[] = {
		1, 2, 3, 0,
		5, 6, 7, 4,
		9, 10, 11, 8,
		13, 12,
		15, 16, 14
	};

	static int uppers[] = {
		14, 15, 15, 16,
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 10,
		12, 12, 13
	};

	static int downers[] = {
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 12, 13, 13,
		15, 16,
		0, 1, 3
	};

	static char *msg1 = "Flock Size: ";

	int imflag = 0;
	int ffflag = 0;
	int swflag = 0;
	int start_flock_value;
	int start_view;




	start_flock_value = -1;

	our_flock_value = start_flock_value;
	our_flocksize = fflock3ptr->n;
	our_accuracy = (int)(1/dt+.5);

	switch (axis)
	{
	case 'x':
		our_view = 0;
		break;
	case 'y':
		our_view = 1;
		break;
	case 'z':
		our_view = 2;
		break;
	case 'w':
		our_view = 3;
		break;
	}
	start_view = our_view;

	our_accuracy_value = -1;
	start_accuracy = our_accuracy_value;

	our_trace = tracetype;

	items = 0;
	height = 2 + FontHeight + 8 +
		5 * (3 * FontHeight / 2) + FontHeight / 2 + 4;
	width = 2 * sR.Xmax / 3;

	HideCursor();
	PushCursorPosition();

	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Lorenz Tweaks", BLACK);
	Centers(&tR, &centerx, &centery);
	PushMouseRectLimit(&tR);


	R.Xmin = tR.Xmin + StringWidth(msg1) + 12;
	R.Xmax = tR.Xmax - 12 * 8;
	row = R.Ymin = tR.Ymin + FontHeight + 8;
	R.Ymax = R.Ymin + FontHeight + 4;
	R1 = R;
	JString(msg1, R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, SML, flocksizeR, 3, our_flock_value);
	for (i = 0; i < 3; i++)
		bR[items++] = &flocksizeR[i];
	R.Xmin = flocksizeR[2].Xmax + 8;
	R.Xmax = tR.Xmax - 4;
	OffsetRect(&R, 0, -2);
	flocksizeTB.nR = R;
	PaintNumberBoxEntry(&flocksizeTB, our_flocksize, GS_UNSIGNED);
	bR[items++] = (rect *) & flocksizeTB;

	row += 3 * FontHeight / 2;
	R = R1;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = R.Ymin + FontHeight + 4;
	JString("View: ", R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, XYZF, viewR, 4, our_view);
	for (i = 0; i < 4; i++)
		bR[items++] = &viewR[i];

	row += 3 * FontHeight / 2;
	R = R1;
	R.Ymin = row;
	R.Ymax = R.Ymin + FontHeight + 4;
	JString("Accuracy: ", R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, LMH, accuracyR, 3, our_accuracy_value);
	for (i = 0; i < 3; i++)
		bR[items++] = &accuracyR[i];
	R.Xmin = accuracyR[2].Xmax + 8;
	R.Xmax = tR.Xmax - 4;
	OffsetRect(&R, 0, -2);
	accuracyTB.nR = R;
	PaintNumberBoxEntry(&accuracyTB, our_accuracy, GS_INTEGER);
	bR[items++] = (rect *) & accuracyTB;

	row += 3 * FontHeight / 2;
	R.Xmin = centerx;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = row + FontHeight + 4;
	JString("Ribbon Erase: ", R.Xmin - 2, R.Ymin + 2, BLACK, LIGHTGRAY, alignRight, alignTop);
	CreateRadioPanel(&R, offon, traceR, 2, our_trace);
	for (i = 0; i < 2; i++)
		bR[items++] = &traceR[i];

	row += 2 * FontHeight;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = R.Ymin + FontHeight + 4;
	CreateRadioPanel(&R, standard_button_texts, doitR, 3, -1);
	for (i = 0; i < 3; i++)
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

		keyword = 0;



		if (n)
		{
			if (button == swRight)
				keyword = 0x1b;

			else if (e.ASCII && e.ASCII != 0xe0)
				keyword = e.ASCII;
			else if (e.ScanCode != 0xff)
				keyword = e.ScanCode << 8;

			if (button == swLeft)
				keyword = 0x0d;
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
		if (keyword == XF10)
		{
			char tbuf[128];

			if (select_file("Save Screen", "*.gif", tbuf, "GIF"))
				SaveScreenGif(tbuf);
		}
		if (keyword == XF1)
		{
			keyword = 0x0d;
			current_item = items - 3;
		}

		if (keyword == 0x1b)
			current_item = items - 2;


		if (keyword == 0x0d)
		{
			if (current_item == items - 2)
			{
				keyword = 0x1b;
				break;
			}
			if (current_item == items - 1)
				break;

			if (current_item == items - 3)
			{
				helptext("attltwk.hlp");
			}

			i = our_flock_value;
			CheckRadioButtons(X, Y, flocksizeR, 3, &our_flock_value, SML);

			if (our_flock_value != i)
			{
				/* Update the flocksizeTB appropriately. */
				static int nfs[3] = {1, 27, 64};

				our_flocksize = nfs[our_flock_value];
				PaintNumberBoxEntry(&flocksizeTB, our_flocksize, GS_UNSIGNED);
			}


			CheckRadioButtons(X, Y, viewR, 4, &our_view, XYZF);
			i = our_accuracy_value;
			CheckRadioButtons(X, Y, accuracyR, 3, &our_accuracy_value, LMH);
			if (our_accuracy_value != i)
			{
				/* Update the flocksizeTB appropriately. */
				static double vs[3] = {1.0/0.03, 1.0/0.015, 1.0/0.01};

				our_accuracy = (int)(vs[our_accuracy_value] + .5);
				PaintNumberBoxEntry(&accuracyTB, our_accuracy, GS_INTEGER);
			}
			CheckRadioButtons(X, Y, traceR, 2, &our_trace, offon);

			if (current_item == 3)
			{
				double d = our_flocksize;

				if (GetNumber(&flocksizeTB, &d, GS_UNSIGNED, 1, 80))
				{
					our_flocksize = d;
					if (our_flock_value != -1)
					{
						PaintRadioButton(&flocksizeR[our_flock_value], false, false, SML[our_flock_value]);
						our_flock_value = -1;
					}
				}

				push(current_item, true);
			}
			if (current_item == 11)
			{
				double d = our_accuracy;

				if (GetNumber(&accuracyTB, &d, GS_INTEGER, 10,1000))
				{
					our_accuracy = (int)(d+.5);
					if (our_accuracy_value != -1)
					{
						PaintRadioButton(&accuracyR[our_accuracy_value], false, false, LMH[our_accuracy_value]);
						our_accuracy_value = -1;
					}
				}

				push(current_item, true);
			}
		}

		navigate(keyword, lefters, righters, uppers, downers, items, bR, &current_item);

		if (last_item != current_item)
		{
			push(last_item, false);
			push(current_item, true);
		}
		if (keyword == 0x1b)
			break;
	}

	if (keyword == 0x1b)
		i = 1;
	else
		i = 2;

	PaintRadioButton(&doitR[i], true, true, standard_button_texts[i]);
	DoublePress(&doitR[i], true, RED);

	if (keyword != 0x1b)
		ExtraHilite(&doitR[i], true);
	WaitForNothing();
	PaintRadioButton(&doitR[i], false, false, standard_button_texts[i]);
	DoublePress(&doitR[i], false, RED);

	if (keyword != 0x1b)
		ExtraHilite(&doitR[i], false);



	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	PopMouseRect();
	ShowCursor();

	if (keyword == 0x0d)
	{
      int acchanged = fabs(our_accuracy - 1.0/dt) > .0005;
      if (Stamping &&
		    (our_trace != tracetype ||
		     acchanged ||
		     start_accuracy != our_accuracy_value ||
		     our_flocksize != fflock3ptr->n ||
		     our_flock_value != start_flock_value ||
		     start_view != our_view))
		{
			preserve_data();
			slide_stamps();
		}

		if (tracetype != our_trace)
		{
			tracetype = our_trace;
			ribbonindex = 0;
			ribbonfull = 0;
		}

		if (acchanged)
		{
			dt = 1.0/our_accuracy;
			dt2 = dt / 2.0;
			dt6 = dt / 6.0;
			lorenzfreq = 1.0 / dt;
		}
		if (start_accuracy != our_accuracy_value)
		{
			switch (our_accuracy_value)
			{
			case 0:
				dt = 0.03;
				break;
			case 1:
				dt = 0.015;
				break;
			case 2:
				dt = .01;
				break;
			}
			dt2 = dt / 2.0;
			dt6 = dt / 6.0;
			lorenzfreq = 1.0 / dt;
		}

		if (our_flocksize != fflock3ptr->n)
		{
			double acenterx, acentery, acenterz;
			double xv, yv, zv;
			double xrange = fhix - flox;
			double yrange = fhiy - floy;
			double zrange = fhiz - floz;

			/*
			 * A bit tougher, we need to put in the right number
			 * of flocks. Kinda like dotflock, but with a
			 * particular number. Can we do this? Sure, but the
			 * dots are going to be in a tiny little circle, and
			 * there might be overlap. Can we avoid the overlap?
			 * Sure, keep track! Just get rid of the empty ones
			 * maybe
			 */
			imflag = 1;
			if (ribbonlength < 16)
				ribbonlength = 16;
			trihedroncount = 0;
			starting_trihedroncount = trihedroncount;
         long_iteration = 0L;

			/*
			 * We want a circle of a tiny radius. We add random
			 * flocks close within the area. We make them differ
			 * by .001. If one overlaps another, keep looking.
			 */


			acenterx = fcenterx + (random(100) - 50) / 1000.0 * (fhix - flox);
			acentery = fcentery + (random(100) - 50) / 1000.0 * (fhiy - floy);
			acenterz = fcenterz + (random(100) - 50) / 1000.0 * (fhiz - floz);



			for (i = 0; i < our_flocksize; i++)
			{
				while (1)
				{
					int j;

					xv = acenterx + xrange * (random(100) - 50) / 100000.0;
					yv = acentery + yrange * (random(100) - 50) / 100000.0;
					zv = acenterz + zrange * (random(100) - 50) / 100000.0;

					for (j = 0; j < i; j++)
					{
						if (xv == fflock3ptr->atom[j].x &&
						    yv == fflock3ptr->atom[j].y &&
						zv == fflock3ptr->atom[j].z)
							break;
					}
					if (i == j)
						break;
				}

				fflock3ptr->atom[i].x = xv;
				fflock3ptr->atom[i].y = yv;
				fflock3ptr->atom[i].z = zv;
				fflock3ptr->color[i] = (i % 13) + 2;
				fflock3ptr->releasetime[i] = long_iteration;
				saved_fflock3ptr->atom[i] = fflock3ptr->atom[i];
				saved_fflock3ptr->releasetime[i] = fflock3ptr->releasetime[i];
				saved_fflock3ptr->color[i] = fflock3ptr->color[i];
			}
			fflock3ptr->n = our_flocksize;
		}


		if (our_flock_value != start_flock_value)
		{
			int nfs;


         long_iteration = 0L;
			ribbonindex = 0;
			ribbonfull = 0;
			switch (our_flock_value)
			{
			case 0:
				nfs = 1;
				break;
			case 1:
				nfs = 3;
				break;
			case 2:
				nfs = 4;
				break;
			}

			dotflock(0.001, nfs);

			if (ribbonlength != 2)
				imflag = 1;
			if (ribbonlength < 16)
				ribbonlength = 16;
			trihedroncount = 0;
			starting_trihedroncount = trihedroncount;
			imflag = 1;
		}
		if (start_view != our_view)
		{
			axis = "xyzw"[our_view];
			if (axis == 'w')
			{
				lorenzflyflag = 1;
				viewspot = flyspot;
				viewtangent = flytangent;
				viewnormal = flynormal;
				viewbinormal = flybinormal;
			}
			else
				lorenzflyflag = 0;
			imflag = 1;
			trihedronon = 0;
		}


		if (imflag)
		{
			ribbonindex = 0;
			ribbonfull = 0;
			installmode();
		}
		if (ffflag)
			fillflock();
		if (swflag)
			setwindow(0);
      announceparms();
	}

}
