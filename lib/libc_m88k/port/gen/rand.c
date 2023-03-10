/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/rand.c	1.4"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/

static long randx=1;

void
_srand(x)
unsigned x;
{
	randx = x;
}

int
_rand()
{
	return(((randx = randx * 1103515245L + 12345)>>16) & 0x7fff);
}
