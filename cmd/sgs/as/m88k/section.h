#ident "@(#) $Header: section.h 4.1 88/04/26 17:00:35 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


/*
 * output section information
 *
 *	The s_typ field takes on the same values as the styp field in
 *	the symbol table entry structure.
 */

struct scninfo {
    FILE *s_fd;			/* file pointer */
    short s_typ;		/* section symbol type */
    long s_any;			/* >0 => code generated */
    long s_up;			/* !=0 => round size */
    long s_buf[TBUFSIZ];	/* code output buffer */
    long s_cnt;			/* number of elements in buffer */
};
