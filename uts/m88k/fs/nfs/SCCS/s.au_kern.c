h56752
s 00217/00000/00000
d D 1.1 90/03/06 12:43:20 root 1 0
c date and time created 90/03/06 12:43:20 by root
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
/*	Copyright (C) 1984, Sun Microsystems, Inc. 	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)au_kern.c	4.1 LAI System V NFS Release 3.2/V3      source";
#endif
/*
 * static char sccsid[] = "@(#)auth_kern.c 1.15 85/04/30 Copyr 1984 Sun Micro";
 */

/*
 * Implements UNIX style authentication parameters in the kernel. 
 * Interfaces with svc_auth_unix on the server.
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

#include "sys/stream.h"
#include "bsd/netinet/in.h"
#include "sys/endian.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#define roundup(x, y)	((((x) + ((y) - 1)) / (y)) * (y))

/*
 * Unix authenticator operations vector
 */
void	authkern_nextverf();
bool_t	authkern_marshal();
bool_t	authkern_validate();
bool_t	authkern_refresh();
void	authkern_destroy();

static struct auth_ops auth_kern_ops = {
	authkern_nextverf,
	authkern_marshal,
	authkern_validate,
	authkern_refresh,
	authkern_destroy
};


/*
 * Create a kernel unix style authenticator.
 * Returns an auth handle.
 */
AUTH *
authkern_create()
{
	register AUTH *auth;

	/*
	 * Allocate and set up auth handle
	 */
	auth = (AUTH *)kmem_alloc((uint)sizeof(*auth));
	auth->ah_ops = &auth_kern_ops;
	auth->ah_verf = _null_auth;
	return(auth);
}

/*
 * authkern operations
 */
/* ARGSUSED */
void
authkern_nextverf(auth)
	AUTH *auth;
{

	/* no action necessary */
}

bool_t
authkern_marshal(auth, xdrs)
	AUTH *auth;
	XDR *xdrs;
{
	caddr_t sercred;
	XDR xdrm;
	struct opaque_auth *cred;
	bool_t ret = FALSE;
#ifdef multiple_groups
	register int *gp;
#endif
	register int gidlen, credsize;
	register long *ptr;
	char hostname[sizeof(utsname.nodename) + 1];
	int hostnamelen;

	/*
	 * First we try a fast path to get through
	 * this very common operation.
	 */
	bcopy(utsname.nodename, hostname, sizeof(utsname.nodename));
	hostname[sizeof(utsname.nodename)] = '\0';
	hostnamelen = nfs_strlen(hostname);
#ifdef multiple_groups
	gp = u.u_groups;
	while (gp < &u.u_groups[NGRPS] && *gp != NOGROUP)
		gp++;
	gidlen = gp - u.u_groups;
#else
	gidlen = 1;
#endif
	credsize = 4 + 4 + roundup(hostnamelen, 4) + 4 + 4 + 4 + gidlen * 4;
	ptr = XDR_INLINE(xdrs, 4 + 4 + credsize + 4 + 4);
	if (ptr) {
		/*
		 * We can do the fast path.
		 */
		IXDR_PUT_LONG(ptr, AUTH_UNIX);	/* cred flavor */
		IXDR_PUT_LONG(ptr, credsize);	/* cred len */
		IXDR_PUT_LONG(ptr, time);
		IXDR_PUT_LONG(ptr, hostnamelen);
		bcopy(hostname, (caddr_t)ptr, (uint)hostnamelen);
		ptr += roundup(hostnamelen, 4) / 4;
		IXDR_PUT_LONG(ptr, u.u_uid);
		IXDR_PUT_LONG(ptr, u.u_gid);
		IXDR_PUT_LONG(ptr, gidlen);
#ifdef multiple_groups
		while (--gp >= u.u_groups)
			IXDR_PUT_LONG(ptr, *gp);
#else
		IXDR_PUT_LONG(ptr, u.u_gid);
#endif
		IXDR_PUT_LONG(ptr, AUTH_NULL);	/* verf flavor */
		IXDR_PUT_LONG(ptr, 0);		/* verf len */
		return(TRUE);
	}
	sercred = kmem_alloc((uint)MAX_AUTH_BYTES);
	/*
	 * serialize u struct stuff into sercred
	 */
	xdrmem_create(&xdrm, sercred, MAX_AUTH_BYTES, XDR_ENCODE);
	if (!xdr_authkern(&xdrm)) {
		ret = FALSE;
		goto done;
	}

	/*
	 * Make opaque auth credentials that point at serialized u struct
	 */
	cred = &(auth->ah_cred);
	cred->oa_length = XDR_GETPOS(&xdrm);
	cred->oa_flavor = AUTH_UNIX;
	cred->oa_base = sercred;

	/*
	 * serialize credentials and verifiers (null)
	 */
	if ((xdr_opaque_auth(xdrs, &(auth->ah_cred)))
	    && (xdr_opaque_auth(xdrs, &(auth->ah_verf)))) {
		ret = TRUE;
	} else {
		ret = FALSE;
	}
done:
	kmem_free(sercred, (uint)MAX_AUTH_BYTES);
	return(ret);
}

/* ARGSUSED */
bool_t
authkern_validate(auth, verf)
	AUTH *auth;
	struct opaque_auth verf;
{

	return(TRUE);
}

/* ARGSUSED */
bool_t
authkern_refresh(auth)
	AUTH *auth;
{
}

void
authkern_destroy(auth)
	register AUTH *auth;
{

	kmem_free((caddr_t)auth, (uint)sizeof(*auth));
}
E 1
