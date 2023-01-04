#ifndef _SYS_UTSNAME_H_
#define _SYS_UTSNAME_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#define	SYS_NMLN	256

struct utsname {
	char	sysname[SYS_NMLN];
	char	nodename[SYS_NMLN];
	char	release[SYS_NMLN];
	char	version[SYS_NMLN];
	char	machine[SYS_NMLN];
};

#ifndef __88000_OCS_DEFINED
#ifdef INKERNEL
extern struct utsname utsname;
#endif
#endif /* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_UTSNAME_H_ */
