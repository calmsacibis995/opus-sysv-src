h47749
s 00157/00000/00000
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
static char SysVr3NFSID[] = "@(#)nfs_sys2.c	4.2 LAI System V NFS Release 3.2/V3	source";
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
#include "sys/dirent.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#if !m88k
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

/*
 * Read directory entries.
 */
nfsgetdents(ip, bufpt, bufsz)
	register struct inode *ip;
	char *bufpt;
	int bufsz;			/* the size of the user buffer */
{
	int error = 0;
	int num;			/* # of bytes sent to user */
	int count;
	uint bsize;			/* directory buffer size */
	struct rnode *rp;
	register struct dirent *nxt;
	register struct dirent *lim;
	caddr_t strt;
	struct ucred cred;
	struct nfsrddirargs rda;	/* buffer to the wire */
	struct nfsrddirres rd;		/* buffer from the wire */

	rp = itor(ip);

	nfs_unmark_iget(rp);

	set_cred(&cred);

	/*
	 * Check to make sure that our cached attributes are still fresh.
	 */
	error = nfs_getattr(ip, &cred);
	if (error) {
		u.u_error = error;
		return(-1);
	}

	if ((rp->r_flags & REOF) &&	/* if we have see EOF here, */
	    ip->i_size == u.u_offset) {	/* & are still at it */
		return(0);		/* then quit now */
	}

	bsize = MIN(itoblksz(ip), itomi(ip)->mi_tsize);
	rd.rd_entries = (struct dirent *)kmem_alloc(bsize);
	rda.rda_fh = *itofh(ip);
	num = 0;
	do {			/* read blocks until user is happy */
		rda.rda_offset = u.u_offset;
		rd.rd_size = bsize;
		rda.rda_count = bsize;

		error = rfscall(itomi(ip), RFS_READDIR,
				xdr_rddirargs, (caddr_t)&rda,
				xdr_getrddirres, (caddr_t)&rd, &cred);
		if (!error)
			error = geterrno(rd.rd_status);
		if (error)
			break;

		if (rd.rd_eof) {	/* notice EOF so we will not do RPC */
			rp->r_flags |= REOF;	/* if we do not need to */
		}
		if (!rd.rd_size)
			break;	/* quit if nothing received */

		nxt = rd.rd_entries;
		lim = (struct dirent *)((char *)rd.rd_entries + rd.rd_size);
		count = 0;
		strt = (caddr_t)nxt;
		while (nxt < lim) {	/* count the size of entries that fit */
			ASSERT(nxt->d_reclen > 0);
			if (num + count + nxt->d_reclen > bufsz)
				break;
			rd.rd_offset = nxt->d_off;
			count += nxt->d_reclen;
			nxt = (struct dirent *)((char *)nxt + nxt->d_reclen);
		}

		if (count) {
			if (u.u_segflg != 1) {
				if (copyout(strt, bufpt, count)) {
					error = EFAULT;
					break;
				}
			} else {
				bcopy(strt, bufpt, count);
			}
			num += count;
			bufpt += count;
		}
		u.u_offset = rd.rd_offset;
	} while (!rd.rd_eof &&			  /* stop at EOF */
		 rda.rda_offset < rd.rd_offset && /* or if no progress */
		 num == 0);			  /* and no data yet */

	kmem_free((caddr_t)rd.rd_entries, bsize);

	if (error) {
		u.u_error = error;
		num = -1;
	}
	return(num);
}
E 1
