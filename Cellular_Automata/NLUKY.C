/* Stuff that only HODGE needs, for an overlay. */
#include "toy.h"
extern void key_push(int n,int inout);



void process_nluky_key(int keyword)
{
	int rf = 0, nf = 0, lf = 0, uf = 0, kf = 0, yf = 0;

	switch (keyword)
	{
	case 'n':
		if (nlukyn > 0)
		{
			nlukyn--;
			rf = nf = 1;
		}
		break;
	case 'N':
		if (nlukyn < 127)
		{
			nlukyn++;
			rf = nf = 1;
		}
		break;

	case 'l':
		if (nlukyl > 0)
		{
			nlukyl--;
			rf = lf = 1;
		}
		break;

	case 'L':
		if (nlukyl < 9)
		{
			nlukyl++;
			rf = lf = 1;
		}
		break;

	case 'u':
		if (nlukyu > 0)
		{
			nlukyu--;
			rf = uf = 1;
		}
		break;
	case 'U':
		if (nlukyu < 9)
		{
			nlukyu++;
			rf = uf = 1;
		}
		break;


	case 'k':
		if (nlukyk > 0)
		{
			nlukyk--;
			rf = kf = 1;
		}
		break;

	case 'K':
		if (nlukyk < 9)
		{
			nlukyk++;
			rf = kf = 1;
		}
		break;

	case 'y':
		if (nlukyy > 0)
		{
			nlukyy--;
			rf = yf = 1;
		}
		break;

	case 'Y':
		if (nlukyy < 9)
		{
			nlukyy++;
			rf = yf = 1;
		}
		break;
	}
	if (rf)
		newcaflag = rebuildflag = 1;

	if (nf)
		TWICE(setNLUKY_N(1));
	if (lf)
		TWICE(setNLUKY_L(1));
	if (uf)
		TWICE(setNLUKY_U(1));
	if (kf)
		TWICE(setNLUKY_K(1));
	if (yf)
		TWICE(setNLUKY_Y(1));

}

int process_nluky(int n)
{
   int row = n/3;
   int col = n%3;
   double d = *nluky_parms[row];


   switch(col)
   {
   case 0:
      if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,0,(row == 0) ? 127 : 9))
         *nluky_parms[row] = d;
	   switch (row)
	   {
	   case 0:
		   TWICE(setNLUKY_N(1));
		   break;
	   case 1:
		   TWICE(setNLUKY_L(1));
		   break;
	   case 2:
		   TWICE(setNLUKY_U(1));
		   break;
	   case 3:
		   TWICE(setNLUKY_K(1));
		   break;
	   case 4:
		   TWICE(setNLUKY_Y(1));
		   break;
	   }
      TWICE(key_push(current_main_item,true));
   	newcaflag = rebuildflag = 1;
      return 0;
   case 1:
      return "nluky"[row];
   case 2:
      return "NLUKY"[row];
   }
   return 0;
}
void load_nluky(void)
{
	unsigned short C, firing_eight_sum;
	unsigned char *p = lookuptable;	/* Don't need HUGE for this one */

	unsigned char bl;

	for (firing_eight_sum = 0; firing_eight_sum < 16; firing_eight_sum++)
	{
		for (C = 0; C < 256; C++)
		{
			if (C == 0)
			{
				/*
				 * A zero cell stays zero unless L <= firing
				 * eightsum <= U, in which case the cell goes
				 * to 1. If however U < L, then we imagine
				 * "good" range wrapping up from L past 8
				 * through 0 and up to U, which means
				 * condition then is L <= firing eighsum OR
				 * firing 8sum <=U
				 */
				bl = 0;
				 /* new */ if (nlukyl <= nlukyu)
				{
					if (nlukyl <= firing_eight_sum && firing_eight_sum <= nlukyu)
						bl++;
				}
				/* new */
				else
				 /* new */ if (nlukyl <= firing_eight_sum || firing_eight_sum <= nlukyu)
					 /* new */ bl++;
			}
			else if (C == 1)
			{
				/*
				 * A firing 1 cell goes to 2, unless K <=
				 * firing 8sum <= Y, when it stays 1.   Here
				 * also if Y < K we imagine wrapping thru 8
				 * so have th condition K <= firing 8sum OR
				 * firing 8sum <=Y.
				 */
				 /* new */ if (nlukyk <= nlukyy)
					if (nlukyk <= firing_eight_sum && firing_eight_sum <= nlukyy)
						bl = 1;
					else
						bl = (nlukyn) ? 2 : 0;
				/* new */
				else
				 /* new */ if (nlukyk <= firing_eight_sum || firing_eight_sum <= nlukyy)
					 /* new */ bl = 1;
				/* new */
				else
					 /* new */ bl = (nlukyn) ? 2 : 0;
			}
			else
			{
				bl = 0;

				if ((C & 1) || (C > nlukyn * 2))
				{
					/*
					 * turn on about 1/16 of the invalid
					 * ones
					 */
					if (C > 240)
						bl = 1;
				}
				else
				{
					if (C < nlukyn * 2)
						bl = C + 2;
					else
						bl = 0;
				}
			}
			*p++ = bl;
		}
	}
}
