h02230
s 00034/00000/00000
d D 1.1 90/03/06 12:29:28 root 1 0
c date and time created 90/03/06 12:29:28 by root
e
u
U
t
T
I 1
#ifndef _SYS_CMN_ERR_H_
#define _SYS_CMN_ERR_H_


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

/* Common error handling severity levels */

#define CE_CONT  0	/* continuation				*/
#define CE_NOTE  1	/* notice				*/
#define CE_WARN	 2	/* warning				*/
#define CE_PANIC 3	/* panic				*/

/*	Codes for where output should go.
*/

#define	PRW_BUF		0x01	/* Output to putbuf.		*/
#define	PRW_CONS	0x02	/* Output to console.		*/

extern short	prt_where;

#endif	/* ! _SYS_CMN_ERR_H_ */
E 1
