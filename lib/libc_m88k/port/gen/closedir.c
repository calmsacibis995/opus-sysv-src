/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/closedir.c	1.4"
/*
	closedir -- C library extension routine

*/

#include	<sys/types.h>
#include	<dirent.h>


extern void	_free();
extern int	_close();

int
_closedir( dirp )
register DIR	*dirp;		/* stream from opendir() */
{
	register int fd;

	_free( dirp->dd_buf );
	fd = dirp->dd_fd;
	_free( (char *)dirp );
	return(_close( fd ));
}
