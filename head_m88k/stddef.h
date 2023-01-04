#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * The ANSI header file stddef.h
 */

typedef int ptrdiff_t;

#include <sys/types.h>

typedef int wchar_t;

#ifndef NULL
#define NULL 0
#endif

#define offsetof(type, member) \
	(size_t) ((size_t) &(type.member) - (size_t) &(type) )


#endif	/* ! _STDDEF_H_ */
