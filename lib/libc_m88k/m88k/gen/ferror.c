
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/ferror.c"

#include <stdio.h>

#undef _ferror
int
_ferror(p)
FILE *p;
{
	return(p->_flag & _IOERR);
}
