/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:sys/execl.c	1.5"
/*
 *	execl(name, arg0, arg1, ..., argn, 0)
 *	environment automatically passed.
 */

#include "shlib.h"

/*
** implement execl() call using varargs - see section 5
** of Programmer's Reference Manual
*/

#include <varargs.h>

#define MAXARGS	100

/*
** the following is the old call scheme
**
** execl(name, args)
** char *name, *args;
*/

_execl(va_alist)
va_dcl
{
	va_list	ap;
	char	*name;
	char	*args[MAXARGS];
	int		argno = 0;

	extern char **environ;

/*
** va_start must be the first executable instruction
*/

	va_start(ap);

/*
** pick up the file name
*/

	name = va_arg(ap, char *);

/*
** get the arguments from the list
*/

	while((args[argno++] = va_arg(ap, char *)) != (char *) 0)
		;

	va_end(ap);

	return (_execve(name, args, environ));
}
