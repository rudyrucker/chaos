#ifndef _BARNEXTSH_
#define _BARNEXTSH_

#include "buttons.h"

#define EV extern void
#define EI extern int
#define ED extern double
#define EF extern fpair
#define EP extern pair
#define EC extern char


EV PushRect(rect *,int *);
EV PopRect(int *);
EV erasecursor(void);
EV drawcursor(void);
EV installmode(void);
EV _installmode(void);
EV setfwindow(void);
EV _setfwindow(void);
EV setiwindow(void);
EV fillflock(void);
EI coloring(pair *p, int which, int i);
EV resetcursor(void);
EV setstartwindow(void);
ED det(double,double,double,double);
EV weighttocuts(ffunctionsystem * K, functionsystem * L);
EV show(pair *,unsigned char);
EV fshow(fpair *,unsigned char);
EV showfixedpoints(functionsystem * K, ffunctionsystem * L);
EV iconflock(void);
EV step(void);
EV sixteenflock(void);
EV frameflock(void);
EV squareflock(void);
EV checkkeyboard(void);
EI checkmouse(void);
EI choose(void);
EF fbarnimage(fbarnmap *h, fpair *w);
EI sixteenbitsa(void);
EV bumpup(int n);
EV	bumpdown(int n);
EV flocktofflock(void);
EV fflocktoflock(void);
EP barnimage(barnmap *h, pair *z);
EV balanceweight(ffunctionsystem * L, int i);
EV Centers(rect *,int *,int *);
EV activate(ffunctionsystem *);
EV _activate(ffunctionsystem *);
EV __activate(ffunctionsystem *);
EP pixel(pair *z);
EP fpixel(fpair *z);
EV spinpalette(void);
EV revspinpalette(void);
EV randomizeifs(void);
EV mousereset(void);
EV detectmode(void);
EV LoadDefaultPalette(char *);
EV installmode(void);
EI LoadParams(char *);
EV resetcursor(void);
EV DrawButtons(void);
EV step(void);
EV checkkeyboard(void);
EV spinpalette(void);
EV revspinpalette(void);
EV restoretextmode(void);
EV changepalette(void);
EV usepalette(void);
EV updatecursor(void);
EI CheckLeftClicks(event *e);
EI CheckRightClicks(event *e);
EV palette_tweaker(void);
EV do_files_menu(void);
EV tweak(int);
EV clear_map_display(void);
EV do_view_menu(void);
EV helptext(char *);
EV ShowAll(void);
EV UpdateAllParams(int);
EI cancel_ok_msg(char *);
EV ForceCursorOn(void);
EV egavgapixel(int,int,int);
EC egavgagetpixel(int,int);
EV reposition_slider_v_base(slider * s, int selected, int floater, int numbers);
EV reposition_slider_v(slider * s, int selected);
EV reposition_slider_X(slider * s, int X, int selected);
EI slider_height(slider * s);
EV create_slider_base(slider * s, rect * mR, int x, int y, int floater, int tinies,
			 int frame, int numbers);
EV create_slider(slider * s, rect * mR, int x, int y);
EI select_file(char *,char *,char *,char *);
EV move_to_corner(rect *R);
EV GifOutput(char *,int);
EI GifDisplay(char *);
EV Bfint(ffunctionsystem * K, functionsystem * L);
EV blast_n(unsigned,unsigned);
EV SplitRectV(rect *,rect *,rect *);
EV SplitRectH(rect *,rect *,rect *);
EP pixel_from_barnimage(barnmap *h,pair *z);
EP fpixel_from_barnimage(fbarnmap *h,fpair *z);

EV invert_tweak_item(int n);
EV invert_main_item(int n);
EV invert_item_round(rect *R);

EI process_minus_key(int);
EI process_plus_key(int);
EV awaken(void);
EV UpdateMapParam(int click);
EV reawaken(int);
EV ShowValues(void);
EV ShowOneValues(int);
EV CheckMainDrags(event *e);
EV CheckTweakDrags(event *e);

EF antifpixel(pair *p);
EP antipixel(pair *p);

EV OutlineCurrentMap(void);
EV CheckDrags(event *);
EI CheckMenuKeys(int);
EV PushCursorPosition(void);
EV PopCursorPosition(void);

EV InitializeCursors(void);
EV ArrowCursor(void);
EV TriangleCursor(void);
EV ShowWeights(void);
EV ConvertParams(int);
EV dozoom(int);
EV DrawTextRect(short,short,short,short,short,short,unsigned char,
		   rect *,int *);
EI jGetString(char *,char *,int);
EI _jGetString(char *dest, char *start, int maxlen, GS_TYPE type);
EV recenter(void);
EV add_new_barnmap(void);
EV PushBarnmap(void);
EI PopBarnmap(void);
EV ClearBarnmapStack(void);
EV _fOutlineMap(int map,int color,int number);
EV zoombox(void);
EV CheckCornerDrags(int,int);
EV MakeStamp(void);
EV install_fill_outline(void);
EV RangeError(char *);
EI process_main_click(int);
EI CheckTweakClicks(event *e);
EV WaitForNothing(void);
EV ShiftRight(void);
EV ShiftLeft(void);
EV restore_stamp(int);
EI BaseBarnmap(void);
EV HLine(int,int,int,int);
EV invert_item_rect(rect *R);
EV invert_item_round(rect *R);
EI ShadowAndSave(rect *R);
EV WeightEditor(void);
EV _useVGApalette(unsigned char *);
EV _useEGApalette(unsigned char *);
EV changepalette(void);
EV randompalette(void);
EV presetpalette(void);
EV SaveParams(char *);
EV load_parameters(void);
EV SaveImageGif(char *);
EV SaveScreenGif(char *);
EV gif_viewer(void);
EV dodefaultpalette(void);
EV jarrow(int x, int y, int width, int height, int border, int fill);
EV rowblast(char *,int,int,int);
EV bump_edmap(void);
EV change_display_mode(void);
EV ErrorBox(char *);
EV LimitMouseRect(rect *R);
EI Overwrite(char *);
EV PaintQuitButton(rect *R,int depress);
EV grayscale(void);
EV PushCursorType(void);
EV PopCursorType(void);
EV PressButton(int,int);
EV InitializeStamps(void);
EV	PushMouseRectLimit(rect *);
EV PopMouseRect(void);
EV ErrorBox(char *);
extern unsigned long realfarcoreleft();
extern FILE *OpenWithError(char *,char *);
EV w_normalize(double *dest, double *source);
EV InfoBox(void);
EV tw_add_new_barnmap(void);
EV ConvertParamsBack(int n);
EV zap_a_map(void);
EV FileError(char *,FILE *);
EV TempFileName(char *,char *);
EV SetCurrentWeight(void);
EI memok(long __nbytes);
extern short gif_decoder(short l);
EV JMoveCursor(int,int);
EV fillfixedpoints(ffunctionsystem * K);

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
#define getcwd(n,u) j_getcwd(n,u)
#endif

#undef EV
#undef EI
#undef ED
#undef EF
#undef EP
#undef EC


#endif
