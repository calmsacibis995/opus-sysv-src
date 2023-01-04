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
/*	Copyright (c) 1986 by Sun Microsystems, Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_server.c	4.3a LAI System V NFS Release 3.2/V3	source";
#endif
/*      @(#)nfs_server.c 2.42 86/01/14 SMI      */

/*
 *	Server side NFS routines.
 */

/*
 * Some code from this module was taken from Convergent's nfs_server.c:
 * 
 * 	Convergent Technologies - System V - Nov 1986
 * 	#ident	"@(#)nfs_server.c	1.6 :/source/uts/common/fs/nfs/s.nfs_server.c 1/20/87 00:03:49"
 * 
 *	#ifdef LAI
 * 	static char SysVNFSID[] = "@(#)nfs_server.c	2.17 LAI System V NFS Release 3.0/V3 source";
 *	#endif
 */

/*
 * Some code from this module was taken from Convergent's nfsd.c:
 *
 *      Convergent Technologies - System V - Nov 1986
 *	#ident  "@(#)nfsd.c     1.6 :/source/uts/common/fs/nfs/s.nfsd.c 1/20/87 00:04:26"
 *	#ifdef LAI
 *	static char SysVNFSID[] = "@(#)nfsd.c   2.11 LAI System V NFS Release 3.0/V3 source";
 *	#endif
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/conf.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/inode.h"
#include "sys/nami.h"
#include "sys/fstyp.h"
#include "sys/file.h"
#include "sys/errno.h"
#include "sys/statfs.h"
#include "sys/dirent.h"
#include "sys/mount.h"
#include "sys/stream.h"
#include "sys/cmn_err.h"
#include "sys/debug.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/auth_unix.h"
#include "sys/fs/nfs/svc.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/ucred.h"

#include "sys/fs/s5param.h"

/*
 * rpc service program version range supported
 */
#define	VERSIONMIN	2
#define	VERSIONMAX	2

/* handy macros */
#if !m88k
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

/*
 * Returns true iff filesystem for a given fsid is exported read-only
 */
#define rdonly(ip)	((ip)->i_mntdev->m_flags & MRDONLY)

struct inode *fhtoip();
void svcerr_progvers();
void rfs_dispatch();

extern caddr_t kmem_alloc();
extern void kmem_free();

struct {
	int	ncalls;		/* number of calls received */
	int	nbadcalls;	/* calls that failed */
	int	reqs[32];	/* count for each request */
} svstat;

char domainname[64] = "unknown";
int domainnamelen = sizeof("unknown") + 1;;

/*
 * NFS Server system call.
 * Does all of the work of running a NFS server.
 * File descriptor fd is for open transport provider.
 */
nfs_svc()
{
	register struct a {
		int fd;
	} *uap;

	uap = (struct a *)u.u_ap;

	server_daemon(uap->fd);
}

server_daemon(fd)
	int fd;
{
	struct inode *rdir;
	struct inode *cdir;
	struct file *fp;
	SVCXPRT *xprt;
	ulong vers;

	fp = getf(fd);
	if (fp == NULL) {
		u.u_error = EBADF;
		return;
	}

	/*
	 * Be sure that rdir (the server's root inode) is set.
	 * Save the current directory and restore it again when
	 * the call terminates.
	 */
	rdir = u.u_rdir;
	cdir = u.u_cdir;
	if (u.u_rdir == NULL) {
		u.u_rdir = u.u_cdir;
	}
	xprt = svckudp_create(fp, NFS_PORT);
	for (vers = VERSIONMIN; vers <= VERSIONMAX; vers++) {
		(void) svc_register(xprt, NFS_PROGRAM, vers, rfs_dispatch,
				    FALSE);
	}
	if (setjmp(u.u_qsav)) {
		for (vers = VERSIONMIN; vers <= VERSIONMAX; vers++) {
			svc_unregister(NFS_PROGRAM, vers);
		}
		SVC_DESTROY(xprt);
		u.u_error = EINTR;
	} else {
		svc_run(xprt);		/* never returns */
	}
	u.u_rdir = rdir;
	u.u_cdir = cdir;
}

/*
 * Get file handle system call.
 * Takes open file descriptor and returns a file handle for it.
 */
nfs_getfh()
{
	register struct a {
		int fdes;
		fhandle_t *fhp;
	} *uap;

	uap = (struct a *)u.u_ap;

	getfilehandle(uap->fdes, uap->fhp);
}

getfilehandle(fdes, fhp)
	int fdes;
	fhandle_t *fhp;
{
	register struct file *fp;
	fhandle_t fh;

	if (!suser()) {
		return;
	}
	fp = getf(fdes);
	if (fp == NULL) {
		return;
	}
	u.u_error = makefh(&fh, fp->f_inode);
	if (!u.u_error) {
		if(copyout((caddr_t)&fh, (caddr_t)fhp, sizeof(fh)))
			u.u_error = EFAULT;
	}
}

nfs_setdomainname()
{
	register struct a {
		char *name;
		int namelen;
	} *uap;

	uap = (struct a *)u.u_ap;

	setdomainname(uap->name, uap->namelen);
}

setdomainname(name, namelen)
	char *name;
	int namelen;
{

	if (!suser())
		return;
	if (namelen > sizeof(domainname) - 1) {
		u.u_error = EINVAL;
		return;
	}
	if (copyin((caddr_t)name, (caddr_t)domainname, namelen)) {
		u.u_error = EFAULT;
		return;
	}
	domainnamelen = namelen;
	domainname[domainnamelen] = '\0';
}

nfs_getdomainname()
{
	register struct a {
		char *name;
		int namelen;
	} *uap;

	uap = (struct a *)u.u_ap;

	getdomainname(uap->name, uap->namelen);
}

getdomainname(name, namelen)
	char *name;
	int namelen;
{
	register int len;

	len = MIN(namelen, domainnamelen + 1);
	if (copyout((caddr_t)domainname, (caddr_t)name, len))
		u.u_error = EFAULT;
}


/*
 * These are the interface routines for the server side of the
 * Networked File System.  See the NFS protocol specification
 * for a description of this interface.
 */

/*
 * Get file attributes.
 * Returns the current attributes of the file with the given fhandle.
 */
int
rfs_getattr(fhp, ns)
	fhandle_t *fhp;
	register struct nfsattrstat *ns;
{
	int error = 0;
	register struct inode *ip;

	ip = fhtoip(fhp);
	if (ip == NULL) {
		ns->ns_status = NFSERR_STALE;
		return;
	}
	iattr_to_nattr(ip, &ns->ns_attr);
	ns->ns_status = puterrno(error);
	iput(ip);
}

/*
 * Set file attributes.
 * Sets the attributes of the file with the given fhandle.
 * Returns the new attributes.
 */
int
rfs_setattr(args, ns)
	struct nfssaargs *args;
	register struct nfsattrstat *ns;
{
	int error;
	register struct inode *ip;

	ip = fhtoip(&args->saa_fh);
	if (ip == NULL) {
		ns->ns_status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
	} else {
		error = rfs_do_setattr(ip, &args->saa_sa);
		if (!error) {
			iattr_to_nattr(ip, &ns->ns_attr);
		}
	}
	ns->ns_status = puterrno(error);
	iput(ip);
}

/*
 * Directory lookup.
 * Returns an fhandle and file attributes for file name in a directory.
 */
int
rfs_lookup(da, dr)
	struct nfsdiropargs *da;
	register struct nfsdiropres *dr;
{
	int error = 0;
	register struct inode *dip, *ip;
	register struct mount *mp;
	struct nx nx;
	extern struct inode *ifind();

	dip = fhtoip(&da->da_fhandle);
	if (dip == NULL) {
		dr->dr_status = NFSERR_STALE;
		return;
	}

	/*
	 * Make sure that we have permission to do a lookup in this
	 * directory.
	 */
	if (dip->i_ftype != IFDIR || dip->i_nlink == 0) {
		error = ENOTDIR;
		iput(dip);
	} else if (FS_ACCESS(dip, ICDEXEC)) {
		error = u.u_error;
		iput(dip);
	} else {
		/*
		 * do lookup.
		 */
		nx.dp = dip;
		nx.comp = da->da_name;
		nx.bufp = da->da_name;
		nx.flags = 0;
		if (FS_NAMEI(dip, &nx, (struct argnamei *)0) == NI_FAIL)
			error = u.u_error;
	}
	if (!error) {
		mp = dip->i_mntdev;
		iput(dip);
		if (ip = ifind(mp, nx.ino)) {
			error = makefh(&dr->dr_fhandle, ip);
			if (!error)
				iattr_to_nattr(ip, &dr->dr_attr);
			iput(ip);
		}
	}
	dr->dr_status = puterrno(error);
}

/*
 * Read symbolic link.
 *
 * V5r3 does not support symbolic links.  We should
 * update this code when it does.
 */
/* ARGSUSED */
int
rfs_readlink(fhp, rl)
	fhandle_t *fhp;
	register struct nfsrdlnres *rl;
{

	rl->rl_status = NFSERR_OPNOTSUPP;
}

/*
 * Free data allocated by rfs_readlink
 */
rfs_rlfree(rl)
	struct nfsrdlnres *rl;
{

	if (rl->rl_data) {
		kmem_free((caddr_t)rl->rl_data, (uint)NFS_MAXPATHLEN);
	}
}

/*
 * Read data.
 * Returns some data read from the file at the given fhandle.
 */
int
rfs_read(ra, rr)
	struct nfsreadargs *ra;
	register struct nfsrdresult *rr;
{
	int error;
	struct inode *ip;

	ip = fhtoip(&ra->ra_fhandle);
	if (ip == NULL) {
		rr->rr_status = NFSERR_STALE;
		return;
	}
	/*
	 * This is a kludge to allow reading of files created
	 * with no read permission.  The owner of the file
	 * is always allowed to read it.
	 */
	if (u.u_uid != ip->i_uid) {
		if (FS_ACCESS(ip, IREAD)) {
			u.u_error = 0;
			/*
			 * Exec is the same as read over the net because
			 * of demand loading.
			 */
			if (FS_ACCESS(ip, IEXEC)) {
				error = u.u_error;
				goto done;
			}
		}
	}

	/*
	 * Allocate space for data.  This will be freed by
	 * xdr_rdresult when it is called with x_op = XDR_FREE.
	 */
	rr->rr_data = (char *)kmem_alloc((uint)ra->ra_count);

	/*
	 * Set up UBLOCK io arguments
	 */
	u.u_count = ra->ra_count;
	u.u_base = rr->rr_data;
	u.u_offset = ra->ra_offset;
	u.u_segflg = 1;

	/*
	 * For now we ignore totcount. (read ahead)
	 */
	FS_READI(ip);
	error = u.u_error;
done:
	if (!error) {
		rr->rr_count = ra->ra_count - u.u_count;
		/*
		 * Free the unused part of the data allocated.
		 */
		if (u.u_count) {
			kmem_free((caddr_t)(rr->rr_data + rr->rr_count),
				  u.u_count);
		}
		iattr_to_nattr(ip, &rr->rr_attr);
	} else if (rr->rr_data != NULL) {
		kmem_free((caddr_t)rr->rr_data, (uint)ra->ra_count);
		rr->rr_data = NULL;
		rr->rr_count = 0;
	}
	rr->rr_status = puterrno(error);
	iput(ip);
}

/*
 * Free data allocated by rfs_read.
 */
rfs_rdfree(rr)
	struct nfsrdresult *rr;
{

	if (rr->rr_data) {
		kmem_free((caddr_t)rr->rr_data, (uint)rr->rr_count);
	}
}

/*
 * Write data to file.
 * Returns attributes of a file after writing some data to it.
 */
int
rfs_write(wa, ns)
	struct nfswriteargs *wa;
	struct nfsattrstat *ns;
{
	register int error = 0;
	struct inode *ip;

	ip = fhtoip(&wa->wa_fhandle);
	if (ip == NULL) {
		ns->ns_status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
	} else if (ip->i_ftype != IFREG) {
		error = EACCES;
	} else {
		if (u.u_uid != ip->i_uid) {
			/*
			 * This is a kludge to allow writes of files created
			 * with read only permission.  The owner of the file
			 * is always allowed to write it.
			 */
			if (FS_ACCESS(ip, IWRITE)) {
				error = u.u_error;
			}

		}
		if (!error) {
			u.u_base = wa->wa_data;
			u.u_count = wa->wa_count;
			u.u_offset = wa->wa_offset;
			u.u_fmode = FWRITE | FSYNC;
			u.u_segflg = 1;

			/*
			 * For now we assume no append mode.
			 */
			FS_WRITEI(ip);
			error = u.u_error;
		}
	}
	if (!error) {
		/*
		 * Get attributes again so we send the latest mod
		 * time to the client side for his cache.
		 */
		iattr_to_nattr(ip, &ns->ns_attr);
	}
	ns->ns_status = puterrno(error);
	iput(ip);
}

/*
 * Create a file.
 * Creates a file with given attributes and returns those attributes
 * and an fhandle for the new file.
 */
int
rfs_create(args, dr)
	struct nfscreatargs *args;
	struct nfsdiropres *dr;
{
	register int error = 0;
	struct inode *ip;
	struct nx nx;
	struct argnamei narg;

	/*
	 * XXX Should get exclusive flag and use it.
	 */
	ip = fhtoip(&args->ca_da.da_fhandle);
	if (ip == NULL) {
		dr->dr_status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
		iput(ip);
	} else if (ip->i_ftype != IFDIR || ip->i_nlink == 0) {
		error = ENOTDIR;
		iput(ip);
	} else {
		int doiput = 1;

		nx.dp = ip;
		nx.comp = args->ca_da.da_name;
		nx.bufp = args->ca_da.da_name;
		nx.flags = 0;
		narg.cmd = NI_CREAT;	/* WHAT ABOUT NI_XCREAT ? */
		narg.mode = args->ca_sa.sa_mode & MODEMSK;
		narg.ftype = 0;
		narg.idev = -1;
		if (FS_NAMEI(ip, &nx, &narg) == NI_FAIL) {
			error = u.u_error;
			doiput = 0;
		}
		ip = nx.dp;
		if (!error && narg.rcode == FSN_FOUND) {
			if (FS_ACCESS(ip, IWRITE)) {
				error = u.u_error;
			} else if (!FS_ACCESS(ip, IMNDLCK) &&
				   ip->i_filocks != NULL) {
				error = u.u_error = EAGAIN;
			} else if (ip->i_ftype == IFREG &&
				   args->ca_sa.sa_size == (ulong)0) {
				FS_ITRUNC(ip);
				error = u.u_error;
			}
		}
		if (!error) {
			error = makefh(&dr->dr_fhandle, ip);
			if (!error)
				iattr_to_nattr(ip, &dr->dr_attr);
		}
		if (doiput)
			iput(ip);
	}
	dr->dr_status = puterrno(error);
}

/*
 * Remove a file.
 * Remove named file from parent directory.
 */
int
rfs_remove(da, status, req)
	struct nfsdiropargs *da;
	enum nfsstat *status;
	struct svc_req *req;
{
	int error = 0;
	register struct inode *ip;
	struct nx nx;
	struct argnamei narg;

	svckudp_dup_enter(req);

	ip = fhtoip(&da->da_fhandle);
	if (ip == NULL) {
		svckudp_dup_fail(req);
		*status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
		iput(ip);
	} else if (ip->i_ftype != IFDIR || ip->i_nlink == 0) {
		error = ENOTDIR;
		iput(ip);
	} else {
		nx.dp = ip;
		nx.comp = da->da_name;
		nx.bufp = nx.comp;
		nx.flags = 0;
		narg.cmd = NI_DEL;
		if (FS_NAMEI(ip, &nx, &narg) == NI_FAIL) {
			/*
			 * Check for a duplicate request.
			 * If the returned error is not ENOENT or if
			 * this was a duplicate request, then return
			 * the error.  Otherwise, it was the result
			 * of a duplicate request and ignore the error.
			 */
			if (u.u_error != ENOENT || !svckudp_dup(req)) {
				error = u.u_error;
			} else {
				u.u_error = 0;
			}
		}
	}
	*status = puterrno(error);

	if (!error) {
		svckudp_dup_success(req);
	} else {
		svckudp_dup_fail(req);
	}
}

/*
 * Rename a file
 * Give a file (from) a new name (to).
 */
int
rfs_rename(args, status, req)
	struct nfsrnmargs *args;
	enum nfsstat *status;
	struct svc_req *req;
{
	int error = 0;
	register struct inode *fdip = NULL;	/* 'from' & 'to' directories */
	register struct inode *tdip = NULL;
	register struct inode *fip = NULL;	/* the files involved */
	register struct inode *tip = NULL;
	struct nx nx;
	struct argnamei narg;

	svckudp_dup_enter(req);

	/*
	 * find the 'to' directory
	 */
	tdip = fhtoip(&args->rna_to.da_fhandle);
	if (tdip == NULL) {
		error = (int) NFSERR_STALE;
		goto out;
	}
	prele(tdip);
	if (rdonly(tdip)) {
		error = EROFS;
		goto out;
	}
	if (tdip->i_ftype != IFDIR || tdip->i_nlink == 0) {
		error = ENOTDIR;
		goto out;
	}

	/*
	 * see if new, 'to' name already exists
	 */
	nx.dp = tdip;
	nx.comp = args->rna_to.da_name;
	nx.bufp = nx.comp;
	nx.flags = 0;
	plock(tdip);
	tdip->i_count++;
	if (FS_NAMEI(tdip, &nx, (struct argnamei *)0) == NI_FAIL)
		error = u.u_error;
	if (error) {
		u.u_error = 0;
		if (error == ENOENT)	/* ok if new name is really new */
			error = 0;
	} else {
		tdip->i_count--;
		prele(tdip);
		tip = iget(tdip->i_mntdev, nx.ino);
		if (!tip) {
			error = EACCES;
		} else {
			prele(tip);
			if (tip->i_ftype == IFDIR)
				error = EISDIR;
		}
	}
	if (error)
		goto out;

	/*
	 * find 'from' directory
	 */
	fdip = fhtoip(&args->rna_from.da_fhandle);
	if (fdip == NULL) {
		error = (int) NFSERR_STALE;
		goto out;
	}
	prele(fdip);
	/*
	 * Make sure we can delete the 'from' entry.
	 */
	if (rdonly(fdip)) {
		error = EROFS;
		goto out;
	}
	if (fdip->i_ftype != IFDIR || fdip->i_nlink == 0) {
		error = ENOTDIR;
		goto out;
	}
	if (FS_ACCESS(fdip, IWRITE)) {
		error = u.u_error;
		goto out;
	}

	/*
	 * Look up the 'from' file, the file to be renamed.
	 */
	nx.dp = fdip;
	nx.comp = args->rna_from.da_name;
	nx.bufp = nx.comp;
	nx.flags = 0;
	plock(fdip);
	if (FS_NAMEI(fdip, &nx, (struct argnamei *)0) == NI_FAIL) {
		/*
		 * Check for a duplicate request.
		 * If the returned error was not ENOENT or if this
		 * was not a duplicate request, then return the error.
		 * Otherwise, this was a duplicate request and ignore
		 * the error.
		 */
		if (u.u_error != ENOENT || !svckudp_dup(req)) {
			error = u.u_error;
		} else {
			error = 0;
		}
		fdip = NULL;
		goto out;
	}
	prele(fdip);

	fip = iget(fdip->i_mntdev, nx.ino);
	if (!fip) {		/* find device # of target file */
		error = u.u_error;
		goto out;
	}
	prele(fip);
	/*
	 * Check to see whether we are attempting to rename a directory
	 * to outside of its old parent directory.  We can not handle
	 * this case. (yet)  Code to update the ".." entry needs to be
	 * added to be able to handle this case.  For the moment, we
	 * just fail these requests.
	 */
	if (fip->i_ftype == IFDIR && tdip != fdip) {
		error = EINVAL;
		goto out;
	}
	if (tdip->i_dev != fip->i_dev) {
		error = EXDEV;
		goto out;
	}
	if (tip) {			/* remove old target name */
		nx.dp = tdip;
		nx.comp = args->rna_to.da_name;
		nx.bufp = nx.comp;
		nx.flags = 0;
		narg.cmd = NI_DEL;
		plock(tdip);
		tdip->i_count++;
		if (FS_NAMEI(tdip, &nx, &narg) == NI_FAIL) {
			error = u.u_error;
			goto out;
		}
	}

	/*
	 * now make the link.
	 */
	nx.dp = tdip;
	nx.comp = args->rna_to.da_name;
	nx.bufp = nx.comp;
	nx.flags = 0;
	narg.ino = fip->i_number;
	narg.cmd = NI_LINK;
	narg.idev = fip->i_dev;
	plock(fip);
	iput(fip);
	fip = NULL;
	plock(tdip);
	tdip->i_count++;
	if (FS_NAMEI(tdip, &nx, &narg) == NI_FAIL)
		error = u.u_error;
	if (error) {			/* if link fails */
		if (tip) {		/* restore old link */
			nx.dp = tdip;
			nx.comp = args->rna_to.da_name;
			nx.bufp = nx.comp;
			nx.flags = 0;
			narg.ino = tip->i_number;
			narg.cmd = NI_LINK;
			narg.idev = tip->i_dev;
			plock(tdip);
			tdip->i_count++;
			if (FS_NAMEI(tdip, &nx, &narg) == NI_FAIL)
				cmn_err(CE_WARN, "rfs_rename: relink failed");
		}
		goto out;
	}

	/*
	 * Now unlink the 'from' file from the 'from' directory
	 */
	nx.dp = fdip;
	nx.comp = args->rna_from.da_name;
	nx.bufp = nx.comp;
	nx.flags = 0;
	narg.cmd = NI_DEL;
	u.u_uid = 0;		/* force the unlink to occur */
	plock(fdip);
	if (FS_NAMEI(fdip, &nx, &narg) == NI_FAIL)
		error = u.u_error;
	fdip = NULL;
out:
	if (tdip) {
		plock(tdip);
		iput(tdip);
	}
	if (tip) {
		plock(tip);
		iput(tip);
	}
	if (fdip) {
		plock(fdip);
		iput(fdip);
	}
	if (fip) {
		plock(fip);
		iput(fip);
	}

	*status = puterrno(error);

	if (!error) {
		svckudp_dup_success(req);
	} else {
		svckudp_dup_fail(req);
	}
}

/*
 * Link to a file.
 * Create a file (to) which is a hard link to the given file (from).
 */
int
rfs_link(args, status, req)
	struct nfslinkargs *args;
	enum nfsstat *status;
	struct svc_req *req;
{
	int error = 0;
	register struct inode *fdip, *tdip;
	struct nx nx;
	struct argnamei narg;

	svckudp_dup_enter(req);

	fdip = fhtoip(&args->la_from);	/* find 'from' file */
	if (fdip == NULL) {
		svckudp_dup_fail(req);
		*status = NFSERR_STALE;
		return;
	}
	prele(fdip);
	if (fdip->i_ftype == IFDIR && !suser()) {
		error = u.u_error;
		goto out;
	}
	tdip = fhtoip(&args->la_to.da_fhandle);	/* find target directory */
	if (tdip == NULL) {
		plock(fdip);
		iput(fdip);
		svckudp_dup_fail(req);
		*status = NFSERR_STALE;
		return;
	}
	if (rdonly(tdip)) {
		error = EROFS;
		iput(tdip);
		goto out;
	}
	if (tdip->i_ftype != IFDIR || tdip->i_nlink == 0) {
		error = ENOTDIR;
		iput(tdip);
		goto out;
	}
	nx.dp = tdip;			/* Now do the link */
	nx.comp = args->la_to.da_name;
	nx.bufp = nx.comp;
	narg.cmd = NI_LINK;
	narg.idev = fdip->i_dev;
	narg.ino = fdip->i_number;
	if (FS_NAMEI(tdip, &nx, &narg) == NI_FAIL) {
		/*
		 * Check for a duplicate request.
		 * If the returned error was not EEXIST or if this
		 * was not a duplicate request, then return the error.
		 * Otherwise, this was a duplicate request and ignore
		 * the error.
		 */
		if (u.u_error != EEXIST || !svckudp_dup(req)) {
			error = u.u_error;
		} else {
			u.u_error = 0;
		}
	}
out:
	plock(fdip);
	iput(fdip);

	*status = puterrno(error);

	if (!error) {
		svckudp_dup_success(req);
	} else {
		svckudp_dup_fail(req);
	}
}

/*
 * Symbolicly link to a file.
 * Create a file (to) with the given attributes which is a symbolic link
 * to the given path name (to).
 *
 * V5r3 does not support symbolic links.  When it does, we should update
 * this code.
 */
/* ARGSUSED */
int
rfs_symlink(args, status)
	struct nfsslargs *args;
	enum nfsstat *status;
{

	*status = NFSERR_OPNOTSUPP;
}

/*
 * Make a directory.
 * Create a directory with the given name, parent directory, and attributes.
 * Returns a file handle and attributes for the new directory.
 */
int
rfs_mkdir(args, dr, req)
	struct nfscreatargs *args;
	struct nfsdiropres *dr;
	struct svc_req *req;
{
	int error = 0;
	struct mount *mp;
	register struct inode *ip;
	struct nx nx;
	struct argnamei narg;

	svckudp_dup_enter(req);

	/*
	 * Should get exclusive flag and pass it on here
	 */
	ip = fhtoip(&args->ca_da.da_fhandle);
	if (ip == NULL) {
		svckudp_dup_fail(req);
		dr->dr_status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
		iput(ip);
	} else if (ip->i_ftype != IFDIR || ip->i_nlink == 0) {
		error = ENOTDIR;
		iput(ip);
	} else {
		nx.dp = ip;
		nx.comp = args->ca_da.da_name;
		nx.bufp = nx.comp;
		nx.flags = 0;
		narg.cmd = NI_MKDIR;
		narg.mode = args->ca_sa.sa_mode & PERMMSK;
		ip->i_count++;
		if (FS_NAMEI(ip, &nx, &narg) == NI_FAIL) {
			/*
			 * Check for a duplicate request.
			 * If the returned error was not EEXIST or
			 * this was not from a duplicate request, then
			 * return the error.  Otherwise, this was the
			 * result from a duplicate request and ignore
			 * the error.
			 */
			if (u.u_error != EEXIST || !svckudp_dup(req)) {
				error = u.u_error;
			} else {
				u.u_error = 0;
			}
		}
	}
	if (!error) {
		/*
		 * The namei has done an iput on the new directory inode,
		 * so use another namei to get the inumber.
		 */
		nx.dp = ip;
		nx.comp = args->ca_da.da_name;
		nx.bufp = nx.comp;
		nx.flags = 0;
		if (FS_NAMEI(ip, &nx, (struct argnamei *)0) == NI_FAIL)
			error = u.u_error;
		if (!error) {
			mp = ip->i_mntdev;
			plock(ip);
			iput(ip);
			if (ip = iget(mp, nx.ino)) {
				error = makefh(&dr->dr_fhandle, ip);
				if (!error)
					iattr_to_nattr(ip, &dr->dr_attr);
				iput(ip);
			} else
				error = u.u_error;
		}
	}
	dr->dr_status = puterrno(error);

	if (!error) {
		svckudp_dup_success(req);
	} else {
		svckudp_dup_fail(req);
	}
}

/*
 * Remove a directory.
 * Remove the given directory name from the given parent directory.
 */
int
rfs_rmdir(da, status, req)
	struct nfsdiropargs *da;
	enum nfsstat *status;
	struct svc_req *req;
{
	int error = 0;
	register struct inode *ip;
	struct nx nx;
	struct argnamei narg;

	svckudp_dup_enter(req);

	ip = fhtoip(&da->da_fhandle);
	if (ip == NULL) {
		svckudp_dup_fail(req);
		*status = NFSERR_STALE;
		return;
	}
	if (rdonly(ip)) {
		error = EROFS;
		iput(ip);
	} else if (ip->i_ftype != IFDIR || ip->i_nlink == 0) {
		error = ENOTDIR;
		iput(ip);
	} else {
		nx.dp = ip;
		nx.comp = da->da_name;
		nx.bufp = nx.comp;
		nx.flags = 0;
		narg.cmd = NI_RMDIR;
		if (FS_NAMEI(ip, &nx, &narg) == NI_FAIL) {
			/*
			 * Check for a duplicate request.
			 * If the error returned was not ENOENT or
			 * if this was not a duplicate request, then
			 * return the error.  Otherwise, this was a
			 * duplicate request and just ignore the error.
			 */
			if (u.u_error != ENOENT || !svckudp_dup(req)) {
				error = u.u_error;
			} else {
				u.u_error = 0;
			}
		}
	}
	*status = puterrno(error);

	if (!error) {
		svckudp_dup_success(req);
	} else {
		svckudp_dup_fail(req);
	}
}

/*
 * Read the contents of the given directory.
 */
int
rfs_readdir(rda, rd)
	struct nfsrddirargs *rda;
	register struct nfsrddirres *rd;
{
	int error = 0;
	register int num;
	register struct inode *ip;

	ip = fhtoip(&rda->rda_fh);
	if (ip == NULL) {
		rd->rd_status = NFSERR_STALE;
		return;
	}
	/*
	 * Check read access to dir.  We have to do this here because
	 * the opendir does not go over the wire.
	 */
	if (FS_ACCESS(ip, IREAD)) {
		error = u.u_error;
		goto bad;
	}
	/*
	 * Allocate space for entries.  This will be freed by rfs_rddirfree.
	 */
	rd->rd_entries = (struct dirent *)kmem_alloc((uint)rda->rda_count);
	rd->rd_bufsize = rda->rda_count;
	rd->rd_offset = rda->rda_offset;
	u.u_offset = rda->rda_offset;
	u.u_segflg = 1;

	num = FS_GETDENTS(ip, (char *)rd->rd_entries, rd->rd_bufsize);

	if (num < 0) {
		rd->rd_size = 0;
		error = u.u_error;
		goto bad;
	}
	rd->rd_size = num;
	rd->rd_eof = u.u_offset == ip->i_size ? TRUE : FALSE;
	rd->rd_offset = u.u_offset;
bad:
	rd->rd_status = puterrno(error);
	iput(ip);
}

/*
 * Free data allocated by rfs_readdir.
 */
rfs_rddirfree(rd)
	struct nfsrddirres *rd;
{

        if (rd->rd_entries) {
		kmem_free((caddr_t)rd->rd_entries, (uint)rd->rd_bufsize);
	}
}

/*
 * Return the file system statistics for the file system
 * containing the given file.
 */
rfs_statfs(fh, fs)
	fhandle_t *fh;
	register struct nfsstatfs *fs;
{
	int error;
	struct statfs sb;
	register struct inode *ip;

	ip = fhtoip(fh);
	if (ip == NULL) {
		fs->fs_status = NFSERR_STALE;
		return;
	}

	(*fstypsw[(ip)->i_fstyp].fs_statfs)(ip, &sb, 0);

	error = u.u_error;
	fs->fs_status = puterrno(error);
	if (!error) {
		fs->fs_tsize = nfs_tsize();
		fs->fs_bsize = NBPSCTR;	/* S5.3 reports in these units */
		fs->fs_blocks = sb.f_blocks;
		fs->fs_bfree = sb.f_bfree;
		fs->fs_bavail = sb.f_bfree;
	}
	iput(ip);
}

/* ARGSUSED */
rfs_null(argp, resp)
	caddr_t *argp;
	caddr_t *resp;
{
	/* do nothing */
}

/* ARGSUSED */
rfs_error(argp, resp)
	caddr_t *argp;
	caddr_t *resp;
{
	/* do nothing */
}

int
nullfree()
{
}

/*
 * rfs dispatch table
 * Indexed by version,proc
 */

struct rfsdisp {
	int	  (*dis_proc)();	/* proc to call */
	xdrproc_t dis_xdrargs;		/* xdr routine to get args */
	int	  dis_argsz;		/* sizeof args */
	xdrproc_t dis_xdrres;		/* xdr routine to put results */
	int	  dis_ressz;		/* size of results */
	int	  (*dis_resfree)();	/* frees space allocated by proc */
} rfsdisptab[][RFS_NPROC]  = {
	{
	/*
	 * VERSION 2
	 * Changed rddirres to have eof at end instead of beginning
	 */
	/* RFS_NULL = 0 */
	{rfs_null, xdr_void, 0,
	    xdr_void, 0, nullfree},
	/* RFS_GETATTR = 1 */
	{rfs_getattr, xdr_fhandle, sizeof(fhandle_t),
	    xdr_attrstat, sizeof(struct nfsattrstat), nullfree},
	/* RFS_SETATTR = 2 */
	{rfs_setattr, xdr_saargs, sizeof(struct nfssaargs),
	    xdr_attrstat, sizeof(struct nfsattrstat), nullfree},
	/* RFS_ROOT = 3 *** NO LONGER SUPPORTED *** */
	{rfs_error, xdr_void, 0,
	    xdr_void, 0, nullfree},
	/* RFS_LOOKUP = 4 */
	{rfs_lookup, xdr_diropargs, sizeof(struct nfsdiropargs),
	    xdr_diropres, sizeof(struct nfsdiropres), nullfree},
	/* RFS_READLINK = 5 */
	{rfs_readlink, xdr_fhandle, sizeof(fhandle_t),
	    xdr_rdlnres, sizeof(struct nfsrdlnres), rfs_rlfree},
	/* RFS_READ = 6 */
	{rfs_read, xdr_readargs, sizeof(struct nfsreadargs),
	    xdr_rdresult, sizeof(struct nfsrdresult), rfs_rdfree},
	/* RFS_WRITECACHE = 7 *** NO LONGER SUPPORTED *** */
	{rfs_error, xdr_void, 0,
	    xdr_void, 0, nullfree},
	/* RFS_WRITE = 8 */
	{rfs_write, xdr_writeargs, sizeof(struct nfswriteargs),
	    xdr_attrstat, sizeof(struct nfsattrstat), nullfree},
	/* RFS_CREATE = 9 */
	{rfs_create, xdr_creatargs, sizeof(struct nfscreatargs),
	    xdr_diropres, sizeof(struct nfsdiropres), nullfree},
	/* RFS_REMOVE = 10 */
	{rfs_remove, xdr_diropargs, sizeof(struct nfsdiropargs),
	    xdr_enum, sizeof(enum nfsstat), nullfree},
	/* RFS_RENAME = 11 */
	{rfs_rename, xdr_rnmargs, sizeof(struct nfsrnmargs),
	    xdr_enum, sizeof(enum nfsstat), nullfree},
	/* RFS_LINK = 12 */
	{rfs_link, xdr_linkargs, sizeof(struct nfslinkargs),
	    xdr_enum, sizeof(enum nfsstat), nullfree},
	/* RFS_SYMLINK = 13 */
	{rfs_symlink, xdr_slargs, sizeof(struct nfsslargs),
	    xdr_enum, sizeof(enum nfsstat), nullfree},
	/* RFS_MKDIR = 14 */
	{rfs_mkdir, xdr_creatargs, sizeof(struct nfscreatargs),
	    xdr_diropres, sizeof(struct nfsdiropres), nullfree},
	/* RFS_RMDIR = 15 */
	{rfs_rmdir, xdr_diropargs, sizeof(struct nfsdiropargs),
	    xdr_enum, sizeof(enum nfsstat), nullfree},
	/* RFS_READDIR = 16 */
	{rfs_readdir, xdr_rddirargs, sizeof(struct nfsrddirargs),
	    xdr_putrddirres, sizeof(struct nfsrddirres), rfs_rddirfree},
	/* RFS_STATFS = 17 */
	{rfs_statfs, xdr_fhandle, sizeof(fhandle_t),
	    xdr_statfs, sizeof(struct nfsstatfs), nullfree},
	}
};

struct rfsspace {
	struct rfsspace *rs_next;
	caddr_t rs_dummy;
};

struct rfsspace *rfsfreesp = NULL;

int rfssize = 0;

caddr_t
rfsget()
{
	register int i;
	register struct rfsdisp *dis;
	register caddr_t ret;

	if (rfssize == 0) {
		for (i = 0; i < 1 + VERSIONMAX - VERSIONMIN; i++) {
			for (dis = &rfsdisptab[i][0];
			     dis < &rfsdisptab[i][RFS_NPROC];
			     dis++) {
				rfssize = MAX(rfssize, dis->dis_argsz);
				rfssize = MAX(rfssize, dis->dis_ressz);
			}
		}
	}

	if (rfsfreesp) {
		ret = (caddr_t)rfsfreesp;
		rfsfreesp = rfsfreesp->rs_next;
	} else {
		ret = kmem_alloc((uint)rfssize);
	}
	return(ret);
}

rfsput(rs)
	struct rfsspace *rs;
{

	rs->rs_next = rfsfreesp;
	rfsfreesp = rs;
}

int nobody = -2;

/*
 * If nfs_portmon is set, then clients are required to use
 * privileged ports (ports < IPPORT_RESERVED) in order to get NFS services.
 */
int nfs_portmon = 0;

void
rfs_dispatch(req, xprt)
	struct svc_req *req;
	register SVCXPRT *xprt;
{
	int which;
	int vers;
	caddr_t	*args = NULL;
	caddr_t	*res = NULL;
	register struct rfsdisp *disp;
	struct authunix_parms *aup;
	struct ucred *oldcr = NULL;
	int error;
	int i;

	svstat.ncalls++;
	error = 0;
	which = req->rq_proc;
	if (which < 0 || which >= RFS_NPROC) {
		svcerr_noproc(req->rq_xprt);
		error++;
		goto done;
	}
	vers = req->rq_vers;
	if (vers < VERSIONMIN || vers > VERSIONMAX) {
		svcerr_progvers(req->rq_xprt, (ulong)VERSIONMIN,
				(ulong)VERSIONMAX);
		error++;
		goto done;
	}
	vers -= VERSIONMIN;
	disp = &rfsdisptab[vers][which];

	/*
	 * Clean up as if a system call just started
	 */
	u.u_error = 0;

	/*
	 * Allocate args struct and deserialize into it.
	 */
	args = (caddr_t *)rfsget();
	bzero((caddr_t)args, disp->dis_argsz);
	if (!SVC_GETARGS(xprt, disp->dis_xdrargs, args)) {
		svcerr_decode(xprt);
		error++;
		goto done;
	}

	/*
	 * Check for unix style credentials
	 */
	if (req->rq_cred.oa_flavor != AUTH_UNIX && which != RFS_NULL) {
		svcerr_weakauth(xprt);
		error++;
		goto done;
	}

	/***** portmon check was here. *****/

	/*
	 * Set uid, gid, and gids to auth params
	 */
	if (which != RFS_NULL) {
		aup = (struct authunix_parms *)req->rq_clntcred;
		oldcr = crget();
		oldcr->cr_uid = u.u_uid;
		if (aup->aup_uid == 0) {
			/*
			 * root over the net becomes nobody on the server
			 */
			u.u_uid = nobody;
		} else {
			u.u_uid = aup->aup_uid;
		}
		oldcr->cr_gid = u.u_gid;
		u.u_gid = aup->aup_gid;
#ifdef multiple_groups
		for (i = 0; i < aup->aup_len; i++) {
			oldcr->cr_groups[i] = u.u_groups[i];
			u.u_groups[i] = aup->aup_gids[i];
		}
		for (; i < NGROUPS; i++) {
			oldcr->cr_groups[i] = u.u_groups[i];
			u.u_groups[i] = NOGROUP;
		}
#endif
	}

	/*
	 * Allocate results struct.
	 */
	res = (caddr_t *)rfsget();
	bzero((caddr_t)res, disp->dis_ressz);

	svstat.reqs[which]++;

	/*
	 * Call service routine with arg struct and results struct
	 */
	(*disp->dis_proc)(args, res, req);

done:
	/*
	 * Free arguments struct
	 */
	if (!SVC_FREEARGS(xprt, disp->dis_xdrargs, args)) {
		error++;
	}
	if (args != NULL) {
		rfsput((struct rfsspace *)args);
	}

	/*
	 * Serialize and send results struct
	 */
	if (!error) {
		if (!svc_sendreply(xprt, disp->dis_xdrres, (caddr_t)res)) {
			error++;
		}
	}

	/*
	 * Free results struct
	 */
	if (res != NULL) {
		if (disp->dis_resfree != nullfree) {
			(*disp->dis_resfree)(res);
		}
		rfsput((struct rfsspace *)res);
	}
	/*
	 * restore original credentials
	 */
	if (oldcr) {
		u.u_uid = oldcr->cr_uid;
		u.u_gid = oldcr->cr_gid;
#ifdef multiple_groups
		for (i = 0; i < NGROUPS; i++)
			u.u_groups[i] = oldcr->cr_groups[i];
#endif
		crfree(oldcr);
	}
	svstat.nbadcalls += error;
}
