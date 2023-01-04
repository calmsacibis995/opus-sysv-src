h63420
s 00169/00000/00000
d D 1.1 90/03/06 12:28:50 root 1 0
c date and time created 90/03/06 12:28:50 by root
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

#ident	"@(#)kern-port:io/mem.c	10.6"

/*
 *	Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel memory
 *	minor device 2 is EOF/NULL
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/immu.h"

mmread(dev)
{
	register unsigned n;
	register pde_t	  *mempde;
	register caddr_t  memvad;
	register c;
	register pde_t *pd;

	dev = minor(dev);
	switch (dev) {

	case (0):
		memvad = (caddr_t) sptalloc(1, 0, -1);
		mempde = (pde_t *) kvtokptbl(memvad);

		while ((u.u_error == 0) && (u.u_count != 0)) {
			pd = kvtopde(u.u_offset);
			if(!pg_tstvalid(pd)) {
				u.u_error = ENXIO;
				goto out;
			}
			n = min(u.u_count, ctob(1));
			pg_setpfn(mempde, btoct(u.u_offset));
			pg_setvalid(mempde);
			mmuflush();
			c = poff(u.u_offset);
			n = min(n, NBPP-c);
			/* Use short copyout for short I/O space */
			if (short_copyout(&memvad[c], u.u_base, n)) {
				u.u_error = ENXIO;
				goto out;
			}
			u.u_offset += n;
			u.u_base += n;
			u.u_count -= n;
		}
out:
		sptfree(memvad, 1, 0);
		break;

	case (1):
		pd = kvtopde(u.u_offset);
		if(!pg_tstvalid(pd)) {
			u.u_error = ENXIO;
			return;
		}
		pd = kvtopde(u.u_offset+u.u_count);
		if(!pg_tstvalid(pd)) {
			u.u_error = ENXIO;
			return;
		}
		/* Use short copyout for short I/O space */
		if (short_copyout(u.u_offset, u.u_base, u.u_count)) {
			u.u_error = ENXIO;
			return;
		}
		u.u_offset += u.u_count;
		u.u_base += u.u_count;
		u.u_count = 0;
		/* drop thru */

	case (2):
		break;

	case (3):
		symread();
		break;;

	default:
		u.u_error = ENODEV;
		break;
	}
}

mmwrite(dev)
{
	register unsigned n;
	register c;
	register pde_t	  *mempde;
	register caddr_t  memvad;
	register pde_t *pd;

	dev = minor(dev);
	switch (dev) {

	case (0):
		memvad = (caddr_t) sptalloc(1, 0, -1);
		mempde = (pde_t *) kvtokptbl(memvad);

		while ((u.u_error == 0) && (u.u_count != 0)) {
			pd = kvtopde(u.u_offset);
			if(!pg_tstvalid(pd)) {
				u.u_error = ENXIO;
				goto out;
			}
			n = min(u.u_count, ctob(1));
			pg_setpfn(mempde, btoct(u.u_offset));
			pg_setvalid(mempde);
			mmuflush();
			c = poff(u.u_offset);
			n = min(n, NBPP-c);
			/* Use short copyin for short I/O space */
			if (short_copyin(u.u_base, &memvad[c], n)) {
				u.u_error = ENXIO;
				goto out;
			}
			u.u_offset += n;
			u.u_base += n;
			u.u_count -= n;
		}
out:
		sptfree(memvad, 1, 0);
		break;

	case (1):	/* Beware of kernel read-only text */
		pd = kvtopde(u.u_offset);
		if(!pg_tstvalid(pd) || pd->pgm.pg_wp) {
			u.u_error = ENXIO;
			return;
		}
		pd = kvtopde(u.u_offset+u.u_count);
		if(!pg_tstvalid(pd) || pd->pgm.pg_wp) {
			u.u_error = ENXIO;
			return;
		}
		/*  Use short copyin for short I/O space   */
		if (short_copyin(u.u_base, u.u_offset, u.u_count)) {
			u.u_error = ENXIO;
			return;
		}
		/* drop thru */

	case (2):
		u.u_offset += u.u_count;
		u.u_base   += u.u_count;
		u.u_count  = 0;
		break;

	default:
		u.u_error = ENODEV;
		break;
	}
}
E 1
