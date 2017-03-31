/* usual.h */

#include "BGIMETA.h"


#if META
 #include "GRconst.h"
 #include "GRports.h"
 #include "GRextrn.h"
#endif

#if MicrosoftC
 #include "colors.h"

 #include <dos.h>
 #include <math.h>
 #include <stdarg.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include <io.h>
 #include <fcntl.h>
 #include <conio.h>
 #include <malloc.h>
#endif

#if TurboC
 #include <dos.h>
 #include <math.h>
 #include <stdarg.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include <io.h>
 #include <fcntl.h>

 #include <conio.h>
 #include <alloc.h>
#endif


#include "ISdefine.h"
#include "IStypedf.h"
#include "ISconst.h"
#include "ISextern.h"
#include "ISems.h"

#if MicrosoftC
 #define MK_FP(seg,ofs)  ((void far *) \
	  (((unsigned long)(seg) << 16) | (unsigned)(ofs)))

 #define farcoreleft() _freect(1)
#endif
