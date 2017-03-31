#include "mag.h"
int CurrentControlPage;
rect controlRect;
rect displayRect;
rect menuRect;
int current_main_item;

int main_items;
rect *mainR[60];
rect buttonR[4];
rect QuitButtonR;
rect randomize_chargeR;
rect randomize_posR;
rect chargeR;

static char *randomposmsg = "Random Positions";
static char *randomchargemsg = "Random Charges";
static int samecharges=true;
static rect runstopstepR[3];
static char *RSS[] = {"Run","Stop","Step"};



numbertool ChargeTool, CapRadTool, RadiusTool, PullTool, FreqTool, FricTool;

struct
{
	numbertool *t;
	char *text;
} tools[] =

{
	&ChargeTool, "Charge",
	&CapRadTool, "Cap",
	&RadiusTool, "Rad",
	&PullTool, "Pull",
	&FreqTool, "Freq",
	&FricTool, "Fric"
};

static void drawone(numbertool *t,double d)
{
   t->value = d;
   RasterOp(zREPz);
	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}


void DrawControlPage(void)
{
}
void drawCenterpullButton(void)
{
   drawone(&PullTool,centerpull);
}
void drawFrictionButton(void)
{
   drawone(&FricTool,friction);
}
void drawFrequencyButton(void)
{
   drawone(&FreqTool,freq);
}
static char *chargemsg = "F5 Charges ";
static enum {SAME,DIFFERENT} charge_thing = SAME;
void set_samecharges(void)
{
   int i;
	long lastcharge;
   samecharges = true;
   if (M.n)
   {
      lastcharge = M.magnet[0].charge;
      for(i=1;i<M.n;i++)
      {
         if (lastcharge != M.magnet[i].charge)
         {
            samecharges = false;
            break;
         }
      }
   }
   if (samecharges)
      chargeunit = M.magnet[0].charge;
}

void drawChargeButton(void)
{
   /* This one is a bit sneaky. We might not have a single charge... */
   int i;

   HideCursor();
   set_samecharges();
   if (samecharges)
   {
      if (charge_thing == DIFFERENT)
      {
         RasterOp(zREPz);
         PenColor(BLUE);
         PaintRect(&chargeR);
         tools[0].t->value = chargeunit;
   		CreateNumberToolTitle(tools[0].t, tools[0].text,WHITE,BLUE);
         mainR[5] = &tools[0].t->TB.nR;
         mainR[6] = &tools[0].t->mR;
         mainR[7] = &tools[0].t->pR;
      }
      else
         drawone(&ChargeTool,chargeunit);
      charge_thing = SAME;
   }
   else
   {
      if (charge_thing == SAME)
      {
         rect R = chargeR;
         RasterOp(zREPz);
         InsetRect(&R,-2,0);
         PenColor(BLUE);
         PaintRect(&R);
         PaintRadioButton(&chargeR,false,false,chargemsg);
         for(i=0;i<3;i++)
            mainR[i+5] = &chargeR;
      }
      charge_thing = DIFFERENT;
   }
   ShowCursor();
}
void drawRadiusButton(void)
{
   drawone(&RadiusTool,magnetradius);
}
void drawCaptureButton(void)
{
   drawone(&CapRadTool,xsection);
}
void RandomizeCharges(void)
{
   int i;


   for (i = 0; i < M.n; i++)
   {
	   M.magnet[i].charge = random(10) + 1;
	   if (random(100) > 50)
		   M.magnet[i].charge *= -1;
   }
   showclub(clubx,cluby,0);
   installmode();
   newholeflag = false;
   setuphole(holenumber);
   samecharges = false;
   drawChargeButton();
}
void RandomizePositions(void)
{
   int i;

   for (i = 0; i < M.n; i++)
   {
	   M.magnet[i].x = maxx / 8 + random(6 * maxx / 8);
	   M.magnet[i].y = maxy / 8 + random(6 * maxy / 8);
   }
   showclub(clubx,cluby,0);
   installmode();
   setuphole(holenumber);
}


static char *msgs[] = {
	"F1 for Help",
	"F2 File    ",
	"F3 Layout  ",
	"F4 Options ",
	NULL
};

void PaintMainButton(int i,int inout)
{
   if (i < 4)
      PaintRadioButton(mainR[i], inout, inout, msgs[i]);
   else if (i == 4 && !samecharges)
      PaintRadioButton(&chargeR,inout,inout,chargemsg);
   else if (i == 'i')
      PaintRadioButton(&randomize_chargeR,inout,inout,randomchargemsg);
   else if (i == 'h')
      PaintRadioButton(&randomize_posR,inout,inout,randomposmsg);
}

void PaintQuitButton(int inout)
{
	rect R = QuitButtonR;
	int cx, cy;

	HideCursor();
   RasterOp(zREPz);
	Centers(&R, &cx, &cy);
	PaintRadioButton(&R, inout, inout, "");
	TextAlign(alignCenter, alignTop);
	PenColor(WHITE);
	BackColor(inout ? RED : DARKGRAY);
	MoveTo(cx, R.Ymin + 4);
	DrawString("Alt-X");
	MoveTo(cx, R.Ymin + FontHeight + 4);
	DrawString("to Exit");
	PushButton(&R, inout);
	ExtraHilite(&R, inout);
	if (inout)
		DoublePress(&R, true, RED);
	ShowCursor();
}

void setupcontrols(void)
{
	rect R;
	int i;

   RasterOp(zREPz);

	ChargeTool.type = GS_INTEGER;
	ChargeTool.value = chargeunit;
   ChargeTool.lo = -500;
   ChargeTool.hi = 500;

   current_main_item = -1;
	CapRadTool.type = GS_UNSIGNED;
	CapRadTool.value = xsection;
   CapRadTool.lo = 0;
   CapRadTool.hi = 20;


	RadiusTool.type = GS_UNSIGNED;
	RadiusTool.value = magnetradius;
   RadiusTool.lo = 1;
   RadiusTool.hi = 60;

	PullTool.type = GS_INTEGER;
	PullTool.value = centerpull;
   PullTool.lo = -500;
   PullTool.hi = 500;

	FreqTool.type = GS_UNSIGNED;
	FreqTool.value = freq;
   FreqTool.lo = 2;
   FreqTool.hi = 10000;

	FricTool.type = GS_FLOAT;
	FricTool.value = friction;
	FricTool.p1 = 4;
	FricTool.p2 = 2;
   FricTool.lo = 0;
   FricTool.hi = 500;

	R.Xmin = sR.Xmax / 4;
	R.Ymin = 0;
	R.Xmax = sR.Xmax;
	R.Ymax = sR.Ymax;

	displayRect = R;
	R.Xmin = 0;
	R.Ymin = 0;
	R.Xmax = sR.Xmax / 4 - 9;
	R.Ymax = sR.Ymax;

	controlRect = menuRect = R;

	PenColor(BLUE);
	PaintRect(&R);
	PenColor(WHITE);
	FrameRect(&R);



	R.Xmin = controlRect.Xmin + 4;
	R.Xmax = controlRect.Xmax - 4;
	R.Ymin = controlRect.Ymin + 4;
	R.Ymax = R.Ymin + FontHeight + 4;
	main_items = 0;

	for (i = 0; i < 4; i++)
	{
		buttonR[i] = R;
		mainR[main_items++] = &buttonR[i];

		PaintRadioButton(&R, false, false, msgs[i]);
		OffsetRect(&R, 0, 3 * FontHeight / 2);
	}

	R.Ymax += FontHeight;
	QuitButtonR = R;
	PaintQuitButton(false);
	mainR[main_items++] = &QuitButtonR;

	R.Ymin = R.Ymax + FontHeight;
	R.Ymax = R.Ymin + FontHeight + 4;
	R.Xmin = StringWidth("Charge") + 8;

	for (i = 0; i < 6; i++)
	{
		tools[i].t->tR = R;
		CreateNumberToolTitle(tools[i].t, tools[i].text,WHITE,BLUE);
		mainR[main_items++] = &tools[i].t->TB.nR;
		mainR[main_items++] = &tools[i].t->mR;
		mainR[main_items++] = &tools[i].t->pR;


      /* The charge rect is sneaky. */
      if (i == 0)
      {
         chargeR = R;
         chargeR.Xmin = QuitButtonR.Xmin;
         charge_thing = SAME;
         drawChargeButton();
      }
		OffsetRect(&R, 0, 3 * FontHeight / 2);
	}

   /* Now let's work on the bottom ones. */
   R.Ymin = sR.Ymax - 3*(3*FontHeight/2);
   R.Ymax = R.Ymin + FontHeight + 4;
   R.Xmin = 4;
   randomize_posR = R;
   PaintRadioButton(&R,false,false,randomposmsg);
   mainR[main_items++] = &randomize_posR;

   OffsetRect(&R,0,3*FontHeight/2);
   randomize_chargeR = R;
   PaintRadioButton(&R,false,false,randomchargemsg);
   mainR[main_items++] = &randomize_chargeR;

   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanel(&R,RSS,runstopstepR,3,stopped);
   for(i=0;i<3;i++)
      mainR[main_items++] = &runstopstepR[i];



}

static void push(int item,int inout)
{
   switch(item)
   {
   case -1:
      break;
   case 5:
      RasterOp(zREPz);
      if (samecharges)
         InvertInsides(&tools[(item-5)/3].t->TB);
      else
         PushButton(&chargeR,inout);
      break;
   case 8:
   case 11:
   case 14:
   case 17:
   case 20:
      RasterOp(zREPz);
      InvertInsides(&tools[(item-5)/3].t->TB);
      break;
   case 25:
   case 26:
      RasterOp(zREPz);
      PushOrDoublePress(mainR[item],inout,item-25 == stopped);
      break;
   default:
      RasterOp(zREPz);
      PushButton(mainR[item],inout);
      break;
   }
   if (item == 4)
      ExtraHilite(mainR[item],inout);
}

void SelectNothing(void)
{
   push(current_main_item,false);
   current_main_item = -1;
}

static int pk[6][2] = {
   'c','C',
   'x','X',
   'k','K',
   'b','B',
   'g','G',
   'f','F'
   };
static int mainkeys[] = {XF1,XF2,XF3,XF4,XALTX};
static int lefters[] = {
   0,1,2,3,4,
   7,5,6,
   10,8,9,
   13,11,12,
   16,14,15,
   19,17,18,
   22,20,21,
   23,
   24,
   27,25,26
   };
static int uppers[] = {
   26,0,1,2,3,
   4,4,4,
   5,6,7,
   8,9,10,
   11,12,13,
   14,15,16,
   17,18,19,
   20,
   23,
   24,24,24
   };
static int downers[] = {
   1,2,3,4,5,
   8,9,10,
   11,12,13,
   14,15,16,
   17,18,19,
   20,21,22,
   23,23,23,
   24,
   26,
   0,0,0
   };


static int ourthings[] = {
   XLARROW,XDARROW,XUARROW,XHOME,XEND,XPGUP,XPGDN,XRARROW,0x0d
   };

static int closest_left(int Y)
{
   int closest = -1;
   int distance = 32767;
   int i;

   for(i=0;i<main_items;i++)
   {
      int cx,cy;
      int dist;
      Centers(mainR[i],&cx,&cy);
      dist = abs(cy - Y);
      if (dist < distance)
      {
         distance = dist;
         closest = i;
      }
   }
   return closest;
}



int ProcessControls(event *e,int n)
{
   int retval = 0;
   int X = e->CursorX;
   int Y = e->CursorY;
   int i;
   int last_item = current_main_item;
   int keyword = 0;
   int button = (e->State & 0x700) >> 8;

   if (n)
   {
      if (e->ASCII && e->ASCII != 0xe0)
         keyword = e->ASCII;
      else if (e->ScanCode != 0xff)
         keyword = e->ScanCode << 8;

    
      if (keyword)
      {
         int z = sizeof(ourthings)/sizeof(int);
         for(i=0;i<z;i++)
         {
            if (keyword == ourthings[i])
               break;
         }
         if (i == z)   
            return keyword;
      }

      if (button == swLeft || button == swRight)
      {
         keyword = 0x0d;
         incsize = (button == swRight);
      }

      if ((keyword == XLARROW || keyword == XHOME || keyword == XEND)
         && current_main_item == -1)
      {
         current_main_item = closest_left(Y);
         move_to_corner(mainR[current_main_item]);
      }
      else if (keyword == XRARROW || keyword == XPGUP || keyword == XPGDN)
      {
         switch(current_main_item)
         {
         case 5:
         case 6:
         case 8:
         case 9:
         case 11:
         case 12:
         case 14:
         case 15:
         case 17:
         case 18:
         case 20:
         case 21:
         case 25:
         case 26:
            current_main_item++;
            move_to_corner(mainR[current_main_item]);
            break;
         default:
            current_main_item = -1;
            HideCursor();
            CursorShowing = false;
            clubx = 16;
            MoveCursor(minx+clubx,cluby);
            if (clubflag == BALLBOX && !stopped)
               showball(ballx,bally,maxcolor);
            showclub(clubx,cluby,maxcolor-1);
            SelectNothing();
         }
      }
    

   }
   else
   {
      current_main_item = -1;
      for(i=0;i<main_items;i++)
      {
         /* something special for charges */
         if (i >= 5 && i <= 7 && !samecharges)
         {
            if (XYInRect(X,Y,&chargeR))
            {
               current_main_item = i;
               break;
            }
         }

         else if (XYInRect(X,Y,mainR[i]))
         {
            current_main_item = i;
            break;
         }
      }
   }

   if (keyword == 0x0d)
   {
      if (current_main_item >= 0)
      {
         int col,row;
         
         if (current_main_item <= 4)
            return mainkeys[current_main_item];

         else if (current_main_item <= 4 + 6*3)
         {
            i = current_main_item - 5;
            row = i / 3;
            col = i % 3;

            if (row == 0 && !samecharges)
               return XF5;

            if (col)
               return pk[row][col-1];
            else
            {
               /* Eeep, We have to process one of the
                  number fuckers. */
               numbertool *t = tools[row].t;

               double d = t->value;
               if (GetNumberBase(&t->TB,&d,t->type,t->lo,t->hi,t->p1,t->p2))
               {
                  t->value = d;
                  switch(row)
                  {
                  case 0:
                     {
               		   magnetstructure tM = M;
                        chargeunit = d;
                        loadcharges();
               		   updateallmagnets(&tM);
                     }
                     break;
                  case 1:
                     xsection = d;
                     break;
                  case 2:
                     magnetradius = d;
			            magrad2 = magnetradius * magnetradius;
			            magrad3 = magrad2 * magnetradius;
                     break;
                  case 3:
                     centerpull = d;
                     break;
                  case 4:
                     freq = d;
                     break;
                  case 5:
                     friction = d;
                     if (friction < 0.0001)
                     {
                        friction = 0;
                        frictiontype = 0;
                     }
                     break;
                  }
               }
               push(current_main_item,true);
            }
         }
         else if (current_main_item == 4 + 6*3 + 1)
            return 'h';
         else if (current_main_item == 4 + 6*3 + 2)
            return 'i';
         else if (current_main_item == 4 + 6*3 + 3)
         {
            if (stopped)
            {
               stopped = false;
               SetStopSign();
            }
         }
         else if (current_main_item == 4 + 6*3 + 4)
         {
            if (!stopped)
            {
               stopped = true;
               SetStopSign();
            }
         }
         else if (current_main_item == 4 + 6*3 + 5)
         {
            if (!stopped)
            {
               stopped = true;
               SetStopSign();
            }
            onestep = true;
         }


      }
   }


   navigate(keyword,lefters,NULL,uppers,downers,main_items,mainR,&current_main_item);

   if (current_main_item != last_item)
   {
      push(last_item,false);
      push(current_main_item,true);
   }
   return retval;
}

void IncreaseCharges(void)
{

	if (clubflag == MAGBOX)
	{
		long oldcharge = M.magnet[mymag].charge;
      long charge = oldcharge;

		if (charge == 0)
			charge = 1;
		else
		{
			if (incsize)
				charge = (charge < 0) ? (charge / 2) : (charge * 2);
			else
				charge++;
		}
		if (charge > 500)
			charge = 500;
      M.magnet[mymag].charge = charge;
		updateonemagnet(mymag, oldcharge);
	}
	else
	{
		magnetstructure tM = M;

		if (!chargeunit)
			chargeunit = 1;
		else if (incsize)
			chargeunit = (chargeunit < 0) ? (chargeunit / 2) : (chargeunit * 2);
      else
  			chargeunit++;
		if (chargeunit > 300)	/* you usually get corners with */
			reversibleflag = 0;	/* high freq and reversibleflag on */
		if (chargeunit > 500)
			chargeunit = 500;
		loadcharges();
		updateallmagnets(&tM);
	}
	drawChargeButton();
}
void DecreaseCharges(void)
{

	if (clubflag == MAGBOX)
	{
		long oldcharge = M.magnet[mymag].charge;
      long charge = oldcharge;

		if (incsize)
		{
			if (charge)
				charge = -1;
			else
				charge = (charge > 0) ? (charge / 2) : (charge * 2);
		}
		else
			charge--;

		/* Max out charge at -50 etc. */
		if (charge < -500)
			charge = -500;
      M.magnet[mymag].charge = charge;
		updateonemagnet(mymag, oldcharge);

	}


	else
	{
		magnetstructure tM = M;

		if (!chargeunit)
			chargeunit = -1;
		else if (incsize)
			chargeunit = (chargeunit > 0) ? (chargeunit / 2) : (chargeunit * 2);
      else
         chargeunit--;
		if (chargeunit < -500)
			chargeunit = -500;
		loadcharges();
		updateallmagnets(&tM);
	}
	drawChargeButton();
}

void SetStopSign(void)
{
   RasterOp(zREPz);
   /* Just paint two buttons... */
   PaintRadioButton(&runstopstepR[0],!stopped,!stopped,RSS[0]);
   PaintRadioButton(&runstopstepR[1],stopped,stopped,RSS[1]);
   current_main_item = -1;
}
