/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: maketables.c 2.2 88/04/08 14:07:04 root Exp $ RISC Source Base"

/*	tool for processing ops.in file.  makes:			*/
/*									*/
/*		ind.out - switch index defines for the instruction classes */
/*		typ.out - type arrays for mnemonics			*/
/*		mnm.out - mnemonic table initialization			*/
/*		sem.out - semantics for instructions			*/

#include <stdio.h>
#include <ctype.h>
#include "dbug.h"

int curtypeno = 0;
int curindno = 0;
FILE *fd_index;
FILE *fd_types;
FILE *fd_ops;
FILE *fd_sem;

static char *ind_head[] = {
    "/* No header yet */",
    NULL
};

static char *typ_head[] = {
    "/* No header yet */",
    NULL
};

static char *mnm_head[] = {
    "/*",
    " * Initialization for mnemonics[].  Format is as follows:",
    " *",
    " *            unsigned    unsigned            unsigned   unsigned",
    " *   char *   short       short      long     long  *    short     short",
    " *   name     tag         nbits      opcode   opndtypes  index     next",
    " *",
    " */",
    "",
    NULL
};

static char *sem_head[] = {
    "/* No header yet */",
    NULL
};

main (argc, argv) 
int argc;
char *argv[];
{
    register char *arg;
    void header ();

    DBUG_ENTER ("main");
    if (argc > 1 && ((arg = argv[1]) != NULL)) {
	if (*arg++ == '-' && *arg++ == '#') {
	    DBUG_PUSH (arg);
	}
    }
    fd_index = fopen ("ind.out", "w");
    header (fd_index, ind_head);
    fd_types = fopen ("typ.out", "w");
    header (fd_types, typ_head);
    fd_ops = fopen ("mnm.out", "w");
    header (fd_ops, mnm_head);
    fd_sem = fopen ("sem.out", "w");
    header (fd_sem, sem_head);
    (void) fprintf (fd_types, "static unsigned long t0[] = {0};\n");
    while (getblock ());
    DBUG_RETURN (0);
}


getblock () 
{
    char line[BUFSIZ];
    char indexname[BUFSIZ];
    register char *lptr;
    register char *ptr;
    int typeno = 0;

    DBUG_ENTER ("getblock");
    if (gets (line) == NULL) {
	DBUG_RETURN (0);
    }
    lptr = line;
    ptr = indexname;
    while (isalnum (*lptr)) {
	*ptr++ = *lptr++;
    }
    *ptr = 0;
    (void) fprintf (fd_index, "#define %s %d\n", indexname, ++curindno);
    while (isspace (*lptr)) {
	lptr++;
    }
    if (*lptr) {
	(void) fprintf (fd_types, "static unsigned long t%d[] = {%s, 0};\n",
		typeno = ++curtypeno, lptr);
    }
    (void) fprintf (fd_sem, "case %s:\n", indexname);
    do {
	if (gets (line) == NULL) {
	    error (line);
	}
	(void) fprintf (fd_sem, "%s\n", line);
    } while (line[0] != '}');
    (void) fprintf (fd_sem, "break;\n");
    if (gets (line) == NULL) {
	error (line);
    }
    do {
	if (line[0] == '#') {	/* send cpp directives on through */
	    (void) fprintf (fd_ops, "%s\n", line);
	    goto getanother;
	}
	(void) fprintf (fd_ops, "{%s,", line);
	(void) fprintf (fd_ops, "\tt%d,", typeno);
	(void) fprintf (fd_ops, "\t%s,", indexname);
	(void) fprintf (fd_ops, "\t0},\n");
getanother: 
	if (gets (line) == NULL) {
	    error (line);
	}
    } while (line[0] != '.');
    DBUG_RETURN (1);
}


error (msg) 
{
    DBUG_ENTER ("error");
    (void) fprintf (stderr, "syntax error: %s\n", msg);
    exit (1);
    DBUG_VOID_RETURN;
}

static void header (fp, hdrp)
FILE *fp;
char **hdrp;
{
    DBUG_ENTER ("header");
    if (hdrp != NULL) {
	while (*hdrp != NULL) {
	    (void) fprintf (fp, "%s\n", *hdrp++);
	}
    }
    DBUG_VOID_RETURN;
}
