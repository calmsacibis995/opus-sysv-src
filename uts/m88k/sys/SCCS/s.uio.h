h01052
s 00036/00000/00000
d D 1.1 90/03/06 12:30:42 root 1 0
c date and time created 90/03/06 12:30:42 by root
e
u
U
t
T
I 1
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)uio.h	7.1 (Berkeley) 6/4/86
 */

#ifndef _UIO_
#define	_UIO_

struct iovec {
	caddr_t	iov_base;
	int	iov_len;
};

#ifndef __88000_OCS_NET_DEFINED

struct uio {
	struct	iovec *uio_iov;
	int	uio_iovcnt;
	off_t	uio_offset;
	int	uio_segflg;
	int	uio_resid;
};

enum	uio_rw { UIO_READ, UIO_WRITE };

/*
 * Segment flag values (should be enum).
 */
#define UIO_USERSPACE	0		/* from user data space */
#define UIO_SYSSPACE	1		/* from system space */
#define UIO_USERISPACE	2		/* from user I space */
#endif
#endif	/* !__88000_OCS_NET_DEFINED	*/
E 1
