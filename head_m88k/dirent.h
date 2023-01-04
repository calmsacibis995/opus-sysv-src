#ifndef _DIRENT_H_
#define _DIRENT_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#define MAXNAMLEN	512		/* maximum filename length */
#include <sys/dirent.h>

/* 
 * The DIR struct is not defined by the 88000 OCS but is required 
 * for compilation.
 * Do NOT depend on or use the elements of this structure for an OCS
 * compliant program.
 */
typedef struct
	{
	int	dd_fd;			/* file descriptor */
	int	dd_loc;			/* offset in block */
	int	dd_size;		/* amount of valid data */
	char	*dd_buf;		/* directory block */
	}	DIR;			/* stream data from opendir() */

#ifndef __88000_OCS_DEFINED
#define DIRBUF		1048		/* buffer size for fs-indep. dirs */

#define rewinddir( dirp )	seekdir( dirp, 0L )
extern DIR		*opendir();
extern struct dirent	*readdir();
extern long		telldir();
extern void		seekdir();
extern int		closedir();
#endif /* !__88000_OCS_DEFINED */


#endif	/* ! _DIRENT_H_ */
