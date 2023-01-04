/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/newerrors.c	1.8" */

#include <stdio.h>
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

#define MAXERRS	(30)


/*
 *	Code from here down implements a new error message strategy,
 *	I.E. make them consistent and give a message more meaningful
 *	than "Nonexistent file" (what it used to print if it couldn't
 *	read the input file for any reason.
 *
 *  DESCRIPTION
 *
 *	Each possible error is assigned a number (actually a preprocessor
 *	symbol) which is recognized by this module.  The appropriate
 *	error message is printed.
 *
 *	It is also handy to have all the error strings in one place
 *	so that format modifications can be made easily.
 *
 *  BUGS
 *
 *	Perhaps it would be better to differentiate more between
 *	errors and warnings via two separate mechanisms, such as
 *	"as_error" and "as_warning".  This would simplify the
 *	code somewhat since the prefix "error - " or "warning -"
 *	could be issued at the entry point, along with incrementing
 *	the appropriate error or warning count.
 *
 */

unsigned short line = 1;
short anyerrs = 0;

/*
 *	Externals used.
 */

extern char *filenames[];

extern int errno;		/* System error return code */
extern char *sys_errlist[];	/* Array of error messages */
extern int sys_nerr;		/* Size of sys_errlist[] */

extern int sprintf ();		/* Formatted print to buffer */
extern int fflush ();		/* Invoke the fflush function */
extern int fprintf ();		/* Invoke the fprintf function */

extern void delexit ();

/*
 *	Define arguments so enough bytes get pushed on stack.
 *
 *	There is some black magic involved in the number of
 *	arguments used.  With 6 longs passed on the stack,
 *	this will support a minimum of 3 arguments of any
 *	normal type on all machines currently supported.
 *	(Worst case is 3 doubles on a pdp11).
 *
 *	Note that this is currently non-portable, and the varargs
 *	mechanism should be used instead (except that varargs is a
 *	rather recent invention and not all unices have it yet).
 */

#define ARGLIST ar0,ar1,ar2,ar3,ar4,ar5

/*
 *	Define a structure with information about each known error.
 */

struct err {
    int err_no;			/* The internal error number */
    int err_flags;		/* Flag bits for special processing */
    int *err_cnt;		/* Pointer to count of these error types */
    char *err_msg;		/* Message to print with ARGLIST */
};

/*
 *	Flag bits for err_flags, to control special processing.
 */

#define E_MSG	001		/* A system error message available */
#define E_FILE	002		/* Output source file name */
#define E_LINE	004		/* Output source line number */

static int deadly;	/* Deadly errors, first one is fatal */
static int badnews;	/* Bad errors, fatal when max count is reached */
static int warnings;	/* Warnings only, never fatal */

static struct err errs[] = {
    ERR_CRIF, E_MSG, &deadly, "\"%s\": can't open input file for reading",
    ERR_COOF, E_MSG, &deadly, "\"%s\": can't open output file for writing",
    ERR_COTW, E_MSG, &deadly, "\"%s\": can't open temporary file for writing",
    ERR_COTR, E_MSG, &deadly, "\"%s\": can't open temporary file for reading",
    ERR_TCL, E_MSG, &deadly, "error flushing and closing temporary file",
    ERR_TWR, E_MSG, &deadly, "error writing temporary file",
    ERR_IBSS, E_FILE|E_LINE, &badnews, "attempt to initialize bss",
    ERR_NWSMO, E_FILE|E_LINE, &badnews, "no white space between mnemonic and operand",
    ERR_STSYN, E_FILE|E_LINE, &badnews, "statement syntax error",
    ERR_OPSYN, E_FILE|E_LINE, &badnews, "operand syntax error",
    ERR_BADSTG, E_FILE|E_LINE, &badnews, "bad string",
    ERR_INVINS, E_FILE|E_LINE, &badnews, "invalid instruction name '%s'",
    ERR_OPDERR, E_FILE|E_LINE, &badnews, "operand error, operand %d ('%s') of '%s'",
    ERR_OPDCNT, E_FILE|E_LINE, &badnews, "'%s' takes %d operands, found %d",
    ERR_OPDTYP, E_FILE|E_LINE, &badnews, "operand '%s' type mismatch for '%s'",
    ERR_DIE, E_FILE|E_LINE, &deadly, "too many errors, goodbye!",
    ERR_MDLBL, E_FILE|E_LINE, &badnews, "multiply defined label '%s'",
    ERR_LBLOS, E_FILE|E_LINE, &badnews, "label '%s' defined outside scope of any section",
    ERR_USTE, E_FILE|E_LINE, &deadly, "unbalanced symbol table entries, too many scope beginnings",
    ERR_USTE2, E_FILE|E_LINE, &deadly, "unbalanced symbol table entries, missing scope endings",
    ERR_USTE3, E_FILE|E_LINE, &deadly, "unbalanced symbol table entries, too many scope ends",
    ERR_CCSTF, E_MSG, &deadly, "cannot create section temp file for write",
    ERR_IAR, E_FILE|E_LINE, &deadly, "internal error; invalid action routine",
    ERR_BTFF, E_FILE|E_LINE, &deadly, "internal error; bad temporary file format",
    ERR_RSNST, E_FILE|E_LINE, &deadly, "reference to symbol not in symbol table",
    ERR_USST, E_FILE|E_LINE, &deadly, "unknown symbol in symbol table",
    ERR_STSOVF, E_FILE, &deadly, "symbol table stack overflow",
    ERR_IOFE, E_FILE, &deadly, "internal error; object file error",
    ERR_SYMOVF, E_FILE, &deadly, "symbol table overflow",
    ERR_HASHOVF, E_FILE, &deadly, "hash table overflow",
    ERR_CRSTBL, E_FILE|E_MSG, &deadly, "cannot realloc string table",
    ERR_CMSTBL, E_FILE|E_MSG, &deadly, "cannot malloc string table",
    ERR_URALIAS, E_FILE, &deadly, "%d unresolved aliases via 'def' pseudos",
    ERR_MALLOC, E_FILE|E_MSG, &deadly, "cannot malloc %d bytes",
    ERR_INVTYP, E_FILE|E_LINE, &deadly, "invalid type",
    ERR_OVF16, E_FILE|E_LINE, &deadly, "absolute value %d too large for 16 bits",
    ERR_RSIAS, E_FILE|E_LINE, &deadly, "reference to symbol in another section",
    ERR_PCOVF8, E_FILE|E_LINE, &deadly, "pc relative reference %d too large for 8 bits",
    ERR_PCOVF16, E_FILE|E_LINE, &deadly, "pc relative reference %d too large for 16 bits",
    ERR_PCOVF26, E_FILE|E_LINE, &deadly, "pc relative reference %d too large for 26 bits",
    ERR_ABSBSS, E_FILE|E_LINE, &deadly, "bss pseudo expression value not absolute",
    ERR_UDI, E_FILE|E_LINE, &deadly, "unable to define identifier",
    ERR_FPBD, E_FILE|E_LINE, &badnews, "file pseudo must precede all def's",
    ERR_TMADIM, E_FILE|E_LINE, &badnews, "too many array dimensions for symbolic debug",
    ERR_UNKSOPT, E_FILE|E_LINE, &badnews, "unknown suboption for %s",
    ERR_CRSYMTBL, E_FILE, &badnews, "cannot reduce symbol table, unused symbols remain",
    ERR_TBLOVFOP, E_FILE, &badnews, "table overflow, some optimizations lost (%s)",
    ERR_UNKOPT, 0, &badnews, "unknown option (%s) ignored",
    ERR_YYERROR, E_FILE|E_LINE, &badnews, "%s",
    ERR_LITOVF, E_FILE|E_LINE, &badnews, "literal constant %ld overflows field (range [%ld...%ld])",
    ERR_BUG, 0, &badnews, "internal error -- (%s)",
    ERR_OBSOLETE, E_FILE|E_LINE, &warnings, "code behavior depends on obsolescent assembler feature",
    0, 0, 0, NULL
};


/*
 *  FUNCTION
 *
 *	as_error    print appropriate message for given error
 *
 *  SYNOPSIS
 *
 *	void as_error (errorno, ARGLIST)
 *	int errorno;
 *	long ARGLIST;
 *
 *  DESCRIPTION
 *
 *	Prints an error message, for the error specified by "errorno".
 *	Note that values for "errorno" are given in "errors.h".
 *
 *	Note that sys_errlist[] and sys_nerr are supplied by the
 *	runtime environment in standard system V UNIX.  Other
 *	implementations may require this module to be customized.
 *
 *	(4.2 BSD supplies sys_errlist[] and sys_nerr also.)
 *
 */

/*VARARGS1*/
void as_error (errorno, ARGLIST)
int errorno;
long ARGLIST;
{
    register struct err *ep;
    
    DBUG_ENTER ("as_error");
    (void) fflush (stderr);
    (void) fprintf (stderr, "%s: ", "as");
    for (ep = errs; ep -> err_no != 0 && ep -> err_no != errorno; ep++) {;}
    if (ep -> err_no != errorno) {
	(void) fprintf (stderr, "warning - unknown internal error %d!", errorno);
    } else {
	(*(ep -> err_cnt))++;
	if (ep -> err_flags & E_FILE) {
	    (void) fprintf (stderr, "\"%s\": ", filenames[0]);
	}
	if (ep -> err_flags & E_LINE) {
	    (void) fprintf (stderr, "line %u: ", line);
	}
	(void) fprintf (stderr, ep -> err_msg, ARGLIST);
	if (ep -> err_flags & E_MSG) {
	    if (errno > sys_nerr) {
		(void) fprintf (stderr, ": !! bad errno %d !!", errno);
	    } else {
		(void) fprintf (stderr, ": %s", sys_errlist[errno]);
	    }
	}
    }
    (void) fprintf (stderr, "\n");
    (void) fflush (stderr);
    if (deadly) {
	delexit ();
    } else if (badnews > MAXERRS) {
	as_error (ERR_DIE);		/* Call recursive to die (deadly) */
    }
    DBUG_VOID_RETURN;
}
