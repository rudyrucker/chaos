#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "game.h"


int current_main_item = 0;

rect mainbuttonR[10];		/* main button rects */
rect *mainR[60];
static char *mainbuttontext[] = {
	"F1 HELP ",
	"F2 File ",
	"F3 Next ",
	"F4 Tweak",
	"F5 Opts ",
	0
};
static int mainkeys[] = {
	XF1, XF2, XF3, XF4, XF5, XALTX
};

void invert_item_round(rect * R)
{
	HideCursor();
	RasterOp(zXORz);
	PenSize(2, 2);
	PenColor(WHITE);
	FrameRoundRect(R, 8, 8);
	PenSize(1, 1);
	RasterOp(zREPz);
	ShowCursor();
}

void invert_item_rect(rect * R)
{
	HideCursor();
	RasterOp(zXORz);
	PenSize(2, 2);
	PenColor(WHITE);
	FrameRect(R);
	PenSize(1, 1);
	RasterOp(zREPz);
	ShowCursor();
}

void invert_item_hilited(rect * R)
{
	rect RR = *R;

	HideCursor();
	RasterOp(zXORz);
	PenColor(WHITE);
	FrameRect(&RR);
	InsetRect(&RR, -1, -1);
	FrameRect(&RR);
	RasterOp(zREPz);
	ShowCursor();
}

void invert_main_item(int n)
{
	if (n == -1)
		return;

	if (n == LEFTARROWBOX)
		invert_item_rect(&left_arrow);
	else if (n == RIGHTARROWBOX)
		invert_item_rect(&right_arrow);
	else if (n > LEFTARROWBOX && n < RIGHTARROWBOX)
		invert_item_rect(&bottom_rects[n - LEFTARROWBOX - 1]);
	else if (n >= TWEAKBASE)
		invert_tweak_item(n - TWEAKBASE);
	else if (n == ALTXEXIT)
		invert_item_hilited(&mainbuttonR[n]);
	else if (n <= 5)
		invert_item_rect(mainR[n]);
}

void corner_main(void)
{
	if (current_main_item == LEFTARROWBOX)
		move_to_corner(&left_arrow);
	else if (current_main_item == RIGHTARROWBOX)
		move_to_corner(&right_arrow);
	else if (current_main_item > LEFTARROWBOX && current_main_item < RIGHTARROWBOX)
		move_to_corner(&bottom_rects[current_main_item - LEFTARROWBOX - 1]);
	else if (current_main_item >= TWEAKBASE)
		move_to_corner(&tweakRects[current_main_item - TWEAKBASE].nR);
	else
		move_to_corner(mainR[current_main_item]);
}

void PaintQuitButton(rect * R, int depress)
{
	int centerx, centery;

	Centers(R, &centerx, &centery);
	HideCursor();
	PenColor(depress ? RED : BUTTONBACK);
	PaintRect(R);
	PushButton(R, depress);
	ExtraHilite(R, depress);
	PenColor(MENUTEXT);
	BackColor(depress ? RED : BUTTONBACK);
	TextAlign(alignCenter, alignTop);
	MoveTo(centerx, R->Ymin + 1);
	DrawString("Alt-X");
	MoveTo(centerx, R->Ymin + 1 + FontHeight);
	DrawString("to Exit");
	ShowCursor();
}

void PressButton(int n, int inout)
{
	PaintRadioButton(&mainbuttonR[n], inout, inout, mainbuttontext[n]);
}



void DrawButtons(void)
{
	int i;
	rect R;
	TextAlign(alignLeft, alignTop);

	PenColor(WHITE);

	PenColor(MENUBACK);
	R.Xmin = 0;
	R.Xmax = minx - 1;
	R.Ymin = 0;
	R.Ymax = sR.Ymax;
	//7 * (FontHeight + 8);
	PaintRect(&R);


	for (i = 0; i < 5; i++)
	{

		R.Xmin = 4;
		R.Xmax = 74;
		R.Ymin = 4 + i * (FontHeight + BUTTONHEIGHT);
		R.Ymax = R.Ymin + FontHeight + 4;

		mainbuttonR[i] = R;
      mainR[i] = &mainbuttonR[i];

		PaintRadioButton(&R, false, false, mainbuttontext[i]);
	}
	/* The QUIT button is bigger */
	R.Xmin = 4;
	R.Xmax = 74;
	R.Ymin = 4 + i * (FontHeight + BUTTONHEIGHT);
	R.Ymax = R.Ymin + 2 * FontHeight + 4;
	mainbuttonR[i] = R;
   mainR[i] = &mainbuttonR[i];

	PaintQuitButton(&R, false);


	invert_main_item(current_main_item);


	/* and draw the empty xobs */
	InitializeStamps();
	for (i = 0; i < 4; i++)
	{
		PenColor(BLACK);
		PaintRect(&stamp_rects[i]);
		PenColor(WHITE);
		FrameRect(&stamp_rects[i]);
      mainR[(3-i)+6] = &stamp_rects[i];
	}

}

void msg(char *what)
{
	MoveTo(0, sR.Ymax - FontHeight);
	TextAlign(alignLeft, alignTop);
	PenColor(WHITE);
	BackColor(BLACK);
	DrawString(what);
}

int CheckMainMenu(int X, int Y)
{
	int i;

	for (i = 0; i < 6; i++)
		if (XYInRect(X, Y, &mainbuttonR[i]))
			return mainkeys[i];


	return 0;
}

void CheckMainDrags(event * e)
{
	int X = e->CursorX;
	int Y = e->CursorY;
	int i;

	for (i = 0; i < 10; i++)
		if (XYInRect(X, Y, mainR[i]))
			current_main_item = i;

}

static int notweak_downers[] = {
   1,2,3,4,5,6,7,8,9,0
   };

static int notweak_uppers[] = {
   9,0,1,2,3,4,5,6,7,8
   };

static int tweak_downers[48] = {
   1,2,3,4,5,6,7,8,9,0,       /* 10 main buttons */
   13,14,15,                  /* 9 rows of 3 */
   16,17,18,
   19,20,21,
   22,23,24,
   25,26,27,
   28,29,30,
   31,32,33,
   34,35,36,
   37,37,37,
   38,                        /* Display mode */
   47,47,                     /* Add/kill */                        
   41,                        /* Left arrow */
   41,42,43,44,45,46,         /* Bottom boxes */
   46                         /* Right arrow */
   };

static int tweak_uppers[48] = {
   9,0,1,2,3,4,5,6,7,8,
   45,45,46,
   10,11,12,
   13,14,15,
   16,17,18,
   19,20,21,
   22,23,24,
   25,26,27,
   28,29,30,
   31,32,33,
   34,
   37,37,
   38,
   40,40,40,47,47,47,
   38
   };

static int tweak_righters[48] = {
   10,13,16,19,22,25,31,40,41,41,
   11,12,0,
   14,15,1,
   17,18,2,
   20,21,3,
   23,24,4,
   26,27,5,
   29,30,5,
   32,33,6,
   35,36,6,
   7,
   39,7,
   47,
   42,43,44,45,46,9,
   9
   };

static int tweak_lefters[48] = {
   12,15,18,21,24,27,33,37,44,46,
   0,10,11,
   1,13,14,
   2,16,17,
   3,19,20,
   4,22,23,
   5,25,26,
   5,28,29,
   6,31,32,
   6,34,35,
   7,
   7,38,
   7,
   9,41,42,43,44,45,
   40
   };




int closest_left(void)
{
   int closest = 9999;
   int best;
   int i;
   /* Always snaps to a main menu thing */

   for(i=0;i<10;i++)
   {
      int dist = abs(cury - mainR[i]->Ymin);

      if (dist < closest)
      {
         closest = dist;
         best = i;
      }
   }

   return best;
}

int closest_right(void)
{
   int closest = 9999;
   int best;
   int dist;
   int i;

   if (!tweaking)
      return -1;

   for(i=10;i<=37;i+=3)
   {
      dist = abs(cury - mainR[i]->Ymin);
      if (dist < closest)
      {
         closest = dist;
         best = i;
      }
   }

   dist = abs(cury - mainR[38]->Ymin);
   if (dist < closest)
   {
      closest = dist;
      best = 38;
   }

   return best;
}

int closest_down(void)
{
   int closest = 9999;
   int best;
   int dist;
   int i;

   if (!tweaking)
      return -1;

   if (fBptr->n >= 6)
   {
      closest = abs(curx - mainR[40]->Xmin);
      best = 40;

      dist = abs(curx - mainR[47]->Xmin);
      if (dist < closest)
      {
         best = 47;
         closest = dist;
      }
   }

   for(i=0;i<fBptr->n;i++)
   {
      dist = abs(curx - mainR[41+i]->Xmin);
      if (dist < closest)
      {
         best = 41 + i;
         closest = dist;
      }
   }

   return best;
}


int CheckMenuKeys(int keyword)
{
	int last_main_item = current_main_item;

	/* I'm superceding a LOT of shit here */
	if (keyword == 0x0d)
	{
		if (current_main_item <= ALTXEXIT)
		{
			static int retvals[] = {XF1, XF2, XF3, XF4, XF5, XALTX};

			keyword = retvals[current_main_item];
         return keyword;
		}
      else if (current_main_item <= ALTXEXIT + 4)
      {
         restore_stamp(3 - (current_main_item - (ALTXEXIT+1)));
         return 0;
      }
		if (tweaking && current_main_item != -1)
		{
			int click = (current_main_item - TWEAKBASE)/3;
         int col = (current_main_item - TWEAKBASE) % 3;

         if (click == 9)
            click += (current_main_item - TWEAKBASE) % 3;

         if (click == 0)
         {
            int doit = false;
            switch(col)
            {
            case 0:
               bump_edmap();
               awaken();
               break;
            case 1:
               process_minus_key(click);
               doit = true;
               break;
            case 2:
               process_plus_key(click);
               doit = true;
               break;
            }
            if (doit)
               awaken();
         }
         else if (click < 9)
         {
            switch(col)
            {
            case 0:
      			process_main_click(click);
               break;
            case 1:
               process_minus_key(click);
               break;
            case 2:
               process_plus_key(click);
               break;
            }
            LimitMouseRect(&sR);
            if (col)
            {
			      ConvertParamsBack(edmap);	/* make sure they match? */
			      reawaken(click);
               if (click == 7)
         			ShowWeights();

            }
         

            
         }
			if (current_main_item == LEFTARROWBOX)
				ShiftLeft();
			else if (current_main_item == RIGHTARROWBOX)
				ShiftRight();
         else if (current_main_item > LEFTARROWBOX &&
             current_main_item < RIGHTARROWBOX)
         {
            edmap = FirstMap + (current_main_item - (LEFTARROWBOX + 1));
            awaken();
         }
         else if (current_main_item == LEFTARROWBOX - 2)
            tw_add_new_barnmap();
      	else if (current_main_item == LEFTARROWBOX - 1)
		      zap_a_map();
         else if (current_main_item == LEFTARROWBOX - 3)
            change_display_mode();


			keyword = 0;
		}
	}
	else if ((keyword == '-' || keyword == '+') && tweaking)
	{
		int click = (current_main_item - TWEAKBASE)/3;

      if (click != DISPLAYMODE && click != ADDMAP && click != KILLMAP)
      {

		   if (keyword == '-')
			   process_minus_key(click);
		   else
			   process_plus_key(click);
		   if (click == CURMAP)
			   awaken();
		   else if (click == INCREMENT)
			   UpdateMapParam(click);
		   else if (click >= 0)
         {
			   reawaken(click);
            if (((current_main_item - TWEAKBASE) % 3) == 0)
               invert_tweak_item(current_main_item - TWEAKBASE);
         }

		   if (click == WEIGHT)
			   ShowValues();
		   if (click != INCREMENT && click != DISPLAYMODE && click != CURMAP)
			   ShowOneValues(edmap);


		   keyword = 0;
      }
	}
	else if (current_main_item == -1)
   {
      int mv = false;
      switch(keyword)
      {
      case XLARROW:
      case XHOME:
      case XEND:
         current_main_item = closest_left();
         mv = true;
         break;
      case XRARROW:
      case XPGUP:
      case XPGDN:
         current_main_item = closest_right();
         mv = true;
         break;
      case XDARROW:
         current_main_item = closest_down();
         mv = true;
         break;
      }

      if (mv && current_main_item != -1)
         move_to_corner(mainR[current_main_item]);
   }
   else
   {
      int *l,*u,*r,*d;

      if (tweaking)
      {
         l = tweak_lefters;
         r = tweak_righters;
         d = tweak_downers;
         u = tweak_uppers;
      }
      else
      {
         l = 0L;
         r = 0L;
         d = notweak_downers;
         u = notweak_uppers;
      }

      navigate(keyword,l,r,u,d,(tweaking) ? 48 : 10,mainR,&current_main_item);

      if (current_main_item == LEFTARROWBOX)
      {
         if (fBptr->n <= 6)
         {
            switch(keyword)
            {
            case XLARROW:
               current_main_item = 8;
               break;
            case XRARROW:
               current_main_item = 41;
               break;
            case XUARROW:
               current_main_item = 38;
               break;
            }
            move_to_corner(mainR[current_main_item]);
         }
      }
      else if (current_main_item == RIGHTARROWBOX)
      {
         if (fBptr->n <= 6)
         {
            switch(keyword)
            {
            case XLARROW:
               current_main_item = 39;
               break;
            case XRARROW:
               current_main_item = 41;
               break;
            case XUARROW:
               current_main_item = 38;
               break;
            case XDARROW:
               current_main_item = 46;
            }
            move_to_corner(mainR[current_main_item]);
         }
      }

      /* now see if it points to an empty box */
      if (current_main_item > LEFTARROWBOX && current_main_item < RIGHTARROWBOX &&
         fBptr->n < 6 && current_main_item > LEFTARROWBOX + fBptr->n)
      {
         switch(keyword)
         {
         case XRARROW:
            current_main_item = 8;
            break;
         case XLARROW:
         case XDARROW:
            current_main_item = min(current_main_item,LEFTARROWBOX + fBptr->n);
            break;
         }
         move_to_corner(mainR[current_main_item]);
      }
   }   

	if (current_main_item != last_main_item)
	{
		invert_main_item(last_main_item);
		invert_main_item(current_main_item);
	}





	return keyword;
}

#pragma argsused
int CheckRightclickMainMenu(int X, int Y)
{

	return 0;
}


int CheckLeftClicks(event * e)
{
	int k = 0;
	int X = e->CursorX;
	int Y = e->CursorY;
	int i;



	if (((e->State >> 8) & 7) == swLeft)
		k = CheckMainMenu(X, Y);

	if (!k)
		k = CheckTweakClicks(e);

	if (!k)
	{
		for (i = 0; i < 4; i++)
		{
			if (XYInRect(X, Y, &stamp_rects[i]))
			{
				k = XALT1 + (i << 8);
				break;
			}
		}
	}
	return k;
}


void CheckDrags(event * e)
{
	/*
	 * See if we've dragged onto either a new main menu item, or
	 * something else completely different.
	 */
	int last_main_item = current_main_item;

	current_main_item = -1;

	CheckMainDrags(e);

	if (tweaking)
		CheckTweakDrags(e);
	if (current_main_item != last_main_item)
	{
		invert_main_item(last_main_item);
		invert_main_item(current_main_item);
	}
}




int CheckRightClicks(event * e)
{
   int i;
   int X = e->CursorX;
   int Y = e->CursorY;

	for (i = 0; i < 4; i++)
	{
		if (XYInRect(X, Y, &stamp_rects[i]))
			return XALTT;
	}

	return 0;
}

/* Here's where we build our special cursor. Goombah! Hooray! */
typedef enum
{
	ARROW, TRIANGLE
} CT;
static CT CursorType;

static CT CursorTypeStack[20];
static int CTSptr = 0;

void PushCursorType(void)
{
	if (CTSptr < 19)
		CursorTypeStack[CTSptr++] = CursorType;
}

void PopCursorType(void)
{
	if (CTSptr)
		CursorType = CursorTypeStack[--CTSptr];
	if (CursorType == ARROW)
		ArrowCursor();
	else
		TriangleCursor();
}


void InitializeCursors(void)
{
	/* A triangle cursor. Gee! */
	/*
	 * Outside:
	 * 
	 * 1000 0000 0000 0000 1100 0000 0000 0000 1110 0000 0000 0000 1111 0000
	 * 0000 0000
	 * 
	 * 1101 1000 0000 0000 1100 1100 0000 0000 1100 0110 0000 0000 1100 0011
	 * 0000 0000
	 * 
	 * 1100 0001 1000 0000 1100 0000 1100 0000 1100 0000 0110 0000 1111 1111
	 * 1111 0000
	 * 
	 * 1111 1111 1111 1000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
	 * 0000 0000
	 * 
	 * 
	 * 
	 * Inside: 1000 0000 0000 0000 1100 0000 0000 0000 1110 0000 0000 0000
	 * 1111 0000 0000 0000
	 * 
	 * 1111 1000 0000 0000 1111 1100 0000 0000 1111 1110 0000 0000 1111 1111
	 * 0000 0000
	 * 
	 * 1111 1111 1000 0000 1111 1111 1100 0000 1111 1111 1110 0000 1111 1111
	 * 1111 0000
	 * 
	 * 1111 1111 1111 1000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
	 * 0000 0000
	 * 
	 */
	static cursor triangle_cursormask = {
		16, 16, 0, 2, 1, 1,
		0x80, 0x00,
		0xc0, 0x00,
		0xe0, 0x00,
		0xf0, 0x00,
		0xd8, 0x00,
		0xcc, 0x00,
		0xc6, 0x00,
		0xc3, 0x00,
		0xc1, 0x80,
		0xc0, 0xc0,
		0xc0, 0x60,
		0xff, 0xf0,
		0xff, 0xf8,
		0, 0, 0, 0, 0, 0
	};
	static cursor triangle_screenmask = {
		16, 16, 0, 2, 1, 1,
		0x80, 0x00,
		0xc0, 0x00,
		0xe0, 0x00,
		0xf0, 0x00,
		0xf8, 0x00,
		0xfc, 0x00,
		0xfe, 0x00,
		0xff, 0x00,
		0xff, 0x80,
		0xff, 0xc0,
		0xff, 0xe0,
		0xff, 0xf0,
		0xff, 0xf8,
		0, 0, 0, 0, 0, 0
	};


	//triangle_screenmask = triangle_cursormask;
	DefineCursor(7, 0, 0, &triangle_screenmask, &triangle_cursormask);
	CursorType = ARROW;
}


void TriangleCursor(void)
{
	static mapArray cmap1 = {7, 7, 7, 7, 7, 7, 7, 7};

	CursorMap(cmap1);
	CursorType = TRIANGLE;
}

void ArrowCursor(void)
{
	static mapArray cmap = {0, 0, 0, 0, 0, 0, 0, 0};

	CursorMap(cmap);
	CursorType = ARROW;
}
