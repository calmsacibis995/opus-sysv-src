/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/execlp.c	1.9"
/*LINTLIBRARY*/
/*
 *	_execlp(name, arg,...,0)	(like execl, but does path search)
 */
#include <varargs.h>
#define MAXARGS 100

extern char *getenv();
extern int execvp();

int
_execlp(va_alist)
va_dcl
{
	va_list	ap;
	char	*name;
	char	*args[MAXARGS];
	int		argno = 0;

/*
** va_start must be the first executable instruction
*/

	va_start(ap);

/*
** pick up file name
*/

	name = va_arg(ap, char *);

/*
** get the arguments from the list
*/

	while((args[argno++] = va_arg(ap, char *)) != (char *) 0)
		;

	va_end(ap);

	return(execvp(name, args));
}
