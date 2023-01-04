/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: program.h 2.2 88/04/08 14:10:46 root Exp $ RISC Source Base"

/*
 *	format of instruction table, mnemonics[]
 *
 *	The fields are used as follows:
 *
 *	name		Holds a pointer to the string that is the
 *			mnemonic's "name".  Among other things, this
 *			string is used by the lexical analysis and
 *			parsing routines to identify mnemonics in the
 *			input file.
 *
 *	tag		Flags field to control special processing.
 *
 *	nbits		Number of bits for the opcode.  Normally used
 *			as the first argument to generate() for obj
 *			code generation.
 *
 *	opcode		Instruction opcode.  Normally used as one of
 *			the arguments to generate() for obj code
 *			generation.
 *
 *	opndtypes	Pointer to a 0 terminated array of longs, each
 *			of which is a valid type for the corresponding
 *			operands.  Is also used to check for correct
 *			number of operands during operand parsing.
 *
 *	index
 *
 *	next
 *
 */

struct mnemonic {
    char *name;
    unsigned short tag;
    unsigned short nbits;
    long opcode;
    unsigned long *opndtypes;
    unsigned short index;
    short next;
};

/* operand semantic information */
/* variables that may be needed in future fp releases:
 *	short fptype;		1=single fp ; 2=double fp constant
 * 	double fpexpval;	holds value of floating point expression */


/*
 *  Operand structure, fields are as follows:
 *
 *	type		Type of the final operand, set to one of the
 *			possible types set in program.h.  I.E.:
 *			ABSMD, REGMD, EXADMD, etc.
 *
 *	exptype		Type of the expression, either set in operand() or
 *			copied from the symbol table entry type field for
 *			symbols.  I.E.:  ABS, TXT, DAT, BSS, etc.
 *
 *	symptr		Pointer to symbol table entry if operand is a
 *			symbol.
 *
 *	expval		Holds value of integer expression, register number,
 *			coded float, first word of a double constant, or
 *			flag for hi16() vs lo16() of symbolic address.
 *
 *	fasciip		Holds second word of a double constant.
 *
 */

typedef struct {
    unsigned char type;		/* Type of operand, REGMD, ABSMD, etc */
    unsigned char exptype;	/* Type of expression, ABS, TXT, DAT, etc */
    symbol *symptr;		/* Pointer to symbol table entry for operand */
    long expval;		/* Integer exp value, reg number, etc */
    char *fasciip;		/* holds second word of a double constant */
    int opflags;		/* Flags to control special processing */
} OPERAND;

/*
 *	Possible bits that can be set independently in opflags to
 *	control special processing.
 */

#define HI16		(0x01)	/* High 16 bits of 32 bit relocation */
#define LO16		(0x02)	/* Low 16 bits of 32 bit relocation */

/*
 *	Modes and other stuff.
 */


#define	PSEUDO		(0x25)

#define REGMD		(0x1)	/* register mode */
#define REGMDS		(0x2)	/* register mode (scaled) */
#define CREGMD		(0x3)	/* control register mode */
#define ABSMD		(0x4)	/* absolute address mode */
#define EXADMD		(0x5)	/* external address mode (PC relative) */

#define TREGMD		(1 << REGMD)
#define TREGMDS		(1 << REGMDS)
#define TCREGMD		(1 << CREGMD)
#define TABSMD		(1 << ABSMD)
#define TEXADMD 	(1 << EXADMD)

#define MEM		(TABSMD | TEXADMD)
#define DEST		(MEM | TREGMD)
#define GEN		(DEST | TABSMD)

#define MAXREGNO	31	/* Maximum value of regular register number */
#define MAXCREGNO	63	/* Maximum value of control register number */
