/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/swab.c	1.4"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Swap bytes in 16-bit [half-]words
 * for going between the 11 and the interdata
 */

void
_swab(pf, pt, n)
register char *pf, *pt;
register int n;
{
	n >>= 1;
	while(--n >= 0) {
		*pt++ = *(pf + 1);
		*pt++ = *pf;
		pf += 2;
	}
}
