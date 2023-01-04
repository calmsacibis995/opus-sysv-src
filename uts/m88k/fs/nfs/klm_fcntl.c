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
/*	Copyright (c) 1984, 1987 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*								*/
/*	#ident  "@(#)s5sys3.c   1.3"				*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)klm_fcntl.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/fstyp.h"
#include "sys/fs/s5macros.h"
#include "sys/systm.h"
#include "sys/mount.h"
#include "sys/ino.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/fs/s5filsys.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/immu.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/user.h"
#include "sys/fs/s5inode.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/fcntl.h"
#include "sys/flock.h"
#include "sys/conf.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "sys/ioctl.h"
#include "sys/var.h"
#include "sys/ttold.h"
#include "sys/open.h"
#include "sys/debug.h"

/*
 * klm_s5fcntl is a replacement for s5fcntl().  The major difference
 * being that it calls s5_reclock() instead of reclock().  This is
 * activated when the lock manager daemon switches the entry point
 * at exection time.
 */
klm_s5fcntl(ip, cmd, arg, flag, offset)
	register struct inode *ip;
	int cmd, arg, flag;
	off_t offset;
{
	struct flock bf;
	int i;

	/*
	 * Call lock manager thru s5_reclock to set 
	 * network (NFS) lock then execute normal
	 */
	switch (cmd) {
	case F_GETLK:
		/* get record lock */
		if (copyin((caddr_t)arg, &bf, sizeof(bf)))
			u.u_error = EFAULT;
		else if ((i = s5_reclock(ip, &bf, 0, flag, offset)) != 0)
			u.u_error = i;
		else if (copyout(&bf, (caddr_t)arg, sizeof(bf)))
			u.u_error = EFAULT;
		break;

	case F_SETLK:
		/* set record lock */
		if (copyin((caddr_t)arg, &bf, sizeof(bf)))
			u.u_error = EFAULT;
		else if ((i = s5_reclock(ip, &bf, SETFLCK, flag, offset)) != 0) {
			/* The following if statement is to maintain
			** backward compatibility. Note: the return
			** value of reclock was not changed to
			** EAGAIN because rdwr() also calls reclock(),
			** and we want rdwr() to meet the standards.
			*/
			if (i == EAGAIN)
				u.u_error = EACCES;
			else
				u.u_error = i;
		}
		break;

	case F_SETLKW:
		/* set record lock and wait */
		if (copyin((caddr_t)arg, &bf, sizeof(bf)))
			u.u_error = EFAULT;
		else if ((i = s5_reclock(ip, &bf, SETFLCK | SLPFLCK,
					 flag, offset)) != 0) {
			u.u_error = i;
		}
		break;

	case F_CHKFL:
		break;

	default:
		u.u_error = EINVAL;
	}
}

/*
 * When the lock manager user daemon executes it changes the s5
 * filesystem entry points for closei and fcntl.  This allows for
 * s5/nfs locking, within the lock manager rules, to function
 * together.  klm_s5closei() is the new entry point for closei.
 */
klm_s5closei(ip, flag, count, offset)
	register struct inode *ip;
	int flag, count;
	off_t offset;
{

	klm_cleanlocks(ip, flag);
	s5closei(ip, flag, count, offset);
}
