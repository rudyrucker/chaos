#include "toy.h"
#include <setjmp.h>
extern jmp_buf beatit;

static void process_insert_key(void)
{

	switch (display_mode)
	{
	case HI:
		display_mode = MED;
		hitomed();
		break;
	case MED:
		display_mode = DOUBLE;
		medtomiddle();
		break;
	case DOUBLE:
		display_mode = COARSE;
		middletocoarse();
		break;
	}
}

static void process_delete_key(void)
{
	switch (display_mode)
	{
	case COARSE:
		display_mode = DOUBLE;
		coarsetomiddle();
		break;
	case DOUBLE:
		display_mode = MED;
		middletomed();
		break;
	case MED:
		if (!allocatefailflag)
      {
         display_mode = HI;
   		medtohi();
      }
		break;
	}
}

static void process_quit_button(char *text,int retval)
{
   char tbuf[128];

   TWICE(PaintQuitButton(true));
   WaitForNothing();
   sprintf(tbuf,"%s: Are you sure?",text);
	if (cancel_ok_msg(tbuf))
		exitflag = retval;
   TWICE(PaintQuitButton(false));
   current_main_item = -1;
   if (exitflag)
      longjmp(beatit,retval);
}


void key_push(int n,int inout)
{
   rect *R = mainR[n];

   if (n != -1)
   {
      if (n <= 5)
      {
         PushButton(R,inout);
         if (n == 5)
            ExtraHilite(R,inout);
      }
      else if (n >= main_items - 3)
      {
         if (n < main_items - 1)
            PushOrDoublePress(R,inout,n - (main_items - 3) == stopped);
         else
         PushButton(R,inout);
      }
      else
      {
         int ii = false;
         text_button *t;
         int doit = false;
         
         n -= 6;
         switch(caotype)
         {
         case CA_HODGE:
         case CA_NLUKY:
            if ((n % 3) == 0)
            {
               ii = true;
               t = &tweakNT[n/3].TB;
            }
            else
               doit = true;
            break;
         case CA_EAT:
            if (n <= 1)
               PushOrDoublePress(R,inout,n == eatmode);
            else
            {
               if (n == 2)
               {
                  ii = true;
                  t = &tweakNT[1].TB;
               }
               else
                  doit = true;
            }
            break;
         case CA_TUBE:
            if (n <= 8)
            {
               if ((n % 3) == 0)
               {
                  ii = true;
                  t = &tweakNT[n/3].TB;
               }
               else
                  doit = true;
            }
            else
               PushOrDoublePress(R,inout,n - 9 == tubefuzz);
            break;
         }

         if (doit)
            PushButton(R,inout);

         if (ii)
            InvertInsides(t);
      }

   }
}




static int ProcessTweakButton(int i)
{
   extern int process_hodge(int);
   extern int process_tube(int);
   extern int process_eat(int);
   extern int process_nluky(int);

   switch(caotype)
   {
   case CA_HODGE:
      return process_hodge(i);
   case CA_TUBE:
      return process_tube(i);
   case CA_EAT:
      return process_eat(i);
   case CA_NLUKY:
      return process_nluky(i);
   }
   return 0;
}

void check_cursor_movement(int X, int Y)
{
   int i;
   current_main_item = -1;
   for(i=0;i<main_items;i++)
   {
      if (XYInRect(X,Y,mainR[i]))
      {
         current_main_item = i;
         break;
      }
   }
}

void check_cursor_movement_and_update(void)
{
	int last_main_item = current_main_item;
	event e;

	PeekEvent(1, &e);
	check_cursor_movement(e.CursorX, e.CursorY);

	if (last_main_item != current_main_item)
	{
      TWICE(key_push(last_main_item,false));
      TWICE(key_push(current_main_item,true));
	}
}


void checkkeyboard(void)
{
	event e;
	int n;
	unsigned keyword, i;
	int X, Y;
   char tbuf[128];
   extern void  process_hodge_key(int);
   extern void  process_nluky_key(int);
   extern void  process_eat_key(int);
   extern void  process_tube_key(int);


	n = KeyEvent(false, &e);
	X = e.CursorX;
	Y = e.CursorY;
	if (!n)
		check_cursor_movement_and_update();

	while (n)
	{
   	int last_main_item = current_main_item;
		keyword = 0;
		if (e.ASCII && e.ASCII != 0xe0)
			keyword = e.ASCII;
		else if (e.ScanCode)
			keyword = e.ScanCode << 8;
		else if ((e.State >> 8) == (swRight | swLeft))
		{
			if (stopped)
				keyword = 0x0D;
		}
		else if ((e.State >> 8) == swRight)
		{
			if (XYInRect(X, Y, &DisplayRect))
				keyword = XDELETE;
         else if (XYInRect(X, Y, mainR[2]))
				keyword = 'T';

		}
		else if ((e.State >> 8) == swLeft)
		{

			/* Translate mouse positions if possible into keyword */

			/*
			 * If he clicks on the display rect, left is same as
			 * INS, right is same as DEL.
			 */

			if (XYInRect(X, Y, &DisplayRect))
				keyword = XINSERT;
         else
            keyword = 0x0d;




		}

      // PROCESS +- KEYS!!

		if (keyword == 0x0d)
		{

         for(i=0;i<main_items;i++)
         {
            if (XYInRect(X,Y,mainR[i]))
            {
               if (i < 5)
               {
                  keyword = XF1 + 0x100 * i;
                  break;
               }
               else if (i==5)
               {
                  keyword = XALTX;
                  break;
               }
               else if (i == main_items - 1)
               {
                  if (!stopped)
                  {
                     stopped = 1;
                     TWICE(setStopSign(1));
                  }
                  else
                     onestep++;
                  break;
               }
               else if (i == main_items - 3)
               {
                  stopped = 0;
                  TWICE(setStopSign(0));
                  current_main_item = -1;
                  break;
               }
               else if (i == main_items - 2)
               {
                  stopped = 1;
                  TWICE(setStopSign(1));
                  current_main_item = -1;
                  break;
               }
               else
                  keyword = ProcessTweakButton(i-6);
            }
         }

		}


		if (keyword == 'o' || keyword == 'O' || keyword == XF4)
			keyword = options_panel();


      navigate(keyword,main_lefters,main_righters,main_uppers,main_downers,
         main_items,mainR,&current_main_item);


		/* First, the keywords that are the same for everyone. */
		if (keyword)
      {
			switch (keyword)
			{
         case XALT1:
         case XALT2:
         case XALT3:
         case XALT4:
            canned_image = (keyword-XALT1) >> 8;
            load_canned_shape(true);
            break;
         case XALT5:
         case XALT6:
         case XALT7:
         case XALT8:
            canned_image = (keyword-XALT5) >> 8;
            load_canned_shape(false);
            break;

			case XALTA:	/* Preset Palette */
				changeVGApalette();
            break;

			case XALTB:	/* Random Palette */
            randompalette();
				break;

			case XALTD:	/* Default Palette */
            setdefaultpalette();
				break;

			case XALTF:	/* View GIF file */
				FLoadGif();
				break;

			case XALTG:	/* GIF Save Image Area */
				if (select_file("Save image as GIF file", "*.GIF", tbuf, "GIF") && Overwrite(tbuf))
					GifOutput(tbuf, 0);
				break;

			case XALTH:	/* GIF Save Entire Screen */
				if (select_file("Save screen as GIF file", "*.GIF", tbuf, "GIF") && Overwrite(tbuf))
					GifOutput(tbuf, 1);
				break;

			case XALTJ:	/* Cycle Palette once forward */
				spinpalette();
				break;

			case XALTK:	/* Cycle Palette once reverse */
				revspinpalette();
				break;

			case XALTL:	/* Load Parameters */
				if (select_file("Load Parameters", "*.TOY", tbuf, "TOY"))
					LoadParameters(tbuf);
				break;

			case XALTM:	/* Monochrome Toggle */
				grayflag ^= 1;
				grayscale();
				usepalette();
				break;

			case XALTP:	/* Palette Editor */
				palette_tweaker();
				break;

			case XALTQ:	/* Quit */
            process_quit_button("Quit to DOS",2);
				break;


			case XALTS:	/* Save Parameters */
				if (select_file("Save Parameters", "*.TOY", tbuf, "TOY") && Overwrite(tbuf))
					SaveParameters(tbuf);
				break;

			case XALTX:	/* Exit Program */
            process_quit_button("EXIT",1);
				break;

         case XALTW:
            InfoBox();
            break;

			case XALTY:	/* Color cycle, 3 way toggle */
				spinflag = (++spinflag) % 3;
				break;

         case XALTZ:
            PushCursorPosition();
			   switch(set_boxes())
            {
            case 0:
               break;
            case 1:
			      switch (display_mode)
			      {
			      case MED:
				      display_mode = DOUBLE;
				      medtomiddle();
                  if (stopped)
                     onestep = 1;
				      break;
			      case DOUBLE:
				      display_mode = COARSE;
                  if (stopped)
                     onestep = 1;
				      middletocoarse();
				      break;
			      }
               break;
            case -1:
               process_delete_key();
               if (stopped)
                  onestep = 1;
               break;
            }
            PopCursorPosition();
			   break;


			case XF1:
				PushCursorPosition();
				helptext("toy.hlp");
				LimitMouse(sR.Xmin, sR.Ymin, sR.Xmax, sR.Ymax);
				PopCursorPosition();
				break;


			case ' ':
				if (!stopped)
				{
					stopped = 1;
					TWICE(setStopSign(0));
               if (current_main_item >= main_items - 3)
   					current_main_item = -1;
				}
				else
            {
               /* check to see if the shift keys are depressed */
               if (e.State & 3)
               {
                  stopped = 0;
   					TWICE(setStopSign(0));
                  if (current_main_item >= main_items - 3)
      					current_main_item = -1;
               }
               else
   					onestep++;
            }
				break;

			case 'b':
			case 'B':
				seatype ^= 1;
				break;

			case 'f':	/* Fast Mode Toggle */
			case 'F':
				fastflag = 1 - fastflag;
				break;

			case 'g':
			case 'G':
			case XF7:
				/* Toggle skipper */
				skipper ^= 1;
				break;

			case XINSERT:	/* insert */
				/*
				 * High to medium, medium to double, double
				 * to coarse
				 */
            process_insert_key();
            if (stopped)
               onestep = 1;
				break;

			case XDELETE:	/* delete */
            process_delete_key();
            onestep = 1;
				break;

			case XF2:
				do_files_menu();
				break;

         case XF3:
			case 't':
				{
					static int nexts[] = {CA_EAT, CA_TUBE, CA_NLUKY, CA_HODGE};

					newcaoflag = newcaflag = rebuildflag = 1;
					caotype = nexts[caotype];
               if (stopped)
                  onestep = 1;
				}
				break;

         case XF5:
         case XALTR:	/* Randomize */
				randomizeflag = 1;
				rebuildflag = 1;
				keybreakflag = 0;
            if (stopped)
               onestep = 1;
				break;

			case XF9:
				changeflag ^= 1;
				break;

			
			case 'w':
			case 'W':
				wrapflag ^= 1;
				break;


         /* Case 't' is with XF3 */

			case 'T':
				{
					static int prevs[] = {CA_TUBE, CA_EAT, CA_HODGE, CA_NLUKY};

					newcaoflag = newcaflag = rebuildflag = 1;
					caotype = prevs[caotype];
               if (stopped)
                  onestep = 1;
				}
				break;

         case 'S':
         case 's':
            ShapeLoader();
            if (stopped)
               onestep = 1;
            break;

         case '.':
         case '>':
            spinspeed /= 2;
            break;

         case ',':
         case '<':
            if (spinspeed == 0)
               spinspeed = 1;
            else if (spinspeed < 16*1024)
               spinspeed *= 2;
			}

      }

		switch (caotype)
		{
		case CA_HODGE:
         process_hodge_key(keyword);
         break;
		case CA_NLUKY:
         process_nluky_key(keyword);
         break;
		case (CA_EAT):
         process_eat_key(keyword);
         break;
		case CA_TUBE:
         process_tube_key(keyword);
         break;
		}

		n = KeyEvent(false, &e);
	   if (last_main_item != current_main_item)
	   {
         TWICE(key_push(last_main_item,false));
         TWICE(key_push(current_main_item,true));
	   }
	}


}

