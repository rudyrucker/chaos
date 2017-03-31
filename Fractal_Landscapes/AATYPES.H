
#ifndef AATYPES_H
#define AATYPES_H

#ifndef NULL
#define NULL ((void *)0L)
#endif /* NULL */

void *aa_malloc(unsigned);	/* heap allocator */
void aa_free(void *);		/* heap free'er */

#ifndef UBYTE
#define UBYTE unsigned char
#endif /* UBYTE */
#ifndef BYTE
#define BYTE signed char
#endif /* BYTE */
#ifndef SHORT
#define SHORT short
#endif /* SHORT */
#ifndef SHORT
#define SHORT short
#endif /* SHORT */
#ifndef USHORT 
#define USHORT unsigned short
#endif /* USHORT */
#ifndef LONG
#define LONG long
#endif /* LONG */
#ifndef ULONG
#define ULONG unsigned long
#endif /* ULONG */

typedef void (*AAvec)();	/* pointer to a void-valued function */
typedef int (*AAivec)();	/* pointer to an integer valued function */

typedef int Boolean;	/* functions explicitly returning 0 or 1 */
#define TRUE 1
#define FALSE 0

/* useful macro to count elements in an array */
#define Array_els(arr) (sizeof(arr)/sizeof(arr[0]))


#endif /* AATYPES_H */
