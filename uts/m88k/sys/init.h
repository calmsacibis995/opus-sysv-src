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
