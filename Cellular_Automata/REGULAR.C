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
void loregular(unsigned char huge *, unsigned char *);

void display_regular(void)
{
	int i;

	for (i = 0; i < MEDYCOUNT; i++)
	{
		if (caotype == CA_NLUKY)
		{
			unsigned char buffer[MEDXCOUNT];

			translate_cao1(buffer, newbufrowptrs[i + 1] + 1, MEDXCOUNT);
			loregular(buffer,
				  MK_FP(display_page ? 0xa800 : 0xa000, i * 160 + FIRSTXBYTE));
		}
		else
			loregular(newbufrowptrs[i + 1] + 1,
				  MK_FP(display_page ? 0xa800 : 0xa000, i * 160 + FIRSTXBYTE));
	}
}

