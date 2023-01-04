#ident "@(#) $Header: usage.c 4.1 88/04/26 17:02:25 root Exp $ RISC Source Base"
/*
 *  FILE
 *
 *	usage.c    internal help stuff
 *
 *  DESCRIPTION
 *
 *	Contains routines for printing internal help or usage
 *	information.
 *
 */

#include <stdio.h>

#include "coffcmp.h"

extern VOID vecprintf ();		/* Print an array of strings */


/*
 *	The following is the internal documentation which gets printed
 *	when invoked with the -h flag.
 */

static char *documentation[] = {
    "",
    "NAME",
    "",
    "\tcoffcmp  ---  compare common object format modules",
    "",
    "DESCRIPTION",
    "",
    "\tcoffcmp is a utility for comparing common object module",
    "\tformat files.  It is intended primarily as an aid for",
    "\tthe compiler/assembler/loader maintenance person.",
    "",
    "SYNOPSIS",
    "",
    "\tcoffcmp [options] file1 file2",
    "",
    "OPTIONS",
    "",
    "\t-h         print this help info",
    "\t-s         silent mode, compare and return code only",
    "\t-# str     use debugging control string str",
    "",
    NULL
};

static char *quickdoc[] = {
    "usage:  coffcmp [-sh] [-# str] file1 file2",
    "coffcmp -h for help",
    NULL
};


/*
 *  FUNCTION
 *
 *	usage    give usage information
 *
 *  SYNOPSIS
 *
 *	int usage (type)
 *	int type;
 *
 *  DESCRIPTION
 *
 *	Gives specified type of usage information.  Type is
 *	usually either "BRIEF" or "VERBOSE".
 *
 */

int usage (type)
int type;
{
    register char **dp;
    register FILE *fp;

    DBUG_ENTER ("usage");
    if (type == VERBOSE) {
	fp = stdout;
	dp = documentation;
    } else {
	fp = stderr;
	dp = quickdoc;
    }
    vecprintf (fp, dp);
    fflush (fp);
    DBUG_RETURN (ERROR_EXIT);
}

