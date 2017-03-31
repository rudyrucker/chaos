
#ifndef AAFLI_H
#define AAFLI_H

#ifndef AATYPES_H
#include "aatypes.h"
#endif /* AATYPES_H */

#ifndef AADOS_H
#include "aados.h"
#endif /* AADOS_H */

#ifndef AAERR_H
#include "aaerr.h"
#endif /* AAERR_H */

#ifndef AASCREEN_H
#include "aascreen.h"
#endif /* AASCREEN_H */


typedef UBYTE Cbuf;		/* compression buffer */

#define FLI_MAXFRAMES (4*1000)	/* Max number of frames... */

/* Normal Magic */
#define FLIH_MAGIC 0xaf11	

/* Frame Magic */
#define FLIF_MAGIC 0xf1fa

typedef struct fli_head
	{
	long size;
	USHORT type;  /* = FLIH_MAGIC */
	USHORT frame_count;
	USHORT width;
	USHORT height;
	USHORT bits_a_pixel;
	SHORT flags;
	SHORT speed;
	long next_head;
	long frames_in_table;
	int file;	/* used by players.  Contains zeros on disk. */
	long frame1_off;	/* used by players.  Contains zeros on disk. */
	long strokes;	/* how many paint strokes etc. made. */
	long session; /* stokes since file's been loaded. */
	char reserved[88];	/* all zeroes on disk */
	} Fli_head;

/* bit defines for flags field */
#define FLI_FINISHED 1	/* finished writing fli */
#define FLI_LOOPED	2	/* fli has a loop frame */

typedef struct fli_frame
	{
	long size;
	USHORT type;		/* = 0xf1fa FLIF_MAGIC */
	SHORT chunks;
	char pad[8];
	} Fli_frame;



typedef struct fli_chunk
	{
	long size;
	SHORT type;
	} Fli_chunk;

/* size of buffer that'll be big enough to hold worst case FLI frame */
#define FLI_CBUF_SIZE (64000L+3*AA_COLORS+2*sizeof(Fli_chunk)+sizeof(Fli_frame))

/* types of chunk in a fli_frame */
#define FLI_COLOR 11
#define FLI_LC	12
#define FLI_BLACK 13
#define FLI_BRUN 15
#define FLI_COPY 16


/* Low level decompression functions */
void fii_unbrun(Cbuf *cbuf, Pixel *screen, int linect);	/* 1st frame */
void fii_unlccomp(Cbuf *cbuf, Pixel *screen);		/* delta frames */
void fii_mem_fcuncomp(Cbuf *cbuf, Cmap *cmap);		/* colors to memory */
void fii_reg_fcuncomp(Cbuf *cbuf);					/* colors to registers */

/* Higher level FLI playing functions */
	/* Decompress a single frame that's in RAM */
void fli_uncomp(Vscreen *f, /* the screen to update */
	Fli_frame *frame, 	   /* Header for this frame */
	Cbuf *cbuf, 		   /* Compressed data for this frame */
	Boolean see_colors);   /* update the hardware color map? */

	/* Read in fli-header, verify that it's a fli-file, and return file
	   handle.  See aaerr.h for negative return values if there are problems. */
Jfile fli_open(char *fliname, Fli_head *fh);

	/* Read in next frame and uncompress onto screen, optionally updating
	   hardware color palette */
Errval fli_read_display_frame(Jfile ff, Vscreen *v, Boolean see_colors);

	/* Read and display next frame onto VGA display */
Errval fli_next_frame(Jfile ff);

	/* Play fli, going on forever or until 'until' function returns FALSE.
	   Until is called with the current frame, the total frame in the fli,
	   and how many times have played entire fli. */
Errval fli_until(char *fliname, 	/* name of fli to play */
	int speed,				/* if speed negative, use speed in file */
	AAivec until);		/* function to call to see when to stop */

	/* The 'until' function we use to construct fli_play */
Boolean fli_until_key(int cur_frame, int frame_count, int cur_loop);

	/* Play fli looping forever until any key is hit */
Errval fli_play(char *fliname);

	/* Play fli once */
Errval fli_once(char *fliname);

#endif /* AAFLI_H */
