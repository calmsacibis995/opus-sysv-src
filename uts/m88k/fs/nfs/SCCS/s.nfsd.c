h53282
s 00587/00000/00000
d D 1.1 90/03/06 12:43:34 root 1 0
c date and time created 90/03/06 12:43:34 by root
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
static char SysVr3NFSID[] = "@(#)nfsd.c	4.2 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Some code from this module taken from Convergent Technologies nfsd.c:
 *
 *	Convergent Technologies - System V - Nov 1986
 * #ident	"@(#)nfsd.c	1.6 :/source/uts/common/fs/nfs/s.nfsd.c 1/20/87 00:04:26"
 *
 * #ifdef LAI
 * static char SysVNFSID[] = "@(#)nfsd.c	2.11 LAI System V NFS Release 3.0/V3 source";
 * #endif
 */

/*
 * Interface code for the NFS device.
 */

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
#if !m88k
#include "sys/inline.h"
#endif
#include "sys/buf.h"
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#include "sys/immu.h"
#endif
#include "sys/region.h"
#include "sys/proc.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"
#include "sys/fs/nfs/nfs_ioctl.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#if !m88k
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

/*
 * nfsdioctl
 *
 * This is an alternate interface for the NFS system calls.  They can
 * either be put into sysent[] and invoked like normal system calls
 * or they can be invoked by doing the proper ioctl on /dev/nfsd.
 *
 * The supported ioctls are:
 *	nfs_svc() - kernel server side daemon entry point
 *	getfh() - returns a file handle for the associated open file
 *	async_daemon() - kernel client side buffer daemon
 */
/* ARGSUSED */
nfsdioctl(dev, cmd, argp, flag)
	dev_t dev;
	int cmd;
	int argp;
	int flag;
{
	struct getfh_args getfh_args;
	struct domnam_args domnam_args;

	switch (cmd) {

	case NIOCNFSD:
		/*
		 * Start up server nfs.
		 */
		server_daemon(argp);
		break;

	case NIOCGETFH:
		/*
		 * Obtain file handle.
		 */
		if(copyin((caddr_t)argp, (caddr_t)&getfh_args,
			  sizeof(getfh_args))) {
			u.u_error = EFAULT;
			return;
		}
		getfilehandle(getfh_args.fd, getfh_args.fhp);
		break;

	case NIOCASYNCD:
		/*
		 * Async_daemon
		 */
		async_daemon();
		break;

	case NIOCSETDOMNAM:
		/*
		 * Set the yellow pages domain name
		 */
		if (copyin((caddr_t)argp, (caddr_t)&domnam_args,
			   sizeof(domnam_args))) {
			u.u_error = EFAULT;
			return;
		}
		setdomainname(domnam_args.name, domnam_args.namelen);
		break;

	case NIOCGETDOMNAM:
		/*
		 * Get the yellow pages domain name
		 */
		if (copyin((caddr_t)argp, (caddr_t)&domnam_args,
			   sizeof(domnam_args))) {
			u.u_error = EFAULT;
			return;
		}
		getdomainname(domnam_args.name, domnam_args.namelen);
		break;

	default:
		u.u_error = EINVAL;
		break;
	}
}

static struct buf *async_bufhead = 0;
static int async_daemon_count = 0;

/*
 * Fill or empty a buffer.  This is the strategy routine for client side I/O.
 */
nfsdstrategy(bp)
	register struct buf *bp;
{
	register struct rnode *rp = bptor(bp);

	/*
	 * If there was an asynchronous write error on this rnode
	 * then we just return the old error code. This continues
	 * until the rnode goes away (zero ref count). We do this because
	 * there can be many procs writing this rnode.
	 */
	if (rp->r_error) {
		bp->b_error = rp->r_error;
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}

	if (async_daemon_count && bp->b_flags & B_ASYNC) {
		if (async_bufhead != NULL) {
			async_bufhead->av_back->av_forw = bp;
			bp->av_back = async_bufhead->av_back;
			async_bufhead->av_back = bp;
		} else {
			async_bufhead = bp;
			async_bufhead->av_back = bp;
		}
		bp->av_forw = NULL;
		wakeup((caddr_t)&async_bufhead);
	} else {
		do_bio(bp);
	}
}

#if SBUFSIZE < NFS_MAXDATA
/* copied from os/bio.c */
#define LTOPBLK(blkno, bsize)	((blkno) * ((bsize) >> SCTRSHFT))
#endif

async_daemon()
{
	register struct buf *bp;

	for (;;) {
		async_daemon_count++;
		while (async_bufhead == NULL) {
			if (sleep((caddr_t)&async_bufhead, (PZERO+1)|PCATCH)) {
				async_daemon_count--;
				return;
			}
		}
		async_daemon_count--;
		bp = async_bufhead;
		if (bp == NULL) {
			continue;
		}
		if (bp->av_forw != NULL) {
			bp->av_forw->av_back = bp->av_back;
		}
		async_bufhead = bp->av_forw;
#if SBUFSIZE < NFS_MAXDATA
		bp->av_forw = NULL;
		/*
		 * Attempt to cluster buffer writes.  This is to help
		 * client write performance.  The basic strategy is to
		 * look through the buffer chain pointed to by async_bufhead
		 * looking for adjacent blocks which need to get written.
		 * Queue a maximum of NFS_MAXDATA/SBUFSIZE buffers of size
		 * SBUFSIZE.  If there is more than 1 buffer that can be
		 * written out, invoke nfs_bwrite() to write out the chain.
		 * A little trickiness: the block numbers in the buffer
		 * header are physical block numbers and not logical block
		 * numbers.  Hence, the calculation for incr.
		 */
		if ((bp->b_flags&B_READ) == B_WRITE && async_bufhead != NULL) {
			register struct buf *bp1, *sbp;
			daddr_t blkno, incr;
			int n;

			bp1 = async_bufhead;
			bp->av_back = bp;
			blkno = bp->b_blkno;
			incr = LTOPBLK(1, SBUFSIZE);
			n = (NFS_MAXDATA / SBUFSIZE) - 1;
			while (bp1 != NULL && n > 0) {
				if (bp1->b_blkno == blkno + incr &&
				    bp1->b_dev == bp->b_dev &&
				    (bp1->b_flags&B_READ) == B_WRITE) {
					/*
					 * Can chain this one.  Take it
					 * off of the list pointed to by
					 * async_bufhead and put it on
					 * the list pointed to by bp.
					 */
					sbp = bp1->av_forw;
					if (bp1 == async_bufhead) {
						if (bp1->av_forw != NULL) {
							bp1->av_forw->av_back =
								bp1->av_back;
							}
						async_bufhead = bp1->av_forw;
					} else {
						bp1->av_back->av_forw =
							bp1->av_forw;
						if (bp1->av_forw != NULL) {
							bp1->av_forw->av_back =
								bp1->av_back;
						}
					}
					bp->av_back->av_forw = bp1;
					bp->av_back = bp1;
					bp1->av_forw = NULL;
					blkno = bp1->b_blkno;
					n--;
					bp1 = sbp;
				} else {
					bp1 = bp1->av_forw;
				}
			}
		}
		/*
		 * If reading, then call nfs_bread() to read in the block.
		 * nfs_bread() attempts to do NFS_MAXDATA reads to increase
		 * read efficiency.
		 */
		if ((bp->b_flags & B_READ) == B_READ)
			nfs_bread(bp);
		/*
		 * Else if writing and if we got more than one, then use
		 * nfs_bwrite() to write out the buffer chain.  Otherwise,
		 * fall into the call to do_bio() which is more efficient
		 * for writing out one buffer.
		 */
		else if ((bp->b_flags & B_READ) == B_WRITE &&
			 bp->av_forw != NULL)
			nfs_bwrite(bp);
		/*
		 * Else simple, just invoke do_bio() to write out one buffer.
		 */
		else
			do_bio(bp);
#else
		do_bio(bp);
#endif
	}
}

do_bio(bp)
	register struct buf *bp;
{
	register struct inode *ip;
	register struct rnode *rp = bptor(bp);
	int resid, count;

	ip = rp->r_ip;
	ASSERT(ip != NULL);

	if ((bp->b_flags & B_READ) == B_READ) {
		bp->b_error = nfs_read(ip,
				       bp->b_un.b_addr,
				       bp->b_blkno << SCTRSHFT,
				       (int)bp->b_bcount,
				       &resid,
				       &rp->r_cred);
		if (bp->b_error == 0) {
			if (resid) {
				bzeroba(bp->b_un.b_addr + bp->b_bcount - resid,
					resid);
			}
		}
	} else {
		/*
		 * If the write fails and it was asynchronous
		 * all future writes will get an error.
		 */
		if (rp->r_error == 0) {
			count = MIN(bp->b_bcount,
				    ip->i_size - (bp->b_blkno << SCTRSHFT));
			if (count < 0) {
				cmn_err(CE_PANIC, "do_bio: write count < 0");
			}
			bp->b_error = nfs_write(ip,
						bp->b_un.b_addr,
						bp->b_blkno << SCTRSHFT,
						count,
						&rp->r_cred);
			if (bp->b_flags & B_ASYNC) {
				rp->r_error = bp->b_error;
			}
		} else {
			bp->b_error = rp->r_error;
		}
	}

	if (bp->b_error) {
		bp->b_flags |= B_ERROR;
	}

	iodone(bp);
}

#if SBUFSIZE < NFS_MAXDATA
/*
 * Write a chain of buffers.  The basic strategy used is to copy
 * the buffers into a large buffer and then nfs_write() it.
 * Some trickiness exists because if a buffer is marked as
 * asynchronous, iodone() calls to brelse() to release the buffer.
 * This changes the av_forw pointer which is also used to chain
 * this list.
 */
nfs_bwrite(bp)
	register struct buf *bp;
{
	struct inode *ip;
	struct rnode *rp = bptor(bp);
	struct buf *bp1, *sbp;
	caddr_t buf, bufp;
	int size, count, error, n;

	/*
	 * If an error has occurred doing asynchronous i/o on this
	 * rnode in the past, we assume that it will happen again.
	 * Simply mark all the buffers as having an error and fill
	 * in the error code.  Then release them, via iodone().
	 */
	if (rp->r_error) {
		bp1 = bp;
		while (bp1 != NULL) {
			bp1->b_error = rp->r_error;
			bp1->b_flags |= B_ERROR;
			sbp = bp1->av_forw;
			iodone(bp1);
			bp1 = sbp;
		}
		return;
	}

	ip = rp->r_ip;
	ASSERT(ip != NULL);

	/*
	 * Figure out how many bytes of i/o we need to do.
	 * The number of bytes is either the cumulative size
	 * of all the buffers or the size of the rest of the
	 * file, whichever is smaller.
	 */
	for (size = 0, bp1 = bp; bp1 != NULL; bp1 = bp1->av_forw) {
		size += bp1->b_bcount;
	}
	size = MIN(size, ip->i_size - (bp->b_blkno << SCTRSHFT));
	if (size <= 0) {
		cmn_err(CE_PANIC, "nfs_bwrite: size <= 0");
	}

	/*
	 * Allocate a buffer big enough to hold all the data.
	 */
	buf = kmem_alloc((uint)size);

	/*
	 * Loop through all the buffers copying them into the
	 * buffer allocated above.
	 */
	count = size;
	bufp = buf;
	bp1 = bp;
	while (count > 0) {
		n = MIN(bp1->b_bcount, count);
		bcopy(bp1->b_un.b_addr, bufp, n);
		count -= n;
		bufp += n;
		bp1 = bp1->av_forw;
	}

	/*
	 * Write the buffer.
	 */
	error = nfs_write(ip, buf, bp->b_blkno << SCTRSHFT, size, &rp->r_cred);

	/*
	 * Free the kmem_alloc'd buffer, we do not need it anymore.
	 */
	kmem_free(buf, (uint)size);

	/*
	 * If an error occurred, mark all the buffers as having an
	 * error and then release them using iodone().  Also fill
	 * in r_error.  If no error has occurred, then just release
	 * all the buffers using iodone().
	 */
	if (error) {
		rp->r_error = error;
		bp1 = bp;
		while (bp1 != NULL) {
			bp1->b_error = error;
			bp1->b_flags |= B_ERROR;
			sbp = bp1->av_forw;
			iodone(bp1);
			bp1 = sbp;
		}
	} else {
		bp1 = bp;
		while (bp1 != NULL) {
			sbp = bp1->av_forw;
			iodone(bp1);
			bp1 = sbp;
		}
	}
}

#define PTOLBLK(blkno, bsize)   ((blkno) / ((bsize) >> SCTRSHFT))

nfs_bread(bp)
	register struct buf *bp;
{
	daddr_t lbn;
	int size, bsize, maxnb, resid;
	register int csize, count;
	caddr_t buf;
	register caddr_t bufp;
	struct rnode *rp;
	struct inode *ip;
	int error;

	/*
	 * Size is the size of the buffers in the system.
	 * Lbn is the logical block number.  The block numbers in the
	 *	buffer header are physical block numbers.  However, all
	 *	getblk()'s and such are done using logical block numbers.
	 * Maxnb is the maximum number of buffers that will fit into
	 *	an NFS_MAXDATA size request.
	 * Bsize is the size of the block that we will try to read.
	 *	We assume that block reads will begin/end on NFS_MAXDATA
	 *	size boundaries.  This is simplify the read-ahead.
	 * Buf is a pointer to the read buffer.
	 * Rp and ip are the rnode and inodes to be used during the i/o.
	 */
	size = bp->b_bcount;
	lbn = PTOLBLK(bp->b_blkno, size);
	maxnb = NFS_MAXDATA / size;
	bsize = (maxnb - (lbn % maxnb)) * size;
	buf = kmem_alloc((uint)bsize);
	rp = bptor(bp);
	ip = rp->r_ip;
	ASSERT(ip != NULL);
	/*
	 * Do the read.
	 */
	error = nfs_read(ip, buf, bp->b_blkno << SCTRSHFT, bsize, &resid,
			 &rp->r_cred);
	/*
	 * If an error occurs, free the kmem_alloc'd buffer, mark the
	 * buffer as having an error, and call iodone() to mark the i/o
	 * complete.
	 */
	if (error) {
		kmem_free(buf, (uint)bsize);
		bp->b_flags |= B_ERROR;
		bp->b_error = error;
		iodone(bp);
		return;
	}
	/*
	 * Csize of the amount of data to copy to a number of buffers.
	 */
	csize = bsize - resid;
	/*
	 * If nothing read, simply free the kmalloc'd buffer and
	 * call iodone() to mark the i/o complete.
	 */
	if (csize == 0) {
		kmem_free(buf, (uint)bsize);
		clrbuf(bp);
		iodone(bp);
		return;
	}
	/*
	 * Fill the first buffer that we were passed.  If we are not
	 * filling the entire buffer, we zero out the unused portion
	 * of the buffer.  We use bzeroba() instead of bzero() because
	 * bzero() requires the arguments to be int aligned.  This may
	 * not necessarily be true if we are reading an odd-sized file.
	 */
	bufp = buf;
	count = MIN(csize, size);
	bcopy(bufp, bp->b_un.b_addr, count);
	if (count != size)
		bzeroba(bp->b_un.b_addr + count, size - count);
	iodone(bp);
	csize -= count;
	bufp += count;
	lbn++;
	/*
	 * Copy the rest of the data that was read into a string of
	 * allocated system buffers.
	 */
	while (csize > 0) {
		count = MIN(csize, size);
		if (!incore(ip->i_dev, lbn, size)) {
			bp = getblk(ip->i_dev, lbn, size);
			bcopy(bufp, bp->b_un.b_addr, count);
			if (count != size)
				bzeroba(bp->b_un.b_addr + count, size - count);
			bp->b_flags |= B_READ|B_DONE;
			bp->b_bcount = size;
			brelse(bp);
		}
		csize -= count;
		bufp += count;
		lbn++;
	}
	/*
	 * Free the allocated buffer.
	 */
	kmem_free(buf, (uint)bsize);
}
#endif
E 1
