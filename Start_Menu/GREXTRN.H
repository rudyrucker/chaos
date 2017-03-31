/* ************************************************************* *
 *  GREXTRN.H  -  MetaWINDOW External Procedure Declarations     *
 *  Copyright (c) 1985-1989  Metagraphics Software Corporation   * 
 *  Include after GRCONST.H, GRPORTS.H                           *      
 * ************************************************************* */

/*     @(#)grextrn.h	3.7     11/29/89 17:04:56  */

#if      MS_DOS

#if      WatcomC | ZortechCPP              
#define SEQ cdecl
#else
#define SEQ far
#endif

#endif /*MS_DOS*/

void  SEQ  AddPt          (point far * /*SRCPT*/, point far * /*DSTPT*/);
void  SEQ  AlignPattern   (short /*PATTERN*/, short /*PATX*/, short /*PATY*/);
void  SEQ  BackColor      (short /*COLOR*/);
void  SEQ  BackLColor     (long /*LCOLOR*/);
void  SEQ  BackPattern    (short /*PATTERN*/);
void  SEQ  BorderColor    (short /*COLOR*/);
void  SEQ  CenterRect     (point far * /*PT*/, short /*WIDTHX*/,
                           short /*HEIGHTY*/, rect far * /*R*/);
short SEQ  CharWidth      (char /*CH*/);
void  SEQ  ClearText      (void);
void  SEQ  ClipRect       (rect far * /*R*/);
word  SEQ  CodeSeg        (void);
void  SEQ  CopyBits       (bitmap far * /*SRCMAP*/, bitmap far * /*DSTMAP*/,
                           rect far * /*SRCRECT*/, rect far * /*DSTRECT*/,
                           rect far * /*DSTCLIP*/, short /*RASOP*/);
void  SEQ  CursorBitmap   (bitmap far * /*NEWMAP*/);
void  SEQ  CursorColor    (long /*FORECOLOR*/, long /*BACKCOLOR*/);
void  SEQ  CursorMap      (short far * /*BUTTONMAP*/);
void  SEQ  CursorStyle    (short /*CURSORNBR*/);
word  SEQ  DataSeg        (void);
void  SEQ  DefineCursor   (short /*CURSORNBR*/, short /*HOTX*/, short /*HOTY*/,
                           cursor far * /*SCREENMASK*/,
                           cursor far * /*CURSORMASK*/);
void  SEQ  DefineDash     (short /*DASHNBR*/, dashRcd far * /*DASHARRAY*/);
void  SEQ  DefinePattern  (short /*PATTERN*/, patRcd far * /*PATIMAGE*/);
void  SEQ  DrawChar       (char /*CH*/);
void  SEQ  DrawString     (char far * /*TEXTSTR*/);
void  SEQ  DrawText       (char far * /*TEXTSTR*/,
                           short /*INDEX*/, short /*COUNT*/);
void  SEQ  DupBitmap      (bitmap far * /*SRCMAP*/,
                           bitmap far * /*DSTMAP*/);
void  SEQ  DupPort        (metaPort far * /*SRCPORT*/,
                           metaPort far * /*DSTPORT*/);
void  SEQ  DupPt          (point far * /*SRCPT*/, point far * /*DSTPT*/);
void  SEQ  DupRect        (rect far * /*SRCRECT*/, rect far * /*DSTRECT*/);
short SEQ  EqualPt        (point far * /*PT1*/, point far * /*PT2*/);
short SEQ  EqualRect      (rect far * /*RECT1*/, rect far * /*RECT2*/);
void  SEQ  EraseArc       (rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ARCANGLE*/);
void  SEQ  EraseOval      (rect far * /*R*/);
void  SEQ  ErasePoly      (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/);
void  SEQ  EraseRect      (rect far * /*R*/);
void  SEQ  EraseRoundRect (rect far * /*R*/, short /*DIAX*/, short /*DIAY*/);
void  SEQ  EventQueue     (short /*TF*/);
word  SEQ  ExtraSeg       (void);
short SEQ  FileAttr       (char far * /*FILENAME*/, short /*ATTR*/);
short SEQ  FileDelete     (char far * /*FILENAME*/);
short SEQ  FileLoad       (char far * /*FILENAME*/,
                           char far * /*DATAAREA*/, word /*AREASIZE*/);
short SEQ  FileQuery      (char far * /*FILESPEC*/,
                           dirRec far * /*DIRINFO*/, short /*DIRSIZE*/);
short SEQ  FileRename     (char far * /*OLDNAME*/, char far * /*NEWNAME*/);
short SEQ  FileStore      (char far * /*FILENAME*/,
                           char far * /*DATAAREA*/, word /*AREASIZE*/);
void  SEQ  FillArc        (rect far * /*R*/, short /*BGNANGLE*/,
                           short /*ARCANGLE*/, short /*PATTERN*/);
void  SEQ  FillOval       (rect far * /*R*/, short /*PATTERN*/);
void  SEQ  FillPoly       (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/, short /*PATTERN*/);
void  SEQ  FillRect       (rect far * /*R*/, short /*PATTERN*/);
void  SEQ  FillRoundRect  (rect far * /*R*/,
                           short /*DIAX*/, short /*DIAY*/, short /*PATTERN*/);
void  SEQ  FrameArc       (rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ARCANGLE*/);
void  SEQ  FrameOval      (rect far * /*R*/);
void  SEQ  FramePoly      (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/);
void  SEQ  FrameRect      (rect far * /*R*/);
void  SEQ  FrameRoundRect (rect far * /*R*/, short /*DIAX*/, short /*DIAY*/);
void  SEQ  Gbl2LclPt      (point far * /*PT*/);
void  SEQ  Gbl2LclRect    (rect far * /*R*/);
void  SEQ  Gbl2VirPt      (point far * /*PT*/);
void  SEQ  Gbl2VirRect    (rect far * /*R*/);
void  SEQ  GetAddress     (void far * /*VARBL*/,
                           short far * /*SGM*/, short far * /*OFF*/);
long  SEQ  GetBits        (short /*X*/, short /*Y*/,bitmap far * /*SRCMAP*/);
short SEQ  GetBMapField   (short /*FIELD*/);
void  SEQ  GetCmdLine     (char far * /*CMDSTRING*/);
short SEQ  GetFontField   (short /*FIELD*/);
void  SEQ  GetLPixel      (short /*X*/, short /*Y*/, long far * /*LCOLOR*/);
void  SEQ  GetPenState    (penState far * /*STATE*/);
short SEQ  GetPixel       (short /*X*/, short /*Y*/);
void  SEQ  GetPort        (metaPort far * far * /*PORTPTR*/);
short SEQ  GetPortField   (short /*FIELD*/);
void       GrInitErr      (int); /* error reporting mechanism */
short SEQ  HardCopy       (metaPort far * /*SOURCE PORT*/ );
void  SEQ  HideCursor     (void);
void  SEQ  HidePen        (void);
short SEQ  ImagePara      (rect far * /*R*/);
long  SEQ  ImageSize      (rect far * /*R*/);
short SEQ  InceptRect     (rect far * /*RECT1*/, rect far * /*RECT2*/,
                           rect far * /*DSTRECT*/);
void  SEQ  InitBitmap     (bitmap far * /*NEWMAP*/, short /*INTERLEAVE*/,
                           short /*BASESEG*/, short /*BASEOFF*/,
                           short /*INTERSEG*/, short /*INTEROFF*/);
short SEQ  InitGrafix     (short /*DEVMODE*/);
void  SEQ  InitMouse      (short /*MOUSPORT*/);
void  SEQ  InitPort       (metaPort far * /*PORT*/);
void  SEQ  InitRowTable   (bitmap far * /*NEWMAP*/, short /*INTERLEAVE*/,
                           short /*INTERSEG*/, short /*INTEROFF*/);
void  SEQ  InsetRect      (rect far * /*R*/, short /*DX*/, short /*DY*/);
void  SEQ  InvertArc      (rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ARCANGLE*/);
void  SEQ  InvertOval     (rect far * /*R*/);
void  SEQ  InvertPoly     (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/);
void  SEQ  InvertRect     (rect far * /*R*/);
void  SEQ  InvertRoundRect(rect far * /*R*/, short /*DIAX*/, short /*DIAY*/);
short SEQ  KeyEvent       (short /*WAIT*/, event far * /*EVNT*/);
void  SEQ  Lcl2GblPt      (point far * /*PT*/);
void  SEQ  Lcl2GblRect    (rect far * /*R*/);
void  SEQ  Lcl2VirPt      (point far * /*PT*/);
void  SEQ  Lcl2VirRect    (rect far * /*R*/);
void  SEQ  LimitMouse     (short /*XMIN*/, short /*YMIN*/,
                           short /*XMAX*/, short /*YMAX*/);
void  SEQ  LineRel        (short /*DLTX*/, short /*DLTY*/);
void  SEQ  LineTo         (short /*X*/, short /*Y*/);
short SEQ  LoadFont       (char far * /*FONTNAME*/);
void  SEQ  LoadPalette    (short /*BGNINDEX*/, short /*ENDINDEX*/,
                           short far * /*COLORS*/);
void  SEQ  MapPoly        (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/,
                           rect far * /*SRCR*/, rect far * /*DSTR*/);
void  SEQ  MapPt          (point far * /*PT*/,
                           rect far * /*SRCR*/, rect far * /*DSTR*/);
void  SEQ  MapRect        (rect far * /*R*/,
                           rect far * /*SRCR*/, rect far * /*DSTR*/);
void  SEQ  MarkerAngle    (short /*ANGLE*/);
void  SEQ  MarkerSize     (short /*WIDTH*/, short /*HEIGHT*/);
void  SEQ  MarkerType     (short /*MARKERNBR*/);
void  SEQ  MetaVersion    (char far * /*STRING < 60 chars*/);
void       MetQuery       (int /*ARGC*/, char *[] /*ARGV*/);
void  SEQ  MiterLimit     (short /*LIMITANGLE*/);
void  SEQ  MoveCursor     (short /*X*/, short /*Y*/);
void  SEQ  MovePortTo     (short /*GBLLEFT*/, short /*GBLTOP*/);
void  SEQ  MoveRel        (short /*DLTX*/, short /*DLTY*/);
void  SEQ  MoveTo         (short /*X*/, short /*Y*/);
void  SEQ  OffsetPoly     (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/, short /*DX*/, short /*DY*/);
void  SEQ  OffsetRect     (rect far * /*R*/, short /*DX*/, short /*DY*/);
void  SEQ  OvalPt         (rect far * /*R*/, short /*ANGLE*/,
                           point far * /*PT*/);
void  SEQ  PaintArc       (rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ENDANGLE*/);
void  SEQ  PaintOval      (rect far * /*R*/);
void  SEQ  PaintPoly      (short /*POLYCNT*/, polyHead far * /*POLYHDR*/,
                           point far * /*POLYPTS*/);
void  SEQ  PaintRect      (rect far * /*R*/);
void  SEQ  PaintRoundRect (rect far * /*R*/, short /*DIAX*/, short /*DIAY*/);
short SEQ  PeekEvent      (short /*INDEX*/, event far * /*EVNT*/);
void  SEQ  PenCap         (short /*CAPSTYLE*/);
void  SEQ  PenColor       (short /*COLOR*/);
void  SEQ  PenDash        (short /*DASHSTYLE*/);
void  SEQ  PenJoin        (short /*JOINSTYLE*/);
void  SEQ  PenLColor      (long /*LCOLOR*/);
void  SEQ  PenMode        (short /*RASOP*/);
void  SEQ  PenNormal      (void);
void  SEQ  PenOffset      (short /*DASHOFFSET*/);
void  SEQ  PenPattern     (short /*PATTERN*/);
void  SEQ  PenSize        (short /*DIAX*/, short /*DIAY*/);
void  SEQ  PlaneMask      (short /*PMASK*/);
void  SEQ  PolyLine       (short /*PTCNT*/, point far * /*PTS*/);
void  SEQ  PolyMarker     (short /*NPTS*/, point far * /*XYPTS*/);
void  SEQ  PopGrafix      (pusharea far * /*SAVEAREA*/);
void  SEQ  PortBitmap     (bitmap far * /*NEWMAP*/);
void  SEQ  PortOrigin     (short /*TOPBOT*/);
void  SEQ  PortSize       (short /*PIXWIDTHX*/, short /*PIXHEIGHTY*/);
void  SEQ  ProtectOff     (void);
void  SEQ  ProtectRect    (rect far * /*R*/);
void  SEQ  Pt2Rect        (point far * /*PT1*/, point far * /*PT2*/,
                           rect far * /*R*/);
short SEQ  PtInArc        (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ARCANGLE*/);
short SEQ  PtInOval       (point far * /*TESTPT*/, rect far * /*R*/);
short SEQ  PtInPoly       (point far * /*TESTPT*/, short /*POLYCNT*/,
                           polyHead far * /*POLYHDRS*/,
                           point far * /*POLYPTS*/);
short SEQ  PtInRect       (point far * /*TESTPT*/, rect far * /*R*/);
short SEQ  PtInRoundRect  (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*DIAX*/, short /*DIAY*/);
short SEQ  PtOnArc        (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*BGNANGLE*/, short /*ARCANGLE*/,
                           short /*SIZEX*/, short /*SIZEY*/);
short SEQ  PtOnLine       (point far * /*TESTPT*/,
                           point far * /*PT1*/, point far * /*PT2*/,
                           short /*SIZEX*/, short /*SIZEY*/);
short SEQ  PtOnOval       (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*SIZEX*/, short /*SIZEY*/);
short SEQ  PtOnRect       (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*SIZEX*/, short /*SIZEY*/);
short SEQ  PtOnRoundRect  (point far * /*TESTPT*/, rect far * /*R*/,
                           short /*DIAX*/, short /*DIAY*/,
                           short /*SIZEX*/, short /*SIZEY*/);
short SEQ  PtOnPoly       (point far * /*TESTPT*/, short /*POLYCNT*/,
                           polyHead far * /*POLYHDRS*/,
                           point far * /*POLYPTS*/, short /*penX*/, short /*penY*/);
short SEQ  PtToAngle      (rect far * /*R*/, point far * /*PT*/);
void  SEQ  PushGrafix     (pusharea far * /*SAVEAREA*/);
short SEQ  QueryColors    (void);
short SEQ  QueryComm      (void);
void  SEQ  QueryCursor    (short far * /*CURSORX*/, short far * /*CURSORY*/,
                           short far * /*CURSORLEVEL*/, short far * /*BUTTONS*/);
short SEQ  QueryError     (void);
short SEQ  QueryGrafix    (void);
void  SEQ  QueryPosn      (short far * /*X*/, short far * /*Y*/);
void  SEQ  QueryRes       (short far * /*RESX*/, short far * /*RESY*/);
short SEQ  QueryX         (void);
short SEQ  QueryY         (void);
void  SEQ  RasterOp       (short /*RASOP*/);
void  SEQ  ReadImage      (rect far * /*SRCR*/, image far * /*DSTIMAGE*/);
void  SEQ  ReadMouse      (short far * /*X*/, short far * /*Y*/, short far * /*SW*/);
void  SEQ  ReadPalette    (int /*PALNBR*/, int /*BGNINDX*/, int /*ENDINDX*/,
                           palData far * /*COLORS[]*/);
void  SEQ  ScaleMouse     (short /*FACTORX*/, short /*FACTORY*/);
void  SEQ  ScalePt        (point far * /*PT*/,
                           rect far * /*SRCR*/, rect far * /*DSTR*/);
void  SEQ  ScreenRect     (rect far * /*SCRNR*/);
void  SEQ  ScreenSize     (short far * /*SCRNXMAX*/, short far * /*SCRNYMAX*/);
void  SEQ  ScrollRect     (rect far * /*R*/, short /*DX*/, short /*DY*/);
void  SEQ  SetBitmap      (short /*PAGE*/);
void  SEQ  SetBits        (short /*X*/, short /*Y*/,long /*color*/,
                           short /*RASOP*/, rect far * /*clip rect*/,
                           bitmap far * /*SRCMAP*/);
void  SEQ  SetHardCopy    (HCInfoRec far *);
void  SEQ  SetDisplay     (short /*PAGE*/);
void  SEQ  SetFont        (fontRcd far * /*FONTBUFFER*/);
void  SEQ  SetLocal       (void);
void  SEQ  SetOrigin      (short /*LCLX*/, short /*LCLY*/);
void  SEQ  SetPalette     (short /*BACKGROUND*/, short /*PALETTE*/);
void  SEQ  SetPenState    (penState far * /*STATE*/);
void  SEQ  SetPixel       (short /*X*/, short /*Y*/);
void  SEQ  SetPort        (metaPort far * /*NEWPORT*/);
void  SEQ  SetPt          (point far * /*PT*/, short /*X*/, short /*Y*/);
void  SEQ  SetRect        (rect far * /*R*/, short /*X1*/, short /*Y1*/,
                           short /*X2*/, short /*Y2*/);
void  SEQ  SetVirtual     (void);
short SEQ  ShiftRect      (rect far * /*R*/, short /*DX*/, short /*DY*/,
                           rect far * /*R1*/, rect far * /*R2*/);
void  SEQ  ShowCursor     (void);
void  SEQ  ShowPen        (void);
word  SEQ  StackPtr       (void);
word  SEQ  StackSeg       (void);
void  SEQ  StopEvent      (void);
void  SEQ  StopMouse      (void);
void  SEQ  StopTimer      (void);
short SEQ  StoreEvent     (event far * /*EVNT*/);
short SEQ  StringWidth    (char far * /*TEXTSTR*/);
void  SEQ  SubPt          (point far * /*SRCPT*/, point far * /*DSTPT*/);
short SEQ  SystemFont     (short /*FONTNBR*/);
void  SEQ  TextAlign      (short /*HORZ*/, short /*VERT*/);
void  SEQ  TextAngle      (short /*ANGLE*/);
void  SEQ  TextExtra      (short /*SPACE*/);
void  SEQ  TextFace       (short /*FACE*/);
void  SEQ  TextMode       (short /*RASOP*/);
void  SEQ  TextPath       (short /*PATH*/);
void  SEQ  TextScore      (short /*GAP*/);
void  SEQ  TextSize       (short /*WIDTHX*/, short /*HEIGHTY*/);
void  SEQ  TextSlant      (short /*ANGLE*/);
void  SEQ  TextSpace      (short /*SPACE*/);
void  SEQ  TextUnder      (short /*BELOW*/);
short SEQ  TextWidth      (char far * /*TEXTSTR*/,
                           short /*INDEX*/, short /*COUNT*/);
void  SEQ  TrackCursor    (short /*TF*/);
void  SEQ  UnionRect      (rect far * /*RECT1*/, rect far * /*RECT2*/,
                           rect far * /*DSTRECT*/);
void  SEQ  UserMouse      (void (far *)() /*MOUSESVC*/,
                           MCBtype far * /*MCB*/);
void  SEQ  MouseSvc       (short, short, short, MCBtype far *);
void  SEQ  UserTimer      (void (far *)() /*TIMERSVC*/, short /*TIMECOUNT*/,
                           CCBtype far * /*CCB*/);
void  SEQ  TimerSvc       (CCBtype far *);
void  SEQ  Vir2GblPt      (point far * /*PT*/);
void  SEQ  Vir2GblRect    (rect far * /*R*/);
void  SEQ  Vir2LclPt      (point far * /*PT*/);
void  SEQ  Vir2LclRect    (rect far * /*R*/);
void  SEQ  VirtualRect    (rect far * /*R*/);
void  SEQ  WriteImage     (rect far * /*DSTR*/, image far * /*SRCIMAGE*/);
void  SEQ  WritePalette   (int /*PALNBR*/, int /*BGNINDX*/, int /*ENDINDX*/,
                           palData far * /*COLORS[]*/);
void  SEQ  XlateImage     (image far * /*SRCIMAGE*/, image far * /*DSTIMAGE*/,
                           short /*DSTBITS*/, short /*DSTPLANES*/,
                           long far * /*XLTABLE*/);
short SEQ  XYInRect       (short /*X*/, short /*Y*/, rect far * /*R*/);
void  SEQ  ZoomBits       (bitmap far * /*SRCMAP*/, bitmap far * /*DSTMAP*/,
                           rect far * /*SRCRECT*/, rect far * /*DSTRECT*/,
                           rect far * /*DSTCLIP*/, short /*RASOP*/);

/* End of File  -  GREXTRN.H */


