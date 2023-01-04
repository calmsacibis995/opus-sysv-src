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
	    case IFIFO:
		switch (mode) {
		    case FREAD:
			if (s5ip->s5i_fwcnt == 0)
				return 1;
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
			if (s5ip->s5i_frcnt == 0)
				return 1;
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

	}
}
