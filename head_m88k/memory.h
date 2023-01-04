#ifndef _MEMORY_H_
#define _MEMORY_H_


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

extern char
	*memccpy(),
	*memchr(),
	*memcpy(),
	*memset();
extern int memcmp();

#endif	/* ! _MEMORY_H_ */
