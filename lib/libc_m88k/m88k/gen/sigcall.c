
#include "sys/signal.h"
#include "sys/errno.h"
/*
	sigcall.c has the most often used signal functions.
	sigops.c has the posix sigset_t manipulation functions.
	sigsysv.c has less often used system V signal functions.
*/

#define		T_SIGACT	0
#define		T_SIGSET	1
#define		T_SIGNAL	2
#ifndef NULL
#define	NULL	0L
#endif

typedef void (*fn)();
fn _sigset();

extern int	errno;

int _preserve_sigmask = 1;
fn __sigtab [NSIG];
char __setflg [NSIG];
struct sigaction __sa[NSIG];

_sigaction(sig, act, oact)
register sig;
register struct sigaction *act;
register struct sigaction *oact;
{
	register fn *p, oldfunc;
	int oldflag;
	struct sigaction local;
	extern int _sigaction();
	extern void _sigcode();

	if (sig <= 0 || sig >= NSIG) {
		errno = EINVAL;
		return(-1);
	}


	p = &__sigtab[sig-1];
	oldfunc = *p;
	oldflag = __setflg[sig-1];

	if (act) {
		local = *act;
		act = &local;
		if (act->sa_handler == (fn)SIG_HOLD) {
			errno = EINVAL;
			return(-1);
		}
		*p = act->sa_handler;
		if (act->sa_handler != (fn)SIG_DFL && 
				((int)act->sa_handler & 1) == 0)
			act->sa_handler = _sigcode;
	}
	__setflg[sig-1] = T_SIGACT;
	if (__sigaction(sig, act, oact) == -1) {
		*p = oldfunc;
		__setflg[sig-1] = oldflag;
		return (-1);
	}

	if (oact && (char *)oact->sa_handler == (char *)_sigcode)
		oact->sa_handler = oldfunc;
	return (0);
}

_sigignore(sig)
register int sig;
{
	if ((int)_sigset(sig, SIG_IGN) == -1)
		return -1;
	return 0;
}

static
fn
_signalcomm(sig, func, comm)
register int sig;
register fn func;
{
	struct sigaction act, oact;

	if (sig == SIGKILL) {
		errno = EINVAL;
		return((fn)-1);
	}

	act.sa_handler = func;
	if (_preserve_sigmask) {
		act.sa_flags = __sa[sig].sa_flags;
		act.sa_mask = __sa[sig].sa_mask;
	}
	else {
		act.sa_flags = 0;
		act.sa_mask.s[0] = 0;
		act.sa_mask.s[1] = 0;
	}
	if (_sigaction(sig, &act, &oact) == -1) {
		return((fn)-1);
	}
	if (_preserve_sigmask) {
		if (oact.sa_flags != __sa[sig].sa_flags || 
			oact.sa_mask.s[0] != __sa[sig].sa_mask.s[0] || 
			oact.sa_mask.s[1] != __sa[sig].sa_mask.s[1]) {
			__sa[sig] = oact;
			act = oact;
			act.sa_handler = func;
			if (_sigaction(sig, &act, NULL) == -1)
				return((fn)-1);
		}
	}
	__setflg[sig-1] = comm;
	return(oact.sa_handler);
}

fn
_signal(sig, func)
register int sig;
register fn func;
{
	return(_signalcomm(sig, func, T_SIGNAL));
}

fn
_sigset(sig, func)
register int sig;
register fn func;
{
	struct sigaction oact;
	sigset_t mask;
	int off;

	_sigprocmask(0, (sigset_t *) 0, &mask);
	off = sigoff(sig);

	if (func == SIG_HOLD)  {
		if (_sigaction(sig, NULL, &oact) == -1)
			return(SIG_ERR);
		if (_sighold(sig) == -1)
			return(SIG_ERR);
		if (mask.s[off] & sigmask(sig))
			return(SIG_HOLD);
		else
			return((fn)oact.sa_handler);
	}
	if (mask.s[off] & sigmask(sig)) {
		if (_signalcomm(sig, func, T_SIGSET) == SIG_ERR)
			return(SIG_ERR);
		_sigrelse(sig);
		return(SIG_HOLD);
	}
	return(_signalcomm(sig, func, T_SIGSET));
}

void 
_sigcall(arg)
register struct siginfo *arg;
{
	register fn func;
	register sig;

	if (arg->si_version != 1) {
		/* sanity test remove later */
		_write(2, "incorrect signal version\n", 25);
		return;
	}

	sig = arg->si_sigframe->sf_signum;

	if (sig > 0 && sig < NSIG) {
		func = __sigtab[ sig - 1 ];
		if (func != (fn)SIG_DFL && (((int) func)&1) == 0) {
			if (__setflg[sig-1] == T_SIGNAL) {
				_sigrelse(sig);
				if (sig != SIGILL &&
						sig != SIGTRAP && sig != SIGPWR)
					_signal(sig, SIG_DFL);
			}
			(*func)(sig, arg);
		}
	}
}
