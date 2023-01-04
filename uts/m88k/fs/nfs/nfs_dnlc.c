/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 *
 *	The copyright above and this notice must be preserved in all
 *	copies of this source code.  The copyright above does not
 *	evidence any actual or intended publication of this source
 *	code.
 *
 *	This is unpublished proprietary trade secret source code of
 *	Lachman Associates.  This source code may not be copied,
 *	disclosed, distributed, demonstrated or licensed except as
 *	expressly authorized by Lachman Associates.
 */
/*	Copyright (c) 1985 by Sun Microsystems.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_dnlc.c	4.3 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/debug.h"
#if !m88k
#include "sys/inline.h"
#endif
#include "sys/mount.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"
#include "sys/fs/nfs/dnlc.h"



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

#define NC_HASH(name, namlen, dp) ((name[0] + name[namlen-1] + \
				    namlen + (int)dp) & (nc_hash_size-1))

#define ANYCRED	((struct ucred *)0)

/*
 * Macros to insert, remove cache entries from hash, LRU lists.
 */
#define INS_HASH(ncp, nch)	ins_hash(ncp, nch)
#define RM_HASH(ncp)		rm_hash(ncp)

#define INS_LRU(ncp1,ncp2)	ins_lru((struct ncache *) ncp1, \
					(struct ncache *) ncp2)
#define RM_LRU(ncp)		rm_lru((struct ncache *) ncp)

#define NULL_HASH(ncp)		(ncp)->hash_next = (ncp)->hash_prev = (ncp)

/*
 * Stats on usefulness of name cache.
 */
struct ncstats {
	int	hits;			/* hits that we can really use */
	int	misses;			/* cache misses */
	int	long_enter;		/* long names tried to enter */
	int	long_look;		/* long names tried to look up */
	int	lru_empty;		/* LRU list empty */
	int	purges;			/* number of purges of cache */
};

struct nc_lru nc_lru;		/* LRU list */

struct ncstats ncstats;		/* cache effectiveness statistics */

struct ncache *dnlc_search();
int doingcache = 1;

#define NC_SIZE         32              /* the size of the name cache */
#define NC_HASH_SIZE    16              /* the number of hash queues */

struct ncache ncache[NC_SIZE];
int nc_size = NC_SIZE;
struct nc_hash nc_hash[NC_HASH_SIZE];
int nc_hash_size = NC_HASH_SIZE;

/*
 * Initialize the directory cache.
 * Put all the entries on the LRU chain and clear out the hash links.
 */
dnlc_init()
{
	register struct ncache *ncp;
	register int i;

	nc_lru.lru_next = (struct ncache *)&nc_lru;
	nc_lru.lru_prev = (struct ncache *)&nc_lru;

	for (i = 0; i < nc_size; i++) {
		ncp = &ncache[i];
		INS_LRU(ncp, &nc_lru);
		NULL_HASH(ncp);
		ncp->dp = ncp->rp = (struct rnode *)0;
	}

	for (i = 0; i < nc_hash_size; i++) {
		ncp = (struct ncache *)&nc_hash[i];
		NULL_HASH(ncp);
	}
}

/*
 * Add a name to the directory cache.
 */
dnlc_enter(dp, name, rp, cred)
	register struct rnode *dp;
	register char *name;
	struct rnode *rp;
	struct ucred *cred;
{
	register int namlen;
	register struct ncache *ncp;
	register int hash;

	if (!doingcache)
		return;

	namlen = nfs_strlen(name);
	if (namlen > NC_NAMLEN) {
		ncstats.long_enter++;
		return;
	}

	hash = NC_HASH(name, namlen, dp);
	ncp = dnlc_search(dp, name, namlen, hash, cred);
	if (ncp != (struct ncache *)0) {
		return;
	}

	/*
	 * Take least recently used cache struct.
	 */
	ncp = nc_lru.lru_next;
	if (ncp == (struct ncache *)&nc_lru) {	/* LRU queue empty */
		ncstats.lru_empty++;
		return;
	}

	/*
	 * Remove from LRU, hash chains.
	 */
	RM_LRU(ncp);
	RM_HASH(ncp);

	/*
	 * Fill in the cache info.
	 */
	ncp->dp = dp;
	ncp->rp = rp;
	ncp->namlen = namlen;
	bcopy(name, ncp->name, (unsigned)namlen);
	ncp->cred = *cred;

	/*
	 * Insert in LRU, hash chains.
	 */
	INS_LRU(ncp, nc_lru.lru_prev);
	INS_HASH(ncp, &nc_hash[hash]);
}

/*
 * Look up a name in the directory name cache.
 */
struct rnode *
dnlc_lookup(dp, name, cred)
	struct rnode *dp;
	register char *name;
	struct ucred *cred;
{
	register int namlen;
	register int hash;
	register struct ncache *ncp;

	if (!doingcache)
		return((struct rnode *)0);

	namlen = nfs_strlen(name);
	if (namlen > NC_NAMLEN) {
		ncstats.long_look++;
		return((struct rnode *)0);
	}
	hash = NC_HASH(name, namlen, dp);

	ncp = dnlc_search(dp, name, namlen, hash, cred);
	if (ncp == (struct ncache *)0) {
		ncstats.misses++;
		return((struct rnode *)0);
	}
	ncstats.hits++;

	/*
	 * Move this slot to the end of LRU
	 * chain.
	 */
	RM_LRU(ncp);
	INS_LRU(ncp, nc_lru.lru_prev);

	/*
	 * If not at the head of the hash chain,
	 * move forward so will be found
	 * earlier if looked up again.
	 */
	if (ncp->hash_prev != (struct ncache *)&nc_hash[hash]) {
		RM_HASH(ncp);
		INS_HASH(ncp, ncp->hash_prev->hash_prev);
	}

	return(ncp->rp);
}

/*
 * Purge the entire cache.
 */
dnlc_purge()
{
	register struct nc_hash *nch;
	register struct ncache *ncp;

	ncstats.purges++;

start:
	for (nch = nc_hash; nch < &nc_hash[nc_hash_size]; nch++) {
		ncp = nch->hash_next;
		while (ncp != (struct ncache *)nch) {
			ASSERT(ncp->dp != 0 && ncp->rp != 0);
			dnlc_rm(ncp);
			goto start;
		}
	}
}

/*
 * Purge all the entries in the cache refering to rp.
 */
dnlc_purge_rp(rp)
	register struct rnode *rp;
{
	register struct ncache *ncp;

start:
	for (ncp = nc_lru.lru_next; ncp != (struct ncache *)&nc_lru;
	     ncp = ncp->lru_next) {
		if (ncp->dp == rp || ncp->rp == rp) {
			dnlc_rm(ncp);
			goto start;
		}
	}
}

/*
 * Purge all the entries in the cache belonging to the file system
 * pointed to by mp.
 */
dnlc_purge_mp(mp)
	register struct mount *mp;
{
	register struct ncache *ncp;

start:
	for (ncp = nc_lru.lru_next; ncp != (struct ncache *)&nc_lru;
	     ncp = ncp->lru_next) {
		if ((ncp->dp && ncp->dp->r_mp == mp) ||
		    (ncp->rp && ncp->rp->r_mp == mp)) {
			dnlc_rm(ncp);
			goto start;
		}
	}
}

/*
 * Remove an entry in the directory name cache.
 */
dnlc_remove(dp, name)
	register struct rnode *dp;
	register char *name;
{
	register int namlen;
	register struct ncache *ncp;
	int hash;

	namlen = nfs_strlen(name);
	if (namlen > NC_NAMLEN) {
		return;
	}

	hash = NC_HASH(name, namlen, dp);

	while (ncp = dnlc_search(dp, name, namlen, hash, ANYCRED)) {
		dnlc_rm(ncp);
	}
}

/*
 * Obliterate a cache entry.
 */
static
dnlc_rm(ncp)
	register struct ncache *ncp;
{
	/*
	 * Remove from LRU, hash chains.
	 */
	RM_LRU(ncp);
	RM_HASH(ncp);

	/*
	 * Remove reference to rnodes.
	 */
	ncp->dp = (struct rnode *)0;
	ncp->rp = (struct rnode *)0;

	INS_LRU(ncp, &nc_lru);		/* Insert at head of LRU list */
	NULL_HASH(ncp);			/* and make a dummy hash chain. */
}

/*
 * Utility routine to search for a cache entry.
 *
 * Note: if MP is defined, this routine expects to be called with the
 * the hash queue already locked.
 */
static struct ncache *
dnlc_search(dp, name, namlen, hash, cred)
	register struct rnode *dp;
	register char *name;
	register int namlen;
	int hash;
	struct ucred *cred;
{
	register struct nc_hash *nhp;
	register struct ncache *ncp;

	nhp = &nc_hash[hash];
	for (ncp = nhp->hash_next; ncp != (struct ncache *)nhp;
	     ncp = ncp->hash_next) {
		if (ncp->dp == dp &&
		    ncp->namlen == namlen &&
		    bcmp(ncp->name, name, namlen) == 0 &&
		    (cred == ANYCRED ||
		    bcmp((char *)&ncp->cred, (char *)cred, sizeof(*cred)) == 0)) {
			return (ncp);
		}
	}
	return((struct ncache *)0);
}

/*
 * insert into hash queue
 */
static
ins_hash(ncp, nch)
	register struct ncache *ncp;
	register struct nc_hash *nch;
{
	register struct ncache *ncp3;

	ncp3 = nch->hash_next;
	nch->hash_next = ncp;
	ncp->hash_next = ncp3;
	ncp3->hash_prev = ncp;
	ncp->hash_prev = (struct ncache*)nch;
}

/*
 * remove from hash queue
 */
static
rm_hash(ncp)
	register struct ncache *ncp;
{

	ncp->hash_prev->hash_next = ncp->hash_next;
	ncp->hash_next->hash_prev = ncp->hash_prev;
}

/*
 * insert into lru list
 */
static
ins_lru(ncp2, ncp1)
	register struct ncache *ncp2, *ncp1;
{
	register struct ncache *ncp3;

	ncp3 = ncp1->lru_next;
	ncp1->lru_next = ncp2;
	ncp2->lru_next = ncp3;
	ncp3->lru_prev = ncp2;
	ncp2->lru_prev = ncp1;
}

/*
 * remove from lru list
 */
static
rm_lru(ncp)
	register struct ncache *ncp;
{

	ncp->lru_prev->lru_next = ncp->lru_next;
	ncp->lru_next->lru_prev = ncp->lru_prev;
}
