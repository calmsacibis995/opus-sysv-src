#ident "@(#) $Header: symbols.h 4.1 88/04/26 17:00:48 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#include "systems.h"

typedef char BYTE;

/*
 *	Possible symbol type values for symbol table entry 'styp' field.
 *	Also used in the section information structure element 's_typ',
 *	and the operand structure element 'exptype'.
 *
 *	Since TYPE is a mask composed of all bits set in the basic
 *	types, it can be used to mask off the basic type.
 *
 */

#define UNDEF	(000)
#define ABS	(001)
#define TXT	(002)
#define DAT	(003)
#define BSS	(004)
#define TYPE	(UNDEF|ABS|TXT|DAT|BSS)
#define	SECTION	(010)
#define TVDEF	(020)
#define EXTERN	(040)

/*
 *	A symbol name, either literal in 'name' or indirect via a 0 in
 *	'zeroes' and an offset into a table of names longer than what
 *	will fit in 'name'.
 */

typedef union {
    char name[9];	/* The literal name string if it fits */
    struct {
	long zeroes;	/* Always zero if offset is valid */
	long offset;	/* Offset into string table for larger names */
    } tabentry;
} name_union;

typedef struct {
    name_union _name;	/* Symbol name, either literal or indirect */
    BYTE tag;
    short styp;		/* Symbol type, UNDEF, ABS, TXT, DAT, etc */
    long value;		/* Symbol value, if known */
    short maxval;	/* Used in span dependent optimizations */
    short sectnum;	/* (COFF) Section number in which symbol found */
} symbol;

#define SYMBOLL	sizeof(symbol)

typedef struct {
    char name[sizeof (name_union)];
    BYTE tag;
    BYTE val;
    BYTE nbits;
    long opcode;
    symbol *snext;
} instr;

#define INSTR sizeof(instr);

#define INSTALL		(1)
#define N_INSTALL	(0)
#define USRNAME		(0)
#define MNEMON		(1)

typedef union {
    symbol *stp;
    instr *itp;
} upsymins;

/*
 *	Possible value types for symbol structure 'tag' field.
 */

#define	INDEX	(0)
#define	CHARPTR	(1)
