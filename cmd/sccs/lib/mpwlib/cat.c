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

#include <varargs.h>
#define	MAXARGS	128

char *cat(va_alist)
va_dcl
{
	register char *d, *s;
	va_list ap;
	char *args[MAXARGS];
	char *dest;
	int argno=0;

	va_start(ap);
	dest=va_arg(ap, char *);
		d = dest;
	while((args[argno++]=va_arg(ap, char *)) != (char *) 0 )
		;

	for (argno=0;s=args[argno];argno++) {
		while (*d++ = *s++) ;
		d--;
	}

	return(dest);
}
