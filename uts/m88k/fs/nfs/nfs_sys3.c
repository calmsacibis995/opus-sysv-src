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
static char SysVr3NFSID[] = "@(#)nfs_sys3.c	4.3 LAI System V NFS Release 3.2/V3	source";
#endif

/*
 * Some code from this module was taken from Convergent's nfs_sys3.c:
 * 
 * 	Convergent Technologies - System V - Nov 1986
 * 	#ident  "@(#)nfs_sys3.c 1.5 :/source/uts/common/fs/nfs/s.nfs_sys3.c 1/20/87 00:04:16"
 * 
 *	#ifdef LAI
 * 	static char SysVNFSID[] = "@(#)nfs_sys3.c       2.9 LAI System V NFS Release 3.0/V3 source";
 *	#endif
 */

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/fcntl.h"
#include "sys/flock.h"
#if clipper || ns32000 || m88k
#include "sys/socket.h"
#endif
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
#include "sys/stat.h"
#include "sys/statfs.h"

#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/auth.h"
#include "sys/fs/nfs/auth_unix.h"
#include "sys/fs/nfs/clnt.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/ucred.h"
#include "sys/fs/nfs/nfs.h"
#include "sys/fs/nfs/nfs_clnt.h"
#include "sys/fs/nfs/rnode.h"
#include "sys/fs/nfs/mount.h"

#include "sys/stream.h"

extern caddr_t kmem_alloc();
extern void kmem_free();

#if !m88k
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

extern struct rnode *nfs_iget();

/* nfs global external data */
extern int nfsfstyp;			/* FSS fstyp[] index for NFS */
extern int nfsd_maj;			/* major device # */

extern int lockd_open;			/* lock manager running? */

nfsstatf(ip, sfp)
	register struct inode *ip;
	register struct ostat *sfp;
{
	int error;
	struct nfsfattr *atp;
	struct rnode *rp;
	struct ucred cred;
	ushort ntype_to_itype();

	set_cred(&cred);
	rp = itor(ip);

	nfs_unmark_iget(rp);

	atp = &rp->r_nfsattr;

	error = nfs_getattr(ip, &cred);
	if (error) {
		u.u_error = error;
		return;
	}

	/*
	 * basic attributes - no question here
	 */
	sfp->st_mode = atp->na_mode & MODEMSK;	/* definitions of both match */
	sfp->st_nlink = atp->na_nlink;
	sfp->st_uid = atp->na_uid;
	sfp->st_gid = atp->na_gid;
	sfp->st_rdev = atp->na_rdev;
	sfp->st_size = atp->na_size;
	sfp->st_atime = atp->na_atime.tv_sec;
	sfp->st_mtime = atp->na_mtime.tv_sec;
	sfp->st_ctime = atp->na_ctime.tv_sec;

	/*
	 * S5 attributes or problematicals
	 */
	sfp->st_dev = ip->i_mntdev->m_dev;
	hibyte(sfp->st_dev) = ~hibyte(sfp->st_dev);

	/*
	 * Overwrite the inumber already stored in the stat buffer.
	 * We need to do this because the inumber stored in the inode
	 * is an internal number only, not the external node number.
	 */
	sfp->st_ino = atp->na_nodeid;

	/*
	 * ntype_to_itype really returns IF*, when we should use SF*.
	 * As it turns out they map the same. NOTE: for an undefined net
	 * type (e.g. NFLNK) we return IFMT (i.e. all type bits on).
	 */
	sfp->st_mode |= ntype_to_itype(atp->na_type);
}

/*
 * FCNTL
 *
 * This routine is based on s5fcntl().
 */
/* ARGSUSED */
nfsfcntl(ip, cmd, arg, flag, offset)
	register struct inode *ip;
	int cmd, arg, flag;
	off_t offset;
{
	struct flock bf;
	int i;

	nfs_unmark_iget(itor(ip));

	switch (cmd) {
	case F_GETLK:		/* get record lock */
		if (copyin((caddr_t)arg, &bf, sizeof bf))
			u.u_error = EFAULT;
		else if ((i = nfs_reclock(ip, &bf, 0, flag, offset)) != 0)
			u.u_error = i;
		else if (copyout(&bf, (caddr_t)arg, sizeof bf))
			u.u_error = EFAULT;
		break;

	case F_SETLK:		/* set record lock */
		if (copyin((caddr_t)arg, &bf, sizeof bf))
			u.u_error = EFAULT;
		else if ((i = nfs_reclock(ip, &bf, SETFLCK, flag, offset)) != 0) {
			/*
			 * The following if statement is to maintain
			 * backward compatibility. Note: the return
			 * value of reclock was not changed to
			 * EAGAIN because rdwr() also calls reclock(),
			 * and we want rdwr() to meet the standards.
			 */
			if (i == EAGAIN)
				u.u_error = EACCES;
			else
				u.u_error = i;
		}
		break;

	case F_SETLKW:		/* set record lock and wait */
		if (copyin((caddr_t)arg, &bf, sizeof bf))
			u.u_error = EFAULT;
		else if ((i = nfs_reclock(ip, &bf, SETFLCK|SLPFLCK, flag,
					  offset)) != 0) {
			u.u_error = i;
		}
		break;

	case F_CHKFL:
		break;

	default:
		u.u_error = EINVAL;
		break;
	}
}

/*
 * IOCTL
 */
/* ARGSUSED */
nfsioctl(ip, cmd, arg, flag)
	register struct inode *ip;
	int cmd, arg, flag;
{

	nfs_unmark_iget(itor(ip));

	u.u_error = EINVAL;		/* not supported */
}

/*
 * The mount system call.
 */
/* ARGSUSED */
nfsmount(bip, mp, rdonly, dataptr, datalen)
	register struct inode *bip;
	register struct mount *mp;
	register int rdonly;
	char *dataptr;
	int datalen;
{
	int error = 0;
	struct inode *rip = NULL;	/* the server's root */
	struct mntinfo *mi = NULL;	/* mount info */
	struct rnode *rp;		/* intermediate for root inode */
	struct ucred cred;		/* handy credentials for getattr() */
	struct nfsfattr na;		/* root inode attributes in nfs form */
	fhandle_t fh;			/* root fhandle */
	struct nfs_args args;		/* nfs mount arguments */

	prele(bip);

	if (copyin(dataptr, (caddr_t)&args, sizeof(args))) {
                error = EFAULT;
		goto errout;
	}

	/* create an NFS mount record to be linked to the mount entry */

	mi = (struct mntinfo *)kmem_alloc((uint)sizeof(*mi));
	mi->mi_addr = NULL;
	mi->mi_printed = 0;
	mi->mi_timeo = NFS_TIMEO;
	mi->mi_retrans = NFS_RETRIES;
	mi->mi_hard = ((args.flags & NFSMNT_SOFT) == 0);
	/* mi->mi_nomand = ((args.flags & NFSMNT_NOMAND) != 0); */
	mi->mi_nomand = 1;	/* not quite yet ... */
	if (args.flags & NFSMNT_RETRANS) {
		mi->mi_retrans = args.retrans;
		if (args.retrans < 0) {
			error = EINVAL;
			goto errout;
		}
	}
	if (args.flags & NFSMNT_TIMEO) {
		mi->mi_timeo = args.timeo;
		if (args.timeo <= 0) {
			error = EINVAL;
			goto errout;
		}
	}
	/*
	 * Copyin the remote address.
	 */
#if clipper || ns32000 || m88k
	/* changing this requires a change to the mount command also */
	mi->mi_addrlen = (uint)sizeof(args.addr);
	mi->mi_addr = kmem_alloc(mi->mi_addrlen);
	bcopy((char *)&args.addr, (char *)mi->mi_addr, mi->mi_addrlen);
#else
	mi->mi_addrlen = args.ralen;
	mi->mi_addr = kmem_alloc(mi->mi_addrlen);
	if (copyin(args.raddr, mi->mi_addr, args.ralen)) {
                error = EFAULT;
                goto errout;
        }
#endif

	if (args.flags & NFSMNT_HOSTNAME) {
		bcopy(args.hostname, mi->mi_hostname, HOSTNAMESZ);
	} else {
		error = EINVAL;
		goto errout;
	}

	/*
	 * Set up a root inode with appropriate real attributes
	 *
	 * Collect info needed to make the root inode/rnode.
	 * We need the file handle from the mount arguments, and we
	 * need an nfsfattr block, which we set all null - except for
	 * type of DIR.
	 */
	set_cred(&cred);
	bzero((caddr_t)&na, sizeof(na));
	na.na_type = NFDIR;
	bcopy((char *)&args.fh, (char *)&fh, sizeof(fh));

	/*
	 * Get server's filesystem stats and set transfer sizes
	 */
	mi->mi_bsize = SBUFSIZE;
	mi->mi_tsize = MIN(NFS_MAXDATA, nfs_tsize());
	mi->mi_stsize = MIN(NFS_MAXDATA, nfs_stsize());
	if (args.flags & NFSMNT_RSIZE) {
		mi->mi_tsize = MIN(mi->mi_tsize, args.rsize);
		if (args.rsize <= 0) {
			error = EINVAL;
			goto errout;
		}
	}
	if (args.flags & NFSMNT_WSIZE) {
		mi->mi_stsize = MIN(mi->mi_stsize, args.wsize);
		if (args.wsize <= 0) {
			error = EINVAL;
			goto errout;
		}
	}

	mp->m_bufp = (caddr_t)mi;
	mp->m_dev = makedev(nfsd_maj, mp - mount);
	mp->m_bsize = mi->mi_bsize;
	mp->m_fstyp = nfsfstyp;
	if (rdonly & MS_RDONLY)
		mp->m_flags |= MRDONLY;

	/*
	 * Now, obtain an rnode, and associate it with
	 * a locked inode. We set attrtime to 0 and call getattr which
	 * will force a refresh of attributes from the net, and fill in
	 * the new root rnode's attributes.
	 */
	rp = nfs_iget(&fh, &na, mp);
	if (rp == NULL) {
		error = u.u_error;
		goto errout;
	}
	rip = iget(rp->r_mp, rtorno(rp));
	if (rip == NULL) {
		error = u.u_error;
		goto errout;
	}
	rp->r_nfsattrtime = 0;
	error = nfs_getattr(rip, &cred);
	if (error) {
		goto errout;
	}
	rip->i_ftype = ntype_to_itype(rp->r_nfsattr.na_type);
	mi->mi_bsize = MIN(rp->r_nfsattr.na_blocksize, SBUFSIZE);
	mp->m_mount = rip;

	if ((rip->i_flag & IISROOT) == 0)
		rip->i_flag |= IISROOT;
	else {
		error = EBUSY;
		goto errout;
	}
	prele(rip);
	plock(bip);
	return;

errout:
	if (mi) {
		if (mi->mi_addr)
			kmem_free(mi->mi_addr, mi->mi_addrlen);
		kmem_free((caddr_t)mi, (uint)sizeof(*mi));
	}
	if (rip)
		iput(rip);
	if (error)
		u.u_error = error;
	else
		u.u_error = EINVAL;
	plock(bip);
}

/*
 * The umount system call for NFS file systems.
 */
nfsumount(mp)
	register struct mount *mp;
{
	register struct rnode *rp;
	register struct inode *mip;
	struct mntinfo *mi;

	xumount(mp);			/* remove text files */

	dnlc_purge_mp(mp);		/* flush DNLC entries for this fs */

	/*
	 * Check all inodes, and flush all that are on this device.
	 */
	mip = mp->m_mount;
	ASSERT(mip != NULL);
	plock(mip);
	if (mip->i_count != 1) {
		prele(mip);
		u.u_error = EBUSY;
		return;
	}
	for (rp = &nfs_rnodes[0]; rp < &nfs_rnodes[nfs_rnum]; rp++) {

		/*
		 * Only look at rnodes on this file system
		 */
		if (rp->r_mp == mp) {

			/*
			 * Is this rnode active?
			 * If it is and is not the root rnode, then
			 * return an error.  Otherwise, just skip it.
			 */
			if (rp->r_ip != NULL) {
				if (rp->r_ip == mip)
					continue;
				prele(mip);
				u.u_error = EBUSY;
				return;
			}

			/*
			 * Is this rnode unattached but active?
			 */
			if ((rp->r_flags & RSTALE) == 0) {
				prele(mip);
				u.u_error = EBUSY;
				return;
			}

			/* remove it from the hash queue */
			rp->r_back->r_forw = rp->r_forw;
			rp->r_forw->r_back = rp->r_back;
			rp->r_forw = rp->r_back = rp;

			/* flush stale bio cache blocks */
			binval(makedev(nfsd_maj, rtorno(rp)));
		}
	}

	mi = (struct mntinfo *)mp->m_bufp;
	mp->m_bufp = NULL;
	mp->m_mount = NULL;
	binval(mip->i_dev);		/* flush stale bio cache blocks */
	iput(mip);			/* release the root inode */
#if defined(u3b2) || defined(i386) || defined(ns32000) || defined(clipper) || defined(m88k)
	punmount(mp);			/* tell the page cache */
#endif
	kmem_free(mi->mi_addr, mi->mi_addrlen);
	kmem_free((caddr_t)mi, (uint)sizeof(*mi));
}

/*
 * Tell user about a NFS file system.
 */
nfsstatfs(ip, sp, ufstyp)
	register struct inode *ip;
	register struct statfs *sp;
	int ufstyp;
{
	int error = 0;
	struct mntinfo *mi;
	fhandle_t *fh;
	struct nfsstatfs fs;
	struct ucred cred;

	nfs_unmark_iget(itor(ip));

	if (ufstyp) {			/* I hope this does not apply */
		u.u_error = EINVAL;
		return;
	}

	set_cred(&cred);
	mi = itomi(ip);
	fh = itofh(ip);
	error = rfscall(mi, RFS_STATFS, xdr_fhandle, (caddr_t)fh,
			xdr_statfs, (caddr_t)&fs, &cred);
	if (!error) {
		error = geterrno(fs.fs_status);
	}
	if (!error) {
		mi->mi_stsize = min(mi->mi_stsize, fs.fs_tsize);
		sp->f_fstyp = nfsfstyp;
		sp->f_bsize = fs.fs_bsize;
		sp->f_frsize = 0;
		sp->f_blocks = fs.fs_blocks * (fs.fs_bsize >> SCTRSHFT);
		sp->f_bfree = fs.fs_bavail * (fs.fs_bsize >> SCTRSHFT);
		sp->f_files = 0;
		sp->f_ffree = 0;
		bcopy((caddr_t)&mi->mi_hostname[0], (caddr_t)sp->f_fname,
		      sizeof(sp->f_fname));
		sp->f_fpack[0] = '\0';
	} else {
		u.u_error = error;
	}
}
