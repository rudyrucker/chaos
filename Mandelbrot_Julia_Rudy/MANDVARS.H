#ifndef _MANDVARSH_
#define _MANDVARSH_
#include <time.h>
extern int mode;
extern char egacolortable[];
extern char vgacolortable[];
extern char *ega_lut;
extern char *vga_lut;
extern char activedactable[17];
extern int palettenumber;
extern int minx;
extern rect sR;
extern int StringWidthX;
extern int FontHeight;
extern stampdata stampviews[STAMPCOUNT];
extern view mandelview,startview,cubicview,ruckerview;

extern int mandelflag,cubicflag;
extern int ruckerflag,juliaflag,newjuliaflag;
extern int insideflag;
extern char *SpecifiedGifName;
extern int maxy,maxx,mode;
extern int SaveMe;
extern int palettenumber;
extern double bandsize;
extern int spinflag;
extern int grayflag;
extern unsigned char defaultpallette[64][3];
extern int maxiteration;
extern int slicetype;
extern int saveptr;
extern int curx,cury;
extern int pixelx,pixely;
extern int cursortype;
extern int cursorshape;
extern long x,y,u,v,a,b,nega,negb,a3,b3,bloat;
extern long lox,hix,loy,hiy;
extern double deltax,deltay,fx,fy;
extern int floatflag,slicetype,newsliceflag,fourdeeflag;
extern double flox,fhix,floy,fhiy,fu,fv,fa,fb,fa3,fb3;
extern int spiraldirection, spiralrun, spiralbaserun;
extern int autozoomflag,exitflag,doneflag,openflag;
extern int SpiralPass;
extern int SpiralMode;
extern int AutoGIF;
extern int tile;
extern int oddrowflag;
extern int OneScreenDone;
extern int QuickMode;
extern int centercursorflag;
extern time_t starttime;
extern int BoxActive;
extern int (*fractal) (void);
extern int chip;
extern unsigned char *savearray[STAMPCOUNT];
extern int  DrawZoomBox;
extern double zoomfactor;
extern int last_saved;
extern int stampstarts[8];
extern int testcenterflag, testcentercount;
extern int oldcolorband;
extern unsigned char iteration;     /* why a uchar? */
extern unsigned char *colorval;
extern int Frozen;
extern int soundflag;
extern int curxinc,curyinc;
extern int curspeed;
extern int acceptable_colors[];
extern char defaultcolortable[];
extern int MyTypeFlag;
extern char *slicename_t[];
extern char *typename_t[];
extern char *fillname_t[];
extern unsigned char defaultpalette[64][3];
extern unsigned char startct[17];
extern int ParameterDisplayMode;
extern rect stamprects[8];
extern char mand12_img[];
extern char mand10_img[];
extern int VeryFirstTime;
extern int fractintmode;
extern int lastrowpainted;
extern int CommPort;
extern int safe_alloc;
extern int prog_init;

/* these are all for fractint style ones */

extern int show_orbit;
extern int newflag;
extern int fractype, numpasses,inside;
extern long creal,cimag,lx0[],ly0[],delx,dely;
extern int xdots, ydots, maxit, colors, bitshift;
extern long lm;
extern int potflag,LogFlag;
extern void *LogTable;
extern int ixstart, ixstop, iystart, iystop;
extern int guessing;
extern int cpu;
extern int andcolor;
extern int show_orbit, orbit_ptr;
extern int symmetry, debugflag;
extern int orbit_color;
extern int singlepassmode;
extern bitmap *theBitmap;
extern fontRec *theFont;
extern metaPort *thePort;
extern double aspect;
extern int hasVGA;
extern int current_preset;
extern int current_main_item;
extern int forcefloat;
extern int locked;
extern int cycle_delay;

extern long starta,startb,startu,startv;
extern double startfa,startfb,startfu,startfv;





#endif
