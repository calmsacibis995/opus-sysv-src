h60818
s 00005/00002/00017
d D 1.2 90/04/20 12:48:31 root 2 1
c modified struct definitions
e
s 00019/00000/00000
d D 1.1 90/04/18 11:57:38 root 1 0
c date and time created 90/04/18 11:57:38 by root
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
 *	@(#)msgbuf.h	7.1 (Berkeley) 6/4/86
 */

#define	MSG_MAGIC	0x063061
D 2
#define	MSG_BSIZE	(4096 - 3 * sizeof (long))
E 2
struct	msgbuf {
	long	msg_magic;
	long	msg_bufx;
	long	msg_bufr;
D 2
	char	msg_bufc[MSG_BSIZE];
E 2
I 2
	char	*msg_bufc;
	long	msg_bsize;
E 2
};
#ifdef INKERNEL
struct	msgbuf msgbuf;
I 2
#define	MSG_BSIZE	msgbuf.msg_bsize
#else
#define	MSG_BSIZE	4096		/*minimum size*/
E 2
#endif
E 1
