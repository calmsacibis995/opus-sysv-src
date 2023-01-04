h21691
s 00069/00000/00000
d D 1.1 90/03/06 12:29:39 root 1 0
c date and time created 90/03/06 12:29:39 by root
e
u
U
t
T
I 1
#ifndef _SYS_FCNTL_H_
#define _SYS_FCNTL_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* Flag values accessible to open(2) and fcntl(2) */
/*  (The first three can only be set by open) */
#define	O_RDONLY 0
#define	O_WRONLY 1
#define	O_RDWR	 2
#define	O_ACCMODE	3
#define	O_NDELAY 04	/* Non-blocking I/O */
#define	O_APPEND 010	/* append (writes guaranteed at the end) */
#define O_SYNC	 020	/* synchronous write option */
#define O_NONBLOCK	 0100

/* Flag values accessible only to open(2) */
#define	O_CREAT	00400	/* open with file create (uses third open arg)*/
#define	O_TRUNC	01000	/* open with truncation */
#define	O_EXCL	02000	/* exclusive open */
#define	O_NOCTTY 04000

/* fcntl(2) requests */
#define	F_DUPFD	0	/* Duplicate fildes */
#define	F_GETFD	1	/* Get fildes flags */
#define	F_SETFD	2	/* Set fildes flags */
#define	F_GETFL	3	/* Get file flags */
#define	F_SETFL	4	/* Set file flags */
#define	F_GETLK	5	/* Get file lock */
#define	F_SETLK	6	/* Set file lock */
#define	F_SETLKW	7	/* Set file lock and wait */
#define F_CHKFL		8	/* Check legality of file flag changes */
#ifndef __88000_OCS_DEFINED
#define	F_SETOWN	9
#define	F_GETOWN	10
#endif /* !__88000_OCS_DEFINED */
#define F_FREESP		11

/* file segment locking set data type - information passed to system by user */
struct flock {
	short	l_type;
	short	l_whence;
	long	l_start;
	long	l_len;		/* len = 0 means until end of file */
        long	l_pid;
        short   l_sysid;
};

/* file segment locking types */
	/* Read lock */
#define	F_RDLCK	01
	/* Write lock */
#define	F_WRLCK	02
	/* Remove lock(s) */
#define	F_UNLCK	03

#define	FD_CLOEXEC	1


#endif	/* ! _SYS_FCNTL_H_ */
E 1
