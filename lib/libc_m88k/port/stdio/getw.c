/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/getw.c	1.8"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * The intent here is to provide a means to make the order of
 * bytes in an io-stream correspond to the order of the bytes
 * in the memory while doing the io a `word' at a time.
 */
#include "shlib.h"
#include <stdio.h>

int
_getw(stream)
register FILE *stream;
{
	int w;
	register char *s = (char *)&w;
	register int i = sizeof(int);

	while (--i >= 0 && !feof(stream) && !ferror(stream))
		*s++ = getc(stream);
	return (feof(stream) || ferror(stream) ? EOF : w);
}
