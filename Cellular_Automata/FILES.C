/*
      (C) Copyright 1990 by Autodesk, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <dir.h>
#include <dos.h>
#include <io.h>

#include "gif.h"
#include "toy.h"

extern int GifDisplay(char *filename);
extern void initialize_buttons(void);

/* A little files menu. Save and load parameters. Hotcha. Also make
a GIF. Maybe view one if there is room. */

static rect itemrects[10];
static rect *bRp[10];
static int items;


static void push(int item,int inout)
{
   if (item != -1)
   {
      PushButton(bRp[item],inout);
      if (item == items - 1)
	 ExtraHilite(bRp[item],inout);
   }
}

void SaveParameters(char *filename)
{
	FILE *fd = fopen(filename, "wt");
	time_t timex;
	int i;
   int ok = true;

	if (!fd)
   {
      FileError(filename,NULL);
      return;
   }

	ok &= EOF != fprintf(fd, "CHAOS Toy Universe Data\n");
	timex = time(NULL);
	ok &= EOF != fprintf(fd, "Created: %s", ctime(&timex));
	ok &= EOF != fprintf(fd, "Type: %s\n", typenames[caotype]);


	switch (caotype)
	{
	case CA_HODGE:
		ok &= EOF != fprintf(fd, "Number: %d\n", maxstate);
		ok &= EOF != fprintf(fd, "Increment: %d\n", increment);
		break;
	case CA_TUBE:
		ok &= EOF != fprintf(fd, "Hiding: %d\n", tuberest);
		ok &= EOF != fprintf(fd, "Eating: %d\n", tubelive);
		ok &= EOF != fprintf(fd, "Alarm: %d\n", tubealarm);
		ok &= EOF != fprintf(fd, "Jazz: %d\n", tubefuzz);
		break;
	case CA_NLUKY:
		for (i = 0; i < 5; i++)
			ok &= EOF != fprintf(fd, "%c: %d\n", "NLUKY"[i], *nluky_parms[i]);
		break;
	case CA_EAT:
		ok &= EOF != fprintf(fd, "Eatmode: %d\n", eatmode);
		ok &= EOF != fprintf(fd, "MaxEat: %d\n", maxeatstate);
		break;
	}
   ok &= EOF != fflush(fd);

   if (!ok || ferror(fd))
   {
      FileError(filename,fd);
      fclose(fd);
      remove(filename);
   }
   else
   	fclose(fd);
}

int get_and_strip(char *tbuf, FILE * fd)
{
	fgets(tbuf, 128, fd);
   if (!feof(fd) && strlen(tbuf))
   	tbuf[strlen(tbuf) - 1] = 0;
	return feof(fd);
}


void files_load_hodge(FILE * fd)
{
	char tbuf[128];
	int t;

	while (1)
	{
		if (get_and_strip(tbuf, fd))
			break;

		if (sscanf(tbuf, "Increment: %d", &t))
			increment = t;
		else if (sscanf(tbuf, "Number: %d", &t))
			maxstate = t;
	}
	caotype = CA_HODGE;
}

void files_load_tube(FILE * fd)
{
	char tbuf[128];
	int t;

	while (1)
	{
		if (get_and_strip(tbuf, fd))
			break;

		if (sscanf(tbuf, "Hiding: %d", &t))
			tuberest = t;
		else if (sscanf(tbuf, "Eating: %d", &t))
			tubelive = t;
		else if (sscanf(tbuf, "Alarm: %d", &t))
			tubealarm = t;
		else if (sscanf(tbuf, "Jazz: %d", &t))
			tubefuzz = t;
	}
	caotype = CA_TUBE;
}

void files_load_nluky(FILE * fd)
{
	char tbuf[128];
	int t;

	while (1)
	{
		if (get_and_strip(tbuf, fd))
			break;

		if (sscanf(tbuf, "N: %d", &t))
			nlukyn = t;
		if (sscanf(tbuf, "L: %d", &t))
			nlukyl = t;
		if (sscanf(tbuf, "U: %d", &t))
			nlukyu = t;
		if (sscanf(tbuf, "K: %d", &t))
			nlukyk = t;
		if (sscanf(tbuf, "Y: %d", &t))
			nlukyy = t;
	}
	caotype = CA_NLUKY;
}

void files_load_eat(FILE * fd)
{
	char tbuf[128];
	int t;

	while (1)
	{
		if (get_and_strip(tbuf, fd))
			break;

		if (sscanf(tbuf, "Eatmode: %d", &t))
			eatmode = t;
		else if (sscanf(tbuf, "MaxEat: %d", &t))
			maxeatstate = t;
	}
	caotype = CA_EAT;
}




void LoadParameters(char *filename)
{
	FILE *fd = fopen(filename, "rt");
	char tbuf[128];
	char sbuf[128];

	if (!fd)
   {
      FileError(filename,NULL);
		return;
   }

	while (1)
	{
		if (get_and_strip(tbuf, fd))
			break;

		if (sscanf(tbuf, "Type: %s", sbuf))
		{
			switch (sbuf[0])
			{
			case 'H':
				files_load_hodge(fd);
				break;
			case 'E':
				files_load_eat(fd);
				break;
			case 'T':
				files_load_tube(fd);
				break;
			case 'N':
				files_load_nluky(fd);
				break;
			}
		}
	}
	newcaoflag = newcaflag = rebuildflag = 1;
	fclose(fd);
}
void FLoadGif(void)
{
	int found = 0;
	int mem_err = 0;
   char chosen_file[128];
   event e;

   HideCursor();
   SetBitmap(GrafPg0);
   SetDisplay(GrafPg0);
   ShowCursor();

	while (!mem_err &&
	       select_file("View GIF image", "*.gif", chosen_file, "GIF"))
	{
      union REGS regs;

      if (access(chosen_file,0))
      {
   	 FileError(chosen_file,NULL);
   	 continue;
      }
      HideCursor();
		BackColor(BLACK);
		EraseRect(&sR);
		if (GifDisplay(chosen_file) == GIF_COOLMAN) {
		    sound(110);
		    delay(10);
		    nosound();
		    while (1)
		    {
			KeyEvent(true, &e);
			if ((e.State >> 8) & 7)
				break;
			if (e.ASCII || e.ScanCode)
				break;
		    }
		}
		else
			mem_err = 1;
      found = 1;

      regs.h.ah = 0;
      regs.h.al = 0x10;
      int86(0x10,&regs,&regs);
      usepalette();
      ShowCursor();
	}

   if (found)
   {
      /* restore the screen to how we like it */
      union REGS regs;
      HideCursor();
      regs.h.ah = 0;
      regs.h.al = 0x10;
      int86(0x10,&regs,&regs);
      TWICE(initialize_buttons());
      usepalette();
      ShowCursor();
   }
}

void do_files_menu(void)
{
   int width=0;
   int height = 7 * (3 * FontHeight/2) + FontHeight * 2 + 8;
   int i;
   int current_item;
   int doit = false;
   rect tR,R;
   rect bR[10];

   static char *msgs[] = {
      "Save Parameters    (Alt-S)",
      "Load Parameters    (Alt-L)",
      "Save image as GIF  (Alt-G)",
      "Save screen as GIF (Alt-H)",
      "View GIF image     (Alt-F)",
      "F1 for HELP",
      "ESC to Cancel",
	 NULL
      };

   static int filekeys[] = {XALTS,XALTL,XALTG,XALTH,XALTF};

   for(i=0;msgs[i];i++)
      width = max(width,StringWidth(msgs[i]) + 24);
   items = i;

   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"File Menu",BLACK);
   ArrowCursor();
   ProtectOff();

   for(i=0;msgs[i];i++)
   {
      R.Xmin = tR.Xmin + 4;
      R.Xmax = tR.Xmax - 4;
      R.Ymin = tR.Ymin + FontHeight + 4 + i * (3*FontHeight/2);
      if (i >= items - 2)
	 R.Ymin += 8;
      R.Ymax = R.Ymin + FontHeight + 4;

      bR[i] = R;
      bRp[i] = &bR[i];

      PaintRadioButtonBase(&R,false,false,msgs[i],DARKGRAY,RED,WHITE);
   }

   LimitMouseRect(&tR);
   current_item = items - 1;
   move_to_corner(bRp[current_item]);
   push(current_item,true);
   ShowCursor();
   LimitMouseRect(&tR);

   while(!doit)
   {
      event e;
      int n;
      int X,Y;
      int button;
      int key = 0;
      int last_item = current_item;

      n = KeyEvent(false,&e);
      X = e.CursorX;
      Y = e.CursorY;

      button = (e.State & 0x700) >> 8;


      current_item = -1;
      for(i=0;i<items;i++)
      {
         if (XYInRect(X,Y,bRp[i]))
         {
            current_item = i;
            break;
         }
      }


      if (n)
      {
	 if (e.ASCII && e.ASCII != 0xe0)
	    key = e.ASCII;
	 else if (e.ScanCode != 0xff)
	    key = e.ScanCode << 8;

	 if (button == swRight)
	    key = 0x1b;

	 if (button == swLeft)
	    key = 0x0d;
      }

      for(i=0;i<5;i++)
      {
	 if (filekeys[i] == key)
	 {
	    current_item = i;
	    key = 0x0d;
	    break;
	 }
      }

      navigate(key,NULL,NULL,(int *)-1,(int *)-1,items,bRp,&current_item);
      if (key == 0x1b)
	 current_item = items - 1;

      if (last_item != current_item)
      {
	 push(last_item,false);
	 push(current_item,true);
      }

      if (key == 0x1b)
	 break;

      if (key == XF1 || (key == 0x0d && current_item == items - 2))
      {
			helptext("TOYFILE.HLP");
	 continue;
      }


      if (key == 0x0d && current_item != -1)
      {
	 doit = true;
	 break;
      }


   }


   PaintRadioButtonBase(bRp[current_item],true,true,msgs[current_item],DARKGRAY,RED,WHITE);
   if (current_item == items - 1)
      ExtraHilite(bRp[items - 1],true);
   DoublePress(bRp[current_item],true,RED);
   WaitForNothing();
   PaintRadioButtonBase(bRp[current_item],false,false,msgs[current_item],DARKGRAY,RED,WHITE);
   if (current_item == items - 1)
      ExtraHilite(bRp[items - 1],false);


   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   LimitMouseRect(&sR);
   ShowCursor();

   if (doit)
   {
      char chosen[128];

      switch(current_item)
      {
      case 0:
			if (select_file("Save Parameters", "*.TOY", chosen, "TOY") && Overwrite(chosen))
				SaveParameters(chosen);
			break;
      case 1:
			if (select_file("Load Parameters", "*.TOY", chosen, "TOY"))
				LoadParameters(chosen);
			break;
      case 2:
			if (select_file("Save image as GIF file", "*.GIF", chosen, "GIF") && Overwrite(chosen))
				GifOutput(chosen, 0);
	 break;
      case 3:
			if (select_file("Save screen as GIF file", "*.GIF", chosen, "GIF") && Overwrite(chosen))
				GifOutput(chosen, 1);
			break;
      case 4:
	 FLoadGif();
	 break;
      }
   }

}


#pragma warn -use
