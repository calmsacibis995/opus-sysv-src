#ifndef _NFS_DNLC_H_
#define _NFS_DNLC_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)dnlc.h	4.1 LAI System V NFS Release 3.2/V3  source        */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Directory name lookup cache.
 * Based on code originally done by Robert Els at Melbourne.
 *
 * Names found by directory scans are retained in a cache
 * for future reference.  It is managed LRU, so frequently
 * used names will hang around.  Cache is indexed by hash value
 * obtained from (dp, name) where the dp refers to the
 * directory containing the name.
 *
 * For simplicity (and economy of storage), names longer than
 * some (small) maximum length are not cached, they occur
 * infrequently in any case, and are almost never of interest.
 */

#define NC_NAMLEN	15	/* maximum name segment length we bother with*/

struct ncache {
	struct ncache	*hash_next, *hash_prev;	/* hash chain, MUST BE FIRST */
	struct ncache	*lru_next, *lru_prev;	/* LRU chain */
	struct rnode	*rp;			/* rnode the name refers to */
	struct rnode	*dp;			/* rnode of parent of name */
	char		namlen;			/* length of name */
	char		name[NC_NAMLEN];	/* segment name */
	struct ucred	cred;			/* credentials */
};

/*
 * Hash list of name cache entries for fast lookup.
 */
struct nc_hash {
	struct ncache *hash_next, *hash_prev;	/* hash chain */
};

/*
 * LRU list of cache entries for aging.
 */
struct nc_lru {
	struct ncache *hash_next, *hash_prev;	/* hash chain, unused */
	struct ncache *lru_next, *lru_prev;	/* LRU chain */
};

#ifdef INKERNEL
extern struct ncache ncache[];          /* the cache itself */
extern int nc_size;                     /* the size of the cache */
extern struct nc_hash nc_hash[];        /* the hash queues */
extern int nc_hash_size;                /* the number of hash queues */
extern struct nc_lru nc_lru;            /* the head of the lru list */
#endif

#endif	/* ! _NFS_DNLC_H_ */
