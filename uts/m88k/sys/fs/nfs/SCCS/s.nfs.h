h49898
s 00444/00000/00000
d D 1.1 90/03/06 12:31:30 root 1 0
c date and time created 90/03/06 12:31:30 by root
e
u
U
t
T
I 1
#ifndef _NFS_NFS_H_
#define _NFS_NFS_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */

/*      @(#)nfs.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*      @(#)nfs.h 2.20 85/08/29 SMI      */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* Maximum size of data portion of a remote request */

#define	NFS_MAXDATA	8192
#define	NFS_MAXNAMLEN	255
#define	NFS_MAXPATHLEN	1024

/*
 * Rpc retransmission parameters
 */
#define	NFS_TIMEO	7	/* initial timeout in tenths of a sec. */
#define	NFS_RETRIES	4	/* times to retry request */

/*
 * maximum transfer size for different interfaces
 */
#define	ECTSIZE	4096
#define	IETSIZE	8192

/*
 * Error status
 * Should include all possible net errors.
 * For now we just cast errno into an enum nfsstat.
 */
enum nfsstat {
	NFS_OK = 0,			/* no error */
	NFSERR_PERM=EPERM,		/* Not owner */
	NFSERR_NOENT=ENOENT,		/* No such file or directory */
	NFSERR_IO=EIO,			/* I/O error */
	NFSERR_NXIO=ENXIO,		/* No such device or address */
	NFSERR_ACCES=EACCES,		/* Permission denied */
	NFSERR_EXIST=EEXIST,		/* File exists */
	NFSERR_NODEV=ENODEV,		/* No such device */
	NFSERR_NOTDIR=ENOTDIR,		/* Not a directory*/
	NFSERR_ISDIR=EISDIR,		/* Is a directory */
	NFSERR_FBIG=EFBIG,		/* File too large */
	NFSERR_NOSPC=ENOSPC,		/* No space left on device */
	NFSERR_ROFS=EROFS,		/* Read-only file system */
	NFSERR_OPNOTSUPP=45,		/* Operation not supported */
	NFSERR_NOBUFS=55,		/* No buffer space available */
	NFSERR_NAMETOOLONG=63,		/* File name too long */
	NFSERR_NOTEMPTY=66,		/* Directory not empty */
	NFSERR_DQUOT=69,		/* Disc quota exceeded */
	NFSERR_STALE=70,		/* Stale NFS file handle */
	NFSERR_REMOTE=71,		/* Too many levels of remote in path */
	NFSERR_WFLUSH			/* write cache flushed */
};

extern enum nfsstat puterrno();
extern int geterrno();

/*
 * File types
 */
enum nfsftype {
	NFNON,
	NFREG,		/* regular file */
	NFDIR,		/* directory */
	NFBLK,		/* block special */
	NFCHR,		/* character special */
	NFLNK		/* symbolic link */
};

/*
 * Special kludge for fifos (named pipes)  [to adhere to NFS Protocol Spec]
 *
 * VFIFO is not in the protocol spec (VNON will be replaced by VFIFO)
 * so the over-the-wire representation is VCHR with a '-1' device number.
 *
 * NOTE: This kludge becomes unnecessary with the Protocol Revision,
 *       but it may be necessary to support it (backwards compatibility).
 */

#define NFS_FIFO_TYPE	NFCHR
#define NFS_FIFO_MODE	S_IFCHR
#define NFS_FIFO_DEV	(~0)

/* identify fifo in nfs attributes */
#define NA_ISFIFO(NA)	(((NA)->na_type == NFS_FIFO_TYPE) && \
			 ((NA)->na_rdev == NFS_FIFO_DEV))

/* set fifo in nfs attributes */
#define NA_SETFIFO(NA)	{ \
			(NA)->na_type = NFS_FIFO_TYPE; \
			(NA)->na_rdev = NFS_FIFO_DEV; \
			(NA)->na_mode = ((NA)->na_mode&~S_IFMT)|NFS_FIFO_MODE; \
			}

/*
 * Size of an fhandle in bytes
 */
#define	NFS_FHSIZE	32

/*
 * File access handle
 * This structure is supposed to be opaque from the client side.
 * It is handed out by a server for the client to use in further
 * file transactions.
 */
typedef union {
	struct {
		dev_t	fh_fsid_u;	/* filesystem id */
		ulong	fh_fno_u;	/* file number */
		ulong	fh_fgen_u;	/* file generation */
	} fh_u;
	char	fh_pad[NFS_FHSIZE];	/* Pad to NFS_FHSIZE */
} fhandle_t;

#define fh_fsid		fh_u.fh_fsid_u
#define fh_fno		fh_u.fh_fno_u
#define fh_fgen		fh_u.fh_fgen_u


/*
 * Arguments to remote write and writecache
 */
struct nfswriteargs {
	fhandle_t	wa_fhandle;	/* handle for file */
	ulong		wa_begoff;	/* beginning byte offset in file */
	ulong		wa_offset;      /* current byte offset in file */
	ulong		wa_totcount;    /* total write count (to this offset)*/
	ulong		wa_count;	/* size of this write */
	char		*wa_data;	/* data to write (up to NFS_MAXDATA) */
};


/*
 * File attributes
 */
struct nfsfattr {
	enum nfsftype	na_type;	/* file type */
	ulong		na_mode;	/* protection mode bits */
	ulong		na_nlink;	/* # hard links */
	ulong		na_uid;		/* owner user id */
	ulong		na_gid;		/* owner group id */
	ulong		na_size;	/* file size in bytes */
	ulong		na_blocksize;	/* prefered block size */
	ulong		na_rdev;	/* special device # */
	ulong		na_blocks;	/* Kb of disk used by file */
	ulong		na_fsid;	/* device # */
	ulong		na_nodeid;	/* inode # */
	struct timeval	na_atime;	/* time of last access */
	struct timeval	na_mtime;	/* time of last modification */
	struct timeval	na_ctime;	/* time of last change */
};

/*
 * Arguments to remote read
 */
struct nfsreadargs {
	fhandle_t	ra_fhandle;	/* handle for file */
	ulong		ra_offset;	/* byte offset in file */
	ulong		ra_count;	/* immediate read count */
	ulong		ra_totcount;	/* total read count (from this offset)*/
};

/*
 * Status OK portion of remote read reply
 */
struct nfsrrok {
	struct nfsfattr	rrok_attr;	/* attributes, need for pagin*/
	ulong		rrok_count;	/* bytes of data */
	char		*rrok_data;	/* data (up to NFS_MAXDATA bytes) */
};

/*
 * Reply from remote read
 */
struct nfsrdresult {
	enum nfsstat	rr_status;		/* status of read */
	union {
		struct nfsrrok	rr_ok_u;	/* attributes, need for pagin*/
	} rr_u;
};
#define	rr_ok		rr_u.rr_ok_u
#define	rr_attr		rr_u.rr_ok_u.rrok_attr
#define	rr_count	rr_u.rr_ok_u.rrok_count
#define	rr_data		rr_u.rr_ok_u.rrok_data


/*
 * File attributes which can be set
 */
struct nfssattr {
	ulong		sa_mode;	/* protection mode bits */
	ulong		sa_uid;		/* owner user id */
	ulong		sa_gid;		/* owner group id */
	ulong		sa_size;	/* file size in bytes */
	struct timeval	sa_atime;	/* time of last access */
	struct timeval	sa_mtime;	/* time of last modification */
};


/*
 * Reply status with file attributes
 */
struct nfsattrstat {
	enum nfsstat	ns_status;		/* reply status */
	union {
		struct nfsfattr ns_attr_u;	/* NFS_OK: file attributes */
	} ns_u;
};
#define	ns_attr	ns_u.ns_attr_u


/*
 * NFS_OK part of read sym link reply union
 */
struct nfssrok {
	ulong	srok_count;	/* size of string */
	char	*srok_data;	/* string (up to NFS_MAXPATHLEN bytes) */
};

/*
 * Result of reading symbolic link
 */
struct nfsrdlnres {
	enum nfsstat	rl_status;		/* status of symlink read */
	union {
		struct nfssrok	rl_srok_u;	/* name of linked to */
	} rl_u;
};
#define	rl_srok		rl_u.rl_srok_u
#define	rl_count	rl_u.rl_srok_u.srok_count
#define	rl_data		rl_u.rl_srok_u.srok_data


/*
 * Arguments to readdir
 */
struct nfsrddirargs {
	fhandle_t rda_fh;	/* directory handle */
	ulong rda_offset;	/* offset in directory (opaque) */
	ulong rda_count;	/* number of directory bytes to read */
};


/*
 * Readdir result
 *	Notice that this structure differs from the similar Sun structure.
 *	The xdr function converts between this structure and the wire.
 *	(The difference is in the shape of the individual entries.)
 */
struct nfsrdok {
	ulong	rdok_offset;		/* next offset (opaque) */
	ulong	rdok_size;		/* size in bytes of entries */
	bool_t	rdok_eof;		/* true if last entry is in result*/
	struct dirent *rdok_entries;	/* variable number of entries */
};

/*
 * Readdir result
 */
struct nfsrddirres {
	ulong		rd_bufsize;	/* size of client request (not xdr'ed)*/
	enum nfsstat	rd_status;
	union {
		struct nfsrdok rd_rdok_u;
	} rd_u;
};
#define	rd_rdok		rd_u.rd_rdok_u
#define	rd_offset	rd_u.rd_rdok_u.rdok_offset
#define	rd_size		rd_u.rd_rdok_u.rdok_size
#define	rd_eof		rd_u.rd_rdok_u.rdok_eof
#define	rd_entries	rd_u.rd_rdok_u.rdok_entries


/*
 * Arguments for directory operations
 */
struct nfsdiropargs {
	fhandle_t	da_fhandle;	/* directory file handle */
	char		*da_name;	/* name (up to NFS_MAXNAMLEN bytes) */
};

/*
 * NFS_OK part of directory operation result
 */
struct  nfsdrok {
	fhandle_t	drok_fhandle;	/* result file handle */
	struct nfsfattr	drok_attr;	/* result file attributes */
};

/*
 * Results from directory operation 
 */
struct  nfsdiropres {
	enum nfsstat	dr_status;	/* result status */
	union {
		struct  nfsdrok	dr_drok_u;	/* NFS_OK result */
	} dr_u;
};
#define	dr_drok		dr_u.dr_drok_u
#define	dr_fhandle	dr_u.dr_drok_u.drok_fhandle
#define	dr_attr		dr_u.dr_drok_u.drok_attr

/*
 * arguments to setattr
 */
struct nfssaargs {
	fhandle_t	saa_fh;		/* fhandle of file to be set */
	struct nfssattr	saa_sa;		/* new attributes */
};

/*
 * arguments to create and mkdir
 */
struct nfscreatargs {
	struct nfsdiropargs	ca_da;	/* file name to create and parent dir */
	struct nfssattr		ca_sa;	/* initial attributes */
};

/*
 * arguments to link
 */
struct nfslinkargs {
	fhandle_t		la_from;	/* old file */
	struct nfsdiropargs	la_to;		/* new file and parent dir */
};

/*
 * arguments to rename
 */
struct nfsrnmargs {
	struct nfsdiropargs rna_from;	/* old file and parent dir */
	struct nfsdiropargs rna_to;	/* new file and parent dir */
};

/*
 * arguments to symlink
 */
struct nfsslargs {
	struct nfsdiropargs	sla_from;	/* old file and parent dir */
	char			*sla_tnm;	/* new name */
	struct nfssattr		sla_sa;		/* attributes */
};

/*
 * NFS_OK part of statfs operation
 */
struct nfsstatfsok {
	ulong fsok_tsize;	/* preferred transfer size in bytes */
	ulong fsok_bsize;	/* fundamental file system block size */
	ulong fsok_blocks;	/* total blocks in file system */
	ulong fsok_bfree;	/* free blocks in fs */
	ulong fsok_bavail;	/* free blocks avail to non-superuser */
};

/*
 * Results of statfs operation
 */
struct nfsstatfs {
	enum nfsstat	fs_status;	/* result status */
	union {
		struct	nfsstatfsok fs_fsok_u;	/* NFS_OK result */
	} fs_u;
};
#define	fs_fsok		fs_u.fs_fsok_u
#define	fs_tsize	fs_u.fs_fsok_u.fsok_tsize
#define	fs_bsize	fs_u.fs_fsok_u.fsok_bsize
#define	fs_blocks	fs_u.fs_fsok_u.fsok_blocks
#define	fs_bfree	fs_u.fs_fsok_u.fsok_bfree
#define	fs_bavail	fs_u.fs_fsok_u.fsok_bavail

/*
 * XDR routines for handling structures defined above
 */
bool_t xdr_attrstat();
bool_t xdr_creatargs();
bool_t xdr_diropargs();
bool_t xdr_diropres();
bool_t xdr_drok();
bool_t xdr_fattr();
bool_t xdr_fhandle();
bool_t xdr_linkargs();
bool_t xdr_rddirargs();
bool_t xdr_putrddirres();
bool_t xdr_getrddirres();
bool_t xdr_rdlnres();
bool_t xdr_rdresult();
bool_t xdr_readargs();
bool_t xdr_rnmargs();
bool_t xdr_rrok();
bool_t xdr_saargs();
bool_t xdr_sattr();
bool_t xdr_slargs();
bool_t xdr_srok();
bool_t xdr_timeval();
bool_t xdr_writeargs();
bool_t xdr_statfs();

/*
 * Remote file service routines
 */
#define	RFS_NULL	0
#define	RFS_GETATTR	1
#define	RFS_SETATTR	2
#define	RFS_ROOT	3
#define	RFS_LOOKUP	4
#define	RFS_READLINK	5
#define	RFS_READ	6
#define	RFS_WRITECACHE	7
#define	RFS_WRITE	8
#define	RFS_CREATE	9
#define	RFS_REMOVE	10
#define	RFS_RENAME	11
#define	RFS_LINK	12
#define	RFS_SYMLINK	13
#define	RFS_MKDIR	14
#define	RFS_RMDIR	15
#define	RFS_READDIR	16
#define	RFS_STATFS	17
#define	RFS_NPROC	18

/*
 * remote file service numbers
 */
#define	NFS_PROGRAM	((ulong)100003)
#define	NFS_VERSION	((ulong)2)
#define	NFS_PORT	2049

#endif	/* ! _NFS_NFS_H_ */
E 1
