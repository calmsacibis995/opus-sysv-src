#ifndef _UNCTRL_H_
#define _UNCTRL_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * unctrl.h
 *
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

extern char	*_unctrl[];

#if	!defined(NOMACROS) && !defined(lint)

#define	unctrl(ch)	(_unctrl[(unsigned) ch])

#endif	/* NOMACROS && lint */

#endif	/* ! _UNCTRL_H_ */
