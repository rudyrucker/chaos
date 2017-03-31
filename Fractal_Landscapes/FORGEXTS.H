/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal codes of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.	                                                     *						                                                           *
******************************************************************************/

#ifndef _FORGEXTSH_
#define _FORGEXTSH_

#define EV extern void
#define EI extern int
#define EF extern float
#define ED extern double
EV HLine(int,int,int,char,int);
EV Centers(rect *,int *,int *);
ED gauss(void);

EV SetPixel10(int,int,char);
EV makedtab(int levels, double gamma, int rgbmap[][3],
	       int divtab[256], int modtab[256], int dith16[16][16]);
EV zap_in_row(unsigned char *buffer,int x,int y,int width);
EV blattbytes(void *,int,int,int);
EV SplitRectV(rect *R,rect *R1,rect *R2);
EV SplitRectH(rect *R,rect *R1,rect *R2);
EV uparam(int);
EV initgauss(unsigned int);
EV initialize_forgery(int who, double fd, double powscale, int n, float **a);
EV initplanetcolors(void);
EV genplanet(float *a, int n, double rmax,
   double rmin, double shang, double siang,double elevfac);
EV initcloudcolors(void);
EV genclouds(float *,int);
EV genproj(float *,int,double);
EV initpurecolors(void);
EV genpures(float *,int);
EV render(void);
EI select_file(char*,char*,char*,char*);
EI GifOutput(char*,int);
EV initseed(void);
EV PushRect(rect *,int *);
EV hsv_rgb(unsigned long h, unsigned int s, unsigned int v, unsigned int *r, unsigned int *g, unsigned int *b);
EV rowblast(unsigned char far *,int,int,int);
EI ShadowAndSave(rect *);
EV jarrow(int,int,int,int,int,int);
EV invert_item_round(rect *);
EI jGetString(char *,char *,int);
EI _jGetString(char *,char *,int,GS_TYPE);
EV PushCursorPosition(void);
EV PopCursorPosition(void);
EV helptext(char *);
EV move_to_corner(rect *);
EV PopRect(int *);
EV DrawTextRect(short NumLines,
		   short MaxChWidth,
		   short GapX,
		   short GapY,
		   short FillColor,
		   short bordercolorQ,
		   unsigned char DoSave,
		   rect * R,
		   int *localerr);
EV PaintMainWindow(void);
EV tick(long);
EV InitTicker(char *,long);
EV CloseTicker(void);
EV initmountaincolors(void);
EI SomethingWaiting(void);
EV PushButton(rect *R,int inout);
EV LimitMouseRect(rect *R);
EV options(void);
EV TempFileName(char *,char *);
EV spectralsynth(float **x, int n, double h);
EV ScaleMountain(float *a,int n,double hgtfac);

EV VGAPaintRect(rect *R,char);
EV VGAHLine(int,int,int,char);
EV VGAVLine(int,int,int,char);
EV VGAOval(rect *,char);
EV VGALine(int,int,int,int,char);

EV do_animation(void);
EV do_rendering(void);
EV animate(void);
EV initialize_memory_meter(void);
EV RangeError(char *);
EV ErrorBox(char *);

EI Overwrite(char *);
EI cancel_ok_msg(char *);
EV SetMeshButton(void);
EV SetTermsButton(void);
EV SetAllSliders(void);
EI GifDisplay(char *);
EV dxfout(void);
EV SaveParameters(char *);
EV LoadParameters(char *);
EV gif_viewer(void);
EV files(void);
EV view_animation(void);
EV AbortCheck(void);


EV BasicCenteredBox(rect *dR,int width,int height,int bcolor,char *title,int titlecolor);
EV WaitForNothing(void);
EV clear_uparams(void);
EV PushPlanetRects(void);
EV PopPlanetRects(void);
EV StampParameters(double fracdim,double elevfac,double power,
   int mesh,int terms,double vturn,double vdown,double siang,double shang);

ED round(double);
EV InfoBox(void);
EV FileError(char *,FILE *);
EV DisplayString13(char *t,int col,int row);
EV DisplayRseed(void);
EV MoveTicker(void);
EV ProcessKey(int X,int Y,int *ci,int *key,rect *bR[],int items,int n,event *e);




void  *_Cdecl j_calloc  (size_t __nitems, size_t __size);
void   _Cdecl j_free  (void *__block);
void  *_Cdecl j_malloc  (size_t  __size);
void far  * _Cdecl j_farcalloc  (unsigned long __nunits, unsigned long __unitsz);
void _Cdecl      j_farfree  (void far *__block);
void far  *_Cdecl j_farmalloc (unsigned long __nbytes);
char *j_strdup(const char *s);								
int memok(long __nbytes);

#ifndef _ALLOCC_
#define calloc(n,s) j_calloc(n,s)
#define free(n) j_free(n);n=NULL
#define malloc(n) j_malloc(n)
#define farcalloc(n,u) j_farcalloc(n,u)
#define farfree(n) j_farfree(n);n=NULL
#define farmalloc(n) j_farmalloc(n)
#define strdup(n) j_strdup(n)
#endif

#undef EV
#undef EI
#undef EF
#undef ED



#endif
