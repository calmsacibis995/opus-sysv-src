h10512
s 00019/00007/00543
d D 1.5 90/04/19 11:46:01 root 5 4
c cleaned print format
e
s 00001/00001/00549
d D 1.4 90/04/02 13:23:02 root 4 3
c check for FNONB instead of FNDELAY for writev
e
s 00010/00000/00540
d D 1.3 90/03/30 12:14:15 root 3 2
c match write and writev for compatiblity
e
s 00002/00001/00538
d D 1.2 90/03/27 19:04:36 root 2 1
c writev's will block if O_NONBLOCK is clear
e
s 00539/00000/00000
d D 1.1 90/03/06 12:39:03 root 1 0
c date and time created 90/03/06 12:39:03 by root
e
u
U
t
T
I 1
#include "sysmacros.h"
#include "param.h"
#include "systm.h"
#include "dir.h"
#include "user.h"
#include "file.h"
#include "inode.h"
#include "fstyp.h"
#include "fcntl.h"
#include "proc.h"
#include "mount.h"
#include "var.h"
#include "fstyp.h"
#include "conf.h"
#include "utsname.h"
#include "sysinfo.h"
I 5
#include "reg.h"
E 5
I 3
#include "fs/s5inode.h"
E 3

/* bsd system calls */
extern 
	socket(), 			/* 00 */
	bind(),				/* 01 */
	listen(),			/* 02 */
	accept(),			/* 03 */
	connect(),			/* 04 */
	socketpair(),			/* 05 */
	sendto(),			/* 06 */
	send(),				/* 07 */
	sendmsg(),			/* 08 */
	recvfrom(),			/* 09 */
	recv(),				/* 10 */
	recvmsg(),			/* 11 */
	shutdown(),			/* 12 */
	setsockopt(),			/* 13 */
	getsockopt(),			/* 14 */
	bsdpipe(),			/* 15 */
	getsockname(),			/* 16 */
	select(),			/* 17 */
	getpeername(),			/* 18 */
	wait3(),			/* 19 */
	setreuid(),			/* 20 */
	setregid(),			/* 21 */
	fchmod(),			/* 22 */
	fchown(),			/* 23 */
	getdtablesize(),		/* 24 */
	getpagesize(),			/* 25 */
	softnetint(),			/* 26 */
	readv(),			/* 27 */
	writev(),			/* 28 */
	gethostid(),			/* 29 */
	gethostname();			/* 30 */
	
int (*bsdsw[])() = {
	socket, 
	bind,
	listen,
	accept,
	connect,
	socketpair,
	sendto,
	send,
	sendmsg,
	recvfrom,
	recv,
	recvmsg,
	shutdown,
	setsockopt,
	getsockopt,
	bsdpipe,
	getsockname,
	select,
	getpeername,
	wait3,
	setreuid,
	setregid,
	fchmod,
	fchown,
	getdtablesize,
	getpagesize,
	softnetint,
	readv,
	writev,
	gethostid,
	gethostname,
};

char *
bsdstring[] = {
	"socket", 			/* 00 */
	"bind",				/* 01 */
	"listen",			/* 02 */
	"accept",			/* 03 */
	"connect",			/* 04 */
	"socketpair",			/* 05 */
	"sendto",			/* 06 */
	"send",				/* 07 */
	"sendmsg",			/* 08 */
	"recvfrom",			/* 09 */
	"recv",				/* 10 */
	"recvmsg",			/* 11 */
	"shutdown",			/* 12 */
	"setsockopt",			/* 13 */
	"getsockopt",			/* 14 */
	"bsdpipe",			/* 15 */
	"getsockname",			/* 16 */
	"select",			/* 17 */
	"getpeername",			/* 18 */
	"wait3",			/* 19 */
	"setreuid",			/* 20 */
	"setregid",			/* 21 */
	"fchmod",			/* 22 */
	"fchown",			/* 23 */
	"getdtablesize",		/* 24 */
	"getpagesize",			/* 25 */
	"softnetint",			/* 26 */
	"readv",			/* 27 */
	"writev",			/* 28 */
	"gethostid",			/* 29 */
	"gethostname", 			/* 30 */
};

int nbsdcalls = sizeof (bsdsw) / sizeof(int);

bsdfunc(cmd)
register unsigned cmd;
{
	register int *regs;
D 5
	int syscall;
E 5
I 5
	int syscall, retval;
E 5

	if (cmd >= nbsdcalls) {
		u.u_error = EINVAL;
		return;
	}

	syscall = u.u_syscall;
	u.u_syscall = cmd + 128;

	regs = u.u_arg;
D 5
	if (begintrace(cmd+128))
		printf("%s(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x) ", bsdstring[cmd], 
E 5
I 5
	if (begintrace(cmd+128)) {
		procname(0);
		printf("(%d)@%x,\t", u.u_procp->p_pid, u.u_ar0[R1]);
		printf("%s(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n", bsdstring[cmd], 
E 5
			regs[0],regs[1],regs[2],regs[3],regs[4],regs[5]);
I 5
	}
E 5

	(*bsdsw[cmd])();

D 5
	if (endtrace(cmd+128))
		printf(" [%s --> %s(0x%x)]\n", bsdstring[cmd],
			u.u_error ? "err" : "ret",
			u.u_error ? u.u_error : u.u_rval1);
E 5
I 5
	if (endtrace(cmd+128)) {
		procname(0);
		if (u.u_error) {
			retval = u.u_error;
			printf("(%d) err,\t", u.u_procp->p_pid);
		}
		else {
			retval = u.u_rval1;
			printf("(%d) end,\t", u.u_procp->p_pid);
		}
		printf("%s -> %d\n", bsdstring[cmd], retval);
	}
E 5

	u.u_syscall = syscall;
}

/* bcs socket functions */
unsigned char berkfuncsw[] = 
{
	0,		/* 0. not used */
	0, 		/* 1. socket */
	1,		/* 2. bind */
	4,		/* 3. connect */
	2,		/* 4. listen */
	3,		/* 5. accept */
	16,		/* 6. getsockname */
	18,		/* 7. getpeername */
	7,		/* 8. send */
	6,		/* 9. sendto */
	8,		/* 10 sendmsg */
	10,		/* 11 recv */
	9,		/* 12 recvfrom */
	11,		/* 13 recvmsg */
	12,		/* 14 shutdown */
	13,		/* 15 setsockopt */
	14,		/* 16 getsockopt */
	27,		/* 17 readv */
	28,		/* 18 writev */
	5,		/* 19 socketpair */
	17,		/* 20 select */
	30,		/* 21 gethostname */
	0,		/* 22 sethostname */
	29,		/* 23 gethostid */
	0,		/* 24 sethostid */
	22,		/* 25 fchmod */
	23,		/* 26 fchown */
};

berkfunc(cmd)
register unsigned cmd;
{
	register int *regs;

	regs = u.u_arg;
	if (cmd > 0 && cmd < 27) {
		if (cmd == 22 || cmd == 24) /* sethostname, sethostid */
			goto nosys;
		if (cmd != 9 && cmd != 12) { /* sendto, recvfrom */
			/* slide args to account for sys3b index */
			regs[5] = regs[4];
			regs[4] = regs[3];
			regs[3] = regs[2];
			regs[2] = regs[1];
			regs[1] = regs[0];
		} else
		 	regs[0] = regs[0] << 16;
		bsdfunc(berkfuncsw[cmd]);
		return;
	}
nosys:
	u.u_error = ENOSYS;
}


/* copied wait code */
wait3()
{
#if m88k
	/* use waitpid */
	psignal(u.u_procp, SIGSYS);
#else
	struct a {
		int	s3bindex;
		int	status;
		int	options;
	} *uap = (struct a *)u.u_ap;
	register opts;
	register struct proc *p;

	opts = uap->options;
loop:
	for (p = u.u_procp->p_child ; p ; p = p->p_sibling) {
		if (p->p_stat == SZOMB) {
			freeproc(p, 1);
			return;
		}
		if (p->p_stat == SSTOP) {
			if ((p->p_flag & STRC || opts & 2 /*WUNTRACED*/) &&
				 (p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_rval1 = p->p_pid;
				u.u_rval2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if (u.u_procp->p_child) {
		if (opts & 1 /*WNOHANG*/) {
			u.u_rval1 = 0;
			return;
		}
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
#endif
}

setreuid()
{
	struct a {
		int	s3bindex;
		int	ruid;
		int	euid;
	} *uap;
	register int ruid, euid;

	uap = (struct a *)u.u_ap;
	ruid = uap->ruid;
	if (ruid == -1)
		ruid = u.u_ruid;
	if (ruid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_ruid != ruid && u.u_uid != ruid && !suser())
		return;
	euid = uap->euid;
	if (euid == -1)
		euid = u.u_uid;
	if (euid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_ruid != euid && u.u_uid != euid && !suser())
		return;
	/*
	 * Everything's okay, do it.
	 */
	u.u_procp->p_uid = ruid;
	u.u_procp->p_suid = euid;
	u.u_ruid = ruid;
	u.u_uid = euid;
}

setregid()
{
	register struct a {
		int	rgid;
		int	egid;
	} *uap;
	register int rgid, egid;

	uap = (struct a *)u.u_ap;
	rgid = uap->rgid;
	if (rgid == -1)
		rgid = u.u_rgid;
	if (rgid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_rgid != rgid && u.u_gid != rgid && !suser())
		return;
	egid = uap->egid;
	if (egid == -1)
		egid = u.u_gid;
	if (egid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_rgid != egid && u.u_gid != egid && !suser())
		return;

	u.u_gid = egid;
	u.u_rgid = rgid;
	u.u_procp->p_sgid = egid;
}

fchmod()
{
	register struct inode *ip;
	register struct a {
		int	s3bindex;
		int	fd;
		int	fmode;
	} *uap;
	struct file *fp, *getf();
	struct argnamei nmarg;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fd)) == NULL)
		return;
	if ((ip = fp->f_inode)->i_ftype == IFMPB) {
		u.u_error = EINVAL;
		return;
	}
	plock(ip);
	nmarg.cmd = NI_CHMOD;
	nmarg.mode = uap->fmode;
	if (FS_SETATTR(ip, &nmarg) == 0) {
		prele(ip);
		return;
	}
	ip->i_flag |= ICHG;
	prele(ip);
}

fchown()
{
	register struct inode *ip;
	register struct a {
		int	s3bindex;
		int	fd;
		int	uid;
		int	gid;
	} *uap;
	struct file *fp, *getf();
	struct argnamei nmarg;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fd)) == NULL)
		return;
	if ((ip = fp->f_inode)->i_ftype == IFMPB) {
		u.u_error = EINVAL;
		return;
	}
	plock(ip);
	nmarg.cmd = NI_CHOWN;
	nmarg.uid = (ushort)uap->uid;
	nmarg.gid = (ushort)uap->gid;
	if (FS_SETATTR(ip, &nmarg) == 0) {
		prele(ip);
		return;
	}
	ip->i_flag |= ICHG;
	prele(ip);
}

getdtablesize()
{
	u.u_rval1 = v.v_nofiles;
}

getpagesize()
{
	u.u_rval1  = NBPP;
}

readv()
{
	extern soreceive();

	sysinfo.sysread++;
	rwuio(FREAD, soreceive);
}

writev()
{
	extern sosend();

	sysinfo.syswrite++;
	rwuio(FWRITE, sosend);
}

rwuio(mode, f)
int (*f)();
{
	register struct a {
		int	s3bindex;
		int	fdes;
		struct	iovec *iovp;
		unsigned iovcnt;
	} *uap = (struct a *)u.u_ap;
	register struct file *fp;
	struct file *getf();
	register struct iovec *iov;
	int i;
	int s3bindex;
	struct uio auio, *uio;
	struct iovec aiov[16];		/* XXX */

	if (uap->iovcnt == 0 || uap->iovcnt > sizeof(aiov)/sizeof(aiov[0])) {
		u.u_error = EINVAL;
		return;
	}
	u.u_error = copyin((caddr_t)uap->iovp, (caddr_t)aiov,
	    uap->iovcnt * sizeof (struct iovec));
	if (u.u_error) {
		u.u_error = EFAULT;
		return;
	}

	if ((fp = getf(uap->fdes)) == NULL)
		return;
	if ((fp->f_flag&mode) == 0) {
		u.u_error = EBADF;
		return;
	}

	uio = &auio;
	uio->uio_iov = iov = aiov;
	uio->uio_iovcnt = uap->iovcnt;
	uio->uio_resid = 0;
	uio->uio_segflg = UIO_USERSPACE;
	for (i = 0; i < uio->uio_iovcnt; i++) {
		if (iov->iov_len < 0) {
			u.u_error = EINVAL;
			return;
		}
		uio->uio_resid += iov->iov_len;
		if (uio->uio_resid < 0) {
			u.u_error = EINVAL;
			return;
		}
		iov++;
	}
	u.u_count = uio->uio_resid;
	uio->uio_offset = fp->f_offset;
	s3bindex = uap->s3bindex;

	if (fp->f_inode->i_ftype == IFMPB) {
		u.u_error = (*f)((struct socket *) fp->f_inode->i_fsptr, 
						0, uio, 0, 0);
		if (u.u_error == 0) {
			u.u_rval1 = u.u_count - uio->uio_resid;
			fp->f_offset += u.u_rval1;
		}
	}
	else
		rdwruio(mode, uio, uap->fdes, fp);

	uap->s3bindex = s3bindex;
}

rdwruio(mode, uio, fdes, fp)
register struct uio *uio;
register struct file *fp;
{
	register struct a {
		int	fdes;
		caddr_t	base;
		int	count;
	} *uap = (struct a *)u.u_ap;
	int ndone;
	register i;
	int flag;
	
I 3
	if ((mode == FWRITE) &&
		(fp->f_inode->i_ftype == IFIFO)  &&
		(uio->uio_resid <= PIPSIZ) &&
		((fp->f_inode->i_size + uio->uio_resid) > PIPSIZ) &&
D 4
		(fp->f_flag & FNDELAY)) {
E 4
I 4
		(fp->f_flag & FNONB)) {
E 4
		/* can't do partials */
		u.u_error = EAGAIN;
		return;
	}
E 3
	ndone = 0;

	flag = fp->f_flag;
	for (i = 0; i < uio->uio_iovcnt; i++) {
		if ((uap->count = uio->uio_iov[i].iov_len) == 0)
			continue;
		uap->fdes = fdes;
		uap->base = uio->uio_iov[i].iov_base;

		rdwr(mode);

		if (u.u_error)
			break;
		ndone += u.u_rval1;
		if (u.u_rval1 != uio->uio_iov[i].iov_len)
			break;
D 2
		fp->f_flag |= FNDELAY;	/* NO DELAY AFTER 1ST OP */
E 2
I 2
		if (mode == FREAD)
			fp->f_flag |= FNDELAY;	/* NO DELAY AFTER 1ST READ */
E 2
	}
	if (u.u_error) {
		/* return partial count if we have any */
		if (ndone)
			u.u_error = 0;
	}
	fp->f_flag = flag;
	u.u_rval1 = ndone;
}

gethostname()
{
	register struct a {
		int	s3bindex;
		char	*name;
		uint	namelen;
	} *uap = (struct a *)u.u_ap;

	if (uap->namelen > sizeof(utsname.nodename))
		uap->namelen = sizeof (utsname.nodename);
	if (copyout(utsname.nodename, uap->name, uap->namelen))
		u.u_error = EFAULT;
}

gethostid()
{
	if ((u.u_rval1 = getinetid()) == 0)
		u.u_error = ENETDOWN;
}
E 1
