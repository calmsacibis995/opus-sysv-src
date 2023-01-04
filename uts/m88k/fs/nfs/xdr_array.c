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
static char SysVr3NFSID[] = "@(#)xdr_array.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif
/*
 * static char sccsid[] = "@(#)xdr_array.c 1.3 85/02/26 Copyr 1984 Sun Micro";
 */

/*
 * Generic XDR routines impelmentation.
 *
 * These are the "non-trivial" xdr primitives used to serialize and de-serialize
 * arrays.  See xdr.h for more info on the interface to xdr.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/cmn_err.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#define LASTUNSIGNED	((uint)0-1)

/*
 * XDR an array of arbitrary elements
 * *addrp is a pointer to the array, *sizep is the number of elements.
 * If addrp is NULL (*sizep * elsize) bytes are allocated.
 * elsize is the size (in bytes) of each element, and elproc is the
 * xdr procedure to call to handle each element of the array.
 */
bool_t
xdr_array(xdrs, addrp, sizep, maxsize, elsize, elproc)
	register XDR *xdrs;
	caddr_t *addrp;		/* array pointer */
	uint *sizep;		/* number of elements */
	uint maxsize;		/* max numberof elements */
	uint elsize;		/* size in bytes of each element */
	xdrproc_t elproc;	/* xdr routine to handle each element */
{
	register uint i;
	register caddr_t target = *addrp;
	register uint c;  /* the actual element count */
	register bool_t stat = TRUE;
	register int nodesize;

	/* like strings, arrays are really counted arrays */
	if (!xdr_u_int(xdrs, sizep)) {
		cmn_err(CE_WARN, "xdr_array: size FAILED");
		return(FALSE);
	}
	c = *sizep;
	if ((c > maxsize) && (xdrs->x_op != XDR_FREE)) {
		cmn_err(CE_WARN, "xdr_array: bad size FAILED");
		return(FALSE);
	}
	nodesize = c * elsize;

	/*
	 * if we are deserializing, we may need to allocate an array.
	 * We also save time by checking for a null array if we are freeing.
	 */
	if (target == NULL)
		switch (xdrs->x_op) {
		case XDR_DECODE:
			if (c == 0)
				return(TRUE);
			*addrp = target = kmem_alloc((uint)nodesize);
			bzero(target, (uint)nodesize);
			break;

		case XDR_FREE:
			return(TRUE);
	}

	/*
	 * now we xdr each element of array
	 */
	for (i = 0; (i < c) && stat; i++) {
		stat = (*elproc)(xdrs, target, LASTUNSIGNED);
		target += elsize;
	}

	/*
	 * the array may need freeing
	 */
	if (xdrs->x_op == XDR_FREE) {
		kmem_free(*addrp, (uint)nodesize);
		*addrp = NULL;
	}
	return(stat);
}
