h40161
s 00160/00000/00000
d D 1.1 90/03/06 12:31:22 root 1 0
c date and time created 90/03/06 12:31:22 by root
e
u
U
t
T
I 1
#ifndef _NFS_AUTH_H_
#define _NFS_AUTH_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/* Copyright (C) 1984, Sun Microsystems, Inc.	*/
/*      @(#)auth.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*
 * Some of this module was taken from Convergent's auth.h:
 *
 *	Convergent Technologies - System V - Nov 1986
 * #ident	"@(#)auth.h	1.3 :/source/uts/common/sys/fs/nfs/s.auth.h 1/12/87 19:38:04"
 *      @(#)auth.h	2.1 System V NFS  source
 *      @(#)auth.h 1.10 85/05/02 SMI
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif


/*
 * auth.h, Authentication interface.
 *
 * The data structures are completely opaque to the client.  The client
 * is required to pass a AUTH * to routines that create rpc
 * "sessions".
 */

#define MAX_AUTH_BYTES	400

/*
 * Status returned from authentication check
 */
enum auth_stat {
	AUTH_OK=0,
	/*
	 * failed at remote end
	 */
	AUTH_BADCRED=1,			/* bogus credentials (seal broken) */
	AUTH_REJECTEDCRED=2,		/* client should begin new session */
	AUTH_BADVERF=3,			/* bogus verifier (seal broken) */
	AUTH_REJECTEDVERF=4,		/* verifier expired or was replayed */
	AUTH_TOOWEAK=5,			/* rejected due to security reasons */
	/*
	 * failed locally
	*/
	AUTH_INVALIDRESP=6,		/* bogus response verifier */
	AUTH_FAILED=7			/* some unknown reason */
};


union des_block {
	struct {
		ulong high;
		ulong low;
	} key;
	char c[8];
};


/*
 * Authentication info.  Opaque to client.
 */
struct opaque_auth {
	enum_t	oa_flavor;		/* flavor of auth */
	caddr_t	oa_base;		/* address of more auth stuff */
	uint	oa_length;		/* not to exceed MAX_AUTH_BYTES */
};


/*
 * Auth handle, interface to client side authenticators.
 */
typedef struct {
	struct	opaque_auth	ah_cred;
	struct	opaque_auth	ah_verf;
	union	des_block	ah_key;
	struct auth_ops {
		void	(*ah_nextverf)();
		int	(*ah_marshal)();	/* nextverf & serialize */
		int	(*ah_validate)();	/* validate varifier */
		int	(*ah_refresh)();	/* refresh credentials */
		void	(*ah_destroy)();	/* destroy this structure */
	} *ah_ops;
	caddr_t ah_private;
} AUTH;


/*
 * Authentication ops.
 * The ops and the auth handle provide the interface to the authenticators.
 *
 * AUTH	*auth;
 * XDR	*xdrs;
 * struct opaque_auth verf;
 */
#define AUTH_NEXTVERF(auth)		\
		((*((auth)->ah_ops->ah_nextverf))(auth))
#define auth_nextverf(auth)		\
		((*((auth)->ah_ops->ah_nextverf))(auth))

#define AUTH_MARSHALL(auth, xdrs)	\
		((*((auth)->ah_ops->ah_marshal))(auth, xdrs))
#define auth_marshall(auth, xdrs)	\
		((*((auth)->ah_ops->ah_marshal))(auth, xdrs))

#define AUTH_VALIDATE(auth, verfp)	\
		((*((auth)->ah_ops->ah_validate))((auth), verfp))
#define auth_validate(auth, verfp)	\
		((*((auth)->ah_ops->ah_validate))((auth), verfp))

#define AUTH_REFRESH(auth)		\
		((*((auth)->ah_ops->ah_refresh))(auth))
#define auth_refresh(auth)		\
		((*((auth)->ah_ops->ah_refresh))(auth))

#define AUTH_DESTROY(auth)		\
		((*((auth)->ah_ops->ah_destroy))(auth))
#define auth_destroy(auth)		\
		((*((auth)->ah_ops->ah_destroy))(auth))


extern struct opaque_auth _null_auth;


/*
 * These are the various implementations of client side authenticators.
 */

/*
 * Unix style authentication
 * AUTH *authunix_create(machname, uid, gid, len, aup_gids)
 *	char *machname;
 *	int uid;
 *	int gid;
 *	int len;
 *	int *aup_gids;
 */
#define	AUTH_NULL	0
#define	AUTH_UNIX	1		/* unix style (uid, gids) */
#define	AUTH_SHORT	2		/* short hand unix style */

#define authNULL	((AUTH *)0)	/* NULL auth handle */

#ifdef INKERNEL
extern AUTH *authkern_create();		/* takes no parameters */
#endif

#endif	/* ! _NFS_AUTH_H_ */
E 1
