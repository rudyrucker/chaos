/* Stuff that only HODGE needs, for an overlay. */
#include "toy.h"
extern void key_push(int n,int inout);

void process_hodge_key(int keyword)
{
	int sidflag = 0, nosflag = 0;

	switch (keyword)
	{
	case 'i':
		if (increment >= 16)
			increment -= 8;
		else if (increment)
			increment--;
		sidflag = 1;
		break;

	case 'I':
		if (increment >= 8)
      {
         if (increment < 255-8)
   			increment += 8;
         else
            increment = 255;
      }
		else 
			increment++;
 
		sidflag = 1;
		break;

	case 'n':
		if (maxstate >= 24)
			maxstate -= 8;
		else if (maxstate > 1)
			maxstate--;
		nosflag = 1;
		break;

	case 'N':
		if (maxstate >= 16)
      {
			if (maxstate < 255-8)
            maxstate += 8;
         else
            maxstate = 255;
      }
		else
			maxstate++;
		nosflag = 1;
		break;
	}
	if (sidflag || nosflag)
		newcaflag = rebuildflag = 1;
	if (sidflag)
		TWICE(setIncrementDecrement(1));
	if (nosflag)
		TWICE(setNumberOfStates(1));
}

int process_hodge(int i)
{
   int row = i/3;
   int col = i%3;

   if (row==0)
   {
      switch(col)
      {
      case 0:
      {   
         double d = maxstate;
         if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,1,255))
            maxstate = d;
   		TWICE(setNumberOfStates(1));
         TWICE(key_push(current_main_item,true));
   		newcaflag = rebuildflag = 1;
         return 0;
      }
      case 1:
         return 'n';
      case 2:
         return 'N';
      }
   }
   switch(col)
   {
   case 0:
   {   
      double d = increment;
      if (GetNumber(&tweakNT[row].TB,&d,GS_INTEGER,0,255))
         increment = d;
		TWICE(setIncrementDecrement(1));
      TWICE(key_push(current_main_item,true));
   	newcaflag = rebuildflag = 1;
      return 0;
   }
   case 1:
      return 'i';
   case 2:
      return 'I';
   }
   return 0;
}
      
