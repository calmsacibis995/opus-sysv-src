/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/gets.c	3.7"
/*LINTLIBRARY*/
/*
 * This version reads directly from the buffer rather than looping on getc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
#include "shlib.h"
#include <stdio.h>
#include "stdiom.h"
/* 7/27/89
 * Modified to check if 'stdin' is open for reading
 * This is a POSIX requirement
 */
#include <errno.h>

extern int _filbuf();
extern _bufsync();
extern char *_memccpy();

char *
_gets(ptr)
char *ptr;
{
	char *p, *ptr0 = ptr;
	register int n;

	if (!( stdin->_flag & (_IOREAD | _IORW))) {
		errno = EBADF;
		return ((char *) NULL);
	}

	for ( ; ; ) {
		if (stdin->_cnt <= 0) { /* empty buffer */
			if (_filbuf(stdin) == EOF) {
				if (ptr0 == ptr)
					return (NULL);
				break; /* no more data */
			}
			stdin->_ptr--;
			stdin->_cnt++;
		}
		n = stdin->_cnt;
		if ((p = _memccpy(ptr, (char *) stdin->_ptr, '\n', n)) != NULL)
			n = p - ptr;
		ptr += n;
		stdin->_cnt -= n;
		stdin->_ptr += n;
		_BUFSYNC(stdin);
		if (p != NULL) { /* found '\n' in buffer */
			ptr--; /* step back over '\n' */
			break;
		}
	}
	*ptr = '\0';
	return (ptr0);
}
