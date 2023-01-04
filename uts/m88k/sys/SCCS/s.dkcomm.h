h47201
s 00032/00000/00000
d D 1.1 90/03/06 12:29:34 root 1 0
c date and time created 90/03/06 12:29:34 by root
e
u
U
t
T
I 1
#ifndef _SYS_DKCOMM_H_
#define _SYS_DKCOMM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)dkcomm.h	1.1	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Definitions for communications with dk driver.
 */

/* definitions used with disk driver ioctls */

#define DKGETFNAME 0
#define DKSETFNAME 1
#define DKGETFPARM 2
#define DKDRAINDISK 3
#define NAMESIZE 128

struct diskcommand {
	int drive;
	int parm;
	char name[NAMESIZE];
};

#endif	/* ! _SYS_DKCOMM_H_ */
E 1
