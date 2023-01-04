h42611
s 00001/00000/00298
d D 1.2 90/03/19 16:27:58 root 3 1
c return old kdebug value wher setting a new value.
e
s 00002/00000/00298
d R 1.2 90/03/19 08:22:28 root 2 1
c return old value of kdebug on debug calls
e
s 00298/00000/00000
d D 1.1 90/03/06 12:28:18 root 1 0
c date and time created 90/03/06 12:28:18 by root
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

#ident	"@(#)kern-port:os/sysm68k.c	1.0"

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/sys_local.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/systm.h"
#include "sys/utsname.h"
#include "sys/swap.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/reg.h"
#include "sys/psw.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/var.h"

char *saveflag = &u.u_trapflag;		/* Flag to save/restore-the-frame */
long *saveaddr = &u.u_trapsave[0];	/* Area to save-the-frame in	*/

#define	MAXSCALL	5
int scflag[MAXSCALL];		/* MAXSCALL * 32 calls */

endtrace(call)
register call;
{
	register off, mask;
	register kd;

	kd = kdebug;

	/* no debug */
	if (!(kd & 6) && !(u.u_procp->p_compat & (STRALL|STRERRS)))
		return 0;

	/* errors only */
	if (((kd & 6) == 4 ||
			(u.u_procp->p_compat & (STRALL|STRERRS)) == STRERRS) &&
			u.u_error == 0)
		return 0; 

	/* trace all */
	if (kd & 6) 
		return 1;

	/* per process trace */
	off = sigoff(call);	/* this works here also */
	mask = sigmask(call);

	return(scflag[off] & mask);
}

begintrace(call)
register call;
{
	register off, mask;

	/* no debug */
	if (!(kdebug & 2) && !(u.u_procp->p_compat & (STRALL)))
		return 0;

	/* trace all */
	if (kdebug & 2)
		return 1;

	off = sigoff(call);	/* this works here also */
	mask = sigmask(call);

	return(scflag[off] & mask);
}

/*
 * syslocal - system call
 */


syslocal()
{
	struct a {
		int cmd;
		int arg1;
		int arg2;
		int arg3;
	} *uap = (struct a *) u.u_ap;
	extern int knochange;

	if ( (uap->cmd&0xff) == SOCKFUNC) {
		bsdfunc((uap->cmd >> 8) & 0xff);
		return;
	}

	switch (uap->cmd) {

	case SUSERMAP:
		usermap(uap->arg1, uap->arg2);
		break;

	case SUSERUNMAP:
		userunmap(uap->arg1, uap->arg2, uap->arg3);
		break;

	case S88CONT: /* instruction continuation after signal handlers */
		sigret();
		break;

	case S88STIME:	/* set internal time, not hardware clock */
		if (suser())
			time = (time_t) uap->arg1;
		break;
 
	case S88SETNAME:	/* rename the system */
	{
		register i, c;

		char	sysnamex[sizeof(utsname.sysname)];

		if (!suser())
			break;

		for (i = 0; (c = fubyte((caddr_t) uap->arg1 + i)) > 0
		    && i < sizeof(sysnamex) - 1; ++i)
			sysnamex[i] = c;

		if (c) {
			u.u_error = c < 0 ? EFAULT : EINVAL;
			break;
		}
		sysnamex[i] = '\0';
		bzero(utsname.sysname, sizeof(utsname.sysname));
		bzero(utsname.nodename, sizeof(utsname.nodename));
		bcopy(sysnamex, utsname.sysname, i);
		bcopy(sysnamex, utsname.nodename, i);
		break;
	} 

	case S88SWAP:
	/*	General interface for adding, deleting, or
	**	finding out about swap files.  See swap.h
	**	for a description of the argument.
	**/
	{
		swpi_t	swpbuf;

		if (copyin(uap->arg1, &swpbuf, sizeof(swpi_t)) < 0) {
			u.u_error = EFAULT;
			return;
		}
		swapfunc(&swpbuf);
		break;
	}

	case S88DELMEM:
	/*	Delete memory from the available list.  Forces
	**	tight memory situation for load testing.
	*/
		if (uap->arg2 != 0x12345678) {
			u.u_error = EINVAL;
			break;
		}
		if (suser())
			delmem(uap->arg1);
		break;

	case S88ADDMEM:		/* Add back previously deleted memory */
		if (uap->arg2 != 0x12345678) {
			u.u_error = EINVAL;
			break;
		}
		if (suser())
			addmem(uap->arg1);
		break;

	case S88DEB:
		{
		register i;
		register struct proc *p;

		if (!suser()) 
			return;

		switch(uap->arg1) {
			case 0:	/* set/reset all */
				if (uap->arg2)
					uap->arg2 = -1;
				for (i = 0; i < MAXSCALL; i++)
					scflag[i] = uap->arg2;
				if (uap->arg2)
					break;
				for (p= &proc[3];p<(struct proc *)v.ve_proc;p++)
					p->p_compat &= ~(STRALL|STRERRS);
				break;

			case 1:	/* selective system calls */
				i = sigoff(uap->arg2);
				scflag[i] |= sigmask(uap->arg2);
				break;

			case 2:	/* kdebug */
I 3
				u.u_rval1 = kdebug;	/* return old kdebug */
E 3
				uap->arg2 &= ~(knochange);/* all but sys opts */
				kdebug &= knochange;
				kdebug |= uap->arg2;
				break;

			case 3:
				if (uap->arg2 == 0) {
					p = u.u_procp;
					uap->arg2 = p->p_pid;
				}
				else {
					for (p= &proc[3];p<(struct proc *)v.ve_proc;p++)
						if (uap->arg2 == p->p_pid)
							break;
				}
				if (uap->arg2 != p->p_pid) {
					u.u_error = ESRCH;
					return;
				}
				switch(uap->arg3) {
					case 0:
						p->p_compat &= ~(STRALL|STRERRS);
						return;
					case 1:
						i = STRALL;
						break;
					case 2:
						i = STRERRS;
						break;
					case 3:
						i = (STRALL|STRERRS);
						break;
					default:
						return;
					}
				p->p_compat |= i;
				break;
			default:
				u.u_error = EINVAL;
				break;
			}
		}
		break;

	case S88FIXALIGN:
		u.u_procp->p_compat |= SALIGN;
		break;

	case S88NIXALIGN:
		u.u_procp->p_compat &= ~SALIGN;
		break;

	case S88RECALIGN:
		{
		register long *p;
		p = (long *) u.u_filler;
		if ((uap->arg1 & 3) || (uap->arg2 & 3)) {
			u.u_error = EINVAL;
			break;
			}
		u.u_procp->p_compat |= SRECALN;
		if (uap->arg1) {
			p[0] = uap->arg1;	/* begin buffer */
			p[1] = uap->arg2;	/* end buffer */
			p[2] = uap->arg1;	/* last value not written */
			}
		}
		break;

	case S88NORECALIGN:
		{
		register long *p;
		p = (long *) u.u_filler;
		u.u_rval1 = p[2];	/* return last value not written */
		u.u_procp->p_compat &= ~SRECALN;
		}
		break;

	case S88NOCORE:
		u.u_procp->p_compat |= SNOCORE;
		break;

	case S88CORE:
		u.u_procp->p_compat &= ~SNOCORE;
		break;

	default:
		u.u_error = EINVAL;
	}
}
E 1
