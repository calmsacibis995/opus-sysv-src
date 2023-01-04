/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/utssys.c	10.9"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/uadmin.h"
#include "sys/mount.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/fstyp.h"
#include "sys/ustat.h"
#include "sys/statfs.h"
#include "sys/file.h"
#include "sys/var.h"
#include "sys/utsname.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/systm.h"
#include "sys/sysmacros.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
#include "sys/conf.h"

#if UPS
#include "sys/ups.h"
#endif
#ifdef UPS
#include "sys/tty.h"
struct tty *ups_tp = 0;
#endif

utssys()
{
	register i;
	register struct a {
		char	*cbuf;
		int	mv;
		int	type;
	} *uap;
	struct ustat ust;
	struct oustat oust;		/* MMV */
	struct statfs stfs;

	uap = (struct a *)u.u_ap;
	switch (uap->type) {
	case 0:		/* uname */
		if (copyout(&utsname, uap->cbuf,
		  sizeof(struct utsname)))
			u.u_error = EFAULT;
		return;

	/* case 1 was umask */

	case 3:		/* new ustat */
	case 2:		/* old ustat */
		if (uap->mv < 0) {
			duustat();
			return;
		}
		for (i = 0; i < v.v_mount; i++) {
			register struct mount *mp;
			register struct inode *ip;

			mp = &mount[i];
			if ((mp->m_flags & MINUSE) && mp->m_dev == uap->mv) {
				ASSERT(mp->m_mount != NULL);
				ip = iget(mp, mp->m_mount->i_number);
				if (ip == NULL) {
					u.u_error = ENOENT;
					return;
				}
				(*fstypsw[mp->m_fstyp].fs_statfs)(ip, 
				  (char *)&stfs, 0);
				if (u.u_error) {
					iput(ip);
					return;
				}
				if (uap->type == 3) {
				ust.f_tfree = (daddr_t)stfs.f_bfree;
				ust.f_tinode = (ino_t)stfs.f_ffree;
				bcopy(stfs.f_fpack, ust.f_fpack, 
				  sizeof(ust.f_fpack));
				bcopy(stfs.f_fname, ust.f_fname, 
				  sizeof(ust.f_fname));

				if (copyout(&ust, uap->cbuf, sizeof(ust)))
					u.u_error = EFAULT;
				}
				else {	/* MMV DELETE OLD STYLE */
				oust.f_tfree = (daddr_t)stfs.f_bfree;
				oust.f_tinode = (ushort)stfs.f_ffree;
				bcopy(stfs.f_fpack, oust.f_fpack, 
				  sizeof(oust.f_fpack));
				bcopy(stfs.f_fname, oust.f_fname, 
				  sizeof(oust.f_fname));

				if (copyout(&oust, uap->cbuf, sizeof(oust)))
					u.u_error = EFAULT;
				}
				iput(ip);
				return;
			}
		}
		u.u_error = EINVAL;
		return;

	default:
		u.u_error = EFAULT;
	}
}

/*
 * administrivia system call
 */

uadmin()
{
	register struct a {
		int	cmd;
		int	fcn;
		int	mdep;
	} *uap;
	static ualock;

	if (!iscons()) {
		u.u_error = EACCES;
		return;
	}
	if (ualock || !suser())
		return;
	ualock = 1;
	uap = (struct a *)u.u_ap;
	switch (uap->cmd) {

	case A_SHUTDOWN:
		{
			register struct proc *p = &proc[4];

			for (; p < (struct proc *)v.ve_proc; p++) {
				if (p->p_stat == NULL)
					continue;
				if (p != u.u_procp)
					psignal(p, SIGKILL);
			}
		}
		delay(HZ);	/* allow other procs to exit */
		{
			register struct mount *mp = &mount[0];

			xumount(mp);
			update();
			srumountfun();
		}
	case A_REBOOT:
		mdboot(uap->fcn, uap->mdep);
		/* no return expected */
		break;

	case A_REMOUNT:
		{
			register struct mount *mp = &mount[0];
	
			iflush(mp);
			/* remount root file system */
			srmountfun(0);
		}
		break;

	default:
		u.u_error = EINVAL;
	}
	ualock = 0;
}

/*
 * Machine dependent code to reboot.
 *  No return from mdboot expected.
 */
mdboot(fcn, mdep)
{
	splhi();
	switch (fcn) {
	case AD_HALT:
		cmn_err(CE_NOTE,"System Halt Requested (%d)",mdep);
		cmn_err(CE_NOTE,"System secured for powering down.");
		break;

	case AD_BOOT:
		cmn_err(CE_NOTE,"System Reboot Requested (%d)",mdep);
		cmn_err(CE_NOTE,"System secured for RESET.");
		break;

	case AD_IBOOT:
		cmn_err(CE_NOTE,"Return to Firmware Requested (%d)",mdep);
		cmn_err(CE_NOTE,"System secured for RESET.");
		break;

	default:
		cmn_err(CE_PANIC,"mdboot: Unknown function code.\n");
	}

#ifdef UPS
	/* code to shutdown Safe Ups */
	if (ups_tp) {
		register bucko = 0x1000000;
		cmn_err(CE_NOTE,"Removing Safe Power Control.");
		while(--bucko);
		(*ups_tp->t_proc)(ups_tp,T_MODEM_OFF);
	}
#endif
#ifdef opus
	syscmd(1, fcn);		/* PC_DOSRTN */
	/* NO RETURN */
#else
	while(1);
#endif
}
