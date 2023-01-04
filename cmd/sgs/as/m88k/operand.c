/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: operand.c 2.3 88/04/08 14:19:32 root Exp $ RISC Source Base"

#include <stdio.h>
#include <ctype.h>
#include "as_ctype.h"
#include "symbols.h"
#include "program.h"
#include "instab.h"
#include "errors.h"
#include "dbug.h"

#define streq(a,b) (strcmp(a,b)==0)

extern char *oplexptr;		/* set up for oplex() by yyparse() */

extern upsymins *lookup ();

extern OPERAND *opertop;	/* bottom of expression stack & target for
				   current operand */
OPERAND *tiptop;		/* top of the expression stack, always
				   starts at opertop */

/* expr1() return codes */
#define ERROR	(-1)
#define FNDEXPR  (0)
#define FNDFLOAT (1)

/*
 *	Register table, might be useful to speed this up with hashing
 *	since it currently does a linear search...   Also might
 *	be integrated with the symbol table stuff...
 */

struct regitem {
    char *name;			/* Literal string for comparisons */
    unsigned char reg;		/* Register value */
};

static struct regitem regitems[] = {
    {"r0", 0},
    {"r1", 1},
    {"r2", 2},
    {"r3", 3},
    {"r4", 4},
    {"r5", 5},
    {"r6", 6},
    {"r7", 7},
    {"r8", 8},
    {"r9", 9},
    {"r10", 10},
    {"r11", 11},
    {"r12", 12},
    {"r13", 13},
    {"r14", 14},
    {"r15", 15},
    {"r16", 16},
    {"r17", 17},
    {"r18", 18},
    {"r19", 19},
    {"r20", 20},
    {"r21", 21},
    {"r22", 22},
    {"r23", 23},
    {"r24", 24},
    {"r25", 25},
    {"r26", 26},
    {"r27", 27},
    {"r28", 28},
    {"r29", 29},
    {"r30", 30},
    {"r31", 31},
    {NULL, 0}
};

/*
 *	Clear out any leftover operand stuff.
 */

static void clearop (opnd)
OPERAND *opnd;
{
    opnd -> type = 0;
    opnd -> exptype = 0;
    opnd -> symptr = NULL;
    opnd -> expval = 0L;
    opnd -> fasciip = NULL;
    opnd -> opflags = 0;
}

/*
 *	for expression parsing by routines other than operand() 
 *	(which calls expr1())
 */

int expr ()
{
    register int rtnval;

    DBUG_ENTER ("expr");
    tiptop = opertop;
    clearop (tiptop);
    switch (expr1 ()) {
	case FNDEXPR: 
	    rtnval = *oplexptr;
	    break;
	case FNDFLOAT: 
	    as_error (ERR_YYERROR, "expecting expression not float constant");
	    rtnval = -1;
	    break;
	default: 
	    rtnval = -1;
    }
    DBUG_RETURN (rtnval);
}


/* for use by routines other than operand() to parse floating point expressions */

int fp_expr () 
{
    register int rtnval;

    DBUG_ENTER ("fp_expr");
    tiptop = opertop;
    clearop (tiptop);
    switch (expr1 ()) {
	case FNDFLOAT: 
	    rtnval = *oplexptr;
	    break;
	case FNDEXPR: 
	    if (opertop -> fasciip != NULL) {
		rtnval = *oplexptr;
	    } else {
		as_error (ERR_YYERROR, "expecting float constant not expression");
		rtnval = -1;
	    }
	    break;
	default: 
	    rtnval = -1;
	    break;
    }
    DBUG_RETURN (rtnval);
}

/* operand tokens - used by operand(), expr1(), and oplex() */

#define	ID	(0200)
#define REG	(0201)
#define NUM	(0202)
#define FPNUM	(0203)
#define FREG	(0204)
#define SREG	(0205)
#define DREG	(0206)
#define XREG	(0207)

/* operand parser - returns 0 on success */

int operand (scaledflag)
int scaledflag;
{
    register struct regitem *regp;
    register char *temp;
    register int rtnval;

    DBUG_ENTER ("operand");
    DBUG_PRINT ("opnd1", ("operand string is '%s'", oplexptr));
    tiptop = opertop;
    clearop (tiptop);
    if (regfound (oplexptr, scaledflag)) {
	DBUG_RETURN (0);
    }
    /* special test for lo16() and hi16() pseudo functions */
    for (temp=oplexptr; *temp != '\000' && *temp != '('; temp++) {;}
    if (*temp == '(') {
	*temp = '\000';
	if (streq (oplexptr, "lo16") || streq (oplexptr, "hi16")) {
	    *temp = '(';
	    oplexptr = temp;
	    if (expr1 () == FNDEXPR) {
		DBUG_PRINT ("x16", ("expval = %#lx", opertop -> expval));
		if (opertop -> exptype == ABS) {
		    opertop -> type = ABSMD;
		} else {
		    opertop -> type = EXADMD;
		}
		if (*(temp - 4) == 'h') {
		    opertop -> opflags |= HI16;
		} else if (*(temp - 4) == 'l') {
		    opertop -> opflags |= LO16;
		} else {
		    as_error (ERR_BUG, "botched handling of hi16/lo16");
		}
		DBUG_PRINT ("x16", ("expval = %#lx", opertop -> expval));
	    } else {
		as_error (ERR_YYERROR, "illegal expression for lo16() or hi16()");
	    }
	    DBUG_RETURN (*oplexptr);
	}
	*temp = '(';
    }
    /* expr */
    switch (expr1 ()) {
	case FNDFLOAT: 
	case FNDEXPR:
	    if (opertop -> exptype == ABS) {
		opertop -> type = ABSMD;
	    } else {
		opertop -> type = EXADMD;
	    }
	    rtnval = *oplexptr;
	    break;
	case ERROR: 
	    rtnval = -1;
	    break;
    }
    DBUG_RETURN (rtnval);
}


/*
expr1() parses an expression returning:

	FNDFLT if a floating point constant was found
	FNDEXP for an expression
	ERROR if an error is encountered
*/

static int expr1 () 
{
    register int op = 0;
    register int min = 0;
    register int comp = 0;
    register int opresult;

    DBUG_ENTER ("expr1");
    DBUG_PRINT ("expr1", ("evaluate expression '%s'", oplexptr));
    while (1) {
	while (*oplexptr == '-') {	/* unary minus */
	    min ^= 1;
	    oplexptr++;
	}
	while (*oplexptr == '~') {	/* one's complement */
	    comp ^= 1;
	    oplexptr++;
	}
	while (*oplexptr == '+') {	/* unary plus */
	    oplexptr++;
	}
	opresult = oplex ();
	DBUG_PRINT ("oprslt", ("oplex found %#o", opresult));
	switch (opresult) {
	    case FPNUM: 
		if (tiptop != opertop + 1) {	/* only [-]* <fpnum> allowed */
		    DBUG_RETURN (ERROR);
		}
		if (comp) {
		    DBUG_RETURN (ERROR);
		}
		if (min) {
		    (tiptop - 1) -> expval |= (1L << 31);
		    *(--(tiptop - 1) -> fasciip) = '-';
		}
		DBUG_RETURN (FNDFLOAT);
	    case '(': 
		DBUG_PRINT ("expr1", ("found '(', recurse"));
		if (expr1 () != FNDEXPR) {
		    DBUG_RETURN (ERROR);
		}
		if (*oplexptr != ')') {
		    as_error (ERR_YYERROR, "unbalanced parentheses");
		    DBUG_RETURN (ERROR);
		}
		(tiptop - 1) -> fasciip = NULL;
		oplexptr++;
	    case ID: 
	    case NUM: 
		DBUG_PRINT ("expr", ("continue with ID or NUM"));
		if (min) {
		    min = 0;
		    if ((tiptop - 1) -> symptr != NULL) {
			as_error (ERR_YYERROR, "Illegal unary minus");
		    }
		    (tiptop - 1) -> expval = -(tiptop - 1) -> expval;
		    if ((tiptop - 1) -> fasciip != NULL) {
			*(--(tiptop - 1) -> fasciip) = '-';
		    }
		}
		if (comp) {
		    comp = 0;
		    if ((tiptop - 1) -> symptr != NULL) {
			as_error (ERR_YYERROR, "Illegal one's complement");
		    }
		    (tiptop - 1) -> expval = ~(tiptop - 1) -> expval;
		    if ((tiptop - 1) -> fasciip != NULL) {
			*(--(tiptop - 1) -> fasciip) = '~';
		    }
		}
		if (op) {	/* perform op */
		    tiptop--;
		    DBUG_PRINT ("expr", ("process op '%c'", op));
		    switch (op) {
			case '<': 	/* Bit field spec */
			    if ((tiptop - 1) -> symptr != NULL ||
				    tiptop -> symptr != NULL ||
			            *oplexptr++ != '>') {
				as_error (ERR_YYERROR, "Illegal bit field spec");
			    }
			    (tiptop - 1) -> symptr = NULL;
			    (tiptop - 1) -> exptype = ABS;
			    (tiptop - 1) -> expval <<= 5;
			    (tiptop - 1) -> expval |= tiptop -> expval & 0x1F;
			    DBUG_PRINT ("bf", ("bit field spec %#x", 
				    (tiptop -1) -> expval));
			    break;
			case '|': 
			    if ((tiptop - 1) -> symptr == NULL) {
				(tiptop - 1) -> symptr = tiptop -> symptr;
				(tiptop - 1) -> exptype = tiptop -> exptype;
			    } else if (tiptop -> symptr != NULL) {
				as_error (ERR_YYERROR, "Illegal bitwise or");
			    }
			    (tiptop - 1) -> expval |= tiptop -> expval;
			    break;
			case '&': 
			    if ((tiptop - 1) -> symptr == NULL) {
				(tiptop - 1) -> symptr = tiptop -> symptr;
				(tiptop - 1) -> exptype = tiptop -> exptype;
			    } else if (tiptop -> symptr != NULL) {
				as_error (ERR_YYERROR, "Illegal bitwise and");
			    }
			    (tiptop - 1) -> expval &= tiptop -> expval;
			    break;
			case '+': 
			    if ((tiptop - 1) -> symptr == NULL) {
				(tiptop - 1) -> symptr = tiptop -> symptr;
				(tiptop - 1) -> exptype = tiptop -> exptype;
			    } else if (tiptop -> symptr != NULL) {
				as_error (ERR_YYERROR, "Illegal addition");
			    }
			    (tiptop - 1) -> expval += tiptop -> expval;
			    break;
			case '-': 
			    if ((tiptop - 1) -> symptr == NULL) {
				if (tiptop -> symptr != NULL) {
				    as_error (ERR_YYERROR, "Illegal subtraction");
				}
				(tiptop - 1) -> symptr = NULL;
				(tiptop - 1) -> exptype = ABS;
				(tiptop - 1) -> expval -= tiptop -> expval;
			    } else if (tiptop -> symptr == NULL) {
				(tiptop - 1) -> expval -= tiptop -> expval;
			    } else if ((tiptop - 1) -> exptype == tiptop -> exptype) {
				(tiptop - 1) -> expval =
				    ((tiptop - 1) -> symptr -> value -
					tiptop -> symptr -> value) +
				    ((tiptop - 1) -> expval - tiptop -> expval);
				(tiptop - 1) -> symptr = NULL;
				(tiptop - 1) -> exptype = ABS;
			    } else {
				as_error (ERR_YYERROR, "Illegal subtraction");
				(tiptop - 1) -> symptr = NULL;
				(tiptop - 1) -> exptype = ABS;
				(tiptop - 1) -> expval = 0;
			    }
			    break;
			case '*': 
			    if ((tiptop - 1) -> symptr != NULL ||
				    tiptop -> symptr != NULL) {
				as_error (ERR_YYERROR, "Illegal multiplication");
			    }
			    (tiptop - 1) -> symptr = NULL;
			    (tiptop - 1) -> exptype = ABS;
			    (tiptop - 1) -> expval *= tiptop -> expval;
			    break;
			case '/': 
			    if ((tiptop - 1) -> symptr != NULL ||
				    tiptop -> symptr != NULL) {
				as_error (ERR_YYERROR, "Illegal division");
			    }
			    (tiptop - 1) -> symptr = NULL;
			    (tiptop - 1) -> exptype = ABS;
			    (tiptop - 1) -> expval /= tiptop -> expval;
			    break;
		    }
		    (tiptop - 1) -> fasciip = NULL;
		}
		break;
	    default: 
		DBUG_RETURN (ERROR);
	}
	switch (*oplexptr) {
	    case '<':		/* Bit field start spec */
	        op = '<';
	        oplexptr++;
	        break;
	    case '|': 
		op = '|';
		oplexptr++;
		break;
	    case '&': 
		op = '&';
		oplexptr++;
		break;
	    case '+': 
		op = '+';
		oplexptr++;
		break;
	    case '-': 
		op = '-';
		oplexptr++;
		break;
	    case '*': 
		op = '*';
		oplexptr++;
		break;
	    case '/': 
		op = '/';
		oplexptr++;
		break;
	    default: 
		DBUG_PRINT ("expval", ("expval = %#lx", (tiptop - 1) -> expval));
		DBUG_RETURN (FNDEXPR);
	}
    }
}


/* lexical analyzer for operand tokens */

static int oplex () 
{
    register char *ahead = oplexptr;
    register char tmpchar;

    DBUG_ENTER ("oplex");
    DBUG_PRINT ("oplx", ("processing '%s'", oplexptr));
    switch (*ahead) {
	case 'A': 
	case 'B': 
	case 'C': 
	case 'D': 
	case 'E': 
	case 'F': 
	case 'G': 
	case 'H': 
	case 'I': 
	case 'J': 
	case 'K': 
	case 'L': 
	case 'M': 
	case 'N': 
	case 'O': 
	case 'P': 
	case 'Q': 
	case 'R': 
	case 'S': 
	case 'T': 
	case 'U': 
	case 'V': 
	case 'W': 
	case 'X': 
	case 'Y': 
	case 'Z': 
	case 'a': 
	case 'b': 
	case 'c': 
	case 'd': 
	case 'e': 
	case 'f': 
	case 'g': 
	case 'h': 
	case 'i': 
	case 'j': 
	case 'k': 
	case 'l': 
	case 'm': 
	case 'n': 
	case 'o': 
	case 'p': 
	case 'q': 
	case 'r': 
	case 's': 
	case 't': 
	case 'u': 
	case 'v': 
	case 'w': 
	case 'x': 
	case 'y': 
	case 'z': 
	case '_': 
	case '.': 
	case '@':
	case '$':
	case '%':
	    {
		register symbol *symptr;

		DBUG_PRINT ("oplx2", ("found start of ID"));
		while (IDFOLLOW (*++ahead)) {;}
		if (tmpchar = *ahead) {
		    *ahead = '\0';
		    symptr = lookup (oplexptr, INSTALL, USRNAME) -> stp;
		    *ahead = tmpchar;
		} else {
		    symptr = lookup (oplexptr, INSTALL, USRNAME) -> stp;
		}
		oplexptr = ahead;
		if ((tiptop -> exptype = symptr -> styp & TYPE) == ABS) {
		    tiptop -> expval = symptr -> value;
		    DBUG_PRINT ("lex", ("ABS symbol = %#lx", tiptop -> expval));
		    tiptop -> symptr = NULL;
		} else {
		    DBUG_PRINT ("lex", ("REL symbol @ %#lx", tiptop -> symptr));
		    tiptop -> symptr = symptr;
		    tiptop -> expval = 0;
		}
		DBUG_PRINT ("lex", ("expval = %#lx", tiptop -> expval));
		tiptop -> fasciip = NULL;
		tiptop++;
		clearop (tiptop);
		DBUG_RETURN (ID);
	    }
	case '0': 
	case '1': 
	case '2': 
	case '3': 
	case '4': 
	case '5': 
	case '6': 
	case '7': 
	case '8': 
	case '9': 
	    {
		register long val;
		register base;
		register c;

		if ((val = *ahead - '0') == 0) {
		    c = *++ahead;
		    if (c == 'x' || c == 'X') {
			base = 16;
			val = 0;
			while (HEXCHAR (c = *++ahead)) {
			    val <<= 4;
			    if ('a' <= c && c <= 'f') {
				val += 10 + c - 'a';
			    } else if ('A' <= c && c <= 'F') {
				val += 10 + c - 'A';
			    } else {
				val += c - '0';
			    }
			}
		    } else {
			if (c == '.' || c == 'E' || c == 'e') {
			    base = 10;/* fp number */
			} else if (OCTCHAR (c)) {
			    base = 8;
			    val = c - '0';
			    while (OCTCHAR (c = *++ahead)) {
				val <<= 3;
				val += c - '0';
			    }
			} else {	/* just 0 */
			    val = 0;
			}
		    }
		} else {
		    base = 10;
		    while (DECCHAR (c = *++ahead)) {
			val *= 10;
			val += c - '0';
		    }
		    DBUG_PRINT ("oplx4", ("found value %d", val));
		}
		/* check if its a fp number */
		if ((base == 10) && (c == '.' || c == 'E' || c == 'e')) {
		    do {
			if (c == 'E' || c == 'e') {
			    /* exp for fp number, next char might be a
			       sign (PLUS/MINUS) so get it here */
			    if (((c = *++ahead) != '+') && (c != '-')) {
				ahead--;
			    }
			}
		    } while (isdigit (c = *++ahead) || isalpha (c));

		    *ahead = '\0';
		    if (validfp (oplexptr)) {
			tiptop -> fasciip = oplexptr;
			*(oplexptr = ahead) = c;
			tiptop -> symptr = NULL;
			tiptop++;
			clearop (tiptop);
			DBUG_RETURN (FPNUM);
		    } else {
			as_error (ERR_YYERROR, "invalid floating point constant");
			(void) fprintf (stderr, "\t\t... float constant: \"%s\"\n", oplexptr);
			*(oplexptr = ahead) = c;
			DBUG_RETURN (-1);
		    }
		} else {
		    *ahead = '\0';
		    tiptop -> fasciip = oplexptr;
		    *(oplexptr = ahead) = c;
		    tiptop -> exptype = ABS;
		    tiptop -> expval = val;
		    tiptop -> symptr = NULL;
		    tiptop++;
		    clearop (tiptop);
		    DBUG_PRINT ("oplx5", ("found NUM of val %d", val));
		    DBUG_RETURN (NUM);
		}
	    }
	case '\0': 
	    DBUG_RETURN (*oplexptr);
	default: 
	    DBUG_RETURN (*oplexptr++);
    }
}

/*
 * Check operand to see if it is a normal register or control register.
 * Return non-zero if a register was found.
 */

regfound (opstr, scaledflag)
char *opstr;
int scaledflag;
{
    register int regno;		/* Register value */
    register int control;	/* Non zero if register is control register */
    register int rtnval;
    register int count;

    DBUG_ENTER ("regfound");
    regno = 0;
    control = 0;
    rtnval = 0;
    if (*opstr == 'c') {
	opstr++;
	control++;
    }
    if (*opstr++ == 'r') {
	for (count = 0; count < 2 && DECCHAR (*opstr); count++) {
	    rtnval = 1;			/* After at least one digit */
	    regno *= 10;
	    regno += *opstr++ - '0';
	}
	DBUG_PRINT ("regno", ("regno = %d", regno));
	if (*opstr != '\000'
	    || (control && (regno > MAXCREGNO))
	    || (!control && (regno > MAXREGNO))) {
		rtnval = 0;
	} else if (rtnval) {
	    opertop -> expval = regno;
	    if (scaledflag) {
		opertop -> type = REGMDS;
		DBUG_PRINT ("regno", ("found scaled register %d", regno));
	    } else {
		if (control) {
		    DBUG_PRINT ("regno", ("found control register %d", regno));
		    opertop -> type = CREGMD;
		} else {
		    DBUG_PRINT ("regno", ("found regular register %d", regno));
		    opertop -> type = REGMD;
		}
	    }
	}
    }	
    DBUG_RETURN (rtnval);
}

