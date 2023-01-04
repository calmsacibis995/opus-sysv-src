/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/tmpnam.c	1.8"
/*LINTLIBRARY*/
#include <stdio.h>

extern char *_mktemp(), *_strcpy(), *_strcat();
static char str[L_tmpnam], seed[] = { 'a', 'a', 'a', '\0' };

static char Xs[] = "XXXXXX";

char *
_tmpnam(s)
char	*s;
{
	register char *p, *q;

	p = (s == NULL)? str: s;
	(void) _strcpy(p, P_tmpdir);
	(void) _strcat(p, seed);
	(void) _strcat(p, Xs);

	q = seed;
	while(*q == 'z')
		*q++ = 'a';
	if (*q != '\0')
		++*q;

	(void) _mktemp(p);
	return(p);
}
