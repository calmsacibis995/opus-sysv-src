#ifndef _SETJMP_H_
#define _SETJMP_H_


/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/


/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _JBLEN

#define _JBLEN	40

typedef int jmp_buf[_JBLEN];
#define sigjmp_buf jmp_buf

#ifndef __88000_OCS_DEFINED

extern int setjmp();
extern void longjmp();
extern int sigsetjmp();
extern void siglongjmp();

#endif	/* !__88000_OCS_DEFINED */

#endif	/* !_JBLEN */

#endif	/* ! _SETJMP_H_ */
