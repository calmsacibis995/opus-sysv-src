h05995
s 00034/00000/00000
d D 1.1 90/03/06 12:30:45 root 1 0
c date and time created 90/03/06 12:30:45 by root
e
u
U
t
T
I 1
#ifndef _SYS_USTAT_H_
#define _SYS_USTAT_H_


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

struct  ustat {
	daddr_t	f_tfree;	/* total free */
	ino_t	f_tinode;	/* total inodes free */ /* ino_t MMV */
	char	f_fname[6];	/* filsys name */
	char	f_fpack[6];	/* filsys pack name */
};

/* DELETE LATER. MMV */
struct  oustat {
	daddr_t	f_tfree;	/* total free */
	short	f_tinode;	/* total inodes free */
	char	f_fname[6];	/* filsys name */
	char	f_fpack[6];	/* filsys pack name */
};

#endif	/* ! _SYS_USTAT_H_ */
E 1
