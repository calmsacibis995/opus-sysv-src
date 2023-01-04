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
static char SysVr3NFSID[] = "@(#)klm_lckmgr.c	4.11 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Kernel<->Network Lock-Manager Interface
 *
 * File- and Record-locking requests are forwarded (via RPC) to a
 * Network Lock-Manager running on the local machine.  The protocol
 * for these transactions is defined in /usr/src/protocols/klm_prot.x
 */

/*
 * XXX - Remove when not debugging.
 *
#define DEBUG	1
#define KLM_DEBUG
 */
#ifdef KLM_DEBUG
#define static
extern int klm_debug;
#endif

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
#if i386 || m88k || ns32000 || clipper
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

#include "sys/stream.h"
#include "sys/tiuser.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"
#include "sys/fs/nfs/xdr.h"

#ifdef  TESTING
#include "lockd.h"
#include "lockmgr.h"
#include "klm_prot.h"
#else
#include "sys/fs/nfs/lockd.h"
#include "sys/fs/nfs/lockmgr.h"
#include "sys/fs/nfs/klm_prot.h"
#endif

#include "sys/fs/nfs/trans.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/auth_unix.h"
#include "sys/fs/nfs/clnt.h"

/*
 * Imported from fs/nfs/lockd.c
 */
extern int		lockd_check();
extern void		lockd_abort();
extern lockd_conf_t	ldconf;
extern lockd_stat_t	ldstat;

/* 
 * Define static parameters for run-time tuning
 */
static int backoff_timeout = 1;		/* wait on klm_denied_nolocks */
static int normal_retry = 1;		/* attempts before klm_working */
static int normal_timeout = 1;
static int working_retry = 0;		/* attempts after klm_working */
static int working_timeout = 2;

/*
 * We must persist for some calls to lock manager,
 * thus we define interruptable and not levels.
 */
#define SLEEP_PCATCH	((PZERO + 1) | PCATCH)
#define SLEEP_NOINTR	(PZERO)

extern int	 sleep();
extern int	 wakeup();

#define clntNULL	((CLIENT *) 0)		/* useful */


/*
 * KLM socket-based client handle cache management.
 */
#define KLM_MAXCLIENTS	4

static struct klm_chtab {
	bool_t	ch_inuse;
	CLIENT	*ch_client;
} klm_chtable[KLM_MAXCLIENTS];

static int klm_chtabavail = KLM_MAXCLIENTS;
static int klm_clwanted = 0;

/*
 * Maintain a cache of socket-based KLM client handles.
 */
static CLIENT *
klm_clget(try, pri, cred)
	int try, pri;
	struct ucred *cred;
{
	register struct klm_chtab *ch;
	/* externs */
	extern CLIENT *clntkudp_create();
	extern void clntkudp_init();
	extern void clntkudp_init2();

	/*
	 * Insure we are running socket-based transport interface.
	 */
	if (ldconf.lc_trans != -1) {
		cmn_err(CE_WARN, "KLM transport incompatible with lockd.");
		lockd_abort();
		return (clntNULL);
	}

	while (klm_chtabavail == 0) {
		klm_clwanted++;
		sleep((caddr_t) &klm_chtabavail, PRIBIO);
		--klm_clwanted;
	}
	--klm_chtabavail;

	for (ch = klm_chtable; ch < &klm_chtable[KLM_MAXCLIENTS]; ch++) {
		if (ch->ch_inuse == FALSE) {
			ch->ch_inuse = TRUE;
			if (ch->ch_client == clntNULL) {
				ch->ch_client = clntkudp_create(
					ldconf.lc_addrbuf, ldconf.lc_addrlen, 
					KLM_PROG, KLM_VERS, try, cred);
			} else {
				clntkudp_init(ch->ch_client, ldconf.lc_addrbuf,
					ldconf.lc_addrlen, try, cred);
			}
			clntkudp_init2(ch->ch_client, ! (pri & PCATCH));
			return (ch->ch_client);
		}
	}
	return (clntNULL);
}


/*
 * Socket-based client handle release.
 */
static void
klm_clfree(client)
	CLIENT *client;
{
	register struct klm_chtab *ch;

	for (ch = klm_chtable; ch < &klm_chtable[KLM_MAXCLIENTS]; ch++) {
		if (ch->ch_client == client) {
			ch->ch_inuse = FALSE;
			break;
		}
	}

	klm_chtabavail++;
	if (klm_clwanted > 0) {
		wakeup((caddr_t) &klm_chtabavail);
	}
	return;
}


/*
 * KLM rpc call names and results for printf().
 */
static char *klmnames[] = {
	"null", "test", "lock", "cancel", "unlock"
};
static char *klmstatnames[] = {
	"granted", "denied", "denied, no locks", "working",
	"interrupted", "denied, no link", "timed out"
};
extern char	*rpcstatnames[];


/*
 * Print KLM message to console.
 */
void
klm_message(which, result, status)
	ulong which;
	enum klm_stats result;
	enum clnt_stat status;
{
	cmn_err(CE_NOTE, "KLM %s to Lock Manager: %s: %s.",
		klmnames[which], klmstatnames[(int) result],
		rpcstatnames[(int) status]);
	return;
}


/*
 * Call the local Network Lock Manager (NLM) with the given request.
 *
 * NOTE:  The only permissible return values are:
 *
 *   	klm_granted | klm_working | klm_denied | klm_denied_nolocks |
 *
 *	klm_interrupted | klm_denied_nolink | klm_timedout
 *
 * WARNING:  The denied_nolink result is special; it denotes that
 *           the lock daemon has died and the caller should unwind
 *	     the call chain and redirect to the restored reclock()
 *           interfaces... can only be returned after lockd_check()!
 */
static klm_stats
nlmcall(which, xdrargs, args, xdrreply, reply, try, pri, tim, cred)
	ulong which;
	xdrproc_t xdrargs;
	klm_lockargs *args;
	xdrproc_t xdrreply;
	klm_testrply *reply;
	struct ucred *cred;
	int pri, try, tim;
{
	CLIENT *client;
	enum clnt_stat status;
	enum klm_stats result;
	/* structures */
	struct timeval tmo;

	tmo.tv_sec = tim;
	tmo.tv_usec = 0;

	/*
	 * Check if lock manager working.
	 */
	switch (lockd_check()) {
	
	case EINTR:	
		return (klm_interrupted);

	case ENOLINK:
		return (klm_denied_nolink);
	}

	/*
	 * Get client handle, may sleep if busy.
	 */
	if ((client = klm_clget(try, pri, cred)) == (CLIENT *) NULL) {
		return (klm_denied_nolocks);
	}
	ldstat.ls_nlm_calls++;

	/*
	 * Have kernel rpc send the request, wait on reply.
	 */
	status = CLNT_CALL(client, which, xdrargs, (caddr_t) args,
			   xdrreply, (caddr_t) reply, tmo);

	klm_clfree(client);

	switch (status) {

	case RPC_SUCCESS:
		/*
		 * Call OK, return KLM status.
		 */
		return (reply->stat);

	case RPC_TIMEDOUT:
		/*
		 * Normal retry from caller.
		 */
		return (klm_timedout);
	
	case RPC_INTERRUPTED:
		/*
		 * Should not receive for dontint...
		 */
		return (klm_interrupted);

	case RPC_AUTHERROR:
	case RPC_CANTENCODEARGS:
	case RPC_CANTDECODERES:
	case RPC_VERSMISMATCH:
	case RPC_PROGVERSMISMATCH:
	case RPC_CANTDECODEARGS:
		/*
		 * Unrecoverable errors: 
		 *
		 * Kill lockd and return to wait.
		 */
		lockd_abort();
		/* FALLTHROUGH */

	default:
		/* 
		 * Recoverable errors:
		 *
		 * Print console message.
		 */
		result = klm_denied_nolocks;
		klm_message(which, result, status);
		return (result);

	}  /* switch status */

	/* NOTREACHED */
}


/*
 * Backoff and sleep for awhile... on the unique lock handle.
 */
static int
klm_wait(lhp, tout)
	lockhandle_t *lhp;
	int tout;
{
	int intr;
	int toid;
	int s;

	s = splhi();
	toid = timeout(wakeup, (caddr_t) lhp, (tout * HZ));
	intr = sleep((caddr_t) lhp, SLEEP_PCATCH);
	untimeout(toid);
	splx(s);
	return (intr);
}


/*
 * Process a lock, unlock, test request.
 *
 * Calls (via RPC) the local lock manager to register the request.
 * Lock requests are cancelled if interrupted by signals.
 */
int
klm_lockctl(lhp, app)
	lockhandle_t *lhp;
	activeproc_t *app;
{
	struct flock *ldp = app->ap_lockp;
	struct ucred *cred = &app->ap_cred;
	int cmd = app->ap_cmd;
	int pri, try, tim;
	int true = 1;
	int error;
	ulong which;
	xdrproc_t xdrargs;
	xdrproc_t xdrreply;
	klm_stats result;
	/* structures */
	klm_lockargs args;
	klm_testrply reply;

	which = KLM_LOCK;
	xdrargs = (xdrproc_t) xdr_klm_lockargs;
	xdrreply = (xdrproc_t) xdr_klm_stat;
	args.block = FALSE;
	args.exclusive = FALSE;
	args.lock.fh.n_bytes = (char *) &lhp->lh_file;
	args.lock.fh.n_len = sizeof(lhp->lh_file);
	args.lock.server_name = lhp->lh_server;
	args.lock.pid = app->ap_oid;
	args.lock.l_offset = ldp->l_start;
	args.lock.l_len = ldp->l_len;

	/* 
	 * Now modify the lock argument
	 * structure for specific cases.
	 */
	switch (ldp->l_type) {

	case F_WRLCK:
		args.exclusive = TRUE;
		break;

	case F_UNLCK:
		which = KLM_UNLOCK;
		xdrargs = (xdrproc_t) xdr_klm_unlockargs;
		break;
	}

	switch (cmd) {

	case F_SETLKW:
		args.block = TRUE;
		break;

	case F_GETLK:
		which = KLM_TEST;
		xdrargs = (xdrproc_t) xdr_klm_testargs;
		xdrreply = (xdrproc_t) xdr_klm_testrply;
		break;
	}
	ASSERT(which < KLM_PROCS_CNT);
	ldstat.ls_klm_procs[which]++;

	/*
	 * Variables pri, try, and tim may change within retry loop
	 * when going non-interruptable or backing off for retry.
	 */
	try = normal_retry;
	pri = SLEEP_PCATCH;
	tim = normal_timeout; 

	while (true) {		/* RETRY */

		/* 
		 * Send the request out to the local lock-manager.
		 */
		result = nlmcall(which, xdrargs, &args, xdrreply, &reply,
				 try, pri, tim, cred);

		ASSERT((int) result < KLM_STATS_CNT);
		ldstat.ls_klm_stats[(int) result]++;

		if (result == klm_denied_nolink) {
			/*
			 * Lock manager died; special return...
			 * should only result from lockd_check().
			 */
			error = ENOLINK;
			goto done;
		}

		switch (which) {
	
		/*
		 * Set a lock.
		 *
		 * NOTE:  Uninterruptable after working.
		 */
		case KLM_LOCK:
			switch (result) {
	
			case klm_granted:
				error = 0;
				goto done;
	
			case klm_denied:
				if (args.block) {
					/* lockd should never denied here */
					error = ENOLCK;
				} else {
					/* caller converts EAGAIN->EACCESS */
					error = EAGAIN;
				}
				goto done;
	
			case klm_timedout:
			case klm_denied_nolocks:
				if (klm_wait(lhp, backoff_timeout)) {
					if (pri & PCATCH) {
						error = EINTR;
						goto done;
					}
				}
				break;
	
			case klm_working:
				try = working_retry;
				tim = working_timeout;
				if (klm_wait(lhp, tim)) {
					which = KLM_CANCEL;
					pri = SLEEP_NOINTR;
				}
				break;
	
			case klm_interrupted:
				/*
				 * Can't blindly abandon operation -
				 * have to clean up resources on lockmgr,
				 * ie., ignore interrupts now.
				 */
				which = KLM_CANCEL;
				pri = SLEEP_NOINTR;
				break;
	
			}  /* switch lock result */
			break;
	
		/*
		 * Unlock a lock.
		 *
		 * NOTE:  Uninterruptable after commit.
		 */
		case KLM_UNLOCK:
			switch (result) {
	
			case klm_granted:
				error = 0;
				goto done;
	
			case klm_denied:
				cmn_err(CE_WARN, "KLM %s %s.",
					klmnames[(int) which],
					klmstatnames[(int) result]);
				error = ENOLCK;
				goto done;
	
			case klm_timedout:
			case klm_denied_nolocks:
				if (klm_wait(lhp, backoff_timeout)) {
					if (pri & PCATCH) {
						error = EINTR;
						goto done;
					}
				}
				break;
	
			case klm_working:
				try = working_retry;
				tim = working_timeout;
				if (klm_wait(lhp, tim)) {
					pri = SLEEP_NOINTR;
				}
				break;
	
			case klm_interrupted:
				/*
				 * Can't blindly abandon operation -
				 * have to clean up resources on lockmgr,
				*  ie., ignore interrupts now.
				 */
				pri = SLEEP_NOINTR;
				break;
	
			}  /* switch unlock result */
			break;
	
		/*
		 * Test a lock.
		 * 
		 * NOTE:  Interruptable at any point.
		 */
		case KLM_TEST:
			switch (result) {
	
			case klm_granted:
				/* 
				 * Unlocked, mark lock available.
				 */
				ldp->l_type = F_UNLCK;
				error = 0;
				goto done;
	
			case klm_denied: {
				klm_holder *hp;
				lockowner_t oid;

				/*
				 * Return lock holder.
				 */
				hp = &reply.klm_testrply.holder;
				ldp->l_type = (hp->exclusive)? F_WRLCK: F_RDLCK;
				ldp->l_start = hp->l_offset;
				ldp->l_len = hp->l_len;
				/* int -> shorts */
				oid.lo_oid = hp->svid;
				ldp->l_pid = oid.lo_pid;
				ldp->l_sysid = oid.lo_sysid;
				error = 0;
				goto done;
			}
	
			case klm_timedout:
			case klm_denied_nolocks:
				if (klm_wait(lhp, backoff_timeout)) {
					error = EINTR;
					goto done;
				}
				break;
	
			case klm_working:
				try = working_retry;
				tim = working_timeout;
				if (klm_wait(lhp, tim)) {
					error = EINTR;
					goto done;
				}
				break;
	
			case klm_interrupted:
				/*
				 * Bail out.
				 */
				error = EINTR;
				goto done;
	
			}  /* switch test result */
			break;
	
		/*
		 * Cancel a previous lock operation; ie. an
		 * interrupt occurred and the lock manager
		 * resources must be cleared.
		 * 
		 * NOTE:  Uninterruptable at any point.
		 */
		case KLM_CANCEL:
			switch (result) {
	
			case klm_denied:
				/* FALLTHROUGH */
	
			case klm_granted:
				/*
				 * Cancelled due to interrupt.
				 */
				error = EINTR;
				goto done;
	
			case klm_timedout:
			case klm_denied_nolocks:
				/* 
				 * Backoff...
				 */
				(void) klm_wait(lhp, backoff_timeout);
				break;
	
			case klm_working:
				/* 
				 * Be patient...
				 */
				try = working_retry;
				tim = working_timeout;
				(void) klm_wait(lhp, tim);
				break;
	
			case klm_interrupted:
				/*
				 * Should not get... persist!
				 */
				break;	
	
			}  /* switch cancel result */
			break;
	
		default:
			cmn_err(CE_PANIC, "klm_lockctl: unknown procedure.");
	
		}  /* switch request */

	}  /* while true */

	/* NOTREACHED */
done:
	if (error) {
		ldstat.ls_klm_errors++;
	}
	return (error);
}
