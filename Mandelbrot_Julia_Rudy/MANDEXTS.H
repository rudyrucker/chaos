#ifndef _MANDEXTSH_
#define _MANDEXTSH_

#define EV extern void
#define EI extern int


EV push_alignment(void);
EV pop_alignment(void);
EV SplitRectH(rect *,rect *,rect *);
EV make_button(button * b, int x, int y, char *text);
EV jarrow(int x, int y, int width, int height, int border, int fill);
EI  jGetString(char *dest, char *start, int maxlen);
EV rowblast(unsigned char far *buffer, int x,int y,int count);
EV draw_buttons(void);
EV helptext(char *filename);
EV do_files_menu(void);
EV do_type_menu(void);
EV do_params_menu(void);
EV do_colors_menu(void);
EV gifsave(int);
EV gifview(void);
EV draw_buttons(void);
EV	SetUpProperView(void);
EV do_stamp(int i);
EV stampinfo(char *t, stampdata * d,int n);
EI  cancel_ok_msg(char *);
EV do_stamp(int n);
EV pushstamp(stampdata *d);
EV erasecursor(void);
EV resetcursor(void);
EV pushview(view *);
EV ForceCursorOn(void);
EV MakeOurCursor(void);
EV PushRect(rect * R, int *err);
EV PopRect(int *err);
EV start_CCM(void);
EV start_cubic(void);
EV start_julia(void);
EV start_mandel(void);
EV useview(void);

EI  GifDisplay(char *);
EV zapbuffer(short far *, int);
EV blast_bottom(int,int);
EI  select_file(char *title, char *filespec, char *chosen, char *ext);
EV spinpalette(void);
EV revspinpalette(void);
EV changepalette(void);
EV usepalette(void);
EV fillcolorval(void);
EV grayscale(void);
EV ProtectCurrentRow(void);
EV reposition_slider_v(slider *s,int selected);
EV reposition_slider_X(slider *s,int X,int selected);
EV create_slider(slider *s,rect *mR,int x,int y);
EI  radio_button_set_height(radio_button_set *rbs);
EI  slider_height(slider *s);
EV drawcursor(void);
EV erasecursor(void);
EI  mandel32(void);
EI  fimand32(void);

EI  julia32(void);
EI  in1man32(void);
EI  in2man32(void);
EI  in1man16(void);
EI  cubicjulia32(void);
EI  cubicmandel32(void);
EI  in1cubicmandel32(void);
EI  in2cubicmandel32(void);
EI  in1cubmand16(void);
EI  julia16(void);
EI  mandel16(void);
EI  cubicmandel16(void);
EI  cubicjulia16(void);
EV cpu_type(void);
EV changepalette(void);
EV usepalette(void);
EV spinpalette(void);
EV revspinpalette(void);
EV grayscale(void);
EV mouse(int a, int b, int c, int d);
EV getmode(void);
EV egavgapixel(int, int, unsigned char);
EI egavgagetpixel(int, int);
EI  GifOutput(char *, int);
EV zapbuffer(short far *, int);
EV erase_old_box(void);
EV updatecursor(void);
EV popview(view *);
EV _readcursor(double *,double*,double*,double*,int);
EV readcursor(void);
EV palette_tweaker(void);
EV _ProtectCurrentRow(void);
EI  trystamps(void);
EV dozoom(double);
EV restoretextmode(void);
EV installmode(void);
EV BoxCursor(void);
EV ArrowCursor(void);
EI  ParseKey(int keyword);
EI  detectmode(void);
EV InitGlobals(void);
EV instruct(void);
EV LoadDefaultPalette(char *);
EV JSetPixel(int,int,char);
EV drawblock(int,int);
EV drawblock_reversed(int,int);

EV stepxy(void);
EV spiralxy(void);
EI checkkeyboard(void);
EI  incubicmandelfloat(void);
EV popstamp(stampdata *);
extern char *flagstonames(void);
EV move_to_corner(rect *);
EV PushCursor(void);
EV PopCursor(void);
EV namestoflags(char *typename);
EI  slicenametoslicetype(char *slicename);
EI  nametofill(char *fillname);
extern char *vflagstonames(stampdata * d);
EV MyTypetoFlags(void);
EV SetEGAfromVGA(void);
EV reposition_slider_v_base(slider *s,int selected,int floater,int numbers);
EV reposition_slider_v(slider *s,int selected);
EV reposition_slider_X(slider *s,int X,int selected);
EI slider_height(slider *s);
EV create_slider_base(slider *s,rect *mR,int x,int y,int floater,int tinies,
   int frame,int numbers);
EV create_slider(slider *s,rect *mR,int x,int y);
EV zoombox(void);
EV range_error(double,double,char *);
EV _updatecursor(int);
EV menutext(char *);
EV draw_params(void);
EV PushCursorPosition(void);
EV PopCursorPosition(void);
EV tick_box(radio_button_set *rbs);
EI radio_button_set_height(radio_button_set *rbs);
EV draw_radio_button_set(radio_button_set *rbs,rect *R,int x,int y);
EV paint_box(rect *R,int color);
EV turn_off_box(rect *R);
EV turn_on_box(rect *R);
EI ShadowAndSave(rect *R);
EV Centers(rect *,int *,int *);
EV LimitMouseRect(rect *R);
EV RangeError(char *);
EV ErrorBox(char *);
EV gif_viewer(void);
EV _useVGApalette(unsigned char *p);
EV _useEGApalette(unsigned char *p);
EI Overwrite(char *);
EV adjustxy(void);
EV dopan(int,int);
EV push_main_button(int,int);
EV DefaultPalette(void);
EV next_preset(void);
EV load_preset(void);
EV PaintQuitButton(int);
EV previous_preset(void);
EV integer_to_float(void);
EV float_to_integer(void);
EV bottomline(void);
EV bumppalette(void);
EV randompalette(void);
EV load_params(char *);
EV Pan(void);
EV save_params(char *);
EV InfoBox(void);
EV main_corner(int);
EI ClosestButton(int,int);
extern unsigned long realfarcoreleft(void);
EV CheckButtons(int,int);
EV hilite_main_button(int,int);
EV blast_in_row(char *,int);
EV slowdown_warning(void);
EV speedup_warning(void);
EI memok(long __nbytes);
extern short gif_decoder(short l);

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
#define free(n) j_free(n)
#define malloc(n) j_malloc(n)
#define farcalloc(n,u) j_farcalloc(n,u)
#define farfree(n) j_farfree(n)
#define farmalloc(n) j_farmalloc(n)
#define strdup(s) j_strdup(s)
#define getcwd(n,u) j_getcwd(n,u)
#endif

#undef EV
#undef EI


#endif

