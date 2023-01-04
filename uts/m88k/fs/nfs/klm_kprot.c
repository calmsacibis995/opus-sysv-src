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
/* @(#)klm_kprot.c	1.2 86/12/09 NFSSRC */
/*     Copyright (C) 1984, Sun Microsystems, Inc.  */
#ifndef lint
static char SysVr3NFSID[] = "@(#)klm_kprot.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * XDR routines for Kernel<->Lock-Manager communication
 *
 * Generated from ../rpcsvc/klm_prot.x (but changed by hand for 5.3 NFS)
 */

#include "sys/types.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/klm_prot.h"

bool_t
xdr_klm_stats(xdrs, objp)
	XDR *xdrs;
	klm_stats *objp;
{

	if (!xdr_enum(xdrs, (enum_t *)objp)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_lock(xdrs, objp)
	XDR *xdrs;
	klm_lock *objp;
{

	if (!xdr_string(xdrs, &objp->server_name, LM_MAXSTRLEN)) {
		return(FALSE);
	}
	if (!xdr_netobj(xdrs, &objp->fh)) {
		return(FALSE);
	}
	if (!xdr_int(xdrs, &objp->pid)) {
		return(FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_offset)) {
		return(FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_len)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_holder(xdrs, objp)
	XDR *xdrs;
	klm_holder *objp;
{

	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return(FALSE);
	}
	if (!xdr_int(xdrs, &objp->svid)) {
		return(FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_offset)) {
		return(FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->l_len)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_stat(xdrs, objp)
	XDR *xdrs;
	klm_testrply *objp;
{

	if (!xdr_klm_stats(xdrs, &objp->stat)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_testrply(xdrs, objp)
	XDR *xdrs;
	klm_testrply *objp;
{
	static struct xdr_discrim choices[] = {
		{ (int) klm_granted, xdr_void },
		{ (int) klm_denied, xdr_klm_holder },
		{ (int) klm_denied_nolocks, xdr_void },
		{ (int) klm_working, xdr_void },
		{ __dontcare__, NULL_xdrproc_t }
	};

	if (!xdr_union(xdrs, (enum_t *)&objp->stat,
		       (char *)&objp->klm_testrply, choices,
		       NULL_xdrproc_t)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_lockargs(xdrs, objp)
	XDR *xdrs;
	klm_lockargs *objp;
{

	if (!xdr_bool(xdrs, &objp->block)) {
		return(FALSE);
	}
	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return(FALSE);
	}
	if (!xdr_klm_lock(xdrs, &objp->lock)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_testargs(xdrs, objp)
	XDR *xdrs;
	klm_lockargs *objp;
{

	if (!xdr_bool(xdrs, &objp->exclusive)) {
		return(FALSE);
	}
	if (!xdr_klm_lock(xdrs, &objp->lock)) {
		return(FALSE);
	}
	return(TRUE);
}

bool_t
xdr_klm_unlockargs(xdrs, objp)
	XDR *xdrs;
	klm_lockargs *objp;
{

	if (!xdr_klm_lock(xdrs, &objp->lock)) {
		return(FALSE);
	}
	return(TRUE);
}
