//#define SIZING
#include "menu.h"
#include <dir.h>
int CommPort;
static rect *bR[8];
static rect chaosR[8];
static int current_main_item = -1;
static int items;
double aspect;
extern char fontname[128];
int OurWhite;
int OurDarkGray;
int OurRed;
int OurLightGray;
extern void ErrorBox(char *);
extern void ailogo(int);
extern void _ailogo(int width,int height,int tlx,int tly,int color,int firsttimeflag,int fill);
extern void move_to_corner(rect *);
extern void InfoBox(void);


#define XOFFSET (280-98)
rect sR;
int FontHeight;
metaPort *thePort;
int StringWidthX;
char *msgs[] = {
   "1. The Mandelbrot Sets ",
   "2. Magnets and Pendulum",
   "3. Strange Attractors  ",
   "4. The Chaos Game      ",
   "5. Fractal Forgeries   ",
   "6. Toy Universes       ",
//   "F1 for HELP",
   "Alt-X to EXIT",
   };




point CircleCenters[] = {
   98+XOFFSET,40-3,
   170+XOFFSET,85-3,
   236+XOFFSET,134-3,
   293+XOFFSET,189-3,
   338+XOFFSET,248-3,
   375+XOFFSET,311-3,
   };

int CircleDiameters[] = {
   95,95,95,95,95,95
   };

rect CircleRects[6];
#define DIAMETER 85

char *logoname = "EGALOGO.MMG";
static short ct[17];
char ct2[17];

image *saved_circle = NULL;

int AbortCheck(void)
{
   event e;
   int key=0;

   if (!PeekEvent(1,&e))
      return false;

   if (e.ASCII && e.ASCII != 0xe0)
      key = e.ASCII;
   else if (e.ScanCode != 0xff)
      key = e.ScanCode << 8;

   return ((key >= '1' && key <= '6') || key == 0x1b);
}


static void push(int n,int inout)
{

   if (n < 0)
      return;


   HideCursor();

   if (n != 6)
   {
      int j;

      if (!inout)
         PopRect(&j);
      else
      {
         rect R = CircleRects[n];

         PushRect(&R,&j);
         PenColor(OurWhite);
         RasterOp(zREPz);
         FrameOval(&R);
         InsetRect(&R,1,1);
         FrameOval(&R);

      }

   }

   PushButton(&chaosR[n],inout);
   ExtraHilite(&chaosR[n],inout);
   ShowCursor();
}

void blastin(void)
{
   int i;
   rect R;
   FILE *ifd;
   int length;
   char *p;

   unsigned char far *image;
   SetRect(&R,0,0,639,119);
   length = (int)ImageSize(&R);
   image = (unsigned char far *)malloc(length);

   p = searchpath(logoname);
   if (!p)
   {
      ErrorBox("Can't find logo file");
      return;
   }

   ifd = fopen(p,"rb");
   fread(ct2,17,1,ifd);
   for(i=0;i<17;i++)
      ct[i] = ct2[i];
   setvbuf(ifd,NULL,_IOFBF,length);
   for(i=0;i<3;i++)
   {

      if (AbortCheck())
         break;
      if (i == 2)
      {
         R.Ymax = R.Ymin + 109;
         length = (int)ImageSize(&R);
      }
      fread(image,length,1,ifd);
      WriteImage(&R,image);
      OffsetRect(&R,0,120);
   }
   fclose(ifd);
   free(image);
}

rect cR;

void SetEGAPalette(void)
{
   union REGS regs;
   struct SREGS sregs;
   regs.h.ah = 0x10;
   regs.h.al = 0x2;
   regs.x.dx = FP_OFF(ct2);
   sregs.es = FP_SEG(ct2);
   int86x(0x10,&regs,&regs,&sregs);
}
#pragma argsused

int main(int argc, char **argv)
{
   int i;
	rect R;
   int retval = 999;
   int key;
   int curx,cury;
   int cx,cy;
   event e;
   short timex;
   char tbuf[128];
   int firsttime;
   int buttonwidth = 0;
   rect frameR;

   if (argc == 1)
   {
      fprintf(stderr,"This program must be run from CHAOS.BAT.\n\n");
      return retval;
   }

   firsttime = argv[1][0] == '1';

   items = 7;

   i = InitGrafix(-EGA640x350);
	if (i != 0)
   {
		printf("Error: Metagraphics not installed. Aborting.\n");
		exit(-1);
   }
   CommPort = QueryComm();
   PeekEvent(1,&e);

	CommPort = QueryComm();
	if (CommPort & MsDriver)
		CommPort = MsDriver;
	else if (CommPort & 2)
		CommPort = MsCOM2;
	else if (CommPort & 1)
		CommPort = MsCOM1;
	if (CommPort)
		InitMouse(CommPort);

	EventQueue(true);
	TrackCursor(true);
   GetPort(&thePort);
   sR = thePort->portRect;
	FontHeight = thePort->txFont->leading;
	StringWidthX = StringWidth("X");
   aspect = (double)thePort->portBMap->pixResX/
            (double)thePort->portBMap->pixResY;
   LimitMouseRect(&sR);
   ScaleMouse(16,24);
   SetDisplay(GrafPg0);

   ailogo(firsttime);
   if (argv[1][0] > '1')
   {
      sprintf(tbuf,"Not enough memory to run %s",msgs[argv[1][0] - '1' - 1] + 3);
      /* strip trailing blanks */
      while(tbuf[strlen(tbuf)-1] == ' ')
         tbuf[strlen(tbuf)-1] = 0;

      ErrorBox(tbuf);
   }

   SetBitmap(GrafPg1);
   blastin();
   ShowCursor();
   timex = e.Time;

   if (AbortCheck())
   {
      KeyEvent(true,&e);
      if (e.ASCII && e.ASCII != 0xe0)
         key = e.ASCII;
      else if (e.ScanCode != 0xff)
         key = e.ScanCode << 8;
      if (key >= '1' && key <= '6')
         retval = key - '0';
      goto done;
   }
   while(1)
   {
      int n= KeyEvent(false,&e);
      if (n || !firsttime || e.Time - timex > 2*18.2)
         break;
   }
   for(OurWhite=0;OurWhite<16;OurWhite++)
      if(ct2[OurWhite] == 63)
         break;

   for(OurDarkGray=0;OurDarkGray<16;OurDarkGray++)
      if(ct2[OurDarkGray] == 56)
         break;

   for(OurRed=0;OurRed<16;OurRed++)
      if(ct2[OurRed] == 36)
         break;

   for(OurLightGray=0;OurLightGray<16;OurLightGray++)
      if(ct2[OurLightGray] == 7)
         break;
      

   HideCursor();
   for(i=0;i<6;i++)
      CenterRect(&CircleCenters[i],CircleDiameters[i],(int)(CircleDiameters[i]/aspect) & ~1,&CircleRects[i]);


   PenColor(OurWhite);
   RasterOp(zXORz);


   for(i=buttonwidth=0;i<6;i++)
      buttonwidth = max(buttonwidth,StringWidth(msgs[i])+12);

   R.Xmin = 6;
   R.Xmax = R.Xmin + buttonwidth + 8;
   R.Ymax = 276;
   R.Ymin = R.Ymax - 7 * (3 * FontHeight/2+2) - 8;
   frameR  = R;
   PenColor(OurLightGray);
   RasterOp(zREPz);
   PaintRect(&R);
   PushButton(&R,false);
   ExtraHilite(&R,false);


   R.Xmin = 10;
   R.Xmax = R.Xmin + buttonwidth;
   for(i=0;i<6;i++)
   {

      R.Ymin = 276 - (7 - i) * (3 * FontHeight/2+2) - 4;
      R.Ymax = R.Ymin + FontHeight + 4;
      chaosR[i] = R;
      bR[i] = &chaosR[i];
      PaintRadioButtonBase(&R,false,false,msgs[i],OurDarkGray,OurRed,OurWhite);
      ExtraHilite(&R,false);
   }

   R.Ymin = 276 - (3 * FontHeight / 2) - 3;
   R.Ymax = R.Ymin + FontHeight + 4;
   chaosR[i] = R;
   bR[i] = &chaosR[i];
   PaintRadioButtonBase(&R,false,false,msgs[i],OurDarkGray,OurRed,OurWhite);
   ExtraHilite(&R,false);

   Centers(&chaosR[0],&cx,&cy);

//   _ailogo(buttonwidth/4-14,buttonwidth/4-14,cx,frameR.Ymax + 3,BLACK,false,true);

   CursorStyle(0);
   SetEGAPalette();
   SetDisplay(GrafPg1);




   R.Xmin = 0;
   R.Xmax = 100;
   R.Ymin = 0;
   R.Ymax = ((int)(100 / aspect)) & ~1;

   cR = R;
   Centers(&cR,&curx,&cury);

   current_main_item = 0;
   push(current_main_item,true);
   move_to_corner(&chaosR[current_main_item]);
   ShowCursor();


   while(retval == 999)
   {
      int n = KeyEvent(false,&e);
      int X = e.CursorX;
      int Y = e.CursorY;
      int button = (e.State & 0x700) >> 8;
      int last_item = current_main_item;
    
      key = 0;
      if (n)
      {
         if (e.ASCII && e.ASCII != 0xe0)
            key = e.ASCII;
         else if (e.ScanCode != 0xff)
            key = e.ScanCode << 8;

         if (button == swLeft)
            key = 0x0d;

         if (button == swRight)
            key = 0x1b;
      }
      else
      {
         point P;
         P.X = X;
         P.Y = Y;
         current_main_item = -1;
         for(i=0;i<items;i++)
         {
            if ((i < 6 && PtInOval(&P,&CircleRects[i])) || PtInRect(&P,&chaosR[i]))
            {
               current_main_item = i;
               break;
            }
         }

      }
      if (key == 0x1b || key == XALTX || key == XALTQ)
      {
         key = 0x0d;
         current_main_item = items - 1;
      }
      navigate(key,0L,0L,(int *)-1L,(int *)-1L,items,bR,&current_main_item);

      if (key >= '1' && key <= '6')
      {
         retval = key - '0';
         current_main_item = retval-1;
      }
      if (last_item != current_main_item)
      {
         push(last_item,false);
         push(current_main_item,true);
      }

      if (key == 0x0d)
      {
         if (current_main_item == -1)
            continue;
         if (current_main_item < 6)
         {
            retval = current_main_item + 1;
            break;
         }
         if (current_main_item == items - 1)
            break;

      }

      if (key == XALTW)
         InfoBox();
   }

   PaintRadioButtonBase(bR[current_main_item],true,true,msgs[current_main_item],OurDarkGray,OurRed,OurWhite);
   ExtraHilite(bR[current_main_item],true);
   WaitForNothing();
   PaintRadioButtonBase(bR[current_main_item],false,false,msgs[current_main_item],OurDarkGray,OurRed,OurWhite);
   ExtraHilite(bR[current_main_item],false);


done:
   HideCursor();

   if (retval == 999)
   {
      SetDisplay(TextPg0);
   	ClearText();
   }
	StopMouse();
	StopEvent();
   return retval;
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

