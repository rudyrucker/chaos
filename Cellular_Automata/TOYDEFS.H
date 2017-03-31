#ifndef _TOYDEFSH_
#define _TOYDEFSH_



#define GCR 0x3ce    /* graphics controller */
#define SEQR 0x3c4    /* sequencer register */




#define FIRSTXPIXEL (160)
#define FIRSTXBYTE (FIRSTXPIXEL/8)

#define HIXCOUNT (640-FIRSTXPIXEL)
#define HIXWIDTH (HIXCOUNT+2)

#define MEDXCOUNT (HIXCOUNT/2)
#define MEDXWIDTH (MEDXCOUNT+2)


#define LOXCOUNT (HIXCOUNT/8)
#define LOXWIDTH (LOXCOUNT+2)

/* We want the highest number divisible by 8 and less than 350 here */

#define HIYCOUNT ((350/8)*8)
#define MEDYCOUNT (HIYCOUNT/2)
#define LOYCOUNT (HIYCOUNT/8)
#define MIDDLEYCOUNT (HIYCOUNT/4)
#define MIDDLEXCOUNT (HIXCOUNT/4)

typedef enum
{
	COARSE, DOUBLE, MED, HI
} DISPLAY_MODE;

// Button positions
enum {MAIN_F1HELP,MAIN_QUIT,MAIN_STOPGO,MAIN_FILE,
      MAIN_OPTIONS,MAIN_RANDOMIZE,MAIN_TYPENAME};
// And for each subset
enum {EAT_D=MAIN_TYPENAME+1,EAT_N};// Eat
enum {TUBE_HIDING=MAIN_TYPENAME+1,TUBE_EATING,TUBE_ALARM,TUBE_JAZZ};// Tube
enum {NLUKY_N=MAIN_TYPENAME+1,NLUKY_L,NLUKY_U,NLUKY_K,NLUKY_Y}; // NLUKY
enum {HODGE_NUMBER=MAIN_TYPENAME+1,HODGE_INCREMENT}; // Hodge

enum {CA_HODGE,CA_NLUKY,CA_EAT,CA_TUBE};
#define setNLUKY_N(l) setnlukys(0,l)
#define setNLUKY_L(l) setnlukys(1,l)
#define setNLUKY_U(l) setnlukys(2,l)
#define setNLUKY_K(l) setnlukys(3,l)
#define setNLUKY_Y(l) setnlukys(4,l)


#define COARSECORNERX ((MEDXCOUNT-LOYCOUNT)/2)
#define COARSECORNERY ((MEDYCOUNT-LOYCOUNT)/2)


#define MENUTEXT 15
#define BUTTONTEXT 14
#define MENUBACK 1
#define BUTTONBACK 8
#define BUTTONFRAME 0

#define TWICE(n) {\
   HideCursor();\
   SetBitmap(GrafPg0);n;\
   SetBitmap(GrafPg1);n;\
   SetBitmap(display_page ? GrafPg0 : GrafPg1);\
   ShowCursor();}


#define hasVGA vgaflag

#endif
