#ifndef _SIGNAL_H_
#define _SIGNAL_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#include<sys/signal.h>

#ifndef __88000_OCS_DEFINED
extern	void(*signal())();
extern  void(*sigset())();
#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _SIGNAL_H_ */
