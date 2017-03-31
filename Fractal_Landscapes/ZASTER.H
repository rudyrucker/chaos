#ifndef _ZASTERH_
#define _ZASTERH_
/*

        Zoroaster Definitions

*/

#define MAXVERT 10                 /*  Maximum vertices in polygon  */

/* Fundamental data types */

typedef short scrcoord;            /* Screen co-ordinates */
typedef unsigned short zdepth;     /* Z-buffer depth value */

/* High values for those types */

#define HIGHZ    0xFFFF            /* Largest Z-depth value */
#define HIGHSCRC 32767             /* Largest screen co-ordinate value */

/* Screen point */

struct spoint {
        scrcoord x, y;             /* Actual screen X and Y co-ordinates */
        zdepth z;                  /* Scaled Z-buffer depth value */
};

/* Screen polygon */

struct spolygon { 
        short pcolour;              /* Colour code */
        short npoints;             /* Number of points in polygon */
        scrcoord pymin, pymax;     /* Y extents of points in polygon */
        struct spoint pt[MAXVERT + 1]; /* Actual points */
};

typedef struct spolygon *psp;

extern void zaster(int xsize,
   int ysize,
   int ncolors,
   int (*memall)(unsigned int,unsigned int,unsigned int),
   void (*memline)(int,int,unsigned char **,zdepth **),
   void (*memfree)(void),
   void (*polyrew)(void),
   struct spolygon *(*polyget)(void),
   void (*lineret)(int,unsigned char *));
extern void rastpext(struct spolygon *sp);

#endif



