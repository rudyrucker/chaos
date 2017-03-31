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
#ifndef _FORGVARSH_
#define _FORGVARSH_

#define EI extern int
#define EUC extern unsigned char
#define EC extern char
#define ER extern rect
#define ED extern double


EI CommPort;
extern metaPort *thePort;
extern bitmap *theBitmap;
ER sR;
EI FontHeight;
EI StringWidthX;
EI MenuBackColor;
EI MenuTextColor;
EI ButtonBackColor;
EI ButtonFrameColor;
EI ButtonTextColor;
EI mode;
ED arand, gaussadd, gaussfac;	/* Gaussian random parameters */
EI xdots,ydots;
ER menuRect;
ER tweakRect;
ER clockRect;
ER planetRect;
ER lineRect;
extern unsigned int rseed,seed;
EI seeded;
ED fracdim;
ED elevfac;
ED power;
ED aspect;
extern short ditherm[DITHERN][DITHERN];
extern short colmap[8];
EUC clut[256][3];	/* Colour lookup table */
EUC xlut[256][3];	/* Colour lookup table */
extern short brightpal[];
ED vdown,vturn,mscale;
EI RenderingMode;
EI hasVGA;
EI changed;
extern float *databuffer;

EI meshsizes[];
EI termsizes[];
EI mesh;
ED shang,siang;
EI histogrammer;
ER littlePlanetRects[4];
EI renderingthing,renderingmode,renderingdest;
EI aborted;
EI WireFraming;
EI SmoothingPures;
EUC defaultpalette[64][3];
EUC planet_clut[256][3];	/* Colour lookup table */
EI DoDither,DoIce,DoWholeScreen,DoClouds;
EI ticker_active;
extern animation_parameters animation_start,animation_end;
EI first_mountain_projection;
EI PreScale;
EI ticking;
ED percentage;
EI terms;
ED dPlanetStart;
EI DoRotation;
extern unsigned long tickertape[100];
EI SmoothingPures;
EI WrappingPures;
ED power_slider_value;
EUC rowbuffer[640];
EI frame_stamping;
EI parameter_stamping;
ER TickerFrame;
EI safe_alloc;
EI prog_init;
#undef EI 
#undef EUC
#undef EC
#undef ER
#undef ED

#endif


