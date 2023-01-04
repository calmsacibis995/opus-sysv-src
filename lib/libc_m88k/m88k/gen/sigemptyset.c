#include "sys/signal.h"

/*
	posix sigset_t manipulation functions 
*/

_sigemptyset(set)
sigset_t *set;
{
	set->s[0] = 0;
	set->s[1] = 0;
	return(0);
}
