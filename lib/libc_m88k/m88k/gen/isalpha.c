
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/isalpha.c"

#include "lib_ctype.h"

extern unsigned char __ctype[];

#undef _isalpha
int
_isalpha(c)
int c;
{
	return((__ctype + 1)[c] & (_U | _L));
}
