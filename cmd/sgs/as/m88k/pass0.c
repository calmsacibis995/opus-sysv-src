/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/pass0.c	1.28" */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <paths.h>
#include "systems.h"
#include "sgs.h"
#include "gendefs.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

/*
 *
 *	"pass0.c" is a file containing the source for the setup program
 *	for the SGS Assemblers.  Routines in this file parse the
 *	arguments, determining the source and object file names, invent
 *	names for temporary files, and execute the first pass of the
 *	assembler.
 *
 *	This program can be invoked with the command:
 *
 *			as [flags] ifile [-o ofile]
 *
 *	where [flags] are optional user-specified flags,
 *	"ifile" is the name of the assembly languge source file
 *	and "ofile" is the name of a file where the object program is
 *	to be written.  
 *
 *	The global array "filenames" is used to store all of the file names:
 *	filenames[0]		ifile
 *	filenames[1]		ofile
 *	filenames[2]		temp file 1
 *	filenames[NFILES - 1]	temp files NFILES-2
 *
 */

#ifndef NO
#    define NO		(0)
#endif

#ifndef YES
#    define YES		(1)
#endif

#define	MAXFLAGS (16 - 9)    /* sizeof(xargp)-num of args required for pass1 */

#ifdef BSD_43			/* If BSD flavor system this is in signal.h */
#include <sys/types.h>		/* This is required for <sys/dir.h> */
#include </dir.h>		/* This has MAXNAMLEN (whew!) */
int maxnamlen = MAXNAMLEN;
#else
int maxnamlen = 14;
#endif

int strictalign = NO;		/* Stricter alignment required */

short passnbr = 0;
extern short opt;
extern short workaround;

extern short keeptmps;

extern short tstlookup;

extern short uflag;
extern short dlflag;

extern char *filenames[];
#ifdef EXPR
extern char *exprfilename;
extern FILE *exprfp;
#endif

extern short rflag;

extern char *tempnam ();

static char prefix[] = "asX";
short transvec = NO;

static short macro = NO;
static char *m4name = M4;

#if !NODEFS
static char *regdef = CM4DEFS;
#if TRANVEC
static char *tvdef = CM4TVDEFS;
#endif
#endif

extern char *strcpy ();
extern char *strcat ();
extern char *strncat ();

#define max(A,B) (((A) < (B)) ? (B) : (A))

extern char *malloc ();
extern char *getenv ();
extern char *mktemp ();
extern int access ();
extern void exit ();
extern void as_error ();

/*
 *	Routines which are used before their definitions are seen, so
 *	must be predeclared.
 */

static char *bldofile ();	/* Build output file name from input name */

/*
 *
 *	"tempnam" is the routine that allocates the temporary files needed
 *	by the assembler.  It searches directories for enough space for the
 *	files, then assigns a (guaranteed) unique filename using the prefix
 *	in the function call.
 *
 */

/*
 *
 *	"getargs" is a general purpose argument parsing routine.
 *	It locates flags (identified by a preceding minus sign ('-'))
 *	and initializes any associated flags for the assembler.
 *	If there are any file names in the argument list, then a
 *	pointer to the name is stored in a global variable for
 *	later use.
 *
 */

void getargs (xargc, xargv)
register int xargc;
register char **xargv;
{
    register char ch;

    DBUG_ENTER ("getargs");
    while (xargc-- > 0) {
	if (**xargv == '-') {
	    while ((ch = *++*xargv) != '\0') {
		switch (ch) {
		    case '#':
		        if (*++*xargv != '\0') {
			    DBUG_PUSH (*xargv);
			} else {
			    --xargc;
			    DBUG_PUSH (*++xargv);
			}
			while (*++*xargv != '\0');
			--*xargv;
			break;
		    case 'A':
			strictalign = YES;
			break;
		    case 'm': 
			DBUG_PRINT ("m4", ("preprocess through m4"));
			macro = YES;
			break;
		    case 'Y': 	/* specify directory for m4 and/or cm4defs */
			{
			    char *dirname;
			    char *keys;

			    keys = dirname = (*++xargv);
			    xargc--;
			    while (*dirname) {
				if (*dirname == ',') {
				    dirname++;
				    break;
				} else {
				    dirname++;
				}
			    }
			    while (*keys != ',' && *keys != '\0') {
				switch (*keys) {
				    case 'm': 
					m4name = malloc ((unsigned) (strlen (dirname) + 4));
					(void) strcpy (m4name, dirname);
					(void) strcat (m4name, "/m4");
					break;
#if !NODEFS
				    case 'd': 
#if TRANVEC
					if (transvec) {
					    regdef = malloc ((unsigned) (strlen (dirname) + 11));
					    (void) strcpy (tvdef, dirname);
					    (void) strcat (tvdef, "/cm4tvdefs");
					    break;
					}
#endif
					regdef = malloc ((unsigned) (strlen (dirname) + 9));
					(void) strcpy (regdef, dirname);
					(void) strcat (regdef, "/cm4defs");
					break;
#endif
				    default: 
					as_error (ERR_UNKSOPT, "-Y");
					break;
				}
				keys++;
			    }
			    *xargv = dirname + strlen (dirname) - 1;
			}
			break;
		    case 'o': 
			filenames[1] = malloc ((unsigned) (strlen (*(++xargv)) + 1));
			(void) strcpy (filenames[1], *(xargv));
			--xargc;
			while (*++*xargv != '\0');
			--*xargv;
			break;
		    case 'd': 
			if (*++*xargv == 'l') {
			    dlflag = YES;
			}
			break;

		    case 'u': 
			uflag = YES;
			break;

#if TRANVEC
		    case 't': 
			++*xargv;
			if (**xargv == 'v') {
			    transvec = YES;
			    break;
			}
#endif
		    case 'V': 
			/* version flag */
			(void) fprintf (stderr, "%sas: assembler - %s\n",
				SGS, RELEASE);
			break;
		    case 'n': 
			switch (*++*xargv) {
			    case 'f': /* -nf option */
				/* no frills; disable work-arounds */
				workaround = NO;
				break;
			    default: /* -n option */
				opt = NO;
				--*xargv;
			}
			break;
		    case 'T':
			switch (*++*xargv) {
			    case 'F':
			        keeptmps = YES;
				break;
			    case 'L':
			        tstlookup = YES;
				break;
			}
			break;
		    case 'R': 
			rflag = YES;
			break;
		    default: 
			flags (ch);
			break;
		}
	    }
	    xargv++;
	} else {
	    filenames[0] = *xargv++;
	}
    }
    DBUG_VOID_RETURN;
}

/*
 *	Stuff which must be done before any serious work.
 */

static void initstuff ()
{
    extern char *cfile;
    extern char *calloc ();

    cfile = calloc (1, maxnamlen + 1);
}

/*
 *
 *	"main" is the main driver for the assembler. It calls "getargs"
 *	to parse the argument list, set flags, and stores pointers
 *	to any file names in the argument list .
 *	If the output file was not specified in the argument list
 *	then the output file name is generated. Next the temporary
 *	file names are generated and the first pass of the assembler
 *	is invoked.
 *
 */

main (argc, argv)
int argc;
char **argv;
{
    register short index;
    register FILE *fd;

    DBUG_ENTER ("main");
    initstuff ();
    argc--;
    if (argc <= 0) {
	(void) fprintf (stderr, "Usage: %sas [options] file\n", SGS);
	exit (1);
    }
    argv++;
    getargs (argc, argv);
    DBUG_PRINT ("ifile", ("check for existance of input file '%s'", filenames[0]));
    if ((fd = fopen (filenames[0], "r")) != NULL) {
	(void) fclose (fd);
    } else {
	as_error (ERR_CRIF, filenames[0]);
	exit (1);	/* Just in case, but should never get here */
    }
    DBUG_PRINT ("ofile", ("build output file name if necessary"));
    if (filenames[1] == NULL) {
	filenames[1] = bldofile (filenames[0]);
    }
    DBUG_PRINT ("tnames", ("populate namespace for temp files"));
    for (index = 2; index < NFILES; index++) {
	prefix[2] = 'A' + index - 1;
	filenames[index] = tempnam (TMPDIR, prefix);
    }
#ifdef EXPR
    prefix[2] = 'A' + index - 1;
    exprfilename = tempnam (TMPDIR, prefix);
    DBUG_PRINT ("exprfile", ("expression temp file is '%s'", exprfilename));
    if ((exprfp = fopen (exprfilename, "w+")) == NULL) {
	as_error (ERR_CCSTF);
    }
#endif
    if (macro) {
	DBUG_PRINT ("m4", ("preprocess through m4"));
	/* tell pass1 to unlink its input when through */
	rflag = YES;
	callm4 ();
    }
    exit (aspass1 ());
    DBUG_RETURN (0);
}

callm4 ()
{
    static char *av[] = {
	"m4", 0, 0, 0
    };

    /* 
     *	The following code had to be added with a 'u370' ifdef due
     *	to a MAXI bug concerning static pointers. We think it's fixed
     *	that is why the code is commented out. It can be deleted
     *	when this fact is verified.
     *
     *	char *regdef, *tvdef;
     *	regdef = CM4DEFS;
     *	tvdef = CM4TVDEFS;
     */

    DBUG_ENTER ("callm4");
    av[0] = m4name;
#if !NODEFS
#if TRANVEC
    av[1] = (transvec) ? tvdef : regdef;
    av[2] = filenames[0];
#else
    av[1] = regdef;
    av[2] = filenames[0];
#endif
#else
    av[1] = filenames[0];
#endif
    prefix[2] = 'A';
    filenames[0] = tempnam (TMPDIR, prefix);		/* m4 output file */
    if (callsys (m4name, av, filenames[0]) != 0) {
	(void) unlink (filenames[0]);
	(void) fprintf (stderr, "Assembly inhibited\n");
	exit (100);
    }
    DBUG_VOID_RETURN;
}

callsys (f, v, o)
char f[];
char *v[];
char *o;			/* file name, if any, for redirecting stdout */
{
    int t;
    int status;

    DBUG_ENTER ("callsys");
    DBUG_PRINT ("exec", ("f='%s', o='%s'", f, o==NULL?"<NULL>":o));
    if ((t = fork ()) == 0) {
	if (o != NULL) {	/* redirect stdout */
	    if (freopen (o, "w", stdout) == NULL) {
		(void) fprintf (stderr, "Can't open %s\n", o);
		exit (100);
	    }
	}
	(void) execv (f, v);
	(void) fprintf (stderr, "Can't find %s\n", f);
	exit (100);
    } else {
	if (t == -1) {
	    (void) fprintf (stderr, "Try again\n");
	    DBUG_RETURN (100);
	}
    }
    while (t != wait (&status));
    if ((t = (status & 0377)) != 0) {
	if (t != SIGINT) {			/* interrupt */
	    (void) fprintf (stderr, "status %o\n", status);
	    (void) fprintf (stderr, "Fatal error in %s\n", f);
	}
	exit (100);
    }
    DBUG_RETURN ((status >> 8) & 0377);
}


/*
 *  FUNCTION
 *
 *	bldofile    build output file name from input file name
 *
 *  SYNOPSIS
 *
 *	char *bldofile (ifile);
 *	char *ifile;
 *
 *  DESCRIPTION
 *
 *	Build output file name from the input file name, if user did not
 *	explicitly specify an output file name.  Argument "ifile" points
 *	to the input file name.
 *
 *	If "ofile" is not specified on the command line,
 *	it is created from "ifile" by the following algorithm:
 *
 *	     1.	If the name "ifile" ends with the two characters ".s",
 *		these are replaced with the two characters ".o".
 *
 *	     2.	If the name "ifile" does not end with the two
 *		characters ".s" and is no more than maxnamlen-2 characters
 *		in length, the name "ofile" is created by appending the
 *		characters ".o" to the name "ifile".
 *
 *	     3.	If the name "ifile" does not end with the two
 *		characters ".s" and is greater than maxnamlen-2 characters
 *		in length, the name "ofile" is created by appending the
 *		characters ".o" to the first maxnamlen-2 characters of
 *		"ifile".
 *
 *
 *	Note that rather than try to figure out exactly how many bytes
 *	of memory to malloc for the output file name, based on whether
 *	or not we will be appending to the input file name or replacing
 *	the suffix, we just malloc sufficient space for the worst
 *	case (appending a 2 letter suffix).
 *
 */

static char *bldofile (ifile)
register char *ifile;
{
    register short index;
    register short count;
    register char *ofile;

    DBUG_ENTER ("bldofile");
    DBUG_PRINT ("ifile", ("input file is '%s'", ifile));
    for (index = 0, count = 0; ifile[index] != '\0'; index++, count++) {
	if (ifile[index] == '/') {
	    count = -1;
	}
    }
    ofile = malloc ((unsigned) (strlen (ifile + index - count) + 3));
    if (ofile == NULL) {
	ofile = "a.out";
    } else {
	(void) strcpy (ofile, ifile + index - count);
	if (ofile[count - 2] == '.' && ofile[count - 1] == 's') {
	    ofile[count - 1] = 'o';
	} else {
	    if (count > (maxnamlen - 2)) {
		count = (maxnamlen - 2);
	    }
	    (void) strcpy (ofile + count, ".o");
	}
    }
    DBUG_PRINT ("ofile", ("output file will be '%s'", ofile));
    DBUG_RETURN (ofile);
}

/*
 *	If tempnam is not available, then just use tmpnam(),
 *	ignoring requested directory and prefix information.
 *
 *	The SIGXCPU define from <signal.h> is used to trigger
 *	compilation for BSD flavor systems.
 *	
 *	(OPUS COMMENT: SIGXCPU is not a legitimate way to determine
 *	whether a system is BSD or not with the advent of BCS.  It
 *	has been changed here to BSD_43.)
 *
 *	Versions of Sun OS before 3.2 may not have tempnam in
 *	libc.a.
 */

#ifdef BSD_43		/* If a BSD flavor system this is in <signal.h> */
#ifndef sun		/* Sun OS after 3.2 (at least) have tempnam */

char *tempnam (dir, pfx)
char *dir;
char *pfx;
{
    register char *tmpspace;
    extern char *tmpnam();

    tmpspace = malloc (L_tmpnam);
    return (tmpnam (tmpspace));
}

#endif
#endif

