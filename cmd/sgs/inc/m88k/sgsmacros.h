#ident "@(#) $Header: sgsmacros.h 4.1 88/04/26 17:11:20 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 */

#define MAXLDIRS	8
#define COMF77		"(common)"

#define max(a,b)		(a<b ? b: a)
#define min(a,b)		(a>b ? b: a)

#define size(str)		(strlen(str) + 1)

#define equal(a,b,l)		(strncmp(a,b,l) == 0)

#define copy(a,b,l)		strncpy(a,b,l)

