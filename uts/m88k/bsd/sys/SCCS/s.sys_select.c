h50703
s 00001/00001/00316
d D 1.4 90/07/24 14:01:25 root 4 3
c modified include to not use "sys"
e
s 00003/00000/00314
d D 1.3 90/04/20 12:50:20 root 3 2
c added IFIFO to ino_select switch table.
e
s 00005/00004/00309
d D 1.2 90/03/21 09:47:37 root 2 1
c use myffs instead of the cpu ff1 instruction
e
s 00313/00000/00000
d D 1.1 90/03/06 12:39:02 root 1 0
c date and time created 90/03/06 12:39:02 by root
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
 *	@(#)sys_select.c	7.1 (Berkeley) 6/5/86
 */

#include "param.h"
#include "systm.h"
#include "dir.h"
#include "user.h"
#include "ioctl.h"
#include "file.h"
#include "proc.h"
#include "stat.h"
#include "var.h"
#include "inode.h"
#include "fcntl.h"
#include "termio.h"

int	unselect(), wakeup();
int	selwait, nselcoll;

/*
 * Select system call.
 */
select()
{
	register struct uap  {
		int	s3bindex;
		int	nd;
		fd_set	*in, *ou, *ex;
		struct	timeval *tv;
	} *uap = (struct uap *)u.u_ap;
	register struct proc *p;
	fd_set ibits[3], obits[3];
	struct timeval atv;
	int s, ni, id, sflg;
	long hertz;
	time_t stlbolt;
	extern time_t lbolt;
	int ncoll;

	p = u.u_procp;

	bzero((caddr_t)ibits, sizeof(ibits));
	bzero((caddr_t)obits, sizeof(obits));
	if (uap->nd > v.v_nofiles)
		uap->nd = v.v_nofiles;	/* forgiving, if slightly wrong */
	ni = howmany(uap->nd, NFDBITS);

#define	getbits(name, x) \
	if (uap->name) { \
		u.u_error = copyin((caddr_t)uap->name, (caddr_t)&ibits[x], \
		    (unsigned)(ni * sizeof(fd_mask))); \
		if (u.u_error) { \
			u.u_error = EFAULT; \
			goto done; \
		} \
	}
	getbits(in, 0);
	getbits(ou, 1);
	getbits(ex, 2);
#undef	getbits

	hertz = 0;
	if (uap->tv) {
		u.u_error = copyin((caddr_t)uap->tv, (caddr_t)&atv,
			sizeof (atv));
		if (u.u_error) {
			u.u_error = EFAULT;
			goto done;
		}
		if (itimerfix(&atv)) {
			u.u_error = EINVAL;
			goto done;
		}
		hertz = atv.tv_sec;
		if (hertz > 0x01000000 || hertz < 0)
			hertz = 0x70000000;
		else
			hertz *= HZ;
		hertz += atv.tv_usec / (USEC_PER_TICK);
		stlbolt = lbolt;
	}
retry:
	ncoll = nselcoll;
	p->p_compat |= SSSEL|SSTIMO;
	u.u_rval1 = selscan(ibits, obits, uap->nd);
	if (u.u_error || u.u_rval1)
		goto done;
	s = spl7();
	if ((p->p_compat & SSSEL) == 0 || nselcoll != ncoll) {
		/* 
		 * if a selwakeup happened while in selscan,
		 * scan again
		 */
		p->p_compat &= ~SSSEL;
		splx(s);
		goto retry;
	}
	p->p_compat &= ~SSSEL;
	if (uap->tv) {
		hertz -= (lbolt - stlbolt);
		stlbolt = lbolt;
		if (hertz > 0)
			id = timeout(unselect, (caddr_t)p, hertz);
		else {
			splx(s);
			goto done;
		}
	}
	sflg = sleep((caddr_t)&selwait, PCATCH|(PZERO+1));
	if (hertz)
		untimeout(id);

	if (sflg) {					/* interrupted */
		u.u_error = EINTR;
	}
	else if ((p->p_compat&SSTIMO) == 0) {		/* timeout */
		u.u_rval1 = 0;
	}
	else {						/* select */
		splx(s);
		goto retry;
	}
	splx(s);
done:
	p->p_compat &= ~(SSSEL|SSTIMO);
#define	putbits(name, x) \
	if (uap->name) { \
		int error = copyout((caddr_t)&obits[x], (caddr_t)uap->name, \
		    (unsigned)(ni * sizeof(fd_mask))); \
		if (error) \
			u.u_error = EFAULT; \
	}
	if (u.u_error == 0) {
		putbits(in, 0);
		putbits(ou, 1);
		putbits(ex, 2);
#undef putbits
	}
}

unselect(p)
register struct proc *p;
{
	int s;

	s = spl7();
	p->p_compat &= ~SSTIMO;

	switch (p->p_stat) {

		case SSLEEP:
			setrun(p);
			break;

		case SSTOP:
			unsleep(p);
			break;

		case SRUN:
			break;

		default:
			printf("procp = %x, stat = %x\n", p, p->p_stat);
			panic("unselect : p_stat");
	}
	splx(s);
}

selscan(ibits, obits, nfd)
	fd_set *ibits, *obits;
{
	register int which, i, j;
	register fd_mask bits;
	int flag;
	struct file *fp;
	int n = 0;

	for (which = 0; which < 3; which++) {
		switch (which) {

		case 0:
			flag = FREAD; break;

		case 1:
			flag = FWRITE; break;

		case 2:
			flag = 0; break;
		}
		for (i = 0; i < nfd; i += NFDBITS) {
			bits = ibits[which].fds_bits[i/NFDBITS];
D 2
			while ((j = ffs(bits)) && i + --j < nfd) {
E 2
I 2
			while ((j = myffs(bits)) && i + --j < nfd) {
E 2
				bits &= ~(1 << j);
				fp = u.u_ofile[i + j];
				if (fp == NULL) {
					u.u_error = EBADF;
					break;
				}
				switch (fp->f_inode->i_ftype) {
					default:
						if (ino_select(fp, flag)) {
							FD_SET(i + j, 
								&obits[which]);
							n++;
						}
						break;
					case IFMPB:
						if (soo_select(fp, flag)) {
							FD_SET(i + j, 
								&obits[which]);
							n++;
						}
						break;
				}
			}
		}
	}
	return (n);
}

/*ARGSUSED*/
seltrue(dev, flag)
	dev_t dev;
	int flag;
{

	return (1);
}

selwakeup(p, coll)
	register struct proc *p;
	int coll;
{
	if (coll) {
		nselcoll++;
		wakeup((caddr_t)&selwait);
	}

	if (p) {
		int s = spl7();
		if (p->p_wchan == (caddr_t)&selwait) {

			if (p->p_stat != SSLEEP && p->p_stat != SSTOP) {
				printf("procp = %x, stat = %x\n", p, p->p_stat);
				panic("selwakeup : p_stat");
			}

			if (p->p_stat == SSLEEP)
				setrun(p);
			else
				unsleep(p);
		}
		else 	/* wakeup above did the work */
			p->p_compat &= ~SSSEL;
		splx(s);
	}
}

D 2
#if !m88k
ffs(mask)
E 2
I 2
myffs(mask)
E 2
	register long mask;
{
	register int i, n;

I 2
	if (mask == 0)
		return 0;

E 2
	n = sizeof(long)*8;

	for(i = 1; i < n; i++) {
		if (mask & 1)
			return (i);
		mask >>= 1;
	}
	return (0);
}
D 2
#endif
E 2

#include "conf.h"
#include "sysmacros.h"
#include "tty.h"
D 4
#include "sys/fs/s5inode.h"
E 4
I 4
#include "fs/s5inode.h"
E 4

ino_select(fp, flag)
struct file *fp;
{
	register struct inode *ip;
	register struct tty *tp;
	int maj, s;

	ip = fp->f_inode;
	switch (ip->i_ftype) {
		case IFCHR:
			maj = major(ip->i_rdev);
			if (tp = cdevsw[maj].d_ttys) {
				tp = &tp[minor(ip->i_rdev)];
				return(ttselect(tp, flag));
			}
			else if (cdevsw[maj].d_select) {
				s = spl7();
				maj = (*cdevsw[maj].d_select)(fp, ip, flag);
				splx(s);
				return (maj);
			}
			else
				return(1);

I 3
		case IFIFO:
			return (s5select(ip,  flag));	

E 3
		default:
			return (1);
	}
}
E 1
