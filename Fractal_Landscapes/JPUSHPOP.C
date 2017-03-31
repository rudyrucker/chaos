#include "forge.h"
#include <alloc.h>
#include <dos.h>
#include <ctype.h>
#include <io.h>

/* Push and pop rects. */

typedef struct sl
{
	rect far *R;
	image far *im;
	struct sl *prev;
	char *filename;
	int lines;
	int blocks;
} savelist;

savelist *savetail = NULL;

void PushRect(rect * R, int *err)
{
	/* We don't really use ERR, but its for compatibility */
	savelist *lastsave = savetail;
	long savesize = ImageSize(R);

	/* And we don't give a shit about size. */

	savetail = farcalloc(sizeof(savelist), 1L);
	savetail->prev = lastsave;

   safe_alloc = true;
	savetail->im = (image far *) farcalloc(savesize, 1L);
   safe_alloc = false;
	savetail->R = farmalloc(sizeof(rect));
	*(savetail->R) = *R;

	if (!savetail->im)
	{
		char tbuf[128];
		char tbuf2[128];
		FILE *fd;
		image *tmp;
		char *pathname = getenv("TMP");
		unsigned char drive;
		struct dfree dtable;
		long dfree;

		/* Find out if there is enough space left on the thing */
      if (pathname && access(pathname,0))
         pathname = NULL;
		if (pathname && pathname[1] == ':')
			drive = toupper(pathname[0]) - 'A' + 1;
		else
			drive = 0;

		getdfree(drive, &dtable);
		if (dtable.df_sclus == 0xffff && drive)
		{
			drive = 0;
			getdfree(drive, &dtable);
			pathname = NULL;
			/* and so this doesn't happen again */
			putenv("TMP=");
		}

		dfree = (long) dtable.df_avail * (long) dtable.df_bsec * (long) dtable.df_sclus;

		if (dfree > savesize)
		{
			rect R1;

			sprintf(tbuf, "%lx.dat", savetail);
			TempFileName(tbuf2, tbuf);
			fd = fopen(tbuf2, "wb");

			if (fd)
			{
				int i;
				unsigned long cl = farcoreleft();
				int lines;
				long n;


				R1 = *R;
				R1.Ymax = R1.Ymin;

				i = 1;
				while (i < R->Ymax)
				{
					R1.Ymax = R1.Ymin + i;
					n = ImageSize(&R1);
					if (n < cl)
					{
						savesize = n;
						lines = i;
					}
					i *= 2;
				}




				/*
				 * Let's see how many rows at a time we can
				 * save
				 */
				savetail->lines = lines;
				savetail->blocks = (R->Ymax - R->Ymin + 1) / lines;

				tmp = farmalloc(savesize);
				//setvbuf(fd, (char *) tmp, _IOFBF, (size_t) savesize);
				R1 = *R;
				R1.Ymax = R1.Ymin + lines - 1;
				for (i = R->Ymin; i <= R->Ymax; i += lines)
				{
					R1.Ymax = min(R1.Ymax, R->Ymax);
					ReadImage(&R1, tmp);
					fwrite(tmp, 1, (int) ImageSize(&R1), fd);
					OffsetRect(&R1, 0, lines);
				}
				fclose(fd);
				farfree(tmp);
				savetail->filename = strdup(tbuf2);
				*err = 0;
				return;
			}
		}

		sprintf(tbuf, "Can't allocate %ld bytes", savesize);
		ErrorBox(tbuf);
		*err = 1;
		farfree(savetail->R);
		farfree(savetail);
		savetail = lastsave;
	}
	else
	{
		ReadImage(savetail->R, savetail->im);
		*err = 0;
	}
}
static int putz = 0;
static void heapwalker(void)
{
	struct heapinfo hi;

	hi.ptr = NULL;
	while (heapwalk(&hi) == _HEAPOK)
		putz++;
}

void PopRect(int *err)
{
	*err = 1;
	if (savetail)
	{
		savelist *prev = savetail->prev;

		if (savetail->im)
		{
			WriteImage(savetail->R, savetail->im);
			if (putz)
				heapwalker();
			farfree(savetail->im);
			if (putz)
				heapwalker();

			*err = 0;
		}
		else
		{
			if (savetail->filename)
			{
				rect R = *(savetail->R);
				rect R1 = R;

				FILE *fd = fopen(savetail->filename, "rb");
				long savesize;
				image *tmp;
				int i;

				if (fd)
				{
					R1.Ymax = R1.Ymin;
					R1.Ymax = R1.Ymin + savetail->lines - 1;
					savesize = ImageSize(&R1);
					tmp = farmalloc(savesize);
					//setvbuf(fd, (char *) tmp, _IOFBF, (size_t) savesize);
					for (i = R.Ymin; i <= R.Ymax; i += savetail->lines)
					{
						R1.Ymax = min(R1.Ymax, R.Ymax);
						fread(tmp, 1, (int) ImageSize(&R1), fd);
						WriteImage(&R1, tmp);
						OffsetRect(&R1, 0, savetail->lines);
					}
					fclose(fd);
					farfree(tmp);
					*err = 0;
				}
				remove(savetail->filename);
				free(savetail->filename);

			}
		}

		farfree(savetail->R);
		farfree(savetail);
		savetail = prev;
	}
}
