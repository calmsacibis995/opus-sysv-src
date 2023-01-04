/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)spray.h	3.1 LAI System V NFS Release 3.2/V3	source        */
/*	@(#)spray.h 1.2 85/08/04 SMI */

/*
 * Copyright (c) 1985 by Sun Microsystems, Inc.
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define SPRAYPROG 100012
#define SPRAYPROC_SPRAY 1
#define SPRAYPROC_GET 2
#define SPRAYPROC_CLEAR 3
#define SPRAYVERS_ORIG 1
#define SPRAYVERS 1

#define SPRAYOVERHEAD 86	/* size of rpc packet when size=0 */
#define SPRAYMAX 8845		/* related to max udp packet of 9000 */

int xdr_sprayarr();
int xdr_spraycumul();

struct spraycumul {
	unsigned counter;
	struct timeval clock;
};

struct sprayarr {
	int *data;
	int lnth;
};
