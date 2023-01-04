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
/*	Copyright (C) 1984, Sun Microsystems, Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)au_ux_prot.c	4.1 LAI System V NFS Release 3.2/V3      source";
#endif
/*
 * static char sccsid[] = "@(#)authunix_prot.c 1.1 85/12/18 Copyr 1984 Sun Micro";
 */

/*
 * XDR for UNIX style authentication parameters for RPC
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
#include "sys/utsname.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/auth_unix.h"

/*
 * XDR for unix authentication parameters.
 */
bool_t
xdr_authunix_parms(xdrs, p)
	register XDR *xdrs;
	register struct authunix_parms *p;
{

	if (xdr_u_long(xdrs, &(p->aup_time)) &&
	    xdr_string(xdrs, &(p->aup_machname), MAX_MACHINE_NAME) &&
	    xdr_int(xdrs, &(p->aup_uid)) &&
	    xdr_int(xdrs, &(p->aup_gid)) &&
	    xdr_array(xdrs, (caddr_t *)&(p->aup_gids), &(p->aup_len), NGRPS,
		      sizeof(int), xdr_int)) {
		return(TRUE);
	}
	return(FALSE);
}

/*
 * XDR kernel unix auth parameters.
 * Goes out of the u struct directly.
 * NOTE: this is an XDR_ENCODE only routine.
 */
xdr_authkern(xdrs)
	register XDR *xdrs;
{
#ifdef multiple_groups
	register int *gp;
#endif
	int uid = u.u_uid;
	int gid = u.u_gid;
	int len;
	caddr_t groups;
	char *name;
	char hostname[sizeof(utsname.nodename) + 1];

	if (xdrs->x_op != XDR_ENCODE) {
		return(FALSE);
	}

	bcopy(utsname.nodename, hostname, sizeof(utsname.nodename));
	hostname[sizeof(utsname.nodename)] = '\0';
	name = hostname;
#ifdef multiple_groups
	gp = u.u_groups;
	while (gp < &u.u_groups[NGRPS] && *gp != NOGROUP)
		gp++;
	len = gp - u.u_groups;
	groups = (caddr_t)u.u_groups;
#else
	len = 1;
	groups = (caddr_t)&gid;
#endif
	if (xdr_u_long(xdrs, (ulong *)&time) &&
	    xdr_string(xdrs, &name, MAX_MACHINE_NAME) &&
	    xdr_int(xdrs, &uid) &&
	    xdr_int(xdrs, &gid) &&
	    xdr_array(xdrs, &groups, (uint *)&len, NGRPS, sizeof(int),
		      xdr_int)) {
		return(TRUE);
	}
	return(FALSE);
}
