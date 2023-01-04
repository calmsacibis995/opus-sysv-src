h12790
s 00901/00000/00000
d D 1.1 90/03/06 12:43:27 root 1 0
c date and time created 90/03/06 12:43:27 by root
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
static char SysVr3NFSID[] = "@(#)nfs_iget.c	4.6 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/mount.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/nami.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
#include "sys/var.h"
#include "sys/sysinfo.h"
#include "sys/buf.h"
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#include "sys/immu.h"
#endif
#include "sys/region.h"
#include "sys/proc.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

extern void kmem_free();

/* nfs global external data */
int nfsfstyp;			/* FSS fstyp[] index for NFS */
int nfsd_maj;			/* major device # */

#ifdef u3b2
extern int nfsd_imaj;		/* internal major device number */
#endif

/*
 *  an array of rnode pointers - hashed by filehandle
 */
#define NHRNODE 16

#define rhash(fhp)	(&hrnode[(fhp)->fh_fno & (NHRNODE-1)])

struct hrnode hrnode[NHRNODE];		/* hash queues */

struct rfreelist rfreelist;		/* the rnode free list anchor */

/*
 * Initialize the nfs (client) data areas.
 */
nfsinit()
{
	register int i;
	register struct rnode *rp;
	register struct hrnode *hrp;
#ifndef u3b2
	register struct bdevsw *bdp;
#endif
	int rnumlimit;
#ifndef u3b2
	extern int nfsdstrategy();
#endif

	nfs_version();

	/*
	 * This code checks to make sure that the number of rnodes
	 * configured into the system is less than or equal to the
	 * maximum allowed.  The limit is determined by the size
	 * of a minor device number.  The code uses -1 as a device
	 * major/minor pair to take the minor of because -1 is
	 * assumed to be all ones.  (True only on a two's complement
	 * system!)
	 */
	rnumlimit = minor((dev_t)-1) + 1;
	if (rnumlimit < nfs_rnum) {
		cmn_err(CE_WARN,
			"nfsinit: reducing the number of NFS rnodes from %d to %d",
			nfs_rnum, rnumlimit);
		nfs_rnum = rnumlimit;
	}

	/*
	 * Initialize the rnode freelist.
	 */
	rfreelist.av_forw = rfreelist.av_back = (struct rnode *)&rfreelist;
	rp = nfs_rnodes;
	for (i = 0; i < nfs_rnum; i++) {
		rfreelist.av_forw->av_back = rp;
		rp->av_forw = rfreelist.av_forw;
		rfreelist.av_forw = rp;
		rp->av_back = (struct rnode *)&rfreelist;
		rp->r_forw = rp->r_back = rp;
		rp->r_flags = RSTALE;
		rp++;
	}
#if clipper || ns32000 || m88k
	if (sizeof(struct rnode) != 0x130)
		cmn_err(CE_PANIC, "rnode size is %x\n", sizeof(struct rnode));
#endif

	/*
	 * Initialize the file handle hash indicies into the rnode table.
	 */
	hrp = hrnode;
	for (i = 0; i < NHRNODE; i++) {
		hrp->r_forw = hrp->r_back = (struct rnode *)hrp;
		hrp++;
	}

	/*
	 * Set nfsfstyp as the index into FSS table for nfs entries.
	 */
	nfsfstyp = 0;
	for (i = 0; i < nfstyp; i++) {
		if (fstypsw[i].fs_init == nfsinit) {
			nfsfstyp = i;
			break;
		}
	}
	if (nfsfstyp == 0 || i == nfstyp)
		cmn_err(CE_PANIC, "nfsinit: nfsinit not found in fstypsw");

	/*
	 * Find our major device number.
	 */
#ifdef u3b2
	for (i = 0; i < 128; i++)
		if (MAJOR[i] == nfsd_imaj)
			break;
	if (i >= 128) {
		nfsd_maj = 0;
	} else {
		nfsd_maj = i;
	}
#else
	bdp = bdevsw;
	for (i = 0; i < bdevcnt; i++) {
		if (bdp->d_strategy == nfsdstrategy)
			break;
		bdp++;
	}
	if (i == bdevcnt)
		cmn_err(CE_PANIC, "nfsinit: nfsd not found in bdevsw");
	nfsd_maj = i;
#endif

	/*
	 * Init routines for various pieces of the NFS code.
	 */
	kmem_init();		/* initialize the kernel heap manager */
	dnlc_init();		/* initialize the directory name cache */
	clinit();		/* initialize the client handle table */
	svckudp_dupinit();	/* initialize the dup request cache */
}

/*
 * File system specific inode initialization.
 * We connect the inode with its rnode.
 */
struct inode *
nfsiread(ip)
	register struct inode *ip;
{
	ushort ntype_to_itype();
	register struct rnode *rp;

	/*
	 * Called to fill in a new inode. since the inode number for
	 * NFS files is really the rnode index, we had better have an
	 * rnode, allocated by nfs_iget(), waiting to be connected.
	 * The inode should not have an rnode already, since rnodes
	 * are disconnected from inodes in iput().
	 */
	ASSERT(ip->i_number >= 0 && ip->i_number < nfs_rnum);

	ASSERT(ip->i_fsptr == NULL);

	rp = rnotor(ip->i_number);

	ASSERT(rp->r_ip == NULL);

	ASSERT(!(rp->r_flags & RSTALE));

	nfs_unmark_iget(rp);

	ip->i_dev = makedev(nfsd_maj, rtorno(rp));
	ip->i_ftype = ntype_to_itype(rp->r_snfsattr.na_type);
	ip->i_size = rp->r_snfsattr.na_size;

	ip->i_fsptr = (int *)rp;	/* link rnode to inode */
	rp->r_ip = ip;			/* back link inode to rnode */

	nfs_attrcache(ip, &rp->r_snfsattr, SFLUSH);

	return(ip);
}

/*
 * Note that an inode is now unused. (called from iput())
 *
 * We are not called from iget() when recycling inodes.
 */
nfsiput(ip)
	register struct inode *ip;
{
	register struct rnode *rp;

	ASSERT(ip->i_fsptr != NULL);
	rp = itor(ip);
	ASSERT(rp->r_ip == ip);
	ASSERT(ip->i_count >= 1);

	nfs_unmark_iget(rp);

	if (ip->i_count == 1) {
		/*
		 * Flush all dirty blocks belonging to this rnode.
		 */
		if (rp->r_flags & RDIRTY) {
			struct ucred cred;

			set_cred(&cred);
			nfs_fsync(ip, &cred);
		}
		
		/*
		 * Wait on all i/o scheduled for this rnode to finish
		 */
		biowait(ip->i_dev, 1);
		
		/*
		 * If the file was removed while it was open it was
		 * renamed instead.  Here we remove the renamed file.
		 */
		if (rp->r_unlname != NULL) {
			int error;
			enum nfsstat status;
			struct nfsdiropargs da;

			plock(rp->r_unldip);
			setdiropargs(&da, rp->r_unlname, rp->r_unldip);
			error = rfscall(itomi(rp->r_unldip), RFS_REMOVE,
					xdr_diropargs, (caddr_t)&da,
					xdr_enum, (caddr_t)&status,
					&rp->r_unlcred);
			if (!error)
				error = geterrno(status);
			iput(rp->r_unldip);
			rp->r_unlcred.cr_ref = 0;
			kmem_free((caddr_t)rp->r_unlname, (uint)NFS_MAXNAMLEN);
			rp->r_unlname = NULL;
		}

		/*
		 * Flush any cached pages that might belong to this file.
		 *
		 * It is unfortunate that this must be done here.  It
		 * would be nice to be able to cache pages between uses
		 * of executable files.
		 */
		nfs_pflush(ip);
		
		/*
		 * Add to the freelist.
		 *
		 * First, we check to make sure that no one else has
		 * found this rnode in the meantime.  If so, we don't
		 * want to add it to the free list.  This will remove
		 * the possibility that it will get reused in the
		 * meantime.
		 *
		 * Note: the last iput on a root inode is special, we
		 * don't want it to hang around in the hash queues.
		 */
		if (rp->r_iget == NULL) {
			rfreelist.av_back->av_forw = rp;
			rp->av_forw = (struct rnode *)&rfreelist;
			rp->av_back = rfreelist.av_back;
			rfreelist.av_back = rp;
			rp->r_flags |= RSTALE;
			if (ip->i_flag & IISROOT) {
				rp->r_back->r_forw = rp->r_forw;
				rp->r_forw->r_back = rp->r_back;
				rp->r_forw = rp->r_back = rp;
			}
		}
		ip->i_fsptr = NULL;
		rp->r_ip = NULL;
	}
}

/*
 * Update the inode with the current time.
 */
nfsiupdat(ip, ta, tm)
	register struct inode *ip;
	time_t *ta, *tm;
{
	int error;
	struct ucred cred;
	struct nfssattr sa;

	nfs_unmark_iget(itor(ip));

	/*
	 * We should never change the modification times just because
	 * the client kernel thinks things have changed.  We should
	 * change the times only for the utimes system call.
	 * Remember that the NFS server is responsible for maintaining
	 * the state of the inode, including noticing accesses and
	 * modifications of the file
	 *
	 * We use a rather ugly hack to distinguish the system call from
	 * 'ordinary' time changes.
	 */
	if (ta != &time || tm != &time) {
		set_sa(&sa);
		set_cred(&cred);

		/* the update work is done in nfs_attrcache */
		sa.sa_atime.tv_sec = *ta;
		sa.sa_atime.tv_usec = 0;
		sa.sa_mtime.tv_sec = *tm;
		sa.sa_mtime.tv_usec = 0;
		error = nfs_setattr(ip, &sa, &cred);
		if (error)
			u.u_error = error;
	}

	ip->i_flag &= ~(IACC|IUPD|ICHG|ISYN);
}

/*
 * Set the file size to 0 and whatever fs specific work that implies.
 */
nfsitrunc(ip)
	register struct inode *ip;
{
	int error;
	struct ucred cred;
	struct nfssattr sa;

	nfs_unmark_iget(itor(ip));

	if (ip->i_ftype != IFREG) {
		u.u_error = EINVAL;
		return;
	}

	/*
	 * Truncate the file by requesting an attribute set of size 0.
	 */
	set_sa(&sa);
	set_cred(&cred);

	sa.sa_size = 0;

	if ((error = nfs_setattr(ip, &sa, &cred))) {
		u.u_error = error;
	} else {
		ip->i_size = 0;
	}
}

/*
 * Find an rnode given file a handle.
 * We will either find an existing rnode/inode based on cached
 * fhandle or we will set up a new rnode and cache it.
 */
struct rnode *
nfs_iget(fhp, atp, mp)
	fhandle_t *fhp;
	struct nfsfattr *atp;
	struct mount *mp;
{
	register struct rnode *rp;
	register struct hrnode *hrp;

	/*
	 * First we get an rnode for the filehandle: either we find it in
	 * the rnode cache or we have to make one from a free rnode.
	 * For a new rnode/inode the attributes have to be established by us.
	 */

	/* attempt to reclaim the same old rnode */
	hrp = rhash(fhp);
start:
	for (rp = hrp->r_forw; rp != (struct rnode *)hrp; rp = rp->r_forw) {
		if (!bcmp((char *)fhp, (char *)&rp->r_fh, sizeof(rp->r_fh)) &&
		    rp->r_mp == mp) {
			if (rp->r_flags & RSTALE) {
				rp->av_back->av_forw = rp->av_forw;
				rp->av_forw->av_back = rp->av_back;
				rp->r_flags &= ~RSTALE;
				rp->r_snfsattr = *atp;
			}
			break;
		}
	}

	if (rp == (struct rnode *)hrp) {	/* we have to find a new one */

		/* grab the first rnode on the free list */
		if ((rp = rfreelist.av_forw) == (struct rnode *)&rfreelist) {
			cmn_err(CE_WARN, "nfs_iget: rnode table overflow");
			u.u_error = ENFILE;
			return(NULL);
		}

		ASSERT(rp->r_flags & RSTALE);

		/* remove it from free list */
		rp->av_back->av_forw = rp->av_forw;
		rp->av_forw->av_back = rp->av_back;
		rp->av_forw = rp->av_back = rp;

		/* remove from the DNLC */
		dnlc_purge_rp(rp);
		
		/* unchain it */
		rp->r_back->r_forw = rp->r_forw;
		rp->r_forw->r_back = rp->r_back;

		/* initialize it */
		bzero((caddr_t)rp, sizeof(*rp));
		rp->r_fh = *fhp;
		rp->r_nfsattr = *atp;
		rp->r_snfsattr = *atp;
		rp->r_mp = mp;

		/* put it on its hash list */
		hrp->r_forw->r_back = rp;
		rp->r_forw = hrp->r_forw;
		hrp->r_forw = rp;
		rp->r_back = (struct rnode *)hrp;

		/* flush stale bio cache blocks */
		binval(makedev(nfsd_maj, rtorno(rp)));
	}

	nfs_mark_iget(rp);

	if (rp->r_ip != NULL)
		nfs_attrcache(rp->r_ip, atp, SFLUSH);

	return(rp);
}

/*
 * Free an rnode.  This routine is used to free an rnode if a
 * call to iget() was not done or if the call to iget() failed.
 * The rnode is placed back on the rnode freelist.
 */
nfs_rfree(rp)
	register struct rnode *rp;
{

	nfs_unmark_iget(rp);

	if (rp->r_iget == NULL) {
		rfreelist.av_back->av_forw = rp;
		rp->av_forw = (struct rnode *)&rfreelist;
		rp->av_back = rfreelist.av_back;
		rfreelist.av_back = rp;
		rp->r_flags |= RSTALE;
	}
}

struct nfsiget *nfsiget_head = NULL;

nfs_mark_iget(rp)
	struct rnode *rp;
{
	struct nfsiget *np;

	if (nfsiget_head == NULL) {
		np = (struct nfsiget *)kmem_alloc((uint)sizeof(*np));
	} else {
		np = nfsiget_head;
		nfsiget_head = np->n_next;
	}
	np->n_pid = u.u_procp->p_pid;
	np->n_next = rp->r_iget;
	rp->r_iget = np;
}

nfs_unmark_iget(rp)
	struct rnode *rp;
{
	struct nfsiget *np;
	struct nfsiget *pnp;

	pnp = NULL;
	np = rp->r_iget;
	while (np != NULL) {
		if (np->n_pid == u.u_procp->p_pid) {
			if (pnp == NULL) {
				rp->r_iget = np->n_next;
			} else {
				pnp->n_next = np->n_next;
			}
			np->n_next = nfsiget_head;
			nfsiget_head = np;
			return;
		}
		pnp = np;
		np = np->n_next;
	}
}

/*
 * Force synchronous write of delayed buffers for this inode/rnode.
 */
nfs_flush(ip)
	register struct inode *ip;
{

	if (itor(ip)->r_flags & RDIRTY) {	/* work only if necessary */
		itor(ip)->r_flags &= ~RDIRTY;
		bflush(ip->i_dev);
		biowait(ip->i_dev, 0);
	}
}

/*
 * Mark any buffers in the cache for this rnode as stale.
 */
nfs_inval(ip)
	register struct inode *ip;
{

	binval(ip->i_dev);
}

/*
 * Mark any free buffers in the cache for this rnode as stale.
 */
nfs_invalfree(ip)
	register struct inode *ip;
{

	binvalfree(ip->i_dev);
}

/*
 * Compare two byte strings.
 */
int
bcmp(p1, p2, cnt)
	register char *p1, *p2;
	register int cnt;
{

	while (--cnt >= 0) {
		if (*p1++ != *p2++)
			return(1);
	}
	return(0);
}

/*
 * Initialize the directory operations argument struct.
 */
setdiropargs(da, nm, dip)
	struct nfsdiropargs *da;
	char *nm;
	struct inode *dip;
{

	da->da_fhandle = *itofh(dip);
	da->da_name = nm;
}

/*
 * The following is a routine that should probably live bio.c.
 *
 * Its purpose is to wait on all the i/o hooked to a particular
 * dev_t to finish.  It is used in nfsiput() to make sure that
 * all the potentials users of an rnode have finished before
 * proceeding and from nfs_flush() to wait on all writes to
 * finish on this rnode before proceeding.  The 'wait' flag is
 * used to indicate whether or not to wait on all buffers or
 * just the buffers that are being written.
 */
biowait(dev, wait)
	register dev_t dev;
	register int wait;
{
	register struct buf *bp;
	register int i, s;

	wait = wait ? B_DONE : B_DONE | B_READ;
#if m88k
	for (i = v.v_buf, bp = bufz; i; i--, bp++) 
#else
	for (i = v.v_buf, bp = buf; i; i--, bp++) 
#endif
	{
#if 0
		if (bp->b_dev == dev &&
		    (wait || (bp->b_flags & B_READ) == B_WRITE))
#else
		if (bp->b_dev == dev && (bp->b_flags & wait) == 0)
#endif
		{
			syswait.iowait++;
			s = spl6();
			while ((bp->b_flags & B_DONE) == 0) {
				bp->b_flags |= B_WANTED;
				sleep((caddr_t)bp, PRIBIO);
			}
			splx(s);
			syswait.iowait--;
		}
	}
}

/* from os/bio.c */
#define notavail(bp) \
{ \
	register s; \
\
	s = spl6(); \
	bp->av_back->av_forw = bp->av_forw; \
	bp->av_forw->av_back = bp->av_back; \
	bp->b_flags |= B_BUSY; \
	bfreelist.b_bcount--; \
	splx(s); \
}

/*
 * The following is a routine that should probably live bio.c.
 *
 * Its purpose is to invalidate all the blocks belonging to a
 * particular dev_t which are on the freelist.  It is used from
 * nfs_attrcache() (via nfs_invalfree()) when the attributes
 * have changed unexpectedly.
 */
binvalfree(dev)
	dev_t dev;
{
	register struct buf *bp;
	register struct buf *dp;
	int s;

	dp = &bfreelist;
loop:
	s = spl6();
	bp = bfreelist.av_forw;
	while (bp != dp) {
		if (bp->b_dev == dev) {
			if (bp->b_flags & B_DELWRI) {
				bp->b_flags |= B_ASYNC;
				notavail(bp);
				(void) splx(s);
				bwrite(bp);
				goto loop;
			} else {
				bp->b_flags |= B_STALE|B_AGE;
			}
		}
                bp = bp->av_forw;
	}
	splx(s);
}

/*
 * The code following is a copy of iget() that does not cross mount points.
 */

/*
 *	inode hashing
 */

#define	NHINO	128
#define ihash(X)	(&hinode[(int) (X) & (NHINO-1)])

struct	hinode	{
	struct	inode	*i_forw;
	struct	inode	*i_back;
};
extern struct hinode hinode[NHINO];

extern struct ifreelist ifreelist;

/*
 * Look up an inode by mount point (device) and inumber.
 * If it is in core (in the inode structure), honor the locking
 * protocol.
 * If it is not in core, read it in from the specified device.
 * In all cases, a pointer to a locked inode structure is returned.
 *
 * printf warning: inode table overflow -- if the inode structure is
 *					   full.
#ifdef i386
 * panic: no imt -- if the mounted filesystem is not in the mount table.
 *	"cannot happen"
#endif
 */

#ifndef DEBUG
struct inode *
ifind(mp, ino)
	register struct mount *mp;
	register ino;
#else
struct inode *
ifind(mp, ino, caller)
	register struct mount *mp;
	register ino;
	int *caller;
#endif
{
	register struct inode *ip;
	register struct hinode *hip;
	register short fstyp;
	register struct inode *tip;

	sysinfo.iget++;
	fstyp = mp->m_fstyp;
loop:
	hip = ihash(ino);
	for (ip = hip->i_forw; ip != (struct inode *)hip; ip = ip->i_forw)
		if (ino == ip->i_number && mp == ip->i_mntdev)
			goto found;

	ip = ifreelist.av_forw;
	while ((ip != (struct inode *)&ifreelist)) {
		/*
		 * If inode locked, assume it has been found in the cache
		 * by another process, so march on, march on...
		 */
		if (!(ip->i_flag & ILOCK))
			break;
		ip = ip->av_forw;
	}
	if (ip == (struct inode *)&ifreelist) {
		cmn_err(CE_WARN, "iget - inode table overflow");
		if (fsinfo[fstyp].fs_notify & NO_IGET) {
			struct argnotify noarg;

			noarg.cmd = NO_IGET;
			noarg.data1 = (long)mp;
			noarg.data2 = (long)ino;
			(void)(*fstypsw[fstyp].fs_notify)((struct inode *)0,
							  &noarg);
		}
		syserr.inodeovf++;
		u.u_error = ENFILE;
		return(NULL);
	}

	ASSERT(ip == ip->av_back->av_forw);
	ASSERT(ip == ip->av_forw->av_back);
	ip->av_back->av_forw = ip->av_forw;	/* remove from free list */
	ip->av_forw->av_back = ip->av_back;

	/* Remove from old hash list. */

	ip->i_back->i_forw = ip->i_forw;
	ip->i_forw->i_back = ip->i_back;
	ip->i_flag = ILOCK;
	ASSERT(addilock(ip) == 0);
	ASSERT(ip->i_count == 0);

	/*
	 * If a block number list was allocated for this file
	 * (because it is a 413), then free the space now since
	 * we are going to reuse this table slot for a different
	 * inode.  See the code in mmgt/region.c/mapreg.
	 * Also, dispose of the left-over data structures associated with
	 * an old inode whose slot we are about to re-use.  If FS_NOICACHE
	 * is set for this fstyp then old inodes don't remain in
	 * the cache and this disposal is taken care of at the time of
	 * last reference to the inode in iput().  We also assume that
	 * this disposal need not be done at all unless a new fstyp
	 * is being assigned to the inode.
	 */

	if (ip->i_fstyp) {
		long flags;

		FS_FREEMAP(ip);
		flags = fsinfo[ip->i_fstyp].fs_flags;
		if ((flags & FS_NOICACHE) == 0
		  && (ip->i_fstyp != fstyp || (flags & FS_RECYCLE)))
			FS_IPUT(ip);
	}

	hip->i_forw->i_back = ip;	/* insert into new hash list */
	ip->i_forw = hip->i_forw;
	hip->i_forw = ip;
	ip->i_back = (struct inode *)hip;

	ip->i_mntdev = mp;
	ip->i_mnton = NULL;
	ip->i_dev = mp->m_dev;
	ip->i_fstyp = fstyp;
	ip->i_number = ino;
	ip->i_count = 1;
#ifdef FSPTR
	ip->i_fstypp = &fstypsw[fstyp];
#endif
#ifdef DEBUG
	ilog(ip, 0, caller);
#endif
	tip = FS_IREAD(ip);
	/*
	 * fs_iread returns NULL to indicate an error.
	 * If NULL is returned, no fs dependent inode has been
	 * assigned to the fs independent inode.
	 * In this case the newly allocated fs independent
	 * inode should be deallocated, that is, returned to the
	 * free list and taken off the hash list. If it stayed
	 * on the hash list it could be "found" and its use would
	 * cause a panic because it has no fs specific portion.
	 */
	if (tip == NULL) {
		ip->i_back->i_forw = ip->i_forw; /* remove from hash list */
		ip->i_forw->i_back = ip->i_back;
		ip->i_forw = ip->i_back = ip;
		ifreelist.av_back->av_forw = ip;
		ip->av_forw = (struct inode *)&ifreelist;
		ip->av_back = ifreelist.av_back;
		ifreelist.av_back = ip;
		ip->i_fsptr = NULL;
		ip->i_count = 0;
		ip->i_fstyp = 0;
#ifdef DEBUG
		/*
		 * Use inline code (temporarily) here instead of a call to
		 * prele() to allow use of an otherwise-bogus assertion in
		 * prele().
		 */
		ip->i_flag &= ~ILOCK;
		if (ip->i_flag & IWANT) {
			ip->i_flag &= ~IWANT;
			wakeup((caddr_t)ip);
		}
		ASSERT(rmilock(ip) == 0);
#else
		prele(ip);
#endif
	}
	return(tip);
found:
	if (ip->i_flag & ILOCK) {
		ip->i_flag |= IWANT;
		sleep(ip, PINOD);
		goto loop;
	}

	if (ip->i_count == 0) {
		/* remove from freelist */
		ASSERT(ip->av_back->av_forw == ip);
		ASSERT(ip->av_forw->av_back == ip);

		ip->av_back->av_forw = ip->av_forw;
		ip->av_forw->av_back = ip->av_back;
	}
	ip->i_count++;
	ip->i_flag |= ILOCK;
	ASSERT(addilock(ip) == 0);
#ifdef DEBUG
	ilog(ip, 1, caller);
#endif
	return(ip);
}
E 1
