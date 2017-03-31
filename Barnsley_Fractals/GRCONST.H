#ifndef _GRCONSTH_
#define _GRCONSTH_

/* ************************************************************ * 
 *  GRCONST.H  -  MetaWINDOW 'Constant' and 'Type' Definitions  *
 *  Copyright (c) 1985-1989  Metagraphics Software Corporation  * 
 * ************************************************************ */

/*     @(#)grconst.h	3.15     12/4/89 12:49:28  */

#ifndef NO_INC             /**************************************/
#include "metcompl.h"      /*   C Compiler fixups and standard   */
#endif                     /*   include files                    */
                           /**************************************/

                           /**************************************/
#define  MW_Trim 1         /*  Abbreviated MW header defines     */        
                           /**************************************/

#ifdef   MetaVer4
#error  You may not use Version 3 Headers with MetaVer4 defined.
#endif /*MetaVer4*/
                           
             
#ifndef  MS_DOS            /****************************************/
#define  MS_DOS     1      /*  Operating System: Defaults to DOS   */
#endif /*MS_DOS*/          /****************************************/


#ifndef  MW_Ver3           /* Generational compatibility define */  
#define  MW_Ver3    1
#define  MW_Ver4    0
#define  GrQuery MetQuery
#endif /*MW_Ver3*/

/* data size decls */
#define  Byte  unsigned char   /* "Byte" type equivalence     */
#define  word  unsigned short  /* "word" type equivalence     */

/* PENMODE & TEXTMODE RasterOp's    */
#define zREPz        0   
#define zORz         1
#define zXORz        2
#define zNANDz       3
#define zNREPz       4
#define zNORz        5
#define zNXORz       6
#define zANDz        7

/* Graphics Adaptor Definitions */
#define ATI800x600   533   /* ATI VGAWonder                    */
#define ATI640x480X  534   /*   640x480 256-color					*/
#define ATI1024x768F 535   /*   1024x768 16-color					*/
#define ATT          128   /* AT&T Std. Color & DEB Adaptors   */
#define ATT640x400   128   /*   640x400 monochrome             */
#define DEB640x200   129   /*   640x200 16-color               */
#define DEB640x400   130   /*   640x400 16-color               */
#define CGA          6     /* IBM Color Graphics Adaptor (CGA) */
#define CGA320x200   4     /*   320x200 4-color                */
#define CGA640x200   6     /*   640x200 2-color                */
#define COR             34 /* Cornerstone                      */
#define COR1600x1280V   34 /*   Vista 1600 1600x1280 mono      */
#define COR1600x1280VA  37 /*   Vista 1600 1600x1280 no em, 2 mon */
#define COR1600x1280DP  35 /*   DualPage   1600x1280 mono      */
#define COR1600x1280DPA 36 /*   DualPage 1600x1280, no JP1     */
#define COR1600x1280MCA 39 /* DualPage 1600x1280 Micro Channel */
#define EGA          512   /* IBM Enhanced Graphics Adaptor    */
#define EGAMono      512   /*   640x350 monochrome             */
#define EGA320x200   544   /*   320x200 16-color               */
#define EGA640x200   528   /*   640x200 16-color               */
#define EGA640x350   560   /*   640x350 16-color (128k EGA)    */
#define MGA640x350   561   /*   640x350 2-color                */
#define EGA640x480   562   /*   640x480 16-color  (VGA)        */
#define MGA640x480   563   /*   640x480 2-color   (MCGA)       */
#define VGA320x200   564   /*   320x200 256-color (PS2 VGA)    */
#define EGA640x350S  565   /*   640x350 16-color (no bios-EGA) */
#define VGA640x350S  566   /*   640x350 16-color (no bios-VGA) */ 
#define VGA640x480S  567   /*   640x480 16-color (no bios-VGA) */   
#define EGA640x350MP 131   /* IBM EGA, in multi planar setting */	
#define JRT320x200   569   /*   320x200 16-color IBM PCjr & Tandy 1000 EX */
#define EVA          516   /* Tseng Labs EVA/480               */
#define EVA640x480   516   /*   640x480 16-color               */
#define EVR          1024  /* Everex Graphics Edge Adaptor     */
#define EVR640x200   1024  /*   640x200 16-color               */
#define EVR640x400   1025  /*   640x400 4-color                */
#define EVX800x600   532   /* Everex EVGA EV-673 Adaptor       */
#define FKY1120x750  2200  /* Fujitsu FMR 70 16 color          */	
#define FKY640x400   2201  /* Fujitsu FMR 60 16 color          */
#define FKY1120x750m 2202  /* Fujitsu FMR 70 monochrome        */	
#define GEN          529   /* Genoa SuperEGA HiRes             */
#define GEN640x480   529   /*   640x480 16-color               */
#define GEN800x600   518   /*   800x600 16-color               */
#define GEN1024x768  519   /* Genoa SuperVGA HiRes-10 16-color */ 
#define GEN640x350X  530   /*   640x350 256-color              */  
#define GEN640x480X  531   /*   640x480 256-color              */  
#define HER          8     /* Hercules/AST Monochrome Adaptor  */
#define HER720x348   8     /*   720x348 monochrome             */
#define MDS          2     /* MDS Genius Graphics Display      */
#define MDS736x1008  2     /*   736x1008 monochrome            */
#define NNR512x512   4096  /* Number Nine 512x485 256-color    */
#define NNR1024x768  4097  /*    2048x4 @ 1024x768x16          */
#define NSI          514   /* NSI Logic Smart EGA/Plus         */
#define NSI800x600   514   /*   800x600 16-color               */
#define OCD800x600   571   /* Orchid Designer VGA 800x600x16   */ 
#define OCD1024x768  572   /*   1024x768 16 color              */
#define OCD640x350X  573   /*   640x350 256 color              */   
#define OCD640x480X  574   /*   640x480 256 color              */   
#define PAR          517   /* Paradise AutoSwitch EGA/480      */
#define PAR640x480   517   /*   640x480 16-color               */
#define PAR800x600   579   /* Paradise OEM 8/16 800x600 16-color */
#define SIG          32    /* Sigma Design                     */
#define SIG640x400   32    /*   640x400 16-color, Color-400    */
#define SIG1664x1200 33    /*   LaserView 1664x1200 monochome  */
#define SIG1664x1200A 38   /*   1664x1200 @0A000               */
#define SIG1664x1200MCA 41 /*   1664x1200 Micro channel        */
#define STB          256   /* STB GraphicsPlus-II Adaptor      */
#define STB640x352   256   /*   640x352 monochrome             */
#define STB320x200   257   /*   320x200 16-color               */
#define STB640x200   258   /*   640x200 4-color                */
#define STB640x400   384   /*   640x400 monochrome             */
#define STB800x600   571   /* STB VGA Extra/EM 800x600x16      */       
#define STB1024x768  572   /*   1024x768 16 color              */       
#define STB640x350X  573   /*   640x350 256 color              */ 
#define STB640x480X  574   /*   640x480 256 color              */ 
#define TEC          16    /* Tecmar Graphics-Master Adaptor   */
#define TEC720x352   16    /*   720x352 monochrome             */
#define TEC720x704   17    /*   720x704 monochrome             */
#define TEC320x200   18    /*   320x200 16-color               */
#define TEC640x200   19    /*   640x200 16-color               */
#define TEC640x400   20    /*   640x400 16-color               */
#define TEC640x350X  575   /* Tecmar VGA/AD, 640x350 256-color */
#define TEC640x480X  576   /*   640x480 256-color              */
#define TEC800x600   577   /*   800x600 16-color               */
#define TEC1024x768  578   /*   1024x768 16-color              */ 
#define TOS          16384 /* Toshiba 3100                     */
#define TOS640x400   16384 /*   640x400 monochrome             */
#define TARG512x482  8192  /* TrueVision Targa M8 512x482 256-color */
#define TTS          2048  /* IBM 3270 PC                      */
#define TTS720x350   2048  /*   720x350 monochrome             */
#define TTS360x350   2049  /*   360x350 4-color                */
#define VESA800x600  526   /* V.E.S.A. std 800x600 16-color    */
#define VGA          520   /* Video-7 VEGA Deluxe              */
#define VGA640x480   520   /*   640x480 16-color               */
#define VGA752x410   521   /*   752x410 16-color               */
#define VGA720x540   522   /* V7 VGA  720x540 16-color         */
#define VGA800x600   523   /*   800x600 16-color  (VGA)        */
#define VGA640x480X  525   /* V7 640x480 256-color             */
#define VGA1024x768x2 527  /*   1024x768 2-color               */
#define VGA1024x768  536   /* VGA 1024 i 1024x768 16-color     */
#define WYS          64    /* Wyse WY-700 Graphics Display     */
#define WYS1280x800  64    /*   1280x800 monochrome            */
#define WYS1280x400  65    /*   1280x400 monochrome            */
#define WYS640x400   66    /*   640x400 monochrome             */
#define MEC1120x750  610   /* NEC PC 98XA monochrome           */
#define MEC640x400   611   /* NEC PC 98VM monochrome           */
#define NEC1120x750  626   /* NEC PC 98XA 16-color             */
#define NEC640x400   627   /* NEC PC 98VM 16-color             */

/* Defines for National Design Genesis-1024/1280 */
#define NDI640x480x4D   0x4140   /* 640x480   16-color, digitl */
#define NDI640x480x4A   0x4141   /* 640x480   16-color, analog */
#define NDI1024x768x4J  0x4148   /* 1024x768  16-color, JVC    */
#define NDI1024x768x4M  0x4149   /* 1024x768  16-color, Mitsu. */
#define NDI1280x1024x4J 0x414A   /* 1280x1024 16-color, JVC    */
#define NDI960x720x4N   0x414B   /* 960x720   16-color, NEC XL */

/* Defines for RenaissanceGRX Rendition-II       */
#define GRX640x480x4A   0x4240   /* 640x480   16-color, analog */
#define GRX800x600x4A   0x4242   /* 800x600   16-color, analog */
#define GRX1024x768x4A  0x4246   /* 1024x768  16-color, analog */
#define GRX640x480x8A   0x4248   /* 640x480  256-color, analog */
#define GRX800x600x8A   0x424A   /* 800x600  256-color, analog */
#define GRX1024x768x8A  0x424C   /* 1024x768 256-color, analog */

/* Defines for Hewlett-Packard HP82328 IGC       */
#define HP1024x768x4    0x664A /* 1024x768  16-color         */

/* Defines for IBM 8514A (and compatibles)       */
#define EFF640x480x4    0x7000 /*  640x480   16-color          */
#define EFF640x480x8    0x7001 /*  640x480  256-color          */
#define EFF1024x768x4I  0x7040 /* 1024x768   16-color, intlacd */
#define EFF1024x768x4N  0x7041 /* 1024x768   16-color, non-inl */
#define EFF1024x768x8I  0x7042 /* 1024x768  256-color, intlacd */
#define EFF1024x768x8N  0x7043 /* 1024x768  256-color, non-inl */
#define EFF1280x1024x4I 0x7080 /* 1280x1024  16-color, intlacd */
#define EFF1280x1024x4N 0x7081 /* 1280x1024  16-color, non-inl */
#define EFF1280x1024x8I 0x7082 /* 1280x1024 256-color, intlacd */
#define EFF1280x1024x8N 0x7083 /* 1280x1024 256-color, non-inl */

#define CUST_LIN   CGA640x200  /* User defined linear memory */
#define CUST_BS    SIG640x400  /* User defined Bank select   */
#define CUST_EGA   EGA640x350  /* User defined EGA Superset  */

/* Mouse Definitions */
#define COM1            1  /* Mouse Systems mouse, COM1      */
#define COM2            2  /* Mouse Systems mouse, COM2      */
/* #define MsMouse     16     Microsoft Bus Mouse - obselete */
#define MsCOM1         17  /* Microsoft Serial Mouse, COM1   */
#define MsCOM2         18  /* Microsoft Serial Mouse, COM2   */
#define MsDriver       32  /* Microsoft Mouse driver         */
#define FKYMsDriver    128 /* FKY Microsoft Mouse driver, uses int 99 */
#define swRight         1 
#define swMiddle        2  /*   (not on 2 button mice)       */
#define swLeft          4
#define swAll           7

#define black           0  /* all bits OFF */
#define Black           0
#define white          -1  /* all bits ON  */ 
#define White          -1
#define TextPg0         0
#define GrafPg0       256
#define GrafPg1       257
#define ScrnXmin        0
#define ScrnYmin        0
#define True            1
#define true            1
#define False           0
#define false           0

#define cNormal         0  /* TEXTFACE                       */
#define cBold           1
#define cItalic         2
#define cUnderline      4
#define cStrikeout      8
#define cMirrorX       32
#define cMirrorY       64
#define cProportional 128

enum {alignLeft,alignCenter,alignRight};
enum {alignBaseline,alignBottom,alignMiddle,alignTop};


#ifdef OLDWAY
#define alignLeft       0  /* TEXTALIGN - Horizontal         */
#define alignCenter     1
#define alignRight      2

#define alignBaseline   0  /* TEXTALIGN - Vertical           */
#define alignBottom     1
#define alignMiddle     2
#define alignTop        3
#endif

#define pathRight       0  /* TEXTPATH definitions           */
#define pathUp        900
#define pathLeft     1800
#define pathDown     2700

#define capFlat         0  /* PENCAP definitions             */
#define capRound        1
#define capSquare       2

#define joinRound       0  /* PENJOIN definitions            */
#define joinBevel       1
#define joinMiter       2

typedef Byte image;         /* "image" type equivalence      */

typedef struct _point       /* "Point" structure type        */
   {short    X;             /* X coordinate                  */
    short    Y;             /* Y coordinate                  */
   }  point;

typedef struct _rect       /* "Rectangle" structure type     */
   {short    Xmin;           /* minimum X                    */
    short    Ymin;           /* minimum Y                    */
    short    Xmax;           /* maximum X                    */
    short    Ymax;           /* maximum Y                    */
   }  rect;

typedef struct _polyHead   /* Polygon "header" structure     */
   {short    polyBgn;        /* beginning index              */
    short    polyEnd;        /* ending index                 */
    rect     polyRect;       /* boundry limits               */
   }  polyHead;

typedef struct _event      /* Event record structure         */
   {Byte     ASCII;          /* ASCII character code         */ 
    Byte     ScanCode;       /* Keyboard scan code           */
    short    State;          /* Keyboard & mouse switches    */
    short    CursorX;        /* Cursor X position            */
    short    CursorY;        /* Cursor Y position            */
    short    Time;           /* System time of event         */
    long     evntData;       /* User defined 32 bit item     */
   }  event;

#if      MW_Ver4   
#define  evntASCII    ASCII         
#define  evntScanCode ScanCode     
#define  evntState    State        
#define  evntCursorX  CursorX      
#define  evntCursorY  CursorY      
#define  evntTime     Time         
#endif /*MW_Ver4*/

typedef struct _cursor     /* Cursor Image definition        */
   {short    curWidth;       /* must be 16                   */
    short    curHeight;      /* must be 16                   */
    short    curAlign;       /* must be 0                    */
    short    curRowBytes;    /* must be 2                    */
    char     curBits;        /* must be 1                    */
    char     curPlanes;      /* must be 1                    */
    Byte     curData[32];    /* cursor data bytes            */
   }  cursor;

typedef struct _patRcd     /* Pattern Image definition       */
   {short    patWidth;       /* must be 8                    */
    short    patHeight;      /* must be 8                    */
    short    patAlign;       /* must be zero                 */
    short    patRowBytes;    /* must be equal to patBits     */
    char     patBits;        /* value of 1,2,4 or 8          */
    char     patPlanes;      /* value of 1 thru 32           */
    char     patData[32];    /* pattern data bytes           */
   }  patRcd;

typedef struct _pnState    /* Pen State record structure     */
   {long     psBkColor;      /* Background color             */
    long     psPnColor;      /* Pen color                    */
    point    psPnLoc;        /* Pen location                 */
    point    psPnSize;       /* Pen size                     */
    short    psPnMode;       /* Pen mode (rasterOp)          */
    short    psPnPat;        /* Pen pattern index            */
    short    psPnCap;        /* Pen end-cap style            */
    short    psPnJoin;       /* Line join style              */
    short    psPnDash;       /* Line dash style              */
    short    psPnOffset;     /* Dash starting offset         */
    short    psPnLevel;      /* Pen visibility level         */
   }  penState;

typedef struct _dirRec     /* FILEQUERY - directory record   */
   {char     reserved[21];   /* (DOS reserved)               */
    char     fileAttr;       /* File attribute               */
    short    fileTime;       /* File create time             */
    short    fileDate;       /* File create date             */
    long     fileSize;       /* File size (bytes)            */
    char     fileName[14];   /* "FILENAME.EXT\0"             */
   }  dirRec;

typedef struct _imageHeader /* Image Header record structure */
   {short    imWidth;        /* Pixel width (X)              */
    short    imHeight;       /* Pixel height (Y)             */
    short    imAlign;        /* Image alignment              */
    short    imRowBytes;     /* Bytes per row                */
    char     imBits;         /* Bits per pixel               */
    char     imPlanes;       /* Planes per pixel             */
/*  char     imData[]; */    /* (image data, variable length)*/
   } imageHeader;

typedef struct  _dashRcd   /* DEFINEDASH data record         */
   {short    nCnts;          /* number of active entries, 1-8*/
    short    cnt1;           /* distance ON,  1              */
    short    cnt2;           /* distance OFF, 2              */
    short    cnt3;           /* distance ON,  3              */
    short    cnt4;           /* distance OFF, 4              */
    short    cnt5;           /* distance ON,  5              */
    short    cnt6;           /* distance OFF, 6              */
    short    cnt7;           /* distance ON,  7              */
    short    cnt8;           /* distance OFF, 8              */
   } dashRcd;

/* Write/ReadPalette */
typedef  struct   _palData
{
   word palRed;
   word palGreen;
   word palBlue;
}palData;

typedef	struct  _HCInfoRec
{  short	pins;				   /* Number of elements in print swath. Use 1 for Laser printers  */
   short inlen;            /* the number of bytes in following string   */
   Byte	init[0x20];	      /* The printer init code(s).                 */
   short rslen;            /* the number of bytes in following string   */
   Byte	reset[0x10];		/* printer clean up code(s)                  */
   short hdlen;            /* the number of bytes in following string   */
   Byte	header[0x08];		/* sent before every line                    */
   short pstlen;           /* the number of bytes in following string   */
   Byte	pstam[0x08];		/* sent after length spec                    */
   short trlen;            /* the number of bytes in following string   */
   Byte	trailer[0x10];	   /* sent after every line                     */ 
   short	hcMode;	   		/* operational mode bits                     */
   short	prtPort;		   	/* system printer port to use, 0-3 or 80-83  */	
   Byte  hcSIG[2];			/* set to 'HC' for validity check            */ 
   short	xlatMask;		   /* max color number in xlaTable              */
   Byte	xlaTable[0x10];	/* color translation table                   */
}HCInfoRec;


#define	HC_ROT	0x01	/* when set, rotate pic 90 CCW	*/
#define	HC_XL		0x08	/* When set, disables default background xlate  */
#define  HC_ASC   0x10  /* when set, emits ascii length, instead of hex */
                        /* Typically used for HP LaserJet printers      */

typedef  short mapArray[8];  /* CursorMap data record         */

/* Sample User Definable Types: */

typedef struct _CCBtype  /*User Definable Clock_Communication_Block*/
   {short    Timer;
    short    tX;
    short    tY;
    short    tSw;
   } CCBtype;

typedef struct _MCBtype  /*User Definable Mouse_Communication_Block*/
   {short    mX;
    short    mY;
    short    mSw;
   } MCBtype;

/* End of File - GRCONST.H */

#endif
