/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/memchr.c	1.3"
/*LINTLIBRARY*/
/*
 * Return the ptr in sp at which the character c appears;
 *   NULL if not found in n chars; don't stop at \0.
 */
char *
_memchr(sp, c, n)
register char *sp, c;
register int n;
{
	while (--n >= 0)
		if (*sp++ == c)
			return (--sp);
	return (0);
}
