/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)subr_log.c	7.1 (Berkeley) 6/5/86
 */

/*
 * Error log buffer for kernel printf's.
 */

#include "sys/types.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/ioctl.h"
#include "sys/msgbuf.h"
#include "sys/file.h"
#include "sys/errno.h"
#include "sys/buf.h"

#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4

#define LOG_RDPRI	(PZERO + 1)

#define LOG_NBIO	0x02
#define LOG_ASYNC	0x04
#define LOG_RDWAIT	0x08

#define minor_t	unsigned char

struct logsoftc {
	int	sc_state;		/* see above for possibilities */
	struct	proc *sc_selp;		/* process waiting on select call */
	int	sc_pgrp;		/* process group for async I/O */
} logsoftc;

int	log_open;			/* also used in log() */

/*ARGSUSED*/
logopen(dev)
	minor_t dev;
{

	if (dev == 1)			/* write device */
		return;
	if (log_open) {
		u.u_error = EBUSY;
		return;
	}
	log_open = 1;
	logsoftc.sc_selp = 0;
	logsoftc.sc_pgrp = u.u_procp->p_pgrp;
	/*
	 * Potential race here with putchar() but since putchar should be
	 * called by autoconf, msg_magic should be initialized by the time
	 * we get here.
	 */
	if (msgbuf.msg_magic != MSG_MAGIC) {
		register int i;

		msgbuf.msg_magic = MSG_MAGIC;
		msgbuf.msg_bufx = msgbuf.msg_bufr = 0;
		for (i=0; i < MSG_BSIZE; i++)
			msgbuf.msg_bufc[i] = 0;
	}
	return (0);
}

/*ARGSUSED*/
logclose(dev, flag)
	minor_t dev;
{
	if (dev == 1)		/* write device */
		return;
	log_open = 0;
	logsoftc.sc_state = 0;
	logsoftc.sc_selp = 0;
	logsoftc.sc_pgrp = 0;
}

/*ARGSUSED*/
logread(dev)
	minor_t dev;
{
	register long l;
	register int s;
	int error = 0;

	if (dev != 0) {
		u.u_error = ENXIO;
		return;
	}
	s = splhigh();
	while (msgbuf.msg_bufr == msgbuf.msg_bufx) {
		if (logsoftc.sc_state & LOG_NBIO) {
			splx(s);
			u.u_error = ewouldblock();
			return;
		}
		logsoftc.sc_state |= LOG_RDWAIT;
		sleep((caddr_t)&msgbuf, LOG_RDPRI);
	}
	splx(s);
	logsoftc.sc_state &= ~LOG_RDWAIT;

	while (u.u_count > 0) {
		l = msgbuf.msg_bufx - msgbuf.msg_bufr;
		if (l < 0)
			l = MSG_BSIZE - msgbuf.msg_bufr;
		l = MIN(l, u.u_count);
		if (l == 0)
			break;
		iomove((caddr_t)&msgbuf.msg_bufc[msgbuf.msg_bufr],
			(int)l, B_READ);
		if (u.u_error)
			break;
		msgbuf.msg_bufr += l;
		if (msgbuf.msg_bufr < 0 || msgbuf.msg_bufr >= MSG_BSIZE)
			msgbuf.msg_bufr = 0;
	}
}

logwrite(dev)
	minor_t dev;
{
	register  c;
	if (dev != 1) {
		u.u_error = ENXIO;
		return;
	}
	while (u.u_count) {
		if ((c = fubyte(u.u_base++)) < 0) {
			u.u_error = EFAULT;
			break;
		}
		u.u_count--;
		outputc (c, TOLOG | TOCONS, (struct tty *) 0);
	}
}

/*ARGSUSED*/
logselect(fp, ip, rw)
	struct file *fp;
	struct inode *ip;
	int rw;
{
	int s = splhigh();

	switch (rw) {

	case FREAD:
		if (msgbuf.msg_bufr != msgbuf.msg_bufx) {
			splx(s);
			return (1);
		}
		logsoftc.sc_selp = u.u_procp;
		break;
	}
	splx(s);
	return (0);
}

logwakeup()
{

	if (!log_open)
		return;
	if (logsoftc.sc_selp) {
		selwakeup(logsoftc.sc_selp, 0);
		logsoftc.sc_selp = 0;
	}
	if (logsoftc.sc_state & LOG_ASYNC)
		signal(logsoftc.sc_pgrp, SIGIO); 
	if (logsoftc.sc_state & LOG_RDWAIT) {
		wakeup((caddr_t)&msgbuf);
		logsoftc.sc_state &= ~LOG_RDWAIT;
	}
}

/*ARGSUSED*/
logioctl(dev, cmd, arg, mode)
{
	long l, *data;
	int s;

	data = &l;
	switch (cmd) {

	/* return number of characters immediately available */
	case FIONREAD:
		s = splhigh();
		l = msgbuf.msg_bufx - msgbuf.msg_bufr;
		splx(s);
		if (l < 0)
			l += MSG_BSIZE;
		*(off_t *)data = l;
		break;

	case FIONBIO:
		if (copyin((caddr_t) arg, (caddr_t) data, sizeof (*data))) {
			u.u_error = EFAULT;
			return;
		} 
		if (*(int *)data)
			logsoftc.sc_state |= LOG_NBIO;
		else
			logsoftc.sc_state &= ~LOG_NBIO;
		break;

	case FIOASYNC:
		if (copyin((caddr_t) arg, (caddr_t) data, sizeof (*data))) {
			u.u_error = EFAULT;
			return;
		} 
		if (*(int *)data)
			logsoftc.sc_state |= LOG_ASYNC;
		else
			logsoftc.sc_state &= ~LOG_ASYNC;
		break;

	case TIOCSPGRP:
		if (copyin((caddr_t) arg, (caddr_t) data, sizeof (*data))) {
			u.u_error = EFAULT;
			return;
		} 
		logsoftc.sc_pgrp = *(int *)data;
		break;

	case TIOCGPGRP:
		*(int *)data = logsoftc.sc_pgrp;
		if (copyout((caddr_t) data, (caddr_t) arg, sizeof (*data))) {
			u.u_error = EFAULT;
			return;
		} 
		break;

	default:
		return (-1);
	}
	return (0);
}

prfloginit(addr, size)
char *addr;
{
	msgbuf.msg_bufc = addr;
	msgbuf.msg_bsize = size;
}
