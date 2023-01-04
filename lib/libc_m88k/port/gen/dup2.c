/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/dup2.c	1.2"
#include	<limits.h>	/* Get definition for OPEN_MAX */
#include	<fcntl.h>
#include	<sys/errno.h>	/* Get definition for EBADF */
#if m88k
#include	<unistd.h>
#endif

int
_dup2(fildes, fildes2)
int	fildes,		/* file descriptor to be duplicated */
	fildes2;	/* desired file descriptor */
{
#if m88k
	register open_max = _openmax();
#endif
	int	tmperrno;	/* local work area */
	extern	int	errno;	/* system error indicator */

	/* Be sure fildes is valid and open */
	if (_fcntl(fildes, F_GETFL, 0) == -1) {
		errno = EBADF;
		return (-1);
	}

	/* Be sure fildes2 is in valid range */
#if m88k
	if (fildes2 < 0 || fildes2 >= open_max) {
#else
	if (fildes2 < 0 || fildes2 >= OPEN_MAX) {
#endif
		errno = EBADF;
		return (-1);
	}

	/* Check if file descriptors are equal */
	if (fildes == fildes2) {
		/* open and equal so no dup necessary */
		return (fildes2);
	}

	/* Close in case it was open for another file */
	/* Must save and restore errno in case file was not open */
	tmperrno = errno;
	_close(fildes2);
	errno = tmperrno;

	/* Do the dup */
	return (_fcntl(fildes, F_DUPFD, fildes2));
}

