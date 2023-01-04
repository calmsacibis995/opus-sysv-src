h52350
s 00119/00000/00000
d D 1.1 90/03/06 12:28:06 root 1 0
c date and time created 90/03/06 12:28:06 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/pipe.c	10.12"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/fstyp.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/debug.h"
#include "sys/conf.h"

short pipefstyp;

/*
 * Generic pipe sys call. 
 * Switch to current pipe fs
 */

pipe()
{
	register struct inode *ip;
	register struct file *rf, *wf;
	struct mount *mp;
	int r;

	/* get an inode on the device associated with pipefstyp */
	ASSERT(pipefstyp > 0 && pipefstyp < nfstyp);
	mp = fsinfo[pipefstyp].fs_pipe;
	ASSERT(mp != NULL);
	if ((ip = (*fstypsw[pipefstyp].fs_getinode)(mp, FSG_PIPE, -1)) == NULL)
		return;
	if ((rf = falloc(ip, FREAD)) == NULL) {
		iput(ip);
		return;
	}
	r = u.u_rval1;
	if ((wf = falloc(ip, FWRITE)) == NULL) {
		rf->f_count = 0;
		rf->f_next = ffreelist;
		ffreelist = rf;
		u.u_ofile[r] = NULL;
		iput(ip);
		return;
	}
	u.u_rval2 = u.u_rval1;
	u.u_rval1 = r;
	wf->f_flag = FWRITE;
	wf->f_inode = ip;
	rf->f_flag = FREAD;
	rf->f_inode = ip;
	ip->i_count = 2;
	prele(ip);
}

/*
 * Lock a pipe.
 * If it's already locked,
 * set the WANT bit and sleep.
 */
#ifndef DEBUG
plock(ip)
	register struct inode *ip;
#else
plock(ip, caller)
	register struct inode *ip;
	int *caller;
#endif
{
#ifdef DEBUG
	ilog(ip, 3, caller);
#endif
	ASSERT(ip->i_count > 0);
	while (ip->i_flag & ILOCK) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
	}
	ASSERT(ip->i_count > 0);
	ip->i_flag |= ILOCK;
	ASSERT(addilock(ip) == 0);
}

/*
 * Unlock a pipe.
 * If WANT bit is on,
 * wakeup.
 * This routine is also used
 * to unlock inodes in general.
 */
#ifndef DEBUG
prele(ip)
	register struct inode *ip;
#else
prele(ip, caller)
	register struct inode *ip;
	int *caller;
#endif
{
#ifdef DEBUG
	ilog(ip, 4, caller);
#endif
	ASSERT(ip->i_flag & ILOCK);

	ip->i_flag &= ~ILOCK;
	if (ip->i_flag & IWANT) {
		ip->i_flag &= ~IWANT;
		wakeup((caddr_t)ip);
	}
	ASSERT(rmilock(ip) == 0);
}
E 1
