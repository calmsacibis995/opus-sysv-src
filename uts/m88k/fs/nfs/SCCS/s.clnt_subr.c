h31748
s 00488/00000/00000
d D 1.1 90/03/06 12:43:22 root 1 0
c date and time created 90/03/06 12:43:22 by root
e
u
U
t
T
I 1
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
/*	Copyright (c) 1986 by Sun Microsystems.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)clnt_subr.c	4.3 LAI System V NFS Release 3.2/V3	source";
#endif

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/mount.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/debug.h"
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#include "sys/immu.h"
#endif
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/fcntl.h"
#include "sys/flock.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

/*
 *	  Subroutines used by the V5.3 client side NFS.
 */

/*
 * Update the files system dependent superblock.
 * There is not much to do in the NFS case.
 */
/* ARGSUSED */
nfsupdate(mp)
	register struct mount *mp;
{
}

/*
 * Check access rights for an inode.
 *
 *	The wierd values returned by this function seem to be necessary.
 *	It was adapted from the S5 code.  Notice that it returns 1 to
 *	deny permission.  Further, a denial may or may NOT imply an error.
 */
int
nfsaccess(ip, mode)
	register struct inode *ip;
	register int mode;
{
	register int ret = 0;
	register struct nfsfattr *atp;
	struct rnode *rp;
	ushort ntype_to_itype();
	struct ucred cred;

	rp = itor(ip);

	nfs_unmark_iget(rp);

	set_cred(&cred);
	if ((ret = nfs_getattr(ip, &cred)) != 0) {
		u.u_error = ret;
		return(ret);
	}

	atp = &rp->r_nfsattr;

	switch (mode) {
	case ISUID:
	case ISGID:
	case ISVTX:
		if ((atp->na_mode & mode) == 0)
			ret = 1;
		break;

	case IMNDLCK:
		/* test order important */ 
		if (ntype_to_itype(atp->na_type) != IFREG) {
			ret = 1;
		} else if ((atp->na_mode & (ISGID|(IEXEC>>3))) != ISGID) { 
			ret = 1;
		} else if (itomi(ip)->mi_nomand) {
			u.u_error = EDEADLK;
			ret = 1;
		}
		break;

	case IOBJEXEC:
		if (ntype_to_itype(atp->na_type) != IFREG ||
		    (atp->na_mode & (IEXEC|IEXEC>>3|IEXEC>>6) == 0)) {
			u.u_error = EACCES;
			ret = 1;
			break;
		}
		/* fall into next case */

	case ICDEXEC:			
		mode = IEXEC;
		goto exec;

	case IWRITE:
		if (rdonlyfs(ip->i_mntdev)) {
			u.u_error = EROFS;
			ret = 1;
			break;
		}

		if (ip->i_flag & ITEXT) {
			xrele(ip);
			if (ip->i_flag & ITEXT) {
				u.u_error = ETXTBSY;
				ret = 1;
				break;
			}
		}
		/* fall into next case */

	case IREAD:
	case IEXEC:
exec:
		if (u.u_uid == 0)
			break;

		/* select appropriate triplet */
		if (u.u_uid != ip->i_uid) {
			mode >>= 3;
#ifdef multiple_groups
			if (!gidmember(ip->i_gid))
#else
			if (u.u_gid != ip->i_gid)
#endif
				mode >>= 3;
		}

		if ((atp->na_mode & mode) == 0) {
			u.u_error = EACCES;
			ret = 1;
		}
		break;
	}

	return(ret);
}

/* ARGSUSED */
nfs_fsync(ip, cred)
	struct inode *ip;
	struct ucred *cred;
{

	if (itor(ip)->r_flags & RDIRTY) {
		nfs_flush(ip);		/* start delayed writes */
	}
}

/*
 * Convert network type to System V flavor.
 *
 *	NOTE: we should come up with new V.3 types that can
 *	represent NFNON and NFLNK.  Although not operable
 *	they may appear in an rnode from a stat(). For now we set
 *	all bits on.  At least in the kernel, all users of inode
 *	type compare equality and don't check a bit so it'l work.
 */
ushort
ntype_to_itype(na_type)
	enum nfsftype na_type;
{
	register ushort ftype;

	switch (na_type) {
	case NFREG:
		ftype = IFREG;
		break;
	case NFDIR:
		ftype = IFDIR;
		break;
	case NFBLK:
		ftype = IFBLK;
		break;
	case NFCHR:
		ftype = IFCHR;
		break;
	case NFLNK:
		/* someday perhaps ... ftype = IFLNK; */
		/* for now fall into our "flag as invalid" hack */
	case NFNON:
		ftype = IFMT;
		break;

	default:
		ftype = IFMT;
		break;
	}
	return(ftype);
}

/*
 * Fill a credential structure from u-block information.
 */
set_cred(cr)
	register struct ucred *cr;
{
#ifdef multiple_groups
	register int i;
#endif

	cr->cr_ref = 1;
	cr->cr_uid = u.u_uid;
	cr->cr_gid = u.u_gid;
#ifdef multiple_groups
	for (i = 0; i < NGROUPS; i++)
		cr->cr_groups[i] = u.u_groups[i];
#else
	cr->cr_groups[0] = u.u_gid;
	cr->cr_groups[1] = NOGROUP;
#endif
	cr->cr_ruid = u.u_ruid;
	cr->cr_rgid = u.u_rgid;
}

/*
 * Clear an attribute structure before using giving trying to send it
 * over the wire.
 */
set_sa(sa)
	register struct nfssattr *sa;
{

	sa->sa_mode = (ulong)-1;
	sa->sa_uid = (ulong)-1;
	sa->sa_gid = (ulong)-1;
	sa->sa_size = (ulong)-1;

	sa->sa_atime.tv_sec = -1;
	sa->sa_atime.tv_usec = -1;

	sa->sa_mtime.tv_sec = -1;
	sa->sa_mtime.tv_usec = -1;
}

/*
 * Copy some attributes from the NFS structure in the rnode to the inode.
 */
nfs_rattr_to_iattr(ip)
	register struct inode *ip;
{
	register struct rnode *rp;

	rp = itor(ip);

	ip->i_uid = rp->r_nfsattr.na_uid;
	ip->i_gid = rp->r_nfsattr.na_gid;
	if (ip->i_size < rp->r_nfsattr.na_size)
		ip->i_size = rp->r_nfsattr.na_size;
	ip->i_nlink = rp->r_nfsattr.na_nlink;
	ip->i_rdev = rp->r_nfsattr.na_rdev;
}

/*
 * Timeout values for attributes for regular files, and for directories.
 */
static int nfsac_regtimeo = 3;
static int nfsac_dirtimeo = 30;

nfs_attrcache(ip, na, fflag)
	struct inode *ip;
	struct nfsfattr *na;
	enum staleflush fflag;
{
	register struct rnode *rp;

	rp = itor(ip);			/* find corresponding rnode */

	/*
	 * check the new modify time against the old modify time
	 * to see if cached data is stale
	 */
	if (na->na_mtime.tv_sec != rp->r_nfsattr.na_mtime.tv_sec ||
	    na->na_mtime.tv_usec != rp->r_nfsattr.na_mtime.tv_usec) {
		/*
		 * The file has changed.
		 * If this was unexpected (fflag == SFLUSH),
		 * flush the delayed write blocks associated with this inode
		 * from the buffer cache and mark the cached blocks on the
		 * free list as invalid. Also flush the page cache.
		 * If this is a text mark it invalid so that the next pagein
		 * from the file will fail.
		 * If the inode is a directory, purge the directory name
		 * lookup cache.
		 */
		if (fflag == SFLUSH) {
			nfs_invalfree(ip);
		}
		nfs_pflush(ip);		/* get file out of page cache */
		if (ip->i_ftype == IFDIR) {
			dnlc_purge_rp(rp);
		}
	}

	rp->r_nfsattr = *na;
	if (fflag == SFLUSH)
		nfs_rattr_to_iattr(ip);		/* save the attributes */

	/* set time when these attributes will expire in the cache */
	rp->r_nfsattrtime = time;
	if (ip->i_ftype == IFDIR)
		rp->r_nfsattrtime += nfsac_dirtimeo;
	else
		rp->r_nfsattrtime += nfsac_regtimeo;
}

/*
 * refresh possibly stale cached attributes
 */
int
nfs_getattr(ip, cred)
	struct inode *ip;
	struct ucred *cred;
{
	register int error = 0;
	struct nfsattrstat *ns;
	struct rnode *rp = itor(ip);

	nfs_fsync(ip, cred);		/* sync so mod time is right */

	/*
	 * if cached attributes are stale refresh from across the net
	 */

	if (time > rp->r_nfsattrtime) {
		ns = (struct nfsattrstat *)kmem_alloc((uint)sizeof(*ns));
		error = rfscall(itomi(ip), RFS_GETATTR,
				xdr_fhandle, (caddr_t)itofh(ip),
				xdr_attrstat, (caddr_t)ns, cred);
		if (!error) {
			error = geterrno(ns->ns_status);
			if (!error)
				nfs_attrcache(ip, &ns->ns_attr, SFLUSH);
		}
		kmem_free((caddr_t)ns, (uint)sizeof(*ns));
	}

	return(error);
}

/*
 * set the attributes of an NFS file - the internal version
 */
int
nfs_setattr(ip, sap, cred)
	register struct inode *ip;
	struct nfssattr *sap;
	struct ucred *cred;
{
	int error;
	struct nfssaargs args;
	struct nfsattrstat *ns;

	nfs_fsync(ip, cred);

	args.saa_sa = *sap;
	args.saa_fh = *itofh(ip);
	ns = (struct nfsattrstat *)kmem_alloc((uint)sizeof(*ns));
	error = rfscall(itomi(ip), RFS_SETATTR,
			xdr_saargs, (caddr_t)&args,
			xdr_attrstat, (caddr_t)ns, cred);
	if (!error) {
		error = geterrno(ns->ns_status);
		if (!error) {
			nfs_attrcache(ip, &ns->ns_attr, SFLUSH);
		}
	}
	kmem_free((caddr_t)ns, (uint)sizeof(*ns));

	return(error);
}

/*
 * open the indicated inode with the mode given
 */
/* ARGSUSED */
nfsopeni(ip, mode)
	register struct inode *ip;
	int mode;
{
	register struct rnode *rp;
	int error;
	struct ucred cred;

	rp = itor(ip);

	nfs_unmark_iget(rp);

	/* force validation of cached data by getting the attributes */
	set_cred(&cred);
	error = nfs_getattr(ip, &cred);
	if (error) {
		u.u_error = error;
	} else {
		rp->r_flags |= ROPEN;
	}
}

/*
 * close the file associated with the indicated inode which was open
 * with flags that were passed
 */
/* ARGSUSED */
nfsclosei(ip, flag, count, offset)
	register struct inode *ip;
	int flag, count;
	off_t offset;
{
	register struct rnode *rp;

	rp = itor(ip);

	nfs_unmark_iget(rp);

	/*
	 * If the Lock Manager is not running, quick return.
	 */
	nfs_cleanlocks(ip, USE_PID);

	if ((unsigned)count > 1 || ip->i_count > 1)
		return;

	rp->r_flags &= ~ROPEN;

	/*
	 * If this is a close of a file open for writing or an unlinked
	 * open file or a file that has had an asynchronous write error,
	 * flush synchronously. This allows us to invalidate the files
	 * buffers if there was a write error or the file was unlinked.
	 */
	if (flag & FWRITE || rp->r_unldip != NULL || rp->r_error) {
		struct ucred cred;

		set_cred(&cred);
		nfs_fsync(ip, &cred);
	}

	if (rp->r_unldip != NULL || rp->r_error) {
		nfs_inval(ip);
	}

	if (flag & FWRITE) {
		if (rp->r_error) {
			u.u_error = rp->r_error;
		}
	}
}
E 1
