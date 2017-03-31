#ifndef _MAGEXTSH_
#define _MAGEXTSH_

#define ER extern rect
#define EI extern int
#define ED extern double
#define EV extern void

EV ErrorBox(char *);
EV PushCursorType(void);
EV PushCursorPosition(void);
EV Centers(rect *,int *,int *);
EI ShadowAndSave(rect *);
EV LimitMouseRect(rect *);
EV PushMouseRectLimit(rect *);
EV move_to_corner(rect *);
EI select_file(char *,char *,char *,char *);
EI GifOutput(char *,int);
EV PopMouseRect(void);
EV PopRect(int *);
EV PopCursorType(void);
EV PopCursorPosition(void);
EV helptext(char *);
EV PushRect(rect *,int *);
EI Overwrite(char *);
EV usepalette(void);
EV _useEGApalette(unsigned char *);
EV _useVGApalette(unsigned char *);
EV JHideCursor(void);
void showmagnet(int x, int y, unsigned char color);
EV JShowCursor(void);
EI ProcessControls(event *e,int n);
EV NewControlPage(int n);
EV F3_button(void);
EV F4_button(void);
EV F5_button(void);
EV F6_button(void);
EV F7_button(void);
EV F8_button(void);
EV changeVGApalette(void);
EV setdefaultpalette(void);
EV FLoadGif(void);
EV DoGifOutput(int ws);
EV FLoadLayout(void);
EV CannedLayout(void);
EV palette_tweaker(void);
EV ResetLayout(void);
EV FSaveLayout(void);
EV drawSoundButton(void);
EV InfoBox(void);
EV drawForceTypeButton(void);
EV drawCenterpullButton(void);
EV DecreaseCharges(void);
EV IncreaseCharges(void);
EV NewRadius(void);
EV drawFrictionButton(void);
EV drawFrequencyButton(void);
EV RandomizePositions(void);
EV RandomizeCharges(void);
EV ToggleBasins(void);
EV drawRadiusButton(void);
EI memorylength;
EV drawLengthButton(void);
EV resetLength(void);
EV saveconfig(configstructure *p);
EV installmode(void);
EV setuphole(int n);
EV drawTraceTypeButton(void);
EV drawCaptureButton(void);
EV updateonemagnet(int,long);
EV drawChargeButton(void);
EV loadcharges(void);
EV updateallmagnets(magnetstructure *oldM);
EV invoke_palette(void);
EI detectmode(void);
EV setuphole1(int n);
EV setupcontrols(void);
EV setuphole2(void);
EV checkmouse(void);
EV updatespeeds(void);
extern unsigned char trackball(void);
EV jpixel(int,int,int);
EV jxpixel(int,int,int);
EV moveball(void);
EI distance(int,int);
EV showclub(int,int,int);
EV drawCircleRadiusButton(void);
EV NotifyBasins(void);
EV showball(int,int,int);
EV showmagnets(void);
EV DrawControlPage(void);
EV jxwbresen(int x1, int y1, int x2, int y2, unsigned char color, int width);
EV	jxbresen(int oldballx, int oldbally, int ballx, int bally, int color);
EV setbounds(void);
EV PaintQuitButton(int n);
EV startconfig(void);
EV SetStopSign(void);
EV ResetLayout(void);
EV GrayOut(rect *);
EV spinpalette(void);
EV revspinpalette(void);
EV randompalette(void);
EV changepalette(void);
EV grayscale(void);
EV set_samecharges(void);
EV PaintMainButton(int,int);
EV do_files_menu(void);
EI options_panel(void);
EV charge_panel(void);
EV FGifOutput(int);
EV add_a_magnet(int,int);
EV delete_a_magnet(void);
EV SelectNothing(void);
extern unsigned long realfarcoreleft(void);
EV LoadDefaultPalette(char *);
EV gif_viewer(void);
EV TempFileName(char *,char *);
EV DoAllButtons(void);
EV ForceCursorOff(void);
EV ResetBasins(void);
EI memok(long __nbytes);
EI GifDisplay(char *_f);
extern short gif_decoder(short l);
EV basin_menu(void);
EV MasterReset(void);

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
#undef ED
#undef EI
#undef ER


#endif
