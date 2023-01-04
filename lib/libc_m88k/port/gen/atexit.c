#ident "@(#) $Header: atexit.c 4.1 88/04/26 18:15:13 root Exp $ RISC Source Base"
#include "atexit.h"


/*
 *  FUNCTION
 *
 *	atexit    ansi draft standard atexit call
 *
 *  SYNOPSIS
 *
 *	int atexit (func)
 *	void (*func)();
 *
 *  DESCRIPTION
 *
 *	The atexit function conforms to the draft ansi C standard
 *	for atexit.  Atexit registers the function pointed to by
 *	its argument, in a first in last out queue, to be called
 *	without arguments at normal program termination.
 *
 *	The draft ansi C standard specifies that the queue depth shall
 *	be sufficient to register at least 32 user specified functions.
 *	This implementation contains no built in restriction since it uses
 *	malloc to obtain space to store the queue.  
 *
 *	The draft standard does not mention the traditional call to
 *	exit without any cleanup (_exit), so it is presumed that functions
 *	registered with atexit are only executed upon a call to exit.
 *
 *  RETURNS
 *
 *	Returns zero if the registration succeeds, nonzero if it fails.
 *
 */

int _atexit (func)
void (*func)();
{
    register struct qdfunc *newfunc;
    register int rtnval;
    extern char *_malloc ();
    extern struct qdfunc *_atexit_queue;

    newfunc = (struct qdfunc *) _malloc ((unsigned) sizeof (struct qdfunc));
    if (newfunc == NULL) {
	rtnval = 1;
    } else {
	newfunc -> qnext = _atexit_queue;
	newfunc -> funcp = func;
	_atexit_queue = newfunc;
	rtnval = 0;
    }
    return (rtnval);
}

