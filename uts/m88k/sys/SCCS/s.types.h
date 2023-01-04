h57320
s 00003/00000/00119
d D 1.2 90/04/24 11:13:56 root 2 1
c added volatile
e
s 00119/00000/00000
d D 1.1 90/03/06 12:30:41 root 1 0
c date and time created 90/03/06 12:30:41 by root
e
u
U
t
T
I 1
#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_


/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
I 2
#ifndef __DCC__
#define	volatile
#endif
E 2

#ifndef _CLOCK_T_
#define	_CLOCK_T_
typedef long	clock_t;
#endif

typedef	long		daddr_t;	/* <disk address> type */
typedef	unsigned short	ushort;
typedef	unsigned long	ino_t;		/* <inode> type */

#ifndef _TIME_T_
#define	_TIME_T_
typedef long	time_t;
#endif

#ifndef _UID_T_
#define _UID_T_
typedef	unsigned long	uid_t;
#endif
typedef	unsigned long 	dev_t;		/* <old device number> type */

#ifndef _GID_T_
#define _GID_T_
typedef	unsigned long	gid_t;
#endif

typedef	long		off_t;		/* ?<offset> type */
typedef	long		key_t;		/* IPC key type */
typedef	long		pid_t;
typedef	unsigned long	mode_t;

#ifndef _SIZE_T_
#define	_SIZE_T_
typedef unsigned int	size_t;
#endif

typedef unsigned long	nlink_t;	/* for stat struct */

/*
 * Select defines per Berk 4.3 MAN
 */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#define	NBBY	8		/* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#ifdef INKERNEL
#define FD_ZERO(p)	memset((char *)(p), 0, sizeof(*(p)))
#else
#define FD_ZERO(p)	_memset((char *)(p), 0, sizeof(*(p)))
#endif

typedef	char *		caddr_t;	/* ?<core address> type */
typedef	unsigned char	unchar;
typedef	unsigned int	uint;
typedef	unsigned long	ulong;

#ifndef __88000_OCS_DEFINED
typedef	struct { int r[1]; } *	physadr;
typedef	short		cnt_t;		/* ?<count> type */
#define	LABELSIZE	24
typedef	int		label_t[LABELSIZE];
typedef	long		paddr_t;	/* <physical address> type */
typedef	unsigned char	use_t;		/* use count for swap.  */
typedef	short		sysid_t;
typedef	short		index_t;
typedef	short		lock_t;		/* lock work for busy wait */

/*
 * Distributed UNIX hook 
 */
typedef struct cookie {
	long	c_sysid;
	long 	c_rcvd;
} *cookie_t;

#ifndef _SIGSET_T
#define	_SIGSET_T
typedef struct sigset { unsigned long s[2]; } sigset_t;
#endif
#endif /* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_TYPES_H_ */
E 1
