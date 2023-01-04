/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getlogin.c	1.9"
/*LINTLIBRARY*/
#include <sys/types.h>
#include "utmp.h"
#include "fcntl.h"

#define NULL 0

extern long _lseek();
extern int _open(), _read(), _close(), _ttyslot();

static char utmp_file[] = "/etc/utmp";

char *
_getlogin()
{
	register me, uf;
	struct utmp ubuf ;
	static char answer[sizeof(ubuf.ut_user)+1] ;

	/* posix requires a controlling tty for getlogin to succeed, 
	 * this is the only way to check for the existance of a controlling tty.
	 */
	if ((me = _open("/dev/tty", O_RDONLY)) < 0)
		return(NULL);
	_close(me);
	/* end of posix check */

	if((me = _ttyslot()) < 0)
		return(NULL);
	if((uf = _open(utmp_file, 0)) < 0)
		return(NULL);
	(void) _lseek(uf, (long)(me * sizeof(ubuf)), 0);
	if(_read(uf, (char*)&ubuf, sizeof(ubuf)) != sizeof(ubuf)) {
		(void) _close(uf);
		return(NULL);
	}
	(void) _close(uf);
	if(ubuf.ut_user[0] == '\0')
		return(NULL);
	_strncpy(&answer[0],&ubuf.ut_user[0],sizeof(ubuf.ut_user)) ;
	answer[sizeof(ubuf.ut_user)] = '\0' ;
	return(&answer[0]);
}
