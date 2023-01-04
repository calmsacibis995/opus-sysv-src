h43190
s 00107/00000/00000
d D 1.1 90/03/06 12:31:33 root 1 0
c date and time created 90/03/06 12:31:33 by root
e
u
U
t
T
I 1
#ifndef _NFS_RNODE_H_
#define _NFS_RNODE_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)rnode.h	4.2 LAI System V NFS Release 3.2/V3	source        */

/*      @(#)rnode.h 1.10 85/04/30 SMI      */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

struct rnode {
	struct rnode	*r_forw;	/* hashed by filehandle or free */
	struct rnode	*r_back;
	struct rnode	*av_forw;	/* free list pointers */
	struct rnode	*av_back;
	fhandle_t	r_fh;		/* file handle */
	short		r_error;	/* async write error */
	ushort		r_flags;	/* flags, see below */
	daddr_t		r_lastr;	/* last block read (read-ahead) */
	struct ucred	r_cred;		/* current credentials */
	struct ucred	r_unlcred;	/* unlinked credentials */
	char		*r_unlname;	/* unlinked file name */
	struct inode	*r_unldip;	/* parent dir of unlinked file */
	struct inode	*r_ip;		/* cooresponding inode */
	struct nfsfattr	r_nfsattr;	/* cached nfs attributes */
	time_t 		r_nfsattrtime;	/* time attributes cached */
	struct nfsfattr	r_snfsattr;	/* saved nfs attributes */
	struct mount	*r_mp;		/* pointer to the mount table entry */
	struct nfsiget	*r_iget;	/* marker for intermediate iget */
};

struct nfsiget {
	struct nfsiget	*n_next;
	short		n_pid;
};

/*
 * Flags
 */
#define	REOF		0x001		/* EOF encountered on read */
#define	RDIRTY		0x002		/* dirty buffers may be in buf cache */
#define ROPEN		0x004		/* the vnode is currently open */
#define RSTALE		0x008		/* available for re-use */
#define RNOCACHE	0x010		/* Don't cache b/c of Lock Manager */

/* given a buffer header pointer, return the associated inumber */
#define bptorno(bp)	(minor((bp)->b_dev))

/* given an 'r-number', find the rnode */
#define rnotor(n)	(&nfs_rnodes[n])

/* given a buffer header pointer, return the associated rnode */
#define bptor(bp)	rnotor(bptorno(bp))

/* given an rnode pointer, return its inumber */
#define rtorno(rp)	(rp-nfs_rnodes)

/* given the rnode pointer, return the file handle */
#define	rtofh(rp)	(&(rp)->r_fh)

/* given the inode, return the associated rnode */
#define	itor(ip)	((struct rnode *)((ip)->i_fsptr))

/* given an inode, return the file handle */
#define	itofh(ip)	rtofh(itor(ip))

/*
 * The rnode hash queues -- indexed via the filehandle
 */
struct hrnode {
	struct rnode *r_forw;
	struct rnode *r_back;
};

/*
 * The rnode list anchor.  This is used to anchor the free list
 * and the reserved list.
 */
struct rfreelist {
	struct rnode *r_forw;
	struct rnode *r_back;
	struct rnode *av_forw;
	struct rnode *av_back;
};

/*
 * Extern declarations for the rnode table and the number of rnodes.
*/
#ifdef INKERNEL
extern struct rnode nfs_rnodes[];
extern int nfs_rnum;
#endif

#endif	/* ! _NFS_RNODE_H_ */
E 1
