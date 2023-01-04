/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/bio.c	10.5"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/fs/s5param.h"
#include "sys/fs/s5macros.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/var.h"
#include "sys/conf.h"
#include "sys/cmn_err.h"

extern dkstrategy();
/* Convert logical block number to a physical number */
/* given block number and block size of the file system */
/* Assumes 512 byte blocks (see param.h). */
#define LTOPBLK(blkno, bsize)	(blkno * ((bsize>>SCTRSHFT)))
/*
 * Unlink a buffer from the available list and mark it busy.
 * (internal interface)
 */
#define notavail(bp) \
{\
	register s;\
\
	s = spl6();\
	bp->av_back->av_forw = bp->av_forw;\
	bp->av_forw->av_back = bp->av_back;\
	bp->b_flags |= B_BUSY;\
	bfreelist.b_bcount--;\
	splx(s);\
}

/* count and flag for outstanding async writes */

int basyncnt, basynwait;

/*
 * The following several routines allocate and free
 * buffers with various side effects.  In general the
 * arguments to an allocate routine are a device and
 * a block number, and the value is a pointer to
 * to the buffer header; the buffer is marked "busy"
 * so that no one else can touch it.  If the block was
 * already in core, no I/O need be done; if it is
 * already busy, the process waits until it becomes free.
 * The following routines allocate a buffer:
 *	getblk
 *	bread
 *	breada
 * Eventually the buffer must be released, possibly with the
 * side effect of writing it out, by using one of
 *	bwrite
 *	bdwrite
 *	bawrite
 *	brelse
 */

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */
struct buf *
bread(dev, blkno, bsize)
register dev_t dev;
daddr_t blkno;
long bsize;
{
	register struct buf *bp;

	sysinfo.lread++;
	bp = getblk(dev, blkno, bsize);
	if (bp->b_flags&B_DONE)
		return(bp);
	bp->b_flags |= B_READ;
	bp->b_bcount = bsize;
	(*bdevsw[bmajor(dev)].d_strategy)(bp);
	u.u_ior++;
	sysinfo.bread++;
	iowait(bp);
	return(bp);
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
struct buf *
breada(dev, blkno, rablkno, bsize)
register dev_t dev;
daddr_t blkno, rablkno;
long bsize;
{
	register int i, racount;
	register struct buf *bp;
	register struct buf *sbp, *nbp;

	sysinfo.lread++;
	sbp = getblk(dev, blkno, bsize);
	if (sbp->b_flags & B_DONE) {
		return(sbp);	/* requested block in core; no read required */
	}

	if (rablkno & 0x80000000) {
		/* read next n contiguous blocks */
		racount = rablkno & ~0x80000000;
		rablkno = blkno + 1;
	} else {
		/* readahead specified block */
		racount = 1;
	}
	sbp->av_forw = NULL;
	nbp = sbp;

	/*  collect all the readahead blocks;
	**  if any are DONE, put them back, in the process guaranteeing
	**  they're not LRU on the free list
	*/
	for (i=0; i<racount; ++i) {
		bp = getblk(dev, rablkno+i, bsize);
		if (bp->b_flags & B_DONE) {
			brelse(bp);	/* move to end of free list */
		} else {
			nbp->av_forw = bp;
			nbp = bp;
			nbp->av_forw = NULL;
		}
	}

	/*  Start up physical reads; all but the first are ASYNC;
	**  all but the last are WAITed (for dkstart)
	*/
	for (bp = sbp; bp; bp = nbp) {
		bp->b_flags |= B_READ | B_ASYNC;
		if (bp == sbp)
			bp->b_flags &= ~B_ASYNC;
		if (nbp = bp->av_forw)
			bp->b_flags |= B_WAIT;
		bp->b_bcount = bsize;
		(*bdevsw[bmajor(dev)].d_strategy)(bp);
		ATOMIC (bp->b_flags &= ~B_WAIT);
		u.u_ior++;
		sysinfo.bread++;
	}

	/*  Wait for actual requested block to finish
	*/
	iowait(sbp);
	return(sbp);
}

#if 0
/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
struct buf *
breada(dev, blkno, rablkno, bsize)
register dev_t dev;
daddr_t blkno, rablkno;
long bsize;
{
	register struct buf *bp, *rabp, *nbp, *sbp;
	register int	racount, i;

	bp = NULL;
	nbp = NULL;
	sbp = NULL;
	if (!incore(dev, blkno, bsize)) {
		sysinfo.lread++;
		bp = getblk(dev, blkno, bsize);
		if ((bp->b_flags&B_DONE) == 0) {
			bp->b_flags |= B_READ;
			bp->b_bcount = bsize;
			sbp = nbp = bp;
			sbp->av_forw = NULL;
			u.u_ior++;
			sysinfo.bread++;
		}
	}
	else if (rablkno & 0x80000000)
		return(bread(dev, blkno, bsize));

	racount = 0;
	if (rablkno & 0x80000000) {
		rablkno = blkno + (rablkno & ~0x80000000);
		while (rablkno > blkno && !incore(dev, rablkno, bsize)) {
			rablkno--;
			racount++;
		}
		rablkno++;
	}
	else {
		if (rablkno && !incore(dev, rablkno, bsize))
			racount = 1;
	}
	
	for (; racount != 0; racount--, rablkno++) {
             if (bfreelist.b_bcount>1) {
		rabp = getblk(dev, rablkno, bsize);
		if (rabp->b_flags & B_DONE)
			brelse(rabp);
		else {
			rabp->b_flags |= B_READ|B_ASYNC;
			rabp->b_bcount = bsize;
			if (sbp) {
				nbp->av_forw = rabp;
				nbp = rabp;
				rabp->av_forw = NULL;
			}
			else {
				sbp = nbp = rabp;
				sbp->av_forw = NULL;
			}
			u.u_ior++;
			sysinfo.bread++;
		}
	     }
	}
	while (sbp) {
		nbp = sbp->av_forw;
		if (nbp)
			sbp->b_flags |= B_WAIT;
		(*bdevsw[bmajor(dev)].d_strategy)(sbp);
		ATOMIC (sbp->b_flags &= ~B_WAIT);
		sbp = nbp;
	}
	if (bp == NULL)
		return(bread(dev, blkno, bsize));
	iowait(bp);
	return(bp);
}
#endif

/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
bwrite(bp,rtn)
register struct buf *bp;
register int rtn;
{
	register flag;
	register s;

	sysinfo.lwrite++;
	flag = bp->b_flags;
	s = spl6();
	bp->b_flags &= ~(B_READ | B_DONE | B_ERROR | B_DELWRI);
	if (flag & B_ASYNC)
		basyncnt++;
	splx(s);
	(*bdevsw[bmajor(bp->b_dev)].d_strategy)(bp);
	ATOMIC (bp->b_flags &= ~B_LOPRI);
	u.u_iow++;
	sysinfo.bwrite++;
	if ((flag&B_ASYNC) == 0) {
		iowait(bp);
		brelse(bp);
	} else {
		if (!(flag & B_DELWRI))
			geterror(bp);
	}
}

/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * Also save the time that the block is first marked as delayed
 * so that it will be written in a reasonable time.
 */
bdwrite(bp)
register struct buf *bp;
{
	sysinfo.lwrite++;
	if (!(bp->b_flags & B_DELWRI))
		bp->b_start = lbolt;
	bp->b_flags |= B_DELWRI | B_DONE;
	bp->b_resid = 0;
	brelse(bp);
}

/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
bawrite(bp)
register struct buf *bp;
{

	if (bfreelist.b_bcount>4)
		bp->b_flags |= B_ASYNC;
	bwrite(bp);
}

/*
 * release the buffer, with no I/O implied.
 */
brelse(bp)
register struct buf *bp;
{
	register struct buf **backp;
	register s;

	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	else	/* favor process waiting for a specific buffer */
	if (bfreelist.b_flags&B_WANTED) {
		ATOMIC (bfreelist.b_flags &= ~B_WANTED);
		wakeup((caddr_t)&bfreelist);
	}
	if (bp->b_flags&B_ERROR) {
		bp->b_flags |= B_STALE|B_AGE;
		bp->b_flags &= ~(B_ERROR|B_DELWRI);
		bp->b_error = 0;
	}
	s = spl6();
	if (bp->b_flags & B_AGE) {
		backp = &bfreelist.av_forw;
		(*backp)->av_back = bp;
		bp->av_forw = *backp;
		*backp = bp;
		bp->av_back = &bfreelist;
	} else {
		backp = &bfreelist.av_back;
		(*backp)->av_forw = bp;
		bp->av_back = *backp;
		*backp = bp;
		bp->av_forw = &bfreelist;
	}
	bp->b_flags &= ~(B_WANTED|B_BUSY|B_ASYNC|B_AGE);
	bfreelist.b_bcount++;
	splx(s);
}

/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */
incore(dev, blkno, bsize)
register dev_t dev;
register daddr_t blkno;
register long bsize;
{
	register struct buf *bp;
	register struct buf *dp;

	blkno = LTOPBLK(blkno, bsize);
	dp = bhash(dev, blkno);
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw)
		if (bp->b_blkno==blkno && bp->b_dev==dev && (bp->b_flags&B_STALE)==0)
			return((int)bp);
	return(0);
}

/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 */
struct buf *
getblk(dev, blkno, bsize)
register dev_t dev;
register daddr_t blkno;
long bsize;
{
	register struct buf *bp;
	register struct buf *dp;
	register s;

	if (bmajor(dev) >= bdevcnt)
		cmn_err(CE_PANIC,"blkdev");

	blkno = LTOPBLK(blkno, bsize);
	s = spl6();
    loop:
	splx(s);
	if ((dp = bhash(dev, blkno)) == NULL)
		cmn_err(CE_PANIC,"devtab");
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw) {
		if (bp->b_blkno!=blkno || bp->b_dev!=dev || bp->b_flags&B_STALE)
			continue;
		s = spl6();
		if (bp->b_flags&B_BUSY) {
			bp->b_flags |= B_WANTED;
			syswait.iowait++;
			if (bp->b_flags & B_LOPRIQ) {
				dkpromote(bp);		/* XXX */
			}
			sleep((caddr_t)bp, PRIBIO+1);
			syswait.iowait--;
			goto loop;
		}
		notavail(bp);
		splx(s);
		return(bp);
	}
	s = spl6();
	if (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep((caddr_t)&bfreelist, PRIBIO+1);
		goto loop;
	}
	bp = bfreelist.av_forw;
	notavail(bp);
	splx(s);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC | B_LOPRI;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = dev;
	bp->b_blkno = blkno;
	bp->b_bcount = bsize;
	bp->b_resid = 0;
	return(bp);
}

/*
 * get an empty block,
 * not assigned to any particular device
 */
struct buf *
geteblk()
{
	register struct buf *bp;
	register struct buf *dp;
	register s;

loop:
	s = spl6();
	while (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep((caddr_t)&bfreelist, PRIBIO+1);
	}
	dp = &bfreelist;
	bp = bfreelist.av_forw;
	notavail(bp);
	splx(s);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC | B_LOPRI;
		bwrite(bp);
		goto loop;
	}
	bp->b_flags = B_BUSY|B_AGE;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = (dev_t)NODEV;
	bp->b_bcount = SBUFSIZE;
	bp->b_resid = 0;
	return(bp);
}

/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
iowait(bp)
register struct buf *bp;
{
	register s;

	syswait.iowait++;
	s = spl6();
	while ((bp->b_flags&B_DONE)==0)
		sleep((caddr_t)bp, PRIBIO);
	splx(s);
	syswait.iowait--;
	geterror(bp);
}

/*
 * Mark I/O complete on a buffer, release it if I/O is asynchronous,
 * and wake up anyone waiting for it.
 */
iodone(bp)
register struct buf *bp;
{
	register s;

	bp->b_flags |= B_DONE;
	bp->b_flags &= ~B_LOPRI;
	if (bp->b_flags&B_ASYNC) {
		s = spl6();
		if (!(bp->b_flags & B_READ))
			basyncnt--;
		if (basyncnt==0 && basynwait) {
			basynwait = 0;
			wakeup((caddr_t)&basyncnt);
		}
		splx(s);
		brelse(bp);
	} else {
		bp->b_flags &= ~B_WANTED;
		wakeup((caddr_t)bp);
	}
}

/*
 * Zero the core associated with a buffer.
 */
clrbuf(bp)
struct buf *bp;
{
	bzero(bp->b_un.b_words, bp->b_bcount);
	bp->b_resid = 0;
}

/*
 * make sure all write-behind blocks
 * on dev (or NODEV for all)
 * are flushed out.
 * (from closef and update)
 */
bflush(dev)
register dev_t dev;
{
	register struct buf *bp;
	register s;

loop:
	s = spl6();
	for (bp = bfreelist.av_forw; bp != &bfreelist; bp = bp->av_forw) {
		if (bp->b_flags&B_DELWRI && 
		    (dev == NODEV||dev==bp->b_dev)) {
			bp->b_flags |= B_ASYNC;
			notavail(bp);
			bwrite(bp);
    			splx(s);
    			goto loop;
		}
	}
	if (dev == NODEV || bdevsw[bmajor(dev)].d_strategy == dkstrategy) {
		dkpromoteall();			/* XXX */
	}
	splx(s);
}

/*
 * flush delayed writes after time delay
 */
bdflush()
{
	register struct buf *bp;
	register autoup;
	register s;

	autoup = v.v_autoup*HZ;

	for(;;) {
		sysupdate();

		loop:
		s = spl6();
		for (bp = bfreelist.av_forw; bp != &bfreelist; bp=bp->av_forw){
			if (bp->b_flags&B_DELWRI &&
			    ((lbolt - bp->b_start) >= autoup)) {
				bp->b_flags |= B_ASYNC | B_LOPRI;
				notavail(bp);
				bwrite(bp);
				splx(s);
				goto loop;
			}
		}
		splx(s);

		sleep(bdflush, PRIBIO);
	}
}

/*
 * wait for asynchronous writes to finish
 */
bdwait()
{
	register s;

	s = spl6();
	while (basyncnt) {
		basynwait = 1;
		sleep((caddr_t)&basyncnt, PRIBIO);
	}
	splx(s);
}

/*
 * Invalidate blocks for a dev after last close.
 */
binval(dev)
register dev_t dev;
{
	register struct buf *dp;
	register struct buf *bp;
	register i;

	for (i=0; i<v.v_hbuf; i++) {
		dp = (struct buf *)&hbuf[i];
		for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
			if (bp->b_dev == dev)
				bp->b_flags |= B_STALE|B_AGE;
	}
}

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device hash buffer lists to empty.
 */

binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register unsigned i;
	register caddr_t pbuffer;

	dp = &bfreelist;
	dp->b_forw = dp->b_back =
	    dp->av_forw = dp->av_back = dp;
	pbuffer = bufferz;

	for (i=0, bp=bufz; i<v.v_buf; i++,bp++,pbuffer += SBUFSIZE) {
		bp->b_dev = NODEV;
		bp->b_un.b_addr = pbuffer;
		bp->b_back = dp;
		bp->b_forw = dp->b_forw;
		dp->b_forw->b_back = bp;
		dp->b_forw = bp;
		bp->b_flags = B_BUSY;
		bp->b_bcount = 0;
		brelse(bp);
	}

	pfreecnt = v.v_pbuf;
	pfreelist.av_forw = bp = pbufz;
	for (i=0; i < v.v_pbuf-1; bp++, i++)
		bp->av_forw = bp+1;
	bp->av_forw = NULL;
	for (i=0; i < v.v_hbuf; i++)
		hbuf[i].b_forw = hbuf[i].b_back = (struct buf *)&hbuf[i];
}

