
/* *********************************************************** *
 *   GRPORTS.H  -  MetaWINDOW 'Port' and 'Bitmap' Definitions  *
 *  Copyright (c) 1985-1989  Metagraphics Software Corporation *
 * *********************************************************** */

/*     @(#)grports.h	3.3     11/13/89 10:14:45  */


/* --------------------------------------------------------- *
 * WARNING - USE ONLY WITH LARGE MEMORY MODEL CONFIGURATIONS * 
 * If you use "metaPort" or "bitmap" data structures within  *
 * your program, you must operate with either a "data" or    *
 * "large" memory model configuration.  The pointer          *
 * variables used within these structures must be 32 bit (4  *
 * byte, far) pointers.  The "small" and "program" memory    *
 * models use only 16 bit (2 byte, near) pointers and will   *
 * not allocate these data structures properly.              *
 * --------------------------------------------------------- */


typedef struct _devProc  /* Device Procedure List Structure */
 {char    procSign[8];      /* devProc signature "devProc" */
  short (far *procMeta[6])();  /* optimization pointers     */
  short (far *procDev[256])(); /* device procedure pointers */
 } devProc;

typedef struct _map              /* "rowTable" Raster Line Pointers */
  {Byte  far *rowTable[1280];    /* Table of row pointers     */
  } map;

typedef struct _bitmap  /* "bitmap" Data Structure           */
   {
    short      devClass;      /* Device class                  */
    short      devType;       /* Device type                   */
    devProc far *devProcs;    /* Ptr to device procedure list  */
    short      rowBytes;      /* Bytes per scan line           */
    short      pixWidth;      /* Pixels horizontal             */
    short      pixHeight;     /* Pixels vertical               */
    short      pixResX;       /* Pixels per inch horzontally   */
    short      pixResY;       /* Pixels per inch vertically    */
    short      pixBits;       /* Color bits per pixel          */
    short      pixPlanes;     /* Color planes per pixel        */
    map far *mapTable[24];    /* Pointers to rowTable(s)       */
    short      mapActive;     /* Active logical page           */
    short      mapPages;      /* Number of mapList entries,1-4 */
    short      mapList[4];    /* List of logical pages loaded  */
    short      mapRsvd[6];    /*  (reserved for future use)    */
    short      mapSegment;    /* Map segment                   */
    short      mapHandle;     /* Map handle                    */
    short (far *mapManager)();/* Pointer to paging manager     */
   }  bitmap;

                 /* Bitmap Device Type (devType) Definitions   */
#define devSTD        0    /* Standard memory mapped bitmap    */
#define devEGA        1    /* IBM EGA/VGA display bitmap       */
#define devND1     0x40    /* NDI Genesis-1024 bitmap          */
#define devND2     0x41    /* NDI Genesis-1280 bitmap          */
#define devHP1     0x42    /* Hewlett Packard1                 */
#define devEMS     0x80    /* EMS expanded memory bitmap       */
#define devWYS     0x81    /* Wyse 1280x800 display bitmap     */
#define devSIG     0x82    /* Sigma Design Color-400 bitmap    */
#define devLASVU   0x83    /* Sigma Design LaserView/plus      */
#define devVISTA   0x84    /* Cornerstone VISTA 1600           */
#define devREV512  0x85    /* Number Nine Revolution 512x484   */
#define devDUALPG  0x86    /* Cornerstone DualPage System      */
#define devTSENG   0x87    /* Tseng labs Bank Select - STB/GENOA/ORCHID */
#define devDUALGRAY 0x81   /* Cornerstone grey scale           */


#if   ! MW_Trim
#define c_devClass   0   /* "GetBMapField" index definitions */
#define c_devType    2
#define c_rowBytes   8
#define c_pixWidth  10
#define c_pixHeight 12
#define c_pixRsX    14
#define c_pixRsY    16
#define c_pixBits   18
#define c_pixPlanes 20
#define c_mapTable  22
#endif /*MW_Trim*/

#ifndef  GRFONTS_H
#include "grfonts.h"   /* "fontRec" Data Structure           */
#endif /*GRFONTS_H*/

typedef struct _metaPort /* "metaPort" Data Structure       */
   {bitmap far *portBMap;/*  Pointer to "bitmap" record     */
   rect     portRect;    /* 'Local' coordinate port bounds  */
   point    portOrgn;    /* 'Global' origin of portRect     */
   rect     portVirt;    /* 'Virtual' port bounds           */
   short    portFlgs;    /*  Port Flags                     */
   rect     portClip;    /* 'Local' clipping rectangle      */
  int far  *portRgn;     /*    (reserved)                   */
   short    bkPat;       /*  Background pattern index       */
   long     bkColor;     /*  Background color               */
   long     pnColor;     /*  Pen color                      */
   point    pnLoc;       /*  Pen location                   */
   point    pnSize;      /*  Pen size                       */
   short    pnMode;      /*  Pen mode (rasterOp)            */
   short    pnPat;       /*  Pen pattern index              */
   short    pnCap;       /*  Pen end-cap style              */
   short    pnJoin;      /*  Line join style                */
   short    pnDash;      /*  Line dash style                */
   short    pnOffset;    /*  Dash sequence offset           */
   short    pnLevel;     /*  Pen visibility level           */
   fontRec far *txFont;  /*  Pointer to current font record */
   short    txFace;      /*  Text facing flags              */
   short    txMode;      /*  Text mode (rasterOp)           */
   short    txUnder;     /*  Text underline position        */
   short    txScore;     /*  Text underline scoring         */
   short    txPath;      /*  Text path                      */
   point    txAlign;     /*  Text alignment                 */
   short    txAngle;     /*  Text angle (stroked)           */
   point    txSize;      /*  Text size  (stroked)           */
   short    txSlant;     /*  Text slant (stroked)           */
   short    txExtra;     /*  Text justify bits              */
   short    txSpace;     /*  Space justify bits             */
   short    mkType;      /*  Marker type index              */
   point    mkSize;      /*  Marker size                    */
   short    mkAngle;     /*  Marker angle                   */
   short    spare[24];   /*    (spares)                     */
   }  metaPort;

   typedef struct _pusharea   /* PUSHGRAFIX/POPGRAFIX savearea  */
   {short    saveArea[sizeof(metaPort)/sizeof(short)];
   }  pusharea;


#if      MW_Ver4     /* MetaWINDOW Version 4 upward compatibility */
#define grafMap   bitmap       
#define grafMapp  bitmap       
#define portMap   portBMap
#define grafPort  metaPort     
#define pixBytes  rowBytes     
#define portFlags portFlgs     
#define rowMap    map          
#endif /*MW_Ver4*/


#if    ! MW_Trim
#define c_portBMap   0   /* "GetPortField" Index Definitions */
#define c_portRect   4
#define c_portOrgn  12
#define c_portVirt  16
#define c_portFlgs  24
#define c_portClip  26
#define c_portRgn   34
#define c_bkPat     38
#define c_bkColor   40
#define c_pnColor   44
#define c_pnLoc     48
#define c_pnSize    52
#define c_pnMode    56
#define c_pnPat     58
#define c_pnCap     60
#define c_pnJoin    62
#define c_pnDash    64
#define c_pnOffset  66
#define c_pnLevel   68
#define c_txFont    70
#define c_txFace    74
#define c_txMode    76
#define c_txUnder   78
#define c_txScore   80
#define c_txPath    82
#define c_txAlign   84
#define c_txAngle   88
#define c_txSize    90
#define c_txSlant   94
#define c_txExtra   96
#define c_txSpace   98
#define c_mkType   100
#define c_mkSize   102
#define c_mkAngle  106
#define c_spare    108

#define c_xmin       0
#define c_ymin       2
#define c_xmax       4
#define c_ymax       6
#define c_X          0
#define c_Y          2
#define c_seg        0
#define c_off        2
#endif /*MW_Trim*/

/* End of File - GRPORTS.H */


