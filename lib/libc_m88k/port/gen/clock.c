/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/clock.c	1.6"
/*LINTLIBRARY*/

#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>	/* for HZ (clock frequency in Hz) */
#define TIMES(B)	(B.tms_utime+B.tms_stime+B.tms_cutime+B.tms_cstime)

extern long _times();
static long first = 0L;

#ifdef HZ
#undef HZ
#define	HZ	(_sysconf(3))
#endif

long
_clock()
{
	struct tms buffer;

	if (_times(&buffer) != -1L && first == 0L)
		first = TIMES(buffer);
	return ((TIMES(buffer) - first) * (1000000L/HZ));
}
