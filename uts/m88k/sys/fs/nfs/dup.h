#ifndef _NFS_DUP_H_
#define _NFS_DUP_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)dup.h	4.1 LAI System V NFS Release 3.2/V3  source        */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * The format of a cache entry for the server side kernel udp
 * duplicate request cache.  Entries in the cache are identified
 * by the xid, address, and RPC triple of the request to be
 * checked for.  Entries in the cache are accessed via either
 * hash queues or a circular LRU list.
 */
struct dupreq {
	ulong		dr_xid;
	TADDR 		dr_addr;
	uint		dr_alen;
	ulong		dr_proc;
	ulong		dr_vers;
	ulong		dr_prog;
	int		dr_flags;
	int		dr_valid;
	struct dupreq	*dr_chain;
};

/*
 * Flag bits
 */
#define DR_BUSY		0x1
#define DR_WANTED	0x2

/*
 * The format of a hash bucket.
 */
struct duphash {
	struct dupreq	*dh_chain;
};

#ifdef INKERNEL
extern struct dupreq dupcache[];
extern int maxdupreqs;
extern struct duphash drhashtbl[];
extern int drhashsz;
#endif

#endif	/* ! _NFS_DUP_H_ */
