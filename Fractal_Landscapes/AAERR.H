
#ifndef AAERR_H
#define AAERR_H

#define AA_SUCCESS 0			/* No Problem! */
#define AA_ERR_MISC -1			/* Unclassified error */
#define AA_ERR_NOMEM -2		/* Not enough memory */
#define AA_ERR_CANTFIND -3		/* File doesn't exist */
#define AA_ERR_CANTMAKE -4		/* Can't create file */
#define AA_ERR_BADTYPE -5		/* File is not the right type */
#define AA_ERR_BADDATA -6		/* File damaged internally */
#define AA_ERR_WRONGREZ -7		/* File isn't 320x200 */
#define AA_ERR_SHORTREAD -8    /* Not all of file is there */
#define AA_ERR_SHORTWRITE -9   /* Couldn't write all of file */
#define AA_ERR_SEEK	-10		/* Error during file seek */

typedef int Errval;	
char *fli_error_message(Errval err); /* return something to say about error */

#endif /* AAERR_H */
