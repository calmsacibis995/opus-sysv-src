h33862
s 00141/00000/00000
d D 1.1 90/03/06 12:30:27 root 1 0
c date and time created 90/03/06 12:30:27 by root
e
u
U
t
T
I 1
#ifndef _SYS_STROPTS_H_
#define _SYS_STROPTS_H_


/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#include <sys/ioctl.h>

/*
 * Read options
 */

#define RNORM 	0			/* read msg norm */
#define RMSGD	1			/* read msg discard */
#define RMSGN	2			/* read msg no discard */

/*
 * Events for which to be sent SIGPOLL signal
 */
#define S_INPUT		001		/* regular priority msg on read Q */
#define S_HIPRI		002		/* high priority msg on read Q */
#define S_OUTPUT	004		/* write Q no longer full */
#define S_MSG		010		/* signal msg at front of read Q */

/*
 * Flags for recv() and send() syscall arguments
 */
#define RS_HIPRI	1		/* send/recv high priority message */

/*
 * Flags returned as value of recv() syscall
 */
#define MORECTL		1		/* more ctl info is left in message */
#define MOREDATA	2		/* more data is left in message */

/*
 *  Stream Ioctl defines
 */
#define I_NREAD		_IOR(S,1,long)
#define I_PUSH		_IO(S,2)
#define I_POP		_IO(S,3)
#define I_LOOK		_IO(S,4)
#define I_FLUSH		_IO(S,5)
#define I_SRDOPT	_IO(S,6)
#define I_GRDOPT	_IOR(S,7,int)
#define I_STR		_IOWR(S,8,struct strioctl)
#define I_SETSIG	_IO(S,9)
#define I_GETSIG	_IOR(S,10,long)
#define I_FIND		_IO(S,11)
#define I_LINK		_IO(S,12)
#define I_UNLINK	_IO(S,13)
#define I_PEEK		_IOWR(S,15,struct strpeek)
#define I_FDINSERT	_IOW(S,16,struct strfdinsert)
#define I_SENDFD	_IO(S,17)
#define I_RECVFD	_IOR(S,18,struct strrecvfd)

/*
 * User level ioctl format for ioctl that go downstream I_STR 
 */
struct strioctl {
	int 	ic_cmd;			/* command */
	int	ic_timout;		/* timeout value */
	int	ic_len;			/* length of data */
	char	*ic_dp;			/* pointer to data */
};

/*
 * Stream buffer structure for send and recv system calls
 */
struct strbuf {
	int	maxlen;			/* no. of bytes in buffer */
	int	len;			/* no. of bytes returned */
	char	*buf;			/* pointer to data */
};

/* 
 * stream I_PEEK ioctl format
 */

struct strpeek {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long	      flags;
};

/*
 * stream I_FDINSERT ioctl format
 */
struct strfdinsert {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long	      flags;
	int	      fildes;
	int	      offset;
};

/*
 * receive file descriptor structure
 */
struct strrecvfd {
#ifdef INKERNEL
	union {
		struct file *fp;
		int fd;
	} f;
#else
	int fd;
#endif
	unsigned long uid;
	unsigned long gid;
	char fill[8];
};

#ifndef __88000_OCS_DEFINED

#define	STR		('S'<<8)

/*
 * Flush options
 */

#define FLUSHR 1			/* flush read queue */
#define FLUSHW 2			/* flush write queue */
#define FLUSHRW 3			/* flush both queues */

/*
 * Value for timeouts (ioctl, select) that denotes infinity
 */
#define INFTIM		-1
#endif


#endif	/* ! _SYS_STROPTS_H_ */
E 1
