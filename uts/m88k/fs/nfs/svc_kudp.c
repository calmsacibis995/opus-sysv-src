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
 *	Copyright 1986 Convergent Technologies Inc.
 *	Copyright (C) 1984, Sun Microsystems, Inc.
 */
#ifndef lint
static char SysVr3NFSID[] = "@(#)svc_kudp.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Some of this module was taken from Convergent Technologies svc_kudp.c:
 *
 *	Convergent Technologies - System V - Nov 1986	*
 * #ident  "@(#)nfs_sys3.c 1.7 :/source/uts/common/fs/nfs/s.nfs_sys3.c 3/11/87 14:15:29"
 * 
 * #ifdef LAI
 * static char SysVNFSID[] = "@(#)nfs_sys3.c       2.9 LAI System V NFS Release 3.0/V3 source";
 * #endif
 */

/*      @(#)svc_kudp.c 1.21 86/01/14 SMI      */

/*
 * Server side for UDP/IP based RPC in the kernel.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/errno.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/stream.h"
#include "sys/cmn_err.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/rpc_msg.h"
#include "sys/fs/nfs/trans.h"
#include "sys/fs/nfs/svc.h"
#include "sys/fs/nfs/dup.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

/*
 * Routines exported through ops vector.
 */
bool_t		svckudp_recv();
bool_t		svckudp_send();
enum xprt_stat	svckudp_stat();
bool_t		svckudp_getargs();
bool_t		svckudp_freeargs();
void		svckudp_destroy();

/*
 * Server transport operations vector.
 */
struct xp_ops svckudp_op = {
	svckudp_recv,		/* Get requests */
	svckudp_stat,		/* Return status */
	svckudp_getargs,	/* Deserialize arguments */
	svckudp_send,		/* Send reply */
	svckudp_freeargs,	/* Free argument data space */
	svckudp_destroy		/* Destroy transport handle */
};

/*
 * Transport private data.
 * Kept in xprt->xp_p2.
 */
struct udp_data {
	int	 ud_flags;			/* flag bits, see below */
	TRANS	*ud_trans;			/* udp/ip transport */
	ulong	 ud_xid;			/* transaction id */
#if STREAM_XPRT
	mblk_t	*ud_inmblk;			/* input mblk chain */
#else
	caddr_t	 ud_inmbuf;			/* input area */
#endif
	XDR	 ud_xdrin;			/* input xdr stream */
	XDR	 ud_xdrout;			/* output xdr stream */
	char	 ud_verfbody[MAX_AUTH_BYTES];	/* verifier */
};

typedef struct udp_data		UDPDATA;

/*
 * Flags
 */
#define UD_BUSY		0x001		/* buffer is busy */
#define UD_WANTED	0x002		/* buffer wanted */

/*
 * useful NULLs
 */
#define	mpNULL		((mblk_t *)0)
#define udNULL		((UDPDATA *)0)
#define svcNULL		((SVCXPRT *)0)

/*
 * Server statistics
 */
struct {
	int rscalls;
	int rsbadcalls;
	int rsnullrecv;
	int rsbadlen;
	int rsxdrcall;
} rsstat;

/*
 * Create a transport record.
 * The transport record, output buffer, and private data structure
 * are allocated.  The output buffer is serialized into using xdrmem.
 * There is one transport record per user process which implements a
 * set of services.
 */
SVCXPRT *
svckudp_create(fp, port)
	struct file *fp;
	ushort port;
{
	register SVCXPRT *xprt;
	register UDPDATA *ud;
	register TRANS *tp;

	/*
	 * Create udp/ip transport handle.
	 */
	if ((tp = kudp_create(fp, TP_OPENFP)) == tpNULL) {
		/* u.u_error set */
		xprt = svcNULL;
		goto bad;
	}

	/*
	 * Allocate and initialize server private data.
	 */
	ud = (UDPDATA *)kmem_alloc((uint)sizeof(UDPDATA));
	bzero((caddr_t)ud, sizeof(*ud));
	ud->ud_trans = tp;		/* used */

	/*
	 * Allocate and initialize server public server data.
	 */
	xprt = (SVCXPRT *)kmem_alloc((uint)sizeof(SVCXPRT));
	xprt->xp_raddr = kmem_alloc((uint)sizeof(TADDR));
	xprt->xp_addrlen = 0;
#if STREAM_XPRT
	xprt->xp_p1 = NULL;
#else
	xprt->xp_p1 = kmem_alloc((uint)UDPMSGSIZE);
#endif
	xprt->xp_p2 = (caddr_t)ud;
	xprt->xp_verf.oa_base = ud->ud_verfbody;
	xprt->xp_ops = &svckudp_op;
	xprt->xp_port = port;	   	/* unused */
	xprt->xp_sock = NULL;		/* unused */		

	/*
	 * Register rpc server transport.
	 */
	xprt_register(xprt);
bad:
	return(xprt);
}

/*
 * Destroy a transport record.
 * Frees the space allocated for a transport record.
 */
void
svckudp_destroy(xprt)
	register SVCXPRT *xprt;
{
	register UDPDATA *ud = (UDPDATA *)xprt->xp_p2;

#if STREAM_XPRT
	if (ud->ud_inmblk != mpNULL) {
		freemsg(ud->ud_inmblk);
	}
	if (ud->ud_trans != tpNULL) {
		kudp_destroy(ud->ud_trans);
	}
#else
	if (ud->ud_inmbuf) 
		kmem_free(ud->ud_inmbuf, UDPMSGSIZE);
	kmem_free(xprt->xp_p1, UDPMSGSIZE);
	/* ud->ud_trans gets closed when nfsd exits */
#endif
	kmem_free((caddr_t)ud, (uint)sizeof(UDPDATA));
	kmem_free(xprt->xp_raddr, (uint)sizeof(TADDR));
	kmem_free((caddr_t)xprt, (uint)sizeof(SVCXPRT));
}

/*
 * Receive rpc requests.
 * Pulls a request in off the socket, checks if the packet is intact,
 * and deserializes the call packet.
 */
bool_t
svckudp_recv(xprt, msg)
	register SVCXPRT *xprt;
	struct rpc_msg *msg;
{
	register UDPDATA *ud = (UDPDATA *)xprt->xp_p2;
	register TRANS *tp = ud->ud_trans;
	register XDR *xdrs;
	register int size;
	/* externals */
	extern int nfs_portmon;

	rsstat.rscalls++;

#if !STREAM_XPRT
	ud->ud_inmbuf = kmem_alloc(UDPMSGSIZE);
	if (ud->ud_inmbuf == 0)  {
		u.u_error = EAGAIN;
		return (FALSE);
	}
	xprt->xp_addrlen = sizeof(TADDR);
	{
	int s;
	s = spl7();
	size = ku_recvfrom(tp, xprt->xp_raddr, ud->ud_inmbuf, UDPMSGSIZE); 
	splx(s);
	}

	if (size < 4 * sizeof(ulong)) {
		rsstat.rsbadlen++;
		goto bad;
	}

	xdrs = &(ud->ud_xdrin);
	xdrmem_create(xdrs, ud->ud_inmbuf, size, XDR_DECODE);
	if (!xdr_callmsg(xdrs, msg)) {
		rsstat.rsxdrcall++;
		goto bad;
	}
	ud->ud_xid = msg->rm_xid;
	return(TRUE);

bad:
	kmem_free(ud->ud_inmbuf, UDPMSGSIZE);
	ud->ud_inmbuf = 0;
	rsstat.rsbadcalls++;
	return(FALSE);
#else
	/*
	 * Wait until error or a message is received.
	 */
	xprt->xp_addrlen = sizeof(TADDR);
	size = kudp_recvfrom(tp, &ud->ud_inmblk, xprt->xp_raddr, 
			     &xprt->xp_addrlen, TP_INFINITY, 0);
	if (size < 0) {
		rsstat.rsnullrecv++;
		u.u_error = kudp_geterr(tp);
		return(FALSE);
	}

	/*
	 * If monitoring, then check security.
	 * If not secure, then reply weakauth.
	 */
	if ((nfs_portmon == 1) && 
	    (!kudp_secure(tp, xprt->xp_raddr, xprt->xp_addrlen))) {
		freemsg(ud->ud_inmblk);
		ud->ud_inmblk = mpNULL;
		rsstat.rsbadcalls++;
		svcerr_weakauth(xprt);
		return(FALSE);
	}

	/* 
	 * Check for short message.
	 */
	if (size < (4 * BYTES_PER_XDR_UNIT)) {
		rsstat.rsbadlen++;
		goto bad;
	}

	/*
	 * Initialize xdr mblk decode.
	 */
	xdrs = &(ud->ud_xdrin);
	xdrmblk_init(xdrs, ud->ud_inmblk, XDR_DECODE, size);

	/*
	 * Deserialize the call msg.
	 */
	if (!xdr_callmsg(xdrs, msg)) {
		rsstat.rsxdrcall++;
		goto bad;
	}

	/*
	 * Save the xid for reply msg.
	 */
	ud->ud_xid = msg->rm_xid;
	return(TRUE);

bad:
	freemsg(ud->ud_inmblk);
	ud->ud_inmblk = mpNULL;
	rsstat.rsbadcalls++;
	return(FALSE);
#endif
}

/*
 * Send rpc reply.
 * Serialize the reply packet and call kudp_sendto to sent it.
 */
/* ARGSUSED */
bool_t
svckudp_send(xprt, msg)
	register SVCXPRT *xprt;
	struct rpc_msg *msg;
{
	register UDPDATA *ud = (UDPDATA *)xprt->xp_p2;
	register mblk_t *mp = mpNULL;
	register XDR *xdrs;
	int stat = FALSE;
	int s;

	s = splimp();
	while (ud->ud_flags & UD_BUSY) {
		ud->ud_flags |= UD_WANTED;
		sleep((caddr_t)ud, PZERO-2);
	}
	ud->ud_flags |= UD_BUSY;
	(void) splx(s);

#if STREAM_XPRT
	/* 
	 * Allocate enough space to encode the message header.
	 * The xdr mblk routines will allocate more if needed.
	 */
	while ((mp = allocb(sizeof(struct rpc_msg), BPRI_LO)) == mpNULL) {
		if (strwaitbuf(sizeof(struct rpc_msg), BPRI_LO)) {
			cmn_err(CE_WARN, "svckudp_send: allocb ENOSR");
			goto done;
		}
	}

	/*
	 * Initialize xdr mblk encode for max udp message.
	 */
	xdrs = &(ud->ud_xdrout);
	xdrmblk_init(xdrs, mp, XDR_ENCODE, UDPMSGSIZE);
#else
	/*
	 * Initialize xdr and encode for max udp message.
	 */
	xdrs = &(ud->ud_xdrout);
	xdrmem_create(xdrs, xprt->xp_p1, UDPMSGSIZE, XDR_ENCODE);
#endif

	/* 
	 * Set the reply xid to the call xid.
	 */
	msg->rm_xid = ud->ud_xid;

	/*
	 * Serialize the reply message.
	 */
	if (!xdr_replymsg(xdrs, msg)) {
		cmn_err(CE_WARN, "svckudp_send: xdr_replymsg failed");
#if STREAM_XPRT
		freemsg(mp);
#endif
		goto done;
	}

	/* 
	 * Send reply msg off to client.
	 */
#if STREAM_XPRT
	if (kudp_sendto(ud->ud_trans, mp, xprt->xp_raddr,
			xprt->xp_addrlen, 0) < 0) {
		cmn_err(CE_WARN, "svckudp_send: kudp_sendto error %d",
			kudp_geterr(ud->ud_trans));
	} else {
		stat = TRUE;
	}
#else
	{
	int slen, err;
	slen = (int) XDR_GETPOS(xdrs);
	if (err = ku_sendto(ud->ud_trans, xprt->xp_raddr, xprt->xp_p1, slen))
		cmn_err(CE_WARN, "svckudp_send: kudp_sendto error = %d", err);
	else
		stat = TRUE;
	}
#endif

done:
	/*
	 * This is completely disgusting.  If public is set it is
	 * a pointer to a structure whose first field is the address
	 * of the function to free that structure and any related
	 * stuff.  (see rrokfree in nfs_xdr.c).
	 */
	if (xdrs->x_public) {
		(**((int (**)())xdrs->x_public))(xdrs->x_public);
	}

	/*
	 * Wakeup any others waiting on this handle.
	 */
	s = splimp();
	ud->ud_flags &= ~UD_BUSY;
	if (ud->ud_flags & UD_WANTED) {
		ud->ud_flags &= ~UD_WANTED;
		wakeup((caddr_t)ud);
	}
	(void) splx(s);

	return(stat);
}

/*
 * Return transport status.
 */
/* ARGSUSED */
enum xprt_stat
svckudp_stat(xprt)
	SVCXPRT *xprt;
{
	
	return(XPRT_IDLE);
}

/*
 * Deserialize arguments.
 */
bool_t
svckudp_getargs(xprt, xdr_args, args_ptr)
	SVCXPRT *xprt;
	xdrproc_t xdr_args;
	caddr_t args_ptr;
{

	return((*xdr_args)(&(((UDPDATA *)(xprt->xp_p2))->ud_xdrin), args_ptr));
}

bool_t
svckudp_freeargs(xprt, xdr_args, args_ptr)
	SVCXPRT *xprt;
	xdrproc_t xdr_args;
	caddr_t args_ptr;
{
	register UDPDATA *ud = (UDPDATA *)xprt->xp_p2;
	register XDR *xdrs = &(ud->ud_xdrin);

#if STREAM_XPRT
	if (ud->ud_inmblk != mpNULL) {
		freemsg(ud->ud_inmblk);
		ud->ud_inmblk = mpNULL;
	}
#else
	if (ud->ud_inmbuf) {
		kmem_free(ud->ud_inmbuf, UDPMSGSIZE);
		ud->ud_inmbuf = NULL;
	}
#endif
	if (args_ptr) {
		xdrs->x_op = XDR_FREE;
		return((*xdr_args)(xdrs, args_ptr));
	}
	return(TRUE);
}

/*
 * the dup cacheing routines below provide a cache of non-failure
 * transaction id's.  rpc service routines can use this to detect
 * retransmissions and re-send a non-failure response.
 */

#define XIDHASH(xid)    ((xid) & (drhashsz-1))
#define DRHASH(dr)	XIDHASH((dr)->dr_xid)
#define REQTOXID(req)	(((UDPDATA *)((req)->rq_xprt->xp_p2))->ud_xid)

int dupreqs;
int dupchecks;

/*
 * MAXDUPREQS is the number of cached items.  It should be adjusted
 * to the service load so that there is likely to be a response entry
 * when the first retransmission comes in.
 */
#define MAXDUPREQS	400

#define DRHASHSZ	32

struct dupreq dupcache[MAXDUPREQS];
int maxdupreqs = MAXDUPREQS;
struct duphash drhashtbl[DRHASHSZ];
int drhashsz = DRHASHSZ;

/*
 * drmru points to the head of a circular linked list in lru order.
 * drmru->dr_next == drlru
 */
struct dupreq *drmru;

/*
 * Initialize the locks and semaphores used in the duplicate request cache.
 */
svckudp_dupinit()
{
	register struct duphash *dhp;
	register struct dupreq *dr;

	for (dr = dupcache; dr < &dupcache[maxdupreqs]; dr++) {
		dr->dr_flags = 0;
	}

	for (dhp = drhashtbl; dhp < &drhashtbl[drhashsz]; dhp++) {
		dhp->dh_chain = NULL;
	}

	drmru = &dupcache[0];
}

svckudp_dup_enter(req)
	register struct svc_req *req;
{
	register struct duphash *dhp;
	register struct dupreq *dr;
	register struct dupreq *dr2;
	register SVCXPRT *xp;
	register ulong xid;

	xp = req->rq_xprt;
	xid = REQTOXID(req);
	dhp = &drhashtbl[XIDHASH(xid)];
	/*
	 * Check through the hash queues for an entry already there.
	 * If it is found, wait until the busy bit is cleared.
	 */
	dr = dhp->dh_chain;
	while (dr != NULL) {
		if ((dr->dr_xid == xid) &&
		    (dr->dr_prog == req->rq_prog) &&
		    (dr->dr_vers == req->rq_vers) &&
		    (dr->dr_proc == req->rq_proc) &&
		    (dr->dr_alen == xp->xp_addrlen) &&
		    (bcmp((caddr_t)&dr->dr_addr, 
			  (caddr_t)xp->xp_raddr, 
			  sizeof(dr->dr_addr)) == 0)) {
			while (dr->dr_flags & DR_BUSY) {
				dr->dr_flags |= DR_WANTED;
				sleep((caddr_t)dr, PZERO);
			}
			dr->dr_flags |= DR_BUSY;
			return;
		}
		dr = dr->dr_chain;
	}
	/*
	 * The required entry wasn't already in the cache.  Get
	 * a free slot to (re)use.  Slots are reused in a LRU
	 * manner.  A free slot is one in which the semaphore
	 * is not already held.
	 */
	do {
		dr = drmru;
		if (drmru == &dupcache[maxdupreqs - 1])
			drmru = &dupcache[0];
		else
			drmru++;
	} while (dr->dr_flags & DR_BUSY);
	dr->dr_flags |= DR_BUSY;
	unhash(dr);
	/*
	 * Check one more time to make sure that the entry did not
	 * get put into the cache while we were constructing another
	 * entry.  If so, just drop this entry, it will get reused
	 * eventually.
	 */
	dr2 = dhp->dh_chain;
	while (dr2 != NULL) {
		if ((dr2->dr_xid == xid) &&
		    (dr2->dr_prog == req->rq_prog) &&
		    (dr2->dr_vers == req->rq_vers) &&
		    (dr2->dr_proc == req->rq_proc) &&
		    (dr2->dr_alen == xp->xp_addrlen) &&
		    (bcmp((caddr_t)&dr2->dr_addr, 
			  (caddr_t)xp->xp_raddr, 
			  sizeof(dr2->dr_addr)) == 0)) {
			dr->dr_flags &= ~DR_BUSY;
			while (dr2->dr_flags & DR_BUSY) {
				dr2->dr_flags |= DR_WANTED;
				sleep((caddr_t)dr2, PZERO);
			}
			dr2->dr_flags |= DR_BUSY;
			return;
		}
		dr2 = dr2->dr_chain;
	}

	/*
	 * The entry still isn't in the cache, so use this slot and
	 * add it to the cache.
	 */
	dr->dr_xid = xid;
	dr->dr_prog = req->rq_prog;
	dr->dr_vers = req->rq_vers;
	dr->dr_proc = req->rq_proc;
	bcopy((caddr_t)xp->xp_raddr, (caddr_t)&dr->dr_addr, xp->xp_addrlen);
	dr->dr_alen = xp->xp_addrlen;
	dr->dr_flags = DR_BUSY;
	dr->dr_valid = 0;

	dr->dr_chain = dhp->dh_chain;
	dhp->dh_chain = dr;
}

svckudp_dup_fail(req)
	register struct svc_req *req;
{
	register struct duphash *dhp;
	register struct dupreq *dr;
	register struct dupreq *drp;
	register SVCXPRT *xp;
	register ulong xid;

	xp = req->rq_xprt;
	xid = REQTOXID(req);
	dhp = &drhashtbl[XIDHASH(xid)];
	dr = dhp->dh_chain;
	drp = NULL;
	while (dr != NULL) {
		if ((dr->dr_xid == xid) &&
		    (dr->dr_prog == req->rq_prog) &&
		    (dr->dr_vers == req->rq_vers) &&
		    (dr->dr_proc == req->rq_proc) &&
		    (dr->dr_alen == xp->xp_addrlen) &&
		    (bcmp((caddr_t)&dr->dr_addr, 
			  (caddr_t)xp->xp_raddr, 
			  sizeof(dr->dr_addr)) == 0)) {
			if (drp == NULL) {
				dhp->dh_chain = dr->dr_chain;
			} else {
				drp->dr_chain = dr->dr_chain;
			}
			dr->dr_flags &= ~DR_BUSY;
			if (dr->dr_flags & DR_WANTED) {
				dr->dr_flags &= ~DR_WANTED;
				wakeup((caddr_t)dr);
			}
			return;
		}
		drp = dr;
		dr = dr->dr_chain;
	}
	cmn_err(CE_PANIC, "svckudp_dup_fail: not in cache");
}

svckudp_dup_success(req)
	register struct svc_req *req;
{
	register struct duphash *dhp;
	register struct dupreq *dr;
	register SVCXPRT *xp;
	register ulong xid;

	xp = req->rq_xprt;
	xid = REQTOXID(req);
	dhp = &drhashtbl[XIDHASH(xid)];
	dr = dhp->dh_chain;
	while (dr != NULL) {
		if ((dr->dr_xid == xid) &&
		    (dr->dr_prog == req->rq_prog) &&
		    (dr->dr_vers == req->rq_vers) &&
		    (dr->dr_proc == req->rq_proc) &&
		    (dr->dr_alen == xp->xp_addrlen) &&
		    (bcmp((caddr_t)&dr->dr_addr, 
			  (caddr_t)xp->xp_raddr, 
			  sizeof(dr->dr_addr)) == 0)) {
			dr->dr_valid = 1;
			dr->dr_flags &= ~DR_BUSY;
			if (dr->dr_flags & DR_WANTED) {
				dr->dr_flags &= ~DR_WANTED;
				wakeup((caddr_t)dr);
			}
			return;
		}
		dr = dr->dr_chain;
	}
	cmn_err(CE_PANIC, "svckudp_dup_success: not in cache");
}

svckudp_dup(req)
	register struct svc_req *req;
{
	register SVCXPRT *xp;
	register struct dupreq *dr;
	register struct duphash *dhp;
	register ulong xid;
	register int ret;

	dupchecks++;
	xp = req->rq_xprt;
	xid = REQTOXID(req);
	dhp = &drhashtbl[XIDHASH(xid)];
	dr = dhp->dh_chain;
	while (dr != NULL) {
		if ((dr->dr_xid == xid) &&
		    (dr->dr_prog == req->rq_prog) &&
		    (dr->dr_vers == req->rq_vers) &&
		    (dr->dr_proc == req->rq_proc) &&
		    (dr->dr_alen == xp->xp_addrlen) &&
		    (bcmp((caddr_t)&dr->dr_addr, 
			  (caddr_t)xp->xp_raddr, 
			  sizeof(dr->dr_addr)) == 0)) {
			ret = dr->dr_valid;
			dupreqs += ret;
			return(ret);
		}
		dr = dr->dr_chain;
	}
	cmn_err(CE_PANIC, "svckudp_dup: not in cache");
}

static
unhash(dr)
	register struct dupreq *dr;
{
	register struct duphash *dhp;
	register struct dupreq *drt;
	register struct dupreq *drtprev;

	dhp = &drhashtbl[DRHASH(dr)];
	drt = dhp->dh_chain;
	drtprev = NULL;
	while (drt != NULL) {
		if (drt == dr) {
			if (drtprev == NULL) {
				dhp->dh_chain = drt->dr_chain;
			} else {
				drtprev->dr_chain = drt->dr_chain;
			}
			return;
		}
		drtprev = drt;
		drt = drt->dr_chain;
	}
}
