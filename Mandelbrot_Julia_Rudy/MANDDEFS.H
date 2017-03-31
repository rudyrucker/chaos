#ifndef _MANDDEFSH_
#define _MANDDEFSH_
#define STAMPCOUNT 4
typedef struct
{
	int key;
	rect R;
} button;
typedef struct
{
	double vlox, vhix, vloy, vhiy;
} view;
typedef struct
{
	view v;
	int mandelflag, cubicflag, insideflag;
	int ruckerflag, juliaflag;
	double fa, fb;
	double fu, fv;
   int iterations;
} stampdata;
typedef struct
{
	int x, y;
} pair;

#define BUTTONBACK DARKGRAY
#define BUTTONFRAME BLACK
#define BUTTONTEXT YELLOW
#define BUTTONBOLD 3
#define MENUTEXT WHITE
#define MENUBACK BLUE


#define	PANFACTOR 0.5
#define	ITERATIONCAP 2000
#define	SPIRALBAILCOUNT 100
#define true 1
#define false 0

#define sign(x)		((x)<0 ? -1 : 1)

#define STAMPCOUNT 4

#define METASHEL
#define  MINIMUMAUDIBLE 4
#define  NULLTONE 30
#define  BASETONE 200
#define  TONERANGE 700

#define	ZOOMFACTOR 0.35
#define BUTTONBOUNCETIME 2

//enum {HORIZONTAL,VERTICAL};
#define BUTTONHEIGHT (FontHeight + 8)

#endif
