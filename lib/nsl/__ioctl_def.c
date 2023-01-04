/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libnsl:nsl/__ioctl_def.c	1.2"

#if m88k
void (*__ioctlnsl)() = 0;
#else
void (*__ioctl)() = 0;
#endif
