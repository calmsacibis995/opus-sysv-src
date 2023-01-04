h22385
s 00170/00000/00000
d D 1.1 90/03/06 12:27:56 root 1 0
c date and time created 90/03/06 12:27:56 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/fio.c	10.8"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/fs/s5param.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/immu.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/fstyp.h"
#include "sys/nami.h"
#include "sys/inode.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/mount.h"
#include "sys/var.h"
#include "sys/sysinfo.h"
#include "sys/acct.h"
#include "sys/open.h"
#include "sys/cmn_err.h"

/*
 * Convert a user supplied file descriptor into a pointer
 * to a file structure.
 * Only task is to check range of the descriptor.
 */
struct file *
getf(f)
register int f;
{
	register struct file *fp;

	if (0 <= f && f < v.v_nofiles) 
		if ((fp = u.u_ofile[f]) != NULL) 
			return(fp);
	u.u_error = EBADF;
	return(NULL);
}

/*
 * Internal form of close.
 * Decrement reference count on file structure.
 * Also make sure the pipe protocol does not constipate.
 *
 * Decrement reference count on the inode following
 * removal to the referencing file structure.
 * On the last close switch out to the device handler for
 * special files.  Note that the handler is called
 * on every open but only the last close.
 */
closef(fp)
register struct file *fp;
{
	register struct inode *ip;
	register unsigned ftype;

	/* Sanity check. */
	if (fp == NULL || (fp->f_count) <= 0)
		return;
	ip = fp->f_inode;
	if ((ftype = ip->i_ftype) != IFMPB)
		plock(ip);
	FS_CLOSEI(ip, fp->f_flag, fp->f_count, fp->f_offset);
	if ((unsigned)fp->f_count > 1) {
		fp->f_count--;
		if (ftype != IFMPB)
			prele(ip);
		return;
	}
	fp->f_count = 0;
	fp->f_next = ffreelist;
	ffreelist = fp;
	/* don't free inodes allocated for sockets, they are really mbufs */
	/* they may even be reallocated by now */
	if (ftype != IFMPB)
		iput(ip);
}

/*
 * Test if the current user is the super user.
 */
suser()
{

	if (u.u_uid == 0) {
		u.u_acflag |= ASU;
		return(1);
	}
	u.u_error = EPERM;
	return(0);
}

/*
 * Allocate a user file descriptor.
 */
ufalloc(i)
register i;
{

	for (; i < v.v_nofiles; i++)
		if (u.u_ofile[i] == NULL) {
			u.u_rval1 = i;
			u.u_pofile[i] = 0;
			return(i);
		}
	u.u_error = EMFILE;
	return(-1);
}

/*
 * Allocate a user file descriptor and a file structure.
 * Initialize the descriptor to point at the file structure.
 *
 * file table overflow -- if there are no available file structures.
 */
struct file *
falloc(ip, flag)
struct inode *ip;
{
	register struct file *fp;
	register i;

	if ((i = ufalloc(0)) < 0)
		return(NULL);
	if ((fp = ffreelist) == NULL) {
		cmn_err(CE_NOTE, "File table overflow\n");
		syserr.fileovf++;
		u.u_error = ENFILE;
		return(NULL);
	}
	ffreelist = fp->f_next;
	u.u_ofile[i] = fp;
	fp->f_count++;
	fp->f_flag = flag;
	fp->f_inode = ip;
	fp->f_offset = 0;
	return(fp);
}

struct file *ffreelist;
int nfile;		/* netstat support */

finit()
{
	register struct file *fp;

	nfile = v.v_file;
	for (ffreelist = fp = &file[0]; fp < &file[v.v_file-1]; fp++)
		fp->f_next = fp+1;
}

unfalloc(fp)
register struct file	*fp;
{
	if (--fp->f_count <= 0) {
		fp->f_next = ffreelist;
		ffreelist = fp;
	}
}
E 1
