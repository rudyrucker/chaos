/* Stuff that only HODGE needs, for an overlay. */
#include "toy.h"
extern void key_push(int n,int inout);

void process_eat_key(int keyword)

{
	int rf = 0, nf = 0, df = 0;

	switch (keyword)
	{
	case 'e':
		maxeatstate--;
		if (!maxeatstate)
			maxeatstate = 1;
		rf = nf = 1;
		break;

	case 'E':
		maxeatstate++;
		if (!maxeatstate)
			maxeatstate = 255;
		rf = nf = 1;
		break;

	case 'D':
	case 'd':
		eatmode ^= 1;
		rf = df = nf = 1;
		break;
	}

	if (rf)
		newcaflag = rebuildflag = 1;
	if (nf)
		TWICE(setEATN(1));
	if (df)
		TWICE(setEatD(1));
}

int process_eat(int i)
{
   if (i == 0 || i == 1)
      return 'd';
   else
   {
      i -= 2;
      if (i == 0)
      {
         double d = maxeatstate;
         if (GetNumber(&tweakNT[1].TB,&d,GS_INTEGER,1,255))
            maxeatstate = d;
         TWICE(setEATN(1));
         TWICE(key_push(current_main_item,true));
   		newcaflag = rebuildflag = 1;
         return 0;
      }
      else return "eE"[i-1];
   }
}


