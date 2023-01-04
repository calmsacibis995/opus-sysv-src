/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: symbols2.h 2.2 88/04/08 14:11:49 root Exp $ RISC Source Base"

/*
 *	NINSTRS	= number of legal assembler instructions (from ops.out)
 *	NHASH	= the size of the hash symbol table
 *	NSYMS	= the number of symbols allowed in a user program
 */

#if FLOAT
#    define NINSTRS	(394)
#else
#    define NINSTRS	(358)
#endif

#define NHASH		(38999)
#define	NSYMS		(NHASH - NINSTRS)
