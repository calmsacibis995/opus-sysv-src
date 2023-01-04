#ident "@(#) $Header: errors.c 4.1 88/04/26 17:02:20 root Exp $ RISC Source Base"
/*
 *  FILE
 *
 *	errors.c    error handling module for coffcmp utility
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
 */

#include <stdio.h>

#include "coffcmp.h"

/*
 *	Externals used.
 */

extern int errno;			/* System error return code */
extern char *sys_errlist[];		/* Array of error messages */
extern int sys_nerr;			/* Size of sys_errlist[] */

/*
 *	Local function declarations.
 */
 
static VOID prt_err ();


/*
 *  FUNCTION
 *
 *	Error    print appropriate message for given error
 *
 *  SYNOPSIS
 *
 *	Error (errorno, arg1, arg2, arg3, arg4, arg5)
 *	int errorno;
 *	char *arg1, *arg2, *arg3, *arg4, *arg5;
 *
 *  DESCRIPTION
 *
 *	Prints an error message, with up to 5 arguments, for the
 *	error specified by "errorno".  Note that values for
 *	"errorno" are given in "errors.h".
 *
 *	Note that sys_errlist[] and sys_nerr are supplied by the
 *	runtime environment in standard system V UNIX.  Other
 *	implementations may require this module to be customized.
 *
 */

/*VARARGS1*/
Error (errorno, arg1, arg2, arg3)
int errorno;
char *arg1, *arg2, *arg3;
{
    DBUG_ENTER ("Error");
    fflush (stdout);
    fflush (stderr);
    fprintf (stderr, "%s: ", "coffcmp");		/* FIX SOMETIME */
    switch (errorno) {
	case ERR_OPEN:
	    fprintf (stderr, "\"%s\" can't open", arg1);
	    prt_err ();
	    break;
	case ERR_SEEK:
	    fprintf (stderr, "\"%s\" seek error", arg1);
	    break;
	case ERR_DIFF:
	    fprintf (stderr, "\"%s\" \"%s\" different at offset %ld\n",
	    arg1, arg2, (OFFSET)arg3);
	    break;
	default:
	    fprintf (stderr, "internal error; bad error %d\n", errorno);
	    break;
    }
    fflush (stdout);
    fflush (stderr);
    DBUG_VOID_RETURN;
}


/*
 *  FUNCTION
 *
 *	prt_err    substitute for library perror call
 *
 *  SYNOPSIS
 *
 *	VOID prt_err ()
 *
 *  DESCRIPTION
 *
 *	Performs similarly to the library call "perror" except that
 *	there are no arguments and we have control over the output stream.
 *
 */
 
static VOID prt_err ()
{
    if (errno > sys_nerr) {
         fprintf (stderr, ": !! bad errno %d !!\n", errno);
    } else {
	 fprintf (stderr, ": %s\n", sys_errlist[errno]);
    }
    fflush (stderr);
}
