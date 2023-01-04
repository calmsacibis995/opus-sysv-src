h62524
s 00001/00001/00059
d D 1.2 90/03/27 17:07:42 root 2 1
c file is now a pointer
e
s 00060/00000/00000
d D 1.1 90/03/06 12:29:40 root 1 0
c date and time created 90/03/06 12:29:40 by root
e
u
U
t
T
I 1
#ifndef _SYS_FILE_H_
#define _SYS_FILE_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * One file structure is allocated for each open/creat/pipe call.
 * Main use is to hold the read/write pointer associated with
 * each open file.
 */

typedef struct file
{
	char	f_flag;
	cnt_t	f_count;		/* reference count */
	union {
		struct inode *f_uinode;	/* pointer to inode structure */
		struct file  *f_unext;	/* pointer to next entry in freelist */
	} f_up;
	off_t	f_offset;		/* read/write character pointer */
} file_t;

#define f_inode		f_up.f_uinode
#define f_next		f_up.f_unext

D 2
extern struct file file[];	/* The file table itself */
E 2
I 2
extern struct file *file;	/* The file table itself */
E 2
extern struct file *ffreelist;	/* Head of freelist pool */

/* flags */

#define	FOPEN	0xffffffff
#define	FREAD	0x01
#define	FWRITE	0x02
#define	FNDELAY	0x04
#define	FAPPEND	0x08
#define FSYNC	0x10
#define	FNONB	0x40
#define	FMASK	0x7f		/* FMASK should be disjoint from FNET */
#define FNET	0x80		/* needed by 3bnet */

/* open only modes */

#define	FCREAT	0x100
#define	FTRUNC	0x200
#define	FEXCL	0x400

#endif	/* ! _SYS_FILE_H_ */
E 1
