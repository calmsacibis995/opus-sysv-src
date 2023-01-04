h09144
s 00043/00000/00000
d D 1.1 90/03/06 12:30:16 root 1 0
c date and time created 90/03/06 12:30:16 by root
e
u
U
t
T
I 1
#ifndef _SYS_SEMA_H_
#define _SYS_SEMA_H_


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
 * defines for semaphore mapping
 */

typedef int	sema_t;

#define ALLOC_LOCK(X)
#define EXTERN_LOCK(X)
#define INITLOCK(X,Y)
#define SPSEMA(X)
#define SVSEMA(X)
#define	PSEMA(X,Y)
#define	VSEMA(X,Y)

#define appsema(a,b)	1
#define apvsema(a,b)	1
#define psema(a,b)	sleep(a,b)
#define vsema(a,b)	wakeup(a)
#define initsema(a,b)	*a = b
#define initlock(a,b)	*a = b
#define cvsema(a)	wakeup(a)
#define splrf()		spl6()
#define ptob(x)		x

#endif	/* ! _SYS_SEMA_H_ */
E 1
