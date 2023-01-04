/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getpwuid.c	1.8"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <pwd.h>
extern struct passwd *_getpwent();
extern void _setpwent(), _endpwent();

struct passwd *
_getpwuid(uid)
register int uid;
{
	register struct passwd *p;

	_setpwent();
	while((p = _getpwent()) && p->pw_uid != uid)
		;
	_endpwent();
	return(p);
}
