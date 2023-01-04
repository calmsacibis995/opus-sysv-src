#include "sys/signal.h"
#include "sys/errno.h"
extern errno;

/*
	posix sigset_t manipulation functions 
*/

_sigismember(set, signo)
sigset_t *set;
register int signo;
{
	int off;

	if (signo <= 0 || signo >= NSIG) {
		errno = EINVAL;
		return (-1);
	}
	off = sigoff(signo);
	return((set->s[off] & sigmask(signo)) ? 1 : 0);
}
