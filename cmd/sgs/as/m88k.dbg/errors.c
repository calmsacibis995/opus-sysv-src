/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/errors.c	1.8" */

/*
 *
 *	"errors.c" is a file containing a routine "aerror" that prints
 *	out  error messages and (currently) terminates execution when
 *	an error is encountered.  It prints out  the  file  and  line
 *	number  where  the  error  was  encountered and uses a switch
 *	statement to select a diagnostic message based on  the  error
 *	code.   If  this  error  was  reported by anything other than
 *	"makeinst" (indicated by "pass" being non-zero), the  command
 *	"rm" is "exec"ed to remove all temporary files.   The following
 *	global variables are used in this file:
 *
 *	line	Gives the line  number  in  the  current  file  being
 *		assembled.
 *
 *	filenames  This is an array of pointers to character strings
 *		that contains pointers to all of the file names
 *		obtained from the argument list. This array is used
 *		to obtain the names of the temporary files to be
 *		removed.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include "systems.h"
#include "gendefs.h"
#include "dbug.h"		/* Special macro based C debugging package */

unsigned short cline = 0;	/* `c' line number (from `.ln') */

char *cfile;			/* name of `c' source file */

char *filenames[NFILES];
#ifdef EXPR
char *exprfilename;
FILE *exprfp; 
#endif

short rflag = NO;	/* if set, remove (unlink) input when through */

extern void exit ();

void deltemps ()
{
    register short i;

    if (rflag) {
	(void) unlink (filenames[0]);
    }
    for (i = 2; i < NFILES; ++i) {
	(void) unlink (filenames[i]);
    }
#ifdef EXPR
    (void) unlink (exprfilename); 
#endif
}

void delexit ()
{
    (void) unlink (filenames[1]);	/* unlink object (.o) file */
    deltemps ();
    exit (127);
}

void onintr ()
{
    (void) signal (SIGINT, (int (*)()) onintr);
    (void) signal (SIGTERM, (int (*)()) onintr);
    delexit ();
}
