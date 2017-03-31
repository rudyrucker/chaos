
/* ************************************************************* * 
 *  METCOMPL.H  -  Compiler dependent defines and include files. *
 *  Copyright (c) 1985-1989  Metagraphics Software Corporation   * 
 * ************************************************************* */

/*     @(#)metcompl.h	3.1     12/4/89 11:07:23  */


/* C Compiler to use, choose one  */
#define MicrosoftC  0   /* 5.0, 5.1        */
#define QuickC      0   /* 1.0- 2.0        */   
#define LatticeC    0   /* 5.0, 6.01       */
#define MetawareC   0   /* 1.4, 1.5        */ 
#define TurboC      1   /* 1.1, 1.5, 2.0   */
#define ZortechCPP  0   /* 1.07            */
#define WatcomC     0   /* 6.0, 7.0        */
#define IC16M       0   /* 4.0             */
#define TI34010C    0   /*                 */


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

/* Compiler Dependent Defines */ 

#if      IC16M       /* Instant C is like Microsoft C */
#undef   MicrosoftC
#define  MicrosoftC  1        /* 5.0, 5.1                */
#endif /*IC16M*/  


#if      MicrosoftC
#define  FAR_malloc _fmalloc  /* Must use 'far' pointers */
#define  FAR_free   _ffree    /* for MetaWINDOW bitmaps. */
#endif /*MicrosoftC*/

#if      TurboC             
#define  FAR_malloc farmalloc /* Must use 'far' pointers */ 
#define  FAR_free   farfree   /* for MetaWINDOW bitmaps. */
#endif /*TurboC*/

#if      LatticeC 
#define  FAR_malloc malloc    
#define  FAR_free   free      
#define  far                  /* V3 didn't implement properly */
#define  signed               /* V3 didn't implement properly */               
#endif /*LatticeC */

#if      MetawareC
#define  far
#define  getch  getchar
#define  putch  putchar
#define  FAR_malloc malloc    /* Must use 'far' pointers */
#define  FAR_free   free      /* for MetaWINDOW bitmaps. */
/*pragma on Prototype_conversion_warn */
#endif /*MetawareC*/

#if      WatcomC             
#define  FAR_malloc _fmalloc  /* Must use 'far' pointers  */
#define  FAR_free   _ffree    /* for MetaWINDOW bitmaps.  */
#endif /*WatcomC */

#if      ZortechCPP             
#define  FAR_malloc farmalloc /* Must use 'far' pointers */ 
#define  FAR_free   farfree   /* for MetaWINDOW bitmaps. */
#define  putch  putchar
#endif /*ZortechCPP */

#if      TI34010C 
#define far                   /* No 'far' or 'signed'      */
#define signed                /* keywords for TI34010C.    */
#endif /*TI34010C*/


#ifndef   NO_INCLUDES

/* Compiler Includes Needed by MetaWINDOW */

#if      !TI34010C            /* Files everyone uses, except TI */
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#endif /*!TI34010C*/

#if      MicrosoftC
#include "process.h"
#include "conio.h"
#include "malloc.h"           /* Memory allocation functions */
#include "memory.h"
#include "dos.h"
#endif /*MicrosoftC*/

#if      TurboC             
#include "process.h"
#include "conio.h"
#include "alloc.h"            /* Memory allocation functions */ 
#include "mem.h"
#include "dos.h"
#endif /*TurboC*/

#if      LatticeC 
#include "dos.h"              /* Memory allocation functions */
#endif /*LatticeC*/

#if      WatcomC             
#include "stddef.h"           /* Defines the cdecl keyword */
#include "conio.h"
#include "malloc.h"           /* Memory allocation function */
#include "dos.h"
#endif /*WatcomC*/

#if      ZortechCPP             
#include "dos.h"
#endif /*ZortechCPP*/

#endif /* NO_INCLUDES  - - - - - - - - - - - - - - - - - - - - - */

/* METCOMPL.H - End of File */

