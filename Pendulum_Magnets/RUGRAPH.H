/* rugraph.h
This is the header to go with rugraph.c, containing my graphics
and mouse functions.
*/

#define sign(x)		((x)<0 ? -1 : 1)

typedef struct {
	int	x,y;
} pair;
/*--------------Global Variables-----------*/

int mouseflag = 1;		/* assume mouse present, set flag to 0 if none */
int projector = 0;  /* 1 means use max color for liquid crystal projector

/* To avoid using the TC .BGI files (which do not support mode 0x13 in
   any case), I keep  my  own  graphics  info.*/

struct graphinfo {
	int maxx, maxy; unsigned char maxcolor;
	}
	modeinfo[0x14] = {
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		319,199,3,	      /* 4 = CGA */
		0,0,0,
		39,199,1,	      /* 6 = HiCGA */
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		639,349,15,	      /* 0x10 = EGA */
		0,0,0,
		639,478,15,	      /* 0x12 = VGA */
		319,199,255	      /* 0x13 = MCGA */
	};

/*-----External Function Declarations-------*/

extern unsigned int sixteenbits(void);
/*  Uses 16 bit rule 30 shifts to give a fast random integer*/

extern void rsetmode(int m);
extern int rgetmode(void);
/*  setmode and getmode use int 10h functions 0 and F*/

extern void changepalette();
extern void usepalette();
/*  The assembly part of the program maintains a cgapalettenumber, an
   egacolortable, and a vgacolortable.  changepalette alters what is
   in these data regions, and usepalette uses int 10h functions to
   invoke the new palettes.*/

extern void cgapixel(int i, int j, unsigned char color);
extern void egavgapixel(int i, int j, unsigned char color);
extern void mcgapixel(int i, int j, unsigned char color);
/*  These are direct video writes, with (i,j) pixel coordinates.*/

extern void mouse(int a,int b,int c,int d);
/*  mouse(a,b,c,d) loads a,b,c,d into ax,bx,cx,dx and invokes int 33h
   to call the mouse function number a, which may return info to the
   ax,bx,cx, and dx registers.*/
/*-----------------------Function Declarations--------------*/
int	detectmode(void);
void	installmode();
void restoretextmode();
void pixel(int i, int j, unsigned char color);
void xpixel(int i, int j, unsigned char color);
void message(char s[]);
void	helpscreen();
void mousereset();
int mousebutton();
pair mousemotion();
void wbresen(int xa, int ya, int xb,int yb,unsigned char color,int width);
void xwbresen(int xa,int ya, int xb,int yb,unsigned char color,int width);
void bresen(int xa, int ya, int xb,int yb,unsigned char color);
void xbresen(int xa,int ya, int xb,int yb,unsigned char color);
