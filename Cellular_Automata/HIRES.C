/* Stuff only needed for HIRES */
#include "toy.h"
extern int evenoddflag;
extern int modechanged;
extern unsigned char *oddeven[2];
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
void translate_cao1(unsigned char *dest, unsigned char *src, int count);
void blattbytes(unsigned char huge *, int, int);

void updateega(void)
{
	int row;
	int i;

	evenoddflag = 0;
	modechanged = false;

	for (i = 0, row = 1; i < HIYCOUNT && !modechanged; row++, i++)
	{
		switch (caotype)
		{
		case CA_HODGE:
			updaterowcao0(
				      oddeven[evenoddflag],
				      egarowptrs[row] + 1,
				      HIXCOUNT, HIXCOUNT + 2);
			break;
		case CA_TUBE:
		case CA_NLUKY:
			updaterowcao1(
				      oddeven[evenoddflag],
				      egarowptrs[row] + 1,
				      HIXCOUNT, HIXCOUNT + 2);
			break;
		case CA_EAT:
			updaterowcao2(
				      oddeven[evenoddflag],
				      egarowptrs[row] + 1,
				      HIXCOUNT, HIXCOUNT + 2);
			break;

		}
		memcpy(egarowptrs[row - 1] + 1, oddeven[1 - evenoddflag], HIXCOUNT + 2);
		evenoddflag = 1 - evenoddflag;
		if (!changeflag)
			checkkeyboard();
		else
			check_cursor_movement_and_update();
	}

	/* Still have one more to put in? */


	memcpy(egarowptrs[row - 1] + 1, oddeven[1 - evenoddflag], HIXCOUNT + 2);
	display_hi();
	HideCursor();
	/* Now flip to the page we just painted */
	SetDisplay(display_page ? GrafPg1 : GrafPg0);
	SetBitmap(display_page ? GrafPg1 : GrafPg0);
	ShowCursor();
	display_page = 1 - display_page;

}



void hiresupdate(void)
{

	int i;

	/* Wrap around... */
	if (wrapflag)
	{
		memcpy(egarowptrs[0], egarowptrs[HIYCOUNT], HIXCOUNT + 2);
		memcpy(egarowptrs[HIYCOUNT + 1], egarowptrs[1], HIXCOUNT + 2);

		for (i = 0; i < HIYCOUNT + 2; i++)
		{
			egarowptrs[i][0] = egarowptrs[i][HIXCOUNT];
			egarowptrs[i][HIXCOUNT + 1] = egarowptrs[i][1];
		}
	}


	updateega();

}

void display_hi(void)
{
	int i;

	for (i = 0; i < HIYCOUNT; i++)
	{
		if (caotype == CA_NLUKY)
		{
			unsigned char buffer[HIXCOUNT + 20];

			translate_cao1(buffer, egarowptrs[i + 1] + 1, HIXCOUNT);
			blattbytes(buffer, i, display_page);
		}
		else
			blattbytes(egarowptrs[i + 1] + 1, i, display_page);
	}
}
