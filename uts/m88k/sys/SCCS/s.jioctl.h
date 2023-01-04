h33406
s 00076/00000/00000
d D 1.1 90/03/06 12:29:51 root 1 0
c date and time created 90/03/06 12:29:51 by root
e
u
U
t
T
I 1
#ifndef _SYS_JIOCTL_H_
#define _SYS_JIOCTL_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
**	Unix to Jerq I/O control codes
*/

#define	JTYPE		('j'<<8)
#define	JBOOT		(JTYPE|1)
#define	JTERM		(JTYPE|2)
#define	JMPX		(JTYPE|3)
#define	JTIMO		(JTYPE|4)	/* Timeouts in seconds */
#define	JWINSIZE	(JTYPE|5)
#define	JTIMOM		(JTYPE|6)	/* Timeouts in millisecs */
#define	JZOMBOOT	(JTYPE|7)
#define JAGENT		(JTYPE|9)	/* control for both directions */
#define JTRUN		(JTYPE|10)	/* send runlayer command to layers*/

struct jwinsize
{
	char	bytesx, bytesy;	/* Window size in characters */
	short	bitsx, bitsy;	/* Window size in bits */
};

/**	Channel 0 control message format **/

struct jerqmesg
{
	char	cmd;		/* A control code above */
	char	chan;		/* Channel it refers to */
};

/*
**	Character-driven state machine information for Jerq to Unix communication.
*/

#define	C_SENDCHAR	1	/* Send character to layer process */
#define	C_NEW		2	/* Create new layer process group */
#define	C_UNBLK		3	/* Unblock layer process */
#define	C_DELETE	4	/* Delete layer process group */
#define	C_EXIT		5	/* Exit */
#define	C_DEFUNCT	6	/* Send terminate signal to proc. group */
#define	C_SENDNCHARS	7	/* Send several characters to layer proc. */
#define	C_RESHAPE	8	/* Layer has been reshaped */
#define C_RUN           9       /* Run command in layer */

/*
**	Usual format is: [command][data]
*/

/*
 *	This defines things to do with the host agent on the Blit.
 */

struct bagent{
	int size;	/* size of src string going in and dest string out */
	char * src;	/* address of the source byte string */
	char * dest;	/* address of the destination byte string */
};

#endif	/* ! _SYS_JIOCTL_H_ */
E 1
