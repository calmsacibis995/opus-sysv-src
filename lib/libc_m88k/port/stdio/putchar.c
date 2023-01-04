/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/putchar.c	1.6"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * A subroutine version of the macro putchar
 * Modified for POSIX
 */
#include "shlib.h"
#include <stdio.h>
#undef _putchar
/*
 * Since <stdio.h> is included 'putc' becomes a macro.
 * undef putc to avoid it
 */
#undef _putc

int
_putchar(c)
register char c;
{
	return(_putc(c, stdout));
}
