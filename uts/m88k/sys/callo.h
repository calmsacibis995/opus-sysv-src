#ifndef _SYS_CALLO_H_
#define _SYS_CALLO_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *	The callout structure is for a routine arranging
 *	to be called by the clock interrupt
 *	(clock.c) with a specified argument,
 *	in a specified amount of time.
 *	Used, for example, to time tab delays on typewriters.
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

struct	callo
{
	int	c_time;		/* incremental time */
	int	c_id;		/* timeout id */
	caddr_t	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
};
extern	struct	callo	callout[];

#endif	/* ! _SYS_CALLO_H_ */
