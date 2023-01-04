h05336
s 00030/00000/00000
d D 1.1 90/03/06 12:29:41 root 1 0
c date and time created 90/03/06 12:29:41 by root
e
u
U
t
T
I 1
#ifndef _SYS_FSID_H_
#define _SYS_FSID_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*      @(#)fsid.h	2.1 System V NFS  source        */
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* Fstyp names for use in fsinfo structure. These names */
/* must be constant across releases and will be used by a */
/* user level routine to map fstyp to fstyp index as used */
/* ip->i_fstyp. This is necessary for programs like mount. */

#define S51K	"S51K"
#define PROC	"PROC"
#define DUFST	"DUFST"
#define NFS	"NFS"

#endif	/* ! _SYS_FSID_H_ */
E 1
