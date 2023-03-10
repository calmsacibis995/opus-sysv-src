/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/ftok.c	1.4"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

key_t
_ftok(path, id)
char *path;
char id;
{
	struct stat st;

	return(_stat(path, &st) < 0 ? (key_t)-1 :
	    (key_t)((key_t)id << 24 | ((long)(unsigned)minor(st.st_dev)) << 16 |
		(unsigned)st.st_ino));
}
