h54496
s 00000/00004/00022
d D 1.2 90/05/05 06:43:50 root 2 1
c removed mototola references
e
s 00026/00000/00000
d D 1.1 90/03/06 12:30:17 root 1 0
c date and time created 90/03/06 12:30:17 by root
e
u
U
t
T
I 1
#ifndef _SYS_SMPGMAP_H_
#define _SYS_SMPGMAP_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

D 2
/*		Copyright (c) 1987 Motorola Inc.		*/
/*		All Rights Reserved				*/
/*		@(#)smpgmap.h	10.1			*/

E 2
#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#define SMPGSIZE	1024

struct smpgmap {
	caddr_t	smp_addr;	/* address of page with free chunk(s) */
	uint	smp_bmap;	/* map of free chunks in page */
};

extern struct smpgmap smpgmap[];
extern caddr_t smpgalloc();

#endif	/* ! _SYS_SMPGMAP_H_ */
E 1
