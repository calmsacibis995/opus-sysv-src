/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/tell.c	1.6"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * return offset in file.
 */

extern long _lseek();

long
_tell(f)
int	f;
{
	return(_lseek(f, 0L, 1));
}
