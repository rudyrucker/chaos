#include "mag.h"


/* This is the charge editor popup. */

static numbertool NT[40];
static rect *bR[MAXMAGS*3 + 6];
static rect doitR[3];
static char *doitmsgs[] = {
   "F1 for HELP",
   "ESC to Cancel",
   "ACCEPT"
    };
static int items;
static rect oR[40];
static rect nextR;
static int page;
static char *pagemsg = "Next Page";

static void hilite_magnet(int n)
{
   PenMode(zXORz);
   PenColor(WHITE);
   PaintOval(&oR[n]);
   PenMode(zREPz);
}

static void push(int n,int inout)
{
   int i;

   if (n == -1)
      return;

   if (M.n > 10)
      i = 4;
   else
      i = 3;

   if (n >= items - i )
   {
      PushButton(bR[n],inout);
      if (n == items - 1)
         ExtraHilite(bR[n],inout);
   }
   else
   {
      int row = n / 3;

      if (row + page * 10 < M.n)
      {
         int col = n % 3;

         if (col == 0)
            InvertInsides(&NT[row + page*10].TB);
         else
            PushButton(bR[n],inout);

         hilite_magnet(row + page * 10);
      }
   }
}
static int lefters[34],righters[34],uppers[34],downers[34];
static int saved_uppers[34],saved_downers[34];

static void draw_entries(void)
{
   int i;
   int p10 = page * 10;
   char tbuf[28];
   int grayed = false;

   for(i=0;i<10 && i + p10 < M.n;i++)
   {
      int j = i + p10;
      numbertool *t = &NT[j];

      PaintNumberBoxEntry(&t->TB,t->value,GS_INTEGER);
      sprintf(tbuf,"% 2d: ",j+1);
      MoveTo(t->tR.Xmin - 4,t->tR.Ymin + 2);
   	TextAlign(alignRight, alignTop);
      PenColor(BLACK);
      BackColor(LIGHTGRAY);
      DrawString(tbuf);
      if (page == 0 && i >= M.n % 10)
      {
         PaintNTMinus(t);
         PaintNTPlus(t);
      }
   }
   for(;i<10;i++)
   {
      int j = i + p10;
      numbertool *t = &NT[j];
      PaintTextBoxEntry(&t->TB,DARKGRAY,WHITE,"");
      MoveTo(t->tR.Xmin - 4,t->tR.Ymin + 2);
   	TextAlign(alignRight, alignTop);
      PenColor(BLACK);
      BackColor(LIGHTGRAY);
      DrawString("    ");
      GrayOut(&t->TB.nR);
      GrayOut(&t->mR);
      GrayOut(&t->pR);
      grayed = true;
   }
   if (grayed)
   {
      int j;      
      memcpy(saved_uppers,uppers,sizeof uppers);
      memcpy(saved_downers,downers,sizeof downers);
      i = M.n % 10;
      for(j=0;j<3;j++)
         downers[3*(i - 1)+j] = items - 4;
      uppers[items - 4] = (i-1) * 3;
   }
   else if (page == 0)
   {
      memcpy(uppers,saved_uppers,sizeof uppers);
      memcpy(downers,saved_downers,sizeof downers);
   }


    
      

}



void charge_panel(void)
{
   rect R,tR;
   int width = sR.Xmax/4;
   int height;
   int doit = false;
   int i;
   char tbuf[128];
   int cx,cy;
   int current_item;
   magnetstructure tM = M;
   point P;
   double d;
   int j;

   items = 0;
   if (holenumber == 0)
      return;

   i = (M.n > 10) ? 11 : M.n;

   height =  (3 + i) * (3*FontHeight/2) + 2 * FontHeight;

   HideCursor();
	if (!CursorShowing)
		showclub(clubx, cluby, 0);


   PushCursorPosition();
   BasicBox(&tR,width,height,LIGHTGRAY,"Charge Editor",BLACK,4,4);
   PushMouseRectLimit(&tR);
   Centers(&tR,&cx,&cy);

   R.Ymin = tR.Ymin + FontHeight + 8;
   R.Ymax = R.Ymin + FontHeight + 4;
   i = (tR.Xmax - tR.Xmin)/3;
   R.Xmin = tR.Xmin + i;
   R.Xmax = tR.Xmax - 4;

   for(i=0;i<M.n && i < 10;i++)
   {

      NT[i].tR = R;
      NT[i].value = M.magnet[i].charge;
      NT[i].type = GS_INTEGER;
      NT[i].lo = -500;
      NT[i].hi = 500;
      sprintf(tbuf,"% 2d:",i+1);
      CreateNumberToolTitle(&NT[i],tbuf,BLACK,LIGHTGRAY);
      lefters[items] = righters[items+1] = items + 2;
      lefters[items + 1] = righters[items + 2] = items;
      lefters[items + 2] = righters[items] = items + 1;

      bR[items++] = &NT[i].TB.nR;
      bR[items++] = &NT[i].mR;
      bR[items++] = &NT[i].pR;
      OffsetRect(&R,0,3*FontHeight/2);

      SetPt(&P,tM.magnet[i].x+minx,tM.magnet[i].y);
      CenterRect(&P,19,19,&oR[i]);
   }

   for(i=10;i<40;i++)
   {
      if (i < M.n)
      {
         SetPt(&P,tM.magnet[i].x+minx,tM.magnet[i].y);
         CenterRect(&P,19,19,&oR[i]);
      }
      NT[i] = NT[i%10];
      NT[i].value = M.magnet[i].charge;
   }


   OffsetRect(&R,0,FontHeight/2);
   R.Xmin = tR.Xmin + 4;
   page = 0;

   if (M.n > 10)
   {
      PaintRadioButton(&R,false,false,pagemsg);
      nextR = R;
      uppers[items] = items - 3;
      downers[items] = items + 1;
      righters[items] = lefters[items] = items;
      bR[items++] = &nextR;
      OffsetRect(&R,0,3*FontHeight/2);
   }


   for(i=0;i<3;i++)
   {
      PaintRadioButton(&R,false,false,doitmsgs[i]);
      doitR[i] = R;
      if (i == 0 && M.n <= 10)
         uppers[items] = items - 3;
      else
         uppers[items] = items - 1;
      if (i == 2)
         downers[items] = 0;
      else
         downers[items] = items + 1;

      lefters[items] = righters[items] = items;
      
      bR[items++] = &doitR[i];
      OffsetRect(&R,0,3*FontHeight/2);
   }

   /* Now create the uppers and downers */
   uppers[0] = uppers[1] = uppers[2] = items - 1;
   downers[0] = downers[1] = downers[2] = 3;

   for(i=1;i<M.n-1 && i < 9;i++)
   {
      for(j=0;j<3;j++)
      {
         uppers[3*i+j] = 3*(i-1)+j;
         downers[3*i+j] = 3*(i+1)+j;
      }
   }
   for(j=0;j<3;j++)
   {
      uppers[3*i+j] = 3*(i-1)+j;
      downers[3*i+j] = items - ((M.n > 10) ? 4 : 3);

   }


    

   current_item = items - 1;
   push(current_item,true);
   move_to_corner(bR[current_item]);
   ShowCursor();

   while(1)
   {
      event e;
      int n;
      int X,Y;
      int key = 0;
      int button;
      int last_item = current_item;

      n = KeyEvent(false,&e);
      X = e.CursorX;
      Y = e.CursorY;
      button = (e.State & 0x700) >> 8;

      current_item = -1;
      for(i=0;i<items;i++)
      {
         if (XYInRect(X,Y,bR[i]))
         {
            current_item = i;
            break;
         }
      }
      if (n)
      {
         if (e.ASCII && e.ASCII != 0xe0)
            key = e.ASCII;
         else if (e.ScanCode != 0xff)
            key = e.ScanCode << 8;

         if (button == swRight)
            key = 0x1b;

         if (button == swLeft)
            key = 0x0d;
      }


      if (key == 0x1b)
      {
         current_item = items - 2;
         key = 0x0d;
      }
      if (key == XF1)
      {
         current_item = items - 3;
         key = 0x0d;
      }

      navigate(key,lefters,righters,uppers,downers,items,bR,&current_item);


      if (last_item != current_item)
      {
         push(last_item,false);
         push(current_item,true);
      }

      if (key == 0x0d)
      {
         if (current_item == items - 1)
         {
            doit = true;
            break;
         }
         if (current_item == items - 2)
            break;

         if (current_item == items - 3)
            helptext("magcharg.hlp");


         if (M.n > 10 && current_item == items - 4)
         {
            /* Paginate */
            page++;
            if (page * 10 > M.n)
               page = 0;
            draw_entries();
         }
         i = min(M.n * 3,30);
         if (current_item < i)
         {
            int row = current_item / 3;
            int col = current_item % 3;
            int p10 = row+page*10;
            int pnt = false;
            numbertool *t = &NT[p10];

            if (p10 < M.n)
            {

               switch(col)
               {
               case 0:
                  d = t->value;
                  if (GetNumber(&t->TB,&d,t->type,t->lo,t->hi))
                  {
                     t->value = d;
                  }
                  push(current_item,true);
                  hilite_magnet(p10);
                  break;
               case 1:
                  t->value -= 1;
                  pnt = true;
                  break;
               case 2:
                  t->value += 1;
                  pnt = true;
                  break;
               }
               t->value = min(t->value,500);
               t->value = max(t->value,-500);

               if (pnt)
            	   PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);
            }
         }
               

      }

   }


   if (current_item != -1 && current_item < M.n)
   {
      PenMode(zXORz);
      PenColor(WHITE);
      PaintOval(&oR[current_item/3]);
      PenMode(zREPz);
   }

   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   PopMouseRect();
   ShowCursor();

   if (doit)
   {
      HideCursor();
      for(i=0;i<M.n;i++)
         M.magnet[i].charge = NT[i].value;

      newholeflag = false;
      set_samecharges();
      startconfig();
      drawChargeButton();
      if (CursorShowing)
      {
         CursorShowing = false;
         MoveCursor(ballx+minx,bally);
      }
      ShowCursor();
   }
   else if (!CursorShowing)
   {
      HideCursor();
		showclub(clubx, cluby, maxcolor - 1);
      ShowCursor();        /* it will be cleaned up in a bit */
   }


}
