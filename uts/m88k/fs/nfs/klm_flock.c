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
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*								*/
/*	#ident	"@(#)kern-port:os/flock.c	10.8"		*/

#ifndef lint
static char SysVr3NFSID[] = "@(#)klm_flock.c	4.13 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Lock Manager alternate interface for file and record locking.
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
#include "sys/var.h"

#include "sys/tiuser.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/time.h"
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
 * Imported from other KLM machinery.
 */
extern int		klm_lockctl();
extern int 		lockd_check();
extern int 		lockd_patched;
extern lockd_conf_t	ldconf;
extern lockd_stat_t	ldstat;

/*
 * Imported from NFS machinery.
 */
extern caddr_t	kmem_alloc();
extern int	kmem_free();
extern int	nfs_flush();
extern int	nfs_inval();
extern int	set_cred();

/*
 * Major imports from System V, Release 3.
 *
 * NOTE:  We assume these interfaces are stable.
 */
extern int	prele();
extern int	plock();
extern int	psignal();
extern int	reclock();
extern int	cleanlocks();


/*
 * Allocate new per operation lock handle.
 */
static lockhandle_t *
alloc_lockhandle()
{
	ldstat.ls_lockhandles++;
	return((lockhandle_t *) kmem_alloc((uint) sizeof(lockhandle_t)));
}


/*
 * Free used per operation lock handle.
 */
static void
free_lockhandle(lhp)
	register lockhandle_t *lhp;
{
	(void) kmem_free((caddr_t) lhp, (uint) sizeof(lockhandle_t));
	--ldstat.ls_lockhandles;
	return;
}


/*
 * Allocate new per active locking process structure.
 */
static activeproc_t *
alloc_activeproc()
{
	ldstat.ls_activeprocs++;
	return((activeproc_t *) kmem_alloc((uint) sizeof(activeproc_t)));
}


/*
 * Free used per active locking process structure.
 */
static void
free_activeproc(app)
	register activeproc_t *app;
{
	(void) kmem_free((caddr_t) app, (uint) sizeof(activeproc_t));
	--ldstat.ls_activeprocs;
	return;
}


/*
 * Search and count reclock() generated granted locks.
 */
int
klm_getgranted()
{
	register struct inode *ip;
	register struct filock *flp;
	register lockhandle_t *lhp;
	int granted = 0;

	ip = &inode[0];
	while (ip < (struct inode *) v.ve_inode) {
		lhp = (lockhandle_t *) ip->i_filocks;
		if ((lhp != lhpNULL) && (lhp->lh_magic != KLM_LOCK_MAGIC)) {
			flp = (struct filock *) ip->i_filocks;
			while (flp) {
				granted++;
				flp = flp->next;
			}
		}
		ip++;
	}
	return(granted);
}


/*
 * Search and count reclock() generated sleeping locks.
 * Called from lockd to by ioctl().
 */
int
klm_getsleeping()
{
	register struct filock *flp;
	int sleeping = 0;
	/* externs */
	extern struct filock *sleeplcks;

	for (flp = sleeplcks; flp; flp = flp->next) {
		sleeping++;
	}
	return(sleeping);
}


/*
 * Flush remaining reclock() locks from inode lock lists.
 * Called by lockd when changing from state reclaim. 
 *
 * XXX - Recovery should transfer to lock manager...
 *       but until, we must not start /etc/lockd with
 *       outstanding granted or sleeping locks, else
 *       we *LOSE* the resource and sleeping locks
 *       *SLEEP FOREVER*.  For now, caller does
 *       best effort by printing WARNING to
 *       console.
 */
int
klm_flushlocks()
{
	register struct inode *ip;
	register struct filock *flp;
	extern struct filock *sleeplcks;
	int flushed = 0;

	for (ip = &inode[0]; ip < (struct inode *) v.ve_inode; ip++) {
		flp = (struct filock *) ip->i_filocks;
		while (flp) {
			/* XXX */
			flushed++;
			flp = flp->next;
		}
		ip->i_filocks = (long *) NULL;
	}
	for (flp = sleeplcks; flp; flp = flp->next) {
		/* XXX */
		flushed++;
	}
	sleeplcks = (struct filock *) NULL;
	return (flushed);
}


/*
 * Convert lock's [start, whence] to given whence.
 * 
 * NOTE:  We use the inode size at time of lock
 *	  in order to minimize effect of network.
 */
static int
klm_convoff(ldp, whence, offset, isize)
	struct flock *ldp;
	int whence;
	off_t offset;
	off_t isize;
{
	if (ldp->l_whence == 1)
		ldp->l_start += offset;
	else if (ldp->l_whence == 2)
		ldp->l_start += isize;
	else if (ldp->l_whence != 0)
		return(EINVAL);
	if (ldp->l_start < 0)
		return(EINVAL);
	if (whence == 1)
		ldp->l_start -= offset;
	else if (whence == 2)
		ldp->l_start -= isize;
	else if (whence != 0)
		return(EINVAL);
	ldp->l_whence = whence;

	return(0);
}


/*
 * Since the KLM/NLM protocols do not support
 * getting AND waiting for a lock to free, we 
 * emulate via a test/lock/wait sequence.
 */
static int
klm_getlkw(lhp, app)
	lockhandle_t *lhp;
	activeproc_t *app;
{
	register struct flock *ldp = app->ap_lockp;
	int error = 0;
	/* structures */
	struct flock savld;

	ASSERT(app->ap_flags & AP_GETLKW);

	/*
	 * Save lock data.
	 */
	savld = *ldp;

	/*
	 * First, we try the F_GETLK.
	 * If unlocked, we lucked out...
	 */
	if ((error = klm_lockctl(lhp, app)) ||
	    (ldp->l_type == F_UNLCK)) {
		return (error);
	}

	/*
	 * ...else, we have to go the long way.
	 *
	 * NOTE:  We have a race condition here,
	 *        the inode must be unlocked and
	 *        the call made before others
	 *        get to run...
	 */
	if (app->ap_flags & AP_PLOCK) {
		prele(lhp->lh_ip);
	}

	/*
	 * Restore lock data, change operation
	 * to F_SETLKW, and retry request.
	 */
	*ldp = savld;
	app->ap_cmd = F_SETLKW;
	error = klm_lockctl(lhp, app);

	/*
	 * Relock inode before unlocking file.
	 */
	if (app->ap_flags & AP_PLOCK) {
		plock(lhp->lh_ip);
	}
	app->ap_flags &= ~AP_PLOCK;

	/*
	 * If not error, then unlock the file.
	 */
	if (error == 0) {
		app->ap_cmd = F_SETLK;
		ldp->l_type = F_UNLCK;
		error = klm_lockctl(lhp, app);
	}

	return (error);
}


/*
 * File system independent KLM record locking interface.
 */
int
klm_reclock(lhp, app, cmd, flag, offset)
	lockhandle_t *lhp;
	activeproc_t *app;
	int cmd, flag;
	off_t offset;
{
	struct flock *ldp = app->ap_lockp;
	struct inode *ip = lhp->lh_ip;
	long lck_end;
	short whence;
	int error;

#ifdef KLM_DEBUG
	if (klm_debug > 1) {
		printf("klm_reclock(): called, cmd 0x%x, type %d\n", 
			cmd, ldp->l_type); 
	}
#endif
	/* 
	 * Check access permissions.
	 */
	if ((cmd & SETFLCK) &&
	    ((ldp->l_type == F_RDLCK && (flag & FREAD) == 0) ||
	     (ldp->l_type == F_WRLCK && (flag & FWRITE) == 0))) {
		error = EBADF;
		goto done;
	}
	
	/* 
	 * Normalize l_start to beginning of file.
	 * 
	 * NOTE:  We return per reclock() for some
	 *	  errors without un-normalizing.
	 */
	whence = ldp->l_whence;
	if (error = klm_convoff(ldp, 0, offset, ip->i_size)) {
		goto done;
	}

	/*
	 * Check lock range for SVR3 specific boundary conditions.
	 * 
	 * NOTE: KLM expects lock to be in [start, length] format,
	 *       with length non-negative.  NLM defined MAXLEN and
	 *	 SVR3 defined MAXEOF are equivalent at this point.
	 */
	if (ldp->l_len < 0) {
		error = EINVAL;
		goto done;
	}
	if (ldp->l_len > 0) {
		lck_end = (ldp->l_start + ldp->l_len) - 1;
	} else {
		lck_end = MAXEND;  /* 0 => eof */
	}
	/* check for arithmetic overflow */
	if (ldp->l_start > lck_end) {
		error = EINVAL;
		goto done;
	}

	/*
	 * Call the protocol dependent KLM interface.
	 */
	if (app->ap_flags & AP_GETLKW) {
		/*
		 * If emulating an "F_GETLKW", then
		 * do test/sleep-lock/unlock sequence.
		 */ 
		error = klm_getlkw(lhp, app);

	} else if ((app->ap_cmd == F_SETLKW) &&
		   (app->ap_flags & AP_PLOCK)) {
		/*
		 * If blocking operation and inode locked,
		 * then release, do request, and relock...
		 */
		prele(ip);
		error = klm_lockctl(lhp, app);
		plock(ip);

	} else {
		/*
		 * ...else, simple non-blocking.
		 */
		error = klm_lockctl(lhp, app);
	}
	app->ap_flags &= ~(AP_PLOCK | AP_GETLKW);

	/*
	 * Convert l_start to original whence.
	 */
	lck_end = (ldp->l_start + ldp->l_len) - 1;
	/* check for arithmetic overflow */
	if (lck_end < 0) {
		ldp->l_len = 0;
	}
	(void) klm_convoff(ldp, whence, offset, ip->i_size);
done:
#ifdef KLM_DEBUG
	if ((klm_debug > 1) || (klm_debug && error)) {
		printf("klm_reclock(): returning, cmd 0x%x, error %d\n",
			cmd, error); 
	}
#endif
	return (error);
}


/*
 * Release active locking process.
 */
static void
klm_relactive(lhp, old)
	lockhandle_t *lhp;
	activeproc_t *old;
{
	register activeproc_t *app;	
	register activeproc_t *next;	
	register activeproc_t *prev = appNULL;	
	
	next = lhp->lh_procs;
	while (app = next) {	
	        next = app->ap_next;
		if (app->ap_oid == old->ap_oid) {
			if (prev == appNULL) {
				lhp->lh_procs = app->ap_next;
			} else {
				prev->ap_next = app->ap_next;
			}
			app->ap_next = appNULL;
			free_activeproc(app);
		} else {
			prev = app;
		}
	}
	return;
}


/*
 * Get the active process and setup parameters for operation.
 *
 * 	cmd & SETFLCK indicates setting a lock.
 * 	cmd & SLPFLCK indicates wait if blocking lock.
 * 	cmd & INOFLCK indicates the associated inode is locked.
 */
static activeproc_t *
klm_getactive(lhp, ldp, cmd, errp)
	register lockhandle_t *lhp;
	struct flock *ldp;
	int cmd;
	int *errp;
{
	register activeproc_t *app;

	/*
	 * If not already on active process list,
	 * register new active locking process.
	 */
	for (app = lhp->lh_procs; app; app = app->ap_next) {
		if ((app->ap_pid == u.u_procp->p_epid) && 
		    (app->ap_sysid == u.u_procp->p_sysid)) {
			ASSERT((app->ap_flags & AP_BUSY) == 0);
			break;
		}
	}
	if (app == appNULL) {
		if ((app = alloc_activeproc()) == appNULL) {
			*errp = ENOLCK;
			return (app);
		}
		app->ap_flags = 0;
		app->ap_pid = u.u_procp->p_epid;
		app->ap_sysid = u.u_procp->p_sysid;
		app->ap_pgrp = u.u_procp->p_pgrp;
		app->ap_next = lhp->lh_procs;
		lhp->lh_procs = app;
	}
	app->ap_lockp = ldp;
	set_cred(&app->ap_cred);
	app->ap_flags |= AP_BUSY;
	ldstat.ls_busyoperations++;

	/*
	 * The reclock() interface allows the caller to
	 * block waiting on a F_GETLK of a blocked lock,
	 * ie. the missing F_GETLKW of the NLM protocol.
	 * Local mandatory locking uses to block on a 
	 * read or write while not having a residual 
	 * lock to cleanup.  We emulate...
 	 */
	if (cmd & SETFLCK) {
		if (cmd & SLPFLCK) {
			app->ap_cmd = F_SETLKW;
		} else {
			app->ap_cmd = F_SETLK;
		}
	} else {
		if (cmd & SLPFLCK) {
			app->ap_flags |= AP_GETLKW;
		}
		app->ap_cmd = F_GETLK;
	}
	
	/*
	 * The reclock() interface allows the caller
	 * to pass in a locked inode (mandatory locking).
	 * We recognize and release the inode prior to
	 * any operation that might block on a lock.
	 */
	if (cmd & INOFLCK) {
		app->ap_flags |= AP_PLOCK;
	}

	*errp = 0;
	return (app);
}


/*
 * Initialize a local filesystem independent lock handle.
 */
static int
klm_initlocal(lhp)
	lockhandle_t *lhp;
{
	int error;

	ASSERT(lhp->lh_ip);

	/*
	 * The locking client acts as the file server
	 * producing a fhandle for the lockhandle.
	 *
	 * NOTE:  Using makefh() here for local file
	 *	  systems is the only known local file
	 *	  system dependency.  It is necessary
	 *	  since local and remote fh's must 
	 *	  compare equal upon lock server.
	 */
	error = makefh(&lhp->lh_file, lhp->lh_ip);
	lhp->lh_server = ldconf.lc_hostbuf;

	ldstat.ls_local_fsops++;
	return (error);
}


/*
 * Initialize a remote lock handle.
 */
static int
klm_initremote(lhp)
	register lockhandle_t *lhp;
{
	struct inode *ip;
	struct rnode *rp;

	ip = lhp->lh_ip;
	ASSERT(ip);

	rp = itor(ip);
	ASSERT(rp);

	/*
	 * If not already synchronous, then sync dirty
	 * write blocks and mark the rnode NOCACHE.  This
	 * insures the buffer cache will not give inconsistent
	 * results on locked files shared between clients.
	 */
	 if ((rp->r_flags & RNOCACHE) == 0) {
		/* 
		 * NOTE:  KLM side effect.
		 */
		rp->r_flags |= RNOCACHE; 
		if (rp->r_flags & RDIRTY) {
			nfs_flush(ip);
		}
		nfs_inval(ip);
	}
	lhp->lh_flags |= LH_NOCACHE;

	/* 
	 * Copy the file server created fhandle
	 * from the rnode and pass in the lockhandle
	 * back to the file server's lock manager.
	 */
	bcopy((caddr_t) rtofh(rp), (caddr_t) &lhp->lh_file,
	      sizeof(fhandle_t));
	lhp->lh_server = &((itomi(ip))->mi_hostname[0]);
	
	ldstat.ls_remote_fsops++;
	return (0);
}


/*
 * Release the lock handle.
 */
static void
klm_relhandle(lhp, app)
	register lockhandle_t *lhp;
	register activeproc_t *app;
{
	struct inode *ip = lhp->lh_ip;

	ASSERT(ip);

	/*
	 * If passed a current active locking process, 
	 * then turn off busy: if shutting down the KLM,
	 * then free the active process structure.
	 */
	if (app != appNULL) {
		app->ap_flags &= ~AP_BUSY;
		--ldstat.ls_busyoperations;
		if (lhp->lh_flags & LH_FLUSH) {
			klm_relactive(lhp, app);
		}
	}

	/*
	 * If the lock handle has any remaining
	 * active locking processes, then return.
	 */
	if (lhp->lh_procs != appNULL) {
		return;
	}

	/* 
	 * If we have no active locking processes, then
	 * we reenable remote file caching, disconnect
	 * from the inode, and free the handle.
	 */
	if (lhp->lh_flags & LH_NOCACHE) {
		struct rnode *rp = itor(ip);

		ASSERT(rp);
		rp->r_flags &= ~RNOCACHE;
		lhp->lh_flags &= ~LH_NOCACHE;
	}

	/*
	 * If shutting down KLM, the lock handle
	 * is already disconnected from inode.
	 */
	if ((lhp->lh_flags & LH_FLUSH) == 0) {
		ip->i_filocks = (long *) NULL;
	}
	free_lockhandle(lhp);
	return;
}


/*
 * Get a lock handle for use by the caller.
 */
static lockhandle_t *
klm_gethandle(ip, initfun, errp)
	struct inode *ip;
	int (*initfun) ();
	int *errp;
{
	register lockhandle_t *lhp;

	/*
	 * If this inode has no active locking processes,
	 * then allocate a new lock handle and initialize.
	 */
	if ((lhp = (lockhandle_t *) ip->i_filocks) == lhpNULL) {
		if ((lhp = alloc_lockhandle()) == lhpNULL) {
			*errp = ENOLCK;
			goto done;
		}
		bzero((caddr_t) lhp, sizeof(lockhandle_t));
		lhp->lh_magic = KLM_LOCK_MAGIC;
		lhp->lh_ip = ip;

		/*
		 * Init for local or remote file systems.
		 */
		if (*errp = initfun(lhp)) {
			free_lockhandle(lhp);
			lhp = lhpNULL;
			goto done;
		}

		/*
		 * Connect to inode file locks list.
		 */
		ip->i_filocks = (long *) lhp;
			
	} else {
		ASSERT(lhp->lh_magic == KLM_LOCK_MAGIC);
		ASSERT(lhp->lh_ip == ip);
		*errp = 0;
	}
done:
#ifdef KLM_DEBUG
	if (klm_debug && (*errp)) {
		printf("klm_gethandle(): error %d\n", *errp); 
	}
#endif
	return (lhp);
}


/*
 * Local (S51k, S52K...) filesystem dependent reclock() interface.
 * Initializes lock handle, then calls filesystem independent one.
 *
 * NOTE:  Intercepts filesystem calls to reclock() in os/flock.c.
 */
int
lfs_reclock(ip, ldp, cmd, flag, offset)
	struct inode *ip;
	struct flock *ldp;
	int cmd;
	int flag;
	off_t offset;
{
	lockhandle_t *lhp;
	activeproc_t *app;
	int error;

	ASSERT(ip && ldp);

	/*
	 * Block request if not working.
	 */
	if (error = lockd_check()) {
		goto done;
	}

	/* 
	 * Get lock handle.
	 */
	if ((lhp = klm_gethandle(ip, klm_initlocal, &error)) == lhpNULL) {
		goto done;
	}

	/*
	 * Get active process structure for operation.
	 */
	if ((app = klm_getactive(lhp, ldp, cmd, &error)) == appNULL) {
		klm_relhandle(lhp, appNULL);
		goto done;
	}

	/*
	 * Call file system independent interface.
	 */
	error = klm_reclock(lhp, app, cmd, flag, offset);

	/* 
	 * Release the file handle and
	 * wake any blocked operations.
	 */
	klm_relhandle(lhp, app);
done:
	/*
	 * If the lock manager has died, 
	 * redirect to patched interface.
	 */
	if (error == ENOLINK) {
		ASSERT(lockd_patched == 0);
		cmn_err(CE_NOTE, "KLM: lockd closed, reclock redirected.");
		return(reclock(ip, ldp, cmd, flag, offset));
	}
	return (error);
}


/*
 * Remote (NFS) filesystem dependent reclock() interface.
 * Initializes lock handle, then calls filesystem independent one.
 *
 * NOTE:  Called from nfsfcntl() in fs/nfs/nfs_subr.c.
 */
nfs_reclock(ip, ldp, cmd, flag, offset)
	struct inode *ip;
	struct flock *ldp;
	int cmd;
	int flag;
	off_t offset;
{
	lockhandle_t *lhp;
	activeproc_t *app;
	int error;

	ASSERT(ip && ldp);

	/*
	 * Block request if not working.
	 */
	if (error = lockd_check()) {
		return (error);
	}

	/* 
	 * Get lock handle.
	 */
	if ((lhp = klm_gethandle(ip, klm_initremote, &error)) == lhpNULL) {
		return (error);
	}

	/*
	 * Get active process structure for operation.
	 */
	if ((app = klm_getactive(lhp, ldp, cmd, &error)) == appNULL) {
		klm_relhandle(lhp, appNULL);
		return (error);
	}

	/*
	 * Call file system independent interface.
	 */
	error = klm_reclock(lhp, app, cmd, flag, offset);

	/* 
	 * Release the file handle and
	 * wake any blocked operations.
	 */
	klm_relhandle(lhp, app);

	return (error);
}


/*
 * Filesystem independent cleanlocks() interface.
 * Remove all locks on file owned by calling process, per flags:
 *
 * If flag == USE_PID, match epid with ap_pid in activeproc_t.
 * If flag == IGN_PID, clean regardless of caller's epid.
 */
static int
klm_cleanlocks(lhp, flag)
	lockhandle_t *lhp;
	int flag;
{
	register activeproc_t *curr;
	register activeproc_t *next;
	int error = 0;
	/* structures */
	struct flock ld;

	/*
	 * Build an "unlock all records" operation.
	 */
	ld.l_type = F_UNLCK;
	ld.l_whence = 0;
	ld.l_start = 0;
	ld.l_len = 0;

	/*
	 * Search per inode active process list
	 * unlocking all records that match owner.
	 */
	next = lhp->lh_procs;
	while (curr = next) {		/* linkage */
		next = curr->ap_next;

	        /* 
		 * If the [p_epid, p_sysid] of a process is listed,
		 * locks may be owned and must be cleaned.
		 *
		 * NOTE:  Some processes may represent other systems,
		 *        thus IGN_PID says to remove all per sysid.
		 */
		if (((flag != IGN_PID) && 
		     (curr->ap_pid != u.u_procp->p_epid)) ||
		    (curr->ap_sysid != u.u_procp->p_sysid)) {
			continue;
		}
		ASSERT((curr->ap_flags & AP_BUSY) == 0);

		/*
		 * Found one, do operation as owner.
		 */
		curr->ap_lockp = &ld;
		curr->ap_cmd = F_SETLK;
		set_cred(&curr->ap_cred);
		curr->ap_flags |= AP_BUSY;
		ldstat.ls_busyoperations++;

		/*
		 * Unlock via file system independent interface.
		 */
		error = klm_reclock(lhp, curr, SETFLCK, 0, 0);

		/*
		 * If success, remove cleaned process
		 * from the active locking process list.
		 */
		curr->ap_flags &= ~AP_BUSY;
		--ldstat.ls_busyoperations;
		if (error == ENOLINK) {
			break;
		}
		klm_relactive(lhp, curr);

	} /* for */

	return (error);
}


/*
 * Local (S51k, S52K...) filesystem dependent cleanlocks() interface.
 * Generate lockhandle and call filesystem independent interface.
 *
 * NOTE:  Intercepts filesystem calls to cleanlocks() in os/flock.c.
 */
int
lfs_cleanlocks(ip, flag)
	struct inode *ip;
	int flag;
{
	lockhandle_t *lhp;
	int error;

	ASSERT(ip);

	/*
	 * Check inode for active locks.
	 */
	if (ip->i_filocks == (long *) NULL) {
		return (0);
	}
	
	/*
	 * Block request if not working.
	 */
	if (error = lockd_check()) {
		goto done;
	}

	/* 
	 * Get lock handle.
	 */
	if ((lhp = klm_gethandle(ip, klm_initlocal, &error)) == lhpNULL) {
		goto done;
	}

	/*
	 * Call file system independent interface.
	 */
	error = klm_cleanlocks(lhp, flag);

	/* 
	 * Release the file handle and
	 * wake any blocked operations.
	 */
	klm_relhandle(lhp, appNULL);
done:
	/*
	 * If the lock manager has died, 
	 * redirect to patched interface.
	 */
	if (error == ENOLINK) {
		ASSERT(lockd_patched == 0);
		cmn_err(CE_NOTE, "KLM: lockd closed, cleanlock redirected.");
		return (cleanlocks(ip, flag));
	}
	return (0);
}


/*
 * Remote (NFS) filesystem dependent cleanlocks() interface.
 * Generate lockhandle and call filesystem independent interface.
 *
 * NOTE:  Called from nfsclosei() in fs/nfs/nfs_clnt.c.
 */
int
nfs_cleanlocks(ip, flag)
	struct inode *ip;
	int flag;
{
	lockhandle_t *lhp;
	int error;

	ASSERT(ip);

	/*
	 * Check inode for active locks.
	 */
	if (ip->i_filocks == (long *) NULL) {
		return (0);
	}
	
	/*
	 * Block request if not working.
	 */
	if (error = lockd_check()) {
		return (0);
	}

	/* 
	 * Get lock handle.
	 */
	if ((lhp = klm_gethandle(ip, klm_initremote, &error)) == lhpNULL) {
		return (0);
	}

	/*
	 * Call file system independent interface.
	 */
	(void) klm_cleanlocks(lhp, flag);

	/* 
	 * Release the file handle and
	 * wake any blocked operations.
	 */
	klm_relhandle(lhp, appNULL);

	return (0);
}


/*
 * Called from KLM shutdown when lockd closes
 * to signal processes with lost locks.
 *
 * NOTE:  Examine for OS portability.
 */
static int 
klm_siglost(lhp)
	lockhandle_t *lhp;
{
	register proc_t *p;
	register activeproc_t *app;
	register activeproc_t *next;
	int signaled = 0;

	next = lhp->lh_procs;
	while (app = next) {
		next = app->ap_next;

		/*
		 * Send SIGLOST to process (*OS* dependent!).
		 * Slight fudge here, but not worth more effort.
		 */
		for (p = &proc[1]; p < (proc_t *) v.ve_proc; p++) {
			if (p->p_pid == app->ap_pid) {
#if m88k
				if ((p->p_sig.s[1] & sigmask(SIGLOST)) == 0) {
					(void) psignal(p, SIGLOST);
					signaled++;
				}
#else
				if ((p->p_sig & (1L << (SIGLOST-1))) == 0) {
					(void) psignal(p, SIGLOST);
					signaled++;
				}
#endif
				break;
			}
		}

		/*
		 * If not busy, free active process structure, else
		 * structure will be freed when operation redirects.
		 */
		if ((app->ap_flags & AP_BUSY) == 0) {
			klm_relactive(lhp, app);
		}
	}
	return (signaled);
}


/*
 * Shutdown the KLM machinery.
 *  
 * Initiate flush of all active locking processes.
 * Called by the lockd when changing from state recovery.
 *
 * NOTE:  Must not be called unless in recovery state.
 */
int 
klm_shutdown()
{
	register struct inode *ip;
	register lockhandle_t *lhp;
	int corrupted = 0;
	int signaled = 0;

	/*
	 * Cycle through inode list disconnecting lock handles
	 * and setting flushed.  Signal all remaining active
	 * locking processes.
	 */
	for (ip = &inode[0]; ip < (struct inode *) v.ve_inode; ip++) {
		if ((lhp = (lockhandle_t *) ip->i_filocks) != lhpNULL)  {
			if (lhp->lh_magic != KLM_LOCK_MAGIC) {
				corrupted++;
				continue;
			}
			ip->i_filocks = (long *) NULL;
			lhp->lh_flags |= LH_FLUSH;
			signaled += klm_siglost(lhp);
		}
	}
	if (corrupted) {
		cmn_err(CE_WARN, "KLM: %d lock handles corrupted.", corrupted);
	}
	return (signaled);
}
