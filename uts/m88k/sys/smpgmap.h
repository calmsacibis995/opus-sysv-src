#ifndef _SYS_SMPGMAP_H_
#define _SYS_SMPGMAP_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

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
