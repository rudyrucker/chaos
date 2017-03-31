#ifndef _BARNVARSH_
#define _BARNVARSH
#include "buttons.h"
#define EV extern void
#define EI extern int
#define ED extern double
#define EF extern fpair
#define EP extern pair
#define EC extern char
#define ER extern rect
#define EU extern unsigned char

EI minx,maxx,maxy,miny;
EI FontHeight;
EI MenuBackColor,MenuTextColor,ButtonBackColor,ButtonFrameColor,
   ButtonTextColor;
ER sR;
EI StringWidthX;
EI palettenumber;
EI curx,cury;
extern ffunctionsystem *fBptr, *fUptr;
extern functionsystem *Bptr;
ED flox, fhix, floy, fhiy;
EI joshcolor[16];
EI lox, hix, loy, hiy, deltax, deltay, centerx, centery;
EI startlox, starthix, startloy, starthiy, startdeltax, startdeltay;
EI startcenterx, startcentery;
ED fstartlox, fstarthix, fstartloy, fstarthiy;
ED  fstartdeltax, fstartdeltay, fstartcenterx, fstartcentery;
extern unsigned int iteration;
extern flockstruct *flockptr;
extern fflockstruct *fflockptr;

EU maxcolor;
EI barnmapflag;
EI projector;
EI notrace;
EI flocktype;
EI mouseflag;
EI redmap;
EI edmap;
EI stage;
EI tweaking;
EI tweakmode;
EI exitflag;
EI coloring_style;
EI cursoron;
EI spinflag;
EI checking_limits;
ER current_limit;
EI monoflag;
EI realmode;
ED fdeltax,fdeltay;
ED fstepx,fstepy;
ED fcenterx,fcentery;
EI current_main_item;
ER left_arrow, right_arrow;
ER bottom_rects[10];
ER display_rect;
extern metaPort *thePort;
extern bitmap *theBitmap;
EI triangle_editing_mode;
EI triangle_display_mode;
EI undoable;
ED cornermax;
extern fbarnmap previous_barnmap;
ED zoomfactor;
EI Stamping;
ER stamp_rects[4];
extern text_button tweakRects[];
EI FirstMap;
EI mode;
EU startvgapalette[];
EU startegapalette[];
EI available_colors[];
EC current_barnmap_name[];
ED aspect;
EI barnmapsp;
EI hasVGA;
EI grayflag;
ER *mainR[60];
ER main_items;
EI dirty_bit;
EI locked;
EI CommPort;
EI safe_alloc;
EI prog_init;

#undef EV
#undef EI
#undef ED
#undef EF
#undef EP
#undef EC
#undef ER
#undef EU

#endif
