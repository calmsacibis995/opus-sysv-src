#ifndef _NSADDR_H_
#define _NSADDR_H_

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)head:nsaddr.h	1.3.1.1"
/*
	stoa - convert string to address

	atos - convert address to string

	header file
*/


#define	OCT	0	/* octal type */
#define	HEX	1	/* hexadecimal type */
#define	RAW	2	/* string type */
#define KEEP	8	/* keep protocol field	*/

struct address {
	char		*protocol;
	struct netbuf	addbuf;
};

struct netbuf	*stoa(/* str, netbuf */);
char		*atos(/* str, netbuf, type */);
struct address	*astoa(/* str, addr */);
char		*aatos(/* str, addr, type */);

#endif	/* ! _NSADDR_H_ */
