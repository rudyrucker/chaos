#ifndef AADOS_H
#define AADOS_H

#ifndef AATYPES_H
#include "aatypes.h"
#endif /* AATYPES_H */

typedef int Jfile;

dos_key_is(void);  /* return 1 if a key is ready to read, 0 if not */
dos_key_in(void);  /* return next keyboard input.  Wait for it. */	

Jfile dos_open(char *title, int mode); /* open a file that already exists.*/
/* defines for mode parameter to dos_open  */
#define DOS_READ_ONLY 0
#define DOS_WRITE_ONLY 1
#define DOS_READ_WRITE 2

Jfile dos_create(char *title);	/* Create a new read/write file */
void dos_close(Jfile f);		/* close file */

/* File read/write.  Normally don't use dos_rw, but go through macros */
long dos_rw(Jfile f,void *buf,long size,int ah);
#define dos_read(f,b,size) dos_rw(f,b,size,0x3f)
#define dos_write(f,b,size) dos_rw(f,b,size,0x40)

long dos_seek (Jfile f, long offset, int mode);
long dos_tell (Jfile f);
/* defines for mode parameter to dos_seek  */
#define DOS_SEEK_START	0
#define DOS_SEEK_RELATIVE	1
#define DOS_SEEK_END	2


/* size of a buffer long enough to hold any MS-DOS path */
#define DOS_PATH_SIZE 80

/* this is the data structure used by dos_first() dos_next () in searching
   directories */
typedef struct fndata 
	{
	char reserved[21];
	char attribute;
	USHORT time, date;
	long size;
	char name[13];
	char fordos[128-43];
	} Fndata;

Fndata *dos_get_dta (void); /* get the 'DTA' area for directory search */
Boolean dos_first(char *pattern, int attributes);
/* defines for attributes parameters */
#define DOS_ATTR_DIR	16
#define DOS_ATTR_NORMAL  0
Boolean dos_next (void);


/* A list of dos_devices that are actually connected, 
	made by dos_get_devlist () */
extern char dos_devices[26];
extern int dos_dev_count;
void dos_get_devlist (void);

/* This gets rid of the ms-dos abort/retry/fail message.  Afterwards
   ms-dos acts like it was always a fail.  dos_get_devlist () calls this... */
void dos_no_abort_retry_cancel (void); 

/* Hey dos - I want to go to this directory.  Actually this changes
   both device and directory at once.  eg name could be
   		C:\VPAINT\FISHIES  */
Boolean dos_change_dir(char *name);

/* Tell DOS it's time to go to another drive mon.  0 = A:, 1 = B: ...
   you get the idea */
Boolean dos_change_dev(int newdev);

/* Hey DOS, what drive is we on? 0 = A: */
int dos_get_dev (void);

/* Get the current device and directory into path */
Boolean dos_get_dir(char path[DOS_PATH_SIZE]);

/* Create a directory.  (Don't include device info.  If needed
   do a dos_change_dev(dev) first.) */
Boolean dos_make_dir(char *name);

/* Figure how much free space is on a device.  0 = current device. 1 = A:... */
long dos_dfree(int dev);

#endif /* AADOS_H */
