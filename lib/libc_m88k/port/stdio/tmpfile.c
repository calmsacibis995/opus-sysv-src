/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/tmpfile.c	1.8"
/*LINTLIBRARY*/
/*
 *	_tmpfile - return a pointer to an update file that can be
 *		used for scratch. The file will automatically
 *		go away if the program using it terminates.
 */
#include <stdio.h>

extern FILE *_fopen();
extern int _unlink();
extern char *_tmpnam();
extern void perror();

FILE *
_tmpfile()
{
	char	tfname[L_tmpnam];
	register FILE	*p;

	(void) _tmpnam(tfname);
	if((p = _fopen(tfname, "w+")) == NULL)
		return NULL;
	else
		(void) _unlink(tfname);
	return(p);
}
