/*----------------------------------------------------------------------*/
/* Copyright (c) 1987							 */
/* by CompuServe Inc., Columbus, Ohio.  All Rights Reserved		 */
/*----------------------------------------------------------------------*/

/*
 * ABSTRACT: The compression algorithm builds a string translation table that
 * maps substrings from the input string into fixed-length codes.  These
 * codes are used by the expansion algorithm to rebuild the compressor's
 * table and reconstruct the original data stream.  In it's simplest form,
 * the algorithm can be stated as:
 *
 * "if <w>k is in the table, then <w> is in the table"
 *
 * <w> is a code which represents a string in the table.  When a new character k
 * is read in, the table is searched for <w>k.  If this combination is found,
 * <w> is set to the code for that combination and the next character is read
 * in.  Otherwise, this combination is added to the table, the code <w> is
 * written to the output stream and <w> is set to k.
 *
 * The expansion algorithm builds an identical table by parsing each received
 * code into a prefix string and suffix character.  The suffix character is
 * pushed onto the stack and the prefix string translated again until it is a
 * single character.  This completes the expansion. The expanded code is then
 * output by popping the stack and a new entry is made in the table.
 *
 * The algorithm used here has one additional feature.  The output codes are
 * variable length.  They start at a specified number of bits.  Once the
 * number of codes exceeds the current code size, the number of bits in the
 * code is incremented.  When the table is completely full, a clear code is
 * transmitted for the expander and the table is reset. This program uses a
 * maximum code size of 12 bits for a total of 4096 codes.
 *
 * The expander realizes that the code size is changing when it's table size
 * reaches the maximum for the current code size.  At this point, the code
 * size in increased.  Remember that the expander's table is identical to the
 * compressor's table at any point in the original data stream.
 *
 * The compressed data stream is structured as follows: first byte denoting the
 * minimum code size one or more counted byte strings. The first byte
 * contains the length of the string. A null string denotes "end of data"
 *
 * This format permits a compressed data stream to be embedded within a
 * non-compressed context.
 *
 * AUTHOR: Steve Wilhite
 *
 * REVISION HISTORY: Speed tweaked a bit by Jim Kent 8/29/88
 *
 */

#include <setjmp.h>
#include "toy.h"

#define LARGEST_CODE	4095
#define TABLE_SIZE	(4*1024)


unsigned char *gif_byte_buff;
extern FILE *giffd;
extern int safe_alloc;

unsigned char *gif_wpt;
long gif_wcount;

jmp_buf recover;

static short *prior_codes;
static short *code_ids;
static unsigned char *added_chars;

static short code_size;
static short clear_code;
static short eof_code;

static short bit_offset;
static short max_code;
static short free_code;

static void init_table(short min_code_size)
{

	code_size = min_code_size + 1;
	clear_code = 1 << min_code_size;
	eof_code = clear_code + 1;
	free_code = clear_code + 2;
	max_code = 1 << code_size;


	/* zero_words(code_ids, TABLE_SIZE); */
	memset(code_ids, 0, TABLE_SIZE * 2);

}


static void flush(short n)
{

	if (putc(n, giffd) == EOF)
		longjmp(recover, -3);
	if (fwrite(gif_byte_buff, 1, n, giffd) < n)
		longjmp(recover, -3);
}


static void write_code(short code)
{
	long temp;
	register short byte_offset;
	register short bits_left;

	byte_offset = bit_offset >> 3;
	bits_left = bit_offset & 7;

	if (byte_offset >= 254)
	{
		flush(byte_offset);
		gif_byte_buff[0] = gif_byte_buff[byte_offset];
		bit_offset = bits_left;
		byte_offset = 0;
	}

	if (bits_left > 0)
	{
		temp = ((long) code << bits_left) | gif_byte_buff[byte_offset];
		gif_byte_buff[byte_offset] = temp;
		gif_byte_buff[byte_offset + 1] = temp >> 8;
		gif_byte_buff[byte_offset + 2] = temp >> 16;
	}
	else
	{
		gif_byte_buff[byte_offset] = code;
		gif_byte_buff[byte_offset + 1] = code >> 8;
	}
	bit_offset += code_size;
}

int gifrow, gifcol;
unsigned GifEOF;
static int lastrowblasted;
void clearGIFlines(int wholescreen)
{
	int mymin = (wholescreen) ? 0 : 160;
   rect R;

	RasterOp(zXORz);
	PenColor(WHITE);
	SetRect(&R, mymin, 0, mymin + 1, lastrowblasted);
	PaintRect(&R);
	SetRect(&R, sR.Xmax - 1, 0, sR.Xmax, lastrowblasted);
	PaintRect(&R);
	RasterOp(zREPz);
}

unsigned char GetGifPixel(int wholescreen)
{
	int mymin = (wholescreen) ? 0 : 160;
	unsigned char ch = GetPixel(gifcol + mymin, gifrow);

	gifcol++;
	if (gifcol + mymin > sR.Xmax)
	{
		gifcol = 0;
		gifrow++;
		if (gifrow > sR.Ymax)
		{
			GifEOF = true;
		}
		else
		{
			/* XOR a couple of pixels on each side */
			RasterOp(zXORz);
			PenColor(WHITE);
			SetPixel(mymin, gifrow - 1);
			SetPixel(mymin + 1, gifrow - 1);
			SetPixel(sR.Xmax - 1, gifrow - 1);
			SetPixel(sR.Xmax, gifrow - 1);
         lastrowblasted = gifrow-1;
		}
	}


	return ch;
}






/*
 * Function: Compress a stream of data bytes using the LZW algorithm.
 *
 * Inputs: min_code_size the field size of an input value.  Should be in the
 * range from 1 to 9.
 *
 * Returns: 0	normal completion -1	(not used) -2	insufficient dynamic
 * memory -3	bad "min_code_size" < -3	error status from either the
 * get_byte or put_byte routine
 */
static short compress_data(int min_code_size, int wholescreen)
{
	short status;
	short prefix_code;
	short d;
	register int hx;
	register short suffix_char;



	if (min_code_size < 2 || min_code_size > 9)
		if (min_code_size == 1)
			min_code_size = 2;
		else
			return -3;


	status = setjmp(recover);

	if (status != 0)
	{
		return status;
	}

	bit_offset = 0;
	init_table(min_code_size);
	write_code(clear_code);
	suffix_char = GetGifPixel(wholescreen);
	gif_wcount -= 1;

	prefix_code = suffix_char;

	while (!GifEOF)
	{
		suffix_char = GetGifPixel(wholescreen);
#pragma warn -amb		/* avoid stupid warning message */
		hx = prefix_code ^ suffix_char << 5;
#pragma warn .amb
		d = 1;

		for (;;)
		{
			if (code_ids[hx] == 0)
			{
				write_code(prefix_code);

				d = free_code;

				if (free_code <= LARGEST_CODE)
				{
					prior_codes[hx] = prefix_code;
					added_chars[hx] = suffix_char;
					code_ids[hx] = free_code;
					free_code++;
				}

				if (d == max_code)
					if (code_size < 12)
					{
						code_size++;
						max_code <<= 1;
					}
					else
					{
						write_code(clear_code);
						init_table(min_code_size);
					}

				prefix_code = suffix_char;
				break;
			}

			if (prior_codes[hx] == prefix_code &&
			    added_chars[hx] == suffix_char)
			{
				prefix_code = code_ids[hx];
				break;
			}

			hx += d;
			d += 2;
			if (hx >= TABLE_SIZE)
				hx -= TABLE_SIZE;
		}
	}

	write_code(prefix_code);

	write_code(eof_code);


	/* Make sure the code buffer is flushed */

	if (bit_offset > 0)
		flush((bit_offset + 7) / 8);

	flush(0);		/* end-of-data */
	return 0;
}


#pragma warn -use










short gif_compress_data(int min_code_size, int wholescreen)
{
	int ret;
	long needed;
   char tbuf[128];

	ret = -2;		/* out of memory default */
   gif_byte_buff = NULL;
   added_chars = NULL;
   prior_codes = NULL;
   code_ids = NULL;

   needed = 256 + 3L;
   safe_alloc = 1;
   if (NULL == (gif_byte_buff = farmalloc(needed)))
      goto OUT;

	needed = (long) TABLE_SIZE *sizeof(short);

 	safe_alloc = 1;
	if (NULL == (prior_codes = farmalloc(needed)))
		goto OUT;

 	safe_alloc = 1;
   if (NULL == (code_ids = farmalloc(needed)))
		goto OUT;

   safe_alloc = 1;
	needed = (long) TABLE_SIZE;
	if (NULL == (added_chars = farmalloc(needed)))
		goto OUT;

	GifEOF = false;
	gifrow = 0;
	gifcol = 0;
	ret = compress_data(min_code_size, wholescreen);
   goto FREE;

OUT:
   sprintf(tbuf,"Needed %l bytes, but only %l avail\n", needed, realfarcoreleft());
   ErrorBox(tbuf);
FREE:
	if (prior_codes)
		farfree(prior_codes);
	if (code_ids)
		farfree(code_ids);
	if (added_chars)
		farfree(added_chars);
   if (gif_byte_buff)
      farfree(gif_byte_buff);
	return (ret);
}
