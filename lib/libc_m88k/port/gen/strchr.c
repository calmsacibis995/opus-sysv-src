/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/strchr.c	1.4"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#define	NULL	0

char *
_strchr(sp, c)
register char *sp, c;
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return(NULL);
}
