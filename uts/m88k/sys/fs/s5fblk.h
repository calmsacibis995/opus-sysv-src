#ifndef _FS_S5FBLK_H_
#define _FS_S5FBLK_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved 	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

struct	fblk
{
	int	df_nfree;
	daddr_t	df_free[NICFREE];
};

#endif	/* ! _FS_S5FBLK_H_ */
