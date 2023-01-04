/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/strftime.c	1.9"
/*LINTLIBRARY*/

extern int _ascftime();

int
_strftime(buf, size, format, tm)
char	*buf, *format;
struct tm	*tm;
{
	char buffer[2048];
	register i;
	i = _ascftime(buffer, format, tm);
	if ( (i + 1) < size) {
		_strncpy(buf, buffer, size);
		return(i);
	} else
		return(0);
}
