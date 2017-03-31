#ifndef _TOYVARSH_
#define _TOYVARSH_

#define EI extern int
#define EUC extern unsigned char
#define EC extern char
#define ER extern rect

EC *typenames[];
extern radio_button ztB;

EUC maxstate,increment,maxeatstate;
EUC nlukyn,nlukyl,nlukyu,nlukyk,nlukyy;
EUC tubefuzz,tubealarm,tubelive,tuberest;
EI eatmode;
EUC *nluky_parms[];
EUC caotype;
EI startrows[20];
extern radio_button *zButtons[];
EC ztname[];
EI buttonwidth;
EI current_main_item;
ER ButtonRect;
ER BottomRect;
EI stopped;
extern DISPLAY_MODE display_mode;

EUC *egarowptrs[HIYCOUNT + 2];
EUC *newbufrowptrs[MEDYCOUNT + 2];
EUC *oldbufrowptrs[MEDYCOUNT + 2];
EI display_page;
EUC far *fakelookuptable;
EUC far *lookuptable;
EUC huge *egabuf;
EI allocatefailflag;
EUC huge *oldbuf;
EUC huge *newbuf;
EI newcaoflag;
EI newcaflag;
EI rebuildflag;
ER sR;
EI FontHeight;
EI StringWidthX;
extern double aspect;
EUC defaultpalette[64][3];
EI mode;
EC vgacolortable[64][3];
EC egacolortable[17];
extern metaPort *thePort;
EI monoflag;
ER mainbuttonR[20];
EC *maintexts[];
ER DisplayRect;
EI palettenumber;
EUC activedactable[16];
EI grayflag;
EI exitflag;
EI randomizeflag;
EUC keybreakflag;
EI spinflag;
EI onestep;
EI seatype;
EI fastflag;
EI skipper;
EI changeflag;
EI wrapflag;

EUC HODGE_colortable[16*3];
EUC NLUKY_colortable[16*3];
EUC EAT_colortable[16*3];
EUC TUBE_colortable[16*3];
EUC HODGE_ct[17];
EUC NLUKY_ct[17];
EUC EAT_ct[17];
EUC TUBE_ct[17];

EI main_items;
EI vgaflag;
extern numbertool tweakNT[6];
ER *mainR[50];
EI canned_image;
EI main_lefters[],main_righters[],main_uppers[],main_downers[];
EI init_mem_err;
EI Comm;
EI prog_init;
EI spinspeed;


EUC defHODGEpal[16*3];
EUC defHODGEct[16];
EUC defTUBEpal[16*3];
EUC defTUBEct[16];
EUC defEATpal[16*3];
EUC defEATct[16];
EUC defNLUKYpal[16*3];
EUC defNLUKYct[16];



#undef EI
#undef EUC
#undef EC
#undef ER

#endif
