#ifndef _NFS_MOUNT_H_
#define _NFS_MOUNT_H_

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)mount.h	4.1 LAI System V NFS Release 3.2/V3	source        */


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Some of this module was taken from Convergent's mount.h:
 *
 *	Convergent Technologies - System V - Nov 1986
 * #ident	"@(#)mount.h	1.3 :/source/uts/common/sys/fs/nfs/s.mount.h 1/12/87 19:38:18"
 *      @(#)mount.h	1.1 System V NFS  source
 */

/*
 * Get the nfs specific hidden args to mount system call.
 * From sun mount.h, Convergent modified to pass opaque addresses.
 */
struct nfs_args {
#if clipper || ns32000 || m88k
	struct sockaddr	addr;			/* file server address */
#else
	caddr_t		raddr;			/* file server address */
	uint		ralen;			/* file server address length */
#endif
	fhandle_t	fh;			/* File handle to be mounted */
	int		flags;			/* flags */
	int		wsize;			/* write size in bytes */
	int		rsize;			/* read size in bytes */
	int		timeo;			/* initial timeout in .1 secs */
	int		retrans;		/* times to retry send */
	char		hostname[HOSTNAMESZ+1];	/* server name */
};

/*
 * NFS mount option flags
 */
#define	NFSMNT_SOFT	0x001		/* soft mount (hard is default) */
#define	NFSMNT_WSIZE	0x002		/* set write size */
#define	NFSMNT_RSIZE	0x004		/* set read size */
#define	NFSMNT_TIMEO	0x008		/* set initial timeout */
#define	NFSMNT_RETRANS	0x010		/* set number of request retrys */
#define	NFSMNT_HOSTNAME	0x020		/* set hostname for error printf */

#define	NFSMNT_NOMAND	0x080		/* no i/o on mandatory lock files */

#endif	/* ! _NFS_MOUNT_H_ */
