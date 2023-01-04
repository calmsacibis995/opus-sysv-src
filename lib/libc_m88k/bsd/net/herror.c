/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)herror.c	6.3 (Berkeley) 6/27/88";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <sys/uio.h>

char	*__h_errlist[] = {
	"Error 0",
	"Unknown host",				/* 1 HOST_NOT_FOUND */
	"Host name lookup failure",		/* 2 TRY_AGAIN */
	"Unknown server error",			/* 3 NO_RECOVERY */
	"No address associated with name",	/* 4 NO_ADDRESS */
};
int	__h_nerr = { sizeof(__h_errlist)/sizeof(__h_errlist[0]) };

extern int	h_errno;

/*
 * herror --
 *	print the error indicated by the h_errno value.
 */
_herror(s)
	char *s;
{
	struct iovec iov[4];
	register struct iovec *v = iov;

	if (s && *s) {
		v->iov_base = s;
		v->iov_len = _strlen(s);
		v++;
		v->iov_base = ": ";
		v->iov_len = 2;
		v++;
	}
	v->iov_base = h_errno < __h_nerr ? __h_errlist[h_errno] : "Unknown error";
	v->iov_len = _strlen(v->iov_base);
	v++;
	v->iov_base = "\n";
	v->iov_len = 1;
	_writev(2, iov, (v - iov) + 1);
}
