/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/cuserid.c	1.8"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <stdio.h>
#include <pwd.h>

extern char *_strcpy(), *_getlogin();
extern int _getuid();
extern struct passwd *_getpwuid();
static char res[L_cuserid];

char *
_cuserid(s)
char	*s;
{
	register struct passwd *pw;
	register char *p;

	if (s == NULL)
		s = res;
#if !m88k
	p = _getlogin();
	if (p != NULL)
		return (_strcpy(s, p));
	pw = _getpwuid(_getuid());
#else
	pw = _getpwuid(_geteuid());
#endif
	_endpwent();
	if (pw != NULL)
		return (_strcpy(s, pw->pw_name));
	*s = '\0';
	return (NULL);
}
