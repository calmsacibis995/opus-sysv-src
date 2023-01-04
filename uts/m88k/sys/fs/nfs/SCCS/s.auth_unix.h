h01603
s 00068/00000/00000
d D 1.1 90/03/06 12:31:23 root 1 0
c date and time created 90/03/06 12:31:23 by root
e
u
U
t
T
I 1
#ifndef _NFS_AUTH_UNIX_H_
#define _NFS_AUTH_UNIX_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)auth_unix.h 1.3 84/12/22 SMI      */
/*      @(#)auth_unix.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*
 * auth_unix.h, Protocol for UNIX style authentication parameters for RPC
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * The system is very weak.  The client uses no encryption for  it
 * credentials and only sends null verifiers.  The server sends backs
 * null verifiers or optionally a verifier that suggests a new short hand
 * for the credentials.
 */

/* The machine name is part of a credential; it may not exceed 255 bytes */
#define MAX_MACHINE_NAME 255

/* gids compose part of a credential; there may not be more than 10 of them */
#define NGRPS 8

#ifndef NOGROUP
#define NOGROUP	-1
#endif

/*
 * Unix style credentials.
 */
struct authunix_parms {
	ulong	 aup_time;
	char	*aup_machname;
	int	 aup_uid;
	int	 aup_gid;
	uint	 aup_len;
	int	*aup_gids;
};

extern bool_t xdr_authunix_parms();

/* 
 * If a response verifier has flavor AUTH_SHORT, 
 * then the body of the response verifier encapsulates the following structure;
 * again it is serialized in the obvious fashion.
 */
struct short_hand_verf {
	struct opaque_auth new_cred;
};

#endif	/* ! _NFS_AUTH_UNIX_H_ */
E 1
