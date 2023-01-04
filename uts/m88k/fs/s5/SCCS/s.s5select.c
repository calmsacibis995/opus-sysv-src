h30898
s 00004/00012/00107
d D 1.3 90/04/21 09:38:39 root 3 2
c return selectable when peer has gone away.
e
s 00002/00001/00117
d D 1.2 90/04/20 12:36:45 root 2 1
c removed IFCHR case
e
s 00118/00000/00000
d D 1.1 90/03/06 12:43:57 root 1 0
c date and time created 90/03/06 12:43:57 by root
e
u
U
t
T
I 1
/*
 * $Header: s5select.c  88/01/15 09:28:02 bruce Exp $
 * $Locker:  $
 *
 * Author (a) 1987, Damon Anton Permezel, all bugs revered
 */
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/fs/s5param.h"
#include "sys/fs/s5inode.h"
#include "sys/fs/s5macros.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/dirent.h"
#include "sys/signal.h"
#include "sys/errno.h"
#include "sys/psw.h"
#include "sys/user.h"
#include "sys/mount.h"
#include "sys/fstyp.h"
#include "sys/inode.h"
#include "sys/nami.h"
#include "sys/buf.h"
#include "sys/cmn_err.h"
#include "sys/conf.h"
#include "sys/file.h"
#include "sys/debug.h"

extern selwait;

s5select(ip, mode)
register struct inode *ip; {
	register struct buf *bp;
	register struct s5inode *s5ip;
	register long bsize;
	register unsigned on, n;
	register daddr_t bn;
	register type;
	register dev_t dev;

	if (ip->i_ftype == IFREG
	&&  ((ip->i_flag & ICHKLCK) || !s5access(ip, IMNDLCK))) {
		s5chklock(ip, mode);
		if (u.u_error)
			return 0;
	}

	s5ip = (struct s5inode *)ip->i_fsptr;
	ASSERT(s5ip != NULL);
	type = s5ip->s5i_mode & IFMT;

	switch (type) {
I 2
D 3
#if 0
E 2
	    case IFCHR:
		dev = (dev_t)ip->i_rdev;
		ip->i_flag |= IACC;
		if (cdevsw[major(dev)].d_str) {
			ASSERT(ip->i_sptr);
			return strselect(ip, mode);
		} else
			return (*cdevsw[major(dev)].d_select)(dev, mode);
D 2

E 2
I 2
#endif
E 3
E 2
	    case IFIFO:
		switch (mode) {
		    case FREAD:
I 3
			if (s5ip->s5i_fwcnt == 0)
				return 1;
E 3
			if (ip->i_size == 0) {
				if (s5ip->s5i_rsel
				    && s5ip->s5i_rsel->p_wchan
				    == (caddr_t)&selwait)
					s5ip->s5i_self |= S5I_RCOLL;
				else
					s5ip->s5i_rsel = u.u_procp;
				return 0;
			}
			break;
				
		    case FWRITE:
I 3
			if (s5ip->s5i_frcnt == 0)
				return 1;
E 3
			if (ip->i_size >= PIPSIZ) {
				if (s5ip->s5i_wsel
				    && s5ip->s5i_wsel->p_wchan
				    == (caddr_t)&selwait)
					s5ip->s5i_self |= S5I_WCOLL;
				else
					s5ip->s5i_wsel = u.u_procp;
				return 0;
			}
			break;
		}
		return 1;

	case IFBLK:
		return 1;

	case IFREG:
	case IFDIR:
		/*
		 * No reason why we can't do the same as for FIFO,
		 * if we can only figure out what the users' current
		 * file offset is for his reference to this file.
		 * ?pass it down in the FS_SELECT() call?
		 *
		 * (part of the problem also lies in the fact that multiple
		 *  users can have multiple references to the inode, but
		 *  we have only one place to record selectors.  This merely
		 *  implies that we have a potential for more collisions,
		 *  though.)
		 *
		 * MORE WORK TO DO HERE!
		 */
		return 1;

D 3
	default:
		u.u_error = ENODEV;
E 3
	}
}
E 1
