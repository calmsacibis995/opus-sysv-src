h50358
s 00079/00000/00000
d D 1.1 90/03/06 12:28:13 root 1 0
c date and time created 90/03/06 12:28:13 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:nudnix/string.c	10.3"

/*	This file originally contained 32B assembly code for the following
 *	C library routines.  These assembly routines have been replaced
 *	with the original C library C code for porting to the MC-68020.
 *	Future optimization and performance enhancements may want to
 *	code these routines in 68k assembly code.  However with the
 *	internal cache of the MC-68020, little performance may be realized.
 *							KJR
 */

strcmp(s1, s2)
register char *s1, *s2;
{
	if (s1 == s2)
		return(0);
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(*s1 - *--s2);
}

strncmp(s1, s2, n)
register char *s1, *s2;
register n;
{
	if (s1 == s2)
		return(0);
	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return((n < 0)? 0: *s1 - *--s2);
}

strlen(s)
register char *s;
{
	register n;

	n = 0;
	while (*s++)
		n++;
	return(n);
}

char *
strcpy(s1, s2)
register char *s1, *s2;
{
	register char *os1;

	os1 = s1;
	while (*s1++ = *s2++)
		;
	return(os1);
}

char *
strncpy(s1, s2, n)
register char *s1, *s2;
register n;
{
	register char *os1 = s1;

	n++;				
	while ((--n > 0) &&  ((*s1++ = *s2++) != '\0'))
		;
	if (n > 0)
		while (--n > 0)
			*s1++ = '\0';
	return (os1);
}
E 1
