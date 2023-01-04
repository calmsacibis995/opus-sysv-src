#ifndef _NFS_TRANS_H_
#define _NFS_TRANS_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*
 *	Copyright 1986 Convergent Technologies Inc.
 */

/*      @(#)trans.h	4.1 LAI System V NFS Release 3.2/V3	source        */

/*
 * Portions of this module were taken from Convergent's trans.h:
 *	Convergent Technologies - System V - Jan 1987
 * #ident	"@(#)trans.h	1.2 :/source/uts/common/sys/fs/nfs/s.trans.h 1/12/87 19:39:15"
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Remote Procedure Call streams transport provider interface.
 */

#ifdef INKERNEL
#include "sys/socket.h"

/*
 * Transport handle, private.
 */
typedef char TRANS;		/* XXX will have to do, for now */

#define	tpNULL	((TRANS *)0)

/*
 * Transport address, opaque.
 */
typedef struct sockaddr TADDR;	/* XXX will have to do, for now */

#define	taNULL	((TADDR *)0)

/*
 * Transport public.
 */
#define	TP_INFINITY	(-1)	/* infinity receive time out (none) */

/*
 * Below are the transport handle creation routines for the various
 * implementations of streams rpc transport.  They can return NULL
 * if a creation failure occurs.
 */

/*
 * Kernel socket-head streams-based udp/ip transport.
 *
 * TRANS *
 * kudp_create(fp, flags)
 *	struct file *fp;
 *	int	flags;
 */
extern TRANS *kudp_create();

/* kudp_create() flags */
#define	TP_OPENFP	0x01	/* create transport from file pointer */
#define TP_SECURE	0x02	/* if possible, bind to secure(?) port */

/* XXX 'til ops vector */
extern int	 kudp_sendto();
extern int	 kudp_recvfrom();
extern int	 kudp_geterr();
extern int	 kudp_secure();
extern void	 kudp_destroy();
#endif

#endif	/* ! _NFS_TRANS_H_ */
