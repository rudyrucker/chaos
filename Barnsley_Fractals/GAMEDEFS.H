#ifndef _BARNDEFSH_
#define _BARNDEFSH_
#define HUH

#include "scodes.h"
#define COLORINGSTYLECOUNT 9
#define RCOLORIZE(x)  (x) &= 15; if (!(x)) (x)=15
#define _colorize(x) (available_colors[(x) % 10])
#define colorize(x) (locked ? _colorize(x) : ((x) % 15 + 1))
#define JCOLORIZE(x) x = colorize(x)
//#define JCOLORIZE(x)  (x) &= 15; x = joshcolor[x]
#define MENUBACK MenuBackColor
#define MENUTEXT MenuTextColor
#define BUTTONBACK ButtonBackColor
#define BUTTONFRAME ButtonFrameColor
#define BUTTONTEXT ButtonTextColor

#define sign(x)		((x)<0 ? -1 : 1)

#define MAXBARNMAPS 20
#define MAXFLOCK 3500
#define FLOXSTART -2.3
#define FHIXSTART  2.3
#define FLOYSTART -2.1
#define FHIYSTART  2.1

/* biggest flock is the iconflock, which is a picture of the square,
which is 2*640 + 2*480 + ~400 eye + ~400 mouth =  3040.  There are 5
bytes per flock item so flock is about 15,200 bytes, less than a quarter
of a memory segment. We'll plan for 16,000.*/


/* The	basic  operation  of the  program is that I have a flock of at
   most MAXFLOCK  pairs  atom[i],  and  a  set  of  at  most  MAXBARNMAPS
   functions h[j].  During each step, each atom[i] is replaced by its
   barnimage under one of the barnmaps h[j].  A weighted randomizer is used to
   decide which h[j] to use on a given atom[i]. */

#define MAXINDEX 1024
#define BUMP 64
#define WEIGHTINC 0.2

/* The interval from 0 to MAXINDEX is divided into bands corresponding
   to the different possible h[j]'s.  The bands will be  specified  by
   giving a hicut[j] value for each j.	My randomizer returns an index
   between 0 and MAXINDEX, and if hicut[j-1] <= index < hicut[j], then
   the barnmap  h[j] will be chosen.  If I want to make a particular h[j]
   more likely, I increase hicut[j] and  decrease  hicut[j-1]  by  the
   amount  BUMP.   (I  do not bump above MAXINDEX or below 0, and when
   moving the bounds of one band, I adjust the	bounds	of  the  other
   bands.)  This  INDEX and BUMP combination represents an increase of
   12% probability per bump. I use this when adding probability to a
   map because otherwise I would need to figure out where to find some
   usable weight.  When I am subtracting probablility from a map, I
   work with the realvalued weight[j] of that map.  I have routines
   cutstoweight and weighttocuts which connect the two representations
   of weight.*/

#define BLOAT 0x400  /* we started with 0x800, tried 0x200 */

/* This program gains speed by using integer arithmetic.  The idea  is
   instead  of	working  with  real  numbers  fx  and  fy, I work with
   integers x and y, where x=BLOAT*fx and y=BLOAT*fy.  x is thought of
   as  representing the real number (x/BLOAT).	To correctly compute a
   product x *= y, I  actually	set  x	=  (x  *  y)  /  BLOAT.   This
   computation is done in assembly language by the sequence

	   mov ax,_x
	   mov bx,_y
	   mul bx
	   REPT SHIFT
		   shl dx,1
		   rcr ax,1
	   ENDM
	   mov _x,ax

   The	constant SHIFT is defined so that BLOAT = 1 << SHIFT.  In this
   program I take SHIFT = 11,  which  means  that  in  a  sixteen  bit
   integer  x, I use 1 bit for the sign, 4 bits for the integral part,
   and 11 bits for the fractional  part,  allowing  me	to  deal  with
   positive  and  negative reals with absolute value between 2 ^ (-11)
   and 16 - 2 ^ (-11), meaning absolute  value	between  .0004882  and
   15.999512.  A number less than .0004882 will appear to be zero, and
   a number greater than 15.99512 will overflow into the sixteenth (or
   higher)  bits,  and	give  spurious results.  The SHIFT constant is
   declared in barna.asm. */

typedef struct
{
	double a, b, c, d, e, f;
} fbarnmap;

/* A barnmap is an "affine" or "double linear" barnmap in the plane of
   the form:

	newx = a * x + b * y + e;
	newy = c * x + d * y + f;

   In matrix form this would be :

	(newx)   (a  b)   (x)   (e)
	(    ) = (    ) * ( ) + ( )
	(newy)   (c  d)   (y)   (f)

    Such a barnmap is specified by its action on 3 points	(a  triangle).
    Such  barnmaps	can perform scaling, rotation, translation, and shear.
    In general a barnmap must actually shrink the area of some  (and  thus
    all)  test	squares  or  it  will  not,  upon iteration, lead to a
    limiting fractal.  This procedure will  also  work	for  nonlinear
    barnmaps,  provided  they  are "contractive" in the sense of shrinking
    things.  Using a system of two barnmaps  (the  positive  and  negative
    pair  square  roots)  will	in fact produce Julia sets.  Note also
    that there is an obvious generalization to 3D.  */

typedef struct
{
	double x, y;
} fpair;			/* one could go to "triple" for 3D */

typedef struct
{
	int n;
	fbarnmap h[MAXBARNMAPS];
	double weight[MAXBARNMAPS];
	fpair fixedpoint[MAXBARNMAPS];
} ffunctionsystem;

/* This non-integer type is used for the highest level interface,
  and for the weighting function. */

typedef struct
{
	int x, y;
} pair;				/* go to "triple" for 3D */

typedef struct
{
	int a, b, c, d, e, f;
} barnmap;

typedef struct
{
	int n;
	barnmap h[MAXBARNMAPS];
	int hicut[MAXBARNMAPS];
	pair fixedpoint[MAXBARNMAPS];
} functionsystem;

/* The hicut[n] determine the probabilities of selecting the h[n],  as
   mentioned  in  the  comment	after BUMP above.*/

typedef struct
{
	int n;
	pair atom[MAXFLOCK];
	unsigned char color[MAXFLOCK];
} flockstruct;

typedef struct
{
   int n;
   fpair fatom[MAXFLOCK];
} fflockstruct;



/* Now a reasonable way of canning the fuckers. */
typedef struct {
   fbarnmap h;
   double weight;
} mapper;

typedef struct {
   int n;
   mapper *maps;
   char name[32];
   double lox,loy,hix,hiy;
   int dirty;
} barnmap_set;

enum {
   F1HELP,F2FILE,F3MAPS,F4TWEAK,F5OPTS,ALTXEXIT
};

enum
{
	CURMAP,ZROT, XROT, XSCALE, YSCALE, XTRANS, YTRANS, WEIGHT, INCREMENT, DISPLAYMODE,
   ADDMAP,KILLMAP
};


#define TWEAKBASE (ALTXEXIT+5)

#define LEFTARROWBOX (TWEAKBASE+30)
#define RIGHTARROWBOX (LEFTARROWBOX+7)

#define FDELTAXLIMIT 1024
#define FDELTAXMINIMUM 1e-1
#define BUTTONHEIGHT ((mode == 0x12) ? 8 : 6)
#endif


