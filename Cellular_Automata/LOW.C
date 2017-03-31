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
extern rect coarsecorners;
extern rect doublecorners;
void doubleblattbuffer(unsigned char *, int, int, int);

void display_double(void)
{
	int firstrow, firstcol;
	int row, i;

	firstrow = doublecorners.Ymin;
	firstcol = doublecorners.Xmin;


	for (row = firstrow, i = 0; i < MIDDLEYCOUNT; i++, row++)
	{
		if (caotype == CA_NLUKY)
		{
			unsigned char buffer[MIDDLEXCOUNT];

			translate_cao1(buffer, newbufrowptrs[row] + firstcol, MIDDLEXCOUNT);
			doubleblattbuffer(buffer, i, display_page, skipper);
		}
		else
			doubleblattbuffer(newbufrowptrs[row] + firstcol, i, display_page, skipper);
	}
}


void loresupdate(void)
{
	int i;
	int row;
	unsigned char *tmpfp;
	int lastrow, lastcol;
	int firstrow, firstcol;
	int cols, rows;


	/*
	 * firstrow is the first DISPLAYED row, firstcol is the first
	 * DISPLAYED col
	 */

	if (display_mode == MED || !fastflag)
	{
		firstrow = 1;
		firstcol = 1;
		rows = MEDYCOUNT;
		cols = MEDXCOUNT;
	}
	else if (display_mode == DOUBLE)
	{
		firstrow = doublecorners.Ymin;
		firstcol = doublecorners.Xmin;
		rows = MIDDLEYCOUNT;
		cols = MIDDLEXCOUNT;
	}
	else
	{
		firstrow = coarsecorners.Ymin;
		firstcol = coarsecorners.Xmin;
		rows = LOYCOUNT;
		cols = LOXCOUNT;
	}
	lastrow = firstrow + rows - 1;
	lastcol = firstcol + cols - 1;


	if (wrapflag)
	{
		/* copy last visible to one before first visible */
		memcpy(oldbufrowptrs[firstrow - 1] + firstcol - 1,
		       oldbufrowptrs[lastrow] + firstcol - 1,
		       cols + 2);

		/* copy first visible to one after last visible */
		memcpy(oldbufrowptrs[lastrow + 1] + firstcol - 1,
		       oldbufrowptrs[firstrow] + firstcol - 1,
		       cols + 2);

		for (i = 0, row = firstrow - 1; i < rows + 2; i++, row++)
		{
			oldbufrowptrs[row][firstcol - 1] = oldbufrowptrs[row][lastcol];
			oldbufrowptrs[row][lastcol + 1] = oldbufrowptrs[row][firstcol];
		}

	}

	/*
	 * So, if we are not running "fastflag", do the whole medium res
	 * yahootie.
	 */
	modechanged = 0;
	if ((!fastflag) || display_mode == MED)
	{
		for (row = 1; row < MEDYCOUNT + 1 && !modechanged; row++)
		{
			switch (caotype)
			{
			case CA_HODGE:
				updaterowcao0(
					      newbufrowptrs[row] + 1,
					      oldbufrowptrs[row] + 1,
					      MEDXCOUNT, MEDXCOUNT + 2);
				break;
			case CA_NLUKY:
			case CA_TUBE:
				updaterowcao1(
					      newbufrowptrs[row] + 1,
					      oldbufrowptrs[row] + 1,
					      MEDXCOUNT, MEDXCOUNT + 2);
				break;
			case CA_EAT:
				updaterowcao2(
					      newbufrowptrs[row] + 1,
					      oldbufrowptrs[row] + 1,
					      MEDXCOUNT, MEDXCOUNT + 2);
				break;
			case 99:
				memset(newbufrowptrs[row] + 1,
				       (row % 15) + 1, MEDXCOUNT + 2);
				break;
			}
			if (!changeflag)
				checkkeyboard();
			else
				check_cursor_movement_and_update();
		}
	}
	else
	{
		/*
		 * If we are running fast, we just do the things in place,
		 * but we still use the same locations.
		 */
		for (i = 0, row = firstrow; i < rows && !modechanged; row++, i++)
		{
			switch (caotype)
			{
			case CA_HODGE:
				updaterowcao0(
					      newbufrowptrs[row] + firstcol,
					      oldbufrowptrs[row] + firstcol,
					      cols, MEDXCOUNT + 2);
				break;
			case CA_TUBE:
			case CA_NLUKY:
				updaterowcao1(
					      newbufrowptrs[row] + firstcol,
					      oldbufrowptrs[row] + firstcol,
					      cols, MEDXCOUNT + 2);
				break;
			case CA_EAT:
				updaterowcao2(
					      newbufrowptrs[row] + firstcol,
					      oldbufrowptrs[row] + firstcol,
					      cols, MEDXCOUNT + 2);
				break;
			case 99:
				memset(newbufrowptrs[row] + firstcol,
				       (row % 15) + 1, MEDXCOUNT + 2);
				break;
			}
			if (!changeflag)
				checkkeyboard();
			else
				check_cursor_movement_and_update();

		}
	}

	switch (display_mode)
	{
	case MED:
		display_regular();
		break;
	case DOUBLE:
		display_double();
		break;
	case COARSE:
		display_coarse();
		break;
	}
	/* swap the buffers */
	for (i = 0; i < MEDYCOUNT + 2; i++)
	{
		tmpfp = oldbufrowptrs[i];
		oldbufrowptrs[i] = newbufrowptrs[i];
		newbufrowptrs[i] = tmpfp;
	}

	HideCursor();
	/* Now flip to the page we just painted */
	SetDisplay(display_page ? GrafPg1 : GrafPg0);
	SetBitmap(display_page ? GrafPg1 : GrafPg0);
	ShowCursor();
	display_page = 1 - display_page;
	tmpfp = (unsigned char *) newbuf;
	newbuf = oldbuf;
	oldbuf = tmpfp;
}







