/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)yppasswd.h	3.1 LAI System V NFS Release 3.2/V3	source        */
/*	@(#)yppasswd.h 1.1 85/03/19 SMI */

/*
 * Copyright (c) 1985 by Sun Microsystems, Inc.
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define YPPASSWDPROG 100009
#define YPPASSWDPROC_UPDATE 1
#define YPPASSWDVERS_ORIG 1
#define YPPASSWDVERS 1

struct yppasswd {
	char *oldpass;		/* old (unencrypted) password */
	struct passwd newpw;	/* new pw structure */
};

int xdr_yppasswd();
