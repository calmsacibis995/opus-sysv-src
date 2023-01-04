/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getpwnam.c	1.10"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <pwd.h>

extern struct passwd *_getpwent();
extern void _setpwent(), _endpwent();
extern int strncmp();

struct passwd *
_getpwnam(name)
char	*name;
{
	register struct passwd *p;

	_setpwent();
	while ((p = _getpwent()) && _strcmp(name, p->pw_name))
		;
	_endpwent();
	return (p);
}
