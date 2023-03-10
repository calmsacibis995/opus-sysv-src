/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/sleep.c	1.11"
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
/*
 * Suspend the process for `sleep_tm' seconds - using alarm/pause
 * system calls.  If caller had an alarm already set to go off `n'
 * seconds from now, then Case 1: (sleep_tm >= n) sleep for n, and
 * cause the callers previously specified alarm interrupt routine
 * to be executed, then return the value (sleep_tm - n) to the caller
 * as the unslept amount of time, Case 2: (sleep_tm < n) sleep for
 * sleep_tm, after which, reset alarm to go off when it would have
 * anyway.  In case process is aroused during sleep by any caught
 * signal, then reset any prior alarm, as above, and return to the
 * caller the (unsigned) quantity of (requested) seconds unslept.
 */
#include <signal.h>
#include <setjmp.h>

extern int pause();
extern unsigned _alarm();
static jmp_buf env;

unsigned
_sleep(sleep_tm)
unsigned sleep_tm;
{
	int  alrm_flg;
	struct sigaction act, oact;
	int  awake();
	unsigned unslept, alrm_tm, left_ovr;
	sigset_t set, oset, aset;

	if(sleep_tm == 0)
		return(0);

	alrm_tm = _alarm(0);			/* prev. alarm time */
	_memset(&act, 0, sizeof (act));
	act.sa_handler = awake;
	_sigaction(SIGALRM, &act, &oact);

	alrm_flg = 0;
	left_ovr = 0;

	if(alrm_tm != 0) {	/* skip all this if no prev. alarm */
		if(alrm_tm > sleep_tm) {	/* alarm set way-out */
			alrm_tm -= sleep_tm;
			++alrm_flg;
		} else {	/* alarm will shorten sleep time */
			left_ovr = sleep_tm - alrm_tm;
			sleep_tm = alrm_tm;
			alrm_tm = 0;
			--alrm_flg;
			(void) _sigaction(SIGALRM, &oact, 
					(struct sigaction *) 0);
		}
	}

	/* block SIGALRM */
	_sigemptyset(&set);
	_sigaddset(&set, SIGALRM);
	_sigprocmask(SIG_BLOCK, &set, &oset);

	/* the actual alarm */
	_alarm(sleep_tm);

	/* aset is old mask not including SIGALRM */
	aset = oset;
	_sigdelset(&aset, SIGALRM);
	_sigsuspend(&aset);

	/* restore original mask */
	_sigprocmask(SIG_SETMASK, &oset, (sigset_t *) 0);
	unslept = _alarm(0);
	if(alrm_flg >= 0)
		(void) _sigaction(SIGALRM, &oact, (struct sigaction *) 0);
	if(alrm_flg > 0 || (alrm_flg < 0 && unslept != 0))
		(void) _alarm(alrm_tm + unslept);
	return(left_ovr + unslept);
}

static
awake() {
	return;
}
