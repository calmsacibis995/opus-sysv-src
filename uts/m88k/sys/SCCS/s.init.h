h54149
s 00036/00000/00000
d D 1.1 90/03/06 12:29:47 root 1 0
c date and time created 90/03/06 12:29:47 by root
e
u
U
t
T
I 1
#ifndef _SYS_INIT_H_
#define _SYS_INIT_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

extern int cinit(), binit(), inoinit(), fsinit(), finit(), iinit(), flckinit();
extern int strinit(), debuginit(), spl0();
extern int startup(), clkstart(), fpuinit();

int      (*init_tbl[])() = {
	startup,
	clkstart,
	cinit,
	binit,
	inoinit,
	fsinit,
	finit,
	spl0,
	iinit,
	flckinit,
#ifdef STREAMS
	strinit,
#endif /* STREAMS */
/*     	debuginit,    not right now for 68020 */
	fpuinit,
	0
};


#endif	/* ! _SYS_INIT_H_ */
E 1
