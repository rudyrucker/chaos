/* Stuff that only HODGE needs, for an overlay. */
#include "toy.h"
extern void key_push(int n,int inout);



void process_tube_key(int keyword)
{
	int rf = 0, hf = 0, jf = 0, ff = 0, ef = 0;

	switch (keyword)
	{
	case 'h':
		if (tuberest > 1)
		{
			tuberest--;
			rf = hf = 1;
		}
		break;

	case 'H':
		if (tuberest < 127)
		{
			rf = hf = 1;
			tuberest++;
		}
		break;

	case 'a':
		if (tubealarm > 0)
		{
			tubealarm--;
			rf = jf = 1;
		}
		break;

	case 'A':
		if (tubealarm < 9)
		{
			tubealarm++;
			rf = jf = 1;
		}
		break;

	case 'j':
	case 'J':
		tubefuzz = 1 - tubefuzz;
		rf = ff = 1;
		break;

	case 'e':
		if (tubelive > 2)
		{
			tubelive--;
			rf = ef = 1;
		}
		break;

	case 'E':
		if (tubelive < 127)
		{
			tubelive++;
			rf = ef = 1;
		}
		break;
	}
	if (rf)
		newcaflag = rebuildflag = 1;
	if (hf)
		TWICE(setTubeHiding(1));
	if (jf)
		TWICE(setTubeFreak(1));
	if (ff)
		TWICE(setTubeFuzz(1));
	if (ef)
		TWICE(setTubeEating(1));

}

int process_tube(int i)
{
   int row = i/3;
   int col = i%3;
   double d;
   if (i == 9 || i == 10)
      return "jJ"[i-9];

   switch(row)
   {
   case 0:
      switch(col)
      {
      case 0:
         d = tuberest;
         if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,1,127))
            tuberest = d;
         TWICE(setTubeHiding(1));
         TWICE(key_push(current_main_item,true));
   		newcaflag = rebuildflag = 1;
         return 0;
      case 1:
         return 'h';
      case 2:
         return 'H';
      }
      break;
   case 1:
      switch(col)
      {
      case 0:
         d = tubelive;
         if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,2,127))
            tubelive = d;
         TWICE(setTubeEating(1));
         TWICE(key_push(current_main_item,true));
   		newcaflag = rebuildflag = 1;
         return 0;
      case 1:
         return 'e';
      case 2:
         return 'E';
      }
      break;
   case 2:
      switch(col)
      {
      case 0:
         d = tubealarm;
         if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,0,9))
            tubealarm = d;
         TWICE(setTubeFreak(1));
         TWICE(key_push(current_main_item,true));
   		newcaflag = rebuildflag = 1;
         return 0;
      case 1:
         return 'a';
      case 2:
         return 'A';
      }
      break;
   }

   return 0;

}
void load_tube(void)
{
	/* Tube rule */

	unsigned short C, firing_eight_sum;
	int Fuzz, A, O, E;
	unsigned char NewC;
	unsigned char *p = lookuptable;


	Fuzz = tubefuzz;
	A = tubealarm;
	O = tubelive;
	E = tuberest;



	for (firing_eight_sum = 0; firing_eight_sum < 16; firing_eight_sum++)
	{
		int SafeCondition = (firing_eight_sum < A || ((Fuzz == 1) &&
					      (firing_eight_sum == A + 1)));

		for (C = 0; C < 256; C++)
		{
			/*
			 * Keep around a few illegally high ones for
			 * aesthetic interest
			 */
			NewC = 0;
			if (C & 1)
			{
				if (C == 1)
					NewC = (SafeCondition) ? 1 : 3;
				else if (C < 2 * O - 1)
					NewC = C + 2;
				else if (C == 2 * O - 1)
					NewC = 0;
			}
			else
			{
				if (0 <= C && C < 2 * E)
					NewC = C + 2;
				else if (C == 2 * E)
					NewC = (SafeCondition) ? 1 : 3;
			}
			*p++ = NewC;
		}
	}
}
