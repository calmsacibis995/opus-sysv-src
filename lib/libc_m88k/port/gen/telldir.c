/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/telldir.c	1.3"
/*
	telldir -- C library extension routine

*/

#include	<sys/types.h>
#include	<sys/stat.h>
#include 	<dirent.h>

extern long	_lseek();

long
_telldir( dirp )
DIR	*dirp;			/* stream from opendir() */
{
	struct dirent *dp;
	struct stat sbuf;

	/* posix: check if directory was closed, or rmdir'ed */
	if ((_fstat( dirp->dd_fd, &sbuf ) < 0) || 
		(sbuf.st_mode & S_IFMT) != S_IFDIR  ||
		(sbuf.st_nlink == 0))
		return -1;

	if (_lseek(dirp->dd_fd, 0, 1) == 0)	/* if at beginning of dir */
		return(0);			/* return 0 */
	dp = (struct dirent *)&dirp->dd_buf[dirp->dd_loc];
	return(dp->d_off);
}
