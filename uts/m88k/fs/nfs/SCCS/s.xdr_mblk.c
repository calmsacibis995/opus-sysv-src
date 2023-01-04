h03411
s 00463/00000/00000
d D 1.1 90/03/06 12:43:39 root 1 0
c date and time created 90/03/06 12:43:39 by root
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
/*
 *	Copyright 1986 Convergent Technologies, Inc.
 *	Copyright (C) 1984, Sun Microsystems, Inc.
 */
#ifndef lint
static char SysVr3NFSID[] = "@(#)xdr_mblk.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * This module has some code from Convergent's xdr_mblk.c module:
 *
 *	Convergent Technologies - System V - Dec 1986
 * #ident	"@(#)xdr_mblk.c	1.5 :/source/uts/common/fs/nfs/s.xdr_mblk.c 3/11/87 14:16:35"
 */

/*
 * XDR implementation for kernel-level stream messages.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/user.h"
#include "sys/stream.h"
#include "sys/errno.h"
#include "sys/debug.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"

#include "bsd/netinet/in.h"
#include "sys/endian.h"

bool_t	xdrmblk_getlong(), xdrmblk_putlong();
bool_t	xdrmblk_getbytes(), xdrmblk_putbytes();
uint	xdrmblk_getpos();
bool_t	xdrmblk_setpos();
long	*xdrmblk_inline();
long	*xdrmblk_peek();
void	xdrmblk_destroy();

/*
 * XDR on mblks operations vector.
 */
struct xdr_ops xdrmblk_ops = {
	xdrmblk_getlong,
	xdrmblk_putlong,
	xdrmblk_getbytes,
	xdrmblk_putbytes,
	xdrmblk_getpos,
	xdrmblk_setpos,
	xdrmblk_inline,
	xdrmblk_destroy
};

#define mpNULL		((mblk_t *)0)
#define MIN(x,y)	((x)<(y)? (x): (y))

/*
 * Initialize stream-based XDR.
 */
void
xdrmblk_init(xdrs, mp, op, size)
	register XDR *xdrs;
	mblk_t 	*mp;
	enum xdr_op op;
	int size;
{
	/*
	 * Public may only be used by the XDR client.
	 * Base points to the first mblk in the message.
	 * Size always contains the maximum message size.
	 * Private points to the current serialization mblk.
	 * Handy is the byte count remaining to encode/decode.
	 * The data is always between read and write ptrs.
	 * The write ptr is incremented for encoding.
	 * The read ptr is incremented for decoding.
	 */
	xdrs->x_op = op;
	xdrs->x_ops = &xdrmblk_ops;
	xdrs->x_public = (caddr_t)0;		/* for xdr client */
	xdrs->x_base = (caddr_t)mp;		/* message mblk */
	xdrs->x_size = size;			/* message limit */
	xdrs->x_private = (caddr_t)mp;		/* current mblk */
	xdrs->x_handy = size;			/* remaining */

	ASSERT(mp != mpNULL);
	ASSERT(mp->b_rptr <= mp->b_wptr);
	ASSERT(mp->b_datap != (dblk_t *)0);
	ASSERT(mp->b_datap->db_type == M_DATA);
	ASSERT(mp->b_datap->db_base <= mp->b_rptr);
	ASSERT(mp->b_wptr <= mp->b_datap->db_lim);
}

/*
 * This routine will dynamically allocate (encode) 
 * stream blocks when needed or deallocate (decode)
 * stream blocks when no longer needed.  These two
 * #defines bracket the range of sizes allocated.
 */
#define XDRMINCLASS	5	/*  512 bytes */
#define XDRMAXCLASS	7	/* 2048 bytes */

static mblk_t *
xdrmblk_next(xdrs)
	register XDR *xdrs;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;  /* current mblk */
	register mblk_t *nbp = mpNULL;			  /* next mblk */
	register int need, try;

	if ((bp == mpNULL) || (xdrs->x_handy <= 0)) {
		goto out;
	}
	switch ((int)xdrs->x_op) {

	case XDR_ENCODE:
		if (bp->b_rptr == bp->b_wptr) {
			/*
			 * Block empty, reuse it.
			 */
			nbp = bp;
			nbp->b_rptr = nbp->b_wptr = nbp->b_datap->db_base;
			break;
		} 
		if ((nbp = bp->b_cont) != mpNULL) {
			/*
			 * Already allocated, use it.
			 */
			nbp->b_rptr = nbp->b_wptr = nbp->b_datap->db_base;
			break;
		}

		/*
		 * Allocate a block of readily available size.
		 * NOTE: Major assumptions are made here about
		 *	 the stream buffer management system.
		 */
		ASSERT(XDRMINCLASS > 0);
		ASSERT(XDRMAXCLASS < NCLASS);
		for (need = 0; need < XDRMAXCLASS; need++) {
			if (xdrs->x_handy <= rbsize[need]) {
				break;
			}
		}
		try = need;
		while ((nbp = allocb(rbsize[try], BPRI_LO)) == mpNULL) {
			if (--try <= XDRMINCLASS) {
				if (strwaitbuf(rbsize[need], BPRI_LO)) {
					if ((u.u_error == EINTR) ||
					    (--need <= XDRMINCLASS)) { 
						goto out;
					}
					u.u_error = 0;
				}
			}
		}
		bp->b_cont = nbp;
		break;

	case XDR_DECODE:
		/*
		 * Free this mblk and get the next,
		 * until we find an mblk with data.
		 * Note: We always preserve the link
		 * from the first mblk in the message.
		 */
		do {
			nbp = bp->b_cont;
			if (bp != (mblk_t *)xdrs->x_base) {
				((mblk_t *)xdrs->x_base)->b_cont = nbp;
				freeb(bp);
			}
			if (nbp == mpNULL) {
				goto out;
			}	
			/* Advance to next mblk */
			bp = nbp;
		} while (nbp->b_wptr <= nbp->b_rptr);
		break;

	default:
		goto out;

	} /* switch */

	ASSERT(nbp != mpNULL);
	ASSERT(nbp->b_rptr <= nbp->b_wptr);
	ASSERT(nbp->b_datap != (dblk_t *)0);
	ASSERT(nbp->b_datap->db_type == M_DATA);
	ASSERT(nbp->b_datap->db_base <= nbp->b_rptr);
	ASSERT(nbp->b_wptr <= nbp->b_datap->db_lim);

	/*
	 * Store mblk ptr in private.
	 */
	xdrs->x_private = (caddr_t)nbp;
out:
	return(nbp);
}

/* ARGSUSED */
void
xdrmblk_destroy(xdrs)
	XDR *xdrs;
{
}

bool_t
xdrmblk_getlong(xdrs, lp)
	register XDR *xdrs;
	long *lp;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;

	ASSERT(xdrs->x_op == XDR_DECODE);

	if ((bp->b_wptr - bp->b_rptr) < sizeof(long)) {
		char tmp[sizeof(long)];
		char *tp;

		/* 
		 * When decoding, we can't assume
		 * the stream is dblk'd on longs.
		 */
		tp = tmp;
		while (bp->b_wptr - bp->b_rptr) {
			*tp++ = *bp->b_rptr++;
		}
		if ((bp = xdrmblk_next(xdrs)) == mpNULL) {
			return(FALSE);
		}
		while (tp < &tmp[sizeof(long)]) {
			*tp++ = *bp->b_rptr++;
		}
		*lp = ntohl(*((long *)tmp));
	} else {
		*lp = ntohl(*((long *)bp->b_rptr));
		bp->b_rptr += sizeof(long);
	}
	xdrs->x_handy -= sizeof(long);
	return(TRUE);
}

bool_t
xdrmblk_putlong(xdrs, lp)
	register XDR *xdrs;
	long *lp;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;

	ASSERT(xdrs->x_op == XDR_ENCODE);

	if ((bp->b_datap->db_lim - bp->b_wptr) < sizeof(long)) {
		/*
		 * Put long in next block.
		 */
		if ((bp = xdrmblk_next(xdrs)) == mpNULL) {
			return(FALSE);
		}
	}
	*(long *)bp->b_wptr = htonl(*lp);
	xdrs->x_handy -= sizeof(long);
	bp->b_wptr += sizeof(long);
	return(TRUE);
}

bool_t
xdrmblk_getbytes(xdrs, addr, len)
	register XDR *xdrs;
	caddr_t addr;
	uint len;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;
	register uint size;

	ASSERT(xdrs->x_op == XDR_DECODE);

	while ((size = (bp->b_wptr - bp->b_rptr)) < len) {
		if (size) {
			bcopy(bp->b_rptr, addr, size);
			xdrs->x_handy -= size;
			bp->b_rptr += size;
			addr += size;
			len -= size;
		}
		if ((bp = xdrmblk_next(xdrs)) == mpNULL) {
			return(FALSE);
		}
	}
	bcopy(bp->b_rptr, addr, len);
	xdrs->x_handy -= len;
	bp->b_rptr += len;
	return(TRUE);
}

bool_t
xdrmblk_putbytes(xdrs, addr, len)
	register XDR *xdrs;
	caddr_t addr;
	uint len;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;
	register uint size;

	ASSERT(xdrs->x_op == XDR_ENCODE);

	while ((size = (bp->b_datap->db_lim - bp->b_wptr)) < len) {
		if (size) {
			bcopy(addr, bp->b_wptr, size);
			xdrs->x_handy -= size;
			bp->b_wptr += size;
			addr += size;
			len -= size;
		}
		if ((bp = xdrmblk_next(xdrs)) == mpNULL) {
			return(FALSE);
		}
	}
	bcopy(addr, bp->b_wptr, len);
	xdrs->x_handy -= len;
	bp->b_wptr += len;
	return(TRUE);
}

uint
xdrmblk_getpos(xdrs)
	register XDR *xdrs;
{

	return(xdrs->x_size - xdrs->x_handy);
}

bool_t
xdrmblk_setpos(xdrs, pos)
	register XDR *xdrs;
	uint pos;
{
	mblk_t *mp = (mblk_t *)xdrs->x_base; 
	register mblk_t *bp;
	register int hdy = pos;
	int ret = FALSE;

	/*
	 * This is the routine that takes the hit for xdrmblk_next.
	 * By allocating and freeing on the fly we limit our options
	 * in setting the position.  Given the "good citizen" behavior
	 * of xdr_mblk toward streams and the LOW client-side static 
	 * buffer requirements, it seems well worth the loss in 
	 * generality. 
	 */
	switch ((int) xdrs->x_op) {
	
	case XDR_ENCODE:
		/*
		 * We can only set the encode position 
		 * into already allocated data blocks.
		 */
		for (bp = mp; (bp != mpNULL) && (hdy > 0); bp = bp->b_cont) {
			hdy -= (uint)(bp->b_wptr - bp->b_rptr);
			if (hdy <= 0 ) {
				xdrs->x_private = (caddr_t)bp;
				xdrs->x_handy = xdrs->x_size - pos;
				bp->b_wptr += hdy;  /* subtracting */
				ret = TRUE;
			}
		}
		break;

	case XDR_DECODE:
		/*
		 * We can only set the decode position to zero
		 * and then only prior to the deserialization.
		 * Could do better, but makes no sense to me.
		 */
		ret = ((hdy == 0) && (xdrs->x_size == xdrs->x_handy));
		break;

	} /* switch */ 

	return(ret);
}

long *
xdrmblk_inline(xdrs, len)
	register XDR *xdrs;
	register int len;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;
	long *buf;

	if (xdrs->x_handy < len) {
		return((long *)NULL);
	}
	switch ((int)xdrs->x_op) {
	
	case XDR_ENCODE: 
		if ((bp->b_datap->db_lim - bp->b_wptr) < len) {
			return((long *)NULL);
		}
		buf = (long *)bp->b_wptr;
		bp->b_wptr += len;
		break;

	case XDR_DECODE: 
		if ((bp->b_wptr - bp->b_rptr) < len) {
			return((long *)NULL);
		}
		buf = (long *)bp->b_rptr;
		bp->b_rptr += len;
		break;

	default:
		return((long *)NULL);

	} /* switch */

	xdrs->x_handy += len;
	return(buf);
}

long *
xdrmblk_peek(xdrs, len)
	register XDR *xdrs;
	register int len;
{
	register mblk_t *bp = (mblk_t *)xdrs->x_private;

	/*
	 * Return a "peek" pointer at
	 * the current decode position.
	 *
	 * NOTE:  Although dangerous, might as
	 * well be formalized since everyone's
	 * doing it.
	 */
	if ((bp->b_wptr - bp->b_rptr) >= len) {
		return((long *)bp->b_rptr);
	}
	return((long *)NULL);
}
E 1
