h54920
s 00025/00000/00000
d D 1.1 90/03/06 12:30:38 root 1 0
c date and time created 90/03/06 12:30:38 by root
e
u
U
t
T
I 1
#ifndef _SYS_TIMES_H_
#define _SYS_TIMES_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Structure returned by times()
 */
struct tms {
	clock_t	tms_utime;		/* user time */
	clock_t	tms_stime;		/* system time */
	clock_t	tms_cutime;		/* user time, children */
	clock_t	tms_cstime;		/* system time, children */
};

#endif	/* ! _SYS_TIMES_H_ */
E 1
