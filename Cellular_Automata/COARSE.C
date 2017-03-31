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
void locoarse(unsigned char huge *, unsigned char *, int);

void display_coarse(void)
{
	/* Display the contents of "newbuf" in coarse mode. */
	/*
	 * Pulled out of the main loop for fun; why the hell not do it all at
	 * once now, since we are double- buggering?
	 */
	int firstrow, firstcol;
	int row, i;

	firstrow = coarsecorners.Ymin;
	firstcol = coarsecorners.Xmin;

	for (row = firstrow, i = 0; i < LOYCOUNT; i++, row++)
	{
		if (caotype == CA_NLUKY)
		{
			unsigned char buffer[LOXCOUNT];

			translate_cao1(buffer, newbufrowptrs[row] + 1 + firstcol, LOXCOUNT);
			locoarse(buffer,
				 MK_FP(display_page ? 0xa800 : 0xa000, i * 8 * 80 + FIRSTXBYTE), skipper);
		}
		else
			locoarse(newbufrowptrs[row] + firstcol,
				 MK_FP(display_page ? 0xa800 : 0xa000, i * 8 * 80 + FIRSTXBYTE), skipper);
	}
}

