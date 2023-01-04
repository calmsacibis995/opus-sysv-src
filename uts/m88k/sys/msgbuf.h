/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)msgbuf.h	7.1 (Berkeley) 6/4/86
 */

#define	MSG_MAGIC	0x063061
struct	msgbuf {
	long	msg_magic;
	long	msg_bufx;
	long	msg_bufr;
	char	*msg_bufc;
	long	msg_bsize;
};
#ifdef INKERNEL
struct	msgbuf msgbuf;
#define	MSG_BSIZE	msgbuf.msg_bsize
#else
#define	MSG_BSIZE	4096		/*minimum size*/
#endif
