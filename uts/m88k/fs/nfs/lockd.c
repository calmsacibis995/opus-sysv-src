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
static char SysVr3NFSID[] = "@(#)lockd.c	4.15a LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Interface code for Network Lock Manager lockd device driver.
 */

/*
 * XXX - Remove when not debugging.
 *
#define KLM_DEBUG
#define DEBUG	1
 */
#ifdef KLM_DEBUG
#define static
extern int klm_debug;
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/mount.h"
#include "sys/signal.h"
#include "sys/dir.h"
#ifdef u3b2 
#include "sys/psw.h" 
#include "sys/pcb.h" 
#endif
#include "sys/user.h"
#include "sys/nami.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/debug.h"
#if !m88k
#include "sys/inline.h"
#endif
#include "sys/buf.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/cmn_err.h"
#include "sys/proc.h"
#include "sys/fcntl.h"
#include "sys/flock.h"


#include "sys/tiuser.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

#ifdef  TESTING
#include "lockmgr.h"
#include "lockd.h"
#else
#include "sys/fs/nfs/lockmgr.h"
#include "sys/fs/nfs/lockd.h"
#endif

/* 
 * Imported from fs/nfs/klm_flock.c
 */
extern int 	klm_getgranted();
extern int	klm_getsleeping();
extern int 	klm_flushlocks();
extern int	klm_shutdown();
extern int	lfs_reclock();
extern int	lfs_cleanlocks();

/* 
 * Imported from fs/nfs/kmem_alloc.c
 */
extern caddr_t	kmem_alloc();
extern int	kmem_free();

/*
 * Imported from os/flock.c
 */
extern int	reclock();
extern int	cleanlocks();

/*
 * KLM <-> lockd communication.
 */
lockd_conf_t	ldconf;		/* set configuration */
lockd_info_t	ldinfo;		/* get information */

/*
 * KLM/lockd statistics: nlminfo command.
 */
lockd_stat_t	ldstat;

/*
 * Lockd context information.
 */
static lockd_state_t	lockd_state;	/* kernel lockd state */
static int 		lockd_sleepers;	/* sleeping clients count */
static struct proc     *lockd_process;	/* lockd process */

int			lockd_patched;	/* The Crime */



/*
 * Switch from (to) System V.3 locking 
 * to (from) Network Lock Manager type.
 * 
 */
static void
lockd_intercept(yep)
	int yep;
{
	ASSERT(lockd_patched != yep);

	lockd_patched = yep;
	return;
}


/*
 * The system-wide "switching" reclock() interface.
 *
 * NOTE:  The os/flock.c entry point name MUST be edited
 *        to change reclock() to sys_reclock()!
 */
int
reclock(ip, lckdat, cmd, flag, offset)
	struct	inode  *ip;
	struct	flock *lckdat;
	int	cmd;
	int	flag;
	off_t	offset;
{
	if (lockd_patched) {
		return (lfs_reclock(ip, lckdat, cmd, flag, offset));
	} else {
		return (sys_reclock(ip, lckdat, cmd, flag, offset));
	}
	/* NOTREACHED */
}


/*
 * The system-wide "switching" cleanlocks() interface.
 *
 * NOTE:  The os/flock.c entry point name MUST be edited
 *        to change cleanlocks() to sys_cleanlocks()!
 */
int
cleanlocks(ip, flag)
	struct inode *ip;
	int	flag;
{
	if (lockd_patched) {
		(void) lfs_cleanlocks(ip, flag);
	} else {
		(void) sys_cleanlocks(ip, flag);
	}
	return (0);
}


/*
 * Change lockd state and wakeup anyone waiting on change.
 */
static void
lockd_transit(state)
	lockd_state_t state;
{
	ldstat.ls_lockd_state = lockd_state = state;
	if (lockd_sleepers > 0) {
		wakeup((caddr_t) &lockd_state);
	}
}


/*
 * Wait until lockd transits from state.
 */
static int
lockd_wait(state)
	lockd_state_t state;
{
	int interrupt = 0;

	while (! interrupt && (lockd_state == state)) {
		lockd_sleepers++;
		interrupt = sleep((caddr_t) &lockd_state, (PZERO+1)|PCATCH);
		--lockd_sleepers;
	}
	return (interrupt);
}


/*
 * XXX - Reclaim local locks in lockd.
 */
static int
lockd_reclaim()
{
	/* not implemented */
	return (0);
}


/*
 * XXX - Save local locks from lockd in kernel memory.
 */
/* ARGSUSED */
static int
lockd_save(argp, flag)
	caddr_t argp;
	int flag;
{
	/* not implemented */
	return (0);
}


/*
 * XXX - Restore local locks from kernel memory to reclock().
 */
static int
lockd_restore()
{
	/* not implemented */
	return (0);
}


/* ================  /dev/lockd  ================ */

/*
 * /dev/lockd init call.
 */
int
lckdinit()
{
	lockd_transit(lkd_closed);
	return (0);
}


/*
 * /dev/lockd open call.
 */
/* ARGSUSED */
int
lckdopen(dev)
	dev_t dev;
{
	int toid;
	int secs, s;

	if (! suser()) {
		return (u.u_error);
	}

	/*
	 * Hold off new lockd until recovery complete.
	 */
	if (lockd_wait(lkd_recover)) {
		return (u.u_error = EINTR);
	}

	/*
	 * Try to find a "hole" where the SysV reclock()
	 * machinery is quiescent: ie. no sleeping locks.
	 * 
	 * XXX - Remove granted locks check from loop
	 *       when reclaim implemented.  Reclaim will
	 *       move granted locks to Lock Manager.
	 */
	for (secs = 60; secs; --secs) {
		if ((klm_getgranted() == 0) && (klm_getsleeping() == 0)) {
			break;
		}
		s = splhi();
		toid = timeout(wakeup, (caddr_t) &lockd_state, HZ);
		sleep((caddr_t) &lockd_state, (PZERO+1)|PCATCH);
		untimeout(toid);
		splx(s);
	}
	if (secs == 0) {
		cmn_err(CE_NOTE, "/dev/lockd: open failed, sleeping locks.");
		return (u.u_error = EBUSY);
	}

	/* 
	 * Only one lockd will pass gate and start.
	 */
	if (lockd_state != lkd_closed) {
		return (u.u_error = EINVAL);
	}
	lockd_transit(lkd_opened);
	lockd_process = u.u_procp;

	/*
	 * Clear the lockd information structure.
	 *
	 * NOTE:
	 *	Asserts are bogus under certain conditions.
	 */
	/* ASSERT(ldstat.ls_busyoperations == 0);	*/
	/* ASSERT(ldstat.ls_lockhandles == 0);		*/
	/* ASSERT(ldstat.ls_activeprocs == 0);		*/
	bzero((caddr_t) &ldstat, sizeof(lockd_stat_t));

	/*
	 * Intercept reclock() and cleanlocks() calls,
	 * clients sleep until existing local locks have
	 * been uploaded and granted by lockd.
	 */
	lockd_intercept(1);

	return (u.u_error = 0);
}


/*
 * /dev/lockd ioctl call.
 */
/* ARGSUSED */
int
lckdioctl(dev, cmd, argp, flag)
	dev_t dev;
	int cmd;
	caddr_t argp;
	int flag;
{
	int error = 0;
	int cnt;

	switch (cmd) {

	case L_SETCONF: {
		caddr_t kptr;
		uint klen;

		/*
		 * Local NLM setting KLM configuration.
		 */
		if (lockd_state != lkd_opened) {
			return (u.u_error = EINVAL);
		}

		/*
		 * Copyin configuration structure.
		 *
		 * NOTE:  Transport identifier has two domains:
		 *
		 *  -1 => NFS 3.2.2 socket-based transport;
		 *   0 => not initialized:  Activate error;
		 *   + => NFS 3.2.4+ TPI transport endpoint.
		 */
		if (copyin(argp, (caddr_t) &ldconf, sizeof(lockd_conf_t))) {
			return (u.u_error = EFAULT);
		}

		/* 
		 * Copyin KLM->lockd transport address.
		 */
		klen = ldconf.lc_addrlen;
		if ((kptr = (char *) kmem_alloc(klen)) == NULL) {
			return (u.u_error = EFAULT);
		}
		if (copyin(ldconf.lc_addrbuf, kptr, klen)) {
			(void) kmem_free(kptr, klen);
			return (u.u_error = EFAULT);
		}
		ldconf.lc_addrbuf = kptr;

		/* 
		 * Copyin this machine's hostname.
		 * Do not assume null terminated.
		 */
		klen = ldconf.lc_hostlen + 1;
		if ((kptr = (char *) kmem_alloc(klen)) == NULL) {
			return (u.u_error = EFAULT);
		}
		if (copyin(ldconf.lc_hostbuf, kptr, ldconf.lc_hostlen)) {
			(void) kmem_free(kptr, klen);
			return (u.u_error = EFAULT);
		}
		kptr[ldconf.lc_hostlen] = NULL;
		ldconf.lc_hostbuf = kptr;
		ldconf.lc_hostlen = klen;

		break;
	}

	case L_GETINFO:	
		/* 
		 * Get KLM information for lockd.
		 */
		ldinfo.li_lockd_state = lockd_state;
		ldinfo.li_sys_granted = klm_getgranted();
		ldinfo.li_sys_sleeping = klm_getsleeping();
		ldinfo.li_klm_inodes = ldstat.ls_lockhandles;
		ldinfo.li_klm_procs = ldstat.ls_activeprocs;
		ldinfo.li_klm_busy = ldstat.ls_busyoperations;

		/*
		 * Copyout the KLM information.
		 */
		if (copyout((caddr_t) &ldinfo, argp, sizeof(lockd_info_t))) {
			return (u.u_error = EFAULT);
		}
		break;

	case L_RECLAIM:
		/*
		 * Optionally called by child of lockd,
		 * to provide kernel context for reclaims.
		 */
		if (lockd_state != lkd_opened) {
			return (u.u_error = EINVAL);
		}
		lockd_transit(lkd_reclaim);

		/*
		 * Reclaim existing local locks.
		 */
		error = lockd_reclaim();

		/*
		 * After reclaim, transit back to open.
		 */
		lockd_transit(lkd_opened);
		break;

	case L_RECOVER:
		/* 
		 * Optionally called by lockd multiple times.
		 */
		if (lockd_state == lkd_working) {
			lockd_transit(lkd_recover);
		} else if (lockd_state != lkd_recover) {
			return (u.u_error = EINVAL);
		}
		 
		/* 
		 * Save lockd state in temporary storage.
		 */
		error = lockd_save(argp, flag);
		break;

	case L_ACTIVATE:
		/*
		 * Insure that transport has been set.
		 */
		if (ldconf.lc_trans == 0) {
			cmn_err(CE_WARN, "/dev/lockd: transport not set.");
			return (u.u_error = EINVAL);
		}

		/*
		 * Retry to activate until reclaims done.
		 */
		if (lockd_state == lkd_reclaim) {
			return (u.u_error = EAGAIN);
		}

		/*
		 * Reclaim should remove local locks from inode list,
		 * we now remove the rest and flush their owners.
		 */
		if ((cnt = klm_flushlocks()) > 0) {
			cmn_err(CE_NOTE, "/dev/lockd: %d lock(s) not reclaimed.",
				cnt);
		}

		/*
		 * Lock Manager ready to receive requests,
		 * permit new local locks to go through.
		 */
		if (lockd_state != lkd_opened) {
			return (u.u_error = EINVAL);
		}
		lockd_transit(lkd_working);
		break;

	default:
		cmn_err(CE_NOTE, "/dev/lockd: unknown ioctl cmd.");
		return (u.u_error = EINVAL);
	}

	return (u.u_error = error);
}


/*
 * /dev/lockd close call.
 */
/* ARGSUSED */
int
lckdclose(dev)
	dev_t dev;
{
	int abting, dieing, cnt;

	/*
	 * Two boundary contitions.
	 */
	abting = (lockd_state == lkd_opened);
	dieing = (lockd_state == lkd_recover);

	/*
	 * If not recover or working, then abnormal close.
	 */
	if ((lockd_state != lkd_recover) && (lockd_state != lkd_working)) {
		cmn_err(CE_NOTE, "/dev/lockd: unexpected close.");
	}
	lockd_transit(lkd_recover);

	/*
	 * Save should remove active locking processes
	 * from inode list, we now disconnect the rest
	 * and signal each that locks have been lost.
	 */
	if ((! abting) && ((cnt = klm_shutdown()) > 0)) {
		cmn_err(CE_NOTE, "/dev/lockd: %d process(s) lost locks.", cnt);
	}

	/*
	 * Stop intercepting local locks.
	 */
	lockd_intercept(0);

	/* 
	 * If recover, then last call to savelock()
	 * has been made and we must recover local
	 * locks from kernel memory and grant.
	 */
	if ((! abting) && dieing && ((cnt = lockd_restore()) > 0)) {
		cmn_err(CE_NOTE, "/dev/lockd: %d lock(s) not recovered.", cnt);
	}

	/*
	 * Now deallocate space and close the lockd.
	 */
	(void) kmem_free(ldconf.lc_addrbuf, ldconf.lc_addrlen);
	(void) kmem_free(ldconf.lc_hostbuf, ldconf.lc_hostlen);
	bzero((caddr_t) &ldconf, sizeof(lockd_conf_t));
	lockd_process = (struct proc *) NULL;

	lockd_transit(lkd_closed);

	return (0);
}


/*
 * Lockd state validation machine.
 *
 * Checks and continues/redirects/blocks/awakens
 * lock clients during lockd state transitions.
 */
int
lockd_check()
{
	/*
	 * Do forever or until lockd state is working or error.
	 * 
	 * Return values:
	 *	  (0)    - Continue operation;
	 * 	 EINTR   - Interrupted by client;
	 *      ENOLINK  - Network Lock Manager down.
	 *
	 * NOTE:  ENOLINK is a special error return,
	 *	  donoting that the lockd has died, the
	 *        reclock() interfaces are re-patched, and
	 *	  that the caller can be redirected.
	 */
	do switch ((int) lockd_state) {

	case lkd_loaded:  /* lockd:  ~init, ~open, ~intercept, ~locking */
		/*
		 * PANIC:  Something is radically wrong.
		 */
		cmn_err(CE_PANIC, "/dev/lockd: loaded state, panic.");
		
	case lkd_closed:  /* lockd:  init, ~open, ~intercept, ~locking */
		/*
		 * Request intercepted but lockd closed due to internal
		 * processing latency, redirecting to restored interfaces.
		 *
		 * NOTE:  Return ENOLINK to denote lock manager down.
		 */
		ASSERT(lockd_patched == 0);
		return (ENOLINK);

	case lkd_opened:  /* lockd: init, open, intercept, ~locking
		/* 
		 * Delay incoming requests until lockd working.
		 */
		/* FALLTHROUGH */

	case lkd_reclaim:  /* lockd: init, open, intercept, reclaim */
		/* 
		 * Delay new local lock requests until reclaim complete.
		 */
		/* FALLTHROUGH */

	case lkd_recover:  /* lockd: init, open, intercept, recover */
		/* 
		 * Delay new local lock requests until recovery complete.
		 */
		if (lockd_wait(lockd_state)) {
			return (EINTR);
		}
		break;

	case lkd_working:  /* lockd:  init, open, intercept, locking */
		/* 
		 * Ok, allow operation to continue.
		 */
		return(0);

	default:
		cmn_err(CE_PANIC, "/dev/lockd: invalid state, panic.");

	} while (1);		/* forever */

	/* NOTREACHED */
}


/*
 * If KLM should become insane or inoperative,
 * this routine will force recovery by killing
 * the user-level lock manager.
 */
void
lockd_abort()
{
	/*
	 * Send SIGTERM to lock manager.
	 */
	if ((lockd_state != lkd_closed) && 
	    (lockd_process != (struct proc *) NULL)) {
		cmn_err(CE_NOTE, "/dev/lockd: KLM insane, SIGTERM lockd.");
		psignal(lockd_process, SIGTERM);
	}
	return;
}
