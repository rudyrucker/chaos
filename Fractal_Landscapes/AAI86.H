
#ifndef AAI86_H
#define AAI86_H

/* intel 8086 emulator... */
struct i86_byte_regs 
	{
	unsigned char al, ah, bl, bh, cl, ch, dl, dh;
	unsigned int si, di, ds, es;
	};
struct i86_word_regs
	{
	unsigned ax,bx,cx,dx;
	unsigned int si, di, ds, es;
	};
union i86_regs
	{
	struct i86_byte_regs b;
	struct i86_word_regs w;
	};

/* do software int */
i86_sysint(int interrupt, union i86_regs *s, union i86_regs *d); 


void i86_bcopy(void *s, void *d, int bytes);	 /* copy bytes */
void i86_wcopy(void *s, void *d, int words);	 /* copy 16-bit-words */
void i86_bzero(void *d, int bytes);	/* set some bytes to zero */
void i86_wzero(void *d, int words); /* set some 16-bit-words to zero */
int i86_bsame(void *d, int count);	/* how many bytes in a row the same ? */
int i86_wsame(void *d, int count);	/* how many 16-bit-words the same? */
int i86_bcontrast(void *s1, void *s2, int count); /* bytes til 1st match? */
int i86_wcontrast(void *s1,void *s2,int count); /* words til 1st match? */
int i86_bcompare(void *s1,void *s2,int count); /* how many bytes match */
int i86_wcompare(void *s1,void *s2,int count); /* how many words match */
void i86_even_copy(void *s, void *d, int bytes); /* copy even # of bytes */
void i86_exchange_bytes(char *s1, char *s2, int count); /* exchange buffers */
void i86_back_bcopy(char *s,char *d,int count); /* copy bytes backwards */
void *i86_norm_ptr(void *ptr);	/* fold offset into address */
void *i86_enorm_ptr(void *ptr);	/* fold offset into address and force even */
void *i86_make_ptr(int offset, int seg);	/* convince C it's a pointer... */
long i86_make_long(int loword, int hiword);	/* convince C it's a long */
long i86_ptr_to_long(void *ptr); /* convert segment:offset to linear address */
int i86_ptr_seg(void *ptr);		/* return segment portion of pointer */
int i86_ptr_offset(void *ptr);	/* return offset portion of pointer */

#endif /* AAI86_H */
