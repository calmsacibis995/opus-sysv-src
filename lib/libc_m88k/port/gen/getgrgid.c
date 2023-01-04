/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getgrgid.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "shlib.h"
#include <grp.h>

extern struct group *_getgrent();
extern void _setgrent(), _endgrent();

struct group *
_getgrgid(gid)
register int gid;
{
	register struct group *p;

	_setgrent();
	while((p = _getgrent()) && p->gr_gid != gid)
		;
	_endgrent();
	return(p);
}
