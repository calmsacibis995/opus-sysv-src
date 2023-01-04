h47399
s 00439/00000/00000
d D 1.1 90/03/06 12:43:35 root 1 0
c date and time created 90/03/06 12:43:35 by root
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
/*	Copyright (C) 1984, Sun Microsystems, Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)svc.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif
/*
 * static char sccsid[] = "@(#)svc.c 1.30 85/11/19 Copyr 1984 Sun Micro";
 */

/*
 * Server-side remote procedure call interface.
 *
 * There are two sets of procedures here.  The xprt routines are
 * for handling transport handles.  The svc routines handle the
 * list of service routines.
 */

#include "sys/param.h"
#include "sys/types.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/rpc_msg.h"
#include "sys/fs/nfs/svc.h"
#include "sys/fs/nfs/svc_auth.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

caddr_t rqcred_head = NULL;	/* head of cached, free authentication */
				/* parameters */

#define NULL_SVC	((struct svc_callout *)0)
#define RQCRED_SIZE	400

/*
 * The services list
 * Each entry represents a set of procedures (an rpc program).
 * The dispatch routine takes request structs and runs the
 * apropriate procedure.
 */
static struct svc_callout {
	struct svc_callout *sc_next;
	ulong sc_prog;
	ulong sc_vers;
	void (*sc_dispatch)();
} *svc_head;

static struct svc_callout *svc_find();

/* ***************  SVCXPRT related stuff **************** */

/*
 * Activate a transport handle.
 */
/* ARGSUSED */
void
xprt_register(xprt)
	SVCXPRT *xprt;
{
}

/* ********************** CALLOUT list related stuff ************* */

/*
 * Add a service program to the callout list.
 * The dispatch routine will be called when a rpc request for this
 * program number comes in.
 */
/* ARGSUSED */
bool_t
svc_register(xprt, prog, vers, dispatch, protocol)
	SVCXPRT *xprt;
	ulong prog;
	ulong vers;
	void (*dispatch)();
	int protocol;
{
	struct svc_callout *prev;
	register struct svc_callout *s;
	register struct svc_callout *ns;

	if ((s = svc_find(prog, vers, &prev)) != NULL_SVC) {
		if (s->sc_dispatch == dispatch) {
			goto pmap_it;  /* he is registering another xprt */
		}
		return(FALSE);
	}
	s = (struct svc_callout *)kmem_alloc((uint)sizeof(*s));
	if ((ns = svc_find(prog, vers, &prev)) != NULL_SVC) {
		kmem_free((caddr_t)s, (uint)sizeof(*s));
		if (ns->sc_dispatch == dispatch) {
			goto pmap_it;	/* he is registering another xprt */
		}
		return(FALSE);
	}
	s->sc_prog = prog;
	s->sc_vers = vers;
	s->sc_dispatch = dispatch;
	s->sc_next = svc_head;
	svc_head = s;
pmap_it:
	return(TRUE);
}

/*
 * Remove a service program from the callout list.
 */
void
svc_unregister(prog, vers)
	ulong prog;
	ulong vers;
{
	struct svc_callout *prev;
	register struct svc_callout *s;

	if ((s = svc_find(prog, vers, &prev)) == NULL_SVC) {
		return;
	}
	if (prev == NULL_SVC) {
		svc_head = s->sc_next;
	} else {
		prev->sc_next = s->sc_next;
	}
	kmem_free((caddr_t)s, (uint)sizeof(*s));
}

/*
 * Search the callout list for a program number, return the callout
 * struct.
 */
static struct svc_callout *
svc_find(prog, vers, prev)
	ulong prog;
	ulong vers;
	struct svc_callout **prev;
{
	register struct svc_callout *s, *p;

	p = NULL_SVC;
	for (s = svc_head; s != NULL_SVC; s = s->sc_next) {
		if ((s->sc_prog == prog) && (s->sc_vers == vers))
			goto done;
		p = s;
	}
done:
	*prev = p;
	return(s);
}

/* ******************* REPLY GENERATION ROUTINES  ************ */

/*
 * Send a reply to an rpc request
 */
bool_t
svc_sendreply(xprt, xdr_results, xdr_location)
	register SVCXPRT *xprt;
	xdrproc_t xdr_results;
	caddr_t xdr_location;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = SUCCESS;
	rply.acpted_rply.ar_results.where = xdr_location;
	rply.acpted_rply.ar_results.proc = xdr_results;
	return(SVC_REPLY(xprt, &rply));
}

/*
 * No procedure error reply
 */
void
svcerr_noproc(xprt)
	register SVCXPRT *xprt;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = PROC_UNAVAIL;
	SVC_REPLY(xprt, &rply);
}

/*
 * Can't decode args error reply
 */
void
svcerr_decode(xprt)
	register SVCXPRT *xprt;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = GARBAGE_ARGS;
	SVC_REPLY(xprt, &rply);
}

/*
 * Some system error
 */
void
svcerr_systemerr(xprt)
	register SVCXPRT *xprt;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = SYSTEM_ERR;
	SVC_REPLY(xprt, &rply);
}

/*
 * Authentication error reply
 */
void
svcerr_auth(xprt, why)
	SVCXPRT *xprt;
	enum auth_stat why;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_DENIED;
	rply.rjcted_rply.rj_stat = AUTH_ERROR;
	rply.rjcted_rply.rj_why = why;
	SVC_REPLY(xprt, &rply);
}

/*
 * Auth too weak error reply
 */
void
svcerr_weakauth(xprt)
	SVCXPRT *xprt;
{

	svcerr_auth(xprt, AUTH_TOOWEAK);
}

/*
 * Program unavailable error reply
 */
void
svcerr_noprog(xprt)
	register SVCXPRT *xprt;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = PROG_UNAVAIL;
	SVC_REPLY(xprt, &rply);
}

/*
 * Program version mismatch error reply
 */
void
svcerr_progvers(xprt, low_vers, high_vers)
	register SVCXPRT *xprt;
	ulong low_vers;
	ulong high_vers;
{
	struct rpc_msg rply;

	rply.rm_direction = REPLY;
	rply.rm_reply.rp_stat = MSG_ACCEPTED;
	rply.acpted_rply.ar_verf = xprt->xp_verf;
	rply.acpted_rply.ar_stat = PROG_MISMATCH;
	rply.acpted_rply.ar_vers.low = low_vers;
	rply.acpted_rply.ar_vers.high = high_vers;
	SVC_REPLY(xprt, &rply);
}

/* ******************* SERVER INPUT STUFF ******************* */

/*
 * Get server side input from some transport.
 *
 * Statement of authentication parameters management:
 * This function owns and manages all authentication parameters, specifically
 * the "raw" parameters (msg.rm_call.cb_cred and msg.rm_call.cb_verf) and
 * the "cooked" credentials (rqst->rq_clntcred).  However, this function
 * does not know the structure of the cooked credentials, so it make the
 * following two assumptions: a) the structure is contiguous (no pointers), and
 * b) the structure size does not exceed RQCRED_SIZE bytes.
 * In all events, all three parameters are freed upon exit from this routine.
 * The storage is trivially management on the call stack in user land, but
 * is mallocated in kernel land.
 */
void
svc_getreq(xprt)
	register SVCXPRT *xprt;
{
	register enum xprt_stat stat;
	int prog_found;
	ulong low_vers;
	ulong high_vers;
	caddr_t cred_area;	/* too big to allocate on call stack */
	void (*dispatch)();
	struct svc_req r;
	struct rpc_msg msg;

	/*
	 * Firstly, allocate the authentication parameters' storage
	 */
	if (rqcred_head) {
		cred_area = rqcred_head;
		rqcred_head = *(caddr_t *)rqcred_head;
	} else {
		cred_area = kmem_alloc((uint)(2*MAX_AUTH_BYTES+RQCRED_SIZE));
	}
	msg.rm_call.cb_cred.oa_base = cred_area;
	msg.rm_call.cb_verf.oa_base = &cred_area[MAX_AUTH_BYTES];
	r.rq_clntcred = &cred_area[2*MAX_AUTH_BYTES];

	/* now receive msgs from xprtprt (support batch calls) */
	do {
		u.u_error = 0;
		if (SVC_RECV(xprt, &msg)) {

			/* now find the exported program and call it */
			register struct svc_callout *s;
			enum auth_stat why;

			r.rq_xprt = xprt;
			r.rq_prog = msg.rm_call.cb_prog;
			r.rq_vers = msg.rm_call.cb_vers;
			r.rq_proc = msg.rm_call.cb_proc;
			r.rq_cred = msg.rm_call.cb_cred;
			/* first authenticate the message */
			if ((why =  _authenticate(&r, &msg)) != AUTH_OK) {
				svcerr_auth(xprt, why);
				goto call_done;
			}
			/* now match message with a registered service*/
			prog_found = FALSE;
			low_vers = 0 - 1;
			high_vers = 0;
			for (s = svc_head; s != NULL_SVC; s = s->sc_next) {
				if (s->sc_prog == r.rq_prog) {
					if (s->sc_vers == r.rq_vers) {
						dispatch = s->sc_dispatch;
						(*dispatch)(&r, xprt);
						goto call_done;
					}  /* found correct version */
					prog_found = TRUE;
					if (s->sc_vers < low_vers)
						low_vers = s->sc_vers;
					if (s->sc_vers > high_vers)
						high_vers = s->sc_vers;
				}   /* found correct program */
			}
			/*
			 * if we got here, the program or version
			 * is not served ...
			 */
			if (prog_found)
				svcerr_progvers(xprt, low_vers, high_vers);
			else
				svcerr_noprog(xprt);
		} else {
			/*
			 * SVC_RECV returned NULL.  This may be due to the nfsd
			 * being killed.  If it is, u.u_error will be set
			 * to EINTR.  If so, longjmp back through u.u_qsav.
			 */
			if (u.u_error == EINTR) {
				/*
				 * free authentication parameters' storage
				 */
				*(caddr_t *)cred_area = rqcred_head;
				rqcred_head = cred_area;
				longjmp(u.u_qsav);
			}
		}
	call_done:
		if ((stat = SVC_STAT(xprt)) == XPRT_DIED) {
			SVC_DESTROY(xprt);
			break;
		}
	} while (stat == XPRT_MOREREQS);
	/*
	 * free authentication parameters' storage
	 */
	*(caddr_t *)cred_area = rqcred_head;
	rqcred_head = cred_area;
}

int Rpccnt = 0;

/*
 * This is the rpc server side idle loop
 * Wait for input, call server program.
 */
void
svc_run(xprt)
	SVCXPRT *xprt;
{
	while (TRUE) {
		svc_getreq(xprt);
		Rpccnt++;
	}
}
E 1
