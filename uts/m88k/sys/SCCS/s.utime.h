h44888
s 00023/00000/00000
d D 1.1 90/03/06 12:30:46 root 1 0
c date and time created 90/03/06 12:30:46 by root
e
u
U
t
T
I 1
#ifndef _SYS_UTIME_H_
#define _SYS_UTIME_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/


/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

struct utimbuf {
	time_t		actime;
	unsigned long	acusec;
	time_t		modtime;
	unsigned long	modusec;
};

#endif	/* ! _SYS_UTIME_H_ */
E 1
