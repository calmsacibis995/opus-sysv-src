h10918
s 00129/00000/00000
d D 1.1 90/03/06 12:43:28 root 1 0
c date and time created 90/03/06 12:43:28 by root
e
u
U
t
T
I 1
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
/*	Copyright (c) 1985 Sun Microsystems Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_kprot.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif
/*      @(#)kern_prot.c 1.1 85/12/18 SMI; from UCB 5.17 83/05/27        */

/*
 * Routines to handle credentials.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"

#include "sys/fs/nfs/ucred.h"

extern caddr_t kmem_alloc();

/*
 * Routines to allocate and free credentials structures
 */

int cractive = 0;

struct credlist {
	union {
		struct ucred cru_cred;
		struct credlist *cru_next;
	} cl_U;
#define	cl_cred	cl_U.cru_cred
#define	cl_next	cl_U.cru_next
};

struct credlist *crfreelist = NULL;

/*
 * Allocate a zeroed cred structure and crhold it.
 */
struct ucred *
crget()
{
	register struct ucred *cr;

	cractive++;
	if (crfreelist) {
		cr = &crfreelist->cl_cred;
		crfreelist = ((struct credlist *)cr)->cl_next;
	} else {
		cr = (struct ucred *)kmem_alloc((uint)sizeof(*cr));
	}
	bzero((caddr_t)cr, sizeof(*cr));
	crhold(cr);
	return(cr);
}

/*
 * Free a cred structure.
 * Throws away space when ref count gets to 0.
 */
crfree(cr)
	struct ucred *cr;
{
	int s;

	s = spl6();
	if (--cr->cr_ref != 0) {
		(void) splx(s);
		return;
	}
	((struct credlist *)cr)->cl_next = crfreelist;
	crfreelist = (struct credlist *)cr;
	cractive--;
	(void) splx(s);
}

/*
 * Copy cred structure to a new one and free the old one.
 */
struct ucred *
crcopy(cr)
	struct ucred *cr;
{
	struct ucred *newcr;

	newcr = crget();
	*newcr = *cr;
	crfree(cr);
	newcr->cr_ref = 1;
	return(newcr);
}

/*
 * Dup cred struct to a new held one.
 */
struct ucred *
crdup(cr)
	struct ucred *cr;
{
	struct ucred *newcr;

	newcr = crget();
	*newcr = *cr;
	newcr->cr_ref = 1;
	return(newcr);
}
E 1
