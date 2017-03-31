#include "mag.h"
/*------------------Mouse and Keyboard Functions-------------*/

int CursorShowing = 0;
int incsize = 0;
extern C_struct C;
                                
static void adjust_centerpull(int increment,double multiplier)
{
   if (centerpull == 0 && increment < 0)
      centerpull = -1;
   else if (incsize)
      centerpull *= multiplier;
   else
      centerpull += increment;
   centerpull = max(centerpull,-500);
   centerpull = min(centerpull,500);
   drawCenterpullButton();
}
static int CursorLevel(void)
{
   short x,y,l,b;
   QueryCursor(&x,&y,&l,&b);
   return l;
}

static void MaybeShowCursor(void)
{
   if (CursorLevel() < 0)
      ShowCursor();
}

static void MaybeHideCursor(void)
{
   if (CursorLevel >= 0)
      HideCursor();
}

static void adjust_radius(double multiplier)
{
   radius *= multiplier;
   radius = max(radius,1.0);
   radius = min(radius,135);
   NewRadius();
}

static void adjust_friction(double m1,double m2)
{
   if (incsize)
      friction *= m1;
   else
      friction *= m2;
   friction = min(friction,500.0);
   friction = max(friction,0.0);
	drawFrictionButton();
}

static void adjust_frequency(int direction)
{
   if (!direction)
   	freq = (incsize) ? freq * 2 : (freq + 1);
   else
		freq = (incsize) ? freq / 2 : (freq - 1);
   freq = max(freq,2);
   freq = min(freq,10000);
   drawFrequencyButton();
}


void MasterReset(void)
{
   if (CursorShowing)
      HideCursor();
   CursorShowing = false;
   if (basinflag)
   {
      firsttimeflag = 1;
      if (!CursorShowing)
         showball(ballx,bally,maxcolor);
   }
	ResetLayout();
   ResetBasins();
   MoveCursor(minx+clubx,cluby);

   inptr = outptr = erasing = 0;
}
void ForceCursorOn(void)
{
   short cx,cy,cl,cb;

   do
   {
      ShowCursor();
      QueryCursor(&cx,&cy,&cl,&cb);
   } while(cl < 0);
}

void do_quit_button(char *text, int retval)
{
	char tbuf[128];
   PushCursorPosition();
   ForceCursorOn();
	sprintf(tbuf, "%s: Are you sure?", text);
	PaintQuitButton(true);
	WaitForNothing();
	if (cancel_ok_msg(tbuf))
	{
		exitflag = retval;
		helpscreenflag = 1;
	}
	PaintQuitButton(false);
   if (!CursorShowing)
      HideCursor();
   PopCursorPosition();
}



#define DO(x) ForceCursorOn(); \
   x;\
   if (!CursorShowing) HideCursor()

void checkmouse()
{
	event e;
	int keyword;
	int n;
	int X, Y;
   int XX,YY;
   int i;

	clubxinc = clubyinc = 0;

	n = KeyEvent(false, &e);
   X = e.CursorX;

	keyword = 0;
   i = e.ScanCode << 8;

   if (n && (i == XLARROW || i == XHOME || i == XEND) && X <= minx + 8)
   {
      ProcessControls(&e,n);
      PeekEvent(1,&e);
      X = e.CursorX;
      Y = e.CursorY;
   }
   incsize = 0;

	if (X >= minx - halfclub - 2)
	{
		if (CursorShowing)
      {
   		CursorShowing = false;
			HideCursor();
         if (clubflag == BALLBOX)
            showball(ballx,bally,maxcolor);
         showclub(clubx,cluby,maxcolor-1);
         SelectNothing();
      }
		XX = e.CursorX - minx;
		YY = e.CursorY;

      if (clubflag == BALLBOX && (XX != ballx || YY != bally))
      {
         extern char erasures[512];
			showclub(clubx, cluby, 0);
         showball(ballx,bally,maxcolor);
         ballx = XX;
         bally = YY;
			clubx = XX;
			cluby = YY;
         showball(ballx,bally,maxcolor);
			showclub(clubx, cluby, maxcolor - 1);
         erasures[(inptr+511) & 0x1ff] = false;
      }

		else if (XX != clubx || YY != cluby)
		{
			showclub(clubx, cluby, 0);
			clubx = XX;
			cluby = YY;
			showclub(clubx, cluby, maxcolor - 1);
		}
	}
	else
	{
		if (!CursorShowing)
		{
   		CursorShowing = true;
			nosound();
         if (clubflag == BALLBOX)
            showball(ballx,bally,maxcolor);
         showclub(clubx,cluby,0);
         while(1)
         {
            short cx,cy,cl,cb;
            QueryCursor(&cx,&cy,&cl,&cb);
            if (cl < 0)
      			ShowCursor();
            else
               break;
         }
		}
		if (!n)
			e.ASCII = e.ScanCode = 0;

		keyword = ProcessControls(&e, n);

	}

	if (n || keyword)
	{
		if (X > minx && (e.State >> 8) == swLeft)
		{
			swingflag = 1;
			rollflag = 1;
		}
		else if (X > minx && (e.State >> 8) == swRight)
		{

			switch (clubflag)
			{
			case BALLBOX:
			case CLUB:
				swingflag = 1;
				rollflag = 1;
				break;
			case MAGBOX:
				undoflag = 1;
				break;
			}
		}
      else if (X >= minx - halfclub - 2)
		{
			if (e.ASCII && e.ASCII != 0xe0)
				keyword = e.ASCII;
			else if (e.ScanCode && e.ScanCode != 0xff)
				keyword = e.ScanCode << 8;
		}
      if (keyword)
         nosound();

		switch (keyword)
		{

		case XF1:
         PaintMainButton(0,true);
         WaitForNothing();
         DO(helptext("Magnets.hlp"));
         PaintMainButton(0,false);
         current_main_item = -1;
			break;

      case XF2:
         PaintMainButton(1,true);
         WaitForNothing();
         DO(do_files_menu());
         PaintMainButton(1,false);
         current_main_item = -1;
         break;

      /* XF3 is with ALTN */

		case XF4:
         PaintMainButton(3,true);
         WaitForNothing();
         DO(options_panel());
         PaintMainButton(3,false);
         current_main_item = -1;
         break;

      case XF5:
         PaintMainButton(4,true);
         WaitForNothing();
         DO(charge_panel());
         PaintMainButton(4,false);
         current_main_item = -1;
         break;

      case XALTA:
			changeVGApalette();
         break;
		case XALTB:	/* Random Palettes */
         randompalette();
			break;

		case XALTD:	/* Default Palette */
			setdefaultpalette();
			break;

      case XALTE:
         locked ^= 1;
         break;

		case XALTF:	/* GIF View */
         DO(gif_viewer());
         break;

		case XALTG:	/* GIF Save Image Area */
			DO(FGifOutput(false));	/* Not on any control panel */
			break;


		case XALTH:	/* GIF Save Entire Screen */
			DO(FGifOutput(true));
			break;

      case XALTJ:
			spinpalette();
         break;
      case XALTK:
			revspinpalette();
         break;


		case XALTL:	/* Load Parameters */
         MaybeShowCursor();
			FLoadLayout();
         MaybeHideCursor();
   
         basinstep=8;basinx=basiny=0;
         inptr = outptr = erasing = 0;
			break;

      case XALTM: /* mono flag */
         grayflag ^= 1;
         grayscale();
         usepalette();
         break;

		case XALTN:	/* Next Canned Layout */
      case XF3:
         PaintMainButton(2,true);
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
         }
         if (clubflag == 2)
            clubflag = 1;
			CannedLayout();
         PaintMainButton(2,false);
         inptr = outptr = erasing = 0;
         current_main_item = -1;
         break;

		case XALTP:	/* Palette Editor */
			DO(palette_tweaker());
			break;

		case XALTQ:	/* Quit to DOS */
			do_quit_button("Quit to DOS", 2);
			break;

		case XALTR:	/* Redraw */
			/* clear out old trails */
         MasterReset();
         firsttimeflag = 1;
         inptr = outptr = erasing = 0;
			break;

		case XALTS:	/* Save Parameters */
			DO(FSaveLayout());
			break;

		case XALTV:	/* Sound Toggle */
			soundflag ^= 1;
			if (!soundflag)
				nosound();
			break;

		case XALTW:	/* Information Screen */
			InfoBox();
			break;

		case XALTX:	/* Exit to DOS */
			do_quit_button("EXIT", 1);
			break;

      case XALTY:
         spinflag++;
         if (spinflag == 3)
            spinflag = 0;
         break;


		case 'a':	/* Toggle Forcetype */
      case 'A':
			forcetype ^= 1;
			break;


		case 'b':	/* Decrease Center Pull */
      case 'B':
         adjust_centerpull((keyword == 'b') ? -1 : 1,
            (keyword == 'b') ? 0.5 : 2.0);
         break;

		case 'c':	/* Panel 1 */
			DecreaseCharges();
			break;

		case 'C':
			IncreaseCharges();
			break;


		case 'd':
      case 'D':
         adjust_radius((keyword == 'd') ? 1/1.5 : 1.5);
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(clubx+minx,cluby);
         }
			break;


		case 'e':	/* Lively/Physical Toggle */
		case 'E':
			rkflag ^= 1;
			if (rkflag)
			{
				reversibleflag = 0;
				fspeedx = speedx;
				fspeedy = speedy;
			}
			break;

		case 'f':
         adjust_friction(0.1,0.9);
         break;
		case 'F':
			if (!frictiontype)
			{
				frictiontype = 1;
				friction = 0.01;
   			drawFrictionButton();
			}
         else adjust_friction(10.0,1.1);
			break;

		case 'g':	/* Panel 1 */
      case 'G':
         adjust_frequency(keyword == 'g');
			break;


		case 'h':	/* Randomize magnet position */
		case 'H':
         PaintMainButton('h',true);
         WaitForNothing();
			RandomizePositions();
         PaintMainButton('h',false);
         basinx = basiny = 0;
         basinstep = 8;
         inptr = outptr = erasing = 0;
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(ballx+minx,bally);
         }
			break;

		case 'i':	/* Randomize magnet charges */
		case 'I':
         PaintMainButton('i',true);
         WaitForNothing();
			RandomizeCharges();
         PaintMainButton('i',false);
         basinx = basiny = 0;
         basinstep = 8;
         inptr = outptr = erasing = 0;
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(ballx+minx,bally);
         }
			break;

		case 'j':	
		case 'J':	/* Basin Mode Toggle */
			ToggleBasins();
         MasterReset();
         if (basinflag)
         {
            firsttimeflag = 0;
            showball(ballx,bally,maxcolor);
         }
         rollflag = 1;
			break;

		case 'k':	
         if (incsize)
            magnetradius /= 2;
         else
   			magnetradius--;
			if (magnetradius < 1)
				magnetradius = 1;
			magrad2 = magnetradius * magnetradius;
			magrad3 = magrad2 * magnetradius;
			drawRadiusButton();
			break;

		case 'K':
         if (incsize)
            magnetradius *= 2;
         else
   			magnetradius++;
			magnetradius = min(magnetradius, 60);

			magrad2 = magnetradius * magnetradius;
			magrad3 = magrad2 * magnetradius;
			drawRadiusButton();
			break;

		case 'l':
			memorylength = (incsize) ? memorylength / 2 : (memorylength - 1);
			if (memorylength < 2)
				memorylength = 2;
			resetLength();
         inptr = outptr = erasing = 0;
			break;
		case 'L':
			memorylength = (incsize) ? memorylength * 2 : (memorylength + 1);
			if (memorylength > 512)
				memorylength = 512;
			resetLength();
         inptr = outptr = erasing = 0;
			break;

		case 'n':
		case 'N':	/* save present config */
			sound(600);	/* beep so you know it happened */
			delay(100);
			nosound();
			delay(50);
			if (C.n >= MAXCONFIGS - 1)
			{	/* second beep lo if there's no more room */
				sound(400);
				delay(200);
				nosound();
				break;
			}
			sound(800);	/* optimistic hi second beep if ok */
			delay(200);
			nosound();
			C.n++;
			saveconfig(&C.config[C.n]);
			break;

		case 'o':	/* toggle stride between 1 and 9 */
		case 'O':
			stride ^= 8;
			break;


		case 'r':
		case 'R':	/* clear trails, and reset frictiondebt etc. */
         /* but we _keep_ the ball here */
			installmode();
			setuphole(holenumber);
         ResetBasins();
         firsttimeflag = 1;
         inptr = outptr = erasing = 0;
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(clubx+minx,cluby);
         }
			break;

		case 's':	/* what is this for? */
		case 'S':	/* Reset box on bob */
			installmode();
			saveballflag = 1;
         basinstep=8;basinx=basiny=0;
			setuphole(holenumber);
         inptr = outptr = erasing = 0;
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(clubx+minx,cluby);
         }
			break;


		case 't':
			tracetype--;
			if (tracetype < 0)
				tracetype = 4;
			if (tracetype == 3 || tracetype == 4)
				resetLength();
         if (tracetype == 4 && clubflag == BALLBOX)
            showball(ballx,bally,maxcolor);
         inptr = outptr = erasing = 0;
			break;
		case 'T':
			tracetype = (tracetype + 1) % 5;
			if (tracetype == 3 || tracetype == 4)
				resetLength();
         if (tracetype == 4 && clubflag == BALLBOX)
            showball(ballx,bally,maxcolor);
         inptr = outptr = erasing = 0;
			break;

		case 'u':	/* Reversibility Toggle */
		case 'U':
			reversibleflag ^= 1;
			break;


		case 'v':	
		case 'V':	
			/*
			 * reverseflag: 0 is forward, 1 is reverse, 2 signals
			 * start reverse, 3 indicates start forward, in one
			 * scheme, but in practice now I just use 0 and 2.
			 */
			speedx *= -1;
			speedy *= -1;
			/*
			 * deltaxdebt *= -1; deltaydebt *= -1;
			 */
#pragma warn -sig
			deltax = speedx / freq;
			deltay = speedy / freq;
#pragma warn .sig
			reverseflag |= 2;
			break;


		case 'x':	/* panel 1 */
         if (incsize)
            xsection /= 2;
         else
   			xsection--;
			if (xsection < 0)
				xsection = 0;
			if ((magnetradius == 0) && (xsection == 0))
				xsection = 1;
			drawCaptureButton();
			break;

		case 'X':
	      if (incsize)
            xsection *= 2;
         else
            xsection++;
			if (xsection > 20)
				xsection = 20;
			drawCaptureButton();
			break;

		case 'y':	/* toggle flag between 1 and 2 */
      case 'Y':
			if (!frictiontype)	/* No panel */
				break;
			frictiontype ^= 3;
			break;

      /* WARNING SECRET OPTION HERE */
      case 'z':
         if (basinlimit > 2)
            basinlimit /= 2;
         break;
      case 'Z':
         basinlimit *= 2;
         break;
      case 26:
         basindisplaymode ^= 1;
         break;
      case 2: /* control b */
         DO(basin_menu());
         break;


		case ',':	/* nowhere */
			delayfactor += 10;
			break;

		case '.':
			delayfactor -= 10;
			if (delayfactor < 0)
				delayfactor = 0;
			break;

		case '+':	/* nowhere */
			if (clubflag == MAGBOX)
			{
				if (M.magnet[mymag].charge < 0)
				{
					long oldcharge = M.magnet[mymag].charge;

					M.magnet[mymag].charge *= -1;
					updateonemagnet(mymag, oldcharge);
					drawChargeButton();
				}
			}
			else
			{
				if (chargeunit <= 0)
				{
					magnetstructure tM = M;

					chargeunit *= -1;
					loadcharges();
					updateallmagnets(&tM);
					drawChargeButton();
				}
			}
			break;

		case '-':	/* nowhere */
			if (clubflag == MAGBOX)
			{
				if (M.magnet[mymag].charge > 0)
				{
					long oldcharge = M.magnet[mymag].charge;

					M.magnet[mymag].charge *= -1;
					updateonemagnet(mymag, oldcharge);
					drawChargeButton();
				}
			}
			else
			{
				if (chargeunit > 0)
				{
					magnetstructure tM = M;

					chargeunit = -chargeunit;
					loadcharges();
					updateallmagnets(&tM);
					drawChargeButton();
				}
			}
			break;

		case ' ':	/* spacebar toggles pause */
         if (stopped)
         {
            if (e.State & 3)
            {
               stopped = false;
               SetStopSign();
            }   
            else if (clubflag == CLUB)
               onestep = 1;
         }
         else
         {
            stopped = true;
            SetStopSign();
         }
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
         if (clubflag == MAGBOX)
            clubflag = CLUB;
			holenumber = keyword - '0';
			newholeflag = 1;
			installmode();
			setuphole(holenumber);
         ResetBasins();
         firsttimeflag = 1;
         inptr = outptr = erasing = 0;
         if (CursorShowing)
         {
            HideCursor();
            CursorShowing = false;
            MoveCursor(clubx+minx,cluby);
         }
			break;

		case 0x0d:	/* Enter */
			swingflag = 1;
			rollflag = 1;
			helpscreenflag = 0;
			break;

		case XINSERT:
			/* only do this if we have a plain club. */
			if (clubflag == CLUB)
				add_a_magnet(clubx, cluby);
			break;

		case XDELETE:
			if (clubflag == MAGBOX)
				delete_a_magnet();
			break;

		case XHOME:	/* home */
			clubxinc = -stride;
			clubyinc = -stride;
			break;

		case XUARROW:
			clubxinc = 0;
			clubyinc = -stride;
			break;


		case XLARROW:
			clubxinc = -stride;
			clubyinc = 0;
			break;

		case XRARROW:
			clubxinc = stride;
			clubyinc = 0;
			break;

		case XEND:
			clubxinc = -stride;
			clubyinc = stride;
			break;

		case XDARROW:
			clubxinc = 0;
			clubyinc = stride;
			break;

		case XPGUP:
			clubxinc = stride;
			clubyinc = -stride;
			break;

		case XPGDN:
			clubxinc = stride;
			clubyinc = stride;
			break;
		}
	}
	if ((clubxinc) || (clubyinc))
	{
      if(X > minx)
      {
		   showclub(clubx, cluby, 0);
		   clubx += clubxinc;
		   cluby += clubyinc;
		   if ((clubx < 0) || (clubx > maxx))
			   clubx -= clubxinc;
		   if ((cluby < 0) || (cluby > maxy))
			   cluby -= clubyinc;
		   showclub(clubx, cluby, maxcolor - 1);
      }
      else
      {
         X += clubxinc;
         Y += clubyinc;
         X = max(X,0);
         Y = max(Y,0);
         Y = min(Y,maxy);
         MoveCursor(X,Y);
      }
   
		clubxinc = 0;
		clubyinc = 0;
	}
}
