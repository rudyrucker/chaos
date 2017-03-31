/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal code of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.							     *
*									     *
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <dir.h>
#include <dos.h>
#include <alloc.h>

#include "toy.h"
rect mainbuttonR[20];
rect *mainR[50];
extern int init_mem_err;
int main_items;
rect typenameR;
rect runstopstepR[3];
char *RSS[] = {"Run","Stop","Step"};

rect dummyR[10];
numbertool tweakNT[6];

#define QuitButtonR mainbuttonR[5]

int main_lefters[30] = {
   0,1,2,3,4,5
      };
int main_righters[30] = {
   0,1,2,3,4,5
      };
int main_uppers[30] = {
   -1,0,1,2,3,4
   };
int main_downers[30] = {
   1,2,3,4,5,-1
   };

int main_downers[30];


char *maintexts[] = {
   "F1 for HELP ",
   "F2 File     ",
   "F3 Rules    ",
   "F4 Options  ",
   "F5 Randomize",
   NULL
   };


rect SSR;

#pragma argsused
void setStopSign(int stop)
{
   /* Just paint two buttons... */
   PaintRadioButton(&runstopstepR[0],!stopped,!stopped,RSS[0]);
   PaintRadioButton(&runstopstepR[1],stopped,stopped,RSS[1]);
}

void setTypename(void)
{
   int cx,cy;

   Centers(&typenameR,&cx,&cy);
   MoveTo(cx,cy);
   TextAlign(alignCenter,alignMiddle);
   PenColor(WHITE);
   BackColor(BLUE);
   DrawString(typenames[caotype]);

}
static void CreateButton(rect *S,numbertool *NT,char *label)
{
   rect R;

   NT->type = GS_UNSIGNED;
   R = *S;
   R.Xmin += StringWidthX * 8;
   NT->tR = R;
   CreateNumberTool(NT);
   JString(label,R.Xmin - 4,R.Ymin+4,WHITE,BLUE,alignRight,alignTop);
}

static void add_numbertool(numbertool *t)
{
   mainR[main_items++] = &t->TB.nR;
   mainR[main_items++] = &t->mR;
   mainR[main_items++] = &t->pR;
}

static void add_rects(rect *R,int n)
{
   int i;
   for(i=0;i<n;i++)
      mainR[main_items++] = &R[i];
}

void setNumberOfStates(int label_only)
{
   numbertool *t = &tweakNT[0];

   t->value = maxstate;
   if (!label_only)
   {
      CreateButton(&dummyR[0],t,"Number:");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}

void setIncrementDecrement(int l)
{
   numbertool *t = &tweakNT[1];
   t->value = increment;
   if (!l)
   {
      CreateButton(&dummyR[1],t,"Inc: ");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}
static rect jazzR[2];
static char *OO[] = {"Off","On"};

void setTubeFuzz(int l)
{
   rect R = dummyR[3];
   char *dmsg = "Jazz: ";
   R.Xmin += StringWidth(dmsg) + 4;
   CreateRadioPanelLabel(&R,OO,jazzR,2,tubefuzz,dmsg,WHITE,BLUE);
   if (!l)
      add_rects(jazzR,2);
}


void setTubeFreak(int l)
{
   numbertool *t = &tweakNT[2];
   t->value = tubealarm;
   if (!l)
   {
      CreateButton(&dummyR[2],t,"Alarm: ");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}

void setTubeEating(int l)
{
   
   numbertool *t = &tweakNT[1];
   t->value = tubelive;
   if (!l)
   {
      CreateButton(&dummyR[1],t,"Eating: ");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}

void setTubeHiding(int l)
{
   numbertool *t = &tweakNT[0];
   t->value = tuberest;
   if (!l)
   {
      CreateButton(&dummyR[0],t,"Hiding: ");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}



void setEATN(int l)
{
   numbertool *t = &tweakNT[1];
   t->value = maxeatstate;
   if (!l)
   {
      CreateButton(&dummyR[1],t,"maxEat: ");
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}

rect deterministicR[2];
static char *NY[] = {"No","Yes"};

void setEatD(int l)
{
   rect R = dummyR[0];
   char *dmsg = "Determ: ";
   R.Xmin += StringWidth(dmsg) + 4;
   CreateRadioPanelLabel(&R,NY,deterministicR,2,eatmode,dmsg,WHITE,BLUE);
   if (!l)
      add_rects(deterministicR,2);
}

unsigned char *nluky_parms[] =
{
	&nlukyn,
	&nlukyl,
	&nlukyu,
	&nlukyk,
	&nlukyy
};

void setnlukys(int n, int l)
{
   numbertool *t = &tweakNT[n];

   t->value = *nluky_parms[n];
   if (!l)
   {
      char tbuf[10];
      sprintf(tbuf,"%c:   ","NLUKY"[n]);
      CreateButton(&dummyR[n],t,tbuf);
      add_numbertool(t);
   }
   else
   	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
}


void PaintQuitButton(int inout)
{
   rect R = QuitButtonR;
   int cx,cy;

   HideCursor();

   Centers(&R,&cx,&cy);
   PaintRadioButton(&R,inout,inout,"");
   TextAlign(alignCenter,alignTop);
   PenColor(WHITE);
   BackColor(inout ? RED : DARKGRAY);
   MoveTo(cx,R.Ymin + 4);
   DrawString("Alt-X");
   MoveTo(cx,R.Ymin + FontHeight + 4);
   DrawString("to Exit");
   PushButton(&R,inout);
   ExtraHilite(&R,inout);
   if (inout)
      DoublePress(&R,true,RED);
   ShowCursor();
}



unsigned long realfarcoreleft(void)
{
   unsigned long l1 = farcoreleft();
   struct farheapinfo hi;

   if (farheapcheck() == _HEAPCORRUPT)
      return 0L;

   hi.ptr = NULL;

   while(farheapwalk(&hi) == _HEAPOK)
   {
      if (!hi.in_use)
         l1 += hi.size;
   }
   return l1;
}

