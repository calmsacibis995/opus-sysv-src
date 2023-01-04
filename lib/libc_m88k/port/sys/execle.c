/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:sys/execle.c	1.6"
/*
 *	_execle(file, arg1, arg2, ..., 0, envp)
 */

/*
** implement _execle() call using varargs - see section 5
** of Programmer's Reference Manual
*/

#include <varargs.h>

#define MAXARGS	100

/*
** the following is the old call scheme
**
** _execle(file, args)
** char *file, *args;
*/

/*
** Things have to be done differently whether the Austin (Motorola)
** compiler and varargs implementation is used or whether the 
** Green Hills Copiler and varargs implementation is done
**
** PEG	2/15/88
*/

_execle(va_alist)
va_dcl
{
	va_list	ap1;

	char	*file;
	char	*args[MAXARGS];
	int		argno = 0;
	int		envno = 0;

	char	**env;

/*
** va_start must be the first executable instruction
*/

	va_start(ap1);

/*
** get the file name
*/

	file = va_arg(ap1, char *);

/*
** get the first set of arguments from the list
*/

	while((args[argno++] = va_arg(ap1, char *)) != (char *) 0)
		;

	env = va_arg(ap1, char **);

	envno = argno;

/*
** get the set of environment strings from the list
*/

	while((args[argno++] = *env++) != (char *) 0)
		;

	va_end(ap1);

	return(_execve(file, args, &args[envno]));
}
