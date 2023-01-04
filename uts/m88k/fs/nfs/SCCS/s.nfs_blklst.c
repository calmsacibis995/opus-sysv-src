h29718
s 00176/00000/00000
d D 1.1 90/03/06 12:43:26 root 1 0
c date and time created 90/03/06 12:43:26 by root
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
static char SysVr3NFSID[] = "@(#)nfs_blklst.c	4.3 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/mount.h"
#include "sys/signal.h"
#ifdef u3b2 
#include "sys/psw.h" 
#include "sys/pcb.h" 
#endif
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/debug.h"
#include "sys/buf.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

/*
 * Allocate and build the block address map
 * There is nothing to do for nfs files.
 */
nfsallocmap(ip)
	register struct inode *ip;
{
	register struct rnode *rp = itor(ip);

	ASSERT(rp == &nfs_rnodes[ip->i_number] && ip->i_number < nfs_rnum);

	nfs_unmark_iget(rp);

	return(1);
}

/*
 * Read page from a file.
 *
 * return # of bytes read if no error occurs
 * return -1 - when read error occurs
 */
nfsreadmap(ip, offset, size, vaddr, segflg)
	register struct inode *ip;
	off_t offset;
	int size;
	caddr_t vaddr;
	int segflg;
{
	register struct buf *bp;
	register int bsize;
	register daddr_t bn;
	int ret, on, n;
#if SBUFSIZE < NFS_MAXDATA
	daddr_t rablkno;
#endif

	nfs_unmark_iget(itor(ip));

	if (offset > ip->i_size) {
		u.u_error = EINVAL;
		return(-1);
	}

	if (offset+size > ip->i_size) {
		size = ip->i_size - offset;
	}

	ret = size;
	bsize = SBUFSIZE;
#if SBUFSIZE < NFS_MAXDATA
	rablkno = NFS_MAXDATA / SBUFSIZE;
#endif
	bn = offset / bsize;
	on = offset - (bn * bsize);

	do {
#if SBUFSIZE < NFS_MAXDATA
		if (size > bsize && (bn + 1) % rablkno == 0) {
			bp = breada(ip->i_dev, bn, bn+1, bsize);
		} else {
			bp = bread(ip->i_dev, bn, bsize);
		}
#else
		if (size > bsize) {
			bp = breada(ip->i_dev, bn, bn + 1, bsize);
		} else {
			bp = bread(ip->i_dev, bn, bsize);
		}
#endif
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			return(-1);
		}
		n = bsize - on;
		if (n > size)
			n = size;
		if (segflg != 1) {
			if (copyout(bp->b_un.b_addr + on, vaddr, n)) {
				u.u_error = EFAULT;
				brelse(bp);
				return(-1);
			}
		} else {
			bcopy(bp->b_un.b_addr + on, vaddr, n);
		}
		brelse(bp);
		bn++;
		size -= n;
		offset += n;
		vaddr += n;
		on = 0;
	} while (size > 0);

	return(ret);
}

/*
 * Free the block list attached to an inode.
 *
 * Since NFS files do not have 'maps', there is not a lot of work
 * to do here.
 */
nfsfreemap(ip)
	register struct inode *ip;
{
	register struct rnode *rp = itor(ip);

	nfs_unmark_iget(rp);

	ASSERT(ip->i_count == 0);
	ASSERT(!(rp->r_flags & RDIRTY));	/* forced by close */

	nfs_pflush(ip);
}

/*
 * Purge an inode from the page cache.
 */
nfs_pflush(ip)
	register struct inode *ip;
{
	register struct rnode *rp = itor(ip);

	ASSERT(rp >= nfs_rnodes && (rp - nfs_rnodes) < nfs_rnum);

	flushpgch(ip);		/* flush the page cache */
}
E 1
