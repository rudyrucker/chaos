#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "attract.h"

static char *types[] = {
	"Lorenz",
	"Yorke",
	"Hnon",
	"Hnon Horseshoe",
	"Logistic Map",
	"Logistic Pulse",
	"Logistic Hump",
};

static rect typeR[7];
static rect doitR[3];
static char *doitmsgs[] = {"F1 for HELP", "ESC to Cancel", "ACCEPT"};
static rect *bR[20];
static int ourtype;
static int items;
static void push(int n, int inout)
{
	switch (n)
	{
		case -1:
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		PushOrDoublePress(bR[n], inout, n == ourtype);
		break;
	case 9:
		ExtraHilite(bR[n], inout);
		/* deliberate flow */
	default:
		PushButton(bR[n], inout);
		break;
	}
}

void do_types_menu(void)
{

	rect tR;

	int height = 3 * (3 * FontHeight / 2) + 2 * FontHeight + FontHeight + 12;
	int width = 2 * sR.Xmax / 3;
	int row;
	int i;
	rect R;
	int starttype;
	int current_item;

	static int lefters[] = {
		1, 0,
		3, 2,
		6, 4, 5,
		9, 7, 8
	};

	static int righters[] = {
		1, 0,
		3, 2,
		5, 6, 4,
		8, 9, 7
	};

	static int uppers[] = {
		7, 9,
		0, 1,
		2, 2, 3,
		4, 5, 6
	};

	static int downers[] = {
		2, 3,
		4, 6,
		7, 8, 9,
		0, 0, 1
	};



	items = 0;

	HideCursor();
	PushCursorPosition();
	BasicCenteredBox(&tR, width, height, LIGHTGRAY, "Types Menu", BLACK);
	PushMouseRectLimit(&tR);

	switch (dimension)
	{
	case LORENZ:
		ourtype = 0;
		break;
	case YORKE:
		ourtype = 1;
		break;
	case HENON:
		ourtype = (fancyflag) ? 2 : 3;
		break;
	case LOGISTIC:
		ourtype = 4 + tracetype;
		break;
	}
	starttype = ourtype;

	row = tR.Ymin + FontHeight + 12;
	R.Xmin = tR.Xmin + 4;
	R.Xmax = tR.Xmax - 4;
	R.Ymin = row;
	R.Ymax = R.Ymin + FontHeight + 4;

	CreateRadioPanel(&R, types, typeR, 2, ourtype);
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	row = R.Ymin;
	CreateRadioPanel(&R, types + 2, typeR + 2, 2, ourtype - 2);
	OffsetRect(&R, 0, 3 * FontHeight / 2);
	row = R.Ymin;
	CreateRadioPanel(&R, types + 4, typeR + 4, 3, ourtype - 4);

	for (i = 0; i < 7; i++)
		bR[items++] = &typeR[i];

	OffsetRect(&R, 0, 2 * FontHeight);
	row = R.Ymin;
	CreateRadioPanel(&R, doitmsgs, doitR, 3, -1);
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
		int keyword = 0;
		int last_item = current_item;

		if (n)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				keyword = e.ASCII;
			else if (e.ScanCode != 0xff)
				keyword = e.ScanCode << 8;

			if (button == swLeft)
				keyword = 0x0d;

			if (button == swRight)
				keyword = 0x1b;
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
			current_item = items - 2;
			keyword = 0x0d;
		}

		if (keyword == XF1)
		{
			current_item = items - 3;
			keyword = 0x0d;
		}

		navigate(keyword, lefters, righters, uppers, downers, items, bR, &current_item);


		if (last_item != current_item)
		{
			push(last_item, false);
			push(current_item, true);
		}

		if (keyword == 0x0d)
		{
			if (current_item < 7)
				CheckRadioButtons(X, Y, typeR, 7, &ourtype, types);
			else if (current_item == items - 3)
				helptext("attype.hlp");
			else if (current_item == items - 2)
				break;
			else if (current_item == items - 1)
				break;
		}


	}

	PaintRadioButton(bR[current_item], true, true, doitmsgs[current_item - 7]);
	if (current_item == items - 1)
		ExtraHilite(bR[current_item], true);
	WaitForNothing();
	PaintRadioButton(bR[current_item], false, false, doitmsgs[current_item - 7]);
	if (current_item == items - 1)
		ExtraHilite(bR[current_item], false);

	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	PopMouseRect();
	ShowCursor();

	if (current_item == items - 1)
	{
		if (starttype != ourtype)
		{
			int imflag = 1;
			int swflag = 1;
			int ffflag = 1;

			iteration = 0;

			if (Stamping)
			{
				preserve_data();
				slide_stamps();
			}

			switch (ourtype)
			{
			case 0:/* changing to LORENZ */
				dimension = LORENZ;
				flocktype = 1;
				ribbonlength = 64;
				tracetype = 1;
				break;
			case 1:/* changing to YORKE */
				dimension = YORKE;
				ribbonlength = 4;
				fancyflag = 1;
				tracetype = 0;
				flocktype = 1;
				break;
			case 2:
				dimension = HENON;
				fancyflag = 1;
				ribbonlength = 4;
				tracetype = 0;
				flocktype = 2;
				break;
			case 3:/* changing to HORSESHOE NEBULA */
				dimension = HENON;
				fancyflag = 0;
				ribbonlength = 4;
				tracetype = 0;
				flocktype = 2;
				break;
			case 4:
				dimension = LOGISTIC;
				cursorshape = 1;
				logstartval = LOGSTART;
				logisticlaunchflag = 1;
				tracetype = 0;
				break;
			case 5:
				dimension = LOGISTIC;
				cursorshape = 1;
				logstartval = LOGSTART;
				fx = STARTBEAT;
				pushview(&logisticview);
				delayfactor = 30;
				if (fancyflag != 0)
					delayfactor = 10;
				logisticlaunchflag = 1;
				tracetype = 1;
				break;
			case 6:
				dimension = LOGISTIC;
				cursorshape = 1;
				logstartval = LOGSTART;
				fx = STARTMOTION;
				logisticlaunchflag = 1;
				tracetype = 2;
				break;
			}

			if (imflag)
				installmode();
			if (swflag)
				setwindow(1);
			if (ffflag)
			{
				fillflock();
				saved_fflock3ptr->n = fflock3ptr->n = 6;
			}
		}
	}



}
