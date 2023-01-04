h52758
s 00003/00003/00264
d D 1.2 90/04/05 09:58:12 root 2 1
c removed cr's from cmn_err's
e
s 00267/00000/00000
d D 1.1 90/03/06 12:28:05 root 1 0
c date and time created 90/03/06 12:28:05 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/physio.c	10.7"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/sysinfo.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/immu.h"
#include "sys/errno.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/inode.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/getpages.h"
#include "sys/swap.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
#include "sys/tuneable.h"

#define	SLEEP	0		/* Sleep to get buffer header */
#define NOSLP	1		/* Return immediately if no bp's available */

buf_t*
getpbp(slpflg)
{
	register buf_t* bp = NULL;
	register ospl;

	ospl = spl6();
	while (pfreecnt == 0) {
		if (slpflg)
			goto out;
		pfreelist.b_flags |= B_WANTED;
		sleep(&pfreelist, PRIBIO);
	}
	ASSERT(pfreecnt);
	ASSERT(pfreelist.av_forw);
	pfreecnt--;
	bp = pfreelist.av_forw;
	pfreelist.av_forw = bp->av_forw;
out:
	splx(ospl);
	return(bp);
}

freepbp(bp)
register buf_t* bp;
{
	register ospl;

	ospl = spl6();
	bp->av_forw = pfreelist.av_forw;
	pfreelist.av_forw = bp;
	pfreecnt++;
	if (pfreelist.b_flags & B_WANTED)
		wakeup(&pfreelist);
	pfreelist.b_flags = 0;
	splx(ospl);
}

/*
 * swap I/O
 */

swap(pglptr, npage, rw)
register pglst_t	*pglptr;
{
	register buf_t	*bp;
	register dev_t	dev;
	register int	i;
	register dbd_t	*dbd;
	caddr_t		swapkva;
	register pde_t	*swappde;

	ASSERT(syswait.swap >= 0);
	syswait.swap++;

	u.u_iosw++;
	if (rw) {
		sysinfo.swapin++;
		sysinfo.bswapin += npage;
	} else {
		sysinfo.swapout++;
		sysinfo.bswapout += npage;
	}

	dcacheflush(0);
	bp = getpbp(SLEEP);
	swapkva = (caddr_t) sptalloc(npage, 0, -1);
	swappde = kvtopde(swapkva);
	dbd = (dbd_t *)(pglptr->gp_ptptr + NPGPT * NPDEPP);
	dev = swaptab[dbd->dbd_swpi].st_dev;

	ASSERT(dbd->dbd_blkno >= swaptab[dbd->dbd_swpi].st_swplo);

	bp->b_flags = B_KPHYS | B_BUSY | rw;
	bp->b_dev = dev;
	bp->b_blkno = dbd->dbd_blkno;
	bp->b_bcount = ctob(npage);
	bp->b_un.b_addr = (caddr_t) swapkva;

	for (i = 0;  i < npage;  i++, pglptr++)
		swappde[i] = *(pglptr->gp_ptptr);
	(*bdevsw[bmajor(dev)].d_strategy)(bp);

	iowait(bp);
	bp->b_flags &= ~B_KPHYS;
	if (bp->b_flags & B_ERROR) {
D 2
	   cmn_err(CE_WARN,"swap: bp = 0x%x\n",
E 2
I 2
	   cmn_err(CE_WARN,"swap: bp = 0x%x",
E 2
		bp);
D 2
	   cmn_err(CE_WARN,"swap: dev  = 0x%x, blkno = 0x%x\n",
E 2
I 2
	   cmn_err(CE_WARN,"swap: dev  = 0x%x, blkno = 0x%x",
E 2
		bp->b_dev, bp->b_blkno);
D 2
	   cmn_err(CE_WARN,"swap: addr - 0x%x, flags = 0x%x\n",
E 2
I 2
	   cmn_err(CE_WARN,"swap: addr - 0x%x, flags = 0x%x",
E 2
		bp->b_un.b_addr, bp->b_flags);
	   cmn_err(CE_PANIC, "swap - i/o error in swap");
	}

	/*	Free up the buffer headers
	 */

	freepbp(bp);
	sptfree(swapkva, npage, 0);

	ASSERT(syswait.swap);
	syswait.swap--;
}

/*
 * Raw I/O. The arguments are
 * The strategy routine for the device
 * A buffer, which is usually NULL, or special buffer
 *   header owned exclusively by the device for this purpose
 * The device number
 * Read/write flag
 */

physio(strat, bp, dev, rw)
register struct buf *bp;
int (*strat)();
{	register int		count;
	register caddr_t	base;
	register caddr_t	lbase;
	register int		hpf;
	register int		cc;
	register char		oldsegflg;

	count = u.u_count;
	if ((availrmem - btoc(count)) < tune.t_minarmem) {
		nomemmsg("physio",  btoc(count), 0, 0);
		u.u_error = ENOMEM;
		return;
	}
	base = u.u_base;
	if (server()) {
		if ((lbase = (caddr_t)sptalloc(btoc(count), PG_VALID, 0)) == 0)
			return;
		if (rw == B_WRITE) {
			if (remio(base, lbase, count)) {
				sptfree(lbase, btoc(count), 1);
				return;
			}
		}
		u.u_base = lbase;
		oldsegflg = u.u_segflg;
		u.u_segflg = 1;	/*you're now dealing with kernel space*/
	} else {
		if (userdma(base, count, rw) == NULL) {
			if (u.u_error == 0)
				u.u_error = EFAULT;
			return;
		}
	}

	ASSERT(syswait.physio >= 0);
	syswait.physio++;
	if (rw)
		sysinfo.phread++;
	else
		sysinfo.phwrite++;

	/*	If we need a buffer header,
	 *	get a buffer header off of the free list.
	 */

	hpf = (bp == NULL);
	if (hpf)
		bp = getpbp(SLEEP);

	if (server())
		/* 
		   Opus uses B_PHYS to mean UVA.
		   Here we are dealing with a KVA.
		*/
		bp->b_flags = B_BUSY | B_KPHYS | rw;
	else
		bp->b_flags = B_BUSY | B_PHYS | rw;
	bp->b_error = 0;
	bp->b_dev = dev;
	bp->b_un.b_addr = u.u_base;
	bp->b_blkno = btod(u.u_offset);
#ifdef CMMU
	dcacheflush(0);
#endif
	bp->b_bcount = u.u_count;

	(*strat)(bp);

	iowait(bp);

	if (server()) {
		if (rw == B_READ) {
			u.u_segflg = oldsegflg;
			if (unremio(lbase, base, count))
				printf("unremio failed: err=%d\n",u.u_error);
		}
		sptfree(lbase, btoc(count), 1);
	}
	else
		undma(base, count, rw);

	bp->b_flags &= ~(B_BUSY|B_PHYS|B_KPHYS);

	if (hpf)
		freepbp(bp);

	u.u_count = bp->b_resid;
	ASSERT(syswait.physio);
	syswait.physio--;
}


physck(nblocks, rw)
daddr_t nblocks;
{
	register unsigned over;
	register off_t upper, limit;
	struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap;

	limit = nblocks << SCTRSHFT;
	if (u.u_offset >= limit) {
		if (u.u_offset > limit || rw == B_WRITE)
			u.u_error = ENXIO;
		return(0);
	}
	upper = u.u_offset + u.u_count;
	if (upper > limit) {
		over = upper - limit;
		u.u_count -= over;
		uap = (struct a *)u.u_ap;
		uap->count -= over;
	}
	return(1);
}

E 1
