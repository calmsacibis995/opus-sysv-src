h46778
s 00033/00000/00000
d D 1.1 90/03/06 12:29:54 root 1 0
c date and time created 90/03/06 12:29:54 by root
e
u
U
t
T
I 1
#ifndef _SYS_LPRIO_H_
#define _SYS_LPRIO_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*		Copyright (c) 1985 AT&T		*/
/*		All Rights Reserved		*/
/*		@(#)lprio.h			8.1	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Line Printer Type Devices I/O Control
 */

struct lprio {
	short	ind;
	short	col;
	short	line;
};

/* ioctl commands */
#define	LPR	('l'<<8)
#define	LPRGET	(LPR|01)
#define	LPRSET	(LPR|02)
#define	LPRGETV	(LPR|05)
#define	LPRSETV	(LPR|06)

#endif	/* ! _SYS_LPRIO_H_ */
E 1
