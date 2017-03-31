#include "toy.h"
#include <dir.h>


int arcpts[] = {
	30, 0, 29, 0, 29, 1, 29, 2, 29, 3, 29, 4, 29, 5, 28, 5, 28, 6, 28, 7, 27, 7, 27, 8, 27, 9, 26, 9,
	26, 10, 25, 10, 25, 11, 24, 12, 23, 12, 23, 13, 22, 13, 22, 14, 21, 14, 21, 15, 20, 15, 19, 16,
	18, 16, 18, 17, 17, 17, 16, 17, 16, 18, 15, 18, 14, 18, 13, 19, 12, 19, 11, 19, 10, 20, 9, 20,
	8, 20, 7, 20, 6, 20, 6, 21, 5, 21, 4, 21, 3, 21, 2, 21, 1, 21, 0, 21,
	-1, -1
};

int canned_image = 0;

void load_canned_shape(int overlay)
{
	int i;
   FILE *fd;
	/* This one always resets the display page. */

	/*
	 * Put some or another canned shape into the display. Squares,
	 * circles, frames should do for now.
	 */

	/*
	 * First, just load a square in the middle of the screen, 1/4 the
	 * size of the entire visible display.
	 */



	if (!overlay)
	{
		if (display_mode == HI)
		{
			for (i = 0; i < HIYCOUNT + 2; i++)
				memset(egarowptrs[i], 0, HIXCOUNT + 2);
		}
		else
		{
			for (i = 0; i < MEDYCOUNT + 2; i++)
			{
				memset(oldbufrowptrs[i], 0, MEDXCOUNT + 2);
				memset(newbufrowptrs[i], 0, MEDXCOUNT + 2);
			}
		}
	}


	switch (canned_image)
	{
	case 0:
		if (display_mode != HI)
		{
			for (i = 0; i < MEDYCOUNT / 4; i++)
			{
				memset(oldbufrowptrs[i + (3 * MEDYCOUNT) / 8] + (3 * MEDXCOUNT) / 8,
				       1, MEDXCOUNT / 4);
				memset(newbufrowptrs[i + (3 * MEDYCOUNT) / 8] + (3 * MEDXCOUNT) / 8,
				       1, MEDXCOUNT / 4);
			}
		}
		else
		{
			for (i = 0; i < HIYCOUNT / 4; i++)
				memset(egarowptrs[i + (3 * HIYCOUNT) / 8] + (3 * HIXCOUNT) / 8,
				       1, HIXCOUNT / 4);
		}
		break;
	case 1:
		/* A diamond, instead. */
		if (display_mode == HI)
		{
			int centerx = HIXCOUNT / 2;

			for (i = 0; i < HIYCOUNT / 8; i++)	/* note we double it all */
			{
				memset(egarowptrs[(3 * HIYCOUNT) / 8 + i + 1]
				       + centerx - i, 1, i * 2 + 1);
				memset(egarowptrs[(5 * HIYCOUNT) / 8 - i - 1]
				       + centerx - i, 1, i * 2 + 1);
			}
		}
		else
		{
			int centerx = MEDXCOUNT / 2;

			for (i = 0; i < MEDYCOUNT / 8; i++)
			{
				memset(newbufrowptrs[(3 * MEDYCOUNT) / 8 + i + 1]
				       + centerx - i, 1, i * 2 + 1);
				memset(newbufrowptrs[(5 * MEDYCOUNT) / 8 - i - 1]
				       + centerx - i, 1, i * 2 + 1);
				memset(oldbufrowptrs[(3 * MEDYCOUNT) / 8 + i + 1]
				       + centerx - i, 1, i * 2 + 1);
				memset(oldbufrowptrs[(5 * MEDYCOUNT) / 8 - i - 1]
				       + centerx - i, 1, i * 2 + 1);
			}
		}
		break;

	case 2:
		/* Maybe a circle? Hmm. Let's see if this works at all. */
		if (display_mode == HI)
		{
			int centerx = HIXCOUNT / 2;
			int centery = HIYCOUNT / 2;

			for (i = 0; arcpts[i] != -1; i += 2)
			{
				egarowptrs[centery + arcpts[i + 1]][centerx + arcpts[i]] = 1;
				egarowptrs[centery - arcpts[i + 1]][centerx + arcpts[i]] = 1;
				egarowptrs[centery + arcpts[i + 1]][centerx - arcpts[i]] = 1;
				egarowptrs[centery - arcpts[i + 1]][centerx - arcpts[i]] = 1;
			}
		}
		else
		{
			int centerx = MEDXCOUNT / 2;
			int centery = MEDYCOUNT / 2;

			for (i = 0; arcpts[i] != -1; i += 2)
			{
				newbufrowptrs[centery + arcpts[i + 1]][centerx + arcpts[i]] = 1;
				newbufrowptrs[centery - arcpts[i + 1]][centerx + arcpts[i]] = 1;
				newbufrowptrs[centery + arcpts[i + 1]][centerx - arcpts[i]] = 1;
				newbufrowptrs[centery - arcpts[i + 1]][centerx - arcpts[i]] = 1;

				oldbufrowptrs[centery + arcpts[i + 1]][centerx + arcpts[i]] = 1;
				oldbufrowptrs[centery - arcpts[i + 1]][centerx + arcpts[i]] = 1;
				oldbufrowptrs[centery + arcpts[i + 1]][centerx - arcpts[i]] = 1;
				oldbufrowptrs[centery - arcpts[i + 1]][centerx - arcpts[i]] = 1;
			}
		}
		break;

   case 3:
      /* the Autodesk logo */

      if (display_mode == HI)
      {
			int centerx = HIXCOUNT / 2;
			int centery = HIYCOUNT / 2;
         int row=0;
         char *p = searchpath("acad180.dat");


         if (p)
         {
            fd = fopen(p,"rb");
            if (fd)
            {
               int width = 180;
               int height = width/aspect;

               while(1)
               {
                  unsigned char c = getc(fd);
                  if (c == 254)
                     break;

                  if (c == 255)
                     row = getc(fd);
                  else
                  {
                     egarowptrs[centery - height/2 + row][centerx - width/2 + c] = 1;
                     egarowptrs[centery - height/2 + row][centerx + width/2 - c - 1] = 1;
                  }
               }
               fclose(fd);
            }
         }
      }
      else
      {
			int centerx = MEDXCOUNT / 2;
			int centery = MEDYCOUNT / 2;
         int row=0;
         char *p = searchpath("acad120.dat");
         if (p)
         {

            fd = fopen(p,"rb");
            if (fd)
            {
               int width = 120;
               int height = width/aspect;
               while(1)
               {
                  unsigned char c = getc(fd);
                  if (c == 254)
                     break;

                  if (c == 255)
                     row = getc(fd);
                  else
                  {
                     newbufrowptrs[centery - height/2 + row][centerx - width/2 + c] = 1;
                     oldbufrowptrs[centery - height/2 + row][centerx - width/2 + c] = 1;
                     newbufrowptrs[centery - height/2 + row][centerx + width/2 - c - 1] = 1;
                     oldbufrowptrs[centery - height/2 + row][centerx + width/2 - c - 1] = 1;
                  }
               }
               fclose(fd);
            }
         }
      }
      break;






	}

	switch (display_mode)
	{
	case HI:
		display_hi();
		break;
	case COARSE:
		display_coarse();
		break;
	case DOUBLE:
		display_double();
		break;
	case MED:
		display_regular();
		break;
	}
	HideCursor();
	/* Now flip to the page we just painted */
	SetDisplay(display_page ? GrafPg1 : GrafPg0);
	SetBitmap(display_page ? GrafPg1 : GrafPg0);
	ShowCursor();

	display_page = 1 - display_page;

}

static rect shapeR[4];
static rect doitR[3];
static int our_cannedimage;
static int paintmode = 0;
static int our_paintmode;
static rect modeR[2];
static rect doitR[3];

static rect *bR[10];
static int items;

static void push(int n,int inout)
{
   if (n == -1)
      ;
   else if (n < 4)
      PushOrDoublePress(bR[n],inout,n == our_cannedimage);
   else if (n < 6)
      PushOrDoublePress(bR[n],inout,n - 4 == our_paintmode);
   else
      PushButton(bR[n],inout);
   if (n == items - 1)
      ExtraHilite(bR[n],inout);
}


void ShapeLoader(void)
{
   rect tR;
   int height,width;
   rect R;
   int cx,cy;
   static char *shapemsgs[] = {"Square","Diamond","Circle","Logo"};
   static char *modemsgs[] = {"Replace","Overlay"};
   static char *doitmsgs[] = {"F1 for HELP","ESC to Cancel","ACCEPT"};
   int current_item;
   int row;
   char *p;
   int i;
   int doit = false;
   static int l[] = {3,0,1,2,5,4,8,6,7};
   static int r[] = {1,2,3,0,5,4,7,8,6};
   static int u[] = {6,6,7,8,1,2,4,4,5};
   static int d[] = {4,4,5,5,6,8,0,2,3};


   items = 0;
   height = 3 * (3 * FontHeight/2) + 2 * FontHeight + 4;
   width = 2*sR.Xmax/3;

   HideCursor();
   PushCursorPosition();
   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"Shape Menu",BLACK);
   Centers(&tR,&cx,&cy);

   row = tR.Ymin + FontHeight + 8;
   R.Xmin = tR.Xmin + (tR.Xmax-tR.Xmin)/4;
   R.Xmax = tR.Xmax - 4;
   R.Ymin = row;
   R.Ymax = row + FontHeight + 4;
   CreateRadioPanelLabel(&R,shapemsgs,shapeR,4,our_cannedimage=canned_image,
      "Shape: ",BLACK,LIGHTGRAY);
   for(i=0;i<4;i++)
      bR[items++] = &shapeR[i];

   row += 3*FontHeight/2;
   OffsetRect(&R,0,3*FontHeight/2);
   CreateRadioPanelLabel(&R,modemsgs,modeR,2,our_paintmode = paintmode,"Mode: ",BLACK,LIGHTGRAY);
   bR[items++] = &modeR[0];
   bR[items++] = &modeR[1];

   row += 2*FontHeight;
   OffsetRect(&R,0,2*FontHeight);
   R.Xmin = tR.Xmin + 4;
   CreateRadioPanel(&R,doitmsgs,doitR,3,-1);
   for(i=0;i<3;i++)
      bR[items++] = &doitR[i];

   LimitMouseRect(&tR);
   current_item = items - 1;
   push(current_item,true);
   move_to_corner(bR[current_item]);
   push(current_item,true);
   ShowCursor();
   while(1)
   {
		event e;
		int n = KeyEvent(false, &e);
		int X = e.CursorX;
		int Y = e.CursorY;
		int button = (e.State & 0x700) >> 8;
		int last_item = current_item;

      int key = 0;

   	current_item = -1;
		for (i = 0; i < items; i++)
		{
			if (XYInRect(X, Y, bR[i]))
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

			if (button == swLeft)
				key = 0x0d;

         if (key == 0x0d && current_item == items - 2)
            key = 0x1b;

			if (button == swRight)
				key = 0x1b;

			if (key == 0x1b)
         {
            current_item = items - 2;
   			push(last_item, false);
            push(current_item,true);
            move_to_corner(bR[current_item]);
				break;
         }


		}

		if (key == 0x0d && current_item == items - 2)
			break;

		if (key == 0x0d && current_item == items - 1)
		{
			doit = true;
			break;
		}
		if (key == 0x0d && current_item == items - 3)
			key = XF1;

		if (key == XF1)
      {
			helptext("toyshape.hlp");
         LimitMouseRect(&tR);
      }

      if (key == 0x0d)
      {
         if (current_item < 4)
            CheckRadioButtons(X,Y,shapeR,4,&our_cannedimage,shapemsgs);
         else if (current_item < 6)
            CheckRadioButtons(X,Y,modeR,4,&our_paintmode,modemsgs);
      }

      navigate(key,l,r,u,d,items,bR,&current_item);

      if (current_item != last_item)
      {
         push(last_item,false);
         push(current_item,true);
      }
   }

   if (doit)
      p = doitmsgs[2];
   else
      p = doitmsgs[1];


	PaintRadioButtonBase(bR[current_item], true, true, p, DARKGRAY, RED, WHITE);
	if (doit)
		ExtraHilite(bR[current_item], true);
	WaitForNothing();
	PaintRadioButtonBase(bR[current_item], false, false, p, DARKGRAY, RED, WHITE);
	if (doit)
		ExtraHilite(bR[current_item], false);
	HideCursor();
	PopRect(&i);
	PopCursorPosition();
	LimitMouseRect(&sR);
	ShowCursor();

   if (doit)
   {
      canned_image = our_cannedimage;
      paintmode = our_paintmode;
      load_canned_shape(paintmode);
   }


}

