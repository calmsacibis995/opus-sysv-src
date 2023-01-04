#ident "@(#) $Header: usage.c 4.1 88/04/26 17:03:15 root Exp $ RISC Source Base"
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

#include "coffdump.h"

extern VOID vecprintf ();		/* Print an array of strings */


/*
 *	The following is the internal documentation which gets printed
 *	when invoked with the -H flag.
 */

static char *documentation[] = {
    "",
    "NAME",
    "",
    "\tcoffdump  ---  dump common object format modules",
    "",
    "DESCRIPTION",
    "",
    "\tcoffdump is a utility for dumping common object module",
    "\tformat files in a human readable form.  It is intended",
    "\tprimarily as an aid for the compiler/assembler/loader",
    "\tmaintenance person.",
    "",
    "SYNOPSIS",
    "",
    "\tcoffdump [options] file(s)",
    "",
    "OPTIONS",
    "",
    "\t-# str  use debugging control string str",
    "\t-D      output will be suitable for diff",
    "\t-H      print this help info",
    "\t-a      equivalent to -cfhlorst",
    "\t-c      dump the string table",
    "\t-f      dump each file header",
    "\t-h      dump section headers",
    "\t-l      dump line number information",
    "\t-o      dump each optional header",
    "\t-r      dump relocation information",
    "\t-s      dump section contents",
    "\t-t      dump symbol table entries",
    "",
    "\t        default options are -fho",
    "",
    NULL
};

static char *quickdoc[] = {
    "usage:  coffdump [-DHacfhlorst] [-# str] file(s)...",
    "coffdump -H for help",
    NULL
};


/*
 *  FUNCTION
 *
 *	usage    give usage information
 *
 *  SYNOPSIS
 *
 *	VOID usage (type)
 *	int type;
 *
 *  DESCRIPTION
 *
 *	Gives specified type of usage information.  Type is
 *	usually either "BRIEF" or "VERBOSE".
 *
 */

VOID usage (type)
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
    DBUG_VOID_RETURN;
}

