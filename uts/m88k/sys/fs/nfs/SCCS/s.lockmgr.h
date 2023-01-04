h56722
s 00134/00000/00000
d D 1.1 90/03/06 12:31:29 root 1 0
c date and time created 90/03/06 12:31:29 by root
e
u
U
t
T
I 1
#ifndef _NFS_LOCKMGR_H_
#define _NFS_LOCKMGR_H_

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/* @(#)lockmgr.h	4.7 LAI System V NFS Release 3.2/V3	source */

/* @(#)lockmgr.h	1.2 86/12/09 NFSSRC */
/*      Copyright (C) 1984, Sun Microsystems, Inc.  */


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Header file for Kernel<->Network Lock-Manager implementation
 */

/*
 * Per lock owner structure.
 *
 * We pass the process's [pid, sysid] to the lock manager as
 * the lock owner identifier.  It together with the hostname
 * uniquely identifies the lock owner.  The sysid is used by
 * some file server processes to further qualify the owner.
 *
 * WARNING:  The [pid, sysid] must fit in "int" since the owner
 *	     id is passed to the lock manager in the pid of a
 *	     struct klm_lock.  The KLM/NLM protocol needs to
 *	     accomodate the [pid, sysid] as identifiers.
 */
typedef union {
	int		u_oid;		/* owner id */
	struct {
		short	s_pid;		/* process id */
		short   s_sysid;	/* system id */
	} u_proc;
} lockowner_t;

#define lopNULL		((lockowner_t *)0)	/* useful */

#define	lo_oid		u_oid
#define	lo_pid		u_proc.s_pid
#define	lo_sysid	u_proc.s_sysid

/*
 * Per active locking process structure.
 *
 * We maintain a list of active locking processes off the 
 * lock handle for use in cleaning locks upon process close
 * and signaling lost lock owners upon lockd closed state.
 * We also maintain information about the current locking
 * operation for reuse by each operation.
 * 
 */
typedef struct activeproc_s {
	lockowner_t	 	 ap_owner;	/* locking process */
	int		 	 ap_pgrp;	/* process group */
	int			 ap_cmd;	/* fcntl() command */
	int			 ap_flags;	/* see flags below */
	struct flock		*ap_lockp;	/* lock description */
	struct ucred	 	 ap_cred;	/* user credentials */
	struct activeproc_s	*ap_next;	/* forward link */
} activeproc_t;

#define appNULL		((activeproc_t *)0)	/* useful */

#define	ap_oid		ap_owner.lo_oid
#define	ap_pid		ap_owner.lo_pid
#define	ap_sysid	ap_owner.lo_sysid

/*
 * Flags for active locking process operation.
 */
#define	AP_BUSY		(0x0001)	/* inuse consistency check */
#define	AP_PLOCK	(0x0002)	/* release inode if blocking */
#define	AP_GETLKW	(0x0004)	/* emulating get lock wait */

/*
 * Per active inode lock handle structure.
 * 
 * SVR3 hangs locks off the ip->i_filocks and checks
 * for non-NULL in various places outside os/flock.c,
 * ie. os/sys2.c.  We hang a single lock handle, used 
 * by all lock operations on the inode and a list of
 * active locking processes that have used the lock
 * handle.  We remove an active process when that
 * process closes the inode or the lockd closes.
 *
 * Uniquely describes any file in a domain [server, fhandle].
 *
 * NOTE:  Size of lh_file field should track the size of a fhandle.
 */
#define KLM_LHSIZE	NFS_FHSIZE

typedef struct lockhandle_s {
	struct inode	*lh_ip;		/* inode of file */
	fhandle_t	 lh_file;	/* fhandle of file */
	char		*lh_server;	/* server of file */
	int		 lh_flags;	/* see flags below */
	int		 lh_magic;	/* avoid collusion */
	activeproc_t	*lh_procs;	/* active processes */
} lockhandle_t;

#define lhpNULL		((lockhandle_t *)0)	/* useful */

/*
 * Flags for lock handle.
 */
#define	LH_NOCACHE	(0x0001)	/* remote: rnode caching off */
#define	LH_FLUSH	(0x0002)	/* locks have been flushed */

/*
 * Since we have two+ locking systems and they can
 * not simultaneously be holding locks, we use a
 * magic number to detect collusion.
 */
#define	KLM_LOCK_MAGIC	((int)(0x011451))

#define	lh_fsid		lh_file.fh_fsid
#define	lh_fno		lh_file.fh_fno
#define	lh_fgen		lh_file.fh_fgen


#endif	/* ! _NFS_LOCKMGR_H_ */
E 1
