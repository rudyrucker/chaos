#ifndef _MAGH_
#define _MAGH_
#include "GRconst.h"
#include "GRports.h"
#include "GRextrn.h"
#include "buttons.h"
#include "scodes.h"

#define EV extern void
#define EI extern int
#define ER extern rect

EV PushRect(rect *,int *);
EV PopRect(int *);
EV Centers(rect *,int *,int *);
EV PushMouseRectLimit(rect *);
EV PopMouseRect(void);
EV LimitMouseRect(rect *);



EI FontHeight;
ER sR;
EI StringWidthX;
extern double aspect;
#endif
