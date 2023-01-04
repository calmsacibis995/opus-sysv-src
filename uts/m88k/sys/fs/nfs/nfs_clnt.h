#ifndef _NFS_NFS_CLNT_H_
#define _NFS_NFS_CLNT_H_

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*      @(#)nfs_clnt.h	4.2 LAI System V NFS Release 3.2/V3	source        */

/*      @(#)nfs_clnt.h 2.11 85/09/25 SMI      */

/*
 * inode pointer to mount info
 */
#define	itomi(ip)	((struct mntinfo *)(((ip)->i_mntdev)->m_bufp))

/*
 * Inode pointer to server's block size
 */
#define	itoblksz(ip)	(itomi(ip)->mi_bsize)

#define	HOSTNAMESZ	32

/*
 * NFS private data per mounted file system
 */
struct mntinfo {
	caddr_t		mi_addr;	/* server's address */
	uint		mi_addrlen;	/* size of server's address */
	uint		mi_hard : 1;	/* hard or soft mount */
	uint		mi_printed : 1;	/* not responding message printed */
#ifndef	OLDLOCK
	uint		mi_nomand : 1;	/* no i/o on mandatory mode files */
#endif
	long		mi_tsize;	/* transfer size (bytes) */
	long		mi_stsize;	/* server's max transfer size (bytes) */
	long		mi_bsize;	/* server's disk block size */
	int		mi_timeo;	/* inital timeout in 10th sec */
	int		mi_retrans;	/* times to retry request */
	char		mi_hostname[HOSTNAMESZ];	/* server name */
};

/*
 * enum to specifiy cache flushing action when file data is stale
 */
enum staleflush	{NOFLUSH, SFLUSH};

#endif	/* ! _NFS_NFS_CLNT_H_ */
