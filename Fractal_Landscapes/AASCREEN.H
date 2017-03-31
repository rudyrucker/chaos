
#ifndef AASCREEN_H
#define AASCREEN_H

#ifndef AATYPES_H
#include "aatypes.h"
#endif /* AATYPES_H */


/* Graphics types */
typedef unsigned char Cmap;
typedef unsigned char Pixel;
typedef unsigned char Bitplane;

/* Constants pertaining to 320x200 256 color mode mostly */
#define AA_VGA_SCREEN ((Pixel *)0xa0000000)
#define AA_XMAX 320
#define AA_YMAX 200
#define AA_BPR 320
#define AA_COLORS 256

/* This structure is something we can draw on.  A superset of Vcel
   (and keep it that way or things break!)  */
struct vscreen
	{
	int x, y;	/* upper left corner in screen coordinates */
	unsigned w, h;	/* width, height */
	unsigned bpr;	/* bytes per row of image p */
	Pixel *p;	/* Starts on segment boundary */
	Cmap *cmap;
	long psize;	/* size of pixels */
	Pixel *allocedp;	/* for memory based screens only */
	};
typedef struct vscreen Vscreen;
extern Vscreen aa_screen;
extern Cmap aa_colors[];	/* software echo of color map */
Boolean aa_open_vga_screen(void);	/* opens 256 color screen */
void aa_close_vga_screen(void);
Vscreen *aa_alloc_mem_screen(void);	/* Open a screen can draw on but not see */
void aa_free_mem_screen(Vscreen *ms);	/* dispose of a memory screen */
void aa_copy_screen(Vscreen *source, Vscreen *dest);
void aa_clear_screen(Vscreen *vs);

/* Get the current video mode */
int dos_get_vmode(void);
/* Set video mode.  Mode 0x13 is 320x200 256 color */
void dos_set_vmode(int mode);
/* Set the VGA color map. */
void aa_set_colors(int start, int count, Cmap *cmap);	
/* Wait until in vertical blank */
void aa_wait_vblank(void);
/* Wait until out of vertical blank */
void aa_wait_no_vblank(void);

#endif /* AASCREEN_H */
