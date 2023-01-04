#ifndef _ASSERT_H_
#define _ASSERT_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#ifdef NDEBUG
#define assert(EX)
#else
extern void __assert();
#define assert(EX) if (EX) ; else __assert("EX", __FILE__, __LINE__)
#endif

#endif	/* ! _ASSERT_H_ */
