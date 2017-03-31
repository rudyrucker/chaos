#define _ALLOCC_
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include <setjmp.h>
#include <dir.h>
#include "game.h"

#define SAFETY_CUSH 13000L;	/* Memory meter safety margin. */

/* Smart allocates etc. */

/*  Globals exported  */

int safe_alloc = 0;		/* return NULL on fail if this flag set */
long memused = 0L;		/* Total memory now allocated */
long memhigh = 0L;		/* Memory high water mark */

void *j_calloc(size_t __nitems, size_t __size)
{
	unsigned long size;
	void *t;
	unsigned char huge *pt;

	if ((t = calloc(__nitems, __size)) != NULL)
	{
		pt = t;
		size = *((unsigned short *) (pt - 4));	/* Get size from header */
		size <<= 4;

		memused += size;
		if (memused > memhigh)
			memhigh = memused;

#ifdef AXTDEBUG
		printf("\ncalloc: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

	}
	else
	{
		if (safe_alloc)
		{
			safe_alloc = 0;
			return NULL;
		}
		fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit. \n\
%lu more bytes were requested.",
			 __nitems * __size);

		if (prog_init)
		{
			StopEvent();
			if (CommPort)
			    StopMouse();
		}

		exit(-1);
	}
	safe_alloc = 0;
	return t;
}

void j_free(void *__block)
{
	unsigned long size;
	unsigned char huge *cp = __block;
	unsigned long t_farcore = 0L;

	assert(__block != NULL);/* Better not free a null buffer */
	size = *((unsigned short *) (cp - 4));	/* Size in header */
	size <<= 4;
	memused -= size;
	t_farcore = farcoreleft();
	free(__block);
	if (farcoreleft() > t_farcore)
		memhigh -= (farcoreleft() - t_farcore); /* Freed high block */
	if (memhigh < 0L)		  /* Fragmentation can cause this to
					   * go below zero.  */
		memhigh = 0L;

#ifdef AXTDEBUG
	printf("\nfree: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

}

void *j_malloc(size_t __size)
{
	unsigned long size;
	void *t;
	unsigned char huge *pt;

	if ((t = malloc(__size)) != NULL)
	{
		pt = t;
		size = *((unsigned short *) (pt - 4));	/* Get buffer size from
							 * header */
		size <<= 4;

		memused += size;
		if (memused > memhigh)
			memhigh = memused;

#ifdef AXTDEBUG
		printf("\nmalloc: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

	}
	else
	{
		if (safe_alloc)
		{
			safe_alloc = 0;
			return NULL;
		}

		fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit. \n\
%lu more bytes were requested.",
			 __size);

		if (prog_init)
		{
			StopEvent();
			if (CommPort)
			    StopMouse();
		}

		exit(-1);
	}
	safe_alloc = 0;
	return t;
}

char *j_strdup(const char *s)
{
	unsigned long size;
	char *t;
	char huge *pt;

	if ((t = strdup(s)) != NULL)
	{
		pt = t;
		size = *((unsigned short *) (pt - 4));	/* Get buffer size from
							 * header */
		size <<= 4;

		memused += size;
		if (memused > memhigh)
			memhigh = memused;

#ifdef AXTDEBUG
		printf("\nstrdup: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

	}
	else
	{
		if (safe_alloc)
		{
			safe_alloc = 0;
			return NULL;
		}

		fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit.");


		if (prog_init)
		{
			StopEvent();
			if (CommPort)
			    StopMouse();
		}

		exit(-1);
	}
	safe_alloc = 0;
	return t;
}

char *j_getcwd(char *_buf, int _buflen)
{
	unsigned long size;
	char *t;
	char huge *pt;

	if (_buf != NULL)
		t = getcwd(_buf, _buflen);
	else
	{

    		if ((t = getcwd(_buf, _buflen)) != NULL)
		{
			/* Get buffer size from header */
			pt = t;
			size = *((unsigned short *) (pt - 4));

			size <<= 4;

			memused += size;
			if (memused > memhigh)
				memhigh = memused;

#ifdef AXTDEBUG
		        printf("\ngetcwd: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

		}
		else
		{
			if (safe_alloc)
			{
				safe_alloc = 0;
				return NULL;
			}

			fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit.");


			if (prog_init)
			{
				StopEvent();
				if (CommPort)
				    StopMouse();
			}

			exit(-1);
		}
	}
	safe_alloc = 0;
	return t;
}

void far *j_farcalloc(unsigned long __nunits, unsigned long __unitsz)
{
	unsigned long size;
	void *t;
	unsigned char huge *pt;

	if ((t = farcalloc(__nunits, __unitsz)) != NULL)
	{
		pt = t;
		size = *((unsigned short *) (pt - 4));	/* Get size from header */
		size <<= 4;

		memused += size;
		if (memused > memhigh)
			memhigh = memused;

#ifdef AXTDEBUG
		printf("\nfarcalloc: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

	}
	else
	{
		if (safe_alloc)
		{
			safe_alloc = 0;
			return NULL;
		}

		fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit. \n\
%lu more bytes were requested.",
			 __nunits * __unitsz);

		if (prog_init)
		{
			StopEvent();
			if (CommPort)
			    StopMouse();
		}

		exit(-1);
	}
	safe_alloc = 0;
	return t;
}

void j_farfree(void far * __block)
{
	unsigned long size;
	unsigned char huge *cp = __block;
	unsigned long t_farcore = 0L;

	assert(__block != NULL);/* Better not free a null buffer */
	size = *((unsigned short *) (cp - 4));
	size <<= 4;
	memused -= size;
	t_farcore = farcoreleft();
	farfree(__block);
	if (farcoreleft() > t_farcore)
		memhigh -= (farcoreleft() - t_farcore); /* Freed high block */
	if (memhigh < 0L)		  /* Fragmentation can cause this to
					   * go below zero.  */
		memhigh = 0L;

#ifdef AXTDEBUG
	printf("\nfarfree: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

}

void far *j_farmalloc(unsigned long __nbytes)
{
	unsigned long size;
	void *t;
	unsigned char huge *pt;

	if ((t = farmalloc(__nbytes)) != NULL)
	{
		pt = t;
		size = *((unsigned short *) (pt - 4));
		size <<= 4;
		memused += size;
		if (memused > memhigh)
			memhigh = memused;

#ifdef AXTDEBUG
		printf("\nfarmalloc: size = %lu, used = %ld, \
high = %ld, farcoreleft() = %lu", size, memused, memhigh, farcoreleft());
#endif

	}
	else
	{
		if (safe_alloc)
		{
			safe_alloc = 0;
			return NULL;
		}

		fprintf
			(stderr,
			 "\nThe system memory capacity has been \n\
exceeded. GAMES must exit. \n\
%lu more bytes were requested.",
			 __nbytes);

		if (prog_init)
		{
			StopEvent();
			if (CommPort)
			    StopMouse();
		}

		exit(-1);
	}
	safe_alloc = 0;
	return t;
}

/*  MEMOK  --  Determines if memory in use exceeds safety margin.  If so,
	       returns FALSE.  If free memory is adequate, returns TRUE. */

int memok(long __nbytes)
{
	short fcp;
	long tot_est_mem = farcoreleft() + memhigh - SAFETY_CUSH;

	fcp = (100L * (__nbytes + memused)) / tot_est_mem;

#ifdef AXTDEBUG
	printf("\nmemok fcp = %d, size = %lu, used = %ld, \
high = %ld, fcorelft() = %lu", fcp, __nbytes, memused, memhigh, farcoreleft());
#endif

	return (fcp < 100 && fcp >= 0);
}
