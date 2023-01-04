#ident "@(#) $Header: cuexit.c 4.1 88/04/26 18:15:34 root Exp $ RISC Source Base"
#include "atexit.h"

/*
 *	When the stdio package is linked in, if it is executed it will
 *	place a pointer to the cleanup function in _stdio_cleanup.  When
 *	exit() is called, it first executes all the functions queued
 *	by the user with the atexit() function, and then executes the
 *	specified stdio cleanup function (if any).  See the draft Ansi
 *	C standard for more information.
 */

void (*_stdio_cleanup)();

/*
 *	If the atexit() function is linked in, it builds a queue of
 *	functions to execute starting at _atexit_queue.
 */

struct qdfunc *_atexit_queue;


/*
 *  FUNCTION
 *
 *	exit    clean up and exit
 *
 *  SYNOPSIS
 *
 *	New exit handling with optional call to a function to do cleanup
 *	for stdio.  This routine replaces a rather gross hack in the
 *	original SVR3 source code that was invalidated by the current
 *	SV linkers that allow random access to library archive members.
 *
 *	It also allows implementation of the draft Ansi C standard function
 *	atexit().
 *
 */

void exit (status)
int status;
{
    extern void _exit ();

    while (_atexit_queue != NULL) {
	(*(_atexit_queue -> funcp))();
	_atexit_queue = _atexit_queue -> qnext;
    }
#if m88k
/*
** The following lines have been commented out to force
** the call to _cleanup(). When exit is called the pointer
** is NULL, and so it is possible that buffers which need
** to be flushed are not. This problem can occur if you
** redirect standard out to a file.
**
**	if (_stdio_cleanup != NULL) {
**	(*_stdio_cleanup)();
**	}
*/
	_cleanup();
#else
    if (_stdio_cleanup != NULL) {
	(*_stdio_cleanup)();
    }
#endif
    _exit (status);
}
