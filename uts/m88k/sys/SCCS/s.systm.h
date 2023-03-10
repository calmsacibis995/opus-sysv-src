h49212
s 00071/00000/00000
d D 1.1 90/03/06 12:30:36 root 1 0
c date and time created 90/03/06 12:30:36 by root
e
u
U
t
T
I 1
#ifndef _SYS_SYSTM_H_
#define _SYS_SYSTM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Random set of variables used by more than one routine.
 */

extern struct inode *rootdir;	/* pointer to inode of root directory */
extern short cputype;		/* type of cpu = 40, 45, 70, 780, 0x3b5 */
extern time_t lbolt;		/* time in HZ since last boot */
extern time_t time;		/* time in sec from 1970 */

extern char runin;		/* scheduling flag */
extern char runout;		/* scheduling flag */
extern char runrun;		/* scheduling flag */
extern char curpri;		/* current priority */
extern struct proc *curproc;	/* current proc */
extern struct proc *runq;	/* head of linked list of running processes */

extern		maxmem;		/* max available memory (clicks) */
extern		physmem;	/* physical memory (clicks) on this CPU */
extern		maxclick;	/* Highest physical click + 1.		*/
extern daddr_t	swplo;		/* block number of start of swap space */
extern		nswap;		/* size of swap space in blocks*/
extern dev_t	rootdev;	/* device of the root */
extern dev_t	swapdev;	/* swapping device */
extern dev_t	pipedev;	/* pipe device */
extern char	*panicstr;	/* panic string pointer */
extern		blkacty;	/* active block devices */
extern		pwr_cnt, pwr_act;
extern int 	(*pwr_clr[])();

dev_t getmdev();
struct inode *iget();
struct inode *namei();
struct inode *remote_call();
struct buf *getblk();
struct buf *geteblk();
struct buf *bread();
struct buf *breada();
struct file *falloc();
int	upath();

/*
 * Structure of the system-entry table
 */
extern struct sysent {
	char	sy_narg;		/* total number of arguments */
	char	sy_setjmp;		/* set to 1 if systrap() should not do a setjmp() */
	int	(*sy_call)();		/* handler */
} sysent[];

extern int	selwait;
extern int	kdebug;

#endif	/* ! _SYS_SYSTM_H_ */
E 1
