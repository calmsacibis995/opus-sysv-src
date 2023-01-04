/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/fdopen.c	1.9"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Unix routine to do an "_fopen" on file descriptor
 * The mode has to be repeated because you can't query its
 * status
 */

#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern long _lseek();
extern FILE *_findiop();
extern errno;

FILE *
_fdopen(fd, mode)
int	fd;
register char *mode;
{
	register FILE *iop;
	struct stat st;
	int rv;

	/* 
	 * POSIX/ITS: This is to check if the fd is valid 
	 */
        errno = 0;
	rv = _fstat(fd, &st);
	if (rv == -1 || errno != 0)
		return(NULL);		/* possibly EBADF */

	if((iop = _findiop()) == NULL)
		return(NULL);

	iop->_cnt = 0;
	iop->_flag = 0;
	iop->_file = fd;
	_bufend(iop) = iop->_base = iop->_ptr = NULL;
	switch(*mode) {

		case 'r':
			iop->_flag |= _IOREAD;
			break;
		case 'a':
			(void) _lseek(fd, 0L, 2);
			/* No break */
		case 'w':
			iop->_flag |= _IOWRT;
			break;
		default:
			return(NULL);
	}

	if(mode[1] == '+') {
		iop->_flag &= ~(_IOREAD | _IOWRT);
		iop->_flag |= _IORW;
	}

	return(iop);
}
