/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/system.c	1.9"
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
#include <signal.h>

static char bin_shell[] = "/bin/sh";
static char shell[] = "sh";
static char shflg[]= "-c";

extern int _fork(), _execl(), _wait();
extern	void(*_signal())();

int
_system(s)
char	*s;
{
	int	status, pid, w;
	void (*istat)(), (*qstat)();

	if((pid = _fork()) == 0) {
		(void) _execl(bin_shell, shell, shflg, s, (char *)0);
		_exit(127);
	}
	istat = _signal(SIGINT, SIG_IGN);
	qstat = _signal(SIGQUIT, SIG_IGN);
	while((w = _wait(&status)) != pid && w != -1)
		;
	(void) _signal(SIGINT, istat);
	(void) _signal(SIGQUIT, qstat);
	return((w == -1)? w: status);
}
