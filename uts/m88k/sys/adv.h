#ifndef _SYS_ADV_H_
#define _SYS_ADV_H_


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

/*
 *	advertise structure.
 *	one entry per advertised object.
 */

struct	advertise	{
	int	a_flags;		/* defines are in sys/nserve.h	*/
	int	a_count;		/* number of active rmounts	*/
	char	a_name [NMSZ];		/* name sent to name server	*/
	struct	rcvd	*a_queue;	/* receive queue for this name	*/
	char	*a_clist;		/* ref to authorization list	*/
} ;

#ifdef INKERNEL

extern	struct	advertise	advertise[];

#endif

#endif	/* ! _SYS_ADV_H_ */
