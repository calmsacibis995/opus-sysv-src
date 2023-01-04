h47703
s 00552/00000/00000
d D 1.1 90/03/06 12:43:25 root 1 0
c date and time created 90/03/06 12:43:25 by root
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
/* @(#)klm_lockmgr.c	1.3 86/12/23 NFSSRC */
/*      Copyright (C) 1984, Sun Microsystems, Inc.  */
#ifndef lint
static char SysVr3NFSID[] = "@(#)klm_lockmgr.c	4.2 LAI System V NFS Release 3.2/V3	source";
#endif
/*
 * Kernel<->Network Lock-Manager Interface
 *
 * File- and Record-locking requests are forwarded (via RPC) to a
 * Network Lock-Manager running on the local machine.  The protocol
 * for these transactions is defined in /usr/src/protocols/klm_prot.x
 */

#include "sys/types.h"
#include "sys/param.h"
#ifdef	u3b2
#include "sys/sbd.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#ifdef	u3b
#include "sys/page.h"
#include "sys/region.h"
#endif
#if i386 || clipper || m88k
#include "sys/immu.h"
#include "sys/region.h"
#endif
#include "sys/cmn_err.h"
#include "sys/conf.h"
#include "sys/debug.h"
#include "sys/errno.h"
#include "sys/fcntl.h"
#include "sys/file.h"
#include "sys/flock.h"
#include "sys/fs/s5dir.h"
#include "sys/fstyp.h"
#include "sys/inode.h"
#include "sys/mount.h"
#include "sys/proc.h"
#include "sys/signal.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/user.h"
#include "sys/utsname.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/klm_prot.h"
#include "sys/fs/nfs/trans.h"
#include "sys/fs/nfs/lockmgr.h"
#include "sys/stream.h"
#include "bsd/netinet/in.h"
#include "sys/endian.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/auth_unix.h"
#include "sys/fs/nfs/clnt.h"
					/* XXX, xprt dependence on sockets */
static struct sockaddr_in lm_sa;	/* talk to portmapper & lock-manager */

int talk_to_lockmgr();

extern int wakeup();
extern char *rpcstatnames[];
extern int lockd_open;

static int klm_debug = 0;

/* Define static parameters for run-time tuning */
static int backoff_timeout = 10;	/* time to wait on klm_denied_nolocks */
static int first_retry = 0;		/* first attempt if klm port# known */
static int first_timeout = 1;
static int normal_retry = 1;		/* attempts after new port# obtained */
static int normal_timeout = 10;
static int working_retry = 0;		/* attempts after klm_working */
static int working_timeout = 10;


/*
 * klm_lockctl - process a lock/unlock/test-lock request
 *
 * Calls (via RPC) the local lock manager to register the request.
 * Lock requests are cancelled if interrupted by signals.
 */
klm_lockctl(lh, ld, cmd, cred)
	register lockhandle_t *lh;
	register struct flock *ld;
	int cmd;
	struct ucred *cred;
{
	register int error;
	klm_lockargs args;
	klm_testrply reply;
	ulong xdrproc;
	xdrproc_t xdrargs;
	xdrproc_t xdrreply;
	int sleep_disp = (PZERO+1)|PCATCH;
	int toid;

	/* initialize sockaddr_in used to talk to local processes */
	if (lm_sa.sin_port == 0) {
		lm_sa.sin_addr.s_addr = htonl(INADDR_ANY);
		lm_sa.sin_family = AF_INET;
#ifndef FINISH32
		lm_sa.sin_port = htons(KLM_PORT);	/* avoid pmap stuff */
#endif /* FINISH32 */
	}

	args.block = FALSE;
	args.exclusive = FALSE;
	args.lock.fh.n_bytes = (char *)&lh->lh_id;
	args.lock.fh.n_len = sizeof(lh->lh_id);
	args.lock.server_name = lh->lh_servername;
	args.lock.pid = (int)u.u_procp->p_pid;
	args.lock.l_offset = ld->l_start;
	args.lock.l_len = ld->l_len;
	xdrproc = KLM_LOCK;
	xdrargs = (xdrproc_t)xdr_klm_lockargs;
	xdrreply = (xdrproc_t)xdr_klm_stat;

	/* now modify the lock argument structure for specific cases */
	switch (ld->l_type) {
	case F_WRLCK:
		args.exclusive = TRUE;
		break;
	case F_UNLCK:
		xdrproc = KLM_UNLOCK;
		xdrargs = (xdrproc_t)xdr_klm_unlockargs;
		break;
	}

	switch (cmd) {
	case F_SETLKW:
		args.block = TRUE;
		break;
	case F_GETLK:
		xdrproc = KLM_TEST;
		xdrargs = (xdrproc_t)xdr_klm_testargs;
		xdrreply = (xdrproc_t)xdr_klm_testrply;
		break;
	}

requestloop:
	/* send the request out to the local lock-manager and wait for reply */
	error = talk_to_lockmgr(xdrproc,xdrargs, &args, xdrreply, &reply, cred,
		sleep_disp);	/* sometimes we want noninterruptible RPC */
	if (error == ENOLCK) {
		goto ereturn;	/* no way the request could have gotten out */
	}

	/*
	 * The only other possible return values are:
	 *   klm_granted  |  klm_denied  | klm_denied_nolocks |  EINTR
	 */
	switch (xdrproc) {
	case KLM_LOCK:
		switch (error) {
		case klm_granted:
			error = 0;		/* got the requested lock */
			goto ereturn;
		case klm_denied:
			if (args.block) {
				cmn_err(CE_WARN,
					"klm_lockmgr: blocking lock denied?!");
				goto requestloop;	/* loop forever */
			}
			error = EACCES;		/* EAGAIN?? */
			goto ereturn;
		case klm_denied_nolocks:
			error = ENOLCK;		/* no resources available?! */
			goto ereturn;
		case EINTR:
			if (args.block)
				goto cancel;	/* cancel blocking locks */
			else {
				/*
				 * Can't blindly abandon operation -
				 * have to clean up resources on lockmgr.
				 */
				sleep_disp = PZERO;	/* don't catch
								interrupts */
				goto requestloop;	/* loop forever */
			}
		}

	case KLM_UNLOCK:
		switch (error) {
		case klm_granted:
			error = 0;
			goto ereturn;
		case klm_denied:
			cmn_err(CE_WARN, "klm_lockmgr: unlock denied?!");
			error = EINVAL;
			goto ereturn;
		case klm_denied_nolocks:
			goto nolocks_wait;	/* back off; loop forever */
		case EINTR:
			/*
			 * Can't blindly abandon operation - have to clean
			 * up resources on lockmgr.
			 */
			sleep_disp = PZERO;	/* don't catch interrupts
							anymore */
			goto requestloop;	/* loop forever */
		}

	/*
	 * We allow KLM_TEST to be always interruptible, since it holds
	 * no resources.
	 */
	case KLM_TEST:
		switch (error) {
		case klm_granted:
			ld->l_type = F_UNLCK;	/* mark lock available */
			error = 0;
			goto ereturn;
		case klm_denied:
			ld->l_type = (reply.klm_testrply.holder.exclusive) ?
				     F_WRLCK : F_RDLCK;
			ld->l_start = reply.klm_testrply.holder.l_offset;
			ld->l_len = reply.klm_testrply.holder.l_len;
			ld->l_pid = reply.klm_testrply.holder.svid;
			error = 0;
			goto ereturn;
		case klm_denied_nolocks:
			/*
			 * We won't loop forever, if signal sent during
			 * nolocks_wait.
			 */
			goto nolocks_wait;	/* back off; loop forever */
		case EINTR:
			/*
			 * Unwinding the system call might make more
			 * sense.
			 * We CAN blindly abandon operation - there are no
			 * resources to clean up on lockmgr.
			 */
			goto ereturn;
		}
	}

/*NOTREACHED*/
nolocks_wait:
	toid = timeout(wakeup, (caddr_t)&lm_sa, (backoff_timeout * HZ));
	if (sleep((caddr_t)&lm_sa, sleep_disp)) { /* Sun had (PZERO|PCATCH)
						     What good can that do? */
		error = EINTR;
	} else
		error = 0;
	untimeout(toid);
	if (error)
		goto ereturn;
	else
		goto requestloop;	/* now try again */

cancel:
	/*
	 * If we get here, a signal interrupted a rqst that must be cancelled.
	 * Change the procedure number to KLM_CANCEL and reissue the exact same
	 * request.  Use the results to decide what return value to give.
	 */
	xdrproc = KLM_CANCEL;
	error = talk_to_lockmgr(xdrproc,xdrargs, &args, xdrreply, &reply, cred,
				PZERO);	/* Interrupts not caught */
	switch (error) {
	case klm_granted:
		error = 0;		/* lock granted */
		goto ereturn;
	case klm_denied:
		/* may want to take a longjmp here */
		error = EINTR;
		goto ereturn;
	case EINTR:
		/*
		 * because of issig() stuff, we can still get this even if
		 * sleeping on PZERO
		 */
		{
		struct proc *p;

		p = u.u_procp;
#if m88k
		p->p_sig.s[0] = 0;
		p->p_sig.s[1] = 0;
#else
		p->p_sig = 0;	/* clear all pending interrupt signals */
#endif
		}
		goto cancel;	/* ignore signals til cancel succeeds */

	case klm_denied_nolocks:
		error = ENOLCK;		/* no resources available?! */
		goto ereturn;
	case ENOLCK:
		cmn_err(CE_WARN, "klm_lockctl: ENOLCK on KLM_CANCEL request");
		goto ereturn;
	}
	/* NOTREACHED */
ereturn:
	return(error);
}


/*
 * Send the given request to the local lock-manager.
 * If timeout or error, go back to the portmapper to check the port number.
 * This routine loops forever until one of the following occurs:
 *	1) A legitimate (not 'klm_working') reply is returned (returns 'stat').
 *
 *	2) A signal occurs (returns EINTR).  In this case, at least one try
 *	   has been made to do the RPC; this protects against jamming the
 *	   CPU if a KLM_CANCEL request has yet to go out.
 *
 *	3) A drastic error occurs (e.g., the local lock-manager has never
 *	   been activated OR cannot create a client-handle) (returns ENOLCK).
 */
talk_to_lockmgr(xdrproc, xdrargs, args, xdrreply, reply, cred, sleep_disp)
	ulong xdrproc;
	xdrproc_t xdrargs;
	klm_lockargs *args;
	xdrproc_t xdrreply;
	klm_testrply *reply;
	struct ucred *cred;
	int sleep_disp;
{
	register CLIENT *client;
	struct timeval tmo;
	register int error;
	mblk_t *bp;			/* for sobind() request */
	struct socket *so;		/* for socket creation */
	int toid;

	/* XXX - should use clget() /
	/* set up a client handle to talk to the local lock manager */

	client = clntkudp_create(&lm_sa, sizeof(lm_sa), (ulong)KLM_PROG,
				 (ulong)KLM_VERS, first_retry, cred);

	if (client == (CLIENT *)NULL) {
		return(ENOLCK);
	}
	/*
	 * Set dontint option if PCATCH unset.  XXX Add functionality to
	 * clntkudp_create() and clntkudp_init() lock manager changed to
	 * use clget():
	 */
	if (!(sleep_disp & PCATCH))
		(void) clntkudp_init2(client, 1);
	tmo.tv_sec = first_timeout;
	tmo.tv_usec = 0;

	/*
	 * If cached port number, go right to CLNT_CALL().
	 * This works because timeouts go back to the portmapper to
	 * refresh the port number.
	 */
	if (lm_sa.sin_port != 0) {
		goto retryloop;		/* skip first portmapper query */
	}

	for (;;) {
remaploop:
#ifdef FINISH32		/* XXX, implement for portmapper support */
		/*
		 * Go get the port number from the portmapper...
		 * If return 1, signal was received before portmapper answered;
		 * If return -1, the lock-manager is not registered
		 * Else, got a port number
		 */
		switch (getport_loop(&lm_sa, (ulong)KLM_PROG,
				     (ulong)KLM_VERS, (ulong)KLM_PROTO)) {
		case 1:
			error = EINTR;		/* signal interrupted things */
			goto out;

		case -1:
			cmn_err(CE_WARN,
				"fcntl: Local lock-manager not registered");
			error = ENOLCK;
			goto out;
		}

		/*
		 * If a signal occurred, pop back out to the higher
		 * level to decide what action to take.  If we just
		 * got a port number from the portmapper, the next
		 * call into this subroutine will jump to retryloop.
		 */
		if (ISSIG(u.u_procp)) {
			error = EINTR;
			goto out;
		}
		/* reset the lock-manager client handle */
		(void) clntkudp_init(client, &lm_sa, sizeof(lm_sa),
				     normal_retry, cred);
		if (!(sleep_disp & PCATCH))
			(void) clntkudp_init2(client, 1);
		tmo.tv_sec = normal_timeout;
#else	/* for NON-portmapper support */
		/*
	 	 * Added check of for the lock daemon, part of switch logic for
	 	 * the local / network (s5/nfs) locking methods.
	 	 */
		if (lockd_open == 0) {
			cmn_err(CE_WARN,
				"talk_to_lockmgr: lock manager not present");
			return(ENOLCK);
		}
		/* 
		 * Now check to see if it is running by trying to
		 * bind to the lock manager port number.
		 */
#if clipper || ns32000 || m88k
		/*
		 * Transport Specific Code.
		 */
#if !m88k
#define	EADDRINUSE	48
#endif
		if (udpsocket(&so, &lm_sa)) {
			error = u.u_error;
			if (error != EADDRINUSE) {
				printf("talk_to_lockmgr: socket err 0x%x.\n",
				error);
				return ENOLCK;
			}
		}
		else {
			/* succeeded, so no lock manager */
			soclose(so);
			return ENOLCK;
		}
#else
		if (error = socreate(AF_INET, &so, SOCK_DGRAM, IPPROTO_UDP)) {
			cmn_err(CE_WARN,
				"talk_to_lockmgr: socket creation error %d.",
				error);
			return(ENOLCK);
		}
		bp = allocb(sizeof(TADDR), BPRI_LO);
		if (bp == NULL) {
			soclose(so);
			return(ENOLCK);
		}
		bp->b_wptr = bp->b_rptr + (sizeof(TADDR));
		bcopy(&lm_sa, bp->b_rptr, sizeof(lm_sa));
		error = sobind(so, bp, 0);
		soclose(so);
		freeb(bp);
		if (error != EADDRINUSE && error != EADDRNOTAVAIL) {
			return(ENOLCK);
		}
#endif
#endif /* FINISH32 */

retryloop:
		/* retry the request until completion, timeout, or error */
		for (;;) {
			error = (int) CLNT_CALL(client, xdrproc,
						xdrargs, (caddr_t)args,
						xdrreply, (caddr_t)reply, tmo);

			if (klm_debug) {
				printf(
				    "klm: pid:%d cmd:%d [%d,%d]  stat:%d/%d  ",
				    args->lock.pid,
				    (int) xdrproc,
				    args->lock.l_offset,
				    args->lock.l_len,
				    error, (int) reply->stat);
				printf("KLM_PORT = %d  ", KLM_PORT);
				printf("sin_port = %d\n",ntohs(lm_sa.sin_port));
			}

			switch (error) {
			case RPC_SUCCESS:
				error = (int)reply->stat;
				if (error == (int)klm_working) {
					if (issig() && (sleep_disp&PCATCH)) {
						error = EINTR;
						goto out;
					}
					/* lock-mgr is up...can wait longer */
					(void) clntkudp_init(client, &lm_sa,
							     sizeof(lm_sa),
					    		     working_retry,
							     cred);
					if (!(sleep_disp & PCATCH))
						(void) clntkudp_init2(client, 1);
					tmo.tv_sec = working_timeout;
					continue;	/* retry */
				}
				goto out;	/* got a legitimate answer */

			case RPC_TIMEDOUT:
				goto remaploop;	/* ask for port# again */

			case RPC_INTERRUPTED:
				error = EINTR;
				goto out;

			default:
#ifdef FINISH32
				printf("lock-manager: RPC error: %s\n",
				    clnt_sperrno((enum clnt_stat) error));
#else
				cmn_err(CE_WARN, "lock-manager: RPC error: %s",
					rpcstatnames[(int)error]);
#endif
				/* on RPC error, wait a bit and try again */
				toid = timeout(wakeup, (caddr_t)&lm_sa,
					(normal_timeout * HZ));
				error = sleep((caddr_t)&lm_sa, sleep_disp);
				untimeout(toid);
				if (error) {
					error = EINTR;
					goto out;
				}
				goto remaploop;	/* ask for port# again */
	    
			} /*switch*/

		} /*for*/	/* loop until timeout, error, or completion */
	} /*for*/		/* loop until signal or completion */

out:
	CLNT_DESTROY(client);		/* drop the client handle */
	return(error);
}
E 1
