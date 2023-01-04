/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getgrnam.c	1.8"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <grp.h>

extern struct group *_getgrent();
extern void _setgrent(), _endgrent();
extern int _strcmp();

struct group *
_getgrnam(name)
register char *name;
{
	register struct group *p;

	_setgrent();
	while((p = _getgrent()) && _strcmp(p->gr_name, name))
		;
	_endgrent();
	return(p);
}
