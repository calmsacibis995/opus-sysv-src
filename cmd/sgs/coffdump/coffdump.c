#ident "@(#) $Header: coffdump.c 4.1 88/04/26 17:02:36 root Exp $ RISC Source Base"
#include <stdio.h>
#include <filehdr.h>
#include <ldfcn.h>

#include "coffdump.h"

static VOID options ();		/* Local routine to process options */
static VOID do_file ();		/* Process single file from cmd line */
extern VOID dump ();		/* Dump contents of single module */
extern BOOLEAN GoodMagic ();	/* Test for good magic number */

struct cmdflags flags;		/* Allocate space for cmd line flags */

/*
 *  FUNCTION
 *
 *	main    entry point for common object module dump utility
 *
 *  SYNOPSIS
 *
 *	main (argc, argv)
 *	INT argc;
 *	CHAR *argv[];
 *
 *  DESCRIPTION
 *
 *	Main entry point for the common object module dump utility.
 *	Called by C start up routines.
 *
 */

main (argc, argv)
INT argc;
CHAR *argv[];
{
    extern INT optind;
    
    DBUG_ENTER ("main");
    DBUG_PROCESS (argv[0]);
    if (argc == 1) {
	usage (BRIEF);
    } else {
	options (argc, argv);
	for ( ; optind < argc; optind++) {
	    do_file (argv[optind]);
	}
    }
    DBUG_VOID_RETURN;
}


/*
 *  FUNCTION
 *
 *	options    process command line options
 *
 *  SYNOPSIS
 *
 *	static VOID options (argc, argv)
 *	INT argc;
 *	CHAR *argv[];
 *
 *  DESCRIPTION
 *
 *	Processes command line options using the standard library
 *	routine "getopt".  There are definite advantages to using
 *	the standard routine for parsing the options, even if
 *	it isn't perfect.
 *
 */

static VOID options (argc, argv)
INT argc;
CHAR *argv[];
{
    register INT c;
    extern CHAR *optarg;
    extern INT getopt ();

    DBUG_ENTER ("options");
    while ((c = getopt (argc, argv, "#:acfhlorstHD")) != EOF) {
	switch (c) {
	    case 'a':
	        flags.cflag = TRUE;
		flags.fflag = TRUE;
		flags.hflag = TRUE;
		flags.lflag = TRUE;
		flags.oflag = TRUE;
		flags.rflag = TRUE;
		flags.sflag = TRUE;
		flags.tflag = TRUE;
		break;
	    case 'c':
	  	flags.cflag = TRUE;
		break;
	    case 'D':
	    	flags.Dflag = TRUE;
		break;
	    case 'f':
	        flags.fflag = TRUE;
		break;
	    case 'h':
	        flags.hflag = TRUE;
		break;
	    case 'l':
	        flags.lflag = TRUE;
		break;
	    case 'o':
	        flags.oflag = TRUE;
		break;
	    case 'r':
	        flags.rflag = TRUE;
		break;
	    case 's':
	        flags.sflag = TRUE;
		break;
	    case 't':
	        flags.tflag = TRUE;
		break;
	    case 'H':
	        usage (VERBOSE);
		break;
	    case '#':
		DBUG_PUSH (optarg);
		break;
	}
    }
    if (optind == 1) {
	flags.fflag = TRUE;
	flags.hflag = TRUE;
	flags.oflag = TRUE;
    }
    DBUG_VOID_RETURN;
}
  

/*
 *  FUNCTION
 *
 *	do_file    process a single file from the command line arg list
 *
 *  SYNOPSIS
 *
 *	static VOID do_file (fname)
 *	CHAR *fname;
 *
 *  DESCRIPTION
 *
 *	Given pointer to the name of a file, opens the file, checks
 *	the magic number to make sure it is a simple object file
 *	or an archive, and then processes the file (or archived
 *	files).
 */

static VOID do_file (fname)
CHAR *fname;
{
    LDFILE *ldptr;
    
    DBUG_ENTER ("do_file");
    if (!flags.Dflag) {
	printf ("==== %s ====\n\n", fname);
    }
    ldptr = NULL;
    do {
	if ((ldptr = ldopen (fname, ldptr)) == NULL) {
	    cd_error (ERR_LDOPEN, fname);
	} else {
	    DBUG_3 ("TYPE", "TYPE %#o", TYPE(ldptr));
	    if (GoodMagic ((USHORT) TYPE (ldptr))) {
		dump (ldptr);
	    }
	}
    } while (ldclose (ldptr) == FAILURE);
    DBUG_VOID_RETURN;
}
