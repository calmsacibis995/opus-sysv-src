h11320
s 00058/00000/00000
d D 1.1 90/03/06 12:30:31 root 1 0
c date and time created 90/03/06 12:30:31 by root
e
u
U
t
T
I 1
#ifndef _SYS_SYS3B_H_
#define _SYS_SYS3B_H_


/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#ifndef DIRSIZ
#define	DIRSIZ	14
#endif

/*
 * Commands for sys3b(sys_local) system call
 */


#define	S88CONT		2	/* Instr. cont. after signal handlers	*/
#define	S88STIME	4	/* Set internal time only 		*/
#define	S88SETNAME	5	/* Rename the sytem			*/
#define	S88DELMEM	9	/* Delete available memory for testing	*/
#define	S88ADDMEM	10	/* Return previously deleted memory	*/
#define	S88SWAP		12	/* Manipulate swap space (see swap.h)	*/
#define	S88DEB		18	/* debug flag */
#define	S88FIXALIGN	20	/* fixup  alignment errors */
#define	S88NIXALIGN	21	/* don't fixup  alignment errors */
#define	S88RECALIGN	22	/* record alignment traps */
#define	S88NORECALIGN	23	/* don't record alignment traps */
#define	S88NOCORE	24	/* don't dump core */
#define	S88CORE		25	/* dump core */

#define	S3BSWAP		S88SWAP	/* Declare swap space			  */
#define SETNAME		S88SETNAME

/* opus specific defines */
#define	SUSERMAP	73	/* map user buffer into dma space */
#define	SUSERUNMAP	74	/* unmap user buffer from dma space */
#define	SUSERSLEEP	75	/* user initiated sleep */
#define	SUSERWAKEUP	76	/* user initiated wakeup */
#define	SDOSSYS		77
#define	SGETCWD		78
#define SLIBOPEN	79	/* open library */
#define SLIBCALL	80	/* make library call(s) */
#define SLIBCLOSE	81	/* close library */
#define	SNETTYPE	82
#define	SOCKFUNC	128	/* calls to bsd functions */

#endif	/* ! _SYS_SYS3B_H_ */
E 1
