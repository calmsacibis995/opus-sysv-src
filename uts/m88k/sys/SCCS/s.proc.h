h55177
s 00000/00005/00146
d D 1.4 90/05/05 06:43:08 root 4 3
c removed mototola references
e
s 00000/00005/00151
d D 1.3 90/05/04 15:09:05 root 3 2
c removed CMMU defs
e
s 00001/00001/00155
d D 1.2 90/03/27 17:09:16 root 2 1
c proc is now a pointer
e
s 00156/00000/00000
d D 1.1 90/03/06 12:30:07 root 1 0
c date and time created 90/03/06 12:30:07 by root
e
u
U
t
T
I 1
#ifndef _SYS_PROC_H_
#define _SYS_PROC_H_

#include "sys/time.h"

/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

D 4
/*
 *	Copyright (c) 1987 Motorola Inc
 *	All Rights Reserved
 *		@(#)proc.h	10.2
 */
E 4
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

D 3
#ifndef CMMU
#define	CMMU
#endif
E 3
/*	One structure allocated per active process. It contains all
**	data needed about the process while the process may be swapped
**	out.  Other per process data (user.h) may swapped with the
**	process but in fact it is not.  The pregion structure is an
**	extension of the proc structure.  It is allocated dynamically
**	during system initialization so that the number of pregions
**	per process is not fixed by this structure.  Increasing the
**	number of pregions per process does not require recompiling the
**	system.
*/

typedef struct	proc	{
	char	p_stat;			/* Status of process.		*/
	char	p_pri;			/* priority, negative is high	*/
	char	p_cpu;			/* cpu usage for scheduling	*/
	char	p_nice;			/* nice for cpu usage		*/
	uint	p_flag;			/* Flags defined below.		*/
	ushort	p_uid;			/* real user id			*/
	ushort	p_suid;			/* saved (effective) user id	*/
					/* from exec.			*/
	short	p_pgrp;			/* name of process group	*/
					/* leader			*/
	short	p_pid;			/* unique process id		*/
	short	p_ppid;			/* process id of parent		*/
	ushort	p_sgid;			/* saved (effective) group	*/
					/* id from exec.		*/
	pde_t	p_ubpgtbl[USIZE*NPDEPP];/* Page table for the U-block	*/
D 3
#ifdef CMMU
E 3
#define ONPDEPP	4
	pde_t	p_ubfill[(USIZE*ONPDEPP)-USIZE];
D 3
#endif
E 3
	sde_t	*p_pttbl;		/* pointer table array		*/
	sigset_t	p_sig;		/* signals pending to this	*/
					/* process			*/
	struct	proc	*p_flink;	/* linked list of processes	*/
	struct	proc	*p_blink;	/* linked list of processes	*/
	caddr_t	p_wchan;		/* Wait addr for sleeping  	*/
					/* processes.			*/
	struct	proc	*p_parent;	/* ptr to parent process	*/
	struct	proc	*p_child;	/* ptr to first child process	*/
	struct	proc	*p_sibling;	/* ptr to next sibling		*/
					/* process on chain		*/
	int	p_clktim;		/* time to alarm clock signal	*/
	uint	p_size;			/* size of swappable image	*/
					/* in pages.			*/
					/* sigset turns on this bit	*/
					/* signal does not turn on this bit */
	short	p_xstat;		/* exit status for wait		*/
#if opus
	ushort	p_compat;		/* Opus special */
#endif
	time_t	p_utime;		/* user time, this proc		*/
	time_t	p_stime;		/* system time, this proc	*/
	struct  proc *p_mlink;		/* link list of processes	*/
					/* sleeping on memwant or	*/
					/* swapwant.			*/
	preg_t	*p_region;		/* process regions		*/
	ushort	p_mpgneed;		/* number of memory pages	*/
					/* needed in memwant.		*/
	char	p_time;			/* resident time for scheduling	*/
	short	p_epid;			/* effective pid		*/
					/* normally - same as p_pid	*/
					/* if server - p_pid that sent msg */
	sysid_t p_sysid;		/* normally - same as sysid	*/
	struct	itimerval p_itimer;	/* interval timer support	*/
	sigset_t	p_sigmask;	/* tracing signal mask for /proc */
	sigset_t	p_hold;		/* hold signal bit mask		*/
	sigset_t	p_chold;	/* defer signal bit mask	*/
} proc_t;

#define	p_link	p_flink

#define	isset(p)	((p)->p_sig.s[0] | (p)->p_sig.s[1])
#define	clrsig(p, elem)	{(p)->elem.s[0] = 0; (p)->elem.s[1] = 0;}

D 2
extern struct proc proc[];	/* the proc table itself	*/
E 2
I 2
extern struct proc *proc;	/* the proc table itself	*/
E 2

/* stat codes */

#define	SSLEEP	1		/* Awaiting an event.		*/
#define	SRUN	2		/* Running.			*/
#define	SZOMB	3		/* Process terminated but not	*/
				/* waited for.			*/
#define	SSTOP	4		/* Process stopped by signal	*/
				/* since it is being traced by	*/
				/* its parent.			*/
#define	SIDL	5		/* Intermediate state in	*/
				/* process creation.		*/
#define SXBRK	6		/* Process being xswapped       */

/* flag codes */

#define	SSYS	0x0001		/* System (resident) process.	*/
#define	STRC	0x0002		/* Process is being traced.	*/
#define	SWTED	0x0004		/* Stopped process has been	*/
				/* given to parent by wait	*/
				/* system call.  Don't return	*/
				/* this process to parent again	*/
				/* until it runs first.		*/
#define SNWAKE	0x0008		/* Process cannot wakeup by	*/
				/* a signal.			*/
#define SLOAD	0x0010		/* in core                      */
#define SLOCK   0x0020		/* Process cannot be swapped.	*/
#define	SRSIG	0x0040		/* Set when signal goes remote	*/
#define SPOLL	0x0080		/* Process in stream poll	*/
#define SPRSTOP	0x0100		/* process is being stopped via /proc */
#define SPROCTR	0x0200		/* signal tracing via /proc	*/
#define SPROCIO	0x0400		/* doing I/O via /proc, so don't swap */
#define SSEXEC	0x0800		/* stop on exec			*/
#define SPROPEN	0x1000		/* process is open via /proc	*/
#define SSEL	0x2000		/* process select()ing		*/
#define	SOMASK	0x4000		/* pause flag for sigaction	*/
#define	SKILL	0x8000		/* signal generated by kill	*/

#define PTRACED(p)	((p)->p_flag&(STRC|SPROCTR|SSEXEC|SPROPEN))

#if opus
#define	SCOMPAT	0x0001
#define	SSSEL	0x0002
#define	SSTIMO	0x0004
#define	SALIGN	0x0008		/* fixup alignment traps */
#define	STRALL	0x0010		/* trace all system calls */
#define	STRERRS	0x0020		/* trace system call errors only */
#define	SRECALN	0x0040		/* record alignment traps */
#define	SSERIAL	0x0080
#define	SNOCORE	0x0100
#endif

#endif	/* ! _SYS_PROC_H_ */
E 1
