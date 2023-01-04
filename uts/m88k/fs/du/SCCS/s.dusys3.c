h05203
s 00066/00000/00000
d D 1.1 90/03/06 12:40:56 root 1 0
c date and time created 90/03/06 12:40:56 by root
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

#ident	"@(#)kern-port:fs/du/dusys3.c	1.3"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/fstyp.h"
#include "sys/fs/s5param.h"
#include "sys/systm.h"
#include "sys/file.h"
#include "sys/fs/s5inode.h"
#include "sys/inode.h"
#include "sys/nami.h"
#include "sys/fs/s5filsys.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/debug.h"
#include "sys/rdebug.h"
#include "sys/message.h"


/* fcntl */

dufcntl(ip, cmd, arg, flag, offset)
register struct inode *ip;
int cmd, arg, flag;
off_t offset;
{
	DUPRINT4 (DB_FSS,"dufcntl: ip %x cmd %x offset %x\n", ip, cmd, offset);
	remfileop (ip, flag, offset);
	DUPRINT2 (DB_FSS,"dufcntl: u.u_error = %d \n",u.u_error);
}

/* ioctl */
duioctl(ip, cmd, arg, flag)
register struct inode *ip;
int cmd, arg, flag;
{
	DUPRINT4 (DB_FSS,"duioctl: ip %x cmd %x flag %x\n", ip, cmd, flag);
	remfileop(ip, flag,NULL);
	DUPRINT2 (DB_FSS,"duioctl: u.u_error = %d \n",u.u_error);
}

dunotify(ip, noargp)
register struct inode *ip;
register struct argnotify *noargp;
{
	struct a {int fdes; off_t off; int sbase; } *uap = (struct a *) u.u_ap;
	ASSERT(noargp != 0);
	switch (noargp->cmd) {
	case NO_SEEK:
		if(noargp->data2 == 2) {
			uap->off -= ip->i_size;
			remfileop(ip,NULL,NULL);
			return (uap->off);
		}
	}
	return (uap->off);
}
E 1
