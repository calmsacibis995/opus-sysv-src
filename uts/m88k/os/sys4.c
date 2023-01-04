/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/sys4.c	10.16"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/tuneable.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/immu.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/nami.h"
#include "sys/inode.h"
#include "sys/fstyp.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/var.h"
#include "sys/clock.h"
#include "sys/conf.h"

/*
 * Everything in this file is a routine implementing a system call.
 */


gtime()
{
	u.u_rtime = time;
}

stime()
{
	register struct a {
		time_t	time;
	} *uap;
	extern wtodc();

	uap = (struct a *)u.u_ap;
	if (suser()) {
		time = uap->time;
		wtodc(time);
		rem_date();
	}
}

setuid()
{
	register unsigned uid;
	register struct a {
		int	uid;
	} *uap;

	uap = (struct a *)u.u_ap;
	uid = uap->uid;
	if (uid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_uid  && (uid == u.u_ruid || uid == u.u_procp->p_suid))
		u.u_uid = uid;
	else if (suser()) {
		u.u_uid = uid;
		u.u_procp->p_uid = uid;
		u.u_procp->p_suid = uid;
		u.u_ruid = uid;
	}
}

getuid()
{

	u.u_rval1 = u.u_ruid;
	u.u_rval2 = u.u_uid;
}

setgid()
{
	register unsigned gid;
	register struct a {
		int	gid;
	} *uap;

	uap = (struct a *)u.u_ap;
	gid = uap->gid;
	if (gid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}

	if (u.u_uid && (gid == u.u_rgid || gid == u.u_procp->p_sgid))
		u.u_gid = gid;
	else if (suser()) {
		u.u_gid = gid;
		u.u_procp->p_sgid = gid;
		u.u_rgid = gid;
	}
}

getgid()
{

	u.u_rval1 = u.u_rgid;
	u.u_rval2 = u.u_gid;
}

getpid()
{
	u.u_rval1 = u.u_procp->p_pid;
	u.u_rval2 = u.u_procp->p_ppid;
}

setpgrp()
{
	register struct proc *p = u.u_procp;
	register struct a {
		int	flag;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (uap->flag) {
		if (p->p_pgrp != p->p_pid) {
			u.u_ttyp = NULL;
			u.u_ttyip = NULL;
		}
		p->p_pgrp = p->p_pid;
	}
	u.u_rval1 = p->p_pgrp;
}

sync()
{

	update();
}

nice()
{
	register n;
	register struct a {
		int	niceness;
	} *uap;
	int chgpri();

	uap = (struct a *)u.u_ap;
	n = uap->niceness;
	u.u_procp->p_nice = chgpri(u.u_procp->p_nice, n);
	u.u_rval1 = u.u_procp->p_nice - NZERO;
}

/*
 * Unlink system call.
 */
unlink()
{
	struct a {
		char	*fname;
	};
	struct argnamei nmarg;

	nmarg.cmd = NI_DEL;
	(void) namei(upath, &nmarg);
}

chdir()
{
	chdirec(&u.u_cdir);
}

chroot()
{
	if (!suser())
		return;
	chdirec(&u.u_rdir);
}

chdirec(ipp)
register struct inode **ipp;
{
	register struct inode *ip;
	struct a {
		char	*fname;
	};
	struct argnotify noarg;
	register flag;

	if ((ip = namei(upath, 0)) == NULL)
		return;
	if (ip->i_ftype != IFDIR) {
		u.u_error = ENOTDIR;
		goto bad;
	}
	if (FS_ACCESS(ip, ICDEXEC))
		goto bad;
	flag = (ipp == &u.u_rdir) ? NO_CHROOT : NO_CHDIR;
	if (fsinfo[ip->i_fstyp].fs_notify & flag) {
		noarg.cmd = flag;
		FS_NOTIFY(ip, &noarg);
	}
	if (u.u_error)
		goto bad;
	prele(ip);
	if (*ipp) {
		plock(*ipp);
		iput(*ipp);
	}
	*ipp = ip;
	return;

bad:
	iput(ip);
}

chmod()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;
	struct argnamei nmarg;

	uap = (struct a *)u.u_ap;
	if ((ip = namei(upath, 0)) == NULL)
		return;
	nmarg.cmd = NI_CHMOD;
	nmarg.mode = uap->fmode;
	if (FS_SETATTR(ip, &nmarg) == 0) {
		iput(ip);
		return;
	}
	ip->i_flag |= ICHG;
	iput(ip);
}

chown()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	uid;
		int	gid;
	} *uap;
	struct argnamei nmarg;

	uap = (struct a *)u.u_ap;
	if (((unsigned)uap->uid) > MAXUID || ((unsigned)uap->gid) > MAXUID) {
		/* posix */
		u.u_error = EINVAL;
		return;
	}
	if ((ip = namei(upath, 0)) == NULL)
		return;
	nmarg.cmd = NI_CHOWN;
	nmarg.uid = (ushort)uap->uid;
	nmarg.gid = (ushort)uap->gid;
	if (FS_SETATTR(ip, &nmarg) == 0) {
		iput(ip);
		return;
	}
	ip->i_flag |= ICHG;
	iput(ip);
}

/* Ssig() is the common entry for signal, sigset, sighold, sigrelse,
sigignore and sigpause */

ssig()
{
#if BCS
	u.u_error = ENOSYS;
#else
	register a;
	register struct proc *p;
	struct a {
		int	signo;
		void	(*fun)();
	} *uap;
	int mask;
	register off;

	uap = (struct a *)u.u_ap;
	a = uap->signo & SIGNO_MASK;
	if (a <= 0 || a >= NSIG || a == SIGKILL) {
		u.u_error = EINVAL;
		return;
	}
	mask = sigmask(a);
	off = sigoff(a);
	switch (uap->signo & ~SIGNO_MASK) {

	case SIGHOLD:	/* sighold */
		u.u_procp->p_hold.s[off] |= mask;
		u.u_rval1 = 0;
		return;

	case SIGRELSE:	/* sigrelse */
		u.u_procp->p_hold.s[off] &= ~mask;
		u.u_rval1 = 0;
		return;

	case SIGIGNORE:	/* signore */
		u.u_signal[a-1] = SIG_IGN;
		u.u_procp->p_sig.s[off] &= ~mask;
		u.u_procp->p_hold.s[off] &= ~mask;
		u.u_rval1 = 0;
		return;

	case SIGPAUSE:	/* sigpause */
		u.u_procp->p_hold.s[off] &= ~mask;
		u.u_rval1 = 0;
		pause();
		/* not reached */

	case SIGDEFER:		/* sigset */
		if (uap->fun != SIG_DFL && uap->fun != SIG_IGN
			&& uap->fun != SIG_HOLD)
			u.u_procp->p_chold.s[off] |= mask;
		if (u.u_procp->p_hold.s[off] & mask)
			u.u_rval1 = (int) SIG_HOLD;
		else
			u.u_rval1 = (int) u.u_signal[a-1];
		if (uap->fun == SIG_HOLD) {
			u.u_procp->p_hold.s[off] |= mask;
		} else {
			u.u_signal[a-1] = uap->fun;
			u.u_procp->p_hold.s[off] &= ~mask;
		}
		u.u_sigmask[a-1].s[0] = 0;
		u.u_sigmask[a-1].s[1] = 0;
		break;

	case 0:	/* signal */
		u.u_procp->p_chold.s[off] &= ~mask;
		u.u_rval1 = (int) u.u_signal[a-1];
		u.u_signal[a-1] = uap->fun;
		/* p_sig only cleared in signal, not in sigset */
		u.u_procp->p_sig.s[off] &= ~mask;
		u.u_sigmask[a-1].s[0] = 0;
		u.u_sigmask[a-1].s[1] = 0;
		break;

	default:		/* error */
		u.u_error = EINVAL;
		return;
	}
	
	if (a == SIGCLD) {
		a = u.u_procp->p_pid;
		for (p = u.u_procp->p_child ; p ; p = p->p_sibling) {
			if (p->p_stat == SZOMB)
				psignal(u.u_procp, SIGCLD);
		}
	}
#endif
}

kill()
{
	register struct proc *p, *q;
	register arg;
	register struct a {
		int	pid;
		int	signo;
	} *uap;
	int f;

	uap = (struct a *)u.u_ap;
	if (uap->signo < 0 || uap->signo >= NSIG ||
	    (uap->signo && !_validsig(uap->signo))) {
		u.u_error = EINVAL;
		return;
	}
	f = 0;
	arg = uap->pid;
	if (arg > 0)
		p = &proc[1];
	else
		p = &proc[2];
	q = u.u_procp;
	if (arg == 0 && q->p_pgrp == 0) {
		u.u_error = ESRCH;
		return;
	}
	for (; p < (struct proc *)v.ve_proc; p++) {
		if (p->p_stat == NULL)
			continue;
		if (arg > 0 && p->p_pid != arg)
			continue;
		if (arg == 0 && p->p_pgrp != q->p_pgrp)
			continue;
		if (arg < -1 && p->p_pgrp != -arg)
			continue;
		if ((!(u.u_uid == 0 ||
			u.u_uid == p->p_uid ||
			u.u_ruid == p->p_uid ||
			u.u_uid == p->p_suid ||
			u.u_ruid == p->p_suid)) ||
			((p == &proc[1]) && (uap->signo == SIGKILL)))
			if (arg > 0) {
				u.u_error = EPERM;
				return;
			} else
				continue;
		f++;
		if (uap->signo) {
			p->p_flag |= SKILL;
			psignal(p, uap->signo);
		}
		if (arg > 0)
			break;
	}
	if (f == 0)
		u.u_error = ESRCH;
}

times()
{
	register struct a {
		time_t	(*times)[4];
	} *uap;

	uap = (struct a *)u.u_ap;
	if (copyout((caddr_t)&u.u_utime, (caddr_t)uap->times, sizeof(*uap->times)))
		u.u_error = EFAULT;
	spl7();
	u.u_rtime = lbolt;
	spl0();
}

/*
 * Profiling
 */

profil()
{
	register struct a {
		short	*bufbase;
		unsigned bufsize;
		unsigned pcoffset;
		unsigned pcscale;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_prof.pr_base = uap->bufbase;
	u.u_prof.pr_size = uap->bufsize;
	u.u_prof.pr_off = uap->pcoffset;
	u.u_prof.pr_scale = uap->pcscale;
	if(uap->pcscale == 1){       /*  profiling should be off for 1 */
		u.u_prof.pr_scale = 0;
	}
}

/*
 * alarm clock signal
 */
alarm()
{
#if BCS
	u.u_error = ENOSYS;
#else
	extern time_t one_sec;
	register struct proc *p;
	register c;
	register struct a {
		int	deltat;
	} *uap;

	uap = (struct a *)u.u_ap;
	p = u.u_procp;
	spl7();
	c = p->p_clktim;
	p->p_clktim = uap->deltat;
	if (p->p_clktim == 1 && one_sec < (HZ/2))
		p->p_clktim = 2;
	spl0();
	u.u_rval1 = c;
#endif
}

/*
 * indefinite wait.
 * no one should wakeup(&u)
 */
pause()
{
#if BCS
	u.u_error = ENOSYS;
#else
	register struct proc *p;

	p = u.u_procp;
	u.u_oldsig = p->p_hold;
	p->p_flag |= SOMASK;
	clrsig(p, p_hold);
	for (;;)
		sleep((caddr_t)&u, PSLEP);
#endif
}

/*
 * mode mask for creation of files
 */
umask()
{
	register struct a {
		int	mask;
	} *uap;
	register t;

	uap = (struct a *)u.u_ap;
	t = u.u_cmask;
	u.u_cmask = uap->mask & PERMMSK;
	u.u_rval1 = t;
}

/*
 * Set IUPD and IACC times on file.
 */
utime()
{
	register struct a {
		char	*fname;
		time_t	*tptr;
	} *uap;
	register struct inode *ip;
	time_t tv[4];

	uap = (struct a *)u.u_ap;
	if (uap->tptr != NULL) {
		if (server())	/* server already copied time in */
			bcopy((caddr_t)uap->tptr, (caddr_t)tv, sizeof(tv));
		else
			if (copyin((caddr_t)uap->tptr,(caddr_t)tv, sizeof(tv))) {
				u.u_error = EFAULT;
				return;
			}
	} else {
		tv[0] = tv[2] = time;
		tv[1] = tv[3] = 0;
	}
	if (tv[1] < 0 || tv[1] >= 1000000 || tv[3] < 0 || tv[3] >= 1000000) {
		u.u_error = EINVAL;
		return;
	}
	if ((ip = namei(upath, 0)) == NULL)
		return;
	if (u.u_uid != ip->i_uid && u.u_uid != 0) {
		if (uap->tptr != NULL)
			u.u_error = EPERM;
		else
			FS_ACCESS(ip, IWRITE);
	}
	if (u.u_error == 0) {
		ip->i_flag |= IACC|IUPD|ICHG;
		FS_IUPDAT(ip, &tv[0], &tv[2]);
	}
	iput(ip);
}

ulimit()
{
	register struct a {
		int	cmd;
		long	arg;
	} *uap;

	uap = (struct a *)u.u_ap;
	switch (uap->cmd) {

	case 2:	/* Set new file size limit. */
		if (uap->arg > u.u_limit && !suser())
			return;
		u.u_limit = uap->arg;

	case 1:	/* Return current file size limit. */
		u.u_roff = u.u_limit;
		break;

	case 3:	/* Return maximum possible break value. */
		{
		register preg_t	*prp, *dprp, *prp2;

		/*	Find the data region
		 */

		if ((dprp = findpreg(u.u_procp, PT_DATA)) == NULL)
			u.u_roff = 0;
		else {
			/*	Now find the region with a virtual
			 *	address greater than the data region
			 *	but lower than any other region
			 */
			register int size;
			register reg_t *rp;
	
			prp = u.u_procp->p_region;
	
			size = 0;
			while (rp = prp->p_reg) {
				size += rp->r_pgsz;
				prp++;
			}

			size = ctob(tune.t_maxumem - size);

			prp2 = NULL;
			for (prp = u.u_procp->p_region; prp->p_reg; prp++) {
				if (prp->p_regva <= dprp->p_regva)
					continue;
				if (prp2==NULL || prp->p_regva < prp2->p_regva)
					prp2 = prp;
			}
			u.u_roff =(off_t)(dprp->p_regva + size +
			     ctob(dprp->p_reg->r_pgsz + dprp->p_reg->r_pgoff));
			if (prp2 != NULL) {
				/*
				min is signed, need an unsigned min here.
				u.u_roff = min(u.u_roff, (off_t)prp2->p_regva);
				*/
				if (prp2->p_regva < (caddr_t) u.u_roff) 
					u.u_roff = (off_t) prp2->p_regva;
			}
					
		}
		break;
	}
	case 4:	/* Return configured value of NOFILES. */
		u.u_roff = v.v_nofiles;
		break;

	default:
		u.u_error = EINVAL;
	}
}
