#ident "@(#) $Header: atexit.h 4.1 88/04/26 18:07:02 root Exp $ RISC Source Base"
#ifndef NULL
#  define NULL 0
#endif

/*
 *	The queue of functions to execute on exit is kept as a linked
 *	list, with newer functions pushed onto the head of the list.
 */

struct qdfunc {
    struct qdfunc *qnext;
    void (*funcp)();
};


