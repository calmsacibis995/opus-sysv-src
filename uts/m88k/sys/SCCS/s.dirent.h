h63089
s 00030/00000/00000
d D 1.1 90/03/06 12:29:33 root 1 0
c date and time created 90/03/06 12:29:33 by root
e
u
U
t
T
I 1
#ifndef _SYS_DIRENT_H_
#define _SYS_DIRENT_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * The following structure defines the file
 * system independent directory entry.
 *
 */

struct dirent				/* data from readdir() */
	{
	ino_t		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk direntory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[1];	/* name of file */
	};


#endif	/* ! _SYS_DIRENT_H_ */
E 1
