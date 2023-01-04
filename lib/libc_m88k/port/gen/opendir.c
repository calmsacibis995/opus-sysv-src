/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/opendir.c	1.4"
/*
	opendir -- C library extension routine

*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<sys/errno.h>

extern char	*_malloc();
extern int	_open(), _close(), _fstat();
extern int	errno;

#define NULL	0

DIR *
_opendir( filename )
char		*filename;	/* name of directory */
{
	register DIR	*dirp;		/* -> malloc'ed storage */
	register int	fd;		/* file descriptor for read */
	struct stat	sbuf;		/* result of _fstat() */

	if ( (fd = _open( filename, 0 )) < 0 )
		return NULL;
	if ( (_fstat( fd, &sbuf ) < 0)
	  || ((sbuf.st_mode & S_IFMT) != S_IFDIR)
	  || ((dirp = (DIR *)_malloc( sizeof(DIR) )) == NULL)
	  || ((dirp->dd_buf = (char *)_malloc(DIRBUF)) == NULL)
	   )	{
		if ((sbuf.st_mode & S_IFMT) != S_IFDIR)
			errno = ENOTDIR;
		(void)_close( fd );
		return NULL;		/* bad luck today */
		}

	dirp->dd_fd = fd;
	dirp->dd_loc = dirp->dd_size = 0;	/* refill needed */

	return dirp;
}
