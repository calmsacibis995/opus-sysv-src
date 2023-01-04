/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/puts.c	3.8"
/*LINTLIBRARY*/
/*
 * This version writes directly to the buffer rather than looping on putc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
#include "shlib.h"
#include <stdio.h>
#include "stdiom.h"

#if __STDC__		/* __STDC__ defined by Ansi-C */
#define PUTS_RETURN(x) return(((x)==EOF)?EOF:0)		/* assumes EOF != 0 */
#else
#define PUTS_RETURN(x) return(x)
#endif

extern char *_memccpy();

int
_puts(ptr)
char *ptr;
{
	char *p;
	register int ndone = 0, n;
	register unsigned char *cptr, *bufend;

	if (_WRTCHK(stdout))
		PUTS_RETURN (EOF);

	bufend = _bufend(stdout);

	for ( ; ; ptr += n) {
		while ((n = bufend - (cptr = stdout->_ptr)) <= 0) /* full buf */
			if (_xflsbuf(stdout) == EOF)
				PUTS_RETURN(EOF);
		if ((p = _memccpy((char *) cptr, ptr, '\0', n)) != NULL)
			n = p - (char *) cptr;
		stdout->_cnt -= n;
		stdout->_ptr += n;
		_BUFSYNC(stdout);
		ndone += n;
		if (p != NULL) {
			stdout->_ptr[-1] = '\n'; /* overwrite '\0' with '\n' */
			if (stdout->_flag & (_IONBF | _IOLBF)) /* flush line */
				if (_xflsbuf(stdout) == EOF)
					PUTS_RETURN(EOF);
			PUTS_RETURN(ndone);
		}
	}
}
