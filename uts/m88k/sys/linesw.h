#ifndef _SYS_LINESW_H_
#define _SYS_LINESW_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/* Line Discipline Switch Table */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

extern int nulldev();
extern int ttopen(),ttclose(),ttread(),ttwrite(),ttioctl(),ttin(),ttout();
extern int sxtin(),sxtout();

/*   order: open close read write ioctl rxint txint modemint */
struct linesw linesw[]
	={
/* tty ------------- */
	   ttopen,
	   ttclose,
	   ttread,
	   ttwrite,
	   ttioctl,
	   ttin,
	   ttout,
	   nulldev,
/* sxt ------------- */
	   nulldev,
	   nulldev,
	   nulldev,
	   nulldev,
	   nulldev,
	   sxtin,
	   sxtout,
	   nulldev,
	   };
int linecnt ={2};

#endif	/* ! _SYS_LINESW_H_ */
