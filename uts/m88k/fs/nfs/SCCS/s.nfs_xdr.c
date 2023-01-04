h20426
s 00693/00000/00000
d D 1.1 90/03/06 12:43:33 root 1 0
c date and time created 90/03/06 12:43:33 by root
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
/*	Copyright (c) 1985 Sun Microsystems Inc.	*/
#ifndef lint
static char SysVr3NFSID[] = "@(#)nfs_xdr.c	4.1 LAI System V NFS Release 3.2/V3	source";
#endif
/*      @(#)nfs_xdr.c 2.21 85/11/19 SMI      */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#ifdef u3b2
#include "sys/psw.h"
#include "sys/pcb.h"
#endif
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/dirent.h"
#include "sys/errno.h"
#include "sys/stream.h"
#include "sys/cmn_err.h"

#include "sys/fs/nfs/types.h"
#include "sys/fs/nfs/time.h"
#include "sys/fs/nfs/xdr.h"
#include "sys/fs/nfs/nfs.h"

#include "bsd/netinet/in.h"
#include "sys/endian.h"

char *xdropnames[] = {"encode", "decode", "free"};

/*
 * These are the XDR routines used to serialize and deserialize
 * the various structures passed as parameters accross the network
 * between NFS clients and servers.
 */

/*
 * File access handle
 * The fhandle struct is treated a opaque data on the wire
 */
bool_t
xdr_fhandle(xdrs, fh)
	XDR *xdrs;
	fhandle_t *fh;
{

	if (xdr_opaque(xdrs, (caddr_t)fh, NFS_FHSIZE)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_fhandle: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Arguments to remote write and writecache
 */
bool_t
xdr_writeargs(xdrs, wa)
	XDR *xdrs;
	struct nfswriteargs *wa;
{

	if (xdr_fhandle(xdrs, &wa->wa_fhandle) &&
	    xdr_long(xdrs, (long *)&wa->wa_begoff) &&
	    xdr_long(xdrs, (long *)&wa->wa_offset) &&
	    xdr_long(xdrs, (long *)&wa->wa_totcount) &&
	    xdr_bytes(xdrs, &wa->wa_data, (uint *)&wa->wa_count, NFS_MAXDATA)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_writeargs: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * File attributes
 */
bool_t
xdr_fattr(xdrs, na)
	XDR *xdrs;
	register struct nfsfattr *na;
{
	register long *ptr;

	if (xdrs->x_op == XDR_ENCODE) {
		ptr = XDR_INLINE(xdrs, 17 * BYTES_PER_XDR_UNIT);
		if (ptr != NULL) {
			IXDR_PUT_ENUM(ptr, na->na_type);
			IXDR_PUT_LONG(ptr, na->na_mode);
			IXDR_PUT_LONG(ptr, na->na_nlink);
			IXDR_PUT_LONG(ptr, na->na_uid);
			IXDR_PUT_LONG(ptr, na->na_gid);
			IXDR_PUT_LONG(ptr, na->na_size);
			IXDR_PUT_LONG(ptr, na->na_blocksize);
			IXDR_PUT_LONG(ptr, na->na_rdev);
			IXDR_PUT_LONG(ptr, na->na_blocks);
			IXDR_PUT_LONG(ptr, na->na_fsid);
			IXDR_PUT_LONG(ptr, na->na_nodeid);
			IXDR_PUT_LONG(ptr, na->na_atime.tv_sec);
			IXDR_PUT_LONG(ptr, na->na_atime.tv_usec);
			IXDR_PUT_LONG(ptr, na->na_mtime.tv_sec);
			IXDR_PUT_LONG(ptr, na->na_mtime.tv_usec);
			IXDR_PUT_LONG(ptr, na->na_ctime.tv_sec);
			IXDR_PUT_LONG(ptr, na->na_ctime.tv_usec);
			return(TRUE);
		}
	} else {
		ptr = XDR_INLINE(xdrs, 17 * BYTES_PER_XDR_UNIT);
		if (ptr != NULL) {
			na->na_type = IXDR_GET_ENUM(ptr, enum nfsftype);
			na->na_mode = IXDR_GET_LONG(ptr);
			na->na_nlink = IXDR_GET_LONG(ptr);
			na->na_uid = IXDR_GET_LONG(ptr);
			na->na_gid = IXDR_GET_LONG(ptr);
			na->na_size = IXDR_GET_LONG(ptr);
			na->na_blocksize = IXDR_GET_LONG(ptr);
			na->na_rdev = IXDR_GET_LONG(ptr);
			na->na_blocks = IXDR_GET_LONG(ptr);
			na->na_fsid = IXDR_GET_LONG(ptr);
			na->na_nodeid = IXDR_GET_LONG(ptr);
			na->na_atime.tv_sec = IXDR_GET_LONG(ptr);
			na->na_atime.tv_usec = IXDR_GET_LONG(ptr);
			na->na_mtime.tv_sec = IXDR_GET_LONG(ptr);
			na->na_mtime.tv_usec = IXDR_GET_LONG(ptr);
			na->na_ctime.tv_sec = IXDR_GET_LONG(ptr);
			na->na_ctime.tv_usec = IXDR_GET_LONG(ptr);
			return(TRUE);
		}
	}
	if (xdr_enum(xdrs, (enum_t *)&na->na_type) &&
	    xdr_u_long(xdrs, &na->na_mode) &&
	    xdr_u_long(xdrs, &na->na_nlink) &&
	    xdr_u_long(xdrs, &na->na_uid) &&
	    xdr_u_long(xdrs, &na->na_gid) &&
	    xdr_u_long(xdrs, &na->na_size) &&
	    xdr_u_long(xdrs, &na->na_blocksize) &&
	    xdr_u_long(xdrs, &na->na_rdev) &&
	    xdr_u_long(xdrs, &na->na_blocks) &&
	    xdr_u_long(xdrs, &na->na_fsid) &&
	    xdr_u_long(xdrs, &na->na_nodeid) &&
	    xdr_timeval(xdrs, &na->na_atime) &&
	    xdr_timeval(xdrs, &na->na_mtime) &&
	    xdr_timeval(xdrs, &na->na_ctime)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_fattr: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Arguments to remote read
 */
bool_t
xdr_readargs(xdrs, ra)
	XDR *xdrs;
	struct nfsreadargs *ra;
{

	if (xdr_fhandle(xdrs, &ra->ra_fhandle) &&
	    xdr_long(xdrs, (long *)&ra->ra_offset) &&
	    xdr_long(xdrs, (long *)&ra->ra_count) &&
	    xdr_long(xdrs, (long *)&ra->ra_totcount)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_readargs: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Status OK portion of remote read reply
 */
bool_t
xdr_rrok(xdrs, rrok)
	XDR *xdrs;
	struct nfsrrok *rrok;
{

	if (xdr_fattr(xdrs, &rrok->rrok_attr) &&
	    xdr_bytes(xdrs, &rrok->rrok_data, (uint *)&rrok->rrok_count,
		      NFS_MAXDATA)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_rrok: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

struct xdr_discrim rdres_discrim[2] = {
	{ (int)NFS_OK, xdr_rrok },
	{ __dontcare__, NULL_xdrproc_t }
};

/*
 * Reply from remote read
 */
bool_t
xdr_rdresult(xdrs, rr)
	XDR *xdrs;
	struct nfsrdresult *rr;
{

	if (xdr_union(xdrs, (enum_t *)&(rr->rr_status),
		      (caddr_t)&(rr->rr_ok), rdres_discrim, xdr_void)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_rdresult: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * File attributes which can be set
 */
bool_t
xdr_sattr(xdrs, sa)
	XDR *xdrs;
	struct nfssattr *sa;
{

	if (xdr_u_long(xdrs, &sa->sa_mode) &&
	    xdr_u_long(xdrs, &sa->sa_uid) &&
	    xdr_u_long(xdrs, &sa->sa_gid) &&
	    xdr_u_long(xdrs, &sa->sa_size) &&
	    xdr_timeval(xdrs, &sa->sa_atime) &&
	    xdr_timeval(xdrs, &sa->sa_mtime)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_sattr: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

struct xdr_discrim attrstat_discrim[2] = {
	{ (int)NFS_OK, xdr_fattr },
	{ __dontcare__, NULL_xdrproc_t }
};

/*
 * Reply status with file attributes
 */
bool_t
xdr_attrstat(xdrs, ns)
	XDR *xdrs;
	struct nfsattrstat *ns;
{

	if (xdr_union(xdrs, (enum_t *)&(ns->ns_status),
		      (caddr_t)&(ns->ns_attr), attrstat_discrim, xdr_void)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_attrstat: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * NFS_OK part of read sym link reply union
 */
bool_t
xdr_srok(xdrs, srok)
	XDR *xdrs;
	struct nfssrok *srok;
{

	if (xdr_bytes(xdrs, &srok->srok_data, (uint *)&srok->srok_count,
		      NFS_MAXPATHLEN)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_srok: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

struct xdr_discrim rdlnres_discrim[2] = {
	{ (int)NFS_OK, xdr_srok },
	{ __dontcare__, NULL_xdrproc_t }
};

/*
 * Result of reading symbolic link
 */
bool_t
xdr_rdlnres(xdrs, rl)
	XDR *xdrs;
	struct nfsrdlnres *rl;
{

	if (xdr_union(xdrs, (enum_t *)&(rl->rl_status),
		      (caddr_t)&(rl->rl_srok), rdlnres_discrim, xdr_void)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_rdlnres: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Arguments to readdir
 */
bool_t
xdr_rddirargs(xdrs, rda)
	XDR *xdrs;
	struct nfsrddirargs *rda;
{

	if (xdr_fhandle(xdrs, &rda->rda_fh) &&
	    xdr_u_long(xdrs, &rda->rda_offset) &&
	    xdr_u_long(xdrs, &rda->rda_count)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_rddirargs: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Directory read reply:
 * union (enum status) {
 *	NFS_OK: entlist;
 *		boolean eof;
 *	default:
 * }
 *
 * Directory entries
 *	struct  dirent {
 *		long	d_fileno;	* inode number of entry *
 *		long	d_off;		* offset of disk direntory entry *
 *		ushort	d_reclen;	* length of this record *
 *		char	d_name[1];	* name of file *
 *	};
 * are on the wire as:
 * union entlist (boolean valid) {
 * 	TRUE: struct nfsdirent;
 *	      u_long nxtoffset;
 * 	      union entlist;
 *	FALSE:
 * }
 * where struct nfsdirent is:
 * 	struct nfsdirent {
 *		u_long	de_fid;
 *		string	de_name<NFS_MAXNAMELEN>;
 *	}
 */

#define nextdp(dp)	((struct dirent *)((char *)(dp) + (dp)->d_reclen))
#undef DIRSIZ
/*#define DIRSIZ(dp)	(sizeof(struct direct) - MAXNAMLEN + (dp)->d_namlen)*/
#define DIRSIZ(dp)	((dp)->d_reclen)

/*
 * ENCODE ONLY
 */
bool_t
xdr_putrddirres(xdrs, rd)
	XDR *xdrs;
	struct nfsrddirres *rd;
{
	struct dirent *dp;
	char *name;
	int size;
	int xdrpos;
	uint namlen;
	ulong offset;
	bool_t true = TRUE;
	bool_t false = FALSE;

	if (xdrs->x_op != XDR_ENCODE) {
		return(FALSE);
	}
	if (!xdr_enum(xdrs, (enum_t *)&rd->rd_status)) {
		return(FALSE);
	}
	if (rd->rd_status != NFS_OK) {
		return(TRUE);
	}

	xdrpos = XDR_GETPOS(xdrs);
	for (offset = rd->rd_offset, size = rd->rd_size, dp = rd->rd_entries;
	     size > 0;
	     size -= dp->d_reclen, dp = nextdp(dp)) {
		if (dp->d_reclen == 0 /*|| DIRSIZ(dp) > dp->d_reclen */) {
			return(FALSE);
		}
		if (dp->d_ino == 0) {
			continue;
		}
		name = dp->d_name;
		namlen = nfs_strlen(dp->d_name);
		if (!xdr_bool(xdrs, &true) ||
		    !xdr_u_long(xdrs, &dp->d_ino) ||
		    !xdr_bytes(xdrs, &name, &namlen, NFS_MAXNAMLEN) ||
		    !xdr_u_long(xdrs, &dp->d_off)) {
			return(FALSE);
		}

		if (XDR_GETPOS(xdrs) - xdrpos >= rd->rd_bufsize) {
			rd->rd_eof = FALSE;
			break;
		}
	}
	if (!xdr_bool(xdrs, &false)) {
		return(FALSE);
	}
	if (!xdr_bool(xdrs, &rd->rd_eof)) {
		return(FALSE);
	}
	return(TRUE);
}

#define roundtoint(x)	(((x) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define reclen(namlen)	roundtoint(dirsz + (namlen) + 1)

/*
 * DECODE ONLY
 */
bool_t
xdr_getrddirres(xdrs, rd)
	XDR *xdrs;
	struct nfsrddirres *rd;
{
	struct dirent *dp;
	ushort namlen;
	int size;
	bool_t valid;
	ulong offset = (ulong)-1;
	int dirsz;
	ulong ino;

	if (xdrs->x_op != XDR_DECODE) {
		return(FALSE);
	}
	if (!xdr_enum(xdrs, (enum_t *)&rd->rd_status)) {
		return(FALSE);
	}
	if (rd->rd_status != NFS_OK) {
		return(TRUE);
	}

	size = rd->rd_size;
	dp = rd->rd_entries;
	dirsz = dp->d_name - (char *)dp;
	for (;;) {
		if (!xdr_bool(xdrs, &valid)) {
			return(FALSE);
		}
		if (valid) {
			if (!xdr_u_long(xdrs, &ino) ||
			    !xdr_u_short(xdrs, &namlen)) {
				return(FALSE);
			}
			if (reclen(namlen) > size) {
				goto bufovflow;
			}
			if (!xdr_opaque(xdrs, dp->d_name, namlen) ||
			    !xdr_u_long(xdrs, &offset)) {
				return(FALSE);
			}
			dp->d_ino = ino;
			dp->d_reclen = reclen(namlen);
			dp->d_name[namlen] = '\0';
			dp->d_off = offset;
		} else {
			break;
		}
		size -= dp->d_reclen;
		dp = nextdp(dp);
	}
	if (!xdr_bool(xdrs, &rd->rd_eof)) {
		return(FALSE);
	}
bufovflow:
	rd->rd_size = (char *)dp - (char *)(rd->rd_entries);
	rd->rd_offset = offset;
	return(TRUE);
}

/*
 * Arguments for directory operations
 */
bool_t
xdr_diropargs(xdrs, da)
	XDR *xdrs;
	struct nfsdiropargs *da;
{

	if (xdr_fhandle(xdrs, &da->da_fhandle) &&
	    xdr_string(xdrs, &da->da_name, NFS_MAXNAMLEN)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_diropargs %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * NFS_OK part of directory operation result
 */
bool_t
xdr_drok(xdrs, drok)
	XDR *xdrs;
	struct nfsdrok *drok;
{

	if (xdr_fhandle(xdrs, &drok->drok_fhandle) &&
	    xdr_fattr(xdrs, &drok->drok_attr)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_drok %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

struct xdr_discrim diropres_discrim[2] = {
	{ (int)NFS_OK, xdr_drok },
	{ __dontcare__, NULL_xdrproc_t }
};

/*
 * Results from directory operation
 */
bool_t
xdr_diropres(xdrs, dr)
	XDR *xdrs;
	struct nfsdiropres *dr;
{

	if (xdr_union(xdrs, (enum_t *)&(dr->dr_status),
		      (caddr_t)&(dr->dr_drok), diropres_discrim, xdr_void)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_diropres %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * Time structure
 */
bool_t
xdr_timeval(xdrs, tv)
	XDR *xdrs;
	struct timeval *tv;
{

	if (xdr_long(xdrs, &tv->tv_sec) &&
	    xdr_long(xdrs, &tv->tv_usec)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_timeval %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * arguments to setattr
 */
bool_t
xdr_saargs(xdrs, argp)
	XDR *xdrs;
	struct nfssaargs *argp;
{

	if (xdr_fhandle(xdrs, &argp->saa_fh) &&
	    xdr_sattr(xdrs, &argp->saa_sa)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_saargs %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * arguments to create and mkdir
 */
bool_t
xdr_creatargs(xdrs, argp)
	XDR *xdrs;
	struct nfscreatargs *argp;
{

	if (xdr_diropargs(xdrs, &argp->ca_da) &&
	    xdr_sattr(xdrs, &argp->ca_sa)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_creatargs %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * arguments to link
 */
bool_t
xdr_linkargs(xdrs, argp)
	XDR *xdrs;
	struct nfslinkargs *argp;
{

	if (xdr_fhandle(xdrs, &argp->la_from) &&
	    xdr_diropargs(xdrs, &argp->la_to)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_linkargs: %s FAILED",
		xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * arguments to rename
 */
bool_t
xdr_rnmargs(xdrs, argp)
	XDR *xdrs;
	struct nfsrnmargs *argp;
{

	if (xdr_diropargs(xdrs, &argp->rna_from) &&
	    xdr_diropargs(xdrs, &argp->rna_to)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_rnmargs %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * arguments to symlink
 */
bool_t
xdr_slargs(xdrs, argp)
	XDR *xdrs;
	struct nfsslargs *argp;
{

	if (xdr_diropargs(xdrs, &argp->sla_from) &&
	    xdr_string(xdrs, &argp->sla_tnm, (uint)NFS_MAXPATHLEN) &&
	    xdr_sattr(xdrs, &argp->sla_sa)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_slargs %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

/*
 * NFS_OK part of statfs operation
 */
xdr_fsok(xdrs, fsok)
	XDR *xdrs;
	struct nfsstatfsok *fsok;
{

	if (xdr_long(xdrs, (long *)&fsok->fsok_tsize) &&
	    xdr_long(xdrs, (long *)&fsok->fsok_bsize) &&
	    xdr_long(xdrs, (long *)&fsok->fsok_blocks) &&
	    xdr_long(xdrs, (long *)&fsok->fsok_bfree) &&
	    xdr_long(xdrs, (long *)&fsok->fsok_bavail)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_fsok %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}

struct xdr_discrim statfs_discrim[2] = {
	{ (int)NFS_OK, xdr_fsok },
	{ __dontcare__, NULL_xdrproc_t }
};

/*
 * Results of statfs operation
 */
xdr_statfs(xdrs, fs)
	XDR *xdrs;
	struct nfsstatfs *fs;
{

	if (xdr_union(xdrs, (enum_t *)&(fs->fs_status),
		      (caddr_t)&(fs->fs_fsok), statfs_discrim, xdr_void)) {
		return(TRUE);
	}
	cmn_err(CE_WARN, "xdr_statfs: %s FAILED", xdropnames[(int)xdrs->x_op]);
	return(FALSE);
}
E 1
