/* Convert the image on the screen to a GIF file. This should prove
interesting! */

#include "gif.h"
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <string.h>
#include <alloc.h>

#include "mag.h"

struct gif_header gif;
struct gif_image gim;
FILE *giffd;
unsigned char conversion_table[64];
int OurMode;

extern int maxx;
extern int minx;
extern int maxy;
extern int Frozen;
char *ega_lut=(char *)egacolortable;
char *vga_lut=(char *)vgacolortable;
extern int mode;
extern void egavgapixel(int, int, unsigned char);
extern int gif_compress_data(int, int);

static int gifcol, gifrow;

static unsigned char *rowbuf;
static int rowbufptr;


int gif_get_byte(void)
{
	if (rowbufptr > 511)
	{
		fread(rowbuf, 512, 1, giffd);
		rowbufptr = 0;
	}
	return rowbuf[rowbufptr++];

}

int gif_out_line(unsigned char far * pixels, int len)
{
   extern void rowblast(unsigned char far *,int,int,int);

   if ((OurMode == 0x13 && gifrow >= 200) ||
       (OurMode == 0x12 && gifrow >= 480) ||
       (OurMode == 0x10 && gifrow >= 350))
	 return -1;


   if (OurMode == 0x13)
      len = min(len,320 - gim.x);
   else
      len = min(len,640 - gim.x);

   if (OurMode != 0x13)
   	rowblast(pixels, gim.x, gifrow + gim.y, len);
   else
   {
      char *screen = MK_FP(0xa000,(gifrow+gim.y)*320+gim.x);
      memcpy(screen,pixels,len);
   }

 	gifrow++;
	return 0;
}

unsigned char GIFcmap[256 * 3];
unsigned char GIFctab[17];

int GifDisplay(char *filename)
{
	int i, j;
	int c;
	int colors;
	int ret = GIF_COOLMAN;
	union REGS regs;
	struct SREGS sregs;

	if (!memok(16133L))
	{			/* Added mallocs in decoder.c  */
		ErrorBox("Not enough memory to view a Gif file.");
		return OUT_OF_MEMORY;
	}

	giffd = fopen(filename, "rb");
	if (!giffd)
		return -1;
	gifrow = 0;
	gifcol = 0;
	safe_alloc = 1;
	if ((rowbuf = malloc(512)) == NULL)
	{
		ret = OUT_OF_MEMORY;
		goto TRUNCOUT;
	}
	rowbufptr = 512;	/* to get it started */

	fread(&gif, 1, sizeof(gif), giffd);
	colors = (1 << ((gif.colpix & PIXMASK) + 1));
   if (colors > 16 && hasVGA)
   {
      regs.h.ah = 0;
      regs.h.al = OurMode = 0x13;
      int86(0x10,&regs,&regs);
   }
   else
   {
      if (gif.h > 350 && hasVGA)
	 OurMode = 0x12;
      else
	 OurMode = 0x10;

      regs.h.ah = 0;
      regs.h.al = OurMode;
      int86(0x10,&regs,&regs);
   }
	if (gif.colpix & COLTAB)
	{
		fread(GIFcmap, 1, colors * 3, giffd);
		/* Now set up the colormap if appropriate */
		for (i = 0; i < colors * 3; i++)
			GIFcmap[i] >>= 2;
		if (hasVGA)
		{
			/*
			 * This is a VGA. Just write the map, shifted
			 * appropriately.
			 */


			regs.h.ah = 0x10;
			regs.h.al = 0x12;
			regs.x.bx = 0;
			regs.x.cx = colors;
			regs.x.dx = FP_OFF(GIFcmap);
			sregs.es = FP_SEG(GIFcmap);
			int86x(0x10, &regs, &regs, &sregs);

         /* We'll also want to set the 0-16 map here? */
	 for(i=0;i<16;i++)
	    GIFctab[i] = i;
	 GIFctab[16] = 0;
			regs.h.ah = 0x10;
			regs.h.al = 0x2;
			regs.x.dx = FP_OFF(GIFctab);
			sregs.es = FP_SEG(GIFctab);
			int86x(0x10, &regs, &regs, &sregs);

		}
		else
		{

			for (i = 0; i < 16; i++)
			{
				for (j = 0; j < 64; j++)
				{
					unsigned char *p = defaultpalette[j];

					if (p[0] == GIFcmap[i * 3] &&
					    p[1] == GIFcmap[i * 3 + 1] &&
					    p[2] == GIFcmap[i * 3 + 2])
						break;
				}
				GIFctab[i] = j;
			}
			GIFctab[i] = 0;
			regs.h.ah = 0x10;
			regs.h.al = 0x2;
			regs.x.dx = FP_OFF(GIFctab);
			sregs.es = FP_SEG(GIFctab);
			int86x(0x10, &regs, &regs, &sregs);

		}
	}
	for (;;)		/* skip over extension blocks and other junk
				 * til get ',' */
	{
		if ((c = fgetc(giffd)) == EOF)
		{
			ret = BAD_FILE;
			goto TRUNCOUT;
		}
		if (c == ',')
			break;
		if (c == ';')   /* semi-colon is end of piccie */
		{
			ret = BAD_FILE;
			goto TRUNCOUT;
		}
		if (c == '!')   /* extension block */
		{
			if ((c = fgetc(giffd)) == EOF)	/* skip extension type */
			{
				ret = BAD_FILE;
				goto TRUNCOUT;
			}
			for (;;)
			{
				if ((c = fgetc(giffd)) == EOF)
				{
					ret = BAD_FILE;
					goto TRUNCOUT;
				}
				if (c == 0)     /* zero 'count' means end of
						 * extension */
					break;
				while (--c >= 0)
				{
					if (fgetc(giffd) == EOF)
					{
						ret = BAD_FILE;
						goto TRUNCOUT;
					}
				}
			}
		}
	}
	fread(&gim, 1, sizeof(gim), giffd);
	if (gim.flags & COLTAB)
	{
		colors = (1 << ((gim.flags & PIXMASK) + 1));
		fread(&GIFcmap, 1, colors * 3, giffd);
	}
	ret = gif_decoder(gim.w);
TRUNCOUT:
	fclose(giffd);
	if (rowbuf)
		free(rowbuf);
	return ret;
}




int GifOutput(char *filename, int wholescreen)
{
	int i, j;
   extern void clearGIFlines(int);
	if (!memok(20712L))
	{			/* Added up mallocs in comprs.c */
		ErrorBox("Not enough memory to save a Gif file.");
		return 0;
	}

	memset(&gif, 0, sizeof gif);
   HideCursor();
	giffd = fopen(filename, "wb");
	if (!giffd)
	{
      FileError(filename,NULL);
      ShowCursor();
		return 0;
	}
	strcpy(gif.giftype, "GIF87a");
	gif.w = gim.w = (wholescreen) ? 640 : 640 - minx;
	gif.h = gim.h = (mode == 0x10) ? 350 : 480;
	gim.y = 0;
	if (wholescreen)
		gim.x = 0;
	else
		gim.x = 80;
	gim.flags = 0;
	gif.colpix = 128 + (7 << 4) + 3;

	fwrite(&gif, sizeof gif, 1, giffd);
	/* ERROR? */

	/*
	 * Write the color map. For right now, just write the standard color
	 * map; we'll fix it in a few minutes.
	 */

	/*
	 * Build the conversion table... this will go from 0..63 to 0..255,
	 * since << 2 is just WRONG.
	 */

	for (i = 0; i < 64; i++)
		conversion_table[i] = (unsigned char) (((float) i) / 63.0 * 255.0);

	for (i = 0; i < 16; i++)
		for (j = 0; j < 3; j++)
		{
			if (mode == 0x10)
				putc(conversion_table[defaultpalette[ega_lut[i]][j]], giffd);
			else
				putc(conversion_table[vga_lut[i * 3 + j]], giffd);
		}

	putc(',', giffd);
	fwrite(&gim, sizeof gim, 1, giffd);
	putc(8, giffd);

	/* Now we crunch the data. */
	i = gif_compress_data(8, wholescreen);
   clearGIFlines(wholescreen);
 

	putc(';', giffd);
   if (i == -3 || ferror(giffd))
   {
      FileError(filename,giffd);
      fclose(giffd);
      remove(filename);
   }
   else
   	fclose(giffd);
   ShowCursor();
	return 1;

}
