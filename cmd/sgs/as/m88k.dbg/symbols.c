/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/symbols.c	1.8" */

/*
 *	"symbols.c" is a file containing functions for accessing the
 *	symbol table.
 *
 */

#include <stdio.h>
#include <string.h>
#include "syms.h"
#include "systems.h"
#include "symbols.h"
#include "symbols2.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

#ifdef index
#undef index
#endif

extern short passnbr;
extern unsigned numcalls;
extern unsigned numids;
extern unsigned numcoll;

symbol symtab[NSYMS];
upsymins hashtab[NHASH];

/*
 *	An alias table contains ordered pairs of pointers to symbols that
 *	are to be aliased, specified by the user via the 'def' pseudo.
 *	The first symbol takes on all the attributes of the second symbol
 *	after the first pass is complete.  Multiple passes over the
 *	alias table is necessary to resolve all forward references.  I.E.
 *
 *			def	a,b
 *			def	b,c
 *		c:
 *			ld	r1,r2,r3
 *
 *	results in an alias table with the order pairs
 *
 *			a,b
 *			b,c
 *
 *	The first pass through the table finds that since c is defined,
 *	b can be defined.  The second pass then finds b defined and defines
 *	a.
 *
 *	Note that the worst case size of the alias table is related to the
 *	size of the symbol table.  If there are N symbols, there can be
 *	at most N-1 aliases, where all symbols but one appear exactly
 *	once as the first member and the Nth symbol appears as the second
 *	member.  The last slot is left NULL to mark the end of the
 *	aliases.
 */

struct alias {
    symbol *sym1;
    symbol *sym2;
};

struct alias aliases[NSYMS];

static int unresolved;		/* Count of aliases */

/*
 *	Various external symbols referenced.
 */

char *strtab;
long currindex;

long size;
long basicsize = 4 * BUFSIZ;

char *symprt ();
char *realloc ();

extern char *malloc ();
extern void as_error ();

static int symcnt = 0;

/*
 *  FUNCTION
 *
 *	newent    allocate and initialize a new symbol table entry
 *
 *  SYNOPSIS
 *
 *	static symbol *newent (strptr)
 *	char *strptr;
 *
 *  DESCRIPTION
 *
 *	Creates a new symbol table entry for the symbol with name
 *	pointed to by strptr.  The type of the symbol is set to undefined.
 *	All other fields of the entry are set to zero.
 *
 */

static symbol *newent (strptr)
register char *strptr;
{
    register symbol *symptr;
    register char *ptr1;

    DBUG_ENTER ("newent");
    DBUG_PRINT ("sym", ("new symbol table entry at %d for '%s'", symcnt, strptr));
    if (symcnt >= NSYMS) {
	as_error (ERR_SYMOVF);
	symptr = (symbol *)NULL;
    } else {
	symptr = &symtab[symcnt++];
	if (strlen (strptr) > SYMNMLEN) {
	    symptr -> _name.tabentry.zeroes = 0L;
	    symptr -> _name.tabentry.offset = currindex;
	    addstr (strptr);
	} else {
	    /* could use strncpy here?  (fnf) */
	    for (ptr1 = symptr -> _name.name; *ptr1++ = *strptr++;) {;}
	}
	symptr -> tag = 0;
	symptr -> styp = UNDEF;
	symptr -> value = 0L;
	symptr -> maxval = 0;
	symptr -> sectnum = 0;
    }
    DBUG_RETURN (symptr);
}

/*
 *  FUNCTION
 *
 *	lookup    look up a named symbol in the table
 *
 *  SYNOPSIS
 *
 *	upsymins *lookup (sptr, install, usrname)
 *	char *sptr;
 *	BYTE install;
 *	BYTE usrname;
 *
 *  DESCRIPTION
 *
 *	Looks up the symbol whose name is *sptr in the symbol
 *	table and returns a pointer to the entry for that symbol.
 *
 *	Install is INSTALL if symbol is to be installed, in which
 *	case a new entry is allocated and initialized if an existing
 *	entry is not found.  If install is N_INSTALL, it is only looked
 *	up, but not automatically installed if not already in the
 *	symbol table (in this case a pointer to a non-initialized
 *	entry, that may be subsequently used, is returned).
 *
 *	Usrname is USRNAME if the symbol is user-defined, MNEMON if it is
 *	an instruction mnemonic.
 *
 *	[NOTE:  Under the current implementation, usrname
 *	is always USRNAME.  Mnemonics are currently kept in a separate
 *	table, but apparently they were all kept in one table at one
 *	time.  Could they be merged again (fnf)]
 *
 */

upsymins *lookup (sptr, install, usrname)
char *sptr;
BYTE install;
BYTE usrname;
{
    register upsymins *hp;
    unsigned short register hashval;
    unsigned short probe;
    upsymins *ohp;
    char *temp;
    unsigned short hash ();

    DBUG_ENTER ("lookup");
    DBUG_PRINT ("sym", ("lookup symbol '%s'; install = %d", sptr, install));
    numcalls++;
    probe = 1;
    hashval = hash (sptr, NHASH);
    hp = ohp = &hashtab[hashval];
    do {
	if (hp -> stp == NULL) {		/* free */
	    DBUG_PRINT ("sym", ("symbol not found in table"));
	    if (install == INSTALL) {
		numids++;
		hp -> stp = newent (sptr);
	    }
	    DBUG_PRINT ("symprt", ("%s", symprt (hp -> stp)));
	    DBUG_RETURN (hp);
	} else {
	    /* Compare the string given with the symbol string.	 */
	    /* The symbol string can be in either the symbol entry */
	    /* or the string table. */
	    if (hp -> stp -> _name.tabentry.zeroes != 0) {
		temp = (hp -> stp -> _name.name);
	    } else {
		temp = &strtab[hp -> stp -> _name.tabentry.offset];
	    }
	    if (strcmp (sptr, temp) == 0) {
		if (passnbr == 1) {
		    if (install == INSTALL
			&& hp -> itp -> tag
			&& hp -> itp -> snext == NULL) {
			hp -> itp -> snext = newent (sptr);
		    }
		    if (!usrname && hp -> itp -> tag) {
			hp = (upsymins *) &(hp -> itp -> snext);
			DBUG_PRINT ("symprt", ("%s", symprt (hp -> stp)));
			DBUG_RETURN (hp);
		    }
		}
		if (install != INSTALL
		    && !usrname 
		    && hp -> itp -> tag
		    && (passnbr == 2)) {
		    hp = (upsymins *) &(hp -> itp -> snext);
		}
		DBUG_PRINT ("symprt", ("%s", symprt (hp -> stp)));
		DBUG_RETURN (hp);
	    }
	    hashval = (hashval + probe) % NHASH;
	    probe += 2;
	    numcoll++;
	}
    } while ((hp = &hashtab[hashval]) != ohp);
    as_error (ERR_HASHOVF);
    DBUG_RETURN ((upsymins *)NULL);
}

/*
 *	traverse(func)
 *
 *	Goes through the symbol table and calls the function "func"
 *	for each entry.
 *
 */

void traverse (func)
int (*func) ();
{
    register short index;

    DBUG_ENTER ("traverse");
    for (index = 0; index < symcnt; ++index) {
	(*func) (&symtab[index]);
    }
    DBUG_VOID_RETURN;
}

/*
 *	addstr(string)
 *
 *	Enters the "string" into the string table.  Called by
 *	newent().  Space for the string table is initially
 *	malloc()-ed in strtabinit().  If "string" would exceed
 *	the available space, then the string table is realloc()-ed
 *	with a larger size.  This procedure is only used in the
 *	flexnames version of the assembler.
 *
 */

addstr (strptr)
register char *strptr;
{
    register int length;

    DBUG_ENTER ("addstr");
    length = strlen (strptr);
    if (length + currindex >= size) {
	if ((strtab = realloc (strtab, (unsigned) (size += basicsize))) == NULL) {
	    as_error (ERR_CRSTBL);
	}
    }
    (void) strcpy (&strtab[currindex], strptr);
    currindex += length + 1;
    DBUG_VOID_RETURN;
}


/*
 *	strtabinit()
 *
 *	Sets up the string table, with space malloc()-ed.  This
 *	procedure is only used in the flexnames version of the
 *	assembler.
 *
 *	This routine is called once on startup to allocate space for
 *	a string table, used to store symbol names that exceed the
 *	length which can be directly stored in a symbol table structure.
 *	For these long symbols, an offset into this string table is
 *	stored in the symbol table structure, instead of the literal
 *	string, and the string is stored in the string table.
 *
 *	The string table can grow dynamically as necessary.
 *
 *	The initial offset is set to a small number, presumably to avoid
 *	having a 0 offset, thus making internal error detection more
 *	robust.
 *
 */

strtabinit ()
{
    DBUG_ENTER ("strtabinit");
    DBUG_PRINT ("strtab", ("alloc and init string table (size = %d)", basicsize));
    if ((strtab = malloc ((unsigned) (size = basicsize))) == NULL) {
	as_error ("cannot malloc string table");
    }
    currindex = 4;
    DBUG_VOID_RETURN;
}

/*
 *	Note, there are currently two different hash tables in the 
 *	assembler, one for the mnemonic names and one for the symbol
 *	names (could these be merged?).  For consistency and simplicity,
 *	they both use the same hash function.  Since they may be different
 *	sizes, the maximum size is also passed as a parameter.
 */

unsigned short hash (sp, size)
register char *sp;
unsigned short size;
{
    register unsigned short ihash = 0;

    while (*sp) {
	ihash <<= 2;
	ihash += *sp++;
    }
    ihash += *--sp << 5;
    ihash %= size;
    return (ihash);
}

#ifndef NO_DBUG

/*
 *	Support routines for internal debugging package.
 */

static char dbuff[256];	/* Reusable data area */

/*
 *	Symprt builds a human readable string representing
 *	information about a symbol and returns a pointer
 *	to a static data area containing the string.
 *
 *	Note that this function uses a possibly non-portable feature of
 *	sprintf whereby sprintf can be used to append to an existing string.
 *
 */

char *symprt (symptr)
symbol *symptr;
{
    register char *temp;

    dbuff[0] = '\000';
    if (symptr == NULL) {
	strcpy (dbuff, "<NULL SYMBOL>");
    } else {
	if (symptr -> _name.tabentry.zeroes != 0) {
	    temp = (symptr -> _name.name);
	} else {
	    temp = &strtab[symptr -> _name.tabentry.offset];
	}
	sprintf (dbuff, "'%s'", temp);
	switch (symptr -> tag) {
	    case INDEX:
		sprintf (dbuff, "%s INDEX", dbuff);
		break;
	    case CHARPTR:
		sprintf (dbuff, "%s CHARPTR", dbuff);
		break;
	    default:
		sprintf (dbuff, "%s <%d>", dbuff, symptr -> tag);
		break;
	}
	switch (symptr -> styp & TYPE) {
	    case UNDEF:
		sprintf (dbuff, "%s UNDEF", dbuff);
		break;
	    case ABS:
		sprintf (dbuff, "%s ABS", dbuff);
		break;
	    case TXT:
		sprintf (dbuff, "%s TXT", dbuff);
		break;
	    case DAT:
		sprintf (dbuff, "%s DAT", dbuff);
		break;
	    case BSS:
		sprintf (dbuff, "%s BSS", dbuff);
		break;
	    default:
		sprintf (dbuff, "%s <%#x>", dbuff, symptr -> styp & TYPE);
		break;
	}
	if (symptr -> styp & SECTION) {
	    sprintf (dbuff, "%s SECTION", dbuff);
	}
	if (symptr -> styp & TVDEF) {
	    sprintf (dbuff, "%s TVDEF", dbuff);
	}
	if (symptr -> styp & EXTERN) {
	    sprintf (dbuff, "%s EXTERN", dbuff);
	}
	sprintf (dbuff, "%s value=%#lx", dbuff, symptr -> value);
	sprintf (dbuff, "%s maxval=%#x", dbuff, symptr -> maxval);
	sprintf (dbuff, "%s sectnum=%d", dbuff, symptr -> sectnum);
    }
    return (dbuff);
}
#endif

void symalias (symptr1, symptr2)
symbol *symptr1;
symbol *symptr2;
{
    struct alias *aliasp;

    DBUG_ENTER ("symalias");
    DBUG_PRINT ("alias", ("%s", symprt (symptr1)));
    DBUG_PRINT ("alias", ("%s", symprt (symptr2)));
    for (aliasp = aliases; aliasp -> sym1 != NULL; aliasp++) {;}
    DBUG_PRINT ("alias", ("empty slot at %d", aliasp - aliases));
    aliasp -> sym1 = symptr1;
    aliasp -> sym2 = symptr2;
    unresolved++;
    DBUG_VOID_RETURN;
}

void resolvealiases ()
{
    struct alias *aliasp;
    register int foundone;

    DBUG_ENTER ("resolvealiases");
    do {
	foundone = 0;
	for (aliasp = aliases; aliasp -> sym1 != NULL; aliasp++) {
	    if ((aliasp -> sym1 -> styp & TYPE) == UNDEF) {
		if ((aliasp -> sym2 -> styp & TYPE) != UNDEF) {
		    DBUG_PRINT ("alias", ("resolve slot %d", aliasp - aliases));
		    foundone = 1;
		    unresolved--;
		    aliasp -> sym1 -> tag = aliasp -> sym2 -> tag;
		    aliasp -> sym1 -> styp = aliasp -> sym2 -> styp;
		    aliasp -> sym1 -> value = aliasp -> sym2 -> value;
		    aliasp -> sym1 -> maxval = aliasp -> sym2 -> maxval;
		    aliasp -> sym1 -> sectnum = aliasp -> sym2 -> sectnum;
		}
	    }
	}
    } while (foundone);
    if (unresolved > 0) {
	as_error (ERR_URALIAS, unresolved);
    }
    DBUG_VOID_RETURN;
}

