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
