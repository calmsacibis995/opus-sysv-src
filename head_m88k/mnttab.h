#ifndef _MNTTAB_H_
#define _MNTTAB_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*      @(#)mnttab.h	2.1 System V NFS  source        */
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define	NMOUNT	100

/* Format of the /etc/mnttab file which is set by the mount(1m)
 * command
 */
struct mnttab {
	char	mt_dev[32],
		mt_filsys[32];
	short	mt_ro_flg;
	time_t	mt_time;
	char	mt_fstyp[16],
		mt_mntopts[64];

};

#endif	/* ! _MNTTAB_H_ */
