h63719
s 00042/00000/00000
d D 1.1 90/03/06 12:29:52 root 1 0
c date and time created 90/03/06 12:29:52 by root
e
u
U
t
T
I 1
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
E 1
