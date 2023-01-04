h18574
s 01016/00000/00000
d D 1.1 90/03/06 12:43:29 root 1 0
c date and time created 90/03/06 12:43:29 by root
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
#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_nami.c	4.3a LAI System V NFS Release 3.2/V3	source";
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
#include "sys/nami.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/errno.h"
#include "sys/debug.h"
#if !m88k
#include "sys/inline.h"
#endif
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#include "sys/immu.h"
#endif
#include "sys/region.h"
#include "sys/proc.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

extern struct rnode *nfs_iget();

/*
 * Perform NFS specific namei() subcommands
 */
int
nfsnamei(nxp, argp)
	struct nx *nxp;
	struct argnamei *argp;
{
	int error;			/* error returns as needed */
	int retval;			/* set to NI_PASS, _NULL, _DONE, ... */
	bool_t iputdp;			/* see below - a simple flag */
	struct inode *ip;
	struct rnode *rp;
	struct ucred cred;
	struct nfssattr sa;		/* needed by create/mkdir for attrs */
	extern int nfsdstrategy();

	nfs_unmark_iget(itor(nxp->dp));

	/*
	 * most return paths from namei() assume that we will
	 * iput(nxp->dp).  We do the iput at the end of this code based
	 * on the "iputdp" flag. some paths disable the iput if needed.
	 */
	iputdp = TRUE;

	set_cred(&cred);
	ip = (struct inode *)0;

	switch ((argp) ? argp->cmd : 0) {

	case 0:				/* simple lookup */
		error = nfs_lookup(nxp->dp, nxp->comp, &rp, &cred);

		if (error)		/* lookup failed */
			break;

		retval = NI_PASS;

		/* handle symbolic link */
		if (rp->r_nfsattr.na_type == NFLNK) {
			/*
			 * We assume that we always want to follow
			 * symbolic links during lookup operations.
			 */
			retval = getsymlink(rp, nxp, &cred);
			if (retval == NI_FAIL)
				error = u.u_error;
			else
				iputdp = FALSE;
			break;
		}

		if ((ip = rp->r_ip) != NULL && ip->i_flag & IISROOT) {
			nxp->flags |= NX_ISROOT;
			ASSERT(ip->i_mntdev->m_mount == ip);
		}
		nxp->ino = rtorno(rp);
		ASSERT(!(nxp->ino < 0 || nxp->ino > nfs_rnum));
		iputdp = FALSE;
		break;

	case NI_DEL:
		error = nfs_lookup(nxp->dp, nxp->comp, &rp, &cred);
		if (error) {
			error = ENOENT;
			break;
		}
		if (nfsaccess(nxp->dp, IWRITE)) {
			error = u.u_error;
			nfs_rfree(rp);
			break;
		}
		if (rp == (struct rnode *)nxp->dp->i_fsptr) {
			ip = nxp->dp;
		} else {
			ip = iget(rp->r_mp, rtorno(rp));
			if (ip == NULL) {
				error = u.u_error;
				break;
			}
		}
		if (ip->i_ftype == IFDIR && !suser()) {
			error = EPERM;
			if (ip != nxp->dp) {
				iput(ip);
			} else {
				nfs_rfree(rp);
			}
			break;
		}
		if (ip->i_flag & ITEXT) {
			xrele(ip);
			if (ip->i_flag & ITEXT && ip->i_nlink == 1) {
				error = ETXTBSY;
				iput(ip);
				break;
			}
		}
		if (ip->i_mntdev != nxp->dp->i_mntdev) {
			error = EBUSY;
			iput(ip);
			break;
		}
		if (ip != nxp->dp) {
			iput(ip);
		} else {
			nfs_rfree(rp);
		}

		/*
		 * Actually do the remove.
		 */
		error = nfs_remove(nxp->dp, nxp->comp, &cred);
		retval = NI_NULL;
		break;


	case NI_XCREAT:
	case NI_CREAT:
		error = nfs_lookup(nxp->dp, nxp->comp, &rp, &cred);
		retval = NI_DONE;
		/*
		 * it could exist. if we wanted it exclusively, that is
		 * an error and return no locked resources. otherwise,
		 * it does exist so we return its locked inode and FOUND.
		 */
		if (error == 0) {
			/*
			 * We need to follow symbolic links.
			 */
			if (rp->r_nfsattr.na_type == NFLNK) {
				retval = getsymlink(rp, nxp, &cred);
				if (retval == NI_FAIL)
					error = u.u_error;
				else
					iputdp = FALSE;
				break;
			}
			if (rp->r_ip != nxp->dp) {
				ip = iget(rp->r_mp, rtorno(rp));
				if (ip == NULL) {
					error = u.u_error;
					break;
				}
			} else {
				ip = nxp->dp;
				nfs_unmark_iget(rp);
			}
			if (argp->cmd == NI_XCREAT) {
				error = EEXIST;
				if (ip != nxp->dp) {
					iput(ip);
				} else {
					nfs_rfree(rp);
				}
			} else {
				argp->rcode = FSN_FOUND;
				if (ip != nxp->dp) {
					iput(nxp->dp);
					nxp->dp = ip;
				}
				iputdp = FALSE;
			}
			break;
		}

		/*
		 * file doesn't seem to be here so let's create it.
		 */

		if (nfsaccess(nxp->dp, IWRITE)) {
			error = u.u_error;
			break;
		}
		/* set the file mode */
		argp->mode &= MODEMSK;
		argp->mode &= ~(u.u_cmask|ISVTX);
		set_sa(&sa);
		sa.sa_mode = argp->mode;

		error = nfs_create(nxp->dp, nxp->comp, &sa, &ip, &cred);
		if (error == 0) {
			argp->rcode = FSN_NOTFOUND;
			iput(nxp->dp);
			nxp->dp = ip;
			iputdp = FALSE;
		}
		break;


	case NI_LINK:
		/*
		 * link(from_inode, new_file_name);
		 * from_inode is held but not locked on entry and is
		 * in the same state at exit, although we iget/iput it.
		 * new_file_name is in std. (dp,name) form with dp locked.
		 * dp should be iput() on exit.
		 */

		/*
		 * Check to see whether new_file_name is there already
		 */
		error = nfs_lookup(nxp->dp, nxp->comp, &rp, &cred);
		if (!error) {
			error = EEXIST;
			nfs_rfree(rp);
			break;
		}

		/*
		 * Make sure that the target file system and directory
		 * are writeable.
		 */
		if (nfsaccess(nxp->dp, IWRITE)) {
			error = u.u_error;
			break;
		}

		/*
		 * Make sure that the from_inode resides on an NFS
		 * file system.
		 */
		if (bdevsw[bmajor(argp->idev)].d_strategy != nfsdstrategy) {
			error = EXDEV;
			break;
		}

		/*
		 * Make that the "from" file system and "to" file system
		 * are the same.
		 */
		if (rnotor(argp->ino)->r_mp != nxp->dp->i_mntdev) {
			error = EXDEV;
			break;
		}

		/*
		 * get the from_inode
		 */
		if (argp->ino == nxp->dp->i_number) {
			ip = nxp->dp;
		} else if ((ip = iget(nxp->dp->i_mntdev, argp->ino)) == NULL) {
			error = ENOENT;
			break;
		}

		nfs_unmark_iget(itor(ip));

		/*
		 * The mount pointers should still be the same, even
		 * considering that iget cross mount points.  The
		 * original (idev, ino) should point to the hopped mount
		 * point.
		 */
		ASSERT(ip->i_mntdev == nxp->dp->i_mntdev);

		error = nfs_link(ip, nxp->dp, nxp->comp, &cred);
		if (ip != nxp->dp) {
			iput(ip);
		}
		retval = NI_NULL;
		break;


	case NI_MKDIR:
		if (nfsaccess(nxp->dp, IWRITE)) {
			error = u.u_error;
			break;
		}
		/* set the file modes */
		argp->mode &= MODEMSK;
		argp->mode &= ~(u.u_cmask|ISVTX);
		set_sa(&sa);
		sa.sa_mode = argp->mode;

		error = nfs_mkdir(nxp->dp, nxp->comp, &sa, &ip, &cred);
		if (!error)
			iput(ip);
		retval = NI_NULL;
		break;


	case NI_RMDIR:
		error = nfs_lookup(nxp->dp, nxp->comp, &rp, &cred);
		if (error) {
			error = ENOENT;
			break;
		}
		if (nfsaccess(nxp->dp, IWRITE)) {
			error = u.u_error;
			nfs_rfree(rp);
			break;
		}
		if (nxp->dp == rp->r_ip) {	/* rmdir "." !! */
			error = EINVAL;
			nfs_rfree(rp);
			break;
		}
		if (*(nxp->comp)   == '.' &&	/* check for rmdir ".." */
		    *(nxp->comp+1) == '.' &&
		    *(nxp->comp+2) == '\0') {
			error = EEXIST;
			nfs_rfree(rp);
			break;
		}

		ip = iget(rp->r_mp, rtorno(rp));
		if (ip == NULL) {
			error = u.u_error;
			break;
		}

		/* check for dir being a mount point under us */
		if (nxp->dp->i_mntdev != ip->i_mntdev)
			error = EBUSY;

		/* check for not a dir at all */
		else if (ip->i_ftype != IFDIR)
			error = ENOTDIR;

		/* check for current directory */
		else if (ip == u.u_cdir)
			error = EINVAL;

		iput(ip);
		if (error)
			break;

		dnlc_purge_rp(rp);	/* purge this entry from the DNLC */

		error = nfs_rmdir(nxp->dp, nxp->comp, &cred);
		retval = NI_NULL;
		break;


	case NI_MKNOD:
		error = EINVAL;
		break;

	}

	if (error) {
		iputdp = TRUE;
		retval = NI_FAIL;
		u.u_error = error;
	}

	if (iputdp == TRUE)
		iput(nxp->dp);

	return(retval);
}

/*
 * Implement NFS specific CHMOD or CHOWN syscalls.
 */
int
nfssetattr(ip, flagp)
	register struct inode *ip;
	register struct argnamei *flagp;
{
	int error;
	struct ucred cred;
	struct nfssattr sa;

	nfs_unmark_iget(itor(ip));

	if (u.u_uid != ip->i_uid && !suser())
		return(0);

	if (ip->i_mntdev->m_flags & MRDONLY) {
		u.u_error = EROFS;
		return(0);
	}

	set_sa(&sa);
	set_cred(&cred);

	switch (flagp->cmd) {
	case NI_CHMOD:
		sa.sa_mode = (flagp->mode & PERMMSK);
		break;

	case NI_CHOWN:
		sa.sa_uid = flagp->uid;
		sa.sa_gid = flagp->gid;
		break;

	default:
		u.u_error = EINVAL;
		return(0);
	}

	if (error = nfs_setattr(ip, &sa, &cred)) {
		u.u_error = error;
		return(0);
	}

	return(1);
}

/* ARGSUSED */
long
nfsnotify(ip, noargp)
	register struct inode *ip;
	register struct argnotify *noargp;
{

	ASSERT(noargp != NULL);

	switch ((int) noargp->cmd) {
	case NO_SEEK:
		if (noargp->data1 < 0)
			u.u_error = EINVAL;
		return(noargp->data1);
		break;
	
	case NO_IGET:
		ASSERT(noargp->data2 >= 0 && noargp->data2 < nfs_rnum);
		nfs_rfree(rnotor((ino_t)noargp->data2));
		break;
	}
	return(0L);
}

/*
 * NFS operations needed for namei() functions.
 */

/*
 * Return rnode for requested component.
 *
 *	The associated inode (assuming one has been allocated yet)
 *	is NOT locked.
 */

static int
nfs_lookup(dip, nm, rpp, cred)
	register struct inode *dip;
	char *nm;
	struct rnode **rpp;
	struct ucred *cred;
{
	register struct rnode *rp;
	int error;
	struct nfsdiropres *dr;
	struct nfsdiropargs da;
	extern struct rnode *dnlc_lookup();

	/*
	 * Before checking dnlc, call getattr to be
	 * sure directory hasn't changed.  getattr
	 * will purge dnlc if a change has occurred.
	 */
	if (error = nfs_getattr(dip, cred))
		return(error);

	/*
	 * If we are looking for "." return dip, since that is it.
	 * Hope (pray) that the caller's won't try to iput() it twice.
	 * They can detect this condition by testing for ipp == dip
	 * Notice that ".." at root is the same as ".", because S5 handles
	 * crossing the mount point AFTER calling the file system dependent
	 * namei().  That can cause an iget() of a locked inode, except for
	 * this code.
	 */
	if (*nm == '.' &&
	    (*(nm+1) == '\0' ||
	    ((dip->i_flag & IISROOT) && *(nm+1) == '.' && *(nm+2) == '\0'))) {
		ASSERT(dip->i_flag & ILOCK);	/* it had better be locked */
		*rpp = itor(dip);
		nfs_mark_iget(*rpp);
		return(0);
	}

	rp = dnlc_lookup(itor(dip), nm, cred);
	if (rp) {
		if (rp->r_flags & RSTALE) {
			rp->av_back->av_forw = rp->av_forw;
			rp->av_forw->av_back = rp->av_back;
			rp->r_flags &= ~RSTALE;
			rp->r_snfsattr = rp->r_nfsattr;
		}
		nfs_mark_iget(rp);
		*rpp = rp;
		return(0);
	}

	dr = (struct  nfsdiropres *)kmem_alloc((uint)sizeof(*dr));
	setdiropargs(&da, nm, dip);
	error = rfscall(itomi(dip), RFS_LOOKUP, xdr_diropargs, (caddr_t)&da,
			xdr_diropres, (caddr_t)dr, cred);
	if (!error) {
		error = geterrno(dr->dr_status);
	}
	if (!error) {
		rp = nfs_iget(&dr->dr_fhandle, &dr->dr_attr, dip->i_mntdev);
		if (rp == NULL) {
			error = u.u_error;
		} else {
			if (rp->r_nfsattr.na_type == NFDIR)
				dnlc_enter(itor(dip), nm, rp, cred);
		} 
		*rpp = rp;
	} else {
		*rpp = (struct rnode *)0;
	}
	kmem_free((caddr_t)dr, (uint)sizeof(*dr));
	return(error);
}

/*
 * Ask a server to create a named component.
 */
static int
nfs_create(dip, nm, sa, ipp, cred)
	struct inode *dip;
	char *nm;
	struct nfssattr *sa;
	struct inode **ipp;
	struct ucred *cred;
{
	int error;
	struct rnode *rp;
	struct nfscreatargs args;
	struct nfsdiropres *dr;

	*ipp = NULL;
	dr = (struct nfsdiropres *)kmem_alloc((uint)sizeof(*dr));
	setdiropargs(&args.ca_da, nm, dip);
	args.ca_sa = *sa;
	error = rfscall(itomi(dip), RFS_CREATE, xdr_creatargs, (caddr_t)&args,
			xdr_diropres, (caddr_t)dr, cred);
	if (!error) {
		error = geterrno(dr->dr_status);
		if (!error) {
			rp = nfs_iget(&dr->dr_fhandle, &dr->dr_attr,
				      dip->i_mntdev);
			if (rp != NULL) {
				*ipp = iget(rp->r_mp, rtorno(rp));
			}
			if (*ipp == NULL) {
				error = u.u_error;
			}
		}
	}
	kmem_free((caddr_t)dr, (uint)sizeof(*dr));
	return(error);
}

#define PREFIX ".nfsjunk"
#define PREFIXLEN (sizeof(PREFIX)-1)

static char *
newname(s)
	register char *s;
{
	static int ngen = 0;
	char *news;
	register char *s1;
	register int i;

	news = (char *)kmem_alloc((uint)NFS_MAXNAMLEN);
	bcopy(PREFIX, news, PREFIXLEN);
	i = ++ngen;			/* generate a unique # */
	s1 = &news[PREFIXLEN];
	while (i) {
		*s1++ = "0123456789ABCDEF"[i & 0x0f];
		i >>= 4;
	}
	while (*s1++ = *s++) {
		if (s1 == &news[NFS_MAXNAMLEN - 1]) {
			*s1 = '\0';
			break;
		}
	}
	return(news);
}


/*
 * Ask server to delete a file.
 * If the rnode to be removed is open we rename it instead
 * and a later call to iput() will remove the new name.
 */
static int
nfs_remove(dip, nm, cred)
	struct inode *dip;
	char *nm;
	struct ucred *cred;
{
	int error;
	enum nfsstat status;
	struct inode *ip;
	char *tmpname;
	struct rnode *rp;
	struct nfsdiropargs da;
	char *newname();

	status = NFS_OK;
	error = nfs_lookup(dip, nm, &rp, cred);
	if (error == 0) {
		ip = iget(rp->r_mp, rtorno(rp));
		if (ip == NULL) {
			return(u.u_error);
		}
		if ((rp->r_flags & ROPEN) && rp->r_unlname == NULL) {
			tmpname = newname(nm);
			error = nfs_rename(dip, nm, dip, tmpname, cred);
			if (error) {
				kmem_free((caddr_t)tmpname, NFS_MAXNAMLEN);
			} else {
				rp->r_unldip = dip;
				rp->r_unlname = tmpname;
				rp->r_unlcred = *cred;
				dip->i_count++;
			}
		} else {
			setdiropargs(&da, nm, dip);
			error = rfscall(itomi(dip), RFS_REMOVE,
					xdr_diropargs, (caddr_t)&da,
					xdr_enum, (caddr_t)&status, cred);
			nfs_invalfree(ip);
		}
		iput(ip);
	}

	if (error == 0) {
		error = geterrno(status);
	}
	return(error);
}

/*
 * Ask server to create a hard link.
 */
static int
nfs_link(ip, tdip, tnm, cred)
	struct inode *ip;
	struct inode *tdip;
	char *tnm;
	struct ucred *cred;
{
	int error;
	enum nfsstat status;
	struct nfslinkargs args;

	args.la_from = *itofh(ip);
	setdiropargs(&args.la_to, tnm, tdip);
	error = rfscall(itomi(ip), RFS_LINK,
			xdr_linkargs, (caddr_t)&args,
			xdr_enum, (caddr_t)&status, cred);
	if (error == 0)
		error = geterrno(status);
	return(error);
}

/*
 * Ask server to rename a file for us.
 */
static int
nfs_rename(odip, onm, ndip, nnm, cred)
	struct inode *odip;
	char *onm;
	struct inode *ndip;
	char *nnm;
	struct ucred *cred;
{
	int error;
	enum nfsstat status;
	struct nfsrnmargs args;

	if (!strcmp(onm, ".") || !strcmp(onm, "..") ||
	    !strcmp(nnm, ".") || !strcmp (nnm, "..")) {
		error = EINVAL;
	} else {
		dnlc_remove(itor(odip), onm);
		dnlc_remove(itor(ndip), nnm);
		setdiropargs(&args.rna_from, onm, odip);
		setdiropargs(&args.rna_to, nnm, ndip);
		error = rfscall(itomi(odip), RFS_RENAME,
				xdr_rnmargs, (caddr_t)&args,
				xdr_enum, (caddr_t)&status, cred);
		if (error == 0) {
			error = geterrno(status);
		}
	}
	return(error);
}

/*
 * Ask server to create a directory entry.
 */
static int
nfs_mkdir(dip, nm, sa, ipp, cred)
	struct inode *dip;
	char *nm;
	struct nfssattr *sa;
	struct inode **ipp;
	struct ucred *cred;
{
	int error;
	struct rnode *rp;
	struct nfsdiropres *dr;
	struct nfscreatargs args;

	*ipp = NULL;
	dr = (struct nfsdiropres *)kmem_alloc((uint)sizeof(*dr));
	setdiropargs(&args.ca_da, nm, dip);
	args.ca_sa = *sa;
	error = rfscall(itomi(dip), RFS_MKDIR, xdr_creatargs, (caddr_t)&args,
			xdr_diropres, (caddr_t)dr, cred);
	if (!error)
		error = geterrno(dr->dr_status);
#ifdef notdef
	if (!error) {
		/*
		 * Some NFS servers do not return the attributes of
		 * the new directory.  In this case, we need to do
		 * a getattr operation to get the actual new attributes.
		 */
		if (dr->dr_attr.na_type != NFDIR) {
			struct nfsattrstat *ns;

			ns = (struct nfsattrstat *)kmem_alloc((uint)sizeof(*ns));
			error = rfscall(itomi(dip), RFS_GETATTR,
					xdr_fhandle, (caddr_t)&dr->dr_fhandle,
					xdr_attrstat, (caddr_t)ns, cred);
			if (!error)
				error = geterrno(ns->ns_status);
			if (!error)
				dr->dr_attr = ns->ns_attr;
			kmem_free((caddr_t)ns, (uint)sizeof(*ns));
		}
	}
#endif
	if (!error) {
		rp = nfs_iget(&dr->dr_fhandle, &dr->dr_attr, dip->i_mntdev);
		if (rp != NULL) {
			*ipp = iget(rp->r_mp, rtorno(rp));
		}
		if (*ipp == NULL) {
			error = u.u_error;
		}
	}
	kmem_free((caddr_t)dr, (uint)sizeof(*dr));
	return(error);
}

/*
 * Ask server to remove a directory.
 */
static int
nfs_rmdir(dip, nm, cred)
	struct inode *dip;
	char *nm;
	struct ucred *cred;
{
	int error;
	enum nfsstat status;
	struct nfsdiropargs da;

	setdiropargs(&da, nm, dip);
	dnlc_remove(itor(dip), nm);
	error = rfscall(itomi(dip), RFS_RMDIR,
			xdr_diropargs, (caddr_t)&da,
			xdr_enum, (caddr_t)&status, cred);
	if (error == 0)
		error = geterrno(status);
	return(error);
}

/*
 * Handle a symlink during nfsnamei() operation.
 *
 * The return values are:
 *	NI_FAIL		if there are any errors during processing,
 *		u.u_error has the real error code.
 *	NI_RESTART	if an absolute symlink path replaced ours
 *		should be reflected up as NI_RESTART to namei().
 *	NI_SYMRESTART	if a relative symlink path replaced ours
 *		should be reflected up as NI_SYMRESTART to namei().
 *
 * If we can successfully read the contents of the symbolic link,
 * then we call pathcombine() to modify the buffer addressed
 * by nxp->bufp and nxp->comp to install the symlink in place.
 */
static int
getsymlink(rp, nxp, cred)
	struct rnode *rp;
	struct nx *nxp;
	struct ucred *cred;
{
	int error;
	int retval;
	struct inode *ip;
	struct nfsrdlnres rl;

	ip = iget(rp->r_mp, rtorno(rp));
	if (ip == NULL) {
		error = u.u_error;
		goto fail;
	}

	rl.rl_data = (char *)kmem_alloc((uint)NFS_MAXPATHLEN);

	error = nfs_readlink(ip, &rl, cred);
	if (error == 0) {
		if ((error = pathcombine(&rl, nxp)) == 0) {
			if (*(rl.rl_data) == '/')
				retval = NI_RESTART;
			else
				retval = NI_SYMRESTART;
		}
	}

	kmem_free((caddr_t)rl.rl_data, (uint)NFS_MAXPATHLEN);

	iput(ip);

fail:
	if (error) {
		u.u_error = error;
		retval = NI_FAIL;
	}

	return(retval);
}

/*
 * Obtain the contents of a symbolic link from the server.
 */
static int
nfs_readlink(ip, rlp, cred)
	struct inode *ip;
	struct nfsrdlnres *rlp;
	struct ucred *cred;
{
	int error;

	/* rnode is of type NFS SYMLINK */
	ASSERT(itor(ip)->r_nfsattr.na_type == NFLNK);

	/* rlp has a data buffer allocated */
	ASSERT(rlp->rl_data != NULL);	/* should be size NFS_MAXPATHLEN */

	error = rfscall(itomi(ip), RFS_READLINK,
			xdr_fhandle, (caddr_t)itofh(ip),
			xdr_rdlnres, (caddr_t)rlp, cred);
	if (error == 0)
		error = geterrno(rlp->rl_status);
	return(error);
}

/*
 * Install a symlink into a namei() path buffer.
 *
 * The processing is slightly different for relative and absolute symbolic
 * links.  For an absolute symbolic link, we are going to place the
 * contents of the symbolic link at the beginning of the buffer and then
 * concatenate the rest of the original path on the end.  For a relative
 * symbolic link, we are going to place the contents of the symbolic link
 * at the current component pointer and then concatenate the rest of the
 * original path on the end.  This is because namei() retains some local
 * knowledge about addresses in the pathname buffer.
 *
 * return: ENOENT on error (basically only pathname length problems)
 *	   otherwise 0
 */

#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
#if USIZE == 2
#define PATHSIZE	512
#else
#define PATHSIZE	1024
#endif
#endif

static int
pathcombine(rlp, nxp)
	register struct nfsrdlnres *rlp;
	register struct nx *nxp;
{
	register int len;
	register char *cploc;

	/* Calculate the length of the remaining path (possibly 0) */
	len = nfs_strlen(u.u_nextcp);

	if (*rlp->rl_data == '/') {	/* absolute */
		/*
		 * Make sure that we have enough room for the contents of
		 * the symbolic link, a '/', the rest of the path, and a NULL.
		 */
		if (rlp->rl_count + 1 + len + 1 >= PATHSIZE)
			return(ENOENT);
		cploc = nxp->bufp;
	} else {			/* relative */
		/*
		 * Make sure that there is room left in the buffer for the
		 * contents of the symbolic link, a '/', the rest of the path,
		 * and a NULL.  We are (possibly) not at the beginning of the
		 * buffer, hence the first calculation using pointer arithmetic.
		 */
		if (nxp->comp - nxp->bufp + rlp->rl_count + 1 + len + 1 >= PATHSIZE)
			return(ENOENT);
		cploc = nxp->comp;
	}
	/*
	 * Copy the rest of the original path to its final resting
	 * place.  We use ovbcopy() because we do not know whether
	 * we are overlapping buffers.
	 */
	ovbcopy(u.u_nextcp, cploc + rlp->rl_count + 1, len);
	/*
	 * Copy the contents of the symbolic link into place.
	 */
	bcopy(rlp->rl_data, cploc, rlp->rl_count);
	/*
	 * Put in the '/' and then NULL terminate the string.
	 */
	*(cploc + rlp->rl_count) = '/';
	*(cploc + rlp->rl_count + 1 + len) = '\0';

	return(0);
}

/*
 * Overlapped bcopy.
 *
 * Copy `from' to `to' taking into account the operands may represent
 * overlapping buffers.  If `from' is less then `to', we can safely
 * copy from right to left.  If `from' is greater than `to', then we
 * can safely copy from left to right.
 */
ovbcopy(from, to, len)
	char *from, *to;
	int len;
{
	register char *fp, *tp;

	if (from < to) {	/* copy right to left */
		fp = from + len;
		tp = to + len;
		while (len-- > 0)
			*--tp = *--fp;
	} else {		/* copy left to right */
		fp = from;
		tp = to;
		while (len-- > 0)
			*tp++ = *fp++;
	}
}
E 1
