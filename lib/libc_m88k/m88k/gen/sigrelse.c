
#include "sys/signal.h"
#include "sys/errno.h"
/*
	less often used system V signal / sigset functions 
*/

#ifndef NULL
#define	NULL	0L
#endif

_sigrelse(sig)
register int sig;
{
	register int off;
	sigset_t local;
	extern errno;

	if (sig <= 0 || sig >= NSIG || sig == SIGKILL) {
		errno = EINVAL;
		return(-1);
	}
	off = sigoff(sig);
	local.s[off^1] = 0;
	local.s[off] = sigmask(sig);

	return _sigprocmask(SIG_UNBLOCK, &local, NULL);
}
