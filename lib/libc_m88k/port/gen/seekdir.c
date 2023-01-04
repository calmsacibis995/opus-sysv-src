/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/seekdir.c	1.4"
/*
	seekdir -- C library extension routine

*/

#include	<sys/types.h>
#include	<dirent.h>

extern long	_lseek();

#define NULL	0

void
_seekdir(dirp, loc)
register DIR	*dirp;		/* stream from opendir() */
long		loc;		/* position from _telldir() */
{

	register struct dirent * dp;
	if (_telldir(dirp) == loc)
		return; 		/* save time */
	dirp->dd_loc = 0;
	_lseek(dirp->dd_fd, loc, 0);
	dirp->dd_size = 0;
}
