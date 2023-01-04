
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*
 * 7/27/89 - Modified to set errno if file is write-only
 */

#ident	"@(#)libc-m88k:gen/getc.c"

#include <stdio.h>
/* POSIX */
#include <errno.h>

#undef _getc
int
_getc(p)
register FILE *p;
{

	if (!(p->_flag & (_IOREAD | _IORW))) {
		errno = EBADF;
		p->_flag |= _IOERR;
		return (EOF);
	}
	return(--(p)->_cnt < 0 ? _filbuf(p) : (int) *(p)->_ptr++);
}
