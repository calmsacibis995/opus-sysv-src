/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/pass1.c	1.16" */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <paths.h>
#include <scnhdr.h>
#include "systems.h"
#include "symbols.h"
#include "gendefs.h"
#include "codeout.h"
#include "section.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

/*
 *
 *	"pass1.c" is a file containing the main routine for the first
 *	pass of the assembler.  It is invoked with the command:
 *
 *		as1 [flags] ifile ofile t2 t3 t4 t5 t6 ... t[NSECTIONS+5]
 *
 *	where [flags] are optional flags passed from pass 0,
 *	"ifile" is the name of the assembly language input file,
 *	"t2" ... are the names of temporary files to be used
 *	by the assembler, and "ofile" is the file where the object code
 *	is to be written.  Pass1 uses the files as follows:
 *	
 *	name	internal	mode	use
 *
 *	ifile	filenames[0]	read	input source file
 *	ofile	filenames[1]		not used in this pass
 *	t2	filenames[2]	write*	section table
 *	t3	filenames[3]	write*	flexname string table
 *	t4	filenames[4]	write*	symbol table
 *	t5	filenames[5]	write*	span dependent instruction table
 *	t6	filenames[6]	write	temp for section 1
 *	...
 *	tk	filenames[k]	write	temp for section k-5
 *
 *	'*' after mode indicates files used only for multiple process as
 *
 *	The following things are done by this function:
 *
 *	1. Initialization. This consists of calling signal to catch
 *	   interrupt signals for hang-up, break, and terminate. Then
 *	   the argument list is processed by "getargs" followed by the
 *	   initialization of the symbol table with mnemonics for
 *	   instructions and pseudos-ops.
 *
 *	2. "yyparse" is called to do the actual first pass processing.
 *	   This is followed by a call to "cgsect(1)". Normally this
 *	   function is used to change the section into which code
 *	   is generated. In this case, it is only called to make
 *	   sure that the section program counters are up to date.
 *
 *	3. The section name symbols are then defined. Each section
 *	   name symbol has the type of the section and a value of zero.
 *	   The symbol is used to label the beginning of the section
 *	   and later as a reference for relocation entries that are
 *	   relative to the section.
 *
 *	4. Next the symbol (sdicnt) is defined.  This is the internal
 *	   symbol used to remember the number of span dependent
 *	   instructions on which optimizations were performed.
 *
 *	5. The function "dmpstb" is called to dump the symbol
 *	   table out to a temporary file to be used by pass 2 of
 *	   the assembler.
 *
 *	6. The temporary files are closed and the next pass (if any)
 *	   is called.
 *
 */

extern short passnbr;
extern char *filenames[];
extern short anyerrs;
extern long newdot;
extern symbol *dot;

extern void as_error ();
extern void delexit ();
extern void yyparse ();
extern void perfsetup ();
extern void perfreport ();
extern void fixsyms ();
extern void flushbuf ();
extern void cgsect ();
extern upsymins *lookup ();
extern void addpredefs ();

short opt = YES;
short workaround = YES;
FILE *fdin;


static void trapsigs ()
{
    extern void onintr ();

    DBUG_ENTER ("trapsigs");
    if (signal (SIGHUP, SIG_IGN) == SIG_DFL) {
	(void) signal (SIGHUP, (int (*)()) onintr);
    }
    if (signal (SIGINT, SIG_IGN) == SIG_DFL) {
	(void) signal (SIGINT, (int (*)()) onintr);
    }
    if (signal (SIGTERM, SIG_IGN) == SIG_DFL) {
	(void) signal (SIGTERM, (int (*)()) onintr);
    }
    DBUG_VOID_RETURN;
}

aspass1 () 
{
    DBUG_ENTER ("aspass1");
    DBUG_PRINT ("pass1", ("begin pass 1..."));
    passnbr = 1;
    trapsigs ();
    perfsetup ();
    DBUG_PRINT ("ifile", ("open input file '%s' for real now", filenames[0]));
    if ((fdin = fopen (filenames[0], "r")) == NULL) {
	as_error (ERR_CRIF, filenames[0]);
    }
    DBUG_PRINT ("init", ("initialize symbol table with mnemonics and pseudo-ops"));
    strtabinit ();
    dot = (*lookup (".", INSTALL, USRNAME)).stp;
    dot -> value = newdot = 0L;
    DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
    dot -> sectnum = 0;
    addpredefs ();
    yyparse ();
    DBUG_PRINT ("ifile", ("done with input file, close it"));
    (void) fclose (fdin);
    DBUG_PRINT ("sect", ("make sure section program counters are up to date"));
    cgsect (1);
    flushbuf ();
    DBUG_PRINT ("syms", ("define section name symbols"));
    fixsyms ();
    cgsect (1);
    flushbuf ();
    DBUG_PRINT ("perf", ("print performance report if desired"));
    perfreport ();
    if (!anyerrs) {
	DBUG_RETURN (aspass2 ());
    } else {
	delexit ();
    }
    DBUG_RETURN (0);
}

