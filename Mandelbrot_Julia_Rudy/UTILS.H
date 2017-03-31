#ifndef _UTILSH_
#define _UTILSH_
#define EI extern int
#define EV extern void
EI ShadowAndSave(rect *tR);
EV PushCursorPosition(void);
EV PopCursorPosition(void);
EV move_to_corner(rect *R);
EV WaitForNothing(void);
EI SomethingWaiting(void);
EV TempFileName(char *buf,char *name);
EV RangeError(char *msg);
EI cancel_ok_msg(char *msg);
EV ErrorBox(char *s);
EI Overwrite(char *f);
EV DrawString(char *s);
EV Centers(rect *R,int *cx,int *cy);
EV JString(char *text,int x,int y,int fg,int bg,int a1,int a2);
EV _useEGApalette(unsigned char *p);
EV useEGApalette(void);
EV _useVGApalette(unsigned char *p);
EI Key(event *e);
EV PushCursorType(void);
EV PopCursorType(void);
EV LimitMouseRect(rect * R);
EV PushMouseRect(void);
EV PushMouseRectLimit(rect * R);
EV PopMouseRect(void);
#undef EI
#undef EV
#endif
