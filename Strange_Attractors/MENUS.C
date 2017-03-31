#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "attract.h"



rect mainbuttonR[6];		/* main button rects */
static char *mainbuttontext[] = {
	"F1 HELP ",
	"F2 File ",
	"F3 Types",
	"F4 Tweak",
	"F5 Opts ",
	0
};
static int mainkeys[] = {
	XF1, XF2, XF3, XF4, XF5, XALTX
};



void PaintQuitButton(int inout)
{
	rect R = mainbuttonR[5];
	int cx, cy;

	Centers(&R, &cx, &cy);
	PaintRadioButton(&R, inout, inout, "");
	BackColor(inout ? RED : DARKGRAY);
	PenColor(WHITE);
	TextAlign(alignCenter, alignTop);
	MoveTo(cx, R.Ymin + 2);
	DrawString("Alt-X");
	MoveTo(cx, R.Ymin + 2 + FontHeight);
	DrawString("to Exit");
	ExtraHilite(&R, inout);
}



void DrawButtons(void)
{
	int i;
	rect R;

	HideCursor();
	R.Xmin = 0;
	R.Xmax = minx - 1;
	R.Ymin = 0;
	R.Ymax = sR.Ymax;
	PenColor(BLUE);
	PaintRect(&R);
	PenColor(WHITE);
	FrameRect(&R);

	TextAlign(alignLeft, alignTop);

	PenColor(WHITE);

	for (i = 0; i < 5; i++)
	{

		R.Xmin = 4;
		R.Xmax = minx - 4;
		R.Ymin = i * (3 * FontHeight / 2) + 4;
		R.Ymax = R.Ymin + FontHeight + 4;

		mainbuttonR[i] = R;
		PaintRadioButton(&R, false, false, mainbuttontext[i]);
	}
	/* The QUIT button is bigger */
	R.Xmin = 4;
	R.Xmax = minx - 4;
	R.Ymin = i * (3 * FontHeight / 2) + 8;
	R.Ymax = R.Ymin + 2 * FontHeight + 4;
	mainbuttonR[i] = R;
	PaintQuitButton(false);

	/* Draw the frames for the stamps */
	for (i = 0; i < 3; i++)
	{
		PenColor(BLACK);
		PaintRect(&stampingR[i]);
		PenColor(WHITE);
		FrameRect(&stampingR[i]);
	}
	PenColor(BLACK);
	PaintRect(&stampR);
	ShowCursor();

}

void SetButton(i, inout)
{
	rect R = mainbuttonR[i];

	HideCursor();
	if (i < 5)
		PaintRadioButton(&R, inout, inout, mainbuttontext[i]);
	else
	{
		PaintQuitButton(inout);
		if (XYInRect(curx, cury, &R))
		{
			if (inout)
				PushOrDoublePress(&R, inout, true);
			else
			{
				PushButton(&R, true);
				ExtraHilite(&R, true);
			}
		}
	}

	ShowCursor();
}


int CheckMainMenu(int X, int Y)
{
	int i;
	static int stamping_keys[] = {XALT1, XALT2, XALT3};

	for (i = 0; i < 6; i++)
		if (XYInRect(X, Y, &mainbuttonR[i]))
			return mainkeys[i];

	for (i = 0; i < 3; i++)
		if (XYInRect(X, Y, &stampingR[i]))
			return stamping_keys[i];

	return 0;
}


int CheckRightclickMainMenu(int X, int Y)
{
	static int stamping_keys[] = {XALT4, XALT5, XALT6};
	int i;

	for (i = 0; i < 3; i++)
		if (XYInRect(X, Y, &stampingR[i]))
			return stamping_keys[i];

	return 0;
}

int current_main_item;
static void push(int item, int inout)
{
	if (item != -1)
	{
		if (item < 6)
		{
			PushButton(&mainbuttonR[item], inout);
			if (item == 5)
				ExtraHilite(&mainbuttonR[item], inout);
		}
	}
}

void MaybeHiliteSomething()
{
	int last_item = current_main_item;
	int i;

	current_main_item = -1;

	for (i = 0; i < 6; i++)
	{
		if (XYInRect(curx, cury, &mainbuttonR[i]))
		{
			current_main_item = i;
			break;
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (XYInRect(curx, cury, &stampingR[2 - i]))
		{
			current_main_item = i + 6;
			break;
		}
	}
	if (last_item != current_main_item)
	{
		push(last_item, false);
		push(current_main_item, true);
	}
}

void LockToClosestUpdown(void)
{
	/* This only gets called if we are already somewhere on the menu pad. */
	int bestdist = 9999;
	int closest = -1;

	int cys[20];
	int i;
	int x;
	int last_item = current_main_item;

	if (current_main_item == 0 && curyinc < 0)
	{
		current_main_item = 8;
		move_to_corner(&stampingR[0]);
	}
	else if (current_main_item == 8 && curyinc > 0)
	{
		current_main_item = 0;
		move_to_corner(&mainbuttonR[0]);
	}
	else
	{

		for (i = 0; i < 6; i++)
			Centers(&mainbuttonR[i], &x, &cys[i]);
		for (i = 0; i < 3; i++)
			Centers(&stampingR[2 - i], &x, &cys[i + 6]);

		for (i = 0; i < 9; i++)
		{
			int distance = abs(cury - cys[i]);

			if (distance < bestdist)
			{
				if ((curyinc < 0 && cury > cys[i]) ||
				    (curyinc > 0 && cury < cys[i]))
				{
					bestdist = distance;
					closest = i;
				}
			}
		}

		if (closest != -1)
		{
			current_main_item = closest;
			if (closest < 6)
				move_to_corner(&mainbuttonR[closest]);
			else
				move_to_corner(&stampingR[2 - (current_main_item - 6)]);
		}
	}
	if (last_item != current_main_item)
	{
		push(last_item, false);
		push(current_main_item, true);
	}
}

void LockToClosestLeft(void)
{

	int cys[20];
	int i;
	int x;
	int bestdist = 9999;
	int closest = -1;
	int last_item = current_main_item;

	for (i = 0; i < 6; i++)
		Centers(&mainbuttonR[i], &x, &cys[i]);
	for (i = 0; i < 3; i++)
		Centers(&stampingR[2 - i], &x, &cys[i + 6]);


	for (i = 0; i < 9; i++)
	{
		int distance = abs(cury - cys[i]);

		if (distance < bestdist)
		{
			bestdist = distance;
			closest = i;
		}
	}
	if (closest != -1)
	{
		current_main_item = closest;
		if (closest < 6)
			move_to_corner(&mainbuttonR[closest]);
		else
			move_to_corner(&stampingR[2 - (current_main_item - 6)]);
	}
	if (last_item != current_main_item)
	{
		push(last_item, false);
		push(current_main_item, true);
	}
}
