/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/rewinddir.c	1.4"
/*
	seekdir -- C library extension routine

*/

#include	<sys/types.h>
#include	<dirent.h>

extern long	_lseek();

#define NULL	0

#undef _rewinddir
void
_rewinddir(dirp)
register DIR	*dirp;		/* stream from opendir() */
{

	register struct dirent * dp;
	if (_telldir(dirp) == 0)
		return; 		/* save time */
	dirp->dd_loc = 0;
	_lseek(dirp->dd_fd, 0, 0);
	dirp->dd_size = 0;
}
