/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/strncat.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * At most n characters are moved.
 * Return s1.
 */
#if m88k
#include <stdio.h>
#else
#include <sys/types.h>
#endif

char *
_strncat(s1, s2, n)
register char *s1, *s2;
#if m88k
register unsigned int n;
#else
register size_t n;
#endif
{
	register char *os1;

	n++;
	os1 = s1;
	while(*s1++)
		;
	--s1;
	while(*s1++ = *s2++)
		if(--n == 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}
