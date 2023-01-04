h40885
s 00052/00000/00000
d D 1.1 90/03/06 12:30:40 root 1 0
c date and time created 90/03/06 12:30:40 by root
e
u
U
t
T
I 1
#ifndef _SYS_TRAP_H_
#define _SYS_TRAP_H_


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
 *  Exception Vector definitions for M88000
 */

#define		BUSERR		2	/*  Bus error			*/
#define		DACCFLT		2	/*  Data access fault.*/
#define		CACCFLT		3	/*  Code access fault.*/
#define		ADDRERR		4	/*  Address error		*/
#define		INSTERR		5	/*  Illegal instruction		*/
#define		PRIVFLT		6	/*  priviledged instruction 	*/
#define		CHKTRAP		7	/*  tbnd instruction		*/
#define		ZDVDERR		8	/*  Divide by zero		*/
#define		INTOVF		9	/*  Integer overflow. 		*/

#define		SFU1PR		114	/*  Floating point precise	*/
#define		SFU1IMP		115	/*  Floating point imprecise	*/

/*
 *	Special TRAP instruction vectors
 */
					/* TRAP No.			*/
#define		TRCTRAP		128	/*  Trace trap 			*/
#define		BPTFLT		129	/*  Old Breakpoint trap		*/
#define		BPTTRAP		511	/*  New Breakpoint trap		*/

/*
 *	The following are software defined exceptions
 */

#define		RESCHED		512	/*  pseudo trap to signal cpu	*/
#define	USER	1024
					/*  rescheduling. 		*/

#endif	/* ! _SYS_TRAP_H_ */
E 1
