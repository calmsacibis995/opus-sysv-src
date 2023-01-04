/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)sys_socket.c	7.1 (Berkeley) 6/5/86
 */

#include "param.h"
#include "systm.h"
#include "dir.h"
#include "user.h"
#include "proc.h"
#include "file.h"
#include "mbuf.h"
#include "protosw.h"
#include "socket.h"
#include "socketvar.h"
#include "ioctl.h"
#include "uio.h"
#include "stat.h"
#include "inode.h"
#include "unpcb.h"
#include "conf.h"
#include "fstyp.h"
#include "fcntl.h"

int soreceive(), sosend();

#define	ICHECK(ip, p)	if (ip->i_ftype != IFMPB) { \
			u.u_error = EINVAL; \
			printf("%s: ip %x, type %x\n", p, ip, ip->i_ftype); \
			return; \
			}

sockreadi(ip)
struct inode *ip;
{
	soo_rw(ip, soreceive);
}

sockwritei(ip)
struct inode *ip;
{
	soo_rw(ip, sosend);
}

soo_rw(ip, func)
struct inode *ip;
int (*func)();
{
	
	struct uio auio;
	struct iovec aiov;

	ICHECK(ip, "socket r/w");

	aiov.iov_base = (caddr_t)u.u_base;
	aiov.iov_len = u.u_count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = u.u_count;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_offset = u.u_offset;
	u.u_error = (*func)((struct socket *) ip->i_fsptr, 0, &auio, 0, 0);
	u.u_count = auio.uio_resid;
}

sockioctl(ip, cmd, arg, flag)
	struct inode *ip;
	register int cmd;
	caddr_t arg;
{
	register struct socket *so = (struct socket *)ip->i_fsptr;
	register unsigned size;
	char data[IOCPARM_MASK+1];
	register long *temp;

	ICHECK(ip, "ioctl");

	temp = (long *) data;
	size = 0;
	switch (cmd) {

	case FIONBIO:
		if (copyin( arg, temp, sizeof(int))) {
			u.u_error = EFAULT;
			return;
		}
		if (*temp)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		return;

	case FIOASYNC:
		if (copyin( arg, temp, sizeof(int))) {
			u.u_error = EFAULT;
			return;
		}
		if (*temp)
			so->so_state |= SS_ASYNC;
		else
			so->so_state &= ~SS_ASYNC;
		return;

	case FIONREAD:
		*temp = so->so_rcv.sb_cc;
		size = sizeof(int);
		break;

	case FIOSETOWN:
	case SIOCSPGRP:
		if (copyin( arg, temp, sizeof(int))) {
			u.u_error = EFAULT;
			return;
		}
		so->so_pgrp = *temp;
		return;

	case FIOGETOWN:
	case SIOCGPGRP:
		*temp = so->so_pgrp;
		size = sizeof(int);
		break;

	case SIOCATMARK:
		*temp = (so->so_state&SS_RCVATMARK) != 0;
		size = sizeof(int);
		break;

	default:
		/*
		 * Interpret high order word to find
		 * amount of data to be copied to/from the
		 * user's address space.
		 */
		size = (cmd &~ (IOC_INOUT|IOC_VOID)) >> 16;
		if (size > sizeof (data)) {
			u.u_error = EFAULT;
			return;
		}
		if (cmd&IOC_IN) {
			if (size) {
				if (copyin(arg, (caddr_t)data, (u_int)size)) {
					u.u_error = EFAULT;
					return;
				}
			} else
				*(caddr_t *)data = arg;
		} else if ((cmd&IOC_OUT) && size)
			/*
			 * Zero the buffer on the stack so the user
			 * always gets back something deterministic.
			 */
			bzero((caddr_t)data, size);
		else if (cmd&IOC_VOID)
			*(caddr_t *)data = arg;
		/*
	 	* Interface/routing/protocol specific ioctls:
	 	* interface and routing ioctls should have a
	 	* different entry since a socket's unnecessary
	 	*/
#define	cmdbyte(x)	(((x) >> 8) & 0xff)
		if (cmdbyte(cmd) == 'i')
			u.u_error = ifioctl(so, cmd, data);
		else if (cmdbyte(cmd) == 'r')
			u.u_error = rtioctl(cmd, data);
		else
		u.u_error = (*so->so_proto->pr_usrreq)(so, PRU_CONTROL, 
	    	(struct mbuf *)cmd, (struct mbuf *)data, (struct mbuf *)0);
		break;
	}
	/*
	 * Copy any data to user, size was
	 * already set and checked above.
	 */
	if (u.u_error == 0 && (cmd&IOC_OUT) && size)
		if (copyout(data, arg, size))
			u.u_error = EFAULT;
}

soo_select(fp, which)
	struct file *fp;
	int which;
{
	register struct socket *so = (struct socket *)fp->f_inode->i_fsptr;
	register int s = splnet();

	switch (which) {

	case FREAD:
		if (soreadable(so)) {
			splx(s);
			return (1);
		}
		sbselqueue(&so->so_rcv);
		break;

	case FWRITE:
		if (sowriteable(so)) {
			splx(s);
			return (1);
		}
		sbselqueue(&so->so_snd);
		break;

	case 0:
		if (so->so_oobmark ||
		    (so->so_state & SS_RCVATMARK)) {
			splx(s);
			return (1);
		}
		sbselqueue(&so->so_rcv);
		break;
	}
	splx(s);
	return (0);
}

/*ARGSUSED*/
sockstatf(ip, ub)
	register struct inode *ip;
	register struct ostat *ub;
{
	register struct socket *so;

	ICHECK(ip, "statf");

	so = (struct socket *) ip->i_fsptr;

	bzero((caddr_t) ub, sizeof(*ub));
	ub->st_mode = S_IFSOCK;
	
	/*
	return (u.u_error = (*so->so_proto->pr_usrreq)(so, PRU_SENSE,
	    (struct mbuf *)ub, (struct mbuf *)0, 
	    (struct mbuf *)0));
	*/
}

sockclosei(ip, flag, count, offset)
	struct inode *ip;
{
	if ((unsigned) count > 1)
		return;
	ICHECK(ip, "close");
	if (ip->i_fsptr)
		u.u_error = soclose((struct socket *)ip->i_fsptr);
}

socknotify(ip)
register struct inode *ip;
{
	ICHECK(ip, "notify");
	u.u_error = ESPIPE;
}

sockfcntl(ip, cmd, arg, flag)
register struct inode *ip;
{
	register struct socket *so;
	struct proc *p;

	ICHECK(ip, "fcntl");
	so = (struct socket *) ip->i_fsptr;
	switch (cmd)
	{
		case F_CHKFL:
			so->so_state &= ~SS_NBIO;
			if (arg & (FNONB|FNDELAY))
				so->so_state |= SS_NBIO;
			break;

		case F_GETOWN:
			u.u_rval1 = so->so_pgrp;
			break;

		case F_SETOWN:
			{
				struct proc *procfind();

				p = procfind(arg, arg > 0 ? 0 : 1);
				if (p == NULL) {
					u.u_error = ESRCH;
					break;
				}
				so->so_pgrp = arg;
			}
			break;
			
		default:
			u.u_error = EINVAL;
			printf("SOCK FCNTL ip = %x cmd = %x\n", ip, cmd);
			break;
	}
}
