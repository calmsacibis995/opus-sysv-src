#ifndef _SYS_SYSMACROS_H_
#define _SYS_SYSMACROS_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Some macros for units conversion
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* Core clicks to segments and vice versa */

#if NCPS == 1024
#define ctos(x)		(((x) + (NCPS-1)) >> 10)
#define	ctost(x)	((x) >> 10)
#else
#define ctos(x)		(((x) + (NCPS-1)) / NCPS)
#define	ctost(x)	((x) / NCPS)
#endif
#define	stoc(x)		((x) * NCPS)

/* byte address to segment and vice versa  */
#define stob(x)		((unsigned)(x) << BPSSHFT)
#define	btos(x)		((unsigned)(x) >> BPSSHFT)

/* inumber to disk address */
#ifdef INOSHIFT
#define	itod(x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1))>>INOSHIFT)
#else
#define	itod(x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1))/INOPB)
#endif

/* inumber to disk offset */
#ifdef INOSHIFT
#define	itoo(x)	(int)(((unsigned)(x)+(2*INOPB-1))&(INOPB-1))
#else
#define	itoo(x)	(int)(((unsigned)(x)+(2*INOPB-1))%INOPB)
#endif

/* clicks to bytes */
#ifdef BPCSHIFT
#define	ctob(x)	((x)<<BPCSHIFT)
#else
#define	ctob(x)	((x)*NBPC)
#endif

/* bytes to clicks */
#ifdef BPCSHIFT
#define	btoc(x)	(((unsigned)(x)+(NBPC-1))>>BPCSHIFT)
#define	btoct(x)	((unsigned)(x)>>BPCSHIFT)
#else
#define	btoc(x)	(((unsigned)(x)+(NBPC-1))/NBPC)
#define	btoct(x)	((unsigned)(x)/NBPC)
#endif

#define brdev(x)	(x&0x1fff)


/* major part of a device external from the kernel */
#define	major(x)	(int)(((unsigned)(x)>>8)&0x7F)
#define	bmajor(x)	(int)(((unsigned)(x)>>8)&0x7F)

/* minor part of a device external from the kernel */
#define	minor(x)	(int)((x)&0xFF)

/* make a device number */
#define	makedev(x,y)	(dev_t)(((x)<<8) | (y))

/*
 *   emajor() allows kernel/driver code to print external major numbers
 *   eminor() allows kernel/driver code to print external minor numbers
 */

#define emajor(x)	(int)(((unsigned)(x)>>8)&0x7F)
#define eminor(x)	(int)((x)&0xFF)

/*	Calculate user process priority.
*/

#define calcppri(p)	((p->p_cpu) >> 1) +  p->p_nice + (PUSER - NZERO)

/*
 *  Evaluate to true if the process is a server - Distributed UNIX
 */
#define	server()	(u.u_procp->p_sysid != 0)

/*
 * Atomic setting and clearing of bit flags.
 */

#define	ATOMIC(EX)	{ intoff(); EX ; inton(); }

#endif	/* ! _SYS_SYSMACROS_H_ */
