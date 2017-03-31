#ifndef _TOYEXTSH_
#define _TOYEXTSH_

#define EV extern void
#define EI extern int

EV PushCursorPosition(void);
EV PopCursorPosition(void);
EI cancel_ok_msg(char *);
EV LimitMouseRect(rect *);
EV helptext(char *msg);
EI select_file(char *, char *, char *, char *);
EV *normalize(void huge * ptr,
			unsigned short front, unsigned short end);
EV display_hi(void);
EV display_coarse(void);
EV display_double(void);
EV display_regular(void);
EV checkkeyboard(void);
EI options_panel(void);
EV do_files_menu(void);
EV setIncrementDecrement(int);
EV setNumberOfStates(int);
EV setEATN(int);
EV setEatD(int);
EV setTubeHiding(int);
EV setTubeFreak(int);
EV setTubeFuzz(int);
EV setTubeEating(int);
EI GifOutput(char *,int);
EV setnlukys(int n,int l);
EV PushRect(rect *r,int *err);
EV PopRect(int *err);
EV allocatebuffers(void);
EV deallocatebuffers(void);
EV blankbuffers(void);
EV carandomize(void);
EV loadlookuptable(int, int);
EV loresupdate(void);
EV hiresupdate(void);
EV medtohi(void);
EV hitomed(void);
EV FakeGM_init(void);
EV push_alignment(void);
EV pop_alignment(void);
EV helptext(char *);
EV medtomiddle(void);
EV middletocoarse(void);
EV coarsetomiddle(void);
EV middletomed(void);
EV jchangeVGApalette(void);
EV setdefaultpalette(void);
EV load_canned_shape(int);
EV rchangepalette(void);
EV rusepalette(void);
EV spinpalette(void);
EV revspinpalette(void);
EV grayscale(void);
EV palette_tweaker(void);
EV change_a_parameter(int i);
EV FLoadGif(void);
EV LoadParameters(char *filename);
EV SaveParameters(char *filename);
EI Overwrite(char *filename);
EV Centers(rect *,int *,int *);
EV TempFileName(char *,char *);
EV ErrorBox(char *);
EV usepalette(void);
EV setStopSign(int);
EI set_boxes(void);
EV PushMouseRectLimit(rect * R);
EV PopMouseRect(void);
EV changeVGApalette(void);
EV randompalette(void);
EV PaintQuitButton(int);
EV changepalette(void);
EV move_to_corner(rect *);
EV ShapeLoader(void);

EV _useEGApalette(unsigned char *);
EV _useVGApalette(unsigned char *);
EV PushCursorType(void);
EV ArrowCursor(void);
EV InfoBox(void);
EV PopCursorType(void);
EI ShadowAndSave(rect *R);
EV check_cursor_movement_and_update(void);
EV initialize_numbers(void);
EI DragRect(rect *,int,int);
EV installmode(void);
EV load_preset_palettes(void);
EV initialize_buttons(void);
EV ca_randomize(unsigned char *buffer,int count);
EI _jGetString(char *dest, char *start, int maxlen, GS_TYPE type);
EV RangeError(char *);
EV rowblast(unsigned char far *buffer, int x,int y,int count);
extern short gif_decoder(short);
extern unsigned long realfarcoreleft(void);
EI memok(long __nbytes);

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

#undef EI
#undef EV

#endif

