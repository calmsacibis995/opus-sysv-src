h61753
s 00007/00001/00623
d D 1.2 90/04/03 17:02:47 root 2 1
c added nfs_stsize
e
s 00624/00000/00000
d D 1.1 90/03/06 12:43:31 root 1 0
c date and time created 90/03/06 12:43:31 by root
e
u
U
t
T
I 1
/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 *
 *	The copyright above and this notice must be preserved in all
 *	copies of this source code.  The copyright above does not
 *	evidence any actual or intended publication of this source
 *	code.
 *
 *	This is unpublished proprietary trade secret source code of
 *	Lachman Associates.  This source code may not be copied,
 *	disclosed, distributed, demonstrated or licensed except as
 *	expressly authorized by Lachman Associates.
 */

#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_subr.c	4.2a LAI System V NFS Release 3.2/V3	source";
#endif

/*      @(#)nfs_subr.c 2.35 85/11/22 SMI      */

/*
 * Some code from this module was taken from Convergent's nfs_subr.c:
 * 
 * 	Convergent Technologies - System V - Nov 1986
 * 	#ident  "@(#)nfs_subr.c 1.5 :/source/uts/common/fs/nfs/s.nfs_subr.c 1/20/87 00:04:04"
 * 
 *	#ifdef LAI
 * 	static char SysVNFSID[] = "@(#)nfs_subr.c       2.17 LAI System V NFS Release 3.0/V3 source";
 *	#endif
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#include "sys/immu.h"
#endif
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/conf.h"
#include "sys/inode.h"
#include "sys/nami.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/mount.h"
#include "sys/var.h"
#include "sys/stream.h"
#if !m88k
#include "sys/inline.h"
#endif
#include "sys/cmn_err.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

#include "sys/fs/s5inode.h"

#define OWNER(IP)	((u.u_uid == (IP)->i_uid || suser()) ? 0 : u.u_error)

/*
 * Client side utilities
 */

/*
 * client side statistics
 */
struct {
	int	nclsleeps;		/* client handle waits */
	int	nclgets;		/* client handle gets */
	int	ncalls;			/* client requests */
	int	nbadcalls;		/* rpc failures */
	int	reqs[32];		/* count of each request */
} clstat;

#define MAXCLIENTS	4

struct chtab chtable[MAXCLIENTS];
int maxclients = MAXCLIENTS;

int chtable_avail;
int clwanted;

/*
 * Clinit -- initialize the client handle table.
 */
clinit()
{

	chtable_avail = maxclients;
}

CLIENT *
clget(mi, cred)
	struct mntinfo *mi;
	struct ucred *cred;
{
	register struct chtab *ch;
	register CLIENT *cl;

	clstat.nclgets++;

	for (ch = chtable; ch < &chtable[maxclients]; ch++) {
		if (!ch->ch_inuse) {
			ch->ch_inuse = TRUE;
			if (ch->ch_client == NULL) {
				ch->ch_client =
				    clntkudp_create(mi->mi_addr, mi->mi_addrlen,
						    NFS_PROGRAM, NFS_VERSION,
						    mi->mi_retrans, cred);
			} else {
				clntkudp_init(ch->ch_client,
					      mi->mi_addr, mi->mi_addrlen,
					      mi->mi_retrans, cred);
			}
			ch->ch_timesused++;
			return(ch->ch_client);
		}
	}
	cl = clntkudp_create(mi->mi_addr, mi->mi_addrlen,
			     NFS_PROGRAM, NFS_VERSION,
			     mi->mi_retrans, cred);
	clstat.nclsleeps++;	/* still useful */
	return(cl);
}

clfree(cl)
	CLIENT *cl;
{
	register struct chtab *ch;

	for (ch = chtable; ch < &chtable[maxclients]; ch++) {
		if (ch->ch_client == cl) {
			ch->ch_inuse = FALSE;
			return;
		}
	}
	CLNT_DESTROY(cl);
}

char *rpcstatnames[] = {
	"SUCCESS", "CANT ENCODE ARGS", "CANT DECODE RES", "CANT SEND",
	"CANT RECV", "TIMED OUT", "VERS MISMATCH", "AUTH ERROR",
	"PROG UNAVAIL", "PROG VERS MISMATCH", "PROC UNAVAIL",
	"CANT DECODE ARGS", "UNKNOWN HOST", "UNKNOWN", "PMAP FAILURE",
	"PROG NOT REGISTERED", "SYSTEM ERROR", "FAILED"
};
char *rfsnames[] = {
	"null", "getattr", "setattr", "unused", "lookup", "readlink", "read",
	"unused", "write", "create", "remove", "rename", "link", "symlink",
	"mkdir", "rmdir", "readdir", "fsstat"
};

/*
 * Back off for retransmission timeout, MAXTIMO is in 10ths of a sec
 */
#define MAXTIMO	300
#define backoff(tim)	((((tim) << 2) > MAXTIMO) ? MAXTIMO : ((tim) << 2))

int
rfscall(mi, which, xdrargs, argsp, xdrres, resp, cred)
	register struct mntinfo *mi;
	int which;
	xdrproc_t xdrargs;
	caddr_t argsp;
	xdrproc_t xdrres;
	caddr_t resp;
	struct ucred *cred;
{
	CLIENT *client;
	register enum clnt_stat status;
	struct rpc_err rpcerr;
	struct timeval wait;
	struct ucred *newcred;
	int timeo;
	bool_t tryagain;

	rpcerr.re_errno = 0;
	clstat.ncalls++;
	clstat.reqs[which]++;
	newcred = NULL;
	timeo = mi->mi_timeo;
retry:
	client = clget(mi, cred);
	if (client == (CLIENT *)NULL) {
		rpcerr.re_errno = EINTR;
		goto notry;
	}

	/*
	 * If hard mounted fs, retry call forever unless hard error occurs
	 */
	do {
#if clipper || ns32000 || m88k
		/* use HZ units */
		wait.tv_sec = timeo * (HZ / 10);
		wait.tv_usec = 0;
#else
		wait.tv_sec = timeo / 10;
		wait.tv_usec = 100000 * (timeo % 10);
#endif
		status = CLNT_CALL(client, which, xdrargs, argsp,
				   xdrres, resp, wait);
		switch (status) {
		case RPC_SUCCESS:
			tryagain = FALSE;
			break;

		case RPC_INTERRUPTED:
			tryagain = FALSE;
			break;
		/*
		 * Unrecoverable errors: give up immediately
		 */
		case RPC_AUTHERROR:
		case RPC_CANTENCODEARGS:
		case RPC_CANTDECODERES:
		case RPC_VERSMISMATCH:
		case RPC_PROGVERSMISMATCH:
		case RPC_CANTDECODEARGS:
			tryagain = FALSE;
			break;

		default:
			if (!mi->mi_printed) {
				mi->mi_printed = 1;
				cmn_err(CE_WARN,
					"NFS server %s not responding, %s",
					mi->mi_hostname,
					mi->mi_hard ? "still trying"
						    : "giving up");
			}
			timeo = backoff(timeo);
			tryagain = TRUE;
		}
	} while (tryagain && mi->mi_hard);

	if (status != RPC_SUCCESS) {
		CLNT_GETERR(client, &rpcerr);
		if (status != RPC_INTERRUPTED) {
			clstat.nbadcalls++;
			cmn_err(CE_WARN, "NFS %s failed for server %s: %s",
				rfsnames[which], mi->mi_hostname,
				rpcstatnames[(int)status]);
		}
	} else if (resp && *(int *)resp == EACCES &&
		   newcred == NULL && cred->cr_uid == 0 && cred->cr_ruid != 0) {
		/*
		 * Boy is this a kludge!  If the reply status is EACCES
		 * it may be because we are root (no root net access).
		 * Check the real uid, if it isn't root make that
		 * the uid instead and retry the call.
		 */
		newcred = crdup(cred);
		cred = newcred;
		cred->cr_uid = cred->cr_ruid;
		clfree(client);
		goto retry;
	} else if (mi->mi_printed && mi->mi_hard) {
		cmn_err(CE_WARN, "NFS server %s ok", mi->mi_hostname);
	}
	mi->mi_printed = 0;

	clfree(client);
notry:
	if (newcred) {
		crfree(newcred);
	}
	return(rpcerr.re_errno);
}

/* cobble some kind of generation # together */
#ifdef NEVER
#define IGEN(ip)	(((struct s5inode *)(ip->i_fsptr))->s5i_addr[0])
#else
#define IGEN(ip)	((ulong)(((struct s5inode *)(ip->i_fsptr))->s5i_gen))
#endif

/*
 * Make an fhandle from a s5 inode.
 */
makefh(fh, ip)
	register fhandle_t *fh;
	register struct inode *ip;
{
	extern s5init();

	if (fstypsw[ip->i_fstyp].fs_init != s5init) {
		return(EREMOTE);
	}

	bzero((caddr_t)fh, NFS_FHSIZE);
	fh->fh_fsid = ip->i_dev;
	fh->fh_fno = ip->i_number;
	fh->fh_fgen = IGEN(ip);
	return(0);
}

/*
 * Convert an fhandle into a inode.
 *
 * The file system id (fsid) in the file handle is used to find the
 * correct mounted file system.  Then, the inode number (fno) in the
 * file handle is used to get the locked incore inode.  The generation
 * counts are then checked to make sure that the inode has not been
 * reused in between uses by the client system.
 *
 * Slight bit of clumsiness - the routine ifind() is used here instead
 * of the normal iget() because iget() hops mount points and ifind()
 * doesn't.  This is desirable for NFS.
 */
struct inode *
fhtoip(fh)
	register fhandle_t *fh;
{
	register struct inode *ip;
	register struct mount *mp;
	extern struct inode *ifind();

	for (mp = mount; mp < (struct mount *)v.ve_mount; mp++) {
		if ((mp->m_flags & MINUSE) && mp->m_dev == fh->fh_fsid) {
			break;
		}
	}
	if (mp >= (struct mount *)v.ve_mount) {
		return(NULL);
	}

	ip = ifind(mp, fh->fh_fno);

	if (ip == NULL) {
		return(NULL);
	}
	if (IGEN(ip) != fh->fh_fgen) {
		iput(ip);
		return(NULL);
	}
	return(ip);
}


/*
 * General utilities
 */

I 2
#if !m88k
E 2
/*
 * Returns the prefered transfer size in bytes based on
 * what network interfaces are available.
 */
nfs_tsize()
{

	return(NFS_MAXDATA);
}

D 2
#if !m88k
E 2
I 2
nfs_stsize()
{

	return(NFS_MAXDATA);
}

E 2
splnet()
{

	return(splstr());
}

splimp()
{

	return(splstr());
}
#endif

/*
 * Convert inode type to NFS "network" type.
 */
static enum nfsftype
iftovt(itype)
	register ushort itype;
{

	switch (itype & IFMT) {
	case IFDIR:
		return(NFDIR);
	case IFCHR:
		return(NFCHR);
	case IFBLK:
		return(NFBLK);
	case IFREG:
		return(NFREG);
	case IFMPC:
	case IFMPB:
	case IFIFO:
		return(NFNON);
	default:
		return(NFNON);
	}
	/* NOTREACHED */
}

/*
 * Get file attributes. Entered with ip locked.
 * Use UBLOCK credentials.
 */
iattr_to_nattr(ip, nap)
	register struct inode *ip;
	register struct nfsfattr *nap;
{
	struct ostat dfst;

	/*
	 * Copy attr information from inode entry.
	 */
	nap->na_type = iftovt(ip->i_ftype);
	nap->na_fsid = ip->i_dev;
	nap->na_fsid = nap->na_fsid & 0xffff;
	nap->na_nodeid = ip->i_number;
	nap->na_nlink = ip->i_nlink;
	nap->na_uid = ip->i_uid;
	nap->na_gid = ip->i_gid;
	nap->na_rdev = ip->i_rdev;
	nap->na_rdev = nap->na_rdev & 0xffff;
	nap->na_size = ip->i_size;
	nap->na_blocks = (ip->i_size + ip->i_mntdev->m_bsize - 1) /
			 ip->i_mntdev->m_bsize;

	/*
	 * Call FS_STATF to transfer information
	 * from the disk inode (file modes & dates).
	 */
	if (ip->i_flag & (IACC|IUPD|ICHG))
		FS_IUPDAT(ip, &time, &time);
	FS_STATF(ip, &dfst);
	nap->na_mode = ip->i_ftype | (dfst.st_mode & MODEMSK);
	nap->na_atime.tv_sec = dfst.st_atime; nap->na_atime.tv_usec = 1000000-1;
	nap->na_mtime.tv_sec = dfst.st_mtime; nap->na_mtime.tv_usec = 1000000-1;
	nap->na_ctime.tv_sec = dfst.st_ctime; nap->na_ctime.tv_usec = 1000000-1;

	switch(ip->i_ftype & IFMT) {
	case IFBLK:
	case IFCHR:
		nap->na_blocksize = SBUFSIZE;
		break;

	default:
		nap->na_blocksize = ip->i_mntdev->m_bsize;
		break;
	}
}

/*
 * Set file attributes, Entered with ip locked.
 * Use UBLOCK credentials.
 */
rfs_do_setattr(ip, sap)
	register struct inode *ip;
	register struct nfssattr *sap;
{
	int chtime = 0;
	int error = 0;
	time_t atime, mtime;
	struct ostat dfst;
	struct argnamei nmarg;

	/*
	 * Change file access modes.
	 * Server daemon takes on uid & gid of requester.
	 */
	if (sap->sa_mode != (ulong) -1) {
		nmarg.cmd = NI_CHMOD;
		nmarg.mode = sap->sa_mode & ~IFMT;
		if (FS_SETATTR(ip, &nmarg) != 0)
			ip->i_flag |= ICHG|ISYN;
		else
			error = u.u_error;
	}
	/*
	 * Change file ownership.
	 */
	if ((sap->sa_uid != (ulong) -1) || (sap->sa_gid != (ulong) -1)) {
		nmarg.cmd = NI_CHOWN;
		if (sap->sa_uid != (ulong) -1)
			nmarg.uid = (ushort)sap->sa_uid;
		else
			nmarg.uid = ip->i_uid;
		if (sap->sa_gid != (ulong) -1)
			nmarg.gid = (ushort)sap->sa_gid;
		else
			nmarg.gid = ip->i_gid;
		if (FS_SETATTR(ip, &nmarg) != 0)
			ip->i_flag |= ICHG|ISYN;
		else
			error = u.u_error;
	}
	/*
	 * Truncate file. Must have write permission and not be a directory.
	 * Use FS_ACCESS to determine accessibility
	 */
	if (sap->sa_size != (ulong) -1) {
		if ((ip->i_ftype & IFMT) == IFDIR) {
			error = EISDIR;
			goto out;
		}
		if (FS_ACCESS(ip, IWRITE)) {
			error = u.u_error;
			goto out;
		}
		/*
		 * System V can only truncate to zero.
		 */
		if (sap->sa_size != 0) {
			error = EINVAL;
			goto out;
		}
		FS_ITRUNC(ip);
	}
	/*
	 * Change file access or modified times.
	 */
	FS_STATF(ip, &dfst);
	if (sap->sa_atime.tv_sec != -1) {
		error = OWNER(ip);
		if (error)
			goto out;
		atime = sap->sa_atime.tv_sec;
		chtime++;
	} else {
		atime = dfst.st_atime;
	}
	if (sap->sa_mtime.tv_sec != -1) {
		error = OWNER(ip);
		if (error)
			goto out;
		mtime = sap->sa_mtime.tv_sec;
		chtime++;
	} else {
		mtime = dfst.st_mtime;
	}
	if (chtime)
		ip->i_flag |= IACC|IUPD|ICHG|ISYN;

	FS_IUPDAT(ip, &atime, &mtime);	/* XXX should be asyn for perf */
out:
	return(error);
}

/*
 * puterrno is used by the server to translate SV errors to network wide
 * error numbers
 */
enum nfsstat
puterrno(error)
	int error;
{
	switch (error) {
	case ENOSR:
		return(NFSERR_NOBUFS);

	case EREMOTE:
	case EMULTIHOP:
		return(NFSERR_REMOTE);

	default:
		return((enum nfsstat)error);
	}
}

/*
 * geterrno is used by the client to translate NFS errors to SV
 * error codes (returns an int)
 */
geterrno(status)
	enum nfsstat status;
{
	switch (status) {
	case NFSERR_OPNOTSUPP:
		return(EINVAL);

	case NFSERR_NAMETOOLONG:
		return(EINVAL);

	case NFSERR_NOTEMPTY:
		return(EEXIST);

	case NFSERR_STALE:
		return(EBADFD);

	case NFSERR_REMOTE:
		return(EMULTIHOP);

	default:
		return((int)status);
	}
}

/*
 * nfs_strlen() - a renamed strlen() that allows NFS to run under
 * System V without RFS string routines
 */
nfs_strlen(s)
	register char *s;
{
	register int n;

	n = 0;          
	while (*s++ != '\0')
		n++;
	return(n);
}
E 1
