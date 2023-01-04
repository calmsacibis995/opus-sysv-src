#ifndef _SYS_DEBUG_H_
#define _SYS_DEBUG_H_


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

#define DEBUG 1

#ifdef	DEBUG
#define ASSERT(EX) if (!(EX)) assfail("EX", __FILE__, __LINE__); else
#else
#define ASSERT(x)
#endif

#endif	/* ! _SYS_DEBUG_H_ */
