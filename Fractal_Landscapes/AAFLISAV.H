
#ifndef AAFLISAV_H
#define AAFLISAV_H

#ifndef AAFLI_H
#include "aafli.h"
#endif /* AAFLI_H */

/* Longest run for byte-oriented compression */
#define FLI_MAX_RUN 127


/** Low level assembler routines used by fii_lccomp to do a single line **/
/* Find out how far until have the next match of mustmatch or more pixels */
int fii_tnskip(Pixel *s1,Pixel *s2,int bcount,int mustmatch);
/* Find out how far until next run of identical pixels mustmatch long */
int fii_tnsame(Pixel *s,int bcount,int mustmatch);

/** Mid level routines to compress 1st frame, delta frames, and color maps **/
/* Run length compress a single frame using Animator 8086 
	1st frame technique */
Cbuf *fii_brun(Pixel *s1,Pixel *s2,USHORT *cbuf,int width,int height);
/* Delta compress a single frame using Animator 8086 byte-oriented scheme */
Cbuf *fii_lccomp(Pixel *s1,Pixel *s2,USHORT *cbuf,int width,int height);
/* fii_fccomp - compress an rgb triples color map doing 'skip' compression */
Cbuf *fii_fccomp(Cmap *s1, Cmap *s2, USHORT *cbuf, int count);
#define FLI_EMPTY_DCOMP 8  /* Size returned by above 3 to indicate no change */

/** Higher level routines to write FLI files **/

/* Compress a frame into a ram-buffer.  Return size of frame. */
long fli_comp_frame(Cbuf *comp_buf, 		/* Buffer FLI_CBUF_SIZE or bigger */
	Pixel *last_screen, Cmap *last_cmap, 	/* Data from previous frame */
	Pixel *this_screen, Cmap *this_cmap,	/* Data for this frame */
	int type);								/* FLI_BRUN?  FLI_LC? */	

/* Open fli file and write out first frame.  Return file handle if things
   go well, error code otherwise */
Jfile fli_create(char *fliname, 	/* file name */
	Fli_head *fh, 					/* fli_create will initialize this */
	int speed);						/* Speed in 1/70th of a second */

/* Write succeeding frames of a fli. */
Errval fli_write_next(Jfile ff, 		/* Fli file returned by fli_create */
	Fli_head *fh, 						/* Same header used by fli_create */
	Vscreen *this, 						/* Current frame */
	Vscreen *last);						/* Previous frame */

/* Finish up writing a FLI file...
   Write the 'ring frame', that is the difference between the first and
   last frame of a fli.  Pass in the final frame of the FLI in last_frame.
   firstf_buf will be loaded with the first frame of the FLI as a side
   effect.  */ 
Errval fli_end(Jfile ff, Fli_head *fh, 
	Vscreen *end_frame, Vscreen *firstf_buf);

#endif /* AAFLISAV_H */
