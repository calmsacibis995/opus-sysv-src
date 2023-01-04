h43638
s 00100/00000/00000
d D 1.1 90/03/06 12:31:28 root 1 0
c date and time created 90/03/06 12:31:28 by root
e
u
U
t
T
I 1
#ifndef _NFS_LOCKD_H_
#define _NFS_LOCKD_H_

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/* @(#)lockd.h	4.9 LAI System V NFS Release 3.2/V3	source */


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * KLM lock device data structures common with NLM lock manager.
 */

/*
 * Lock device IOCTL's -  Rev'd for LAI NFS release 3.2.2.
 */
#define L_SETCONF	11	/* set the kernel configuration */
#define L_GETINFO	12	/* get all kernel information */
#define L_ACTIVATE	13	/* activate the lock manager */
#define L_RECLAIM	14	/* upload locks to lock manager */
#define L_RECOVER	15	/* download locks from lock manager */

/*
 * Valid states for KLM lock device.
 *
 * Transits: loaded, closed, then opened through closed.
 *
 * NOTE:  Reclaim is optional state.
 */
typedef enum {
	lkd_loaded,		/* ~init, ~open, ~intercept, ~locking */
	lkd_opened,		/* init, open, intercept, ~locking */
	lkd_reclaim,		/* init, open, intercept, reclaim */
	lkd_working,		/* init, open, intercept, locking */
	lkd_recover,		/* init, open, intercept, recover */
	lkd_closed		/* init, ~open, ~intercept, ~locking */
} lockd_state_t;

/*
 * The NLM lock daemon sets KLM configuration with this structure.
 */
typedef struct lockd_conf {
	int		lc_trans;		/* transport identifier */
	struct netbuf	lc_addr;		/* transport address */
	struct netbuf	lc_host;		/* nlm's hostname */
} lockd_conf_t;

#define lc_hostlen	lc_host.len
#define lc_hostbuf	lc_host.buf
#define lc_addrlen	lc_addr.len
#define lc_addrbuf	lc_addr.buf

/*
 * The NLM lock daemon gets KLM information with this structure.
 */
typedef struct lockd_info {
	lockd_state_t	li_lockd_state;		/* /dev/lockd state */
	long		li_sys_granted;		/* SysV granted locks */
	long		li_sys_sleeping;	/* SysV sleeping locks */
	long		li_klm_inodes;		/* KLM inodes with locks */
	long		li_klm_procs;		/* KLM procs with locks */
	long		li_klm_busy;		/* KLM busy operations */
} lockd_info_t;

/*
 * The nlmstat command reads KLM statistics with this structure.
 */
typedef struct lockd_stat {
	lockd_state_t	ls_lockd_state;		/* lock device state */
	long		ls_lockhandles;		/* #inode with locks */
	long		ls_activeprocs;		/* #procs with locks */
	long		ls_busyoperations;	/* #busy operations */
	long		ls_local_fsops;		/* local fs operations */
	long		ls_remote_fsops;	/* remote fs operations */
	long		ls_klm_procs[5];	/* calls per procedure */
	long		ls_klm_stats[7];	/* results per status */
	long		ls_klm_errors;		/* error returns */
	long		ls_nlm_calls;		/* calls to nlm */
} lockd_stat_t;

#if !m88k
/*
 * System V does not define SIGLOST, we use SIGUSR2 to signal lost locks.
 */
#define SIGLOST		SIGUSR2
#endif


#endif	/* ! _NFS_LOCKD_H_ */
E 1
