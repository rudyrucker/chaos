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

#include <dos.h>
#include <mem.h>
#include "mand.h"

char vgacolortable[51] = {
	00,00,00,
	00,00,42,
	00,21,42,
	00,32,42,
	42, 0, 0,
	30,42,42,
	37,43,34,
	44,44,44,
	21,21,21,
	63,57,20,
	63,46,20,
	63,40,18,
	62,31,12,
	63,21,03,
	63,62,00,
	63,63,63
   };
char defaultcolortable[51];
char egacolortable[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};
char default_palette[51];

void SetEGAPalette(void)
{
   union REGS regs;
   struct SREGS sregs;
   regs.h.ah = 0x10;
   regs.h.al = 0x2;
   regs.x.dx = FP_OFF(egacolortable);
   sregs.es = FP_SEG(egacolortable);
   int86x(0x10,&regs,&regs,&sregs);
}


void usepalette(void)
{
   union REGS regs;
   struct SREGS sregs;

   if (mode==0x10)         /* EGA mode */
   {
      ega_lut = egacolortable;
      SetEGAPalette();
   }
   else
   {
      SetEGAPalette();
      regs.h.ah = 0x10;
      regs.h.al = 0x12;
      regs.x.cx = 17;
      regs.x.bx = 0;
      vga_lut = vgacolortable;
      regs.x.dx = FP_OFF(vga_lut);
      sregs.es = FP_SEG(vga_lut);
      int86x(0x10,&regs,&regs,&sregs);
   }
}



      

#define minimum_luminosity 10
unsigned char rudy_colors1[] =
{
   /* Ten nice colors, leave the others alone... */
   /* 3 reds adding blue;
      4 greens adding blue;
      3 yellows adding blue.
   */
   63,0,0,
   63,0,21,
   63,0,42,

   0,63,0,
   0,63,15,
   0,63,30,
   0,63,45,

   63,63,0,
   63,63,21,
   63,63,42
   };

unsigned char rudyset1[] = {
	0,0,0,
	63,0,0,
	63,0,10,
	63,0,20,
	63,0,30,
	63,0,40,
	63,0,50,
	50,0,63,
	40,0,63,
	30,0,63,
	20,0,63,
	10,0,63,
	63,45,0,
	63,52,0,
	63,55,0,
	63,63,0,
   };

unsigned char rudyset2[] = {
	  0,0,0,
	 63,0,0,
	 63,0,10,
	 63,0,20,
	 63,0,30,
	 63,0,40,
	 0,63,0,
	 0,63,10,
	 0,63,20,
	 0,63,30,
	 0,63,40,
	 0,63,0,
	63,63,10,
	63,63,20,
	63,63,30,
	63,63,40,
};

unsigned char rudyset3[] = {
    0,   0,   0,
   63,   0,   0,
   63,   0,  0,
   63,   0,  0,
    0,  63,  0,
    0,  63,  0,
    0,  63,  0,
    0,   0,  63,
    0,   0,  63,
    0,   0,  63,
    63,  0,  63,
    63,  0,  63,
    63,  0,  63,
    63, 63,  0,
    63, 63,  0,
    63, 63,  0,
   };

unsigned char rudyset4[] = {
   0,   0,   0,
   63,   0,   0 ,
   63,   0,  9  ,
   63,   0,  18 ,
   63,   0,  27 ,
   63,   0,  36 ,
   63,   0,  45 ,
   63,   0,  54 ,
   63,   0,  63 ,
   63,   0,  54 ,
   63,  0,  45 ,
   63,  0,  36 ,
   63,  0,  27 ,
   63,  0,  18 ,
   63,  0,  9  ,
   63,  0,  2  ,
};



void randompalette(void)
{
   int i;
   int limit = (locked ? 10 : 15);

   if (mode == 0x12)
   {
		for (i = 0; i < limit; i++)
		{
			while (1)
			{
				int r, g, b;
				double luminosity;

				r = random(100);
				g = random(100);
				b = random(100);


				luminosity = 0.3 * r + 0.59 * g + 0.11 * b;
				if (luminosity > minimum_luminosity)
				{
               if (locked)
               {
   					vgacolortable[acceptable_colors[i]*3] = (r * 63) / 99;
	   				vgacolortable[acceptable_colors[i]*3+1] = (g * 63) / 99;
		   			vgacolortable[acceptable_colors[i]*3+2] = (b * 63) / 99;
               }
               else
               {
   					vgacolortable[i*3+3] = (r * 63) / 99;
	   				vgacolortable[i*3+4] = (g * 63) / 99;
		   			vgacolortable[i*3+5] = (b * 63) / 99;
               }

					break;
				}
			}
      }
   }
   else
   {
      /* Pick, at random, 15 numbers out of the 64. What fun. */
      /* fill the 64 first */
      char testbed[64];
      int n = 64;

      for(i=0;i<64;i++)
         testbed[i] = i;

      for(i=1;i<16;i++)
      {
         int t = random(n);
         egacolortable[i] = testbed[t];
         /* probably could memcpy(testbed+t,testbed+t+1,n-t) */
         for(;t<n;t++)
            testbed[t] = testbed[t+1];
         n--;
      }
   }
   usepalette();
}



void bumppalette(void)
{
   
   palettenumber = (palettenumber+1) % 5;

   switch(palettenumber+1)
   {
   case 1:
      DefaultPalette();
      return;

   /* The following sets are at Rudy's request, but make the menus
      look most unappetizing. */
   case 2:
      memcpy(vgacolortable,rudyset1,16*3);
      break;
   case 3:
      memcpy(vgacolortable,rudyset2,16*3);
      break;
   case 4:
      memcpy(vgacolortable,rudyset3,16*3);
      break;
   case 5:
      memcpy(vgacolortable,rudyset4,16*3);
      break;

   }
   usepalette();
}
int locked = true;

static int upspin[16] = {
   0,1,        /* 0 and 1 don't change */
   3,5,        /* 2 and 3 change */
   4,          /* 4 is fixed */
   6,9,        /* 5 and 6 change */
   7,8,        /* 7 and 8 are grays, fixed */
   10,11,12,13,14,2, /* 9 to 14 change */
   15          /* 15 is fixed */
   };

static int bigupspin[16] = {
   0,
   2,3,4,5,6,7,8,9,10,11,12,13,14,15,1
   };
static int bigdownspin[16] = {
   0,
   15,1,2,3,4,5,6,7,8,9,10,11,12,13,14
   };
   


static int downspin[16] = {
   0,1,
   14,2,
   4,
   3,5,
   7,8,
   6,9,10,11,12,13,
   15
   };

void spinpalette(void)
{
   int i;
   char spinners[16];

   memcpy(spinners,egacolortable,16);

   for(i=0;i<16;i++)
     egacolortable[i] = spinners[locked ? upspin[i] : bigupspin[i]];
   SetEGAPalette();
}

void revspinpalette(void)
{
   int i;
   char spinners[16];
   memcpy(spinners,egacolortable,16);
   for(i=0;i<16;i++)
     egacolortable[i] = spinners[locked ? downspin[i] : bigdownspin[i]];
   SetEGAPalette();
}







void ForceCursorOn(void)
{
   short cx,cy,cl,cb;

   while(1)
   {
      ShowCursor();
      QueryCursor(&cx,&cy,&cl,&cb);
      if (cl == 0)
         break;
   }
}



/* OK, let's try a BIG box */

static cursor bigbox_screenmask = {
   16,16,0,2,1,1,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static cursor bigbox_cursormask = {
   16,16,0,2,1,1,
   0xff,0xff,
   0xff,0xff,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xc0,0x03,
   0xff,0xff,
   0xff,0xff
   };





static cursor diamond_screenmask = {
   16,16,0,2,1,1,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*
static cursor diamond_cursormask = {
   16,16,0,2,1,1,
   0xfe,0xfe,
   0xfc,0x7e,
   0xf8,0x3e,
   0xf0,0x1e,
   0xe0,0x0e,
   0xc0,0x06,
   0x80,0x02,
   0xc0,0x06,
   0xe0,0x0e,
   0xf0,0x1e,
   0xf8,0x3e,
   0xfc,0x7e,
   0xfe,0xfe,
   0,0
   };
*/
static cursor diamond_cursormask = {
   16,16,0,2,1,1,
   0xff,0xf8,  /* 1111111111111 */
   0xfd,0xf8,  /* 1111110111111 */
   0xf8,0xf8,  /* 1111100011111 */
   0xf0,0x78,  /* 1111000001111 */
   0xe0,0x38,  /* 1110000000111 */
   0xc0,0x18,  /* 1100000000011 */
   0x80,0x08,  /* 1000000000001 */
   0xc0,0x18,
   0xe0,0x38,
   0xf0,0x78,
   0xf8,0xf8,
   0xfd,0xf8,
   0xff,0xf8,
   0x00,0x00,
   0x00,0x00,
   0x00,0x00
   };

void MakeOurCursor(void)
{
   /* We like a little box, about 8x8, we don't like the 16x16 one
      anywhere near as much. */



//   DefineCursor(7,0,0,&boxcursor_screenmask,&boxcursor_cursormask);
   // commenting out this next line makes it an xor cursor... */
   bigbox_screenmask = bigbox_cursormask;
   DefineCursor(7,8,8,&bigbox_screenmask,&bigbox_cursormask);
//   DefineCursor(6,0,0,&trianglecursor_screenmask,&trianglecursor_cursormask);
   diamond_screenmask = diamond_cursormask;
   DefineCursor(6,8,8,&diamond_screenmask,&diamond_cursormask);

}

static int last_protected_row=-1;
void _ProtectCurrentRow(void)
{
   rect R;
   R.Xmin = minx;
   R.Xmax = sR.Xmax;
   R.Ymin = max(pixely - 16,0);
   R.Ymax = min(pixely + 16,maxy);

   /* Maybe unprotect, if curx and cury are not within range. */
   if (cury < R.Ymin || cury > R.Ymax) 
      ProtectOff();
   ProtectRect(&R);
   last_protected_row = pixely;
}


void ProtectCurrentRow(void)
{

   if (pixely != last_protected_row)
      _ProtectCurrentRow();
}

#define BOXCURSOR 7
#define ARROWCURSOR 0

int cursortype;
int lastcursorshape = -1;

void BoxCursor(void)
{
   static mapArray cmap1 = {7,7,7,7,7,7,7,7};
   static mapArray cmap2 = {6,6,6,6,6,6,6,6};
   if (cursortype != BOXCURSOR || cursorshape != lastcursorshape)
      CursorMap(cursorshape ? cmap2 : cmap1);
   cursortype = BOXCURSOR;
   lastcursorshape = cursorshape;
}

void ArrowCursor(void)
{
   static mapArray cmap = {0,0,0,0,0,0,0,0};
   if (cursortype != ARROWCURSOR)
      CursorMap(cmap);
   cursortype = ARROWCURSOR;
}

void CrossCursor(void)
{
   static mapArray cmap = {6,6,6,6,6,6,6,6};
   if (cursortype != ARROWCURSOR)
      CursorMap(cmap);
   cursortype = ARROWCURSOR;
}


static int cursortypestack[20];
static int cursorpos = 0;

void PushCursor(void)
{
   if (cursorpos < 19)
      cursortypestack[cursorpos++] = cursortype;
}

void PopCursor(void)
{
   if (cursorpos)
   {
      int cct;
      cct = cursortypestack[--cursorpos];
      if (cct != cursortype)
      {
         switch(cursortype)
         {
         case ARROWCURSOR:
            ArrowCursor();
            break;
         case BOXCURSOR:
            BoxCursor();
            break;
         }
      }
   }
}





