#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <dir.h>
#include <dos.h>
#include <setjmp.h>
#include "toy.h"

/*--------------Global Variables-----------*/
unsigned char catype = 0, caotype = 0, keybreakflag = 0, donescreenflag = 0;
unsigned char maxstate = 31, increment = 5, maxeatstate = 9;
unsigned char nlukyn = 7, nlukyl = 2, nlukyu = 3, nlukyk = 2, nlukyy = 2, nlukyz = 0;
unsigned char tubefuzz = 0, tubealarm = 3, tubelive = 3, tuberest = 4;

int eatmode = 0;

int exitflag = 0;
int newcaflag = 1;
int newcaoflag = 1;
int blankflag = 1;
int randomizeflag = 1;
int rebuildflag = 0;
int allocatefailflag = 0;
int delayfactor = 0;
int wrapflag = 1;
int blankmode = 0xF;		/* which strips to blank, bit0 strip 1, etc. */
int randomizemode = 0xF;	/* which strips to randomize */
int seatype = 0;
int skipper = 0;
int palettenumber = 1;
int grayflag;
int spinflag;
int vgaflag;
int changeflag = 0;
int hasVGA;
rect sR;
int FontHeight;
int StringWidthX;
int init_mem_err = 0;
int prog_init = 0;
int spinspeed = 0;
int iteration = 0;

DISPLAY_MODE display_mode = MED;

int fastflag = 0;

int stopped = 0;
int onestep = 0;

/*-----------------------External Function Declarations---------*/
metaPort *thePort;

/*-----------------------Main Function-------------*/

#pragma warn -ucp    /* I don't care about signed/unsigned fuckups */


rect DisplayRect = {FIRSTXPIXEL, 0, 639, 349};
rect ButtonRect = {0, 0, FIRSTXPIXEL - 1, 349};
rect BottomRect = {FIRSTXPIXEL, 340, 639, 349};


int Comm;

int current_main_item = 0;



char *typenames[] = {
	"Hodge",
	"NLUKY",
	"Eat",
	"Tube"
};


void setOurBitmap(void)
{
	HideCursor();
	SetDisplay(display_page ? GrafPg1 : GrafPg0);
	SetBitmap(display_page ? GrafPg1 : GrafPg0);
	ShowCursor();
}
jmp_buf beatit;

int main(int argc, char **argv)
{
	unsigned short grafboard;
	extern int disk_error_handler(int errval, int ax, int bp, int si);
   char *p;

	allocatebuffers();
	if (init_mem_err)
   {
      cprintf("\r\n\r\nSorry, not enough memory to run Toy Universes.\r\n");
      return -1;
   }

	harderr(disk_error_handler);

	/* Find out if we have a VGA or EGA at all. */

	grafboard = QueryGrafix();


	if ((grafboard & 0x200) != 0x200)
	{
		printf("This programs requires EGA capability.\n");
		exit(-1);
	}

	if (grafboard == 0xffff || InitGrafix(-EGA640x350) < 0)
	{
		printf("Metagraphics not installed. Execute the command:\n");
		printf("metashel /i\n");
		printf("and then try again.\n");
		exit(-1);
	}

	vgaflag = -1;

	while (argc > 1)
	{
		if (argv[1][0] == '-')
		{
			switch (argv[1][1])
			{
			case 'e':
				vgaflag = 0;
				break;
         case 'v':
	    vgaflag = 1;
	    break;
			}
		}
		argc--;
		argv++;
	}

	if (vgaflag == -1)
	{
		if ((grafboard & 0x300) == 0x300)
			vgaflag = 1;
		else
			vgaflag = 0;
	}



	Comm = QueryComm();
	if (Comm == MsDriver)
		InitMouse(MsDriver);
	else if (Comm == MsCOM1)
		InitMouse(MsCOM1);
	else if (Comm == MsCOM2)
		InitMouse(MsCOM2);
	else if (Comm & 3)
		InitMouse(COM1);
	/*
	 * Probably wrong. Need to check for MS mouse address in some special
	 * way.
	 */


	randomize();


   p = searchpath("system16.fnt");
   if (p)
      LoadFont(p);
	installmode();

	load_preset_palettes();

	usepalette();

	current_main_item = 0;

	usepalette();
   TWICE(initialize_buttons());

	ShowCursor();
   if (allocatefailflag)
      ErrorBox("Not enough memory for hi-res.");

   /* Lets see if there is enough for a later gif */
   if (!memok(20712L))		    /* Added up mallocs in comprs.c */
           ErrorBox("There may not be enough memory to save or view a Gif.");


   prog_init = 1;

   if (!setjmp(beatit))
   {

	   while (!exitflag)
	   {

		   rebuildflag = 0;

		   if (newcaflag && !donescreenflag)
		   {
			   loadlookuptable(increment, maxstate);
			   newcaflag = 0;
		   }

		   if (newcaoflag)
		   {
	    unsigned char *p1,*p2;
	    static int firsttime = true;

	    switch(caotype)
	    {
	    case CA_HODGE:
	       p1 = (char *)HODGE_colortable;
	       p2 = HODGE_ct;
	       break;
	    case CA_EAT:
	       p1 = (char *)EAT_colortable;
	       p2 = EAT_ct;
	       break;
	    case CA_TUBE:
	       p1 = (char *)TUBE_colortable;
	       p2 = TUBE_ct;
	       break;
	    case CA_NLUKY:
	       p1 = (char *)NLUKY_colortable;
	       p2 = NLUKY_ct;
	       break;
	    }
	    memcpy(vgacolortable,p1,16*3);
	    if (!hasVGA)
	       memcpy(egacolortable,p2,16);


			   if (!firsttime)
	    {
	       TWICE(initialize_numbers());
	    }
	    else
	       firsttime = false;
			   usepalette();


			   newcaoflag = 0;
	    current_main_item = -1;
		   }

		   if (blankflag)
		   {
			   blankbuffers();
			   blankflag = 0;
		   }
		   if (randomizeflag)
		   {
			   carandomize();
			   randomizeflag = 0;
		   }
		   while (!exitflag && !rebuildflag)
		   {

			   if (onestep || !stopped)
			   {
				   if (display_mode == HI)
					   hiresupdate();
				   else
					   loresupdate();
				   if (onestep)
					   onestep--;

			   }
			   if (spinflag && (!stopped || (iteration++ > spinspeed)))
			   {
				   if (spinflag == 1)
					   spinpalette();
				   else
					   revspinpalette();
               iteration = 0;
			   }
			   checkkeyboard();

			   if (newcaflag)
				   rebuildflag = 1;
		   }
	   }
   }
	StopMouse();
	StopEvent();
	grayflag = 0;
	grayscale();
	SetDisplay(TextPg0);
	return exitflag;

}




#pragma argsused

#pragma warn -use
