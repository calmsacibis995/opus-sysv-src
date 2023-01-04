h55977
s 00039/00000/00000
d D 1.1 90/03/06 12:31:37 root 1 0
c date and time created 90/03/06 12:31:37 by root
e
u
U
t
T
I 1
#ifndef _NFS_TYPES_H_
#define _NFS_TYPES_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)types.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*      @(#)types.h 1.9 85/02/25 SMI      */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Rpc additions to <sys/types.h>
 */

#define	bool_t	int
#define	enum_t	int
#ifdef FALSE
#undef FALSE
#endif
#define	FALSE	(0)
#ifdef TRUE
#undef TRUE
#endif
#define	TRUE	(1)
#define __dontcare__	-1

#endif	/* ! _NFS_TYPES_H_ */
E 1
