h31349
s 00515/00000/00000
d D 1.1 90/03/06 12:43:37 root 1 0
c date and time created 90/03/06 12:43:37 by root
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
/*	Copyright (C) 1984, Sun Microsystems, Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)xdr.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif
/*
 * static char sccsid[] = "@(#)xdr.c 1.16 86/01/13 Copyr 1984 Sun Micro";
 */

/*
 * Generic XDR routines implementation.
 *
 * These are the "generic" xdr routines used to serialize and de-serialize
 * most common data items.  See xdr.h for more info on the interface to
 * xdr.
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/cmn_err.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

/*
 * constants specific to the xdr "protocol"
 */
#define XDR_FALSE	((long) 0)
#define XDR_TRUE	((long) 1)
#define LASTUNSIGNED	((uint) 0-1)

/*
 * for unit alignment
 */
static char xdr_zero[BYTES_PER_XDR_UNIT] = { 0, 0, 0, 0 };

/*
 * XDR nothing
 */
/* ARGSUSED */
bool_t
xdr_void(xdrs, addr)
	XDR *xdrs;
	caddr_t addr;
{

	return(TRUE);
}

/*
 * XDR integers
 */
bool_t
xdr_int(xdrs, ip)
	XDR *xdrs;
	int *ip;
{

#ifdef lint1
	(void) (xdr_short(xdrs, (short *)ip));
	return(xdr_long(xdrs, (long *)ip));
#else
	if (sizeof(int) == sizeof(long)) {
		return(xdr_long(xdrs, (long *)ip));
	} else {
		return(xdr_short(xdrs, (short *)ip));
	}
#endif
}

/*
 * XDR unsigned integers
 */
bool_t
xdr_u_int(xdrs, up)
	XDR *xdrs;
	uint *up;
{

#ifdef lint1
	(void) (xdr_u_short(xdrs, (ushort *)up));
	return(xdr_u_long(xdrs, (ulong *)up));
#else
	if (sizeof(uint) == sizeof(ulong)) {
		return(xdr_u_long(xdrs, (ulong *)up));
	} else {
		return(xdr_u_short(xdrs, (ushort *)up));
	}
#endif
}

/*
 * XDR long integers
 * same as xdr_u_long - open coded to save a proc call!
 */
bool_t
xdr_long(xdrs, lp)
	register XDR *xdrs;
	long *lp;
{

	if (xdrs->x_op == XDR_ENCODE)
		return(XDR_PUTLONG(xdrs, lp));

	if (xdrs->x_op == XDR_DECODE)
		return(XDR_GETLONG(xdrs, lp));

	if (xdrs->x_op == XDR_FREE)
		return(TRUE);

	cmn_err(CE_WARN, "xdr_long: bad op FAILED");
	return(FALSE);
}

/*
 * XDR unsigned long integers
 * same as xdr_long - open coded to save a proc call!
 */
bool_t
xdr_u_long(xdrs, ulp)
	register XDR *xdrs;
	ulong *ulp;
{

	if (xdrs->x_op == XDR_DECODE)
		return(XDR_GETLONG(xdrs, (long *)ulp));

	if (xdrs->x_op == XDR_ENCODE)
		return(XDR_PUTLONG(xdrs, (long *)ulp));

	if (xdrs->x_op == XDR_FREE)
		return(TRUE);

	cmn_err(CE_WARN, "xdr_u_long: bad op FAILED");
	return(FALSE);
}

/*
 * XDR short integers
 */
bool_t
xdr_short(xdrs, sp)
	register XDR *xdrs;
	short *sp;
{
	long l;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
		l = (long)*sp;
		return(XDR_PUTLONG(xdrs, &l));

	case XDR_DECODE:
		if (!XDR_GETLONG(xdrs, &l)) {
			return(FALSE);
		}
		*sp = (short)l;
		return(TRUE);

	case XDR_FREE:
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_short: bad op FAILED");
	return(FALSE);
}

/*
 * XDR unsigned short integers
 */
bool_t
xdr_u_short(xdrs, usp)
	register XDR *xdrs;
	ushort *usp;
{
	ulong l;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
		l = (ulong)*usp;
		return(XDR_PUTLONG(xdrs, &l));

	case XDR_DECODE:
		if (!XDR_GETLONG(xdrs, &l)) {
			return(FALSE);
		}
		*usp = (ushort)l;
		return(TRUE);

	case XDR_FREE:
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_u_short: bad op FAILED");
	return(FALSE);
}

/*
 * XDR booleans
 */
bool_t
xdr_bool(xdrs, bp)
	register XDR *xdrs;
	bool_t *bp;
{
	long lb;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
		lb = *bp ? XDR_TRUE : XDR_FALSE;
		return(XDR_PUTLONG(xdrs, &lb));

	case XDR_DECODE:
		if (!XDR_GETLONG(xdrs, &lb)) {
			return(FALSE);
		}
		*bp = (lb == XDR_FALSE) ? FALSE : TRUE;
		return(TRUE);

	case XDR_FREE:
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_bool: bad op FAILED");
	return(FALSE);
}

/*
 * XDR enumerations
 */
bool_t
xdr_enum(xdrs, ep)
	XDR *xdrs;
	enum_t *ep;
{
	enum sizecheck { SIZEVAL };	/* used to find the size of an enum */

	/*
	 * enums are treated as ints
	 */
#ifdef lint1
	(void) (xdr_short(xdrs, (short *)ep));
	return(xdr_long(xdrs, (long *)ep));
#else
	if (sizeof(enum sizecheck) == sizeof(long)) {
		return(xdr_long(xdrs, (long *)ep));
	} else if (sizeof(enum sizecheck) == sizeof(short)) {
		return(xdr_short(xdrs, (short *)ep));
	} else {
		cmn_err(CE_WARN, "xdr_enum: can't handle enum");
		return(FALSE);
	}
#endif
}

/*
 * XDR opaque data
 * Allows the specification of a fixed size sequence of opaque bytes.
 * cp points to the opaque object and cnt gives the byte length.
 */
bool_t
xdr_opaque(xdrs, cp, cnt)
	register XDR *xdrs;
	caddr_t cp;
	register uint cnt;
{
	register uint rndup;
	static char crud[BYTES_PER_XDR_UNIT];

	/*
	 * if no data we are done
	 */
	if (cnt == 0)
		return(TRUE);

	/*
	 * round byte count to full xdr units
	 */
	rndup = cnt % BYTES_PER_XDR_UNIT;
	if (rndup > 0)
		rndup = BYTES_PER_XDR_UNIT - rndup;

	if (xdrs->x_op == XDR_DECODE) {
		if (!XDR_GETBYTES(xdrs, cp, cnt)) {
			cmn_err(CE_WARN, "xdr_opaque: decode FAILED");
			return(FALSE);
		}
		if (rndup == 0)
			return(TRUE);
		return(XDR_GETBYTES(xdrs, crud, rndup));
	}

	if (xdrs->x_op == XDR_ENCODE) {
		if (!XDR_PUTBYTES(xdrs, cp, cnt)) {
			cmn_err(CE_WARN, "xdr_opaque: encode FAILED");
			return(FALSE);
		}
		if (rndup == 0)
			return(TRUE);
		return(XDR_PUTBYTES(xdrs, xdr_zero, rndup));
	}

	if (xdrs->x_op == XDR_FREE) {
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_opaque: bad op FAILED");
	return(FALSE);
}

/*
 * XDR counted bytes
 * *cpp is a pointer to the bytes, *sizep is the count.
 * If *cpp is NULL maxsize bytes are allocated
 */
bool_t
xdr_bytes(xdrs, cpp, sizep, maxsize)
	register XDR *xdrs;
	char **cpp;
	register uint *sizep;
	uint maxsize;
{
	register char *sp = *cpp;  /* sp is the actual string pointer */
	register uint nodesize;

	/*
	 * first deal with the length since xdr bytes are counted
	 */
	if (!xdr_u_int(xdrs, sizep)) {
		cmn_err(CE_WARN, "xdr_bytes: size FAILED");
		return(FALSE);
	}
	nodesize = *sizep;
	if ((nodesize > maxsize) && (xdrs->x_op != XDR_FREE)) {
		cmn_err(CE_WARN, "xdr_bytes: bad size FAILED");
		return(FALSE);
	}

	/*
	 * now deal with the actual bytes
	 */
	switch (xdrs->x_op) {

	case XDR_DECODE:
		if (sp == NULL) {
			*cpp = sp = (char *)kmem_alloc(nodesize);
		}
		/* fall into ... */

	case XDR_ENCODE:
		return(xdr_opaque(xdrs, sp, nodesize));

	case XDR_FREE:
		if (sp != NULL) {
			kmem_free((caddr_t)sp, nodesize);
			*cpp = NULL;
		}
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_bytes: bad op FAILED");
	return(FALSE);
}

/*
 * XDR a net object
 */
bool_t
xdr_netobj(xdrs, np)
	XDR *xdrs;
	struct netobj *np;
{

	return(xdr_bytes(xdrs, &np->n_bytes, &np->n_len, MAX_NETOBJ_SZ));
}

/*
 * XDR a descriminated union
 * Support routine for discriminated unions.
 * You create an array of xdrdiscrim structures, terminated with
 * an entry with a null procedure pointer.  The routine gets
 * the discriminant value and then searches the array of xdrdiscrims
 * looking for that value.  It calls the procedure given in the xdrdiscrim
 * to handle the discriminant.  If there is no specific routine a default
 * routine may be called.
 * If there is no specific or default routine an error is returned.
 */
bool_t
xdr_union(xdrs, dscmp, unp, choices, dfault)
	register XDR *xdrs;
	enum_t *dscmp;		/* enum to decide which arm to work on */
	caddr_t unp;		/* the union itself */
	struct xdr_discrim *choices;	/* [value, xdr proc] for each arm */
	xdrproc_t dfault;	/* default xdr routine */
{
	register enum_t dscm;

	/*
	 * we deal with the discriminator;  it's an enum
	 */
	if (!xdr_enum(xdrs, dscmp)) {
		cmn_err(CE_WARN, "xdr_union: dscmp FAILED");
		return(FALSE);
	}
	dscm = *dscmp;

	/*
	 * search choices for a value that matches the discriminator.
	 * if we find one, execute the xdr routine for that value.
	 */
	for (; choices->proc != NULL_xdrproc_t; choices++) {
		if (choices->value == dscm)
			return((*(choices->proc))(xdrs, unp, LASTUNSIGNED));
	}

	/*
	 * no match - execute the default xdr routine if there is one
	 */
	return((dfault == NULL_xdrproc_t) ? FALSE :
					    (*dfault)(xdrs, unp, LASTUNSIGNED));
}

/*
 * Non-portable xdr primitives.
 * Care should be taken when moving these routines to new architectures.
 */

/*
 * XDR null terminated ASCII strings
 * xdr_string deals with "C strings" - arrays of bytes that are
 * terminated by a NULL character.  The parameter cpp references a
 * pointer to storage; If the pointer is null, then the necessary
 * storage is allocated.  The last parameter is the max allowed length
 * of the string as specified by a protocol.
 */
bool_t
xdr_string(xdrs, cpp, maxsize)
	register XDR *xdrs;
	char **cpp;
	uint maxsize;
{
	register char *sp = *cpp;  /* sp is the actual string pointer */
	bool_t sp_null;
	uint size;
	uint nodesize;
	uint realsize;

	/*
	 * first deal with the length since xdr strings are counted-strings
	 */
	if ((xdrs->x_op) != XDR_DECODE)
		size = nfs_strlen(sp);
	if (!xdr_u_int(xdrs, &size)) {
		cmn_err(CE_WARN, "xdr_string: size FAILED");
		return(FALSE);
	}
	if (size > maxsize) {
		cmn_err(CE_WARN, "xdr_string: bad size FAILED");
		return(FALSE);
	}
	nodesize = size + 1;

	/*
	 * now deal with the actual bytes
	 */
	switch (xdrs->x_op) {

	case XDR_DECODE:
		if (sp_null = (sp == NULL))
			*cpp = sp = (char *)kmem_alloc(nodesize);
		sp[size] = 0;
		if (!xdr_opaque(xdrs, sp, size))
			return(FALSE);
		if (sp_null && (realsize = nfs_strlen(sp)) < size) {
			kmem_free((caddr_t)(sp + realsize + 1), size - realsize);
		}
		return(TRUE);

	case XDR_ENCODE:
		return(xdr_opaque(xdrs, sp, size));

	case XDR_FREE:
		if (sp != NULL) {
			kmem_free((caddr_t)sp, nodesize);
			*cpp = NULL;
		}
		return(TRUE);
	}

	cmn_err(CE_WARN, "xdr_string: bad op FAILED");
	return(FALSE);
}
E 1
