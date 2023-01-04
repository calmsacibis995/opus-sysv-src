/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/abort.c	1.12"
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
/*
 *	_abort() - terminate current process with dump via SIGABRT
 */

#include <signal.h>

extern int _kill(), _getpid();
static pass = 0;		/* counts how many times abort has been called*/
extern	void(*_signal())();

int
_abort()
{
	void (*sig)();

	if ((sig = _signal(SIGABRT,SIG_DFL)) != SIG_DFL) 
		(void) _signal(SIGABRT,sig); 
	else if (++pass == 1)
		_cleanup();
	return(_kill(_getpid(), SIGABRT));
}
