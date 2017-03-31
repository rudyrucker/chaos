
/* *************************************************************** *
 *     GRFONTS.H - MetaWINDOW FONT FILE RECORD DATA STRUCTURE     *
 *   Copyright (c) 1985, 1989  Metagraphics Software Corporation   *
 * *************************************************************** */

/*     @(#)grfonts.h	3.4     11/13/89 10:21:36  */

#define  fixed  long          /* for new, higher accuracy fields  */
#define  fontRec     fontRcd  /* old name compatibility           */

#define GRFONTS_H   1   

typedef struct _ofswid    /* Offset/Width Table Entry              */
  { unsigned char  wid;      /* character width                    */
    signed   char  ofs;      /* character offset                   */
  } ofswid;


typedef struct  _fontRcd     /* MetaWINDOW FONT DATA HEADER RECORD */
  {
  Byte           fontVer;       /* Font format version number (=2)    */
  Byte           fontRev;       /* Font style revision designation    */
  Byte           fontNameLen;   /* Font baseName length (0-15)        */
  char           fontBaseName[16]; /* Font Base Name                  */
  char           fontSuffix1[10];  /* Font suffix 1                   */
  char           fontSuffix2[10];  /* Font suffix 2                   */
  char           fontSuffix3[10];  /* Font suffix 3                   */
  char           fontFacing;    /* Synthesized font facing flags      */
  char           fontSign[8];   /* 'METAFONT' signature               */

  Byte           fontWeight;    /* Font weight                        */
  Byte           fontCoding;    /* Character set encoding             */
  long           fontSize;      /* Buffer size needed to hold font    */
  word           fontMax;       /* Maximum character code             */
  word           fontMin;       /* Minimum character code             */
  short          fontPtSize;    /* Point Size                         */
  short          fontfamily;    /* Font family                        */
  short          fontStyle;     /* Device style                       */
  short          fontFlags;     /* Font flags                         */
  long           fontColor[2];  /* Font Character & bkground colors   */

  Byte           minChar;       /* Minimum ASCII character code       */
  Byte           maxChar;       /* Maximum ASCII character code       */
  short          chWidth;       /* Fixed space character width        */
  short          chHeight;      /* Character height                   */
  short          chKern;        /* Fixed space character offset       */
  short          ascent;        /* Ascent                             */
  short          descent;       /* Descent                            */
  short          lnSpacing;     /* Vertical spacing between baselines */
  word           chBad;         /* Char to show for undefined codes   */
  short          chCenter[2];   /* Icon or Marker center (X,Y)        */
  short          chAngle;       /* Italicise slant angle              */
  short          chUnder;       /* recommended txUnder setting        */
  short          chScore;       /* recommended txScore setting        */
  long           locTbl;        /* offset to locTable                 */
  long           ofwdTbl;       /* offset to ofwdTable                */
  long           kernTbl;       /* offset to kernTable                */
  long           sizeTbl;       /* offset to size/rotation tbl(strk)  */
  long           grafMapTbl;    /* offset to font grafMap structure   */
  long           rowTbl;        /* offset to grafMap rowTable(s)      */
  long           fontTbl;       /* offset to font bitImage/nodes tbl  */
  long           fontNotice;    /* offset to trademk/cpyright notice  */
  long           fontSupplier;  /* offset to name of font supplier    */
  long           fontAuthor;    /* offset to name of font author      */
  long           fontInfo;      /* offset to miscellaneous font info  */
  long           fontDate;      /* offset to font creation date       */
  fixed          fontSpacing;   /* Vertical spacing between baselines */
  fixed          fontLowHgt;    /* baseline to top of lowercase 'x'   */
  fixed          fontCapHgt;    /* baseline to top of capital 'H'     */
  fixed          fontAscent;    /* baseline to top of lowercase 'd'   */
  fixed          fontDescent;   /* baseline to bottom of lowercase 'p'*/
  fixed          fontMaxWid;    /* maximum character width            */
  fixed          fontAvgWid;    /* average character width            */
  fixed          fontEmWid;     /* Em space width                     */
  short          fontRsvd;      /*   (reserved for future use)        */
  long           offwidTbl[16]; /* offset to facing off/wid tables    */
/*short          fontData[];*/  /* font data/image tables (variable)  */
  }  fontRcd;

#if      MW_Ver3
#define  fontHeader  fontRcd
#define  fontName    fontBaseName
#define  bmapTbl     grafMapTbl
#define  leading     lnSpacing
#endif /*MW_Ver3*/


#ifdef   COMMENTS
=============================================================================

               MetaWINDOW FONT FILE FORMAT VERSION 2.1

This document describes the font data record fields and structures used by
the MetaWINDOW font system.  The MetaWINDOW font system has been designed
to permit extensions to be added in a simplified manner such that existing
fonts remain upwardly compatible with new versions MetaWINDOW.  Similiarly,
a basic level of downward "compliablity" is offered for fonts created with
later extensions, but, obviously, new extensions and functions will
not be recognized by earlier versions of MetaWINDOW software.  This document
references MetaWINDOW Font File Format Version 2.1 implemented for MetaWINDOW
Version 4 (Font Format Version 2.0 was previously used with MetaWINDOW
Version 3).

The first 256 bytes of a MetaWINDOW binary font contains a font header
comprised of the following information:


MetaWINDOW FONT HEADER RECORD
-----------------------------

fontVer        is the MetaWINDOW font record format version number.  The
               font version designation is divided into two parts,
               minorRev|majorRev, as follows:

               bit:   7654 3210 

                      ---- xxxx    Major Revision
                      xxxx ----    Minor Revision

               The font format version number described in this document is
               Version 2.1 which referenced in fontVer as "0x12"


fontRev        is the revision designation set by the font designer for this
               particular font style.  The revision should be updated to
               reflect font revisions and edits.


fontNameLen    is the length (0-15) of the font baseName which follows.


fontBaseName   The ASCII string for the font name is divided into four parts.
fontSuffix1    The "fontBaseName" is the basic font face styling of the
fontSuffix2    font (up to 15 characters).  Up to three (3) additional
fontSuffix3    suffixes (up to 9 characters in length each) are optionally
               included to describe the fonts facing variation.  For
               example a "Sheffield-DemiOblique" font would be defined as:

                     fontBaseName = "Sheffield"
                     fontSuffix1  = "Demi"
                     fontSuffix2  = "Oblique"
                     fontSuffix3  =  NULL

               The partitioning of the font name facilitates creation
               of unique font abbreviations.  For example, an abbreviated
               font menu name or file name can be created by using the first
               five characters of the fontBaseName, and the first three
               characters of each fontSuffix.  "Sheffield-DemiOblique"
               becomes "SheffDemObl" in abbreviated form.


fontFacing     defines what types of synthesized font facings may be
               created by MetaWINDOW.  In certain cases seperate fonts
               may be created to more accurately represent differernt
               font faces rather than use MetaWINDOWs built-in synthesized
               font facing features.  For these cases, particular synthesized
               faces may be disabled and not performed by MetaWINDOW.
               The "fontFacing" field is defined as follows:

                 7654 3210  FONT FACING (.fontFacing)

                 ---- ---1  Enable(0)/disable(1) synthesized Bold face
                 ---- --1-  Enable(0)/disable(1) synthesized Italic face
                 ---- -1--  Enable(0)/disable(1) synthesized Condensed face
                 ---- 1---  Enable(0)/disable(1) synthesized Extended face
                 ---1 ----  Enable(0)/disable(1) synthesized Underline
                 --1- ----  Enable(0)/disable(1) synthesized Strikeout

               For example, if a seperate font has been created to more
               closely represent "TimesRoman-Italic", the .fontFacing
               flags for the font may be set to "0x0E" to allow only
               synthesized "bold", "underline" and "strikeout" facings
               with "italic", "condensed" and "extended" synthesis disabled.


fontSign       is the Metagraphics signature for a MetaWINDOW font.  This
               field is set equal to the eight ASCII characters "METAFONT"
               and may be tested to verify the identity of a Metagraphics
               font.  The "METAFONT" signature is also used to identify
               byte order sequence for fonts ported from other systems.


fontSize       is the size of the memory font buffer (in bytes) needed to
               hold this font.  fontSize defines the memory requirement
               needed for holding this font, which may be larger than the
               fonts "file size" after the font is decompacted.


fontMin        is the minimum character code value defined in this font.
               (minimum  0).


fontMax        is the maximum character code value defined in this font
               (the maximum character code value is 65535).


fontWeight     indicates the general font "blackness" in a range of values
               from 0 (light) to 255 (max-black).  Eight weight "ranges"
               are predefined for designating a fonts "weight" (the mid
               value is the nominal designation for each range):

                  weightRange   Nominal   Weight

                      0-31         16     Light
                     32-63         48     Book 
                     64-95         80     Medium  (default)
                     96-127       112     Demi-bold
                    128-159       144     Bold
                    160-191       176     Extra-bold
                    192-223       208     Ultra-bold
                    224-255       240     Black


fontCoding     designates the character encoding sequence for this font.
               The character encoding defines the mapping between character
               values and the displayed character.  The following character
               coding sequences are currently defined:

                                0 = ANSI (default)
                                1 = EBCDIC
                                2 = IBM
                                3 = ADOBE
                                4 = MAC
                                5 = KANJI
                                 

fontPtSize     is the designated font size as measured in "points" (1/72nds
               of an inch) for this font when displayed on a device with
               the appropriate resolution (pixResX and pixResY) for which
               this font was created.


fontFamily     identifies a generic font family grouping for this font,
               and a unique font identification number assigned by
               Metagraphics.  The fontFamily field is encoded in the
               following format:

                   FEDC BA98 7654 3210  FONT FAMILY

                   ---- xxxx xxxx xxxx  Font family ID number (0-4095)
                   xxxx ---- ---- ----  Font family group (0-16):
                     0  = undefined
                     1  = Serif (Times, Century Schoolbook, etc.)
                     2  = Sans-Serif (Helvetica, Swiss, etc.)
                     3  = Modern (Pica, Elite, Courier, etc.)
                     4  = Script (Cursive)
                     5  = Symbol
                     7  = Decorative
                     8  = Icon
                   9-15 = (reserved)


fontStyle      defines the font facing style characteristics for this
               font.  fontStyle is defined in the following format:

                  bit:   FDEC BA98 7654 3210  FONT STYLE (.fontStyle)

                  =  1   ---- ---- ---- ---1  Bold
                  =  2   ---- ---- ---- --1-  Italic
                  =  4   ---- ---- ---- -1--  Condensed
                  =  8   ---- ---- ---- 1---  Extended
                  = 16   ---- ---- ---1 ----  Underline
                  = 32   ---- ---- --1- ----  Strikeout
                  = 64   ---- ---- -1-- ----  Outline
                  =128   ---- ---- 1--- ----  Shadow



fontFlags      defines miscelleanous detail information regarding this font:

                 FEDC BA98 7654 3210  FONT FLAGS (.fontFlags)

                 ---- ---- ---- -xxx  Geometry:     0  = bitmap font
                                                    1  = stroked font
                                                    2  = outline font
                                                   3-7 = reserved

                 ---- ---- ---- x---  Pitch:        fixed(0) or
                                                    proportional(1) spacing

                 ---- ---- --xx ----  Compression:  0 = no compression
                                                    1 = 1D run-length encoding
                                                    2 = 2D run-length encoding

                 ---- --xx ---- ----  Offset/Width
                                        Precision:  0  = integer
                                                    1  = 1/256
                                                    2  = 1/65535

                 ---- xx-- ---- ----  Pixel Order:  0  = PC (left=msb) 
                                                    1  = TI (left=lsb)


fontColor      is the character color (fontColor[0]) and backgound color
               (fontColor[1]) of the font.  Normally these values are set
               to 0xFFFFFFFF (white) and 0x00000000 (black) for the character
               foreground and background colors, respectively.  These values
               are updated by MetaWINDOW during execution when a font
               needs to be color translated for multi-bit per pixel adaptors.


minChar        For ASCII encoded fonts, minChar defines the minimum ASCII
               code for the font.  Normally this value is "fontMin & 255".


maxChar        For ASCII encoded fonts, maxChar defines the maximum ASCII
               code for the font.  Normally this value is "fontMax & 255".


chWidth        defines the default character width, in integer pixels, when
               fixed-width facing is active.


chHeight       defines the height of the character bit image in pixels.
               "chHeight" is the equal to the sum of "ascent+descent".


chKern         specifies the default kerning offset, in integer pixels, when
               fixed-width facing is active (default 0).


ascent         is the number of pixels above the baseline the font rises.


descent        is the number of pixels below the baseline descenders fall.


lnSpacing      is the recommended spacing, in integer pixels, to seperate
               consecuative text lines (default chHeight+2).


chBad          is the character to display for "undefined" character codes.


chOrient       defines the orientation of the characters in this font,
               measured in integer tenth of degrees (default 0).


chCenter[2]    defines the center position for characters in symbol or
               marker fonts.  "chCenter" is measured from the lower-left
               corner of the character cell.  For character fonts chCenter
               should be set equal to (0,0).


chAngle        specifies the recommended slant angle for synthesizing
               italic facing.  chAngle is specified in interger tenth
               of degree increments (default -200 = -20.0 degrees).


chUnder        is the recommended TextUnder setting for this font (default 1).


chScore        is the recommended TextScore setting for this font (default 1).


locTbl         is the offset to the start of the "locTable" array.


ofwdTbl        is the offset to the start of the "ofwdTable" array.


kernTbl        is the offset to the start of the "kernTable" array.


sizeTbl        is the offset to the start of the size/rotation table
               for stroked/outline fonts.


bmapTbl        is the offset to the start of the fonts "grafMap"
               (bitmap) data record.


rowTbl         is the offset to the start of the fonts grafMap
               "rowTable" array.


fontTbl        is the offset to the start of the fonts pixel image
               data (for bitmap fonts), or to the fonts geometry data
               (for stroked/outline fonts).


fontNotice     is the offset to the start of the character string of
               the fonts trademark/copyright notice.


fontSupplier   is the offset to the start of the character string of
               the font suppliers name.


fontAuthor     is the offset to the start of the character string of
               the fonts author name.


fontInfo       is the offset to the start of the character string
               containing miscellaneous information about this font.


The following font fields are defined for use with "precision" fonts only
(fontFlags.precision > 0).  


fontSpacing    is the fixed point (integer.fractional) distance recommended
               to seperate consecutive baselines of text vertically.


fontxHgt       is the fixed point distance (integer.fraction) between
               the font baseline to the top of lowercase "x".


fontCapHgt     is the fixed point distance (integer.fraction) between
               the font baseline to the top of capital "H".


fontAscent     is the fixed point distance (integer.fraction) between
               the font baseline to the top of lowercase "d".


fontDescent    is the fixed point distance (integer.fraction) between
               the font baseline to the bottom of lowercase "p".


fontMaxWid     is the fixed point width (integer.fraction) of the
               widest character in the font (typically "M" or "W").


fontAvgWid     is the fixed point value (integer.fraction) for the
               average character width in the font.


fontEmWid      is the fixed point value (integer.fraction) for a
               typographic "em" width for this font.


offwidTbl      is a table of offsets from the beginning of the font buffer
               to the start of multiple facing offset/width facing tables.
               The following bit combinations serve as "offwidTbl" indexes
               for locating different ofwdTbl arrays for use with synthesized
               facing:
               
                      =1      ---1  Bold
                      =2      --1-  Italic
                      =4      -1--  Condensed
                      =8      1---  Extended

               For example, "offwidTbl[3]" is the offset value from the
               beginning of the font record to the start of the "ofwdTable"
               to use when performing synthesized "bold+italic" facing.


=============================================================================
#endif /*COMMENTS*/

/* End of file - METFONTS.H */



