#ifndef _ATTRDEFSH_
#define _ATTRDEFSH_
#define sign(x)		((x)<0 ? -1 : 1)
#define XPIXELS 640
#define MAXFLOCK 80		/* 4 x 4 x 4 cube biggest 3D, 8x8 big 2d */
#define MAXRIBBONLENGTH 128
#define PI 3.14159265358979
#define STAKE 2
#define WIDTH 2
#define LENS 1.5
#define FLYPULSE 255
#define STARTDT 0.01
#define _colorize(x) (available_colors[(x) % 10])
#define colorize(x) (locked ? _colorize(x) : ((x) % 15 + 1))
//#define colorize(x) (( (x) % maxcolor) + 1)
/* our logistic map has form y = x * y * ( 1- y) */
#define FLOX1START  0.0
#define FHIX1START  3.999
#define FLOY1START  0.0
#define FHIY1START  1.0
#define FLOZ1START  0.0
#define FHIZ1START  1.0
/* henone maps are smallish */
#define FLOX2START -1.5
#define FHIX2START  1.5
#define FLOY2START -1.0
#define FHIY2START  1.0
#define FLOZ2START  0.0
#define FHIZ2START  1.0
/* Lorenz map needs quite a bit of room */
#define FLOX3START -30.0047	/* so origin isn't likely to get hit */
#define FHIX3START  31.0
#define FLOY3START -40.0022
#define FHIY3START  31.0
#define FLOZ3START  -3.00461
#define FHIZ3START  70.0
/* Yorke map is trigonometric */
#define FLOX4START  0.0		/* so origin isn't likely to get hit */
#define FHIX4START  1.0
#define FLOY4START  0.0
#define FHIY4START  1.0
#define FLOZ4START  0.0
#define FHIZ4START  1.0
#define LOGSTART 0.217
#define STARTBEAT  3.8
#define STARTMOTION 3.57
#define A110  -0.26813663648754
#define B110   0.98546084298505
#define A210   0.08818611671542
#define B210   0.99030722865609
#define A101  -0.91067559396390
#define B101   0.50446045609351
#define A201  -0.56502889980448
#define B201   0.33630697012268
#define A111   0.31172026382793
#define B111   0.94707472523078
#define A211   0.16299548727086
#define B211   0.29804921230971
#define A11m1 -0.04003977835470
#define B11m1  0.23350105508507
#define A21m1 -0.80398881978155
#define B21m1  0.15506467277737
#define TWOPI  ( 2 * PI )
#define EPSBAR1 1.0 / (4 * PI)	/* epsilon 0.5 */

typedef struct
{
	double vlox, vhix, vloy, vhiy;
} view;

typedef struct
{
	double x, y;
} fpair;

typedef struct
{
	double x, y, z;
} ftriple;

typedef struct
{
	int x, y;
} pair;

typedef struct
{
	pair atom[MAXFLOCK+1];
} screenflock;

typedef struct
{
	double atom[XPIXELS];	/* screenwidth */
} fflockstruct1;

typedef struct
{
	int n;
	fpair atom[MAXFLOCK+1];
	unsigned char color[MAXFLOCK];
   unsigned long releasetime[MAXFLOCK];
} fflockstruct2;

typedef struct
{
	int n;
	ftriple atom[MAXFLOCK+1];
	unsigned char color[MAXFLOCK];
   unsigned long releasetime[MAXFLOCK];
} fflockstruct3;
#define	ls 10.0
#define	lb (-8.0/3)
#define	lr  28.0
#define	ha  1.4
#define	hb  0.3
#define MENUBACK MenuBackColor
#define MENUTEXT MenuTextColor
#define BUTTONBACK ButtonBackColor
#define BUTTONFRAME ButtonFrameColor
#define BUTTONTEXT ButtonTextColor

#define LOGISTIC 1
#define HENON 2
#define LORENZ 3
#define YORKE 4
#define BUTTONHEIGHT   (FontHeight + 8)
#define MAX_NUMBER_ENTRIES 16
#define NUMBER_FVAL 0
#define NUMBER_IVAL 1
#define NUMBER_TVAL 2

typedef union {
   double fval;
   int ival;
   char *tval;
} anyval;

typedef struct {
   char *prompt;
   char *longprompt;
   int typeflag;
   anyval v;
   anyval min;
   anyval max;
   rect tR;
   rect nR;
} number_entry;

typedef struct {
   char *title;
   number_entry **entry; /* NULL TERMINATED */
   rect rejectR;
   rect acceptR;
   rect helpR;
   rect R;
} numbers;




typedef struct {
   ftriple viewbinormal;
   ftriple viewnormal;
   ftriple viewspot;
   double accuracy;
   char view;
   double flox,floy,fhix,fhiy,floz,fhiz;
   int trihedroncount;
   int end_trihedroncount;
   int ribbonlength;
   int tracetype;
   fflockstruct3 *fflock3ptr;
} lorenz_str;

typedef struct {
   int tracetype;
   int startlog;
   int stoplog;
   double humpspot;
   double lvfx;
   double flox,floy,fhix,fhiy;
   fflockstruct1 *fflock1ptr;
} logistic_str;

typedef struct {
   double fha;
   int tracetype;
   int ribbonlength;
   double flox,floy,fhix,fhiy;
   int fancyflag;
   int flocktype;
   fflockstruct2 * fflock2ptr;
   int shiftregister;
} henon_str;

typedef struct {
   double epsilon;
   int yorketopologyflag;
   int tracetype;
   int ribbonlength;
   double omega1,omega2;
   double flox,floy,fhix,fhiy;
   int flocktype;
   int shiftregister;
} yorke_str;

typedef struct {
   int dimension;
   union {
      logistic_str lo;
      yorke_str y;   
      henon_str h;
      lorenz_str lz;
   } s;
} stamp_structure;









#include "scodes.h"




#endif
