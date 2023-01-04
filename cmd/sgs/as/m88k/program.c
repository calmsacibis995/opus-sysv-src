/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:m78/program.c	1.21" */

#include <stdio.h>
#include <filehdr.h>
#include <sgs.h>
#include <storclass.h>
#include <systems.h>
#include <ctype.h>
#ifdef EXPR
#include <linker.h>
#endif
#include "symbols.h"
#include "gendefs.h"
#include "as_ctype.h"
#include "instab.h"
#include "program.h"
#include "ind.out"
#include "typ.out"
#include "scnhdr.h"
#include "syms.h"
#include "errors.h"
#include "dbug.h"

/* Defines for action by expovf routine. */
#define SIGNED		1
#define UNSIGNED	0
#define PRTERR		1
#define NOPRTERR	0

extern void symalias ();	/* Alias two symbols via 'def' pseudo */
extern void as_error ();
extern char *strcpy ();
extern char *symprt ();
extern void atodi ();
extern void opnd_dmp ();
extern void comment ();
extern unsigned short hash ();

int txtsec = -1;
int datsec = -1;
static int firstinstr = 1;

/*
 *	mnemonics[] is a table of machine instruction mnemonics used
 *	by yyparse().  These are accessed thru hashtab[].
 *
 */

static struct mnemonic mnemonics[] = {
#include "mnm.out"
    0
};

#define HASHSIZ (1007)

/*
 *	Note there are two hash tables in the assembler, one for mnemonics
 *	and one for symbols.  They both use the same hash function, but
 *	the tables may be different sizes so the size is also passed to
 *	the function to compute the hash value.  Maybe these two
 *	tables could be merged?
 */

static short mnhashtab[HASHSIZ];

/*
 *	Place predefined equates in the symbol table.  This is done
 *	by simulating a pseudo directive "set" for each equate.
 */

struct predefset {
    char *name;
    long value;
};

struct predefset defsets [] = {
    "eq", 0x2L,
    "ne", 0x3L,
    "gt", 0x4L,
    "le", 0x5L,
    "lt", 0x6L,
    "ge", 0x7L,
    "hi", 0x8L,
    "ls", 0x9L,
    "lo", 0xaL,
    "hs", 0xbL,
    "eq0", 0x2L,
    "ne0", 0xdL,
    "gt0", 0x1L,
    "le0", 0xeL,
    "lt0", 0xcL,
    "ge0", 0x3L,
    "s0z0", 0x1L,
    "s0z1", 0x2L,
    "s1z0", 0x4L,
    "s1z1", 0x8L,
    NULL, 0L
};

static int getexpr ();
static int chekoneid ();
static int getfp ();
static int chekid ();
static int procmnemonic ();
static int sgetc ();
static int lexline ();		/* Do lexical analysis on line */

#ifndef NO_DBUG
static char *opndprt ();
#endif

#if FLOAT
static void psdecint ();
static void psdecfp ();
static void psdblflt ();
#endif

#ifdef EXPR
static void genexpr ();
#endif

static void semantics ();	/* Do line semantic processing */
static void proclabel ();
static void finishline ();
static void semswitch ();
static void procoperands ();
static void testop1 ();
static void psbhw ();
static void pssection ();
static void psbss ();
static void psdef ();
static void psdim ();
static void psglobal ();
static void psstatic ();
static void psfiv ();
static void psln ();
static void pstag ();
static void psstring ();

static unsigned short action;	/* action routine to be called during
				   second pass */

extern int strictalign;		/* Non-zero if stricter alignment */
extern unsigned short line;	/* current line number */
extern unsigned short cline;	/* line number from `.ln' */
extern short bitpos;
extern short opt;		/* optimize flag */
extern long newdot;		/* up-to-date value of "." */
extern int previous;		/* previous section number */
extern symbol *dot;
extern FILE *fdin;
extern char *cfile;		/* Input file name as set by .file directive */
extern int maxnamlen;		/* Max number of characters in a file name */
extern struct scnhdr sectab[];
extern int mksect ();
extern void cgsect ();
extern void generate ();
extern upsymins *lookup ();	/* Look up symbols in the symbol table */
static char as_version[] = "02.01";

void fill ();
void fillbyte ();
void ckalign ();
void bss ();

#if FLOAT
static unsigned long fpval[3];
#endif

extern void deflab ();

char *oplexptr;		/* set up by yyparse() for use in oplex() */

#define OPMAX (50)

/*
 *	operstak[] is used to make operand list (operstak[0] is the first
 *	operand, operstak[1] the second, etc.). operstak[] is also used as
 *	an expression stack by expr().
 *
 */

OPERAND operstak[OPMAX];
OPERAND *opertop;	/* points to first unused element in operstak[] */

static struct token *curtoken;
static int numopnds;		/* Number of operands for instruction */
static int curopnd;
static struct mnemonic *instrptr;

/* yyparse() token types */

#define ENDOFLINE	(0)	/* End of assembly line token */
#define LABEL		(1)	/* Label token */
#define MNEMONIC	(2)	/* Mnemonic token, such as "add" or "extu" */
#define OPNDID		(3)	/* Identifier token */
#define OPNDIDS		(4)	/* Identifier token (scaled) */
#define OPNDSTR		(5)	/* String token, such as "myfile.c" */
#define OPND		(6)	/* Any other operand */

/* for yyparse level tokens, i.e. LABEL's, MNEMONIC's, and OPERAND's */

static struct token {
    char *ptr;		/* Pointer to string literal for token */
    char type;		/* Type identifier */
    short numopnds;	/* Number of operand tokens for this token */
} tokens[BUFSIZ / 2];


/* initializes the hash table for mnemonics */

static void mk_hashtab ()
{
    register struct mnemonic *curptr;
    register unsigned short hval;
    register int i;

    DBUG_ENTER ("mk_hashtab");
    DBUG_PRINT ("hshtb", ("clear out %d hash table entries", HASHSIZ));
    for (i = 0; i < HASHSIZ; i++) {
	mnhashtab[i] = -1;
    }
    DBUG_PRINT ("hshtb", ("initialize mnemonic hash table"));
    for (curptr = mnemonics; curptr -> name != NULL; curptr++) {
	hval = hash (curptr -> name, HASHSIZ);
	curptr -> next = mnhashtab[hval];
	mnhashtab[hval] = curptr - mnemonics;
    }
    DBUG_VOID_RETURN;
}


/* for use by routines other than yyparse(), typically on gencode.c */

struct mnemonic *find_mnemonic (name)
char *name;
{
    register unsigned short hval;
    register int index;
    register struct mnemonic *rtnval = (struct mnemonic *)NULL;

    DBUG_ENTER ("find_mnemonic");
    hval = hash (name, HASHSIZ);
    for (index = mnhashtab[hval]; index >= 0; index = mnemonics[index].next) {
	if (strcmp (mnemonics[index].name, name) == 0) {
	    rtnval = mnemonics + index;
	    break;
	}
    }
    DBUG_RETURN (rtnval);
}

/*
 *	yyparse() is the high level parser for assembly source it calls 
 *	operand() for operand parsing.  yyparse consists of a main loop
 *	(one time thru for each assembly line) containing two phases:
 *
 *	lexing - chopping the line up into labels, mnemonics, and operands.
 *		 identifier and string operands are recognized directly;
 *		 all operands have their whitespace removed.
 *
 *	processing - the labels are processed.  mnemonics are looked up.
 *		     a switch on the instruction index permits tailored
 *		     parsing and processing of the statement.  otherwise
 *		     operand() is called in a loop to parse the operands.
 *		     the operand types are checked.  a switch executes the 
 *		     statement semantics provided for the mnemonic on "ops.in".
 *
 *	Previous versions of this routine used fgets(), which would leave
 *	the tail end of a line longer than BUFSIZ pending in the input
 *	stream to be read as the next line.  This tail end of what was
 *	usually a VERY long comment line, would make little sense when
 *	parsed as the next instruction line.  Now we are careful to
 *	always read to the end of the line or EOF, discarding anything
 *	(without warning) that won't fit in the input buffer.
 */

void yyparse () 
{
    char linebuf[BUFSIZ];	/* contains line being parsed */
    char *bufp;
    int thischar = 0;

    DBUG_ENTER ("yyparse");
    mk_hashtab ();
    for (line = 1; thischar != EOF; line++) {
 	DBUG_PRINT ("line", ("now reading line %d", line));
 	bufp = linebuf;
 	while ((thischar = getc (fdin)) != EOF && thischar != '\n') {
 	    if (bufp < &linebuf[BUFSIZ-1]) {
 		*bufp++ = thischar;
 	    } else {
 		/* Just eat part of line too long, could give warning... */
 	    }
 	}
 	*bufp = '\000';
 	DBUG_PRINT ("line", ("read line '%s'", linebuf));
	if (lexline (linebuf)) {
	    semantics ();
	}
    }
#ifdef EXPR
    exprsect (); 
#endif
    DBUG_VOID_RETURN;
}

void fill (nbytes)
long nbytes;
{
    register long fillval;

    DBUG_ENTER ("fill");
    if (dot -> styp == TXT) {
	fillval = TXTNOP;
    } else {
	fillval = ((FILL << 24) | (FILL << 16) | (FILL << 8) | FILL);
    }
    while (nbytes >= 4) {
	generate (4 * BITSPBY, 0, fillval, NULLSYM);
	nbytes -= 4;
    }
    while (nbytes-- > 0) {
	generate (BITSPBY, 0, fillval, NULLSYM);
    }
    DBUG_VOID_RETURN;
}

void ckalign (size)
long size;
{
    long mod;

    DBUG_ENTER ("ckalign");
    DBUG_PRINT ("align", ("align to %ld at dot = %ld", size, newdot));
    if ((mod = (newdot % size)) != 0) {
	DBUG_PRINT ("align", ("needs %ld bytes of padding", size - mod));
	fill (size - mod);
    }
    DBUG_VOID_RETURN;
}

void fillbyte ()
{
    DBUG_ENTER ("fillbyte");
    if (bitpos) {
	generate (BITSPBY - bitpos, 0, FILL, NULLSYM);
    }
    DBUG_VOID_RETURN;
}

void bss (symptr, val, alignment, valtype)
register symbol *symptr;
long val;
long alignment;
short valtype;
{
    long mod;
    static int bsssec = -1;
    register symbol *bsssym;

    DBUG_ENTER ("bss");
    DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    if (valtype != ABS) {
	as_error (ERR_YYERROR, "`.bss' size not absolute");
    }
    if (val <= 0) {
	as_error (ERR_YYERROR, "Invalid `.bss' size");
    }
    if ((symptr -> styp & TYPE) != UNDEF) {
	as_error (ERR_YYERROR, "multiply define label in `.bss'");
    }
    if (bsssec < 0) {
	bsssym = lookup (_BSS, INSTALL, USRNAME) -> stp;
	bsssec = mksect (bsssym, STYP_BSS);
    }
    if (mod = sectab[bsssec].s_size % alignment) {
	sectab[bsssec].s_size += alignment - mod;
    }
    symptr -> value = sectab[bsssec].s_size;
    sectab[bsssec].s_size += val;
    symptr -> sectnum = bsssec;
    symptr -> styp |= BSS;
    DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    DBUG_VOID_RETURN;
}

/*
 *	lex the line
 *
 *		return 0 if error and no further processing needed on line
 *		return 1 if success and need more processing
 *  NOTES:
 *
 *	Under the current implementation, identifiers are described by
 *	the regular expression:
 *
 *		[@.A-Z_a-z][@$.0-9A-Z_a-z]*
 *
 */

static int lexline (ahead)
register char *ahead;
{
    /* nexttoken points to free location for next token */
    register struct token *nexttoken;
    register char *start;
    register char *end;
    register struct token *mnemtoken;
    register int quoted;
    
    DBUG_ENTER ("lexline");
    nexttoken = tokens;
    /* Scan line eating labels and whitespace until mnemonic is found */
    DBUG_PRINT ("mnem", ("beginning scanning for mnemonic"));
    mnemtoken = NULL;
    while (mnemtoken == NULL) {
	while (WHITESPACE (*ahead)) {
	    ahead++;
	}
	if (IDBEGIN (*ahead)) {
	    start = ahead;
	    while (IDFOLLOW (*++ahead)) {;}
	    end = ahead;
	    while (WHITESPACE (*ahead)) {
		ahead++;
	    }
	    if (*ahead == ':') {
		/* label */
		*end = '\000';
		DBUG_PRINT ("labels", ("found label '%s'", start));
		nexttoken -> type = LABEL;
		nexttoken++ -> ptr = start;
		ahead++;
	    } else if (WHITESPACE (*end) || !OPRCHAR (*ahead)) {
		/* mnemonic */
		*end = '\000';
		DBUG_PRINT ("mne", ("found mnemonic '%s'", start));
		mnemtoken = nexttoken;
		nexttoken -> type = MNEMONIC;
		nexttoken++ -> ptr = start;
	    } else {
		as_error (ERR_NWSMO);
		DBUG_RETURN (0);
	    }
	} else {
	    switch (*ahead) {
		case ';':
		case '\0': 
		case '\n': 
		    DBUG_PRINT ("eol", ("found end of line"));
		    nexttoken -> type = ENDOFLINE;
		    DBUG_RETURN (1);
		default: 
		    as_error (ERR_STSYN);
		    DBUG_RETURN (0);
	    }
	}
    }
    /*
     * scan operands; recognize id's and strings directly;
     * take out white space
     */
    DBUG_PRINT ("opnd", ("begin scanning for operands"));
    mnemtoken -> numopnds = 0;
    if (OPRCHAR (*ahead)) {
	while (1) {
	    mnemtoken -> numopnds++;
	    *end = '\000';	/* close preceding mnemonic or operand */
	    start = ahead;
	    if (IDBEGIN (*ahead)) {		/* try for an id */
		while (IDFOLLOW (*++ahead)) {;}
		end = ahead;
		while (WHITESPACE (*ahead)) {
		    ahead++;
		}
		if (!OPRCHAR (*ahead)) {
		    nexttoken -> type = OPNDID;
		    goto got_opnd;
		}
	    } else if (*ahead == '"') {		/* try for a string */
		quoted = 0;
		while (*++ahead) {
		    if (*ahead == '"' && !quoted) {
			end = ++ahead;
			while (WHITESPACE (*ahead)) {
			    ahead++;
			}
			if (!OPRCHAR (*ahead)) {
			    nexttoken -> type = OPNDSTR;
			    goto got_opnd;
			}
		    } else if (*ahead == '\\' && !quoted) {
			quoted = 1;
		    } else {
			quoted = 0;
		    }
		}
		if (*ahead == '\0') {
		    /* Bad string */
		    as_error (ERR_BADSTG);
		    DBUG_RETURN (0);
		}
	    } else {
		while (HOPRCHAR (*ahead)) {
		    ahead++;
		}
		end = ahead;
	    }
	    while (OPRCHAR (*ahead)) {
		/* remove any whitespace but be careful not to combine   */
		/* tokens previously separated by white space. we assume */
		/* that this is a sytax error.                    */
		if (WHITESPACE (*ahead)) {
		    ahead++;
		} else if (end == ahead) {
		    end++;
		    ahead++;
		} else if (HOPRCHAR (*(ahead - 1)) ||
			   /* no combining problem since previous
			      character was not a white space character 
			      */
			   end == start ||
			   /* no combining problem since this is first
			      character to be copied */
			   SINGCHAR (*(end - 1)) || SINGCHAR (*ahead)) {
		    /* no combining problem since at least one
		       of the characters always stands alone 
		       */
		    *end++ = *ahead++;
		} else {
		    /* Operand syntax error */
		    as_error (ERR_OPSYN);
		    DBUG_RETURN (0);
		}
	    }
	    nexttoken -> type = OPND;
got_opnd: 
	    nexttoken++ -> ptr = start;
	    /*
	     *  The following conversion of a '[' to a ',' is a blatent
	     *  hack to convert the two operand forms of the ld and st
	     *  instructions to look syntactically like the rest of the
	     *  three operand instructions.  Specifically, the form:
	     *
	     *		ld   r1,r2,r3    =>	ld   r1,r2,r3
	     *		ld   r1,r2[r3]   =>	ld   r1,r2,r3]
	     *
	     *  Then on the next token, the trailing ']' is eaten and used
	     *  to set the token type to the special form that indicates
	     *  that the ld or st is the scaled form rather than the unscaled
	     *  form.  This is a real syntactic wart in the current assembly
	     *  language convention...		(Fred Fish, 26-Sept-86)
	     *
	     */  
	    if (*ahead == '[') {
		*ahead = ',';
	    }
	    if (*ahead == ']') {
		*ahead = ' ';
		if ((nexttoken - 1) -> type == OPNDID) {
		    (nexttoken - 1) -> type = OPNDIDS;
		}
	    }
	    /* end blatent hack for scaled vs unscaled ld/st detection */
	    if (*ahead != ',') {
		break;
           }
	    while (WHITESPACE (*++ahead)) {;}
	}
    }
    *end = '\000';
    switch (*ahead) {
	case ';': 
	case '\n': 
	case '\0': 
	    nexttoken -> type = ENDOFLINE;
	    DBUG_RETURN (1);
	default: 
	    as_error (ERR_STSYN);
	    DBUG_RETURN (0);
    }
}

/*
 *	Process the line, after lexical analysis.
 *
 *	Starting at the first token, process all label tokens, and
 *	then at the first mnemonic token, call routine to process
 *	it (and all it's operand tokens).
 *
 */

static void semantics ()
{
    DBUG_ENTER ("semantics");
    curtoken = tokens;
    while (curtoken -> type != ENDOFLINE) {
	if (curtoken -> type == LABEL) {
	    proclabel ();
	} else {
	    if (!procmnemonic ()) {
		break;
	    }
	}
    }
    DBUG_VOID_RETURN;
}

/* .section <id>[,<string>] */

static void pssection ()
{
    register char *s;
    register int att = 0;
    register symbol *sectsym;

    DBUG_ENTER ("pssection");
    if (!chekid (1)) DBUG_VOID_RETURN;
    if (numopnds == 2) {
	if (curtoken[2].type != OPNDSTR) {
	    as_error (ERR_YYERROR, "expecting a string");
	    (void) fprintf (stderr, "\t\t... not \"%s\"\n", curtoken[2].ptr);
	} else {
	    s = (curtoken[2].ptr)++;
	    s[strlen (s) - 1] = '\000';
	    s++;
	    while (*s) {
		switch (*s++) {
		    case 'b': 
		        /* zero initialized block */
			att |= STYP_BSS;
			break;
		    case 'c': 
			/* copy */
			att |= STYP_COPY;
			break;
		    case 'i': 
			/* info */
			att |= STYP_INFO;
			break;
		    case 'd': 
			/* dummy */
			att |= STYP_DSECT;
			break;
		    case 'x': 
			/* executable */
			att |= STYP_TEXT;
			break;
		    case 'n': 
			/* noload */
			att |= STYP_NOLOAD;
			break;
		    case 'o': 
			/* overlay */
			att |= STYP_OVER;
			break;
		    case 'l': 
			/* lib */
			att |= STYP_LIB;
			break;
		    case 'w': 
			/* writable */
			att |= STYP_DATA;
			break;
		    default: 
			as_error (ERR_YYERROR, "invalid section attribute");
			break;
		}
	    }
	}
    } else if (numopnds > 2) {
	as_error (ERR_YYERROR, "'.section' takes one or two operands");
	(void) fprintf (stderr, "\t\t... not %d\n", numopnds);
	DBUG_VOID_RETURN;
    }
    sectsym = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
    cgsect (mksect (sectsym, att));
    DBUG_VOID_RETURN;
}

/* .bss <id>,<expr>,<expr> */

static void psbss ()
{
    register symbol *idsym;
    register alignval;

    DBUG_ENTER ("psbss");
    if (chekid (1) && getexpr (2)) {
	opertop++;		/* Does this have any effect?? */
	if (numopnds == 2) 
		alignval = 1;
	else if (getexpr (3)) {
	    if (operstak[1].exptype != ABS) {
		as_error (ERR_ABSBSS);
	    }
	    alignval = operstak[1].expval;
	} else
	    DBUG_VOID_RETURN;
	idsym = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
	bss (idsym, operstak[0].expval, alignval,
		 (short) operstak[0].exptype);
    }
    DBUG_VOID_RETURN;
}

static void psdef ()
{
    register symbol *symptr;

    DBUG_ENTER ("psdef");
    if (chekoneid ()) {
	symptr = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
	generate (0, DEFINE, NULLVAL, symptr);
    }
    DBUG_VOID_RETURN;
}

static void psdim ()
{
    DBUG_ENTER ("psdim");
    if (numopnds == 0) {
	as_error (ERR_YYERROR, "too few operands");
    } else {
	if (getexpr (1)) {
	    generate (0, SETDIM1, operstak[0].expval, operstak[0].symptr);
	    for (curopnd = 2; curopnd <= numopnds; curopnd++) {
		if (!getexpr (curopnd)) {
		    break;
		}
		generate (0, SETDIM2, operstak[0].expval, operstak[0].symptr);
	    }
	}
    }
    DBUG_VOID_RETURN;
}

static void psglobal ()
{
    register symbol *symptr;

    DBUG_ENTER ("psglobal");
    if (chekoneid ()) {
	symptr = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
	symptr -> styp |= EXTERN;
	DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    }
    DBUG_VOID_RETURN;
}

static void psstatic ()
{
    register symbol *symptr;

    DBUG_ENTER ("psstatic");
    if (chekoneid ()) {
	symptr = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
	symptr -> styp &= ~EXTERN;
	DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    }
    DBUG_VOID_RETURN;
}

/*
 *	Found a directive which takes one string operand.  I.E.:
 *
 *		.file		<string>
 *		.ident		<string>
 *		.version	<string>
 *
 */

static void psfiv ()
{
    register struct token *stoken;

    DBUG_ENTER ("psfiv");
    DBUG_PRINT ("fiv", ("found .file, .ident, or .version directive"));
    stoken = &curtoken[1];
    if (numopnds != 1) {
	as_error (ERR_YYERROR, "expecting exactly one string operand");
	(void) fprintf (stderr, "\t\t... not %d\n", numopnds);
    } else if (stoken -> type != OPNDSTR) {
	as_error (ERR_YYERROR, "expecting a string");
	(void) fprintf (stderr, "\t\t... not \"%s\"\n", stoken -> ptr);
    } else {
	DBUG_PRINT ("stg", ("strip quotes from '%s'", stoken -> ptr));
	(stoken -> ptr)++;
	(stoken -> ptr)[strlen (stoken -> ptr) - 1] = '\000';
	DBUG_PRINT ("stg", ("string is now '%s'", stoken -> ptr));
	switch (instrptr -> index) {
	    case PSIDENT:
	        DBUG_PRINT ("ident", ("process a .ident directive"));
	        comment (stoken -> ptr);
		break;
	    case PSVERS:
		DBUG_PRINT ("vers", ("process a .version directive"));
		if (strcmp (stoken -> ptr, as_version) > 0) {
		    as_error (ERR_YYERROR, "inappropriate assembler version");
		    (void) fprintf (stderr,
			"\t\tcompiler expected %s or greater, have %s\n",
			stoken -> ptr, as_version);
		}
		break;
	    case PSFILE:
		DBUG_PRINT ("file", ("process a .file directive"));
		if (cfile[0] != '\000') {
		    as_error (ERR_YYERROR, "only 1 '.file' allowed");
		}
		if (strlen (stoken -> ptr) > maxnamlen) {
		    as_error (ERR_YYERROR, "'.file' name > 14 characters");
		} else {
		    (void) strcpy (cfile, stoken -> ptr);
		}
		DBUG_PRINT ("file", ("current file set to '%s'", cfile));
		generate (0, SETFILE, NULLVAL, NULLSYM);
		break;
	    default:
		as_error (ERR_YYERROR, "internal bug detected in psfiv()");
		break;
	}
    }
    DBUG_VOID_RETURN;
}

static void psln ()
{
    DBUG_ENTER ("psln");
    if (numopnds == 1) {
	if (!getexpr (1)) DBUG_VOID_RETURN;
	generate (0, LLINENO, operstak[0].expval, operstak[0].symptr);
	cline = operstak[0].expval;
    } else if (numopnds == 2) {
	if (!getexpr (1)) DBUG_VOID_RETURN;
	opertop++;
	if (!getexpr (2)) DBUG_VOID_RETURN;
	generate (0, LLINENUM, operstak[0].expval, operstak[0].symptr);
	generate (0, LLINEVAL, operstak[1].expval, operstak[1].symptr);
	cline = operstak[0].expval;
    } else {
	as_error (ERR_YYERROR, "expecting 1 or 2 operands");
	(void) fprintf (stderr, "\t\t... not %d\n", numopnds);
    }
    DBUG_VOID_RETURN;
}

void pscomm ()
{
    register symbol *symptr;

    DBUG_ENTER ("pscomm");
    DBUG_PRINT ("comm", ("process comm directive"));
    symptr = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
    if (symptr == NULL) {
	as_error (ERR_UDI);
    } else {
	if ((symptr -> styp & TYPE) != UNDEF) {
	    as_error (ERR_YYERROR, "tried to redefine symbol");
	    (void) fprintf (stderr, "\t\t... \"%s\"\n", curtoken[1].ptr);
	} else {
	    symptr -> styp |= (EXTERN | UNDEF);
	    symptr -> value = operstak[1].expval;
	}
    }
    DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    DBUG_VOID_RETURN;
}

void psset ()
{
    register symbol *symptr1;
    register symbol *symptr2 = NULL;
    register long incr;

    DBUG_ENTER ("pscommset");
    DBUG_PRINT ("def", ("process def directive"));
    symptr1 = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
    if (symptr1 == NULL) {
	as_error (ERR_UDI);
    } else {
	if (curtoken[2].type == OPNDID || curtoken[2].type == OPNDIDS) {
	    symptr2 = lookup (curtoken[2].ptr, INSTALL, USRNAME) -> stp;
	    if (symptr2 == NULL) {
		as_error (ERR_UDI);
	    }
	}
	if (symptr1 == dot) {
	    if ((symptr1 -> styp != operstak[0].exptype)) {
		as_error (ERR_YYERROR, "Incompatible types");
	    } else if ((operstak[0].symptr != NULL) &&
		       (symptr1 -> sectnum != operstak[0].symptr -> sectnum)) {
		as_error (ERR_YYERROR, "Incompatible section numbers");
	    } else {
		if ((incr = operstak[0].symptr -> value + operstak[0].expval - symptr1 -> value) < 0) {
		    as_error (ERR_YYERROR, "Cannot decrement '.'");
		} else {
		    fill (incr);
		}
	    }
	} else {
	    if (symptr2 != NULL) {
		symalias (symptr1, symptr2);
	    } else {
		symptr1 -> styp = operstak[1].exptype & TYPE;
		symptr1 -> value = operstak[1].expval;
		DBUG_PRINT ("set2", ("value = %d", symptr1 -> value));
		if (operstak[1].symptr != NULL) {
		    symptr1 -> value += operstak[1].symptr -> value;
		    symptr1 -> maxval = operstak[1].symptr -> maxval;
		    symptr1 -> sectnum = operstak[1].symptr -> sectnum;
		} else {
		    DBUG_PRINT ("set3", ("sectnum = N_ABS"));
		    symptr1 -> sectnum = N_ABS;
		}
	    }
	}
    }
    DBUG_PRINT ("symprt", ("%s", symprt (symptr1)));
    DBUG_VOID_RETURN;
}

static void pstag ()
{
    register symbol *symptr;

    DBUG_ENTER ("pstag");
    if (chekoneid ()) {
	symptr = lookup (curtoken[1].ptr, INSTALL, USRNAME) -> stp;
	generate (0, SETTAG, NULLVAL, symptr);
    }
    DBUG_VOID_RETURN;
}

#if FLOAT

static void psdecint ()
{
    register int i;
		    
    DBUG_ENTER ("psdecint");
    if (strictalign) {
	ckalign (4L);
    }
    for (curopnd = 1; curopnd <= numopnds; curopnd++) {
	if (!getexpr (curopnd)) DBUG_VOID_RETURN;
	if (operstak[0].fasciip == NULL) {
	    as_error (ERR_YYERROR, "expecting decimal constant");
	    DBUG_VOID_RETURN;
	}
	atodi (operstak[0].fasciip, fpval);
	for (i = 0; i <= 2; i++) {
	    generate (32, NOACTION, (long) fpval[i], NULLSYM);
	}
	dot -> value = newdot;				/* synchronize */
	generate (0, NEWSTMT, (long) line, NULLSYM);
    }
    if (strictalign) {
	ckalign (4L);
    }
    DBUG_VOID_RETURN;
}

static void psdecfp ()
{
    DBUG_ENTER ("psdecfp");
    if (strictalign) {
	ckalign (4L);
    }
    for (curopnd = 1; curopnd <= numopnds; curopnd++) {
	if (!getfp (curopnd)) DBUG_VOID_RETURN;
	dot -> value = newdot;
	generate (0, NEWSTMT, (long) line, NULLSYM);
    }
    if (strictalign) {
	ckalign (4L);
    }
    DBUG_VOID_RETURN;
}

static void psdblflt ()
{
    register int width;
    /* spctype distinguishes between byte, half, word, float, double */
    register int spctype;
		    
    DBUG_ENTER ("psdblflt");
    switch (instrptr -> index) {
	case PSDOUBLX: 
	    spctype = 3 * NBPW;
	    if (strictalign) {
		ckalign (4L);
	    }
	    break;
	case PSDOUBLE: 
	    spctype = 2 * NBPW;
	    if (strictalign) {
		ckalign (4L);
	    }
	    break;
	case PSFLOAT: 
	    spctype = NBPW;
	    if (strictalign) {
		ckalign (4L);
	    }
	    break;
    }
    for (curopnd = 1; curopnd <= numopnds; curopnd++) {
	if (!getfp (curopnd)) {
	    DBUG_VOID_RETURN;
	}
	width = spctype;
	if (bitpos + width > spctype) {
	    as_error (ERR_YYERROR, "Expression crosses field boundary");
	}
	switch (spctype) {
	    case 32: /* single fp number */
		if (atofs (operstak[0].fasciip, fpval)) {
		    as_error (ERR_YYERROR, "fp conversion error");
		}
		generate (32, NOACTION, (long) fpval[0], NULLSYM);
		break;
	    case 64: /* double fp number */
		if (atofd (operstak[0].fasciip, fpval)) {
		    as_error (ERR_YYERROR, "fp conversion error");
		}
		generate (32, NOACTION, (long) fpval[0], NULLSYM);
		generate (32, NOACTION, (long) fpval[1], NULLSYM);
		break;
	    case 96: /* double extended fp number */
		if (atofx (operstak[0].fasciip, (OPERAND *)fpval)) {
		    as_error (ERR_YYERROR, "fp conversion error");
		}
		generate (32, NOACTION, (long) fpval[0], NULLSYM);
		generate (32, NOACTION, (long) fpval[1], NULLSYM);
		generate (32, NOACTION, (long) fpval[2], NULLSYM);
		break;
	}
	dot -> value = newdot;
	generate (0, NEWSTMT, (long) line, NULLSYM);
    }
    if (strictalign) {
	ckalign (4L);
    }
    DBUG_VOID_RETURN;
}

#endif	/* FLOAT */

static void psbhw ()
{
    register int width;
    /* spctype distinguishes between byte, half, word, float, double */
    int spctype;

    DBUG_ENTER ("psbhw");
    switch (instrptr -> index) {
	case PSBYTE: 
	    spctype = NBPW / 4;
	    break;
	case PSHALF: 
	    spctype = NBPW / 2;
	    if (strictalign) {
		ckalign (2L);
	    }
	    break;
	case PSWORD: 
	    spctype = NBPW;
	    if (strictalign) {
		ckalign (4L);
	    }
	    break;
    }
    for (curopnd = 1; curopnd <= numopnds; curopnd++) {
	oplexptr = curtoken[curopnd].ptr;
	switch (expr ()) {
	    case ':': 
	        oplexptr++;
	        opertop++;
		if (expr ()) {
		    as_error (ERR_YYERROR, "expression error");
		    (void) fprintf (stderr, "\t\t... in \"%s\"\n",
				    curtoken[curopnd].ptr);
		    DBUG_VOID_RETURN;
		}
		if (operstak[0].symptr != NULL ||
		    operstak[0].expval < 0) {
			as_error (ERR_YYERROR, "bad field width specifier");
		    }
		width = operstak[0].expval;
		if (bitpos + width > spctype) {
		    as_error (ERR_YYERROR, "expression crosses field boundary");
		}
		action = (width == NBPW) ? VRT32 : RESABS;
#ifdef EXPR
	        if ((action == VRT32) && (operstak[1].symptr != NULLSYM)) {
		    genexpr (operstak[1].expval, operstak[1].symptr);
		}
#endif
	        if (operstak[1].symptr != NULLSYM) {
		    generate (width, action, operstak[1].expval, operstak[1].symptr);
		} else {
		    generate (width, NOACTION, operstak[1].expval, operstak[1].symptr);
		}
		opertop = operstak;
		break;
	    case '\000': 
		width = spctype;
	        DBUG_PRINT ("psbhw", ("width = %d", width));
		if (bitpos + width > spctype) {
		    as_error (ERR_YYERROR, "expression crosses field boundary");
		}
		/* Figure out which action routine to use */
		/* in case there is an unresolved symbol. */
		/* If a full word is being used, then     */
		/* a relocatable may be specified.        */
		/* Otherwise it is restricted to being an */
		/* absolute (forward reference).          */
		action = (width == NBPW) ? VRT32 : RESABS;
	        DBUG_PRINT ("psbhw", ("action = %d", action));
#ifdef EXPR
	        if ((action == VRT32) && (operstak[0].symptr != NULLSYM)) {
		    DBUG_PRINT ("psbhw", ("%s", symprt (operstak[0].symptr)));
		    genexpr (operstak[0].expval, operstak[0].symptr);
		}
#endif
	        if (operstak[0].symptr != NULLSYM) {
		    generate (width, action, operstak[0].expval, operstak[0].symptr);
		} else {
		    generate (width, NOACTION, operstak[0].expval, operstak[0].symptr);
		}
		break;
	    default: 
		as_error (ERR_YYERROR, "expression error");
		DBUG_VOID_RETURN;
	}
	dot -> value = newdot;
	generate (0, NEWSTMT, (long) line, NULLSYM);
    }
    switch (instrptr -> index) {
	case PSBYTE: 
	    fillbyte ();
	    break;
	case PSHALF: 
	    fillbyte ();
	    if (strictalign) {
		ckalign (2L);
	    }
	    break;
	case PSWORD: 
	    fillbyte ();
	    if (strictalign) {
		ckalign (4L);
	    }
	break;
    }
    DBUG_VOID_RETURN;
}

/* process operands & check their types */

static void procoperands ()
{
    register unsigned long *opndtype = instrptr -> opndtypes;
    extern int operand ();
    
    DBUG_ENTER ("procoperands");
    DBUG_PRINT ("opnd", ("scan through %d operands", numopnds));
    for (curopnd = 1; curopnd <= numopnds; curopnd++) {
	DBUG_PRINT ("opnd2", ("examine operand %d", curopnd));
	oplexptr = curtoken[curopnd].ptr;
	if (operand (curtoken[curopnd].type == OPNDIDS)) {
	    /* operand error */
	    as_error (ERR_OPDERR, curopnd, curtoken[curopnd].ptr, curtoken -> ptr);
	    DBUG_VOID_RETURN;
	}
	DBUG_PRINT ("opnd", ("%s", opndprt (opertop)));
	DBUG_PRINT ("optype", ("possible type bits are %#x", *opndtype));
	if (*opndtype == 0) {
	    as_error (ERR_OPDCNT, curtoken -> ptr, curopnd - 1, numopnds);
	    DBUG_VOID_RETURN;
	}
	DBUG_PRINT ("optype", ("actual type %#x", 1 << (opertop -> type)));
	if (!(*opndtype & (1 << opertop -> type))) {
	    /* Operand type mismatch */
	    as_error (ERR_OPDTYP, curtoken[curopnd].ptr, curtoken -> ptr);
	    DBUG_VOID_RETURN;
	}
	opertop++;
	opndtype++;
    }
    if (*opndtype != 0) {
	/* Too few operands */
	as_error (ERR_OPDCNT, curtoken -> ptr, curopnd, numopnds);
    }
    DBUG_VOID_RETURN;
}

/*
 *	Check the values of constant expressions to see if they
 *	will fit in the field allocated in the object code.  The
 *	first arg is the value to check, the second arg is a mask
 *	with the appropriate bits set (field of lower 16 bits is
 *	0xFFFF for example).  The third arg is a flag which is nonzero
 *	if the field is signed by default.  The forth arg is a flag which
 *	controls whether or not an error message is automatically
 *	generated if an overflow is detected.
 *
 *	The basic algorithm is as follows:
 *
 *		(A) If value is expected to be unsigned then
 *			(A1) If any bits above the mask are set then overflow.
 *		(B) Else value is expected to be signed so
 *			(B1) Check if value should be negative
 *				(B11) If too large negative, error.
 *			(B2) Else value should be positive
 *				(B21) If too large positive, error.
 *
 *	Returns 0 if the field fits, 1 if an overflow occurs.
 *
 */

static int expovf (value, mask, sflag, errflag)
long value;
unsigned long mask;
int sflag;
int errflag;
{
    register long upperbits;
    register long maskmsb;
    auto int result = 0;

    DBUG_ENTER ("expovf");
    DBUG_PRINT ("evalck", ("value = %#lx mask = %#lx", value, mask));
    DBUG_PRINT ("evalck", ("sflag = %d", sflag));
    upperbits = value & ~mask;
    DBUG_PRINT ("evalck", ("upperbits = %#lx", upperbits));
    maskmsb = mask ^ (mask >> 1);
    DBUG_PRINT ("evalck", ("maskmsb = %#lx", maskmsb));
    if (!sflag) {
	if (upperbits != 0) {
	    DBUG_PRINT ("evalck", ("not signed, definitely overflowed"));
	    if (errflag) {
		as_error (ERR_LITOVF, value, 0L, mask);
	    }
	    result = 1;
	}
    } else {
	if ((value & maskmsb) != 0) {
	    DBUG_PRINT ("evalck", ("value better be negative"));
	    if (~(upperbits | mask) != 0) {
		DBUG_PRINT ("evalck", ("found an upper bit NOT set"));
		if (errflag) {
		    as_error (ERR_LITOVF, value, ~(mask >> 1), mask >> 1);
		}
		result = 1;
	    }
	} else {
	    DBUG_PRINT ("evalck", ("value better be positive"));
	    if (upperbits != 0) {
		DBUG_PRINT ("evalck", ("found an upper bit set"));
		if (errflag) {
		    as_error (ERR_LITOVF, value, ~(mask >> 1), mask >> 1);
		}
		result = 1;
	    }
	}
    }
    DBUG_RETURN (result);
}
#ifdef EXPR
saveexpr (exp, nbytes)
char *exp;
int nbytes;
{
    extern FILE *exprfp;

    DBUG_ENTER ("saveexpr");
    (void) fwrite ((char *) &nbytes, sizeof (int), 1, exprfp);
    (void) fwrite ((char *) &(dot -> sectnum), sizeof (short), 1, exprfp);
    (void) fwrite (exp, nbytes, 1, exprfp);
    DBUG_VOID_RETURN;
}
#endif

#ifdef EXPR

/*
 *	Quick and dirty version, lots of stuff hardcoded in...
 */
static void genexpr (expval, symptr)
long expval;
symbol *symptr;
{
    int operators = 0;
    int operands = 0;
    auto char expbuf[64];
    char *expbufp;

    DBUG_ENTER ("genexpr");
    DBUG_PRINT ("gexp", ("expval = %#lx", expval));
    expbufp = expbuf;
    if (symptr == NULL) {
	if (expval == 0) {
	    DBUG_PRINT ("gxp", ("no work to do"));
	} else {
	    DBUG_PRINT ("gxp", ("found only constant expression"));
	    DBUG_PRINT ("gxp", ("uses value %#lx", expval));
	    *expbufp++ = 1;
	    *expbufp++ = 1;
	    *expbufp++ = 0;
	    *expbufp++ = L_PUSHVAL;
	    *expbufp++ = (expval >> 24) & 0xFF;
	    *expbufp++ = (expval >> 16) & 0xFF;
	    *expbufp++ = (expval >>  8) & 0xFF;
	    *expbufp++ = (expval >>  0) & 0xFF;
	    saveexpr (expbuf, expbufp - expbuf);
	}
    } else {
	DBUG_PRINT ("gexp", ("%s", symprt (symptr)));
	if (expval == 0) {
	    DBUG_PRINT ("gxp", ("found only relocatable expression"));
	    *expbufp++ = 1;
	    *expbufp++ = 0;
	    *expbufp++ = 1;
	    *expbufp++ = L_PUSHSYM;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    saveexpr (expbuf, expbufp - expbuf);
	} else {
	    DBUG_PRINT ("gxp", ("found relocatable + constant expression"));
	    DBUG_PRINT ("gxp", ("uses value %#lx", expval));
	    *expbufp++ = 3;
	    *expbufp++ = 1;
	    *expbufp++ = 1;
	    *expbufp++ = L_PUSHSYM;
	    *expbufp++ = L_PUSHVAL;
	    *expbufp++ = L_ADD;
	    *expbufp++ = (expval >> 24) & 0xFF;
	    *expbufp++ = (expval >> 16) & 0xFF;
	    *expbufp++ = (expval >>  8) & 0xFF;
	    *expbufp++ = (expval >>  0) & 0xFF;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    *expbufp++ = 0;
	    saveexpr (expbuf, expbufp - expbuf);
	}
    }
    DBUG_VOID_RETURN;
} 
#endif

/* Apply hi16 or lo16 operations to expval */

static void hilofix (op)
register OPERAND *op;
{
    DBUG_ENTER ("hilofix");
    DBUG_PRINT ("hilo", ("op -> expval = %#l", op -> expval));
    if ((op -> opflags) & HI16) {
	op -> expval >>= 16;
	op -> expval &= 0xFFFF;
    } else if ((op -> opflags) & LO16) {
	op -> expval &= 0xFFFF;
    }
    DBUG_PRINT ("hilo", ("op -> expval = %#l", op -> expval));
    DBUG_VOID_RETURN;
}


/* semantic switch */
static void semswitch ()
{
    register BYTE nbits = (BYTE) (instrptr -> nbits);
    register long obj = instrptr -> opcode;
    register OPERAND *op1 = &operstak[0];
    register OPERAND *op2 = &operstak[1];
    register OPERAND *op3 = &operstak[2];
    register unsigned short action;
    register long tval;

    DBUG_ENTER ("semswitch");
    switch (instrptr -> index) {
#include "sem.dcl"
#include "sem.out"
    }
    DBUG_VOID_RETURN;
}

static void finishline ()
{
    DBUG_ENTER ("finishline");
    /* end of statement semantics */
    generate (0, NEWSTMT, (long) line, NULLSYM);
    dot -> value = newdot;
    curtoken += numopnds + 1;
    DBUG_VOID_RETURN;
}

static int chekid (n)
int n;
{
    register int rtnval = 1;

    DBUG_ENTER ("chekid");
    if (curtoken[n].type != OPNDID && curtoken[n].type != OPNDIDS) {
	as_error (ERR_YYERROR, "expecting identifier");
	(void) fprintf (stderr, "\t\t... not \"%s\"\n", curtoken[n].ptr);
	rtnval = 0;
    }
    DBUG_RETURN (rtnval);
}

static int getexpr (n)
int n;
{
    register int rtnval = 1;

    DBUG_ENTER ("getexpr");
    oplexptr = curtoken[n].ptr;
    switch (expr ()) {
	case 0: 
	    break;
	default:
	    as_error (ERR_YYERROR, "expression syntax error");
	    (void) fprintf (stderr, "\t\t... in \"%s\"\n", curtoken[n].ptr);
	case -1:
	    rtnval = 0;
	    break;
    }
    DBUG_RETURN (rtnval);
}

static int getfp (n)
int n;
{
    register int rtnval = 1;

    DBUG_ENTER ("getfp");
    oplexptr = curtoken[n].ptr;
    switch (fp_expr ()) {
	case 0:
	    break;
	default:
	    as_error (ERR_YYERROR, "floating point syntax error");
	    (void) fprintf (stderr, "\t\t... in \"%s\"\n", curtoken[n].ptr);
	case -1:
	    rtnval = 0;
    }
    DBUG_RETURN (rtnval);
}

static int chekoneid ()
{
    register int rtnval = 1;

    DBUG_ENTER ("chekoneid");
    if (numopnds != 1) {
	as_error (ERR_YYERROR, "expecting 1 operand");
	(void) fprintf (stderr, "\t\t... not %d\n", numopnds);
	rtnval = 0;
    } else if (!chekid (1)) {
	rtnval = 0;
    }
    DBUG_RETURN (rtnval);
}

static void proclabel ()
{
    register symbol *symptr;
    
    DBUG_ENTER ("proclabel");
    DBUG_PRINT ("lab", ("label '%s'", curtoken -> ptr));
    if (dot -> styp == UNDEF) {
	as_error (ERR_LBLOS, curtoken -> ptr);
    }
    symptr = lookup (curtoken -> ptr, INSTALL, USRNAME) -> stp;
    if ((symptr -> styp & TYPE) != UNDEF) {
	/* Multiply defined label */
	as_error (ERR_MDLBL, curtoken -> ptr);
    }
    symptr -> value = newdot;
    symptr -> styp |= dot -> styp;
    symptr -> sectnum = dot -> sectnum;
    if ((dot -> styp == TXT)) {
	deflab (symptr);
    }
    curtoken++;
    DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    DBUG_VOID_RETURN;
}

/*
 *	Process a statement, starting at the mnemonic token.
 */

static int procmnemonic ()
{
    register int rtnval;
    register symbol *symptr;

    DBUG_ENTER ("procmnemonic");
    numopnds = curtoken -> numopnds;
    DBUG_PRINT ("mnem", ("process mnemonic '%s'", curtoken -> ptr));
    if ((instrptr = find_mnemonic (curtoken -> ptr)) == NULL) {
	/* Invalid instruction name */
	as_error (ERR_INVINS, curtoken -> ptr);
	finishline ();
	rtnval = 0;
    } else {
	if (firstinstr) {
	    firstinstr = 0;
	    /* 
	     * set default UNIX section
	     */
	    symptr = lookup (_TEXT, INSTALL, USRNAME) -> stp;
	    cgsect (mksect (symptr, STYP_TEXT));
	}
	DBUG_PRINT ("opnd", ("processing operands"));
	opertop = operstak;
	/* switch for statements having parsing intermingled with semantics */
	switch (instrptr -> index) {
	    case PSSECTION: 		/* .section <id>[,<string>] */
	        pssection ();
		break;
	    case PSBSS:			/* .bss <id>,<expr>,<expr> */
		psbss ();
		break;
	    case PSDEF:			/* .def <id>   */
		psdef ();
		break;
	    case PSDIM:			/* .dim <expr>[,<expr>]*   */
		psdim ();
		break;
	    case PSSTATIC:		/* static <id> */
		psstatic ();
		break;
	    case PSGLOBAL: 		/* global <id>   */
		psglobal ();
		break;
	    case PSFILE: 		/* .file <string>   */
	    case PSIDENT: 		/* .ident <string>   */
	    case PSVERS: 		/* .version <string> */
		psfiv ();
		break;
	    case PSLN: 			/*   .ln <expr>[,<expr>]   */
		psln ();
		break;
	    case PSTAG: 		/*   .tag <id>   */
		pstag ();
		break;
#if FLOAT
	    case PSDECINT: 
		psdecint ();
		break;
	    case PSDECFP: 
		psdecfp ();
		break;
	    case PSDOUBLX: 
	    case PSDOUBLE: 
	    case PSFLOAT: 
		psdblflt ();
		break;
#endif
	    case PSBYTE:  /*   byte [<num>:]<expr>[,[<num>:]<expr>]*    */
	    case PSHALF:  /*   half [<num>:]<expr>[,[<num>:]<expr>]*    */
	    case PSWORD:  /*   word [<num>:]<expr>[,[<num>:]<expr>]*    */
		psbhw ();
		break;
	    case PSSTRING:		/* .string <string> */
		psstring ();
		break;
	    default:
		procoperands ();
		semswitch ();
	}
	finishline ();
	rtnval = 1;
    }
    DBUG_RETURN (rtnval);
}

/*
 *	Process a '.string' directive.
 *
 *	The function sgetc() is called to return the next byte (any
 *	valid 8 bit byte) from the string.  It handles all the normal
 *	C character quoting, and returns EOF when passed the terminating
 *	null character.
 */

static void psstring ()
{
    register struct token *stoken;
    register int nextch;
    auto char *scan;

    DBUG_ENTER ("psstring");
    DBUG_PRINT ("psst", ("found .string directive"));
    stoken = &curtoken[1];
    if (numopnds != 1) {
	as_error (ERR_YYERROR, "expecting exactly one string operand");
	(void) fprintf (stderr, "\t\t... not %d\n", numopnds);
    } else if (stoken -> type != OPNDSTR) {
	as_error (ERR_YYERROR, "expecting a string");
	(void) fprintf (stderr, "\t\t... not \"%s\"\n", stoken -> ptr);
    } else {
	DBUG_PRINT ("stg", ("strip quotes from '%s'", stoken -> ptr));
	(stoken -> ptr)++;
	(stoken -> ptr)[strlen (stoken -> ptr) - 1] = '\000';
	DBUG_PRINT ("stg", ("string is now '%s'", stoken -> ptr));
	scan = stoken -> ptr;
	while ((nextch = sgetc (&scan)) != EOF) {
	    generate (BITSPBY, 0, (long) nextch, NULLSYM);
	}
    }
    DBUG_VOID_RETURN;
}

/*
 *	This function returns the next character from a string,
 *	interpreting quoted characters, and updating a pointer in
 *	the caller for use the next time.
 *
 *	Note that the end of the string is signaled by a return
 *	value of EOF.
 */

static int sgetc (ppc)
char **ppc;
{
    register int intval;
    register int ch;
    register int i;

    switch (ch = *(*ppc)++) {
	default:
	    return (ch);
	case '\000':
	    return (EOF);
	case '\\':
	    ch = *(*ppc)++;
	    switch (ch) {
		case 'b':  return ('\b');
		case 'f':  return ('\f');
		case 'n':  return ('\n');
		case 'r':  return ('\r');
		case 't':  return ('\t');
	    }
	    if (!OCTCHAR(ch)) {
		return (ch);
	    } else {
		i = 0;
		intval = 0;
		while ((i < 3) && (OCTCHAR(ch))) {
		    i++;
		    intval <<= 3;
		    intval += ch - '0';
		    ch = *(*ppc)++;
		}
		--(*ppc);
		return (intval);
	    }
    }
}

#ifndef NO_DBUG
/*
 *	Support routines for internal debugging package.
 */

static char dbuff[256];	/* Reusable data area */

/*
 *	Opndprt builds a human readable string representing
 *	information about an operand and returns a pointer
 *	to a static data area containing the string.
 *
 *	Note that this function uses a possibly non-portable feature of
 *	sprintf whereby sprintf can be used to append to an existing string.
 *
 */

static char *opndprt (opnd)
OPERAND *opnd;
{
    extern symbol symtab[];

    dbuff[0] = '\000';
    if (opnd == NULL) {
	strcpy (dbuff, "<NULL OPERAND>");
    } else {
	switch (opnd -> type) {
	    case REGMD:
		sprintf (dbuff, "%s REGMD", dbuff);
		break;
	    case REGMDS:
		sprintf (dbuff, "%s REGMDS", dbuff);
		break;
	    case ABSMD:
		sprintf (dbuff, "%s ABSMD", dbuff);
		break;
	    case EXADMD:
		sprintf (dbuff, "%s EXADMD", dbuff);
		break;
	    default:
		sprintf (dbuff, "%s <%u>", dbuff, opnd -> type);
		break;
	}
	switch (opnd -> exptype) {
	    case UNDEF:
		sprintf (dbuff, "%s UNDEF", dbuff);
		break;
	    case ABS:
		sprintf (dbuff, "%s ABS", dbuff);
		break;
	    case TXT:
		sprintf (dbuff, "%s TXT", dbuff);
		break;
	    case DAT:
		sprintf (dbuff, "%s DAT", dbuff);
		break;
	    case BSS:
		sprintf (dbuff, "%s BSS", dbuff);
		break;
	    default:
		sprintf (dbuff, "%s <%u>", dbuff, opnd -> exptype);
		break;
	}
	if (opnd -> symptr != NULL) {
	    sprintf (dbuff, "%s symtab[%u]", dbuff, opnd -> symptr - symtab);
	} else {
	    sprintf (dbuff, "%s <NULL>", dbuff);
	}
	sprintf (dbuff, "%s value=%#lx", dbuff, opnd -> expval);
	sprintf (dbuff, "%s value2=%#lx", dbuff, opnd -> fasciip);
    }
    return (dbuff);
}
#endif

static void fakeset (symname, symval)
char *symname;
long symval;
{
    register symbol *symptr;

    DBUG_ENTER ("fakeset");
    symptr = lookup (symname, INSTALL, USRNAME) -> stp;
    if (symptr == NULL) {
	as_error (ERR_UDI);
    }
    /* reset all but EXTERN bit */
    symptr -> styp &= EXTERN;
    symptr -> styp = ABS;
    symptr -> value = symval;
    symptr -> sectnum = N_ABS;
    DBUG_PRINT ("symprt", ("%s", symprt (symptr)));
    DBUG_VOID_RETURN;
}

void addpredefs ()
{
    struct predefset *pdef;

    DBUG_ENTER ("addpredefs");
    DBUG_PRINT ("defs", ("add predefined names to symbol table"));
    for (pdef = defsets; pdef -> name != NULL; pdef++) {
	fakeset (pdef -> name, pdef -> value);
    }
    DBUG_VOID_RETURN;
}
