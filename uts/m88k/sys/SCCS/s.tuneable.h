h37166
s 00064/00000/00000
d D 1.1 90/03/06 12:30:41 root 1 0
c date and time created 90/03/06 12:30:41 by root
e
u
U
t
T
I 1
#ifndef _SYS_TUNEABLE_H_
#define _SYS_TUNEABLE_H_


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

typedef struct tune {
	int	t_gpgslo;	/* If freemem < t_getpgslow, then start	*/
				/* to steal pages from processes.	*/
	int	t_gpgshi;	/* Once we start to steal pages, don't	*/
				/* stop until freemem > t_getpgshi.	*/
	int	t_gpgsmsk;	/* Mask used by getpages to determine	*/
				/* whether a page is stealable.  The	*/
				/* page is stealable if pte & t_gpgsmsk	*/
				/* is == 0.  Possible values for this	*/
				/* mask are:				*/
				/* 0		- steal any valid page.	*/
				/* PG_REF	- steal page if not	*/
				/*		  referenced in 	*/
				/*		  t_vhandr seconds.	*/
				/*		  mask = 0x00000008	*/
				/* PG_REF|PG_NDREF - steal page if not	*/
				/*		  referenced in 2 *	*/
				/*		  t_vhandr seconds.	*/
				/*		  mask = 0x00000108	*/
	int	t_vhandr;	/* Run vhand once every t_vhandr seconds*/
				/* if freemem < t_vhandl.		*/
	int	t_vhandl;	/* Run vhand once every t_vhandr seconds*/
				/* if freemem < t_vhandl.		*/
	int	t_maxumem;	/* The maximum size of a user's virtual	*/
				/* address space in pages.		*/
	int	t_bdflushr;	/* The rate at which bdflush is run in	*/
				/* seconds.				*/
	int	t_minarmem;	/* The minimum available resident (not	*/
				/* swapable) memory to maintain in 	*/
				/* order to avoid deadlock.  In pages.	*/
	int	t_minasmem;	/* The minimum available swapable	*/
				/* memory to maintain in order to avoid	*/
				/* deadlock.  In pages.			*/
	int	t_maxrdahead;	/* Maximum number of block read aheads  */
} tune_t;

extern tune_t	tune;

/*	The following is the default value for t_gpgsmsk.  It cannot be
 *	defined in /etc/master or /etc/system due to limitations of the
 *	config program.
 */

#define	GETPGSMSK	PG_REF|PG_NDREF

#endif	/* ! _SYS_TUNEABLE_H_ */
E 1
