h59275
s 00001/00001/01298
d D 1.12 91/04/16 11:30:16 root 12 11
c Don't allow a user to change the PSR_BO bit.
e
s 00002/00002/01297
d D 1.11 91/02/19 14:16:18 root 11 10
c fixed sysconf(_SC_SAVED_IDS) and updated BCS revision level
e
s 00009/00003/01290
d D 1.10 90/06/26 00:12:43 root 10 9
c allow renames on IFCHR and IFBLK.
e
s 00003/00000/01290
d D 1.9 90/06/25 20:25:58 root 9 8
c added nosys if XBus is not defined
e
s 00006/00001/01284
d D 1.8 90/05/31 20:14:07 root 8 7
c added hook for vendor functions
e
s 00007/00003/01278
d D 1.7 90/05/08 14:54:40 root 7 6
c itimerfixhas a max of a 100Million Seconds. Stat does not zero the
e
s 00002/00002/01279
d D 1.6 90/04/18 13:47:26 root 6 5
c renamed kmsgread to prfsys
e
s 00002/00000/01279
d D 1.5 90/04/05 16:27:49 root 5 4
c added kmesgread
e
s 00002/00000/01277
d D 1.4 90/04/03 16:29:41 root 4 3
c added function to set initlevel to single user
e
s 00002/00005/01275
d D 1.3 90/03/15 18:25:32 root 3 2
c zero the entire stat buffer
e
s 00001/00001/01279
d D 1.2 90/03/14 15:19:47 root 2 1
c we are not _POSIX_CHOWN_RESTRICTED
e
s 01280/00000/00000
d D 1.1 90/03/06 12:27:53 root 1 0
c date and time created 90/03/06 12:27:53 by root
e
u
U
t
T
I 1
#include "sys/debug.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/pfdat.h"
#include "sys/region.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/fstyp.h"
#include "sys/inode.h"
#include "sys/fs/s5inode.h"
#include "sys/nami.h"
#include "sys/file.h"
#include "sys/psw.h"
#include "sys/m88kbcs.h"
#include "sys/reg.h"
#include "sys/time.h"
#include "sys/var.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "sys/sem.h"
#include "sys/shm.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "sys/utsname.h"
#include "sys/cmn_err.h"

#ifndef OPUSVENDORID
#define	OPUSVENDORID	0xf00
#endif

unamefunc()
{
	register struct a {
		int	cbuf;
		int	mv;
		int	type;
	} *uap;
	uap = (struct a *)u.u_ap;

	uap->type = 0;		/* uname */
	utssys();
}

ustatfunc()
{
	register struct a {
		int	cbuf;
		int	mv;
		int	type;
	} *uap;
	uap = (struct a *)u.u_ap;

	uap->type = uap->cbuf;	/* utssys expects these backward */
	uap->cbuf = uap->mv;
	uap->mv = uap->type;

	uap->type = 3;		/* new ustat */
	utssys();
}

getpgrp()
{
	u.u_rval1 = u.u_procp->p_pgrp;
}

setsid()
{
	register struct a {
		int	flag;
	} *uap;
	register struct proc *p;

	p = u.u_procp;
	if (p->p_pgrp == p->p_pid) {
		u.u_error = EPERM;
		return;
	}

	uap = (struct a *)u.u_ap;
	uap->flag = 1;

	setpgrp();
}

getpsr()
{
	u.u_rval1 = psrcom(0, 0);
}

setpsr()
{
	register struct a {
		int	psr;
	} *uap;
	uap = (struct a *)u.u_ap;
	u.u_rval1 = psrcom(1, uap->psr);
}

D 12
#define	PSRMASK	(PSR_C|PSR_MXM|PSR_BO)
E 12
I 12
#define	PSRMASK	(PSR_C|PSR_MXM)
E 12
psrcom(set, psr)
register unsigned long set, psr;
{
	register unsigned long retval;
	register struct proc *p;

	p = u.u_procp;

	retval = u.u_ar0[PS];
	if (p->p_compat & SSERIAL)
		retval |= PSR_SER;

	if (set) {
		u.u_ar0[PS] = (u.u_ar0[PS] & ~PSRMASK) | (psr & PSRMASK);
		if (psr & PSR_SER)
			p->p_compat |= SSERIAL;
		else
			p->p_compat &= ~SSERIAL;
	}
	return retval;
}

extern time_t one_sec;

getitimer()
{
	register struct a {
		int	which;
		struct itimerval *value;
	} *uap;
	register struct proc *p = u.u_procp;
	register s;
	struct itimerval l;

	uap = (struct a *)u.u_ap;
	if (uap->which != ITIMER_REAL) {
		u.u_error = EINVAL;
		return;
	}
	s = spl7();
	l = p->p_itimer;
	splx(s);
	if (copyout ((caddr_t) &l, (caddr_t) uap->value, 
			sizeof(struct itimerval))) 
		u.u_error = EFAULT;
}

setitimer()
{
	register struct a {
		int	which;
		struct itimerval *value;
		struct itimerval *ovalue;
	} *uap;
	register struct proc *p = u.u_procp;
	struct itimerval old, new;
	register int s;

	uap = (struct a *)u.u_ap;
	if (uap->which != ITIMER_REAL) {
		u.u_error = EINVAL;
		return;
	}
	if (copyin((caddr_t) uap->value, (caddr_t) &new, 
		sizeof (struct itimerval))) {
		u.u_error = EFAULT;
		return;
	}
	if (begintrace(92))
		printf("setitimer (it_value %d, %d , it_interval %d, %d)\n", 
			new.it_value.tv_sec, new.it_value.tv_usec,
			new.it_interval.tv_sec, new.it_interval.tv_usec);

	s = itimerfix(&new.it_value);
	if (!s)
		s = itimerfix(&new.it_interval);

	if (s) {
		u.u_error = EINVAL;
		return;
	}

	s = spl7();
	old = p->p_itimer;
	p->p_itimer = new;
	scanitimer();
	splx(s);

	if (uap->ovalue) {
		if (copyout ((caddr_t) &old, (caddr_t) uap->ovalue, 
				sizeof(struct itimerval))) 
			u.u_error = EFAULT;
	}
}

/*
 * Check that a proposed value to load into the .it_value or
 * .it_interval part of an interval timer is acceptable, and
 * fix it to have at least minimal value (i.e. if it is less
 * than the resolution of the clock, round it up.)
 */

itimerfix(tv)
register struct timeval *tv;
{

D 7
	if (tv->tv_sec < 0 || tv->tv_usec < 0 || tv->tv_usec >= 1000000) 
E 7
I 7
	if (tv->tv_sec < 0 || tv->tv_sec > 100000000 ||
			tv->tv_usec < 0 || tv->tv_usec >= 1000000) 
E 7
		return (1);
	if (tv->tv_usec) {
		tv->tv_usec = timerround(tv->tv_usec);
		if (tv->tv_usec >= timerround(1000000))  {
			tv->tv_usec -= timerround(1000000);
			++tv->tv_sec;
		}
	}
	return (0);
}

#define	OLDWNOHANG	1
/* copied wait code */
waitpid()
{
	register struct a {
		int	pid;
		int	*loc;
		int	options;
	} *uap = (struct a *)u.u_ap;
	register opts;
	register struct proc *p;
	register pgrp;
	register found;

	opts = uap->options;

	if (opts & ~(WNOHANG|WUNTRACED|OLDWNOHANG)) {
		u.u_error = EINVAL;
		return;
	}
	if (opts & OLDWNOHANG)
		opts |= WNOHANG;

loop:
	found = 0;
	for (p = u.u_procp->p_child ; p ; p = p->p_sibling) {
		pgrp = (p->p_stat == SZOMB) ? p->p_sgid : p->p_pgrp;
		if (uap->pid == -1)
			found++;
		else if (uap->pid > 0) {
			if (p->p_pid != uap->pid)
				continue;
			else
				found++;
		}
		else if (uap->pid == 0) {
			if (pgrp != u.u_procp->p_pgrp)
				continue;
			else
				found++;
		}
		else /* if (uap->pid < -1) */ {
			if (pgrp != ( - uap->pid  ))
				continue;
			else
				found++;
		}
		if (p->p_stat == SZOMB) {
			freeproc(p, 1);
			return;
		}
		if (p->p_stat == SSTOP) {
			if (((p->p_flag & STRC) || (opts & WUNTRACED)) &&
				 (p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_rval1 = p->p_pid;
				u.u_rval2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if (found) {
		if (opts & WNOHANG) {
			u.u_rval1 = 0;
			return;
		}
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

getgroups()
{
	u.u_rval1 = 0;		/* NGROUPS == 0 */
}

/*
 * the fpathconf system call.
 */
fpathconf()
{
	struct file *getf();
	register struct file *fp;
	register struct a {
		int	fdes;
		int	name;
	} *uap;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fdes)) == NULL)
		return;
	pathconfcomm();
}

/*
 * the pathconf system call.
 */
pathconf()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	name;
	} *uap;

	uap = (struct a *)u.u_ap;
	if ((ip = namei(upath, 0)) == NULL)
		return;
	iput(ip);
	pathconfcomm();
}

pathconfcomm()
{
	register i;
	register struct a {
		int	fdorpath;	
		int	name;
	} *uap;

	uap = (struct a *)u.u_ap;
	i = uap->name;
	switch(i) {
		case 1:	/* PC_LINK_MAX */
			i = MAXLINK;
			break;
		case 2:	/* PC_MAX_CANON */
		case 3: /* PC_MAX_INPUT */
			i = CANBSIZ;
			break;
		case 4:	/* PC_NAME_MAX */
			i = DIRSIZ;
			break;
		case 5:	/* PC_PATH_MAX */
			i = 511;	/* from nami.c */
			break;
		case 6:	/* PC_PIPE_BUF */
			i = PIPSIZ;
			break;
		case 7:	/* PC_CHOWN_RESTRICTED */
D 2
			i = 1;		/* yes */
E 2
I 2
			i = -1;		/* no */
E 2
			break;
		case 8:	/* PC_NO_TRUNC */
		case 9:	/* PC_VDISABLE */
			i = -1;		/* not defined */
			break;
		case 10: /* PC_BLKSIZE */
			i = 0;		/* indeterminate */
			break;
		default:
			u.u_error = EINVAL;
			return;
	}
	u.u_rval1 = i;
}

memctl()
{
	register i;
	register struct a {
		char 	*base;
		int	length;	
		int	state;
	} *uap;

	uap = (struct a *)u.u_ap;

	dcacheflush(0);
	ccacheflush(0);

	switch (uap->state) {
		case 1:
		case 2:
		case 3:
			if ((((int) uap->base & (NBPC-1)) == 0) &&
				((uap->length & (NBPC-1)) == 0))
				break;
			/* fall thru */
		default:
			u.u_error = EINVAL;
			return;
	}
	if (vtopreg(u.u_procp, uap->base) == NULL) {
		u.u_error = EFAULT;
		return;
	}
}

setpgid()
{
	u.u_error = ENOSYS;
}

extern struct shminfo	shminfo;	/* shared memory info structure */
extern struct seminfo seminfo;		/* param information structure */
extern struct msginfo	msginfo;	/* message parameters */

sysconf()
{
	register struct a {
		int	name;
	} *uap = (struct a *)u.u_ap;
	register i;

	i = uap->name;
	switch(i) {
		case 1:
			i = NCARGS - 1;
			break;
		case 2:
		case 13:
			i = v.v_maxup;
			break;
		case 3:
			i = HZ;
			break;
		case 4:
			i = 0;
			break;
		/*
		 * POSIX:
		 * If the option is not available return
		 * -1 without changing errno.
		 */
		case 6:
I 11
		case 7:		/* saved-set-uid/gid */
E 11
		case 24:
		case 25:
		case 32:
			i = -1;
			break;
		case 5:
			i = v.v_nofiles;
			break;
		case 8:
			i = 198808L;
			break;
		case 9:
D 11
			i = 198902L;
E 11
I 11
			i = 199004L;
E 11
			break;
		case 10:
			i = OPUSVENDORID;
			break;
		case 12:
			i = ((unsigned) USRSTACK)/1024;
			break;
		case 14:
			i = msginfo.msgmax;
			break;
		case 15:
			i = msginfo.msgtql;
			break;
		case 16:
			i = shminfo.shmmax;
			break;
		case 17:
			i = shminfo.shmmin;
			break;
		case 18:
			i = shminfo.shmseg;
			break;
		case 19:
			i = seminfo.semmns;
			break;
		case 20:
			i = seminfo.semvmx;
			break;
		case 21:
			i = seminfo.semmap;
			break;
		case 22:
			i = seminfo.semmsl;
			break;
		case 23:
			i = shminfo.shmmni;
			break;
		case 27:
			i = physmem*(NBPC/1024);
			break;

		case 28:
			i = maxmem*(NBPC/1024);
			break;
D 11
		case 7:
E 11
		case 29:
		case 34:	/* _SC_BSDNETWORK */
			i = 1;
			break;
		case 30:
			i = NBPC;
			break;
		case 31:
			i = shmlba();
			break;
		case 33:
			i = getcr0();
			break;
		case 35:	/* _SC_NPTYS */
			i = npttys();
			break;
		case 26:
			i = USEC_PER_TICK;
			break;
		case 11:
			i = cptgetid();
			if (u.u_error == 0)
				break;
			/* fall thru */
		default:
			u.u_error = EINVAL;
			return;
	}
	u.u_rval1 = i;
}

/*
 * the fstat system call.
 */
nfstat()
{
	struct file *getf();
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		struct stat *sb;
	} *uap;
	struct ostat local;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fdes)) == NULL)
		return;
	plock(ip = fp->f_inode);
	stat1(ip, &local, 0);
	prele(ip);
	statcopyout(&local, uap->sb);
}

/*
 * the stat system call.
 */
nstat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		struct stat *sb;
	} *uap;
	struct ostat local;

	uap = (struct a *)u.u_ap;
	if ((ip = namei(upath, 0)) == NULL)
		return;
	stat1(ip, &local, 0);
	iput(ip);
	statcopyout(&local, uap->sb);
}

statcopyout(src, ub)
register struct ostat *src;
caddr_t ub;
{
	register struct stat *dst;
	struct stat ns;

	dst = &ns;
I 3
D 7
	bzero(dst, sizeof (ns));
E 7
E 3
	dst->st_dev = (ushort) src->st_dev;
	dst->st_ino = (ushort) src->st_ino;
	dst->st_mode = (ushort) src->st_mode;
	dst->st_nlink = (ushort) src->st_nlink;
	dst->st_uid = (ushort) src->st_uid;
	dst->st_gid = (ushort) src->st_gid;
	dst->st_rdev = (ushort) src->st_rdev;
	dst->st_size = src->st_size;
	dst->st_atime = src->st_atime;
	dst->st_mtime = src->st_mtime;
	dst->st_ctime = src->st_ctime;
D 3
	dst->st_ausec = 0;
	dst->st_musec = 0;
	dst->st_cusec = 0;
	if (copyout((caddr_t)dst, (caddr_t)ub, 
			sizeof(ns) - sizeof(ns.st_padding)) < 0)
E 3
I 3
D 7
	if (copyout((caddr_t)dst, (caddr_t)ub, sizeof(ns)) < 0)
E 7
I 7
	dst->st_ausec = 0;
	dst->st_musec = 0;
	dst->st_cusec = 0;
	if (copyout((caddr_t)dst, (caddr_t)ub, 
			sizeof(ns) - sizeof(ns.st_padding)) < 0)
E 7
E 3
		u.u_error = EFAULT;
}


#define	cantmask	(sigmask(SIGKILL))


sigset_t validsigmask = {
  sigmask(SIGHUP) | sigmask(SIGINT) | sigmask(SIGQUIT) | sigmask(SIGILL)
| sigmask(SIGTRAP) | sigmask(SIGABRT) | sigmask(SIGFPE) | sigmask(SIGKILL)
| sigmask(SIGBUS) | sigmask(SIGSEGV) | sigmask(SIGSYS) | sigmask(SIGPIPE)
| sigmask(SIGALRM) | sigmask(SIGTERM) | sigmask(SIGUSR1) | sigmask(SIGUSR2)
| sigmask(SIGCLD) | sigmask(SIGPWR) | sigmask(SIGWINCH) | sigmask(SIGEMT) 
| sigmask(SIGPOLL) ,
  sigmask(SIGURG) | sigmask(SIGIO)  } ;

#define validsig(sig) ((kdebug & 256) | (validsigmask.s[sigoff(sig)] & sigmask(sig)))

_validsig(signo)
register int signo;
{
	return(validsig(signo));
}

/*
 * Generalized interface signal handler.
 */
sigaction()
{
	register struct a {
		int	signo;
		struct	sigaction *nsa;
		struct	sigaction *osa;
	} *uap = (struct a  *)u.u_ap;
	struct sigaction vec;
	register struct sigaction *sa;
	register int sig;
	int bit;

	sig = uap->signo;
	if (sig <= 0 || sig >= NSIG || !validsig(sig)) {
		u.u_error = EINVAL;
		return;
	}
	sa = &vec;
	if (uap->osa) {
		sa->sa_handler = u.u_signal[sig-1];
		sa->sa_mask = u.u_sigmask[sig-1];
		sa->sa_flags = 0;
		if (sig == SIGCLD) {
			if (sa->sa_flags = (u.u_nocld0 & SA_NOCLDSTOP)) {
				sa->sa_handler = u.u_nocld1;
			}
		}
		if (copyout((caddr_t)sa, (caddr_t)uap->osa, sizeof (vec))) {
			u.u_error = EFAULT;
			return;
		}
	}
	if (uap->nsa) {
		if (sig == SIGKILL) {
			u.u_error = EINVAL;
			return;
		}
		if (copyin((caddr_t)uap->nsa, (caddr_t)sa, sizeof (vec))) {
			u.u_error = EFAULT;
			return;
		}
		setsigvec(sig, sa);
	}
}

setsigvec(sig, sa)
register int sig;
register struct sigaction *sa;
{
	register struct proc *p;
	register int bit, off;

	bit = sigmask(sig);
	off = sigoff(sig);
	p = u.u_procp;

	if (begintrace(103))
		printf("sigaction sig = %d, handler = %x, mask = %x, %x\n",
			sig, sa->sa_handler,sa->sa_mask.s[0],sa->sa_mask.s[1]);

	if (sig == SIGCLD) {
		if ((sa->sa_flags&SA_NOCLDSTOP) && sa->sa_handler == SIG_DFL) {
			u.u_nocld0 = SA_NOCLDSTOP;
			u.u_nocld1 = sa->sa_handler;
			sa->sa_handler = SIG_IGN;
		} else {
			u.u_nocld0 = 0;
		}
	}

	/*
	 * Change setting atomically.
	 */
	(void) splhigh();
	u.u_signal[sig-1] = sa->sa_handler;
	u.u_sigmask[sig-1].s[0] = sa->sa_mask.s[0] & ~cantmask;
	u.u_sigmask[sig-1].s[1] = sa->sa_mask.s[1];
	/* treat p_sig as as bsd sys/kern_sig.c setsigvec() does */
	if (sa->sa_handler == SIG_IGN || 
			(sig == SIGCLD && sa->sa_handler == SIG_DFL)) {
		p->p_sig.s[off] &= ~bit;
		p->p_hold.s[off] &= ~bit;
	}
	if (sa->sa_handler != SIG_IGN && sa->sa_handler != SIG_DFL) {
		p->p_chold.s[off] |= bit;
	}
	(void) spl0();
	if (sig == SIGCLD) {
		for (p = u.u_procp->p_child ; p ; p = p->p_sibling) {
			if (p->p_stat == SZOMB)
				psignal(u.u_procp, SIGCLD);
		}
	}
}

sigprocmask()
{
	struct a {
		int	how;
		sigset_t	*set;
		sigset_t	*oset;
	} *uap = (struct a *)u.u_ap;
	register struct proc *p = u.u_procp;
	sigset_t new;

	if (uap->oset) {
		if (copyout((caddr_t) &p->p_hold, uap->oset, 
				sizeof(p->p_hold))) {
			u.u_error = EFAULT;
			return;
		}
	}
	if (uap->set) {
		if (copyin((caddr_t) uap->set, &new, sizeof(new))) {
			u.u_error = EFAULT;
			return;
		}

		if (begintrace(104))
			printf("sigprocmask(how = %d, new mask = %x, %x)\n", 
				uap->how, new.s[0], new.s[1]);

		new.s[0] &= ~cantmask;
		switch (uap->how) {
			case 0:		/* SIG_BLOCK */
				p->p_hold.s[0] |= new.s[0];
				p->p_hold.s[1] |= new.s[1];
				break;
			case 1:		/* SIG_UNBLOCK */
				p->p_hold.s[0] &= ~new.s[0];
				p->p_hold.s[1] &= ~new.s[1];
				break;
			case 2:		/* SIG_SETMASK */
				p->p_hold = new;
				break;
			default:
				u.u_error = EINVAL;
				return;
		}
	}
}

sigpending()
{
	struct a {
		sigset_t 	*set;
	} *uap = (struct a *)u.u_ap;
	register struct proc *p = u.u_procp;
	sigset_t pend;

	pend.s[0] = p->p_sig.s[0] & p->p_hold.s[0];
	pend.s[1] = p->p_sig.s[1] & p->p_hold.s[1];
	if (copyout(&pend, uap->set, sizeof(pend)))
		u.u_error = EFAULT;
}

sigsuspend()
{
	struct a {
		sigset_t *set;
	} *uap = (struct a *)u.u_ap;
	register struct proc *p = u.u_procp;
	sigset_t mask;

	if (copyin((caddr_t) uap->set, (caddr_t) &mask, sizeof(mask))) {
		u.u_error = EFAULT;
		return;
	}

	if (begintrace(106))
		printf("sigsuspend(mask = %x, %x)\n", mask.s[0], mask.s[1]);

	/*
	 * When returning from sigpause, we want
	 * the old mask to be restored after the
	 * signal handler has finished.  Thus, we
	 * save it here and mark the proc structure
	 * to indicate this (should be in u.).
	 */
	u.u_oldsig = p->p_hold;
	p->p_flag |= SOMASK;
	p->p_hold.s[0] = mask.s[0] & ~cantmask;
	p->p_hold.s[1] = mask.s[1];
	for (;;)
		sleep((caddr_t)&u, PSLEP);
	/*NOTREACHED*/
}

sigfillset()
{
	struct a {
		sigset_t *set;
	} *uap = (struct a *)u.u_ap;

	if (copyout((caddr_t) &validsigmask, uap->set, sizeof(validsigmask)))
		u.u_error = EFAULT;
}

sigret()
{
	register i, *regs, *usp;
	register struct fltinfo *fltptr;
	register struct proc *p;
	sigset_t retmask;

	p = u.u_procp;
	regs = u.u_ar0;
	usp = (int *) regs[SP];

	/* get resume kind */
	u.u_traptype = regs[4]; /* make it appear that we just got here */
			     /* for the first time for psignal */

	regs[R31] = fuword((caddr_t) (usp));
	regs[SFIP] = fuword((caddr_t) (usp+4)); 
	regs[SNIP] = fuword((caddr_t) (usp+3)); 
	if (newsig() == 0) {
	/* make sure we don't allow transfer to supervisor mode */
	regs[PS] = (regs[PS] & ~PSR_C) |
		   (fuword((caddr_t) (usp+17)) & PSR_C);
	regs[R4] = fuword((caddr_t) (usp+13));
	regs[R3] = fuword((caddr_t) (usp+14)); 
	regs[R2] = fuword((caddr_t) (usp+15));
	regs[R1] = fuword((caddr_t) (usp+16));
	retmask.s[0] = fuword((caddr_t) (usp+18));
	retmask.s[1] = fuword((caddr_t) (usp+19));
	}
	else {
	retmask.s[0] = fuword((caddr_t) (usp+13));
	retmask.s[1] = fuword((caddr_t) (usp+14));
	}
	retmask.s[0] &= ~cantmask;
	p->p_hold = retmask;

	fltptr = (struct fltinfo *)u.u_trapsave;
	for(i = 0; i < NUMTRAN; i++,fltptr++){
		if(fltptr->tran & TRN_VALID){
			if(!fault(fltptr->addr,p,fltptr->tran & TRN_RW, i))
				dotran(regs,fltptr);
			else{
				procname(0);
				printf("PC = %x\n", regs[SCIP]);
				printf("pid %d SIGKILL: S%d:T%x:A%x:D%x\n", 
					p->p_pid, i,
					fltptr->tran,
					fltptr->addr,
					fltptr->data);
				psignal(p,SIGKILL);
				psig(fltptr->addr);
				return;
			}
		}
	}
	/* keep syscall happy */
	u.u_rval2 = regs[R3]; 
	u.u_rval1 = regs[R2];
}

extern smount(), sumount();
extern fstatfs(), statfs(), sysfs();
extern syslocal(), stime(), uadmin();
extern sysacct(), softnetint(), bsdpipe();
extern stty(), gtty(), netreset(), dumptty();
#if XBus
extern xbus_ctl();
#endif
I 4
extern initlevel();
I 5
D 6
extern kmsgread();
E 6
I 6
extern prfsys();
I 9
extern nosys();
E 9
E 6
E 5
E 4

int (*sys_localsw[])() = 
{
	0,
	smount, 
	sumount,
	fstatfs,
	statfs,
	sysfs,
	syslocal,
	stime,
	uadmin,
	sysacct,
	softnetint,
	bsdpipe,
	gtty,
	stty,
	netreset,
	dumptty,
#if XBus
	xbus_ctl,
I 9
#else
	nosys,
E 9
#endif
I 4
	initlevel,
I 5
D 6
	kmsgread,
E 6
I 6
	prfsys,
E 6
E 5
E 4
};

sys_local()
{
	register unsigned vendor, func;

	vendor = u.u_ar0[R8];
	func = u.u_ar0[R9];
	if (vendor == 1) {
		/* bcs socket functions */
		berkfunc(func);
		return;
	}
I 8
	if (vendor == 2) {	
		/* symbolic links */
		u.u_error = ENOSYS;
		return;
	}
E 8
	if (vendor == OPUSVENDORID) {
		if (func > 0 && func < (sizeof (sys_localsw) / sizeof(int))) {
			(*sys_localsw[func])();
			return;
		}
	}
D 8
	u.u_error = ENOSYS;
E 8
I 8
	vendorfunc(vendor, func);	/* anybody else */
E 8
}

microtime(tmp)
register struct timeval *tmp;
{
	register s;
	register ticks;
	extern time_t time;
	extern time_t one_sec;	/* ticks left in current second */
				/* normally 1..HZ; can go <0 */

	s = spl7();
	tmp->tv_sec = time;
	ticks = HZ - one_sec;
	splx(s);
	while (ticks >= HZ) {
		ticks -= HZ;
		tmp->tv_sec++;
	}
	tmp->tv_usec = ticks * USEC_PER_TICK;
}

/* posix get time */
pxgtime()
{
	microtime((struct timeval *) &u.u_rtime);
}

enosys()
{
	u.u_error = ENOSYS;
}

rename()
{
	register struct a {
		char	*old;
		char 	*new;
	} *uap;
	register struct inode *nip;
	register struct inode *oip;
	register doingdirs;
	int oldparent, newparent;
	struct argnamei nmarg;
	struct direct dir;
	int segflg, error;
	extern short s5fstyp;
	struct mount *mntdev;

	uap = (struct a *)u.u_ap;
	error = 0;

	if ((oip = namei(upath, 0)) == NULL)
		return;

	if (oip->i_fstyp != s5fstyp) {
		error = EINVAL;
		goto iputoip;
	}

D 10
	if (oip->i_ftype != IFDIR && oip->i_ftype != IFREG)  {
		error = EINVAL;
		goto iputoip;
E 10
I 10
	switch (oip->i_ftype) {
		case IFDIR:
		case IFREG:
		case IFBLK:
		case IFCHR:
			break;
		default:
			error = EINVAL;
			goto iputoip;
E 10
	}

	mntdev = oip->i_mntdev;
	doingdirs = oip->i_ftype == IFDIR;

	if (doingdirs && s5access(oip, IWRITE))
		goto iputoip;

	if (s5access(u.u_pdir, IWRITE))
		goto iputoip;

	if (doingdirs && oip->i_number == S5ROOTINO) {
		error = EINVAL;
		goto iputoip;
	}

	if (doingdirs && (checkdots(uap->old) || checkdots(uap->new))) {
		if (u.u_error == 0)
			error = EINVAL;
		goto iputoip;
	}
	oldparent = u.u_pdir->i_number;

	prele(oip);
	u.u_dirp = (caddr_t)uap->new;
	u.u_pdir = (struct inode *) 0;
	nip = namei(upath, 0);
	if (nip) {
		/* target exists */
		/* same files */
		if (nip == oip)
			goto iputnip;

		if (doingdirs && nip->i_number == S5ROOTINO) {
			error = EINVAL;
			goto iputnip;
		}
		newparent = u.u_pdir->i_number;

		if (nip->i_ftype != (doingdirs ? IFDIR : IFREG))  {
			error = doingdirs ? ENOTDIR : EISDIR;
			goto iputnip;
		}

		if (doingdirs && s5access(nip, IWRITE))
			goto iputnip;

		if (s5access(u.u_pdir, IWRITE))
			goto iputnip;

		if (nip->i_dev != oip->i_dev) {
			error = EXDEV;
			goto iputnip;
		}

		if (doingdirs) {
			prele(nip);
			if (checkpath(mntdev, oip->i_number, nip->i_number)) 
				u.u_error = EINVAL;

			plock(nip);
		}
		iput(nip);

		/* remove target */
		if (u.u_error == 0) {
			nmarg.cmd = doingdirs ? NI_RMDIR : NI_DEL;
			(void) namei(upath, &nmarg);
		}
	
		if (u.u_error)
			goto plockoip;
	}
	else {
		/* target doesn't exist */
		if (u.u_error == ENOENT)
			newparent = u.u_pdir->i_number;
		else
			goto plockoip;

		if (u.u_pdir == (struct inode *) 0)
			goto plockoip;

		u.u_error = 0;
		if (s5access(u.u_pdir, IWRITE))
			goto plockoip;

		if (u.u_pdir->i_dev != oip->i_dev) {
			error = EXDEV;
			goto plockoip;
		}

		if (doingdirs && checkpath(mntdev, oip->i_number, newparent)) {
			error = EINVAL;
			goto plockoip;
		}
	}

	/* link old to new */
	nmarg.cmd = NI_LINK;
	nmarg.idev =  oip->i_dev;
	nmarg.ino =  oip->i_number;
	(void)namei(upath, &nmarg);
	plock(oip);
	iput (oip);
	if (u.u_error) {
		if (doingdirs && u.u_error == ENOENT)
			u.u_error = ENOTDIR;
		return;
	}

	/* unlink old */
	u.u_dirp = (caddr_t)uap->old;
	nmarg.cmd = doingdirs ? NI_DELDIR : NI_DEL;
	(void) namei(upath, &nmarg);
	u.u_error = 0;

	if (doingdirs) {
		/* change parent */
		u.u_dirp = (caddr_t)uap->new;
		nip = namei(upath, 0);

		ASSERT(nip != 0);

		u.u_offset = sizeof(struct direct);	/* .. */
		u.u_count = sizeof(struct direct);
		u.u_base = (caddr_t) &dir;
		segflg = u.u_segflg;
		u.u_segflg = 1;
		s5readi(nip);

		if (u.u_error) {
			iput (nip);
			u.u_segflg = segflg;
			printf("rename: s5readi errno = %d\n", u.u_error);
			return;
		}

		ASSERT(dir.d_ino == oldparent);

		dir.d_ino = newparent;

		u.u_offset = sizeof(struct direct);	/* .. */
		u.u_count = sizeof(struct direct);
		u.u_base = (caddr_t) &dir;
		u.u_segflg = 1;
		s5writei(nip);
		u.u_segflg = segflg;
		iput(nip);
		if (u.u_error) {
			printf("rename: s5writei errno = %d\n", u.u_error);
			return;
		}
	}

	/* 
	 * for parent dirs modify link counts (if doingdirs) and
	 * update access times
	 */
	oip = iget(mntdev, oldparent);
	ASSERT(oip != NULL);
	oip->i_nlink -= doingdirs;
	oip->i_flag |= IUPD|ICHG;
	iput(oip);

	nip = iget(mntdev, newparent);
	ASSERT(nip != NULL);
	nip->i_nlink += doingdirs;
	nip->i_flag |= IUPD|ICHG;
	iput(nip);
	return;

iputnip:
	iput(nip);
plockoip:
	plock(oip);
iputoip:
	iput(oip);
	if (error)
		u.u_error = error;
	return;
}

checkpath(mntdev, oldinum, newinum)
struct mount *mntdev;
register long newinum, oldinum;
{
	if (newinum == oldinum)
		return (1);
	while (newinum != S5ROOTINO) {
		newinum = getparent(mntdev, newinum);
		if (u.u_error || newinum == oldinum)
			return (1);
	}
	return (0);
}

getparent(mntdev, inum)
struct mount *mntdev;
register long inum;
{
	register struct inode *ip;
	struct direct dir;
	int segflg;

	ip = iget(mntdev, inum);
	if (ip) {
		u.u_offset = sizeof(struct direct);	/* .. */
		u.u_count = sizeof(struct direct);
		u.u_base = (caddr_t) &dir;
		segflg = u.u_segflg;
		u.u_segflg = 1;
		s5readi(ip);
		u.u_segflg = segflg;
		iput(ip);
		if (u.u_error == 0)
			return(dir.d_ino);
	}	
	return S5ROOTINO;				/* XXX */
}

#ifndef PATHSIZE
#define	PATHSIZE	512
#endif

checkdots(p)
caddr_t p;
{
	register char *cp, *bufp;

	char stkbuf[PATHSIZE];

	cp = stkbuf;
	bufp = cp;

	/* Read in entire pathname.  The functions will return */
	/* the length of the pathname if it was read correctly. */
	/* Otherwise, the following values will be returned:
	/* 	-1	if the user supplied address was not valid */
	/*	-2	if the pathname length was >= PATHSIZE-1
	/*	0	if a null pathname was supplied. This is */
	/*		  normally an error since null pathnames */
	/*		  are not supported (however, they turn up */
	/*		  in Distributed Unix pathname searches: see */
	/*		  comment below) */
	switch (upath(p, bufp, PATHSIZE)) {
	case -2:	/* pathname too long */
		u.u_error = ENAMETOOLONG;
		return 1;
	case 0:		/* null pathname */
		u.u_error = ENOENT;
		return 1;
	case -1:	/* bad user address */
		u.u_error = EFAULT;
		return 1;
	}
	while (*cp) cp++;		/* all the way to the end */
	cp--;
	while (*cp == '/') {
		*cp = '\0';
		cp--;	/* backup over / */
	}
	while (*cp != '/') {
		if(cp > bufp)
			 cp--;	/* backup over component */
		else
			break;
	}
	if (*cp == '/')
		cp++;
	if (cp[0] == '.' && cp[1] == '\0')
		return 1;
	if (cp[0] == '.' && cp[1] == '.' && cp[2] == '\0')
		return 1;
	return 0;
}
E 1
