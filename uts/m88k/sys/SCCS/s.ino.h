h34036
s 00044/00000/00000
d D 1.1 90/03/06 12:29:48 root 1 0
c date and time created 90/03/06 12:29:48 by root
e
u
U
t
T
I 1
#ifndef _SYS_INO_H_
#define _SYS_INO_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*      @(#)ino.h	2.1 System V NFS  source        */
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 *	Inode structure as it appears on a disk block.
 */

struct	dinode
{
	ushort	di_mode;	/* mode and type of file */
	short	di_nlink;    	/* number of links to file */
	ushort	di_uid;      	/* owner's user id */
	ushort	di_gid;      	/* owner's group id */
	off_t	di_size;     	/* number of bytes in file */
	char  	di_addr[39];	/* disk block addresses */
	char	di_gen;		/* file generation number */
	time_t	di_atime;   	/* time last accessed */
	time_t	di_mtime;   	/* time last modified */
	time_t	di_ctime;   	/* time created */
};
/*
 * The 39 address bytes: 13 addresses of 3 bytes each.
 *
 * The 40'th byte is used as generation count to allow detection of
 * the disk inode being reused.
 */

#endif	/* ! _SYS_INO_H_ */
E 1
