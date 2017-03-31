#pragma warn -sig

#include <alloc.h>
#include <assert.h>
#include "toy.h"


#define SEQUENCER 0x3c5

extern DISPLAY_MODE display_mode;
extern int safe_alloc;



extern int seatype;
extern unsigned char nlukyn, nlukyl, nlukyu, nlukyk, nlukyy;
extern unsigned char maxeatstate;
extern int eatmode;

extern int allocatefailflag;

unsigned char huge *oldbuf;
unsigned char huge *newbuf;
unsigned char huge *egabuf;
unsigned char *egarowptrs[HIYCOUNT + 2];
unsigned char *newbufrowptrs[MEDYCOUNT + 2];
unsigned char *oldbufrowptrs[MEDYCOUNT + 2];
extern unsigned char tubefuzz, tubealarm, tubelive, tuberest;
extern rect DisplayRect;

unsigned char far *lookuptable;
unsigned char far *fakelookuptable;
extern int wrapflag;
extern int catype;
extern unsigned char caotype;
extern int skipper;
int display_page = 1;
extern int changeflag;

static unsigned char far *screen = (unsigned char far *) 0xa0000000L;
extern int randomizemode;
extern int fastflag;

void normalize_memcpy(unsigned char huge * d, unsigned char huge * s, int count);
void normalize_memset(unsigned char huge * d, int c, int count);
void updaterowlocao0(int row, unsigned char *lut);
void updaterowlocao1(int row, unsigned char *lut);
void updaterowlocao2(int row, unsigned char *lut);
extern void checkkeyboard(void);

void updaterowcao0(unsigned char huge * dest,
		    unsigned char huge * source,
		    int runwidth,
		    int rowwidth);

void updaterowcao1(unsigned char huge * dest,
		    unsigned char huge * source,
		    int runwidth,
		    int rowwidth);

void updaterowcao2(unsigned char huge * dest,
		    unsigned char huge * source,
		    int runwidth,
		    int rowwidth);

void blattbytes(unsigned char huge *, int, int);
void updaterowegacao0(unsigned char *, unsigned char *, unsigned char *);
void updaterowegacao1(int row, unsigned char *lut);
void updaterowegacao2(unsigned char *, unsigned char *, unsigned char *);
void JSetOrigin(int);



unsigned char evenbuff[HIXCOUNT+2];
unsigned char oddbuff[HIXCOUNT+2];
unsigned char *oddeven[2] = {evenbuff, oddbuff};
int evenoddflag = 0;



rect doubledragrect = {
	FIRSTXPIXEL + HIXCOUNT / 4, HIYCOUNT / 4,
	FIRSTXPIXEL + (3 * HIXCOUNT + 3) / 4 - 1, (3 * HIYCOUNT + 3) / 4 - 1
};


/* The "corners" are actually offsets within the medium
   resolution buffers. */

rect doublecorners = {
	MEDXCOUNT / 4, MEDYCOUNT / 4,
	(3 * MEDXCOUNT + 3) / 4 - 1, (3 * MEDYCOUNT + 3) / 4
};

rect coarsecorners = {
	(3 * MEDXCOUNT) / 8, (3 * MEDYCOUNT) / 8,
	(5 * MEDXCOUNT) / 8 - 1, (5 * MEDYCOUNT) / 8 - 1
};

rect coarsedragrect = {
	FIRSTXPIXEL + HIXCOUNT / 4, HIYCOUNT / 4,
	FIRSTXPIXEL + (3 * HIXCOUNT + 3) / 4 - 1, (3 * HIYCOUNT + 3) / 4 - 1
};


/* A table of points on a circle or so; I did this to
avoid loading floating point libraries */


int modechanged = 0;




void *normalize(void huge * ptr,
		 unsigned short front, unsigned short end)
{
	/* Normalize so that ptr > front and smaller than -end */

	unsigned seg = FP_SEG(ptr);
	unsigned off = FP_OFF(ptr);



	end = 0x10000L - end;

	while (off < front)
	{
		seg--;
		off += 16;
	}
	while (off > end)
	{
		seg++;
		off -= 16;
	}


	assert(seg * 16L + off == FP_SEG(ptr) * 16L + FP_OFF(ptr));

	return MK_FP(seg, off);
}



void translate_cao1(unsigned char *dest, unsigned char *src, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (*src <= 2)
			*dest++ = *src++;
		else
			*dest++ = *src++ / 2 + 1;
	}
}




void blankbuffers(void)
{
	long i;

	/* Just fill 'em all with zeroes */

	if (!allocatefailflag)
	{
		for (i = 0; i < HIYCOUNT + 2; i++)
			memset(egarowptrs[i], 0, HIXWIDTH);
	}
	else
	{
		for (i = 0; i < MEDYCOUNT + 2; i++)
		{
			memset(oldbufrowptrs[i], 0, MEDXCOUNT + 2);
			memset(newbufrowptrs[i], 0, MEDXCOUNT + 2);
		}
	}
}

void carandomize(void)
{
	long row;

	if (display_mode == HI)
	{
		for (row = 0; row < HIYCOUNT; row++)
		{
			ca_randomize(egarowptrs[row], HIXCOUNT + 2);
		}
	}
	else
	{
		for (row = 0; row < MEDYCOUNT; row++)
			ca_randomize(oldbufrowptrs[row], MEDXCOUNT + 2);
	}

}

void codeinfo(void)
{
}

void hitomed(void)
{
	long i;
	int front = (HIXCOUNT - MEDXCOUNT) / 2;



	/* Take the stuff that was in the middle... */
	for (i = 0; i < MEDYCOUNT + 2; i++)
	{
		memcpy(
		       oldbufrowptrs[i],
		       &egarowptrs[HIYCOUNT / 4 + i][front],
		       MEDXCOUNT + 2);
	}
	modechanged = true;


}


void medtohi(void)
{
	long i;
	int front = (HIXCOUNT - MEDXCOUNT) / 2;
	unsigned char *savebuf=NULL;
   
	/* First, move the stuff from newbuf down into egabuf properly. */

	/* Just for now, alloc an oldbuf full; we should have room. */
	/* Just for now, alloc an oldbuf full; we should have room. */
	if (memok((MEDXCOUNT + 2) * (MEDYCOUNT + 2)))
   {
	    safe_alloc = 1;
	    savebuf = farcalloc(MEDXCOUNT + 2, MEDYCOUNT + 2);
	}

   if (!savebuf)
   {
      ErrorBox("Not enough memory for hi-res.");
      allocatefailflag = true;
      display_mode = MED;
      return;
   }

	for (i = 0; i < MEDYCOUNT + 2; i++)
	{
		memcpy(savebuf + i * (MEDXCOUNT + 2),
		       oldbufrowptrs[i],
		       MEDXCOUNT + 2);
	}


	/*
	 * Randomize (or blank) the first 1/4 and the last 1/4 of the screen,
	 * top bottom left right.
	 */

	for (i = 0; i < HIYCOUNT+2; i++)
	{
      extern char onerandom(void);
		if (seatype)
      {
         int j;
         for(j=0;j<HIXCOUNT+2;j++)
            egarowptrs[i][j] = onerandom();
      }
//			ca_randomize(egarowptrs[i], HIXCOUNT+2);
		else
			memset(egarowptrs[i], 0, HIXCOUNT + 2);
	}

	/* Now drop the saved buffer into place */
	for (i = 0; i < MEDYCOUNT + 2; i++)
		memcpy(egarowptrs[HIYCOUNT / 4 + i] + front, savebuf + i * (MEDXCOUNT + 2), MEDXCOUNT + 2);
	free(savebuf);
	modechanged = true;
}


void medtomiddle(void)
{
	/*
	 * Going from medium to middle resolution. Don't need anything here?
	 * Wrong. Clear both pages.
	 */
	HideCursor();
	/* Clear the one we AREN'T displaying first... */
	SetBitmap(display_page ? GrafPg1 : GrafPg0);
	BackColor(BLACK);
	EraseRect(&DisplayRect);
	SetDisplay(display_page ? GrafPg1 : GrafPg0);
	SetBitmap(display_page ? GrafPg0 : GrafPg1);
	EraseRect(&DisplayRect);

	display_page = 1;
	SetBitmap(GrafPg0);
	SetDisplay(GrafPg0);
	ShowCursor();
}

void middletocoarse(void)
{
	modechanged = true;
	medtomiddle();
}

void coarsetomiddle(void)
{
	medtomiddle();
}

void middletomed(void)
{
	medtomiddle();
}






#pragma warn -use
