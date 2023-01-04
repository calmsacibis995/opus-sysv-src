
#include "sys/signal.h"
#include "sys/errno.h"
/*
	less often used system V signal / sigset functions 
*/

#ifndef NULL
#define	NULL	0L
#endif

_pause()
{
	sigset_t local;

	local.s[0] = 0;
	local.s[1] = 0;

	return (_sigsuspend(&local));
}
