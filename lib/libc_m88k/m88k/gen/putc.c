
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*
 * Modified to set errno in case the file is readonly (7/27/89)
 */

#ident	"@(#)libc-m88k:gen/putc.c"

#include <stdio.h>
/* POSIX */
#include <errno.h>

extern unsigned char _ctype[];

#undef _putc
int
_putc(x, p)
register FILE *p;
{

	if (!(p->_flag & (_IOWRT | _IORW))) {
		errno = EBADF;
		return (EOF);
	}
	(--(p)->_cnt < 0 ? 
			_flsbuf((unsigned char) (x), (p)) : 
			(int) (*(p)->_ptr++ = (unsigned char) (x)));
}
