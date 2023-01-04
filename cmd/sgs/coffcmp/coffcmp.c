#ident "@(#) $Header: coffcmp.c 4.1 88/04/26 17:02:09 root Exp $ RISC Source Base"
#include <stdio.h>

#include "coffcmp.h"

static VOID options ();		/* Local routine to process options */
static INT cmp ();		/* Compare two files */
static INT cmpbytes ();		/* Compare regions of files */

BOOLEAN sflag;			/* Silent mode flag, return code only */

/*
 *  FUNCTION
 *
 *	main    entry point for common object module compare utility
 *
 *  SYNOPSIS
 *
 *	main (argc, argv)
 *	INT argc;
 *	CHAR *argv[];
 *
 *  DESCRIPTION
 *
 *	Main entry point for the common object module compare utility.
 *	Called by C start up routines.
 *
 */

main (argc, argv)
INT argc;
CHAR *argv[];
{
    auto struct finfo file1;
    auto struct finfo file2;
    register INT status;
    extern INT optind;
    
    DBUG_ENTER ("main");
    DBUG_PROCESS (argv[0]);
    if (argc == 1) {
	status = usage (BRIEF);
    } else {
	options (argc, argv);
	if (argc - optind != 2) {
	    status = usage (BRIEF);
	} else {
	    file1.fname = argv[optind];
	    file2.fname = argv[optind+1];
	    status = cmp (&file1, &file2);
	}
    }
    DBUG_3 ("status", "exit status is %d", status);
    exit (status);
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
    while ((c = getopt (argc, argv, "#:hs")) != EOF) {
	switch (c) {
	    case 'h':
	        usage (VERBOSE);
		break;
	    case 's':
	        sflag = TRUE;
		break;
	    case '#':
		DBUG_PUSH (optarg);
		break;
	}
    }
    DBUG_VOID_RETURN;
}
  

static INT cmp (file1, file2)
register struct finfo *file1;
register struct finfo *file2;
{
    register INT status;
    auto FILHDR dummy;		/* Need instance of the structure */
    
    DBUG_ENTER ("cmp");
    status = ERROR_EXIT;
    if ((file1 -> fp = fopen (file1 -> fname, "r")) == NULL) {
	Error (ERR_OPEN, file1 -> fname);
    } else if ((file2 -> fp = fopen (file2 -> fname, "r")) == NULL) {
	Error (ERR_OPEN, file2 -> fname);
	fclose (file1 -> fp);
    } else {
	status = cmpbytes (file1, file2, LOW1, HIGH1);
	if (status != ERROR_EXIT) {
	    status = cmpbytes (file1, file2, LOW2, HIGH2);
	}
	fclose (file1 -> fp);
	fclose (file2 -> fp);
    }
    DBUG_RETURN (status);
}


/*
 *	Note that the range is from low (inclusive) to
 *	high (exclusive).  Thus this algorithm will fail to
 *	compare the last byte of a file which is MAX_OFFSET+1
 *	bytes long.
 */
 
static INT cmpbytes (file1, file2, low, high)
register struct finfo *file1;
register struct finfo *file2;
OFFSET low;
OFFSET high;
{
    register INT ch1;
    register INT ch2;
    register INT status;
    register OFFSET current;
    
    DBUG_ENTER ("cmpbytes");
    DBUG_4 ("limits", "compare from %ld to %ld", low, high);
    status = ERROR_EXIT;
    if (fseek (file1 -> fp, low, BEGINNING) != SEEK_OK) {
	Error (ERR_SEEK, file1 -> fname);
    } else if (fseek (file2 -> fp, low, BEGINNING) != SEEK_OK) {
	Error (ERR_SEEK, file2 -> fname);
    } else {
	status = OK_EXIT;
	ch1 = 0;
	ch2 = 0;
	if (high == TO_EOF) {
	    high = MAX_OFFSET;
	}
	DBUG_3 ("scan", "scan start at offset %ld", low);
	for (current = low; current < high; current++) {
	    ch1 = getc (file1 -> fp);
	    ch2 = getc (file2 -> fp);
	    if (ch1 != ch2) {
		status = ERROR_EXIT;
		if (!sflag) {
		    Error (ERR_DIFF, file1 -> fname, file2 -> fname, current);
		}
		break;
	    }
	    if (ch1 == EOF || ch2 == EOF) {
		break;
	    }
	}
	DBUG_3 ("scan", "scan end at offset %ld", --current);
    }
    DBUG_RETURN (status);
}

