
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)head:move.c	1.1	1.2 (Fairchild) 8/8/86"

/*
 replace clipper/move.c using memcpy
*/

move(s2, s1, n)		

char	*s1, *s2;	/* s1: destination addr.
			   s2: source addr.	*/
int	n;		/* bytes to move	*/

{
    char *memcpy();

    (void) memcpy(s1, s2, n);	/* copy n bytes from s2 to s1	*/

}

