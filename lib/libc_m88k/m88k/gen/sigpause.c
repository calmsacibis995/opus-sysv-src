
#include "sys/signal.h"
#include "sys/errno.h"
/*
	less often used system V signal / sigset functions 
*/

#ifndef NULL
#define	NULL	0L
#endif

_sigpause(sig)
register int sig;
{
	int off, smask;
	sigset_t local;
	extern errno;

	if (sig <= 0 || sig >= NSIG || sig == SIGKILL) {
		errno = EINVAL;
		return(-1);
	}
	if (_sigprocmask(NULL, NULL, &local) == -1)
		return -1;

	off = sigoff(sig);
	smask = sigmask(sig);
	local.s[off] &= ~smask;

	return (_sigsuspend(&local));
}
