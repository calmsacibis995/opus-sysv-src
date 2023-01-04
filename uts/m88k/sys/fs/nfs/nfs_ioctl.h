#ifndef _NFS_NFS_IOCTL_H_
#define _NFS_NFS_IOCTL_H_

/*
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

/*      @(#)nfs_ioctl.h	4.2 LAI System V NFS Release 3.2/V3  source        */

/*
 * The various ioctls understood by the NFS pseudo-device.
 */

#define NIOCNFSD	1
#define NIOCGETFH	2
#define NIOCASYNCD	3
#define NIOCSETDOMNAM	4
#define NIOCGETDOMNAM	5
#define NIOCCLNTHAND	6

/*
 * The argument to the nfs_svc(2) system call.
 */
struct nfssvc_args {
	int fd;
	int addr;
	int tsdu;
	char *buf;
};

/*
 * The argument to the getfh(2) system call.
 */
struct getfh_args {
	int fd;
	fhandle_t *fhp;
};

/*
 * The argument to either the setdomainname(2) or getdomainname(2)
 * system calls.
 */
struct domnam_args {
	char *name;
	int namelen;
};

/*
 * The argument to the kclt_create(2) system call.
 */
struct kclt_create_args {
	int nfds;
	int *fds;
	int trans;
	int tsdu;
	ulong pgm;
	ulong vers;
};

#endif	/* ! _NFS_NFS_IOCTL_H_ */
