#ifndef _NFS_UCRED_H_
#define _NFS_UCRED_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)ucred.h	4.1 LAI System V NFS Release 3.2/V3	source        */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

/*
 * Unix credentials.
 */

#ifndef NGROUPS
#define NGROUPS	8
#endif
#ifndef NOGROUP
#define NOGROUP	-1
#endif

struct	ucred	{
	ushort	cr_ref;			/* reference count */
	short	cr_uid;			/* effective uid */
	short	cr_gid;			/* effective gid */
	int	cr_groups[NGROUPS];	/* groups */
	short	cr_ruid;		/* real uid */
	short	cr_rgid;		/* real gid */
};

#ifdef INKERNEL
#define	crhold(cr)	(cr)->cr_ref++

extern struct ucred *crget();
extern struct ucred *crcopy();
extern struct ucred *crdup();
#endif /* INKERNEL */

#endif	/* ! _NFS_UCRED_H_ */
