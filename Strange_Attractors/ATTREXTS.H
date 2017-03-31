#ifndef _ATTREXTSH_
#define _ATTREXTSH_

#include "buttons.h"

#define EV extern void
#define EI extern int
#define ED extern double

//EV setmode(int m);
EI getmode(void);
EV egavgapixel(int i, int j, int color);
EV segavgapixel(int i, int j, int color);
EV xegavgapixel(int i, int j, int color);
EV drawcursor(void);
EV erasecursor(void);
EV mouse(int a, int b, int c, int d);
EV changepalette(void);
EV usepalette(void);
EV spinpalette(void);
EV revspinpalette(void);
EV grayscale(void);
EI sixteenbitsa(void);
EV setwindow(int);
ED fancyhump(double fx, double x, double y);
EV installmode(void);

EV setwindow(int);		/* call with 1 for start setting, 0 otherwise */
EV lstep(void);
EV checkkeyboard(void);
extern unsigned int checkmouse(event *e);
EV monoflock(void);
EV dotflock(double, int);
EV patchflock(void);
EV fillflock(void);
EV addflock(void);
EV eraseribbon(int);
EV erasetrihedron(int);
ftriple lorenzderivs(ftriple *);
ftriple rk4lorenzimage(ftriple *, int *);
fpair henon(fpair *, int *);
fpair genhenon(fpair *, int *);
fpair yorke(fpair *);
pair projectpixel(ftriple *, char);
pair convertpair(fpair *);
pair screentostamp(pair *);
double fancyhump(double, double, double);
extern short gif_decoder(short);

EI detectmode(void);
EV installmode(void);
EV restoretextmode(void);
EV instruct(void);
EV helpscreen(void);
EV useview(void);
EV dozoom(int);
EV _dozoom(int,double,double);
EV dopan(int, int);
EV resetcursor(void);
EV updatecursor(void);
EV mousereset(void);
EI mousebutton(void);
pair mousemotion(void);
EV bresen(int xa, int ya, int xb, int yb, int color);
EV wbresen(int xa, int ya, int xb, int yb, int color, int width);
EV sbresen(int xa, int ya, int xb, int yb, int color);
EV swbresen(int xa, int ya, int xb, int yb, int color, int width);
EV xwbresen(int xa, int ya, int xb, int yb, int color, int width);
EI CheckMainMenu(int X,int Y);
EI CheckRightclickMainMenu(int X,int Y);

EV PushRect(rect *R,int *err);
EV PopRect(int *err);
EV helptext(char *);

EV move_to_corner(rect *R);
EV turn_off_box(rect *R);
EV turn_on_box(rect *R);
EV draw_radio_button_set(radio_button_set *,rect *,int,int);
EV popview(view * w);
EV pushview(view * w);
EV Jspinpalette(void);
EV revJspinpalette(void);
EV DrawButtons(void);
EV color_tester(void);
EV Jusepalette(void);
EV slide_stamps(void);
EV restore_data(int n);
EV preserve_data(void);
EV do_files_menu(void);
EV do_types_menu(void);
EV do_tweaks_menu(void);
EV do_view_menu(void);
EV show_data(int);
EV palette_tweaker(void);
EV randomize_yorkers(void);
EI cancel_ok_msg(char *msg);

EV Centers(rect *,int *,int *);
EV LimitMouseRect(rect *R);
EV ErrorBox(char *);
EV PushCursorType(void);
EV PushCursorPosition(void);
EI ShadowAndSave(rect *);
EV PushMouseRectLimit(rect *);
EI select_file(char *,char *,char *,char *);
EV PopMouseRect(void);
EV PopCursorType(void);
EV PopCursorPosition(void);
EI GifOutput(char *,int);
EV gif_viewer(void);
EV SaveImageGif(char *);
EV SaveScreenGif(char *);
EV LoadParams(char *);
EV Jsetpal(char *,char *);
EV SaveParams(char *);
EV zoombox(void);
EV Bres(int,int,int,int,int);
EV wBres(int,int,int,int,int,int);
EI GifDisplay(char *);
EI ShiftArrows(event *);
EV addrandomflock(void);
EV TempFileName(char *,char *);
EV GrayOut(rect *);
EV ArrowCursor(void);
EV SetButton(int,int);
EV presetpalette(void);
EV randompalette(void);
EV dodefaultpalette(void);
EV InfoBox(void);
EV announceparms(void);
EV LoadDefaultPalette(char *);
EV LockToClosestUpdown(void);
EV LockToClosestLeft(void);
EV MaybeHiliteSomething(void);
extern unsigned long realfarcoreleft(void);
EI Overwrite(char *);
EV rowblast(char far *,int,int,int);
EI memok(long __nbytes);
EV Pix(int,int,int);
EV clearGIFlines(int);

void  *_Cdecl j_calloc	(size_t __nitems, size_t __size);
void   _Cdecl j_free  (void *__block);
void  *_Cdecl j_malloc	(size_t  __size);
void far  * _Cdecl j_farcalloc	(unsigned long __nunits, unsigned long __unitsz);
void _Cdecl	 j_farfree  (void far *__block);
void far  *_Cdecl j_farmalloc (unsigned long __nbytes);
char   *_Cdecl j_strdup   (const char *__s);
char  *_Cdecl j_getcwd (char *_buf, int _buflen);

#ifndef _ALLOCC_
#define calloc(n,s) j_calloc(n,s)
#define free(n) j_free(n);n=NULL
#define malloc(n) j_malloc(n)
#define farcalloc(n,u) j_farcalloc(n,u)
#define farfree(n) j_farfree(n);n=NULL
#define farmalloc(n) j_farmalloc(n)
#define strdup(s) j_strdup(s)
//#define getcwd(n,u) j_getcwd(n,u)
#endif

#undef EV
#undef EI
#undef ED

#endif
