#ifndef _SYS_CB_H_
#define _SYS_CB_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)cb.h	1.3	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* max number of devices of each class */

#define CONMAX 4
#define ASMAX 100
#define LPMAX 3
#define DKMAX 16
#define GNMAX 16
#define MTMAX 4
#define PTMAX 32

#ifndef ASSEM
extern char indtab[];
extern char verstamp[];
#endif

#endif	/* ! _SYS_CB_H_ */
