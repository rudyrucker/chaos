#ifndef _MAGVARSH_
#define _MAGVARSH_

#define ER extern rect
#define EI extern int
#define ED extern double
#define EUC extern unsigned char
#define EL extern long

ER sR;
EI FontHeight;
EI StringWidthX;
ED aspect;
EUC defaultpalette[64][3];
EI hasVGA;
EI mode;
EUC vgacolortable[16][3];
EUC egacolortable[17];
extern metaPort *thePort;
EI FirstLine;
EI CommPort;
EI safe_alloc;
EI prog_init;

EI clubxinc;
EI clubyinc;
EI incsize;
EI minx;
EI halfclub;
EI clubx;
EI cluby;
EI maxcolor;
EI swingflag;
EI rollflag;
extern CLUBTYPE clubflag;
EI undoflag;
EI CurrentControlPage;
EI helpscreenflag;
EI exitflag;
EI soundflag;
EI forcetype;
EI frictiontype;
EL centerpull;
ED radius;
EI rkflag;
EI reversibleflag;
ED fspeedx;
ED fspeedy;
EL speedx;
EL speedy;
ED friction;
EL freq;
EL magnetradius;
EL magrad2;
EL magrad3;
extern magnetstructure M;
EI stride;
EI saveballflag;
EI tracetype;
EI deltax;
EI deltay;
EI reverseflag;
EI xsection;
EI delayfactor;
EI mymag;
EL chargeunit;
EI pauseflag;
EI newholeflag;
EI maxx;
EI maxy;
EI holenumber;
ER displayRect;

EI CursorShowing;
EI grayflag;
EI spinflag;
EI basinflag;
EI basinx;
EI basiny;
EI basinstep;
EUC startvgapalette[16 * 3];
EUC startegapalette[16];
EI ballx,bally;
EI savedballx, savedbally;
EI savedmagnetx, savedmagnety;
EI MagnetIsVisible;
EI centerx, centery;
ED deltaxdebt,deltaydebt;
ED frictiondebtx,frictiondebty;
EI lox, hix, loy, hiy, halfx, halfy;
EI firsttimeflag;
EI stopped,onestep;
EI current_main_item;

EI inptr;
EI outptr;
EI erasing;
EI donttrace;
EI locked;
EI available_colors[10];
EL basinlimit;
EI basindisplaymode;
extern unsigned char GIFctab[];
extern unsigned char GIFcmap[256 * 3];
EI OurMode;

#undef EUC
#undef ED
#undef ER
#undef EI
#undef EL

#endif
