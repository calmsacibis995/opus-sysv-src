/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/ttyslot.c	1.9"
/*LINTLIBRARY*/
/*
 * Return the number of the slot in the utmp file
 * corresponding to the current user: try for file 0, 1, 2.
 * Returns -1 if slot not found.
 */
#include <sys/types.h>
#include "utmp.h"
#define	NULL	0

extern char *_ttyname(), *_strrchr();
extern int _strncmp(), _open(), _read(), _close();

static char utmp_file[] = "/etc/utmp";

int
_ttyslot()
{
	struct utmp ubuf;
	register char *tp, *p;
	register int s, fd;

	if((tp=_ttyname(0)) == NULL && (tp=_ttyname(1)) == NULL &&
					(tp=_ttyname(2)) == NULL)
		return(-1);

	if((p=_strrchr(tp, '/')) == NULL)
		p = tp;
	else
		p++;

	if((fd=_open(utmp_file, 0)) < 0)
		return(-1);
	s = 0;
	while(_read(fd, (char*)&ubuf, sizeof(ubuf)) == sizeof(ubuf)) {
		if(    (ubuf.ut_type == INIT_PROCESS ||
			ubuf.ut_type == LOGIN_PROCESS ||
			ubuf.ut_type == USER_PROCESS ||
			ubuf.ut_type == DEAD_PROCESS ) &&
			_strncmp(p, ubuf.ut_line, sizeof(ubuf.ut_line)) == 0){

			(void) _close(fd);
			return(s);
		}
		s++;
	}
	(void) _close(fd);
	return(-1);
}
