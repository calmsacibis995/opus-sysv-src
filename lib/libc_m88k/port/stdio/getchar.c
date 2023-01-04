/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/getchar.c	1.6"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * A subroutine version of the macro getchar.
 * Modified for POSIX
 */
#include "shlib.h"
#include <stdio.h>
#undef _getchar
/*
 * Since <stdio.h> is included 'getc' becomes a macro.
 * undef getc to avoid it
 */
#undef _getc

int
_getchar()
{
	return(_getc(stdin));
}
