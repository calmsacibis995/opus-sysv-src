h59790
s 00000/00003/00168
d D 1.3 90/05/04 15:10:10 root 3 2
c removed CMMU defs
e
s 00007/00010/00164
d D 1.2 90/05/04 15:05:16 root 2 1
c added CL constants and general clean up
e
s 00174/00000/00000
d D 1.1 90/03/06 12:30:04 root 1 0
c date and time created 90/03/06 12:30:04 by root
e
u
U
t
T
I 1
#ifndef _SYS_PARAM_H_
#define _SYS_PARAM_H_



/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * fundamental variables
 * don't change too often
 */

#ifdef __88000_OCS_DEFINED
#define	HZ	(sysconf(3))
#define	MAXHOSTNAMELEN	256
#else
#include "sys/fs/s5param.h"

#define	OPUSVENDORID	0xF00
#define	MAXPID	30000		/* max process id */
#define	MAXUID	60000		/* max user id */
#define	MAXLINK	1024		/* max links */
#define	MAXPATHLEN	512
#define	MAXHOSTNAMELEN	256

#define	SSIZE	1		/* initial stack size (*4096 bytes) */
#define	SINCR	1		/* increment of stack (*4096 bytes) */
#define	USIZE	2		/* size of user block (*4096 bytes) */

#define	CANBSIZ	256		/* max size of typewriter line	*/
#ifdef INKERNEL
#define	HZ	60		/* 60 ticks/second of the clock */
#else
#define	HZ	(sysconf(3))
#endif

#define	NCARGS	(16*1024)	/* # characters in exec arglist */
				/*   must be multiple of NBPW.  */

/*	The following define is here for temporary compatibility
**	and should be removed in the next release.  It gives a
**	value for the maximum number of open files per process.
**	However, this value is no longer a constant.  It is a
**	configurable parameter, NOFILES, specified in the kernel
**	master file and available in v.v_nofiles.  Programs which
**	include this header file and use the following value may
**	not operate correctly if the system has been configured
**	to a different value.
*/

D 2
#define	NOFILE	100		/* was 20 -mat */
E 2
I 2
#define	NOFILE	100
E 2

/*	The following represent the minimum and maximum values to
**	which the paramater NOFILES in the kernel master file may
**	be set.
*/

#define	NOFILES_MIN	 20
#define	NOFILES_MAX	100

#define	USRSTACK ((caddr_t) 0xf0000000)	/* User's stack */

/*	The following represent the minimum and maximum values to
**	which the paramater NOFILES in the kernel master file may
**	be set.
*/

#define	NOFILES_MIN	 20
#define	NOFILES_MAX	100

/*	The following define builds the file system
**	to use the file system switch and enables profiling
*/

#define	FSPTR		1
#define PRFMAX		2048	/* maximum number of text addresses */
#define PRF_ON  	1	/* profiler collecting samples */
#define PRF_VAL		2	/* profiler contains valid text symbols */
#define PRF_INT		4	/* profile using internal (system) clock */
#define PRF_EXT		8	/* profile using externel (asynch) clock */
#define SPILLSIZE	64	/* no of bytes copied from stack overflow */

extern unsigned  prfstat;	/* state of profiler */

/*
 * priorities
 * should not be altered too much
 */

#define	PMASK	0177
#define	PCATCH	0400
#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define PMEM	0
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	39
#define	PUSER	60
#define	PIDLE	127

D 3
#ifndef CMMU
#define	CMMU
#endif
E 3
/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

D 2
#ifdef CMMU
E 2
#define NBPS	0x400000	/* Number of bytes per segment */
#define BPSSHFT	22		/* Bytes per segmnt shift LOG2(NBPS) if exact */
D 2
#else
#define NBPS	0x40000		/* Number of bytes per segment */
#define BPSSHFT	18		/* Bytes per segmnt shift LOG2(NBPS) if exact */
#endif
E 2
#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#define LOGNBPW	2		/* log2(NBPW) */
D 2
#ifdef CMMU
E 2
#define	NCPS	1024		/* Number of clicks per segment */
D 2
#else
#define	NCPS	64		/* Number of clicks per segment */
#endif
E 2
#define	NBPC	4096		/* Number of bytes per click */
#define	BPCSHIFT	12	/* LOG2(NBPC) if exact */
#ifndef NULL
#define	NULL	0
#endif
#define	CMASK	0		/* default mask for file creation */
#define	NODEV	(dev_t)(-1)
#define	NBPSCTR		512	/* Bytes per disk sector.	*/
#define SCTRSHFT	9	/* Shift for BPSECT.		*/

#define	UMODE	PS_CUR		/* current Xlevel == user */
#define	USERMODE(psw)	((psw & UMODE) == 0)
#define	PS_MSK	0x1C00
#define BASEPRI(ps)	((ps & PS_MSK) != 0)

#define	lobyte(X)	(((unsigned char *)&X)[1])
#define	hibyte(X)	(((unsigned char *)&X)[0])
#define	loword(X)	(((ushort *)&X)[1])
#define	hiword(X)	(((ushort *)&X)[0])

#define	MAXSUSE	255

/* REMOTE -- whether machine is primary, secondary, or regular */
#define SYSNAME 9		/* # chars in system name */
#define PREMOTE 39

#define	NUBLK		31	/* No. of Ublock windows [1..31] */
#define	UB_WANTED	(1<<NUBLK)

#define	CDLIMIT		32768

/*
 * Macros for fast min/max.
 */
#define	MIN(a,b) (((a)<(b))?(a):(b))
#define	MAX(a,b) (((a)>(b))?(a):(b))
#define	imin(a,b) (((a)<(b))?(a):(b))
I 2

#ifndef CLBYTES
#define	CLBYTES	2048
#define	CLSHIFT	11
#define	CLOFSET	(CLBYTES-1)
#endif
E 2

#endif /* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_PARAM_H_ */
E 1
