/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/fread.c	3.17"
/*LINTLIBRARY*/
/*
 * This version reads directly from the buffer rather than looping on getc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
/*
 * Modified to set errno in case the file is read-only (7/26/89)
 * as required by POSIX
 */
#include "shlib.h"
#include <stdio.h>
#include "stdiom.h"
#include <sys/types.h>
/* POSIX */
#include <errno.h>

#define MIN(x, y)	(x < y ? x : y)

extern int _filbuf();
extern _bufsync();
extern char *_memcpy();

int
_fread(ptr, size, count, iop)
char *ptr;
int count;
size_t size;
register FILE *iop;
{
	register unsigned long nleft;
	register int n;

	if (count <= 0) return 0;

	if (!(iop->_flag & (_IOREAD | _IORW))) {
		errno = EBADF;
		return (NULL);
	}
	nleft = (unsigned long) count * size; /* may overflow */
	if (nleft < count || nleft < size)	/* overflow occured */
		return (0);

	/* Put characters in the buffer */
	/* note that the meaning of n when just starting this loop is
	   irrelevant.  It is defined in the loop */
	for ( ; ; ) {
		if (iop->_cnt <= 0) { /* empty buffer */
			if (_filbuf(iop) == EOF)
				return (count - (nleft + size - 1)/size);
			iop->_ptr--;
			iop->_cnt++;
		}
		n = MIN(nleft, iop->_cnt);
		ptr = _memcpy(ptr, (char *) iop->_ptr, n) + n;
		iop->_cnt -= n;
		iop->_ptr += n;
		_BUFSYNC(iop);
		if ((nleft -= n) <= 0)
			return (count);
	}
}
