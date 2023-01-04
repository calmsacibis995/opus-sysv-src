/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1986 by National Semiconductor Corp.	*/
/*	All Rights Reserved					*/


/* #ident	"@(#)libnsl:nsl/_import.h	1.3" */

#ident	"@(#)_import.h	1.2	8/19/86 Copyright (c) 1986 by National Semiconductor Corp."

#ifndef	NOSHLIB
#define free		(*__free)
#define calloc 		(*__calloc)
#define perror		(*__perror)
#define strlen		(*__strlen)
#define write		(*__write)
#if m88k
#define ioctl		(*__ioctlnsl)
#else
#define ioctl		(*__ioctl)
#endif
#define putmsg		(*__putmsg)
#define getmsg		(*__getmsg)
#define errno		(*__errno)
#define memcpy		(*__memcpy)
#define fcntl		(*__fcntl)
#define sigset		(*__sigset)
#define open		(*__open)
#define close		(*__close)
#define ulimit		(*__ulimit)
#else
#define	recv		getmsg
#define	send		putmsg
#endif
