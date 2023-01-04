/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getcwd.c	1.10"
/*LINTLIBRARY*/
/*
 * Library routine to GET the Current Working Directory.
 * arg1 is a pointer to a character buffer into which the
 * path name of the current directory is placed by the
 * subroutine.  arg1 may be zero, in which case the 
 * subroutine will call malloc to get the required space.
 * arg2 is the length of the buffer space for the path-name.
 * If the actual path-name is longer than (arg2-2), or if
 * the value of arg2 is not at least 3, the subroutine will
 * return a value of zero, with errno set as appropriate.
 */

#include <stdio.h>
#include <sys/errno.h>
#if m88k
#include <sys/param.h>
#endif

#if m88k
extern char *getwd();
extern int errno;
extern char *_malloc();
#else
extern FILE *_popen();
extern char *_malloc(), *_fgets(), *_strchr();
extern int errno, _pclose();
#endif

char *
_getcwd(arg1, arg2)
char	*arg1;
int	arg2;
{
#if m88k
	char pathbuf[MAXPATHLEN+1];
#else
	FILE	*pipe;
	static char *cmd = "pwd 2>/dev/null";
	char	*trm;
#endif

#if 1
	/* posix flavor */
	if(arg2 <= 0) {
		errno = EINVAL;
		return(0);
	}
#else
	if(arg2 == 0) {
		errno = EINVAL;
		return(0);
	}
	if(arg2 < 0) {
		errno = ERANGE;
		return(0);
	}
#endif
	if(arg1 == 0)
		if((arg1 = _malloc((unsigned)arg2)) == 0) {
			errno = ENOMEM;
			return(0);
		}
	errno = 0;
#if m88k
	if (getwd(pathbuf) == NULL) {
		errno = EACCES;
		return (0);
	} 
	if (strlen(pathbuf) >= arg2) {
		errno = ERANGE;
		return (0);
	}
	strcpy(arg1, pathbuf);
#else
	if((pipe = _popen(cmd, "r")) == 0) {
		return(0);
	}
	errno = 0;
	if ((_fgets(arg1, arg2, pipe)) == NULL) {
		errno = EACCES;
		 return(0);
		}
	(void) _pclose(pipe);
	trm = _strchr(arg1, '\0');
	if(*(trm-1) != '\n') {
		errno = ERANGE;
		return(0);
	}
	*(trm-1) = '\0';
#endif
	return(arg1);
}
