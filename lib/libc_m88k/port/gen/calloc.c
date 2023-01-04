/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/calloc.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*	calloc - allocate and clear memory block
*/
#define NULL 0
#include "shlib.h"

extern char *_malloc(), *_memset();
extern void _free();

char *
_calloc(num, size)
unsigned num, size;
{
	register char *mp;

	if((mp = _malloc(num *= size)) != NULL)
		(void)_memset(mp, 0, num);
	return(mp);
}

/*ARGSUSED*/
void
_cfree(p, num, size)
char *p;
unsigned num, size;
{
	_free(p);
}
