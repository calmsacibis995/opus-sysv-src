#ident "@(#) $Header: types.h 4.1 88/04/26 17:22:26 root Exp $ RISC Source Base"
/****************************************************************************/
/* types.h      this file contains data structures used by the scheduler    */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include <stdio.h>

# define TRUE   1
# define FALSE  0
# define EQUAL  0       /* lexographic strcmp */
# define REG_NUM 32     /* m88000 has 32 GPRs */

/****************************************************************************/
/*      All of this typedeffing is to provide portablility to machines with */
/*      Different memory arangements                                        */
/****************************************************************************/

typedef unsigned char   REG;

typedef unsigned long   BITMASK;
typedef char            PRIO;
typedef char            OP;

typedef unsigned char    REGMAP;/*  0   implies D  register    */
				/*  1   implies S1 register    */
				/*  2   implies S2 register    */
# define NOT_USED (3)           /*  3   implies field not used */
# define NOT_REG  (4)           /*  4   implies field is string*/

typedef unsigned char    FMT;
typedef unsigned char    BOOLEAN;
typedef unsigned char    UNIT;
typedef unsigned char    BUSY;
typedef unsigned char    CYCLE;
typedef unsigned char    LATENT;

typedef struct OPCODE_TABLE Opcode;

typedef struct INSTRUCTION Inst;

typedef struct LABEL Label;

typedef struct SYMBOL Symbol;
typedef unsigned char Type;
typedef unsigned char State;

typedef struct WRITE_SLOT Write_slot;

struct WRITE_SLOT	{	BITMASK dest;	/* register written */
				UNIT	unit;	/* Function unbusy  */
				PRIO	prio;	/* Write arbitartion*/
			};

struct OPCODE_TABLE	{	char	*spelling;	/* see table.c      */
				FMT	format;		/* parsing info     */
				REGMAP	regs[3];	/* parsing info     */
				UNIT	unit;		/* which functional */
				BUSY	busy;		/* pipeline stalls  */
				BITMASK src1,		/* scoreboards      */
					src2,
					dused,
					dwrite;
				LATENT	latency;	/* length of pipe   */
			};

struct INSTRUCTION	{	Inst	*next;		/* link		      */
				BITMASK src[2],		/* source scoreboards */
					dest[2];	/* result registers   */
				Symbol  *opcode,	/*                    */
					*operand[3];	/*                    */
				char	spelling[4];	/* printing delimiters */
			};

# define sources(i) ((i)->src[0]|(i)->src[1])
# define results(i) ((i)->dest[0]|(i)->dest[1])

struct	LABEL		{	Label	*next;	/* link                */
				Symbol  *symbol;
				Inst    *inst;	/* code                */
				int     use;	/* users of label      */
				BITMASK before,	/* used before modified*/
					refs,	/* source operand      */
					defs;	/* result operand      */
			};

/****************************************************************************/
/* symbol->spelling was given this size to allow br to be changed into br.n */
/* by inst->opcode++                                                        */
/****************************************************************************/
/* all symbols created in the 'normal' way have this size field recomputed  */
/* to the actual number of characters needed                                */
/****************************************************************************/

struct SYMBOL   {       Type    type;
			State   state;
			Symbol  *next;
			union   {       Label   *label; /*********************/
					Inst    *inst;  /* this UNION avoids */
					Opcode  *opcode;/* a lot of pointer  */
					Symbol  *symbol;/* casting           */
					long    value;  /*********************/
				} def;
			char    spelling[12];
		};

# define NORM   0x01
# define STORE  0x02
# define LOAD   0x04
# define C      0x08 /* C and L must have disjoint bits     */
# define L      0x10
# define SUB    0x20 /* with branches, and still be unique  */
# define JMP    0x40
# define BR     0x78 /* BR and SUB must have bits in common */

/****************************************************************************/
/*      CODE READABILITY MACROS                                             */
/****************************************************************************/

# define isopcode(i)      ((i)->opcode->def.opcode)
# define branches(i)      (isopcode(i) && isopcode(i)->format & BR)
# define conditional(i)   (isopcode(i)->format & C)
# define branch(i)        (isopcode(i)->format & L)
# define direct(i)        (isopcode(i)->format == L)
# define labeled(i)	  (isopcode(i)->format & (L|C))
# define jump(i)          (isopcode(i)->format == JMP)
# define load(i)          (isopcode(i)->format & LOAD)
# define store(i)         (isopcode(i)->format & STORE)
# define computational(i) (isopcode(i)->format & NORM)
# define calls(op)        (op->format & SUB)
# define delayed(i)       (isopcode(i)->busy)
# define writes(i)        (isopcode(i)->dwrite)
# define store_double(op) (op && op->format == STORE && op->busy)
# define local_label(l)   (l->symbol->spelling[0] == '@')

/* I could have used <ctypes.h> for some of the below */

# define TERM   0x01
# define SEMI   0x02
# define WHITE  0x04
# define STARTS 0x08
# define CONT   0x10
# define DIGIT  0x20
# define LOWER  0x40
# define DELIM  0x80

extern char    _char_table[];

# define term(c)         (_char_table[(c)+1] & TERM)
# define term_or_semi(c) (_char_table[(c)+1] & (TERM|SEMI))
# define iswhite(c)      (_char_table[(c)+1] & WHITE)
# define starts_name(c)  (_char_table[(c)+1] & STARTS)
# define continues_name(c) (_char_table[(c)+1] & CONT)
# define isdigit(c)      (_char_table[(c)+1] & DIGIT)
# define islower(c)      (_char_table[(c)+1] & LOWER)
# define delimiter(c)    (_char_table[(c)+1] & DELIM)

# define MEMORY 4
# define FPADD  2
# define FPMUL  3
# define IMUL   1
# define CORE   0

/****************************************************************************/
/* In the timing table "table.c", values of 0-2 are used to reindex the     */
/* instruction->operand[] fields to rearange the field ordering with the    */
/* actual instruction register orderings.  The following list define other  */
/* manipulations on the operand fields, and help in creating a symbol table */
/****************************************************************************/
/* In addition, these define the TYPE of Symbol types                       */
/****************************************************************************/

# define REGISTER       0x04
# define CONDITION      0x08
# define LABEL_FIELD    0x10
# define CONTROL_REG    0x20
# define VECTOR         0x40
# define INSTRUCTION    0x80    /* note struct INSTRUCTION is invisible */
				/* except via its typedef Inst          */
# define ANYTHING       0xfc

# define CHAINED	0x01	/* has this branch been forwarded       */
# define chained(s) ((s)->state&CHAINED)

# define constant(op)     (op->type == LABEL_FIELD)
