#ifndef _ATTRVARSH_
#define _ATTRVARSH
#define EV extern void
#define EI extern int
#define ED extern double

ED flox, fhix, floy, fhiy, floz, fhiz;
EI lorenzflyflag;
ED pcentx, pcenty, pcentz, pcentfly;
ED pcanvasz;
extern ftriple flyspot, viewspot;
extern ftriple flytangent, flynormal, flybinormal;
extern ftriple viewtangent, viewnormal, viewbinormal;
EI dimension;
EI tracetype;
EI logisticlaunchflag;
EI pixelx, savepixelx;
extern fflockstruct1 *fflock1ptr;
extern fflockstruct2 *fflock2ptr;
extern fflockstruct3 *fflock3ptr;
extern fflockstruct3 *saved_fflock3ptr;
extern fflockstruct2 *saved_fflock2ptr;
EI flocktype;
EI exitflag, fancyflag, trihedroncount;
ED fxstep, fystep, fx, savefx, lvfx;
EI standardstartlog, standardstoplog, startlog, stoplog;
ED humpshift, humpspot;
EI cleanflag, loghumpselectflag;
EI grayflag, monoflag;
extern unsigned char maxcolor;
ED logstartval;
EI soundflag, yorketopologyflag;
EI hump[], line[];
extern unsigned int iteration;
EI ribbonfull;
EI ribbonindex;
extern screenflock *ribbon[MAXRIBBONLENGTH];
EI ribbonlength;
extern char axis;
extern ftriple newdr, oldtangent[MAXFLOCK];
EI trihedronon;
extern pair antittip[MAXFLOCK], antintip[MAXFLOCK], antibntip[MAXFLOCK];
extern pair ttip[MAXFLOCK], ntip[MAXFLOCK], bntip[MAXFLOCK];
EI tritracetype;
ED dt,dt2,dt6;
ED fdeltax, fdeltay, fdeltaz, fcenterx, fcentery, fcenterz;
ED fdeltax_div_64K, fdeltay_div_64K, fdeltaz_div_64K;

ED cosa, sina;
ED omega1;
ED omega2;
ED epsilon;
ED epsbar;

extern metaPort *thePort;
extern rect sR;
EI FontHeight;
EI StringWidthX;

EI minx,maxx,maxy;
ED fha;
EI yorkenumber;

ED xscale,yscale,zscale,xyscale;

EI MenuBackColor,MenuTextColor,
	   ButtonBackColor,ButtonFrameColor,
	   ButtonTextColor;

EI lorenzfreq;

EI cursorshape;
extern view logisticview;
EI delayfactor;
EI spinflag,cursorshape;
EI palettenumber;
EI Stamping;
extern unsigned long next_release,long_iteration;
EI starting_trihedroncount;
EI cursoron;
EI curx,cury;
extern rect stampingR[6];
extern rect dR,stampR;
EI stamprxmin, stamprxmax, stamprymin, stamprymax;
ED xscreentostamp, yscreentostamp;
ED zoomfactor;
EI mode;
extern unsigned char defaultcolortable[16][3];
EI minimum_luminosity;
EI colorcycleflag;

ED aspect;
extern char egacolortable[17];
EI cursorcolor;
EI savepalnum;
EI ParameterDisplayMode;
extern char vgacolortable[64*3];
extern char ct[17];
EI openflag;
EI singlestepflag;
EI stepme;
EI curxinc, curyinc, curx, cury;
EI curspeed;
extern char defaultpalette[64][3];
EI available_colors[10];
EI current_main_item;
EI locked;
EI onestep,stopped;
EI hasVGA;
EI mousemove;
EI *logsaviour;
EI trihedrons_exist;
EI safe_alloc;
EI CommPort;
EI prog_init;
ED startdeltax, startdeltay, startdeltaz;
ED startcenterx, startcentery, startcenterz;
ED fcurx, fcury;
EI shiftregister;
EI sixteen_count;
EI start_shiftregister;
ED yzoomfactor;
extern rect current_cursor;


#undef EV
#undef EI
#undef ED
#endif
