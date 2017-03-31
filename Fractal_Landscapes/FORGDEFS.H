#ifndef _FORGDEFSH_
#define _FORGDEFSH_
/*
      (C) Copyright 1990 by Autodesk, Inc.

******************************************************************************
*									     *
* The information contained herein is confidential, proprietary to Autodesk, *
* Inc., and considered a trade secret as defined in section 499C of the      *
* penal codes of the State of California.  Use of this information by anyone  *
* other than authorized employees of Autodesk, Inc. is granted only under a  *
* written non-disclosure agreement, expressly prescribing the scope and      *
* manner of such use.	                                                     *						                                                           *
******************************************************************************/

#define MENUBACK MenuBackColor
#define MENUTEXT MenuTextColor
#define BUTTONBACK ButtonBackColor
#define BUTTONTEXT ButtonTextColor
#define BUTTONFRAME ButtonFrameColor
#define FASTDOT
#define DITHERN  4 
#define NCOLOURS (DITHERN * DITHERN)
#define SCRSAT	 63 
#define SCRGAMMA 1.75
typedef enum
{
	Clouds = 0, Mountains, Planet, Craters, Testing,Nothing
} Picture;


enum {VGA256,VGA16,EGA16};
#define Spal(x,ri,gi,bi) clut[x][0] = ri; clut[x][1] = gi; clut[x][2] = bi


typedef struct {
   double dimension,height,power;
   double elevation,azimuth,hour,season;
   int terms;
} animation_parameters;

#ifndef __COLORS
#define __COLORS
#endif

enum COLORS {
   BLACK,
   LIGHTRED,
   YELLOW,
   LIGHTGREEN,
   LIGHTCYAN,
   LIGHTBLUE,
   LIGHTMAGENTA,
   WHITE,
   DARKGRAY,
   RED,
   BROWN,
   GREEN,
   CYAN,
   BLUE,
   MAGENTA,
   LIGHTGRAY,
};

   
#endif
