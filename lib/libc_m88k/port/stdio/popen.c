/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/popen.c	1.13"
/*LINTLIBRARY*/
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define	tst(a,b) (*mode == 'r'? (b) : (a))
#define	RDR	0
#define	WTR	1

extern FILE *_fdopen();
extern int _execl(), _fork(), _pipe(), _close(), _fcntl();
static int popen_pid[20];
extern	void(*_signal())();

static char bin_shell[] = "/bin/sh" ;
static char shell[] = "sh";
static char shflg[] = "-c";

FILE *
_popen(cmd, mode)
char	*cmd, *mode;
{
	int	p[2];
	register int *poptr;
	register int myside, yourside, pid;

	if(_pipe(p) < 0)
		return(NULL);
	myside = tst(p[WTR], p[RDR]);
	yourside = tst(p[RDR], p[WTR]);
	if((pid = _fork()) == 0) {
		/* myside and yourside reverse roles in child */
		int	stdio;

		/* close all pipes from other _popen's */
		for (poptr = popen_pid; poptr < popen_pid+20; poptr++) {
			if(*poptr)
				_close(poptr - popen_pid);
		}
		stdio = tst(0, 1);
		(void) _close(myside);
		(void) _close(stdio);
		(void) _fcntl(yourside, F_DUPFD, stdio);
		(void) _close(yourside);
		(void) _execl(bin_shell, shell, shflg, cmd, (char *)0);
		_exit(1);
	}
	if(pid == -1)
		return(NULL);
	popen_pid[myside] = pid;
	(void) _close(yourside);
	return(_fdopen(myside, mode));
}

int
_pclose(ptr)
FILE	*ptr;
{
	register int f, r;
	int status;
	void (*hstat)(), (*istat)(), (*qstat)();

	f = fileno(ptr);
	(void) _fclose(ptr);
	istat = _signal(SIGINT, SIG_IGN);
	qstat = _signal(SIGQUIT, SIG_IGN);
	hstat = _signal(SIGHUP, SIG_IGN);

	/* while the child is not done and no error has occured wait in the loop*/

	while((r = _wait(&status)) != popen_pid[f] && (r != -1 || errno == EINTR))
		;
	if(r == -1)
		status = -1;
	(void) _signal(SIGINT, istat);
	(void) _signal(SIGQUIT, qstat);
	(void) _signal(SIGHUP, hstat);
	/* mark this pipe closed */
	popen_pid[f] = 0;
	return(status);
}
