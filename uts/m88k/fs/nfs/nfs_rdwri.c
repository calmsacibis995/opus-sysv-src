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
static char SysVr3NFSID[] = "@(#)nfs_rdwri.c	4.4 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/fcntl.h"
#include "sys/flock.h"
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
#include "sys/buf.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#if !m88k
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

/*
 * Client side file operations.
 */

/*
 * Read from the passed inode.
 */
nfsreadi(ip)
	register struct inode *ip;
{

	nfs_unmark_iget(itor(ip));

	nfs_rdwr(ip, B_READ);
}

/*
 * Write to the passed inode.
 */
nfswritei(ip)
	register struct inode *ip;
{

	nfs_unmark_iget(itor(ip));

	nfs_rdwr(ip, B_WRITE);
}

/*
 * Read and write a NFS file.
 */
static
nfs_rdwr(ip, rw)
	register struct inode *ip;
	int rw;
{
	int error = 0;
	struct rnode *rp = itor(ip);
	if (ip->i_ftype != IFREG && ip->i_ftype != IFDIR) {
		u.u_error = EACCES;
		return;
	}

	if (rw == B_WRITE || (rw == B_READ && rp->r_cred.cr_ref == 0)) {
		set_cred(&rp->r_cred);
	}

	/* Set to end for append mode. */
	if ((u.u_fmode & FAPPEND) && rw == B_WRITE) {
		if (error = nfs_getattr(ip, &rp->r_cred)) {
			u.u_error = error;
			return;
		}
		u.u_offset = ip->i_size;
	}

	/* XXX - If mandatory locked file, report deadlock. */
	if ((ip->i_ftype == IFREG) && ! nfsaccess(ip, IMNDLCK)) {
		u.u_error = EDEADLK;
		return;
	}
	/* rwip() does the work. */
	error = rwip(ip, rw);

	if (error && ! u.u_error) {
		u.u_error = error;
	}
	return;
}

/*
 * Do the work for read and write.  Adapted from Sun's rwvp().
 */
static int
rwip(ip, rw)
	register struct inode *ip;
	int rw;
{
	struct buf *bp;
	struct rnode *rp;
	register int n, on;
	daddr_t bn;
	int size;
	register int error = 0;
	int loop_cnt;
	int eof = 0;
#if SBUFSIZE < NFS_MAXDATA
	daddr_t rablkno;
#endif

	if (u.u_count == 0) {		/* quit if nothing to do */
		return(0);
	}
	if (u.u_offset < 0 || (u.u_offset + (off_t)u.u_count) < 0) {
		return(EINVAL);
	}
	size = SBUFSIZE;
#if SBUFSIZE < NFS_MAXDATA
	rablkno = NFS_MAXDATA / size;
#endif
	/*
	 * Deal with one block per trip around loop.
	 */
	loop_cnt = 0;
	rp = itor(ip);
	do {
		loop_cnt++;

		bn = u.u_offset / size;
		on = u.u_offset % size;
		n = MIN((size - on), u.u_count);
		if (rp->r_flags & RNOCACHE) {
			bp = geteblk();
			clrbuf(bp);
			if (rw == B_READ) {
				int resid;

				error = nfs_read(ip, bp->b_un.b_addr + on,
						 u.u_offset, n, &resid,
						 &rp->r_cred);
				if (error) {
					brelse(bp);
					goto bad;
				}
				if (resid) {
					bzeroba(bp->b_un.b_addr + n - resid,
						resid);
				}
			}
		} else if (rw == B_READ) {
			if (incore(ip->i_dev, bn, size)) {
				/*
				 * Get attributes to check whether
				 * incore data is stale.  Stale data
				 * will get flushed.
				 */
				(void) nfs_getattr(ip, &rp->r_cred);
			}
			/*
			 * Check for read-ahead.
			 */
#if SBUFSIZE < NFS_MAXDATA
			if (rp->r_lastr + 1 == bn && (bn % rablkno) == 0) {
				bp = breada(ip->i_dev, bn, bn + rablkno, size);
			} else {
				bp = bread(ip->i_dev, bn, size);
			}
#else
			if (rp->r_lastr + 1 == bn)
				bp = breada(ip->i_dev, bn, bn + 1, size);
			} else {
				bp = bread(ip->i_dev, bn, size);
			}
#endif

			/*
			 * Save the read-ahead address for the next read
			 */
			rp->r_lastr = bn;
		} else {		/* this must be a write */
			if (u.u_offset >= (u.u_limit << SCTRSHFT)) {
				if (loop_cnt > 1) {	/* past max file */
					error = EFBIG;	/* so error */
				}
				goto bad;
			}
			if (rp->r_error) {
				error = rp->r_error;
				goto bad;
			}
			if (n == size) {
				bp = getblk(ip->i_dev, bn, size);
			} else {
				bp = bread(ip->i_dev, bn, size);
			}
		}
		if (bp->b_flags & B_ERROR) {
			error = bp->b_error;
			if (error == 0)
				error = EIO;
			brelse(bp);
			goto bad;
		}
		if (rw == B_READ) {
			int diff;

			/*
			 * Now, figure how bytes we can actually use
			 * from this buffer.  We can find this by
			 * subtracting the current offset in the file
			 * from the size of the file.  If this is less
			 * than or equal to 0 than we have hit EOF.
			 * Otherwise, we have only a partial block
			 * left and so adjust the count (n) accordingly.
			 */
			diff = ip->i_size - u.u_offset;
			if (diff <= 0) {
				brelse(bp);
				return(0);
			}
			if (diff <= n) {
				n = diff;
				eof = 1;
			}
		}

		iomove(bp->b_un.b_addr + on, n, rw);

		if (rw == B_READ) {
			brelse(bp);
		} else {
			/*
			 * i_size is the maximum number of bytes known
			 * to be in the file.
			 * Make sure it is at least as high as the last
			 * byte we just wrote into the buffer.
			 */
			if (ip->i_size < u.u_offset) {
				ip->i_size = u.u_offset;
			}
			if (u.u_error) {
				brelse(bp);
			} else if (rp->r_flags & RNOCACHE) {
				error = nfs_write(ip, bp->b_un.b_addr + on,
						  u.u_offset - n, n,
						  &rp->r_cred);
				brelse(bp);
			} else {
				rp->r_flags |= RDIRTY;
				if (u.u_fmode & FSYNC) {
					bwrite(bp);
				} else if (n + on == size) {
					bawrite(bp);
				} else {
					bdwrite(bp);
				}
			}
		}
	} while (u.u_error == 0 && u.u_count > 0 && !eof);

	if (error == 0)
		error = u.u_error;
bad:
	return(error);
}

/*
 * Write to file.
 * Writes to remote server in largest size chunks that the server can
 * handle.  Write is synchronous.
 */
nfs_write(ip, base, offset, count, cred)
	struct inode *ip;
	caddr_t base;
	int offset;
	int count;
	struct ucred *cred;
{
	int error;
	int tsize;
	struct nfsattrstat *ns;
	struct nfswriteargs wa;

	ns = (struct nfsattrstat *)kmem_alloc((uint)sizeof(*ns));
	do {
		tsize = MIN(itomi(ip)->mi_stsize, count);
		wa.wa_data = base;
		wa.wa_fhandle = *itofh(ip);
		wa.wa_begoff = offset;
		wa.wa_totcount = tsize;
		wa.wa_count = tsize;
		wa.wa_offset = offset;
		error = rfscall(itomi(ip), RFS_WRITE,
				xdr_writeargs, (caddr_t)&wa,
				xdr_attrstat, (caddr_t)ns, cred);
		if (!error) {
			error = geterrno(ns->ns_status);
		}
		count -= tsize;
		base += tsize;
		offset += tsize;
	} while (!error && count);

	if (!error) {
		nfs_attrcache(ip, &ns->ns_attr, NOFLUSH);
	}
	kmem_free((caddr_t)ns, (uint)sizeof(*ns));
	switch (error) {
	case 0:
		break;

	case ENOSPC:
		cmn_err(CE_WARN,
			"nfs_write: NFS write error on host %s, remote file system full",
			itomi(ip)->mi_hostname);
		break;

	case EINTR:
		/* don't say anything if we got interrupted */
		break;

	default:
		cmn_err(CE_WARN,
			"nfs_write: NFS write error on host %s, error %d",
			itomi(ip)->mi_hostname, error);
		break;
	}
	return(error);
}

/*
 * Read from a file over the wire.
 * Reads data in largest chunks our interface can handle.
 */
nfs_read(ip, base, offset, count, residp, cred)
	struct inode *ip;
	caddr_t base;
	int offset;
	int count;
	int *residp;
	struct ucred *cred;
{
	register int error;
	register int tsize;
	struct nfsreadargs ra;
	struct nfsrdresult rr;

	do {
		tsize = MIN(itomi(ip)->mi_tsize, count);
		rr.rr_data = base;
		ra.ra_fhandle = *itofh(ip);
		ra.ra_offset = offset;
		ra.ra_totcount = tsize;
		ra.ra_count = tsize;
		error = rfscall(itomi(ip), RFS_READ,
				xdr_readargs, (caddr_t)&ra,
				xdr_rdresult, (caddr_t)&rr, cred);
		if (!error) {
			error = geterrno(rr.rr_status);
		}
		count -= rr.rr_count;
		base += rr.rr_count;
		offset += rr.rr_count;
	} while (!error && count && rr.rr_count == tsize);

	*residp = count;

	if (!error) {
		nfs_attrcache(ip, &rr.rr_attr, SFLUSH);
	}
	return(error);
}

