//#include "mand.h"

#if defined(FORGE)
#define __COLORS
enum COLORS {
   BLACK,
   LIGHTRED,
   YELLOW,
   LIGHTGREEN,
   LIGHTCYAN,
   LIGHTBLUE,
   LIGHTMAGENTA,
   WHITE,
   DARKGRAY,
   RED,
   BROWN,
   GREEN,
   CYAN,
   BLUE,
   MAGENTA,
   LIGHTGRAY,
};
#endif
   
#include "GRconst.h"
#include "GRports.h"
#include "GRextrn.h"
#include "buttons.h"
#include "scodes.h"

extern int OurWhite;
extern int OurDarkGray;


#ifndef VERSION
#define VERSION 0.123
#endif
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "Dummy, name this program"
#endif

extern rect sR;
extern unsigned long realfarcoreleft(void);
extern int FontHeight;
extern void BasicCenteredBox(rect *,int,int,int,char *,int);
extern void Centers(rect *,int *,int *);
extern void PopRect(int *);

static char *infomsgs[] = {
   "James Gleick's CHAOS: The Software",
	"\x1d Copyright 1990 by Autodesk, Inc.",
   "All Rights Reserved.",
	"Programmed by:",
	"Joshua Gordon, Rudy Rucker, and John Walker",
	NULL
};


void InfoBox(void)
{
	rect tR;
	int row;
	int width = 2 * sR.Xmax / 3;
	int height;
	int cx, cy;
	char tbuf[128];
	int i;
	long l = realfarcoreleft();

	for (i = 0; infomsgs[i]; i++);

	height = (i + 5) * FontHeight + FontHeight;

	HideCursor();
   strcpy(tbuf,PROGRAM_NAME);
   for(i=0;tbuf[i];i++)
      if (tbuf[i] == '_')
         tbuf[i] = ' ';

	BasicCenteredBox(&tR, width, height, OurDarkGray, tbuf, OurWhite);
	Centers(&tR, &cx, &cy);

	row = tR.Ymin + FontHeight + 8;
	TextAlign(alignCenter, alignTop);
	MoveTo(cx, row);
	PenColor(OurWhite);
	BackColor(OurDarkGray);

#ifdef RELEASE
   sprintf(tbuf, "Version 1.00");
#else   
   sprintf(tbuf, "Version %3.2f " __DATE__, VERSION);
#endif
	DrawString(tbuf);

	row += FontHeight;
	for (i = 0; infomsgs[i]; i++, row += FontHeight)
	{
		MoveTo(cx, row);
		DrawString(infomsgs[i]);
	}

	row += FontHeight;
	sprintf(tbuf, "%ld bytes free", l);
	MoveTo(cx, row);
	DrawString(tbuf);

	row += FontHeight;
	MoveTo(cx, row);
	DrawString("Press any key or click to continue");

	while (1)
	{
		event e;

		if (KeyEvent(false, &e))
			break;
	}

	PopRect(&i);
	ShowCursor();
}

