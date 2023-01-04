/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/code.c	1.13" */

#include <stdio.h>
#include "codeout.h"
#include "scnhdr.h"
#include "section.h"
#include "systems.h"
#include "symbols.h"
#include "gendefs.h"
#include "temppack.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugger */

int previous = 0;
int counter = -1;
int seccnt = 0;
short bitpos = 0;
extern symbol symtab[];

extern long newdot;		/* up-to-date value of "." */

extern symbol *dot;
extern unsigned short line;
extern char *filenames[];
extern upsymins *lookup ();
extern void as_error ();
extern char *strncpy ();
extern void as_error ();

struct scnhdr sectab[NSECTIONS + 1];	/* section header table */
struct scninfo secdat[NSECTIONS + 1];	/* section info table */

static void putlong ();

/*
 *	generate creates an intermediate file entry from the info
 *	supplied thru its args. The intermediate file format consists
 *	of fixed sized records of the codebuf structure (see codeout.h).
 *	These entries represent the encoded user program and
 *	will be used as input to the second pass of the assembler.
 *	Different files, with their associated buffers, are maintained
 *	for each section.
 */

void generate (nbits, action, value, sym)
BYTE nbits;
unsigned short action;
long value;
symbol *sym;
{
    register long pckword;
    register struct scninfo *seci;
    unsigned short locindex = 0;
    static int nosecerr = 1;

    DBUG_ENTER ("generate");
    DBUG_PRINT ("gen", ("nbits=%d action=%d value=%#lx", nbits, action, value));
    DBUG_PRINT ("dotc", ("bitpos=%d, nbits=%d, newdot=%#lx", bitpos, nbits, newdot));
    if (sym != NULL) {
	locindex = 1 + (sym - symtab);
	DBUG_PRINT ("sym", ("associated with symtab[%u]", locindex - 1));
    }
    if ((sectab[dot -> sectnum].s_flags & STYP_BSS) && nbits != 0) {
	/* Attempt to initialize bss */
	as_error (ERR_IBSS);
    } else {
	seci = &secdat[dot -> sectnum];
	/* the following "if" statment */
	/* was introduced for software workarounds. */
	/* these functions use it to count */
	/* the number bits which are generated. */
	/* in the normal case "counter == -1" end does not */
	/* affect the code generation */
	if ((dot -> styp & TXT) && (counter >= 0)) {
	    counter += nbits;
	    DBUG_VOID_RETURN;
	}
	/*
	 * Check if any code was generated since last NEWSTMT. If not then no
	 * need to output this record.
	 */
	if (action != NEWSTMT) {
	    seci -> s_any += nbits;		/* indicate code generated */
	} else {
	    if (seci -> s_any) {
		seci -> s_any = 0;		/* reset flag */
	    } else {
		DBUG_VOID_RETURN;
	    }
	}
	if ((seccnt <= 0) && nbits) {
	    if (nosecerr) {
		nosecerr = 0;
		as_error (ERR_YYERROR, "Code generated before naming section");
	    }
	} else {
	    DBUG_PRINT ("cbuf", ("put a codebuf structure out to packed temp file"));
	    pckword = MKACTNUM (action) | MKNUMBITS (nbits);
	    if ((locindex) != 0) {
		pckword |= SYMINDEX | (locindex & LWRBITS);
		if ((value) == 0) {
		    putlong (seci, (long) (pckword | VAL0));
		} else {
		    putlong (seci, pckword);
		    putlong (seci, value);
		}
	    } else if (((value) & UPRBITS) == 0) {
		putlong (seci, (long) (VAL16 | SYMORVAL (value) | pckword));
	    } else {
		putlong (seci, pckword);
		putlong (seci, value);
	    }
	    bitpos += nbits;
	    newdot += bitpos / BITSPBY;
	    DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
	    bitpos %= BITSPBY;
	    DBUG_PRINT ("dotc", ("bitpos=%d, nbits=%d", bitpos, nbits));
	}
    }
    DBUG_VOID_RETURN;
}


void flushbuf ()
{
    register int i;
    register struct scninfo *p;
    fprintf(stderr, "IN FLUSHBUF\n");

    DBUG_ENTER ("flushbuf");
    for (i = 1, p = &secdat[1]; i <= seccnt; i++, p++) {
    fprintf(stderr, "p->s_fd->_flag is %#o\n", p->s_fd->_flag);
	DBUG_PRINT ("flush", ("flush internal buffer for section %d", i));
	fprintf(stderr, "before fwrite (ferror (p -> s_fd)) is %#o\n", ferror (p -> s_fd));
	(void) fwrite ((char *) p -> s_buf, sizeof (long), (int) (p -> s_cnt), p -> s_fd);
    fprintf(stderr, "2: p->s_fd is %#x\n", p->s_fd);
	p -> s_cnt = 0;
	fprintf(stderr, "before fflush (ferror (p -> s_fd)) is %#o, f->s_fd->_flag is %#o\n", ferror (p -> s_fd), p->s_fd->_flag);
	(void) fflush (p -> s_fd);
	fprintf(stderr, "(ferror (p -> s_fd)) is %#o\n", ferror (p -> s_fd));
	if (ferror (p -> s_fd)) {
fprintf(stderr, "11111\n");
	    as_error (ERR_TWR);
	}
	DBUG_PRINT ("close", ("close temporary file for section %d", i));
	fprintf(stderr, "CALLING FCLOSE 1\n");
	/*(void) fclose (1, p -> s_fd); */
    }
    DBUG_VOID_RETURN;
}

/*
 *	cgsect changes the section into which the assembler is 
 *	generating info
 *
 *	First save current section's number and value for "dot".
 */

void cgsect (newsec)
register int newsec;
{
    DBUG_ENTER ("cgsect");
    DBUG_PRINT ("sect", ("change to section %d", newsec));
    sectab[dot -> sectnum].s_size = newdot;
    previous = dot -> sectnum;
    dot -> sectnum = newsec;
    dot -> styp = secdat[newsec].s_typ;
    dot -> value = newdot = sectab[newsec].s_size;
    DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
    DBUG_VOID_RETURN;
}

/*
 * search for (and possibly define) a section by name
 */

int mksect (sym, att)
register symbol *sym;
register int att;
{
    register struct scnhdr *sect;
    register struct scninfo *seci;
    FILE * fd;

fprintf(stderr, "IN MKSECT******\n");
    DBUG_ENTER ("mksect");
    DBUG_PRINT ("sect", ("section '%s', attributes %#x", sym -> _name.name, att));
    if (!(sym -> styp & SECTION)) {
	/* is symbol a previously defined section? */
	if (!att) {
	    as_error (ERR_YYERROR, "Section definition must have attributes");
	} else if ((sym -> styp & TYPE) != UNDEF) {
	    as_error (ERR_YYERROR, "Section name already defined");
	} else if (seccnt >= NSECTIONS) {
	    as_error (ERR_YYERROR, "Too many sections defined");
	} else if (strlen (sym -> _name.name) > 8) {
	    as_error (ERR_YYERROR, "Section name too long");
	} else {
	    seccnt++;
	    if ((fd = fopen (filenames[seccnt + 5], "w")) == NULL) {
		as_error (ERR_CCSTF);
	    }
	    fprintf(stderr, "XXX: FD is %#x, att is %#x, name is %s\n", fd, att,  filenames[seccnt + 5]);
	    fprintf(stderr, "SECCNT = %d, fd->_flag is %#o\n", seccnt, fd->_flag);
	    seci = &secdat[seccnt];
	    sect = &sectab[seccnt];
	    seci -> s_fd = fd;
	    if (att & STYP_TEXT) {
		seci -> s_typ = TXT;
	    } else if (att & STYP_DATA) {
		seci -> s_typ = DAT;
	    } else if (att & STYP_BSS) {
		seci -> s_typ = BSS;
	    } else {
		seci -> s_typ = ABS;
	    }
	    fprintf(stderr, "2222XXX: FD is %#x, att is %#x, name is %s\n", fd, att,  filenames[seccnt + 5]);
	    fprintf(stderr, "2222SECCNT = %d, fd->_flag is %#o\n", seccnt, fd->_flag);
	    sect -> s_flags = att;
	    (void) strncpy (sect -> s_name, sym -> _name.name, 8);
	    sym -> styp = SECTION;
	    sym -> sectnum = seccnt;
	    sym -> value = att;
	}
    } else if (att && (sym -> value != att)) {
	as_error (ERR_YYERROR, "Section attributes do not match");
    }
    DBUG_RETURN (sym -> sectnum);
}

void comment (string)
char *string;
{
    int i;
    int j;
    int k;
    int prevsec;
    int size;
    union {
	long lng;
	char chr[sizeof (long) / sizeof (char)];
    } value;
    static int comsec = -1;

    DBUG_ENTER ("comment");
    DBUG_PRINT ("ident", ("comment '%s'", string));
    if (comsec < 0) {
    fprintf(stderr, "CALLING MKSECT5\n");
#if 0
	comsec = mksect (lookup (_COMMENT, INSTALL, USRNAME) -> stp, STYP_INFO);
#endif
    }
    prevsec = previous;
    cgsect (comsec);
    size = strlen (string) + 1;
    for (i = 0; i < size / 4; i++) {
	for (j = 0; j < sizeof (long) / sizeof (char); j++) {
	    value.chr[j] = *(string + j + i * 4);
	}
	generate (BITSPW, NOACTION, value.lng, NULLSYM);
    }
    if (size % 4) {
	for (k = 0, j = sizeof (long) / sizeof (char) - size % 4; j < sizeof (long) / sizeof (char); j++, k++) {
	    value.chr[j] = *(string + k + i * 4);
	}
	generate ((size % 4) * BITSPBY, NOACTION, value.lng, NULLSYM);
    }
    cgsect (previous);
    previous = prevsec;
    DBUG_VOID_RETURN;
}

#ifdef EXPR
/*
 *  Place nbytes worth of output in the .expr section, and return the
 *  offset into the section at which the nbytes of output was written.
 */

long emitexpr (exp, nbytes)
char *exp;
int nbytes;
{
    register int i;
    register int j;
    auto int prevsec;
    auto int rtnval;
    union {
	long lng;
	char chr[sizeof (long) / sizeof (char)];
    } value;
    static int exprsec = -1;

    DBUG_ENTER ("emitexpr");
    DBUG_PRINT ("expr", ("output %d bytes in .expr section", nbytes));
    if (exprsec < 0) {
    fprintf(stderr, "CALLING MKSECT6\n");
	comsec = mksect (lookup (_COMMENT, INSTALL, USRNAME) -> stp, STYP_INFO);
    }
    prevsec = previous;
    cgsect (exprsec);
    rtnval = sectab[exprsec].s_size;
    for (i = 0; i < nbytes / 4; i++) {
	for (j = 0; j < sizeof (long) / sizeof (char); j++) {
#if clipper || ns32000
	    value.chr[3-j] = *exp++;
#else
	    value.chr[j] = *exp++;
#endif
	}
	generate (BITSPW, NOACTION, value.lng, NULLSYM);
    }
    if (nbytes % 4) {
	for (j = sizeof (long) / sizeof (char) - nbytes % 4; j < sizeof (long) / sizeof (char); j++) {
#if clipper || ns32000
	    value.chr[3-j] = *exp++;
#else
	    value.chr[j] = *exp++;
#endif
	}
	generate ((nbytes % 4) * BITSPBY, NOACTION, value.lng, NULLSYM);
    }
    cgsect (previous);
    previous = prevsec;
    DBUG_RETURN (rtnval);
}
#endif

/*
 *	Save a long value in the section's output buffer, and then test and
 *	flush the buffer if necessary.  Note that flushing in this case
 *	simply refers to the first level flushing of the section data
 *	buffer to stdio's buffer.  The stdio routines determine when
 *	to do their own flushing to a buffer outside the process's
 *	address space.
 */

static void putlong (seci, value)
register struct scninfo *seci;
long value;
{
    DBUG_ENTER ("putlong");
    DBUG_PRINT ("secdat", ("output %#lx", value));
    seci -> s_buf[seci -> s_cnt++] = value;
    if (seci -> s_cnt == TBUFSIZ) {
	DBUG_PRINT ("secflsh", ("flush section output buffer"));
	(void) fwrite ((char *) (seci -> s_buf), sizeof (long), TBUFSIZ, seci -> s_fd);
	seci -> s_cnt = 0;
	if (ferror (seci -> s_fd)) {
   fprintf(stderr, "22222\n");
	    as_error (ERR_TWR);
	}
    }
    DBUG_VOID_RETURN;
}

#ifdef EXPR
void exprsect ()
{
    register char index;
    auto int expsize;
    auto short sectnum;
    auto char expbuf[64];
    extern FILE *exprfp;

    DBUG_ENTER ("exprsect");
    for (index = 0; index <= seccnt; index++) {
	DBUG_PRINT ("exp", ("check for section %d expressions", index));
	(void) fseek (exprfp, 0L, 0);
	while (fread (&expsize, sizeof (int), 1, exprfp) == 1) {
	    DBUG_PRINT ("exp", ("next expression %d bytes", expsize));
	    fread (&sectnum, sizeof (short), 1, exprfp);
	    DBUG_PRINT ("exp", ("next section number %ld", sectnum));
	    fread (expbuf, expsize, 1, exprfp);
	    if (sectnum == index) {
		emitexpr (expbuf, expsize);
	    }
	}
    }
    fclose (2, exprfp);
    DBUG_VOID_RETURN;
} 
#endif
