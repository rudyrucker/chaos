/*      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal code of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.							     *
*									     *
******************************************************************************/

#include "mand.h"

static void On(void)
{
   ProtectOff();
   ArrowCursor();
};
static void Off(void)
{
   if (curx > minx)
      BoxCursor();
   _ProtectCurrentRow();
}
static char pushers[] = "aAbBcCeEfFjJlLmMnNrRsSuUvVwW";

int ParseKey(int keyword)
{
   int uvflag = 0;
   char tbuf[128];
   int i;

   if (keyword == 0x0d && current_main_item != -1)
   {
      
      if (current_main_item < 5)
         keyword = XF1 + 0x100 * current_main_item;
      else if (current_main_item == 5)
         keyword = XALTX;
      else
         keyword = XALT1 + 0x100 * (9 - current_main_item);
   }
   for(i=0;pushers[i];i++)
   {
      if (keyword == pushers[i])
      {
        	pushstamp(&stampviews[saveptr]);
         uvflag = 1;
         SaveMe = 1;
         break;
      }
   }


	switch (keyword)
	{
	case XALTA:
      bumppalette();
		break;

	case XALTB:
      randompalette();
		break;
//	case XALTC:
   case 'c':
   case 'C':
      pushstamp(&stampviews[saveptr]);
		fu = fv = fa = fb = 0;
		slicetype = 0;
		juliaflag = 0;
		ruckerflag = 0;
		zoomfactor = 0.35;
		if (!cubicflag)
		{
			maxiteration = 50;
			mandelflag = 1;
		}
		else
			maxiteration = 20;
		bandsize = 1;
		fillcolorval();
		insideflag = 0;
		fourdeeflag = 0;
		popview(&startview);
		ruckerview = mandelview = cubicview = startview;
		SaveMe = 1;
		uvflag = 1;
		break;

   case XALTD:
      DefaultPalette();
      break;

   case XALTE:
      locked ^= 1;
      fillcolorval();
      uvflag = 1;
      break;

   case XALTF:
      On();
      gif_viewer();
      Off();
      break;

   case XALTG:
		/* Save the mess to a GIF file. */
      On();
		if (select_file("Save image as GIF file", "*.GIF", tbuf, "GIF") && Overwrite(tbuf))
		 	GifOutput(tbuf, 0);
      Off();
		break;
	case XALTH:
      On();
		if (select_file("Save screen as GIF file", "*.GIF", tbuf, "GIF") && Overwrite(tbuf))
			GifOutput(tbuf, 1);
      Off();
		break;


   case XALTI:
      ParameterDisplayMode++;
      if (ParameterDisplayMode >= 3)
         ParameterDisplayMode = 0;
      bottomline();
      break;


      

   case XALTJ:
		spinpalette();
		break;


   case XALTK:
      revspinpalette();
		break;

   case XALTL:
      On();
		if (select_file("Load Fractal Parameters", "*.FRP", tbuf, "FRP"))
			load_params(tbuf);
      Off();
      break;

	case XALTM:
		grayflag ^= 1;
		grayscale();
		usepalette();
		break;

   case XALTN:
      next_preset();
      load_preset();
      break;



   case XALTO:
      if (curx >= minx)
         Pan();
		break;
   case XALTP:
      On();
      palette_tweaker();
      Off();
      break;

   case XALTQ:
      On();
      PaintQuitButton(true);
      WaitForNothing();
		if (cancel_ok_msg("Quit to DOS: Are you sure?"))
			exitflag = 2;
      PaintQuitButton(false);
      Off();
      break;

   case XALTS:
      On();
		if (select_file("Save Fractal Parameters", "*.FRP", tbuf, "FRP") && Overwrite(tbuf))
			save_params(tbuf);
      Off();
      break;


   case XALTV:
		soundflag ^= 1;
		break;

   case XALTW:
      InfoBox();
      break;

   case XALTX:
      On();
      PaintQuitButton(true);
      WaitForNothing();
		if (cancel_ok_msg("EXIT: Are you sure?"))
			exitflag = 1;
      PaintQuitButton(false);
      Off();
      break;
  

   case XALTY:
      spinflag = (++spinflag) % 3;
      break;

   case XALTZ:
      zoombox();
      break;

   case ' ':
		/*
		* Spacebar does 2 things: 1) It toggles updating the
		* screen 2) It toggles a box cursor to the main
		* cursor display.
		*/
      zoombox();
		break;

	case 'a':
   case 'A':
      if (cubicflag && !ruckerflag)
   		fa += 0.02 * (fhix - flox) * (keyword == 'a' ? -1 : 1);
      else
         uvflag = SaveMe = false;
		break;

	case 'b':
   case 'B':
      if (cubicflag && !ruckerflag)
   		fb += 0.02 * (fhix - flox) * (keyword == 'b' ? -1 : 1);
      else
         uvflag = SaveMe = false;
		break;

   case XALTC:
      pushstamp(&stampviews[saveptr]);
		fourdeeflag = 0;
		popview(&startview);
		ruckerview = mandelview = cubicview = startview;
		if (!cubicflag)
			maxiteration = 50;
      else
			maxiteration = 20;
		SaveMe = 1;
		uvflag = 1;
		break;

	case 'd':
   case 'D':
		fourdeeflag ^= 1;
		break;


	case 'e':
   case 'E':
//		if (cubicflag && !juliaflag)
//			readcursor();	/* select new cubic */
		if (!cubicflag)
		{
			fa = fb = fu = fv = 0;	/* coming from mandel */
			cubicview = startview;
			maxiteration = 20;
		}
		juliaflag = 0;
		mandelflag = 0;
		cubicflag = 1;
		ruckerflag = 0;
		break;

	case 'f':	
   case 'F':
      if (cubicflag && !ruckerflag)
      {
   		insideflag++;
   		if (insideflag == 3)
   			insideflag = 0;
      }
      else
         uvflag = SaveMe = false;
		break;

	case 'i':
		if (maxiteration > 200)
			maxiteration -= 100;
		else if (maxiteration > 50)
			maxiteration -= 50;
		else
			maxiteration -= 10;
		if (maxiteration <= 0)
			maxiteration = 10;
		break;

	case 'I':
		if (maxiteration == 10)
			maxiteration = 15;
		else if (maxiteration < 50)
			maxiteration += 10;
		else if (maxiteration < 200)
			maxiteration += 50;
		else if (maxiteration < 32600)
			maxiteration += 100;
		break;

	case 'j':
   case 'J':
		if (!juliaflag)
			newjuliaflag = 1;
		juliaflag = 1;
//		readcursor();
		ruckerflag = 0;
		break;


	case 'l':
		newsliceflag = 1;
		slicetype--;
		if (slicetype == -1)
			slicetype = 5;
      a = starta,b=startb,u=startu,v=startv;
      fa=startfa,fb=startfb,fu=startfu,fv=startfv;
		break;

	case 'L':
		newsliceflag = 1;
		slicetype++;
		slicetype %= 6;
      a = starta,b=startb,u=startu,v=startv;
      fa=startfa,fb=startfb,fu=startfu,fv=startfv;
		break;


	case 'm':
   case 'M':
		if (cubicflag)
		{
			mandelview = startview;
			maxiteration = 50;
		}
		juliaflag = 0;
		mandelflag = 1;
		cubicflag = 0;
		ruckerflag = 0;
		break;

	case 'n':
   case 'N':
      if (!ruckerflag && !cubicflag)
      {
   		insideflag++;
   		if (insideflag == 3)
   			insideflag = 0;
      }
      else
         uvflag = SaveMe = false;
		break;





	case 'r':
   case 'R':
		juliaflag = 0;
		mandelflag = 0;
		cubicflag = 1;
		ruckerflag = 1;
      maxiteration = 20;
		break;

	case 's':
   case 'S':
      if (ruckerflag)
      {
   		insideflag++;
   		if (insideflag == 3)
   			insideflag = 0;
      }
      else
         uvflag = SaveMe = false;
		break;


	case 'u':
		fu -= 0.02 * (fhix - flox);
		break;

	case 'U':
		fu += 0.02 * (fhix - flox);
		break;

	case 'v':
		fv -= 0.02 * (fhix - flox);
		break;

	case 'V':
		fv += 0.02 * (fhix - flox);
		break;

	case 'w':
		bandsize /= 2;
      bandsize = max(bandsize,.001);
		fillcolorval();
		break;

	case 'W':
		bandsize *= 2;
      bandsize = min(bandsize,32767);
		fillcolorval();
		break;

	case 'x':
   case 'X':
		cursorshape ^= 1;
      updatecursor();
		break;

	case XHOME:	/* home */
      if (curx < minx)
      {
         current_main_item = 0;
         main_corner(current_main_item);
      }
      else
      {
   		curxinc = -curspeed;
   		curyinc = -curspeed;
      }
		break;

	case XUARROW:	/* up */
      if (curx < minx)
      {
         current_main_item--;
         if (current_main_item < 0)
            current_main_item = 9;
         main_corner(current_main_item);
      }
      else
      {
   		curxinc = 0;
   		curyinc = -curspeed;
      }
		break;

	case XPGUP:	/* pgup */
		curxinc = curspeed;
		curyinc = -curspeed;
		break;

	case XLARROW:	/* left */
      if (curx < minx)
      {
         current_main_item = ClosestButton(curx,cury);
         main_corner(current_main_item);
         ArrowCursor();
      }
      else
      {
   		curxinc = -curspeed;
	   	curyinc = 0;
      }
		break;

	case XRARROW:	/* right */

      if (curx < minx)
      {
         curxinc = minx + 8 - curx;
         curyinc = 0;
      
      }
      else
      {
   		curxinc = curspeed;
	   	curyinc = 0;
      }
		break;

	case XEND:	/* end */
      if (curx < minx)
      {
         current_main_item = 9;
         main_corner(current_main_item);
      }
      else
      {
   		curxinc = -curspeed;
   		curyinc = curspeed;
      }
		break;

	case XDARROW:	/* down */
      if (curx < minx)
      {
         current_main_item++;
         if (current_main_item > 9)
            current_main_item = 0;
         main_corner(current_main_item);
      }
      else
      {
   		curxinc = 0;
   		curyinc = curspeed;
      }
		break;

	case XPGDN:	/* pgdn */
		curxinc = curspeed;
		curyinc = curspeed;
		break;

	case XINSERT:	/* insert */
      if (!trystamps())
      {
			if (cursorshape == 0)
				dozoom(zoomfactor);
			else
			{
				SaveMe = 1;
         	pushstamp(&stampviews[saveptr]);
				if (!juliaflag)
					newjuliaflag = 1;
				juliaflag = 1;
            ruckerflag = 0;
				readcursor();
				uvflag = 1;
			}
			Frozen = 0;
      }
		break;

	case XDELETE:	/* delete */
		if (cursorshape == 0)
			dozoom(1 / zoomfactor);
		else
		{
			if (mandelflag)
			{
				juliaflag = 0;
				uvflag = 1;
			}
			else
			{
				SaveMe = 1;
         	pushstamp(&stampviews[saveptr]);
				if (!juliaflag)
					readcursor();	/* select new cubic */
				juliaflag = 0;
				ruckerflag = 0;
				uvflag = 1;
			}
		}
		Frozen = 0;
		break;

	case XCRARROW:
		if (zoomfactor < 0.9)
			zoomfactor *= 1.1;
		if (zoomfactor > 0.9)
			zoomfactor = 0.9;
		if (Frozen)
			updatecursor();
		break;
	case XCLARROW:
		if (zoomfactor > 1.0 / (maxy + 1))
			zoomfactor *= 0.9;
		if (zoomfactor < (1.0 / (maxy + 1)))
			zoomfactor = (1.0 / (maxy + 1));
		if (Frozen)
			updatecursor();
		break;
	case XF1:
      push_main_button(0,true);
      WaitForNothing();
      On();
		helptext("mand.hlp");
      push_main_button(0,false);
      Off();
   	break;
	case XF2:
      push_main_button(1,true);
      WaitForNothing();
		do_files_menu();
      push_main_button(1,false);
		break;

	case XF3:
      push_main_button(2,true);
      WaitForNothing();
		do_type_menu();
      push_main_button(2,false);
		break;

	case XF4:
      push_main_button(3,true);
      WaitForNothing();
		do_params_menu();
      push_main_button(3,false);
		break;
	case XF5:
      push_main_button(4,true);
      WaitForNothing();
		do_colors_menu();
      push_main_button(4,false);
		break;

   case XALT1:
   case XALT2:
   case XALT3:
   case XALT4:
   {
      int zed = keyword - XALT1;
      zed >>= 8;
      do_stamp(zed);
   }
      break;
   case XALT5:
   case XALT6:
   case XALT7:
   case XALT8:
   {
      char tbuf[128];
      int i = (keyword - XALT5) >> 8;
      int ll = last_saved - i;
      if (ll < 0)
         ll += STAMPCOUNT;
      if (savearray[ll])
      {
         sprintf(tbuf,"Stamp info for stamp %d",ll+1);
         stampinfo(tbuf,&stampviews[ll],i);
      }
   }
      break;

   case '+':
      next_preset();
      load_preset();
      break;
   case '-':
      previous_preset();
      load_preset();
      break;

   case ',':
      if (!cycle_delay)
         cycle_delay = 1;
      else if (cycle_delay < 16*1024)
         cycle_delay *= 2;
      break;
   case '.':
      if (cycle_delay)
         cycle_delay /= 2;
      break;


	}
   return uvflag;
}
