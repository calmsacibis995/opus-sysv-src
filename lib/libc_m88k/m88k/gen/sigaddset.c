#include "sys/signal.h"
#include "sys/errno.h"
extern errno;

/*
	posix sigset_t manipulation functions 
*/

_sigaddset(set, signo)
sigset_t *set;
int signo;
{
	register off, mask;
	sigset_t l;

	if (signo <= 0 || signo >= NSIG) {
		errno = EINVAL;
		return (-1);
	}
	_sigfillset(&l);
	off = sigoff(signo);
	mask = sigmask(signo);

	if (l.s[off] & mask) {
		set->s[off] |= mask;
		return(0);
	}
	else {
		errno = EINVAL;
		return (-1);
	}
}
