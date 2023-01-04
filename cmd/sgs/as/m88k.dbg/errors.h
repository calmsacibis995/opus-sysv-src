#ident "@(#) $Header: errors.h 4.1 88/04/26 17:00:22 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/



/*
 *  FILE
 *
 *	errors.h    error code definitions
 *
 *  SCCS
 *
 *	@(#)errors.h	1.47	10/27/87
 *
 *  SYNOPSIS
 *
 *	#include "errors.h"
 *
 *  DESCRIPTION
 *
 *	Define error numbers.  Each error is assigned a name of
 *	the form "ERR_XXX..", where XXX... is some mnemonically
 *	meaningful string for the specific error.
 *
 *	Each time an error condition (or warning) is encountered,
 *	the error handling routine "as_error" is called to
 *	issue an appropriate message.  The first argument to
 *	as_error is the error number.
 *
 */


#define ERR_CRIF	1	/* Can't read input file */
#define ERR_COOF	2	/* Can't open output file */
#define ERR_COTW	3	/* Can't open temporary file for write */
#define ERR_COTR	4	/* Can't open temporary file for read */
#define ERR_TCL		5	/* Error closing temporary file */
#define ERR_IBSS	6	/* Attempt to initialize bss */
#define ERR_TWR		7	/* Error writing temporary file */
#define ERR_NWSMO	8	/* No whitespace between mnemonic & operand */
#define ERR_STSYN	9	/* Statement syntax */
#define ERR_OPSYN	10	/* Operand syntax */
#define ERR_BADSTG	11	/* Bad string */
#define ERR_INVINS	12	/* Invalid instruction name */
#define ERR_OPDERR	13	/* Operand error */
#define ERR_OPDCNT	14	/* Operand count error */
#define ERR_OPDTYP	15	/* Operand type mismatch */
#define ERR_DIE		16	/* Too many errors (as_error() use only!) */
#define ERR_MDLBL	17	/* Multiply defined label */
#define ERR_LBLOS	18	/* Label defined outside scope of section */
#define ERR_USTE	19	/* Unbalanced symbol table entries */
#define ERR_USTE2	20	/* Missing scope endings */
#define ERR_USTE3	21	/* Too many scope ends */
#define ERR_CCSTF	22	/* Cannot create section temp file */
#define ERR_IAR		23	/* Invalid action routine */
#define ERR_BTFF	24	/* Bad temporary file format */
#define ERR_RSNST	25	/* Reference to symbol not in symbol table */
#define ERR_USST	26	/* Unknown symbol in symbol table */
#define ERR_STSOVF	27	/* Symbol table stack overflow */
#define ERR_IOFE	28	/* Internal object file error */
#define ERR_SYMOVF	29	/* Symbol table overflow */
#define ERR_HASHOVF	30	/* Hash table overflow */
#define ERR_CRSTBL	31	/* Cannot realloc string table */
#define ERR_CMSTBL	32	/* Cannot malloc string table */
#define ERR_URALIAS	33	/* Unresolved aliases */
#define ERR_MALLOC	34	/* Cannot malloc storage */
#define ERR_INVTYP	35	/* Invalid type */
#define ERR_OVF16	36	/* Absolute too big for 16 bits */
#define ERR_RSIAS	37	/* Reference to symbol in another section */
#define ERR_PCOVF8	38	/* Pc relative offset too big for 8 bits */
#define ERR_PCOVF16	39	/* Pc relative offset too big for 16 bits */
#define ERR_PCOVF26	40	/* Pc relative offset too big for 26 bits */
#define ERR_ABSBSS	41	/* Bss pseudo expression value not abs */
#define ERR_UDI		42	/* Unable to define identifier */
#define ERR_FPBD	43	/* File pseudo must precede all def's */
#define ERR_TMADIM	44	/* Too many array dimensions for sdb */
#define ERR_UNKSOPT	45	/* Unknown suboption */
#define ERR_CRSYMTBL	46	/* Cannot reduce sym table, symbols remain */
#define ERR_TBLOVFOP	47	/* Table overflow, some optimizations lost */
#define ERR_UNKOPT	48	/* Unknown option */
#define ERR_YYERROR	49	/* Syntax type error */
#define ERR_LITOVF	50	/* Overflow of literal constant field */
#define ERR_BUG		51	/* Internal bug detected */
#define ERR_OBSOLETE	52	/* Use of an obsolescent assembler feature */
