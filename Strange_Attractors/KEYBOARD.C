#include "attract.h"
#include <math.h>

int StampTracking=true;

static int CursorLevel(void)
{
   short cx,cy,cl,cb;
   QueryCursor(&cx,&cy,&cl,&cb);
   return cl;
}

static void Con(void)
{
   erasecursor();
   while (CursorLevel() < 0)
      ShowCursor();

}

static void Coff(void)
{
   if (curx > minx && CursorLevel() == 0)
   {
      erasecursor();
      HideCursor();
      drawcursor();
   }
}

static void incdecribbon(double multiplier)
{
   if (dimension == LOGISTIC ||
       (dimension != LORENZ && tracetype == 0))
         return;

	if (Stamping)
	{
		preserve_data();
		slide_stamps();
	}
	ribbonlength *= multiplier;
	if (ribbonlength > MAXRIBBONLENGTH)
		ribbonlength = MAXRIBBONLENGTH;
	if (ribbonlength < 4)
		ribbonlength = 4;
	if (dimension == LORENZ && trihedroncount > 1 && tritracetype == 1 && ribbonlength < 16)
		ribbonlength = 16;
	ribbonindex = 0;
	ribbonfull = 0;
	trihedronon = 0;
	installmode();
}


static void change_lorenz_xyz(char what)
{
	if (dimension == LORENZ)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		lorenzflyflag = 0;
		axis = what;
		installmode();
		ribbonindex = 0;
		ribbonfull = 0;
		trihedronon = 0;
	}
}

static void change_wrap_fly(void)
{
	if (dimension == LORENZ)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		if (1 /* trihedroncount || trihedronon */ )
		{
			lorenzflyflag = 1;
			viewspot = flyspot;
			viewtangent = flytangent;
			viewnormal = flynormal;
			viewbinormal = flybinormal;
			ribbonindex = 0;
			ribbonfull = 0;
			trihedronon = 0;
			installmode();
		}
	}
	else if (dimension == YORKE)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		yorketopologyflag += 1;
		yorketopologyflag %= 3;
		installmode();
		fillflock();
	}

	axis = 'w';

}

static void toggle_erase(void)
{
	if (dimension == LOGISTIC)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		if (tracetype == 0)
		{
			savefx = fx;
			savepixelx = pixelx;
			lvfx = flox + (double) (curx - minx) / xscale;
			cursorshape = 1;
			pushview(&logisticview);
			logstartval = LOGSTART;
			delayfactor = 50;
			if (fancyflag != 0)
				delayfactor = 10;
		}
		logisticlaunchflag = 1;
	}
	tracetype++;
	if (tracetype >= 2 && dimension > LOGISTIC)
		tracetype = 0;
	if (dimension == LOGISTIC && tracetype == 3)
		tracetype = 1;
	if (dimension == LOGISTIC && tracetype == 2)
		installmode();
	if (!tracetype && (dimension == HENON || dimension == YORKE))
		installmode();
	ribbonindex = 0;
	ribbonfull = 0;
}

static void varyhumpspot(double change)
{
	if (dimension == LOGISTIC)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}

		fancyflag = 1;
		humpspot += change;
		if (humpspot < 0.1)
			humpspot = 0.1;
		if (humpspot > 0.9)
			humpspot = 0.9;
		if (humpspot - 0.5 < 0.01 && humpspot - 0.5 > -0.01)
		{
			fancyflag = 0;
			humpspot = 0.5;
		}
		humpshift = log(0.5) / log(humpspot);
		logisticlaunchflag = 1;
		if (!tracetype)
			installmode();
		fillflock();
	}
}


static void incdecprecision(double multiplier, int logchange)
{
   if (dimension == HENON || dimension == YORKE)
      return;

	if (Stamping)
	{
		preserve_data();
		slide_stamps();
	}
	if (dimension == LOGISTIC)
	{
		standardstartlog -= logchange;
		if (standardstartlog < 1)
			standardstartlog = 1;
		standardstoplog = max(20, standardstartlog * 2);
	}
	else
	{
		dt *= multiplier;
		dt = min(.1, dt);
		dt = max(.001, dt);
		dt2 = dt / 2.0;
		dt6 = dt / 6.0;
		lorenzfreq = 1.0 / dt;

	}
   announceparms();
}



static void decrement_chaoticity(double amount)
{
	if (dimension != LORENZ)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		if (dimension == LOGISTIC)
		{
			if (tracetype)
			{
				lvfx -= amount;
				if (lvfx < amount)
					lvfx = amount;
				installmode();
				logisticlaunchflag = 1;
			}
			return;
		}
		if (dimension == HENON && fancyflag)
			fha -= amount;
		if (fha < 0)
			fha = 0;
		if (dimension == YORKE)
		{
			epsilon -= amount;
			if (epsilon < 0)
				epsilon = 0;
			epsbar = epsilon / TWOPI;
		}
		installmode();
		fillflock();
	}
}

void increment_chaoticity(double amount)
{
	if (dimension != LORENZ)
	{
		if (Stamping)
		{
			preserve_data();
			slide_stamps();
		}
		if (dimension == LOGISTIC)
		{
			if (tracetype)
			{
				lvfx += amount;
				if (lvfx > 3.9)
					lvfx = 3.9;
				installmode();
				logisticlaunchflag = 1;
			}
			return;
		}
		if (dimension == HENON && fancyflag)
			fha += amount;
		if (fha > 4.0)
			fha = 4.0;
		if (dimension == YORKE)
		{
			epsilon += amount;
			if (epsilon > 4)
				epsilon = 4;
			epsbar = epsilon / TWOPI;
		}
		installmode();
		fillflock();
	}
}
static int offers[] = {
	XF1, XF2, XF3, XF4, XF5, XALTF, XF10, XALTG, XALTH, XF20,
	XALTL, XALTQ, XALTS, XALTX, 0x1b, 'q', 'Q', XALTP,-1
};
int mousemove = false;


unsigned int checkmouse(event * e)
{
	int button;

	int n;


	int keyword = 0;

	n = KeyEvent(false, e);

	curxinc = e->CursorX - curx;
	curyinc = e->CursorY - cury;

	mousemove = !n;

	if (curxinc || curyinc)
	{
		if (soundflag)
			nosound();
		updatecursor();
	}

	if (n)
	{
		if (soundflag)
			nosound();

		if (e->ASCII && e->ASCII != 0xe0)
			keyword = e->ASCII;
		else if (e->ScanCode)
			keyword = e->ScanCode << 8;
		else
		{
			button = (e->State & 0x700) >> 8;
			if (button == swLeft)
			{
				if (e->CursorX >= minx)
					keyword = XINSERT;
				else
					keyword = CheckMainMenu(e->CursorX, e->CursorY);

			}
			else if (button == swRight)
			{
				keyword = CheckRightclickMainMenu(e->CursorX, e->CursorY);
				if (!keyword)
					keyword = XDELETE;
			}

		}
	}
	if (keyword == 0x0d && !singlestepflag)	/* might be a return while
						 * the cursor is in the
						 * magickbox */
		keyword = CheckMainMenu(curx, cury);
	return keyword;
}

#pragma option -G-
void checkkeyboard()
{
	unsigned int keyword;
	unsigned int n;
	char chosen_file[128];
	event e;
	int i;
	int coffed = false;

	n = checkmouse(&e);	/* note that all characters come from the
				 * event processor in checkmouse now. */

	if (n)
	{
		if (soundflag)
			nosound();
		if (n)
			keyword = n;



		if (keyword >= XF1 && keyword <= XF5)
		{
			int n = (keyword - XF1) >> 8;

			SetButton(n, true);
			WaitForNothing();
			coffed = true;
		}
		if (keyword == XALTX || keyword == XALTX)
		{
			SetButton(5, true);
			WaitForNothing();
			coffed = true;
		}

		for (i = 0; offers[i] != -1; i++)
		{
			if (keyword == offers[i])
				coffed = true;
		}
		if (coffed)
			Con();

		switch (keyword)
		{

		case XF1:
			helptext("attrmain.hlp");
			break;

		case XF2:
			do_files_menu();
			break;
		case XF3:
			do_types_menu();
			break;
		case XF4:
			do_tweaks_menu();
			break;
		case XF5:
			do_view_menu();
			break;

		case XALT1:
			restore_data(0);
			break;
		case XALT2:
			restore_data(1);
			break;
		case XALT3:
			restore_data(2);
			break;
		case XALT4:
			show_data(0);
			break;
		case XALT5:
			show_data(1);
			break;
		case XALT6:
			show_data(2);
			break;

      case XALT9:
         /* More nasty */
         StampTracking ^= 1;
         break;

		case XALT10:	/* nasty nasty. not documented. I don't care.
				 * Toggle whether any trihedrons exist. */
			if (trihedrons_exist)
			{
				ribbonindex = 0;
				ribbonfull = 0;
				installmode();
				setwindow(0);
			}
			trihedrons_exist ^= 1;
			break;




		case XALTA:	/* Preset Palettes */
			presetpalette();
			break;

		case XALTB:	/* Randomize Palette */
			randompalette();
			break;

		case XALTC:	/* Center */
         cleanflag = 1;
         logisticlaunchflag = 1;
			dozoom(0);
			break;

		case XALTD:	/* Default Palette */
			dodefaultpalette();
			break;

		case XALTE:
			locked ^= 1;
			break;

		case XALTF:	/* GIF View */
			gif_viewer();
			break;

		case XALTG:
			if (select_file("Save Image", "*.gif", chosen_file, "GIF") && Overwrite(chosen_file))
				SaveImageGif(chosen_file);
			break;

		case XALTH:	/* GIF Save Entire Screen */
			if (select_file("Save Screen", "*.gif", chosen_file, "GIF") && Overwrite(chosen_file))
				SaveScreenGif(chosen_file);
			break;

		case XALTI:	/* Status line toggle */
			ParameterDisplayMode ^= 1;
         announceparms();
			break;

		case XALTJ:	/* Cycle color once forward */
			Jspinpalette();
			break;

		case XALTK:	/* cycle color once reverse */
			revJspinpalette();
			break;

		case XALTL:	/* Load Parameters */
			if (select_file("Load Parameters", "*.sap", chosen_file, "SAP"))
				LoadParams(chosen_file);
			coffed = false;
			Coff();
			ribbonindex = 0;
			ribbonfull = 0;
			installmode();
			setwindow(0);
			break;

		case XALTM:	/* Monochrome toggle */
			grayflag ^= 1;
			grayscale();
			Jusepalette();
			break;

		case XALTO:	/* Open/Close edges for panning */
         if (dimension == LORENZ && (axis == 'w' || lorenzflyflag))
            ErrorBox("Panning not available in Fly's Eye View");
         else
   			dozoom(-2);
			break;

		case XALTP:	/* Palette Editor */
			palette_tweaker();
			break;

		case XALTQ:	/* QUIT to DOS */
			if (cancel_ok_msg("QUIT to DOS: Are you sure?"))
				exitflag = 2;
			break;

		case XALTR:	/* Redraw */
			installmode();
			cleanflag = 1;
			break;

		case XALTS:	/* Save Parameters */
			if (select_file("Save Parameters", "*.sap", chosen_file, "SAP") && Overwrite(chosen_file))
				SaveParams(chosen_file);
			break;

		case XALTT:	/* Make a Stamp */
			preserve_data();
			slide_stamps();
			break;

		case XALTV:	/* Volume (sound) */
			soundflag ^= 1;
			break;

		case XALTW:
			InfoBox();
			break;

		case XALTX:	/* Exit program */
			if (cancel_ok_msg("EXIT: Are you sure?"))
				exitflag = 1;
			break;


		case XALTY:	/* Color cycle toggle */
			spinflag = (++spinflag) % 3;
			break;

		case XALTZ:	/* Zoombox */
//         if (dimension == LORENZ && (axis == 'w' || lorenzflyflag))
//            ErrorBox("Zooming not available in Fly's Eye View");
//			else
         if (!(dimension == LOGISTIC && tracetype))
         {
            if (dimension == LORENZ && lorenzflyflag)
               resetcursor();
				zoombox();
         }
			break;

		case 'a':	/* Large dec chaoticity */
			decrement_chaoticity(0.1);
			break;

		case 'A':	/* Large inc chaoticity */
			increment_chaoticity(0.1);
			break;

		case 'b':	/* Med dec chaoticity */
			decrement_chaoticity(0.01);
			break;

		case 'B':	/* Med inc chaoticity */
			increment_chaoticity(0.01);
			break;

		case 'c':	/* Small dec chaoticity */
			decrement_chaoticity(0.001);
			break;

		case 'C':	/* Small dec chaoticity */
			increment_chaoticity(0.001);
			break;

		case 'd':	/* Change attractor type */
		case 'D':

			n = ParameterDisplayMode;
			ParameterDisplayMode = false;

			if (Stamping)
			{
				preserve_data();
				slide_stamps();
			}
			iteration = 0;
			long_iteration = 0;
			switch (dimension)
			{
			case LOGISTIC:
				if (tracetype == 2)
					dimension = LORENZ;
				break;
			case HENON:
				if (fancyflag)
					fancyflag = 0;
				else
					dimension = LOGISTIC;
				tracetype = -1;
				break;
			case LORENZ:
				dimension = YORKE;
				break;
			case YORKE:
				dimension = HENON;
				fancyflag = 1;
				break;
			}
			if (dimension != LOGISTIC || tracetype == -1)
				installmode();
			setwindow(1);
			switch (dimension)
			{
			case HENON:
			case YORKE:
				ribbonlength = 4;
				if (dimension == YORKE)
					fancyflag = 1;
				tracetype = 0;
				flocktype = (dimension == YORKE) ? 1 : 2;
				break;
			case LORENZ:
				flocktype = 1;
				ribbonlength = 64;
				tracetype = 1;
				delayfactor = 0;
				break;
			case LOGISTIC:
				cursorshape = 1;
				logstartval = LOGSTART;
				if (tracetype == 0)
				{
					pushview(&logisticview);
					delayfactor = 30;
					if (fancyflag != 0)
						delayfactor = 10;
				}
				logisticlaunchflag = 1;
				tracetype++;
				break;
			}
			fillflock();
			saved_fflock3ptr->n = fflock3ptr->n = 6;
			ParameterDisplayMode = n;
			announceparms();

			break;

		case 'e':	/* Cursor type toggle */
		case 'E':
			if (dimension == LOGISTIC && tracetype)
				break;
			cursorshape ^= 1;
			if (curx > minx)
				drawcursor();

			break;

		case 'f':	/* Add flock */
		case 'F':
			if (Stamping)
			{
				preserve_data();
				slide_stamps();
			}
			addflock();
			if (dimension == LORENZ)
			{
				if (ribbonlength < 16)
					ribbonlength = 16;
				if (flocktype != 2)
					installmode();
				ribbonindex = 0;
				ribbonfull = 0;
			}
			if (dimension == LOGISTIC)
			{
				logisticlaunchflag = 1;
				logstartval =
					(double) (sixteenbitsa() & 0x7FFF) / 0x7FFF;
			}
			break;

		case 'l':	/* Dec ribbon length */
			incdecribbon(0.5);
			break;

		case 'L':	/* Inc ribbon length */
			incdecribbon(2.0);
			break;

		case 'n':	/* Vary attractor params */
			if (dimension != LOGISTIC)
			{
				if (Stamping)
				{
					preserve_data();
					slide_stamps();
				}
				if (dimension == HENON)
					fancyflag ^= 1;

				if (dimension == YORKE)
				{
					randomize_yorkers();
				}
				installmode();
				fillflock();
				setwindow(0);
			}
			break;

		case 'N':
			if (dimension != LOGISTIC)
			{
				if (Stamping)
				{
					preserve_data();
					slide_stamps();
				}
				if (dimension == YORKE)
					yorkenumber = -1;

				if (dimension == HENON)
					fancyflag ^= 1;

				if (dimension == YORKE)
				{
					randomize_yorkers();
				}
				installmode();
				fillflock();
				setwindow(0);
			}
			break;

		case 'o':	/* Vary humpspot param */
			varyhumpspot(-0.1);
			break;

		case 'O':
			varyhumpspot(0.1);
			break;

		case 'p':	/* Auto Stamping toggle */
		case 'P':
			Stamping ^= 1;
			break;

		case 's':	/* Flytrace toggle */
		case 'S':
			if (dimension != LORENZ)
				break;
			tritracetype ^= 1;
			ribbonindex = 0;
			ribbonfull = 0;
			break;

		case 't':	/* toggle erase old points */
		case 'T':
			toggle_erase();
			break;

		case 'U':	/* Inc/Dec precision */
			incdecprecision(1 / 1.3, -20);
			break;

		case 'u':	/* Inc/dec precision */
			incdecprecision(1.3, 20);
			break;
		case 'v':	/* next kind of flock */
		case 'V':
         if (dimension == LOGISTIC)
            break;

			if (Stamping)
			{
				preserve_data();
				slide_stamps();
			}
			flocktype += 1;
			if (flocktype == 3)
				flocktype = 0;
			installmode();
			fillflock();
			trihedronon = 0;
			trihedroncount = 0;
			starting_trihedroncount = trihedroncount;
			break;

		case 'w':	/* Change Yorke wrap, Lorenz fly */
		case 'W':
			change_wrap_fly();
			break;

		case 'x':	/* Lorenz YZ */
		case 'X':
			change_lorenz_xyz('x');
			break;

		case 'y':	/* Lorenz XZ */
		case 'Y':
			change_lorenz_xyz('y');
			break;

		case 'z':	/* Lorenz XY */
		case 'Z':
			change_lorenz_xyz('z');
			break;

		case ',':	/* comma */
			delayfactor += 10;
			break;

		case '.':	/* period */
			delayfactor -= 10;
			if (delayfactor < 0)
				delayfactor = 0;
			break;

		case ' ':	/* spacebar */
			if (!stopped)
         {
				stopped = 1;
            announceparms();
         }
			else
			{
				if (e.State & 3)
            {
					stopped = 0;
               announceparms();
            }
				else
					onestep = true;
			}
			break;

		case 13:
			if (singlestepflag)
				stepme = 1;
			break;


		case XHOME:
			curxinc = -curspeed;
			curyinc = -curspeed;
			break;

		case XUARROW:
			curxinc = 0;
			curyinc = -curspeed;
			break;

		case XPGUP:
			curxinc = curspeed;
			curyinc = -curspeed;
			break;

		case XLARROW:
			curxinc = -curspeed;
			curyinc = 0;
			break;

		case XRARROW:
			curxinc = curspeed;
			curyinc = 0;
			break;

		case XEND:
			curxinc = -curspeed;
			curyinc = curspeed;
			break;

		case XDARROW:
			curxinc = 0;
			curyinc = curspeed;
			break;

		case XPGDN:
			curxinc = curspeed;
			curyinc = curspeed;
			break;

		case XINSERT:
			if (curx < minx)
				break;
			if (Stamping)
			{
				preserve_data();
				slide_stamps();
			}

			if (!cursorshape)
         {
            if (dimension == LORENZ && lorenzflyflag)
               resetcursor();
				dozoom(1);
         }
			else
			{
				if (dimension > LOGISTIC)
				{
					addflock();
					if (dimension == LORENZ)
					{
						if (ribbonlength < 16)
							ribbonlength = 16;
					}
				}
				else
				{
					if (!tracetype)
					{
						lvfx = flox + (double) (curx - minx) / xscale;
						tracetype = 1;
						pushview(&logisticview);
						logstartval = LOGSTART;
						logisticlaunchflag = 1;
					}
					else if (tracetype == 1)
					{
						logstartval =
							(double) (maxy - cury) / maxy;
						logisticlaunchflag = 1;
					}
					else
					{
						logstartval =
							(double) (curx - minx) / maxx;
						installmode();
						cleanflag = 1;
						loghumpselectflag = 1;
					}
				}
			}
			break;

		case XDELETE:
			if (curx < minx)
				break;

			if (!cursorshape)
         {
            if (dimension == LORENZ && lorenzflyflag)
               resetcursor();
				dozoom(-1);
         }
			else
			{
				if (Stamping)
				{
					preserve_data();
					slide_stamps();
				}
				if (dimension == LOGISTIC && tracetype)
				{
               extern int everinMap;

					tracetype = 0;
					popview(&logisticview);
               if (!everinMap)
                  setwindow(1);
               logisticlaunchflag = 1;
					delayfactor = 0;
					installmode();
					break;
				}
				if (dimension == LORENZ)
				{
					if (fflock3ptr->n > 1)
					{
						fflock3ptr->n--;
						saved_fflock3ptr->n--;
						eraseribbon(fflock3ptr->n);
						if (trihedroncount)
						{
							trihedroncount--;
							erasetrihedron(fflock3ptr->n);
						}
					}
					else
					{
						if (trihedroncount)
						{
							eraseribbon(0);
							trihedroncount = 0;
							erasetrihedron(0);
						}
					}
				}
				if ((dimension == HENON || dimension == YORKE))
				{
					if (fflock2ptr->n > 1)
					{
						fflock2ptr->n--;
						saved_fflock2ptr->n--;
						eraseribbon(fflock2ptr->n);
					}
				}
			}
			break;
		}
		if (coffed)
			Coff();
	}
	if (curxinc || curyinc)
		updatecursor();
	/* discard an upclick! */
	WaitForNothing();
	if (keyword >= XF1 && keyword <= XF5)
	{
		int n = (keyword - XF1) >> 8;

		SetButton(n, false);
	}
	if (keyword == XALTX || keyword == 0x1b || keyword == 'Q' || keyword == 'q')
		SetButton(5, false);


}
