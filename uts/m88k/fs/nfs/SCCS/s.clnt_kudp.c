h27199
s 00474/00000/00000
d D 1.1 90/03/06 12:43:21 root 1 0
c date and time created 90/03/06 12:43:21 by root
e
u
U
t
T
I 1
/*
 *	Copyright 1986 Lachman Associates Inc.
 *	Copyright (C) 1984, Sun Microsystems, Inc.
 */
#ifndef lint
static char SysVNFSID[] = "@(#)clnt_kudp.c	2.15 LAI System V NFS Release 3.0/V3 source";
#endif
/*      @(#)clnt_kudp.c 1.36 85/12/23 SMI      */

/*
 * Implements a kernel UPD/IP based, client side RPC.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/psw.h"
#include "sys/pcb.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/rpc_msg.h"
#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/errno.h"

#include "bsd/netinet/in.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

extern ku_recvfrom();

int	ckuwakeup();

enum clnt_stat	clntkudp_callit();
void		clntkudp_abort();
void		clntkudp_error();
bool_t		clntkudp_freeres();
void		clntkudp_destroy();

void	xdrmem_create();

/*
 * Operations vector for UDP/IP based RPC
 */
static struct clnt_ops udp_ops = {
	clntkudp_callit,	/* do rpc call */
	clntkudp_abort,		/* abort call */
	clntkudp_error,		/* return error status */
	clntkudp_freeres,	/* free results */
	clntkudp_destroy	/* destroy rpc handle */
};

/*
 * Private data per rpc handle.  This structure is allocated by
 * clntkudp_create, and freed by cku_destroy.
 */
struct cku_private {
	uint			 cku_flags;	/* see below */
	CLIENT			 cku_client;	/* client handle */
	int			 cku_retrys;	/* request retrys */
	int			 cku_sock;	/* open udp socket */
	struct sockaddr_in	 cku_addr;	/* remote address */
	struct rpc_err		 cku_err;	/* error status */
	XDR			 cku_outxdr;	/* xdr routine for output */
	XDR			 cku_inxdr;	/* xdr routine for input */
	uint			 cku_outpos;	/* position of in output mbuf */
	char			*cku_outbuf;	/* output buffer */
	char			*cku_inbuf;	/* input buffer */
	struct ucred		*cku_cred;	/* credentials */
};

struct {
	int	rccalls;
	int	rcbadcalls;
	int	rcretrans;
	int	rcbadxids;
	int	rctimeouts;
	int	rcwaits;
	int	rcnewcreds;
} rcstat;


#define	ptoh(p)		(&((p)->cku_client))
#define	htop(h)		((struct cku_private *)((h)->cl_private))

/* cku_flags */
#define	CKU_TIMEDOUT	0x001
#define	CKU_BUSY	0x002
#define	CKU_WANTED	0x004
#define	CKU_DONTINT	0x008

/* Times to retry */
#define	RECVTRIES	2
#define	SNDTRIES	4

int	clntkudpxid;		/* transaction id used by all clients */

static
noop()
{
}

/*
 * Create an rpc handle for a udp rpc connection.
 * Allocates space for the handle structure and the private data, and
 * opens a socket.  Note sockets and handles are one to one.
 */
CLIENT *
clntkudp_create(addr, alen, pgm, vers, retrys, cred)
	caddr_t addr;
	uint alen;
	ulong pgm;
	ulong vers;
	int retrys;
	struct ucred *cred;
{
	register CLIENT *h;
	register struct cku_private *p;
	int error = 0;
	struct rpc_msg call_msg;
	extern int nfs_portmon;
	extern AUTH *authkern_create();
	char *dst;
	int length;

	/* allocate the space for the client handle */
	p = (struct cku_private *)kmem_alloc((uint)sizeof(*p));
	bzero((caddr_t)p, sizeof(*p));
	h = ptoh(p);

	if (!clntkudpxid) {
		clntkudpxid = (int)time;
	}

	/* handle */
	h->cl_ops = &udp_ops;
	h->cl_private = (caddr_t)p;
	h->cl_auth = authkern_create();

	/* call message, just used to pre-serialize below */
	call_msg.rm_xid = 0;
	call_msg.rm_direction = CALL;
	call_msg.rm_call.cb_rpcvers = RPC_MSG_VERSION;
	call_msg.rm_call.cb_prog = pgm;
	call_msg.rm_call.cb_vers = vers;

	/* private */
	clntkudp_init(h, addr, alen, retrys, cred);
	p->cku_outbuf = (char *)kmem_alloc((uint)UDPMSGSIZE);
	p->cku_inbuf = (char *)kmem_alloc((uint)UDPMSGSIZE);
	xdrmem_create(&p->cku_outxdr, p->cku_outbuf, UDPMSGSIZE, XDR_ENCODE);

	/* pre-serialize call message header */
	if (!xdr_callhdr(&(p->cku_outxdr), &call_msg)) {
		printf("clntkudp_create - Fatal header serialization error.\n");
		goto bad;
	}
	p->cku_outpos = XDR_GETPOS(&(p->cku_outxdr));

	/* open udp socket */
	error = udpsocket(&p->cku_sock, NULL);
	if (error) {
		printf("clntkudp_create: socket creation problem, %d\n", error);
		goto bad;
	}

	return(h);

bad:
	if (h->cl_auth)
		authkern_destroy(h->cl_auth);
	kmem_free((caddr_t)p->cku_outbuf, (uint)UDPMSGSIZE);
	kmem_free((caddr_t)p->cku_inbuf, (uint)UDPMSGSIZE);
	kmem_free((caddr_t)p, (uint)sizeof(*p));
	return((CLIENT *)NULL);
}

clntkudp_init(h, addr, alen, retrys, cred)
	CLIENT *h;
	caddr_t addr;
	uint alen;
	int retrys;
	struct ucred *cred;
{
	struct cku_private *p = htop(h);

	p->cku_retrys = retrys;
	p->cku_addr = *((struct sockaddr_in *)addr);
	p->cku_cred = cred;
	p->cku_flags = 0;
}

void
clntkudp_init2(h, dontint)
	CLIENT  *h;
	int     dontint;
{
#if 0
	register CKU *p = htop(h);

	if (dontint)
		p->cku_flags |= CKU_DONTINT;
#endif
}

/*
 * Time out back off function. tim is in hz
 */
#define MAXTIMO	(256 * HZ)
#define backoff(tim)	((((tim) << 1) > MAXTIMO) ? MAXTIMO : ((tim) << 1))

/*
 * Call remote procedure.
 * Most of the work of rpc is done here.  We serialize what is left
 * of the header (some was pre-serialized in the handle), serialize
 * the arguments, and send it off.  We wait for a reply or a time out.
 * Timeout causes an immediate return, other packet problems may cause
 * a retry on the receive.  When a good packet is received we deserialize
 * it, and check verification.  A bad reply code will cause one retry
 * with full (longhand) credentials.
 */
enum clnt_stat 
clntkudp_callit(h, procnum, xdr_args, argsp, xdr_results, resultsp, wait)
	register CLIENT *h;
	ulong procnum;
	xdrproc_t xdr_args;
	caddr_t argsp;
	xdrproc_t xdr_results;
	caddr_t resultsp;
	struct timeval wait;
{
	register struct cku_private *p = htop(h);
	register XDR *xdrs;
	int rtries;
	int stries = p->cku_retrys;
	struct sockaddr_in from;
	struct rpc_msg reply_msg;
	int s;
	struct ucred tmpcred;
	int timohz;
	ulong xid;
	int timeout_ident;
	int len;

	rcstat.rccalls++;

	while (p->cku_flags & CKU_BUSY) {
		rcstat.rcwaits++;
		p->cku_flags |= CKU_WANTED;
		sleep((caddr_t)h, PZERO-2);
	}
	p->cku_flags |= CKU_BUSY;

	/*
	 * Set credentials into the u structure
	 */
	tmpcred.cr_uid = u.u_uid;
	tmpcred.cr_gid = u.u_gid;
	tmpcred.cr_ruid = u.u_ruid;
	tmpcred.cr_rgid = u.u_rgid;
	u.u_uid = (ushort)p->cku_cred->cr_uid;
	u.u_gid = (ushort)p->cku_cred->cr_gid;
	u.u_ruid = (ushort)p->cku_cred->cr_ruid;
	u.u_rgid = (ushort)p->cku_cred->cr_rgid; 


	xid = clntkudpxid++;

	/*
	 * This is dumb but easy: keep the time out in units of hz
	 * so it is easy to call timeout and modify the value.
	 */
	timohz = wait.tv_sec;

call_again:
	/*
	 * The transaction id is the first thing in the
	 * preserialized output buffer.
	 */
	(*(ulong *)(p->cku_outbuf)) = xid;

	xdrmem_create(&p->cku_outxdr, p->cku_outbuf, UDPMSGSIZE, XDR_ENCODE);
	xdrs = &p->cku_outxdr;
	XDR_SETPOS(xdrs, p->cku_outpos);

	/*
	 * Serialize dynamic stuff into the output buffer.
	 */
	if (!XDR_PUTLONG(xdrs, (long *)&procnum) ||
	    !AUTH_MARSHALL(h->cl_auth, xdrs) ||
	    !(*xdr_args)(xdrs, argsp)) {
		p->cku_err.re_status = RPC_CANTENCODEARGS;
		goto done;
	}

	len = (int)XDR_GETPOS(&(p->cku_outxdr));

	if (p->cku_err.re_errno =
	    ku_sendto(p->cku_sock, &p->cku_addr, p->cku_outbuf, len)) {
		p->cku_err.re_status = RPC_CANTSEND;
		goto done;
	}

	reply_msg.acpted_rply.ar_verf = _null_auth;
	reply_msg.acpted_rply.ar_results.where = resultsp;
	reply_msg.acpted_rply.ar_results.proc = xdr_results;

	for (rtries = RECVTRIES; rtries; rtries--) {
		s = splnet();
		if (nfssbwait(p->cku_sock, timohz) < 0) {
			(void) splx(s);
			if ( u.u_error == EINTR) {
				p->cku_err.re_status = RPC_INTERRUPTED;
				p->cku_err.re_errno = EINTR;
				goto noretry;
			}
			else {
				p->cku_err.re_status = RPC_TIMEDOUT;
				p->cku_err.re_errno = ETIME;
				rcstat.rctimeouts++;
				goto done;
			}
		}
		len = ku_recvfrom(p->cku_sock, &from, p->cku_inbuf, 
				UDPMSGSIZE);
		u.u_error = 0;
		(void) splx(s);

		if (len < sizeof(ulong)) {
			continue;
		}

		/*
		 * If reply transaction id matches id sent
		 * we have a good packet.
		 */
		if (*((ulong *)(p->cku_inbuf))
		    != *((ulong *)(p->cku_outbuf))) {
			rcstat.rcbadxids++;
			continue;
		}
		break;
	} 

	if (rtries == 0) {
		p->cku_err.re_status = RPC_CANTRECV;
		goto done;
	}

	/*
	 * Process reply
	 */

	xdrs = &(p->cku_inxdr);
	xdrmem_create(xdrs, p->cku_inbuf, len, XDR_DECODE);

	/*
	 * Decode and validate the response.
	 */
	if (xdr_replymsg(xdrs, &reply_msg)) {
		_seterr_reply(&reply_msg, &(p->cku_err));

		if (p->cku_err.re_status == RPC_SUCCESS) {
			/*
			 * Reply is good, check auth.
			 */
			if (!AUTH_VALIDATE(h->cl_auth,
			    &reply_msg.acpted_rply.ar_verf)) {
				p->cku_err.re_status = RPC_AUTHERROR;
				p->cku_err.re_why = AUTH_INVALIDRESP;
			}
			if (reply_msg.acpted_rply.ar_verf.oa_base != NULL) {
				/* free auth handle */
				xdrs->x_op = XDR_FREE;
				(void) xdr_opaque_auth(xdrs,
				    &(reply_msg.acpted_rply.ar_verf));
			} 
		}
	} else {
		p->cku_err.re_status = RPC_CANTDECODERES;
	}

done:
	if (p->cku_err.re_status != RPC_SUCCESS &&
	    p->cku_err.re_status != RPC_CANTENCODEARGS &&
	    --stries > 0) {
		rcstat.rcretrans++;
		timohz = backoff(timohz);
		if (p->cku_err.re_status == RPC_SYSTEMERROR ||
		    p->cku_err.re_status == RPC_CANTSEND) {
			/*
			 * Errors due to lack o resources, wait a bit
			 * and try again.
			 */
			 delay(HZ);
		}
		goto call_again;
	}

noretry:
	u.u_uid = tmpcred.cr_uid;
	u.u_gid = tmpcred.cr_gid;
	u.u_ruid = tmpcred.cr_ruid;
	u.u_rgid = tmpcred.cr_rgid;

	p->cku_flags &= ~CKU_BUSY;
	if (p->cku_flags & CKU_WANTED) {
		p->cku_flags &= ~CKU_WANTED;
		wakeup((caddr_t)h);
	}

	if (p->cku_err.re_status != RPC_SUCCESS) {
		rcstat.rcbadcalls++;
	}

	return(p->cku_err.re_status);
}

/*
 * Return error info on this handle.
 */
void
clntkudp_error(h, err)
	CLIENT *h;
	struct rpc_err *err;
{
	register struct cku_private *p = htop(h);

	*err = p->cku_err;
}

static bool_t
clntkudp_freeres(cl, xdr_res, res_ptr)
	CLIENT *cl;
	xdrproc_t xdr_res;
	caddr_t res_ptr;
{
	register struct cku_private *p = (struct cku_private *)cl->cl_private;
	register XDR *xdrs = &(p->cku_outxdr);

	xdrs->x_op = XDR_FREE;
	return((*xdr_res)(xdrs, res_ptr));
}

void 
clntkudp_abort()
{
}

/*
 * Destroy rpc handle.
 * Frees the space used for output buffer, private data, and handle
 * structure, and closes the socket for this handle.
 */
void
clntkudp_destroy(h)
	CLIENT *h;
{
	register struct cku_private *p = htop(h);

	(void) soclose(p->cku_sock);
	if (h->cl_auth)
		authkern_destroy(h->cl_auth);
	kmem_free((caddr_t)p->cku_outbuf, (uint)UDPMSGSIZE);
	kmem_free((caddr_t)p->cku_inbuf, (uint)UDPMSGSIZE);
	kmem_free((caddr_t)p, (uint)sizeof(*p));
}
E 1
