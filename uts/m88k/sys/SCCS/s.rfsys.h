h32080
s 00048/00000/00000
d D 1.1 90/03/06 12:30:13 root 1 0
c date and time created 90/03/06 12:30:13 by root
e
u
U
t
T
I 1
#ifndef _SYS_RFSYS_H_
#define _SYS_RFSYS_H_


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

#ifdef INKERNEL
extern	char	Domain[];
#endif

/*  opcodes for rfsys system call */

#define RF_FUMOUNT	1	/* forced unmount */
#define RF_SENDUMSG	2	/* send buffer to remote user-level */
#define RF_GETUMSG	3	/* wait for buffer from remote user-level */
#define RF_LASTUMSG	4	/* wakeup from GETUMSG */
#define RF_SETDNAME	5	/* set domain name */
#define RF_GETDNAME	6	/* get domain name */
#define RF_SETIDMAP	7
#define RF_FWFD		8
#define RF_VFLAG	9
#define RF_DISCONN	10	/* return value for link down */
#define RF_VERSION	11
#define RF_RUNSTATE	12	/* see if RFS is running */


/* defines for VFLAG option	*/
#define V_CLEAR 0
#define V_SET	1
#define V_GET	2

/* defines for the VERSION option	*/
#define VER_CHECK	1
#define VER_GET		2

#endif	/* ! _SYS_RFSYS_H_ */
E 1
