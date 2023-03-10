/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/perror.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Print the error indicated
 * in the cerror cell.
 */

extern int errno, sys_nerr, _strlen(), _write();
extern char *sys_errlist[];

void
_perror(s)
char	*s;
{
	register char *c;
	register int n;

	c = "Unknown error";
	if(errno < sys_nerr)
		c = sys_errlist[errno];
	n = _strlen(s);
	if(n) {
		(void) _write(2, s, (unsigned)n);
		(void) _write(2, ": ", 2);
	}
	(void) _write(2, c, (unsigned)_strlen(c));
	(void) _write(2, "\n", 1);
}
