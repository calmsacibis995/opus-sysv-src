/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/isatty.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Returns 1 iff file is a tty
 */
#include "shlib.h"
#include <sys/termio.h>

extern int _ioctl();
extern int errno;

int
_isatty(f)
int	f;
{
	struct termio tty;
	int err ;

	err = errno;
	if(_ioctl(f, TCGETA, &tty) < 0)
	{
		errno = err; 
		return(0);
	}
	return(1);
}
