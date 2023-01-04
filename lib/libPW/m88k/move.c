#ident "@(#) $Header: move.c 4.1 88/04/26 18:02:53 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


char *
move(a,b,n)
register char *a, *b;
int n;
{
	register i;
	char *rs;

	rs = b;
	for (i=0; i<n; i++)
		*b++ = *a++;
	return(rs);
}
