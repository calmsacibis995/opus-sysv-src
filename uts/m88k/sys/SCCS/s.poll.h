h48440
s 00051/00000/00000
d D 1.1 90/03/06 12:30:07 root 1 0
c date and time created 90/03/06 12:30:07 by root
e
u
U
t
T
I 1
#ifndef _SYS_POLL_H_
#define _SYS_POLL_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Structure of file descriptor/event pairs supplied in
 * the poll arrays.
 */
struct pollfd {
	int fd;				/* file desc to poll */
	short events;			/* events of interest on fd */
	short revents;			/* events that occurred on fd */
};

/*
 * Testable select events 
 */
#define POLLIN		01		/* fd is readable */
#define POLLPRI		02		/* priority info at fd */
#define	POLLOUT		04		/* fd is writeable (won't block) */

/*
 * Non-testable poll events (may not be specified in events field,
 * but may be returned in revents field).
 */
#define POLLERR		010		/* fd has error condition */
#define POLLHUP		020		/* fd has been hung up on */
#define POLLNVAL	040		/* invalid pollfd entry */

#ifndef __88000_OCS_DEFINED
/*
 * Number of pollfd entries to read in at a time in poll.
 * The larger the value the better the performance, up to the
 * maximum number of open files allowed.  Large numbers will
 * use excessive amounts of kernel stack space.
 */
#define NPOLLFILE	20
#endif


#endif	/* ! _SYS_POLL_H_ */
E 1
