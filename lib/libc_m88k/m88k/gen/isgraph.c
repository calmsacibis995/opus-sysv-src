
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/isgraph.c"

#include "lib_ctype.h"

extern unsigned char __ctype[];

#undef _isgraph
int
_isgraph(c)
int c;
{
	return((__ctype + 1)[c] & (_P | _U | _L | _N));
}
