/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sccs:lib/mpwlib/cat.c	6.2"
/*
	Concatenate strings.
 
	cat(destination,source1,source2,...,sourcen,0);
 
	returns destination.
*/
#if !clipper
#include "varargs.h"
#endif

#if clipper
char *cat(dest,source, source2)
char *dest, *source, *source2;
#else
char *cat(dest,source, va_alist)
char *dest, *source;
va_dcl
#endif
{
#if !clipper
/* half hearted varargs: works with pcc (m88k & ns32000) and dcc */
	va_list ap;
#endif
	register char *d, *s, **sp;

	d = dest;
/* This was added because of a varargs problem with clipper */
#if clipper
	if ((s = source) == (char *)0) {
		return(dest);
	}
	while (*d++ = *s++) ;
	d--;
	for (sp = &source2; s = *sp; sp++) {
		while (*d++ = *s++) ;
		d--;
	}
#else
	for (sp = &source; s = *sp; sp++) {
		while (*d++ = *s++) ;
		d--;
	}
#endif
	return(dest);
}
