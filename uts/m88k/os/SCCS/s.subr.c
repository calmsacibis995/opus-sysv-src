h13235
s 00076/00000/00000
d D 1.1 90/03/06 12:28:13 root 1 0
c date and time created 90/03/06 12:28:13 by root
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

#ident	"@(#)kern-port:os/subr.c	10.10"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/fstyp.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/mount.h"
#include "sys/var.h"
#include "sys/conf.h"
#include "sys/debug.h"

/*
 * update is the internal name of 'sync'. It goes through the disk
 * queues to initiate sandbagged IO; goes through the I nodes to write
 * modified nodes; and it goes through the mount table to initiate modified
 * super blocks.
 */
update()
{
	sysupdate();
	bflush(NODEV);
}

sysupdate()
{
	register struct inode *ip;
	register struct mount *mp;
	static int	updatelock;

	if (updatelock++)
		return;
	for (mp = mount; mp < (struct mount *)v.ve_mount; mp++) {
		if (mp->m_flags & MINUSE) 
			(*fstypsw[mp->m_fstyp].fs_update)(mp);
	}
	for (ip = &inode[0]; ip < (struct inode *)v.ve_inode; ip++)
		if ((ip->i_flag & ILOCK) == 0 && ip->i_count != 0
		&& (ip->i_flag & (IACC|IUPD|ICHG))) {
			ip->i_flag |= ILOCK;
			ASSERT(addilock(ip) == 0);
			ip->i_count++;
			FS_IUPDAT(ip, &time, &time);
			iput(ip);
		}
	updatelock = 0;
}

/*
 * Routine which sets a user error; placed in
 * illegal entries in the bdevsw and cdevsw tables.
 */
nodev()
{
	u.u_error = ENODEV;
}

/*
 * Null routine; placed in insignificant entries
 * in the bdevsw and cdevsw tables.
 */
nulldev()
{
}
E 1
