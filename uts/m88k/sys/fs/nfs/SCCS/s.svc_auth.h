h56657
s 00035/00000/00000
d D 1.1 90/03/06 12:31:35 root 1 0
c date and time created 90/03/06 12:31:35 by root
e
u
U
t
T
I 1
#ifndef _NFS_SVC_AUTH_H_
#define _NFS_SVC_AUTH_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)svc_auth.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*      @(#)svc_auth.h 1.4 85/04/30 SMI      */

/*
 * svc_auth.h, Service side of rpc authentication.
 * 
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Server side authenticator
 */

extern enum auth_stat _authenticate();

#endif	/* ! _NFS_SVC_AUTH_H_ */
E 1
