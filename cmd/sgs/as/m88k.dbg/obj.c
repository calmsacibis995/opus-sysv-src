/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	This IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/obj.c	1.28" */

#include <stdio.h>
#include <sgs.h>
#include <filehdr.h>
#include <linenum.h>
#include <reloc.h>
#include "scnhdr.h"
#include <syms.h>
#include "systems.h"
#include "symbols.h"
#include "gendefs.h"
#include "codeout.h"
#include "section.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

/*
 *
 *	"obj.c" is a file that contains the routines for
 *	creating the object file during the final pass of the assembler.
 *	These include an assortment of routines for putting
 *	out header information, relocation and line number entries, and
 *	other data that is not part of the object program itself.  The
 *	main routines for producing the object program from the
 *	temporary files can be found in the file "codout.c".
 *
 */

/*
 *
 *	"outblock" is a function that outputs a block "block" with size
 *	"size" bytes to the file whose descriptor appears in "file".
 *
 */

#define outblock(a,b,c)	(void)fwrite((char *)(a),(b),1,(c))

/*
 *
 *	"inblock" is a function that reads a block "block" of size
 *	"size" bytes from the file whose descriptor appears in "file".
 *
 */

#define inblock(a,b,c)	fread((char *)(a),(b),1,(c))

#define MAXSTK	(20)

extern void traverse ();
extern void endef ();
extern void setval ();
extern void setscl ();
extern char *calloc ();
extern void define ();
extern void as_error ();
extern void putindx ();
extern void dfaxent ();

extern short transvec;	/* indicates transfer vector program addressing */

extern long hdrptr;
extern long symbent;
extern long gsymbent;

extern long getindx ();
extern long time ();

extern FILE *fdout;
extern FILE *fdrel;
extern FILE *fdsymb;
extern FILE *fdgsymb;

extern upsymins *lookup ();

extern char *strtab;

extern int seccnt;
extern struct scnhdr sectab[];
extern struct scninfo secdat[];

/*
 *	The following variables are for the .expr section kluge and
 *	allow us to independently seek/read/write the output file, inside
 *	the .expr section raw data, keep track of the current offset, and
 *	keep track of the base offset to compute offsets within the
 *	section.
 */
#ifdef EXPR
extern FILE *fdexpr; 		/* Stream to access .expr raw data */
extern long exprhome;		/* Offset to start of .expr section */
#endif

long sect_lnnoptrs[NSECTIONS + 1];
short sttop = 0;

stent firstsym = {
    0L,
    0L,
    NULL
};

stent *symhead = &firstsym;
stent *symtail = &firstsym;
stent *currsym = &firstsym;


stent *stack[MAXSTK];

symbol *dot;

FILHDR filhead = {
    MAGIC,	/* magic number */
    0,		/* number of sections, adjusted in headers() to seccnt */
    0L,		/* time and date stamp */
    0L,		/* file pointer to symbol table */
    0L,		/* number of symbol table entries */
    0,		/* size of optional header in bytes */
    0
};

static char buffer[BUFSIZ];

static unsigned undefcnt = 0;

/*
 * "headers" is a function that creates the file header and all of
 * the section headers and writes them to the file whose descriptor
 * appears in "fdout".
 *
 * The file header is built in "filhead".  The size of initialized
 * sections is obtained by adding the individual sizes.  The number
 * of symbol table entries are found in "symbent".  These are
 * stored into the header, and the header is written out using
 * "outblock".
 *
 * The dummy section header that points to the symbol table is
 * built in "symhead".  The file offset is obtained from "offset".
 */

void headers ()
{
    register int sectnum;	/* Section number (index) */
    long lin;			/* Number of line-number entries */
    long rel;			/* Number of relocation entries */
    long siz;
    long oldptr;
    long address;
    long secptr;
    long linptr;
    long relptr;
    register struct scnhdr *p;

    DBUG_ENTER ("headers");
    DBUG_PRINT ("fhd", ("create and write out file header and all section headers"));
    oldptr = ftell (fdout);
    (void) fseek (fdout, hdrptr, 0);
    lin = rel = siz = 0;
    DBUG_PRINT ("ents", ("scan through section table accumulating sizes"));
    for (sectnum = 1, p = &sectab[1]; sectnum <= seccnt; sectnum++, p++) {
	lin += p -> s_nlnno;
	rel += p -> s_nreloc;
	if (secdat[sectnum].s_typ != BSS) {
	    siz += p -> s_size;
	}
    }
    address = 0;
    secptr = ((FILHSZ + seccnt * SCNHSZ) + SCTALIGN - 1) & -SCTALIGN;
    relptr = secptr + siz;
    linptr = relptr + rel * RELSZ;
    filhead.f_nscns = seccnt;
    filhead.f_symptr = linptr + lin * LINESZ;
    filhead.f_nsyms = symbent;
    (void) time (&(filhead.f_timdat));
    filhead.f_flags = 0;
#if 0
    if (!undefcnt && !transvec) {
	filhead.f_flags |= F_EXEC;
    }
#endif
    if (!lin) {
	filhead.f_flags |= F_LNNO;
    }
    /* 
     * determine the conversion flag based on source machine  
     */
#if AR16WR
    filhead.f_flags |= F_AR16WR;
#endif
#if AR32WR
    filhead.f_flags |= F_AR32WR;
#endif
#if AR32W
    filhead.f_flags |= F_AR32W;
#endif
    DBUG_PRINT ("filhd", ("output the completed file header"));
    outblock (&filhead, FILHSZ, fdout);
    DBUG_PRINT ("addrs", ("cycle through sections, building and writing headers"));
    for (sectnum = 1, p = &sectab[1]; sectnum <= seccnt; sectnum++, p++) {
	p -> s_paddr = p -> s_vaddr = address;
	if (p -> s_nreloc) {
	    p -> s_relptr = relptr;
	}
	if (p -> s_nlnno) {
	    p -> s_lnnoptr = sect_lnnoptrs[sectnum] = linptr;
	}
	if (secdat[sectnum].s_typ != BSS) {
	    if (p -> s_size) {
		p -> s_scnptr = secptr;
	    }
	    secptr += p -> s_size;
	}
	address += p -> s_size;
	relptr += p -> s_nreloc * RELSZ;
	linptr += p -> s_nlnno * LINESZ;
	if (p -> s_flags & STYP_LIB) {
	    p -> s_flags &= ~STYP_DATA;
	}
	DBUG_PRINT ("shdr", ("output header for section %d", sectnum));
	outblock (p, SCNHSZ, fdout);
    }
    (void) fseek (fdout, oldptr, 0);
    DBUG_VOID_RETURN;
}

/*
 *
 *	"copysect" is a function that is used to copy section
 *	information from an intermediate file to the object file.  It
 *	is passed the name of the intermediate file as a parameter.
 *	The object file should be open when this function is called,
 *	and its descriptor should appear in "fdout".  The intermediate
 *	file is opened, and then copied to "fdout" in a loop that
 *	alternately calls "fread" and "fwrite".
 *
 */

void copysect (file)
char *file;
{
    register FILE *fd;
    register short numelmts;

    DBUG_ENTER ("copysect");
    DBUG_PRINT ("cpsect", ("copy section info from '%s' to the object file", file));
    if ((fd = fopen (file, "r")) == NULL) {
	as_error (ERR_COTR, file);
    }
    do {
	numelmts = fread (buffer, sizeof (*buffer), BUFSIZ, fd);
	(void) fwrite (buffer, sizeof (*buffer), numelmts, fdout);
    } while (numelmts == BUFSIZ);
    (void) fclose (4, fd);
    DBUG_VOID_RETURN;
}

#ifdef EXPR
/*
 *	The apparently superflous fseeks are there to satisfy
 *	the requirement that there be a repositioning call between
 *	fread/fwrite on the same stream.  Apparent a single fseek
 *	that does not move the pointer (fseek (fdexpr, 0L, 1)) does
 *	not satisfy the requirement, at least on a Sun-3 under SunOS 3.2.
 *
 *	Note that we are also called occasionally with a zero value'd
 *	syment when there really isn't any symbol referenced in the
 *	expression.  Sometimes the expressions involve only constant
 *	values, however we must still advance the current file pointer
 *	by reading and doing nothing with the expression.  We can however
 *	check to verify that when no symbol is referenced that syment
 *	is in fact 0.  If not, we have got out of sync somehow.
 *
 */

void pokesyment (syment)
long syment;
{
    char numops;
    char numconst;
    char numsyms;

    DBUG_ENTER ("pokesyment");
    DBUG_PRINT ("syment", ("syment = %ld", syment));
    (void) fseek (fdexpr, (long) 1, 1);
    (void) fseek (fdexpr, (long) -1, 1);
    DBUG_PRINT ("expr", ("file offset = %ld", ftell (fdexpr)));
    fread (&numops, sizeof (char), 1, fdexpr);
    fread (&numconst, sizeof (char), 1, fdexpr);
    fread (&numsyms, sizeof (char), 1, fdexpr);
    DBUG_PRINT ("expr", ("numops=%d numconst=%d numsyms=%d", numops, numconst, numsyms));
    DBUG_PRINT ("expr", ("file offset = %ld", ftell (fdexpr)));
    if (numsyms == 0 && syment != 0) {
	as_error (ERR_BUG, "no symbol in expression from .expr");
    }
    if (numops > 0) {
	DBUG_PRINT ("expr", ("skipping %d ops", numops));
	(void) fseek (fdexpr, (long) numops, 1);
	DBUG_PRINT ("expr", ("file offset = %ld", ftell (fdexpr)));
    } else {
	as_error (ERR_BUG, "missing OPS in .expr section!");
    }	
    if (numconst > 0) {
	if (numconst != 1) {
	    as_error (ERR_BUG, "bogus number of constants in .expr");
	} else {
	    DBUG_PRINT ("expr", ("skipping %d const", numconst));
	    (void) fseek (fdexpr, (long) (numconst * 4), 1);
	    DBUG_PRINT ("expr", ("file offset = %ld", ftell (fdexpr)));
	}
    }
    if (numsyms > 0) {
	if (numsyms != 1) {
	    as_error (ERR_BUG, "bogus number of symbols in .expr");
	} else {
	    DBUG_PRINT ("expr", ("write syment at %ld", ftell (fdexpr)));
	    (void) fseek (fdexpr, (long) 1, 1);
	    (void) fseek (fdexpr, (long) -1, 1);
#if clipper || ns32000
		syment = swapb4(syment);
#endif
	    (void) fwrite (&syment, sizeof (long), 1, fdexpr);
	}
    }
    DBUG_PRINT ("expr", ("file offset = %ld", ftell (fdexpr)));
    DBUG_VOID_RETURN;
}
#endif

/*
 *
 *	"reloc" is a function that reads preliminary relocation entries
 *	from the file whose descriptor appears in "fdrel", processes
 *	them to produce the final relocation entries, and writes them
 *	out to the file whose descriptor appears in "fdout".  It is
 *	passed the following parameter:
 *
 *	num	This gives the number of relocation entries to process.
 *		This is needed because the relocation entries for both
 *		the text section and the data section are written to
 *		the same intermediate file.  When final processing takes
 *		place, these relocation entries need to go into
 *		different places.  Hence the text section entries and
 *		the data section entries must be copied with different
 *		calls.
 *
 *	This function reads preliminary relocation entries in a loop.
 *	A preliminary relocation entry consists of an address, followed
 *	by a tab, followed by the symbol with respect to which the
 *	relocation is to take place, followed by a tab, followed by
 *	the relocation type. "getindx" is used to determine the
 *	symbol table index of the symbol to which the relocation is
 *	to take place.
 *
 */

#ifdef EXPR

void reloc (num)
register long num;
{
    RELOC relentry;
    prelent trelent;
    register long syment;
    register long exproff;

    DBUG_ENTER ("reloc");
    DBUG_PRINT ("rel", ("process and write out %ld relocation entries", num));
    while (num-- > 0L) {
	(void) fread ((char *) (&trelent), sizeof (prelent), 1, fdrel);
	relentry.r_vaddr = trelent.relval;
	relentry.r_type = (unsigned) trelent.reltype;
	DBUG_PRINT ("rel", ("vaddr = %#lx", relentry.r_vaddr));
	DBUG_PRINT ("rel", ("type = %d", relentry.r_type));
	if (trelent.relname != NULL && trelent.relname[0] == '=') {
	    /* The VAX case; formerly the "null name"  ==>	 */
	    /* -1 as symndx; now flagged by special char.	 */
	    relentry.r_symndx = -1L;
	} else if (trelent.relname != NULL && !*trelent.relname) {
	    /* A "null name" now implies a name in the	   */
	    /* table.  For that case, we need to use the	 */
	    /* offset into the string table found in the	 */
	    /* last four bytes of the name as a long offset. */
	    register int i;
	    union {
		long offsets[2];
		char dummy[8];
	    } kludge;
	    for (i = 0; i < 8; i++) {
		kludge.dummy[i] = trelent.relname[i];
	    }
	    if (((syment = getindx (&strtab[kludge.offsets[1]], C_EXT)) < 0L) &&
		    ((syment = getindx (&strtab[kludge.offsets[1]], C_STAT)) < 0L)) {
		as_error (ERR_RSNST);
	    }
	    if (exprhome == 0) {
		as_error (ERR_BUG, "no .expr section!");
		relentry.r_symndx = 0xFFFFFFFF;
	    } else {
		exproff = ftell (fdexpr) - exprhome;
		relentry.r_symndx = exproff;
		DBUG_PRINT ("sym", ("%s", &strtab[kludge.offsets[1]]));
		DBUG_PRINT ("expr", ("poke symbol offset %ld to expr at %ld", syment, exproff));
		pokesyment (syment);
	    }
	} else if (trelent.relname != NULL) {
	    DBUG_PRINT ("rel", ("symbol '%s'", trelent.relname));
	    if (((syment = getindx (trelent.relname, C_EXT)) < 0L) &&
		    ((syment = getindx (trelent.relname, C_STAT)) < 0L)) {
		as_error (ERR_RSNST);
	    }
	    DBUG_PRINT ("rel", ("symbol index = %d", syment));
	    if (exprhome == 0) {
		as_error (ERR_BUG, "no .expr section!");
		relentry.r_symndx = 0xFFFFFFFF;
	    } else {
		exproff = ftell (fdexpr) - exprhome;
		relentry.r_symndx = exproff;
		DBUG_PRINT ("expr", ("poke symindx to expr at %ld", exproff));
		DBUG_PRINT ("sym", ("%s", trelent.relname));
		pokesyment (syment);
	    }
	} else {
	    DBUG_PRINT ("rel", ("output relocation entry for constant only"));
	    if (exprhome == 0) {
		as_error (ERR_BUG, "no .expr section!");
		relentry.r_symndx = 0xFFFFFFFF;
	    } else {
		exproff = ftell (fdexpr) - exprhome;
		relentry.r_symndx = exproff;
		pokesyment (0L);
	    }
	}
	outblock (&relentry, RELSZ, fdout);
    }
    DBUG_VOID_RETURN;
}

#else

reloc(num)
	register long num;
{
	RELOC relentry;
	prelent trelent;
	long syment;

	while (num-- > 0L) {
		fread((char *)(&trelent),sizeof(prelent),1,fdrel);
		relentry.r_vaddr = trelent.relval;
		relentry.r_type = (unsigned)trelent.reltype;
		relentry.r_offset = trelent.reloffset;
		if (trelent.relname != NULL && trelent.relname[0] == '=') 
			/* The VAX case; formerly the "null name" ==>	*/
			/* -1 as symndx; now flagged by special char.	*/
{
			relentry.r_symndx = -1L;
}
		else if (trelent.relname != NULL && !*trelent.relname)
			/* A "null name" now implies a name in the	*/
			/* table.  For that case, we need to use the	*/
			/* offset into the string table found in the	*/
			/* last four bytes of the name as a long offset.*/
		{
			register int	i;
			union
			{
				long	offsets[2];
				char	dummy[8];
			} kludge;

			for (i = 0; i < 8; i++)
				kludge.dummy[i] = trelent.relname[i];
			if (((syment = getindx(&strtab[kludge.offsets[1]],C_EXT)) < 0L) &&
				((syment = getindx(&strtab[kludge.offsets[1]],C_STAT)) < 0L))
{
					as_error (ERR_RSNST);
}
			relentry.r_symndx = syment;

		}
		else if (trelent.relname != NULL) 
		{
			if (((syment = getindx(trelent.relname,C_EXT)) < 0L) &&
				((syment = getindx(trelent.relname,C_STAT)) < 0L))
{
				as_error (ERR_RSNST);
}
			relentry.r_symndx = syment;
		}
		outblock(&relentry, RELSZ, fdout);
	}
}
 
#endif

/*
 *
 *	"invent" is a function that invents a symbol table entry.  This
 *	is used for necessary entries for which no "def" appears.
 *	This includes the special symbols "text", "data", and "bss".
 *	It also includes all symbols declared "global" for which no
 *	"def" appears, and all undefined external symbols.  This
 *	function is passed the following parameters:
 *
 *	sym	This is a pointer to the symbol name.
 *
 *	val	This gives the value for the symbol.
 *
 *	sct	This gives the number of the section for the symbol.
 *
 *	scl	This gives the storage class of the symbol.
 *
 *	These values are put into the right format by storing into
 *	fields of a structure for the symbol table entry, and written
 *	to the file whose descriptor appears in "fdsymb" using
 *	"outblock".
 *
 */

void invent (tsym, val, sct, scl)
register symbol *tsym;
long val;
short sct;
short scl;
{
    SYMENT sment;
    AUXENT axent;
    register short index;	/* loop variable */

    DBUG_ENTER ("invent");
    if (tsym -> _name.tabentry.zeroes == 0) {
	sment.n_zeroes = 0L;
	sment.n_offset = tsym -> _name.tabentry.offset;
    } else {
	for (index = 0; (index < SYMNMLEN) && (sment.n_name[index] = tsym -> _name.name[index]); index++);
	for (; index < SYMNMLEN; index++) {
	    sment.n_name[index] = '\0';
	}
    }
    sment.n_value = val;
    sment.n_scnum = sct;
    sment.n_type = 0;
    sment.n_sclass = scl;
    sment.n_numaux = 0;
    sment.n_dummy = 0;
    if (transvec && sct && (tsym -> styp & TVDEF)) {
	sment.n_numaux = 1;
    }
    outblock (&sment, SYMESZ, fdsymb);
    putindx (tsym, scl, symbent);
    symbent++;
    if (sment.n_numaux) {
	axent.x_sym.x_tagndx = 0L;
#if MOTOROLA
 	axent.x_sym.x_pad1 = 0;
 	axent.x_sym.x_pad2 = 0;
#endif
	axent.x_sym.x_misc.x_lnsz.x_lnno = 0;
	axent.x_sym.x_misc.x_lnsz.x_size = 0;
	for (index = 0; index < DIMNUM; ++index) {
	    axent.x_sym.x_fcnary.x_ary.x_dimen[index] = 0;
	}
#if PRE_OCS
	axent.x_sym.x_tvndx = N_TV;
#endif
	outblock (&axent, AUXESZ, fdsymb);
	++symbent;
    }
    DBUG_VOID_RETURN;
}

/*
 *
 *	"outsyms" is a function used to create symbol table entries
 *	for all static, global, and undefined symbols for which no
 *	".def" appears in the source program. The name of this func-
 *	tion should be passed to "traverse" (in symbols.c) to call
 *	this function for each symbol table entry. This function
 *	first examines the first character of the symbol to see if
 *	it is an assembler defined identifier and ignores it if it
 *	is. Then the type of the symbol is checked to see if it is
 *	defined but not global and assumes the symbol to be a static.
 *	If this fails then the type is checked to see if it is both
 *	global and defined. For both static and global symbols,
 *	"getindx" is called to determine if an entry already exists
 *	for the symbol. If it does not, "invent" is called to create
 *	the entry in the object file symbol table. Finally the type
 *	is checked to see if the symbol is undefined and calls
 *	"invent" to create the entry.
 *
 */

void outsyms (ptr)
register symbol *ptr;
{
    register short sct;
    register char *strptr;

    DBUG_ENTER ("outsyms");
    if (ptr -> _name.tabentry.zeroes == 0) {
	strptr = &strtab[ptr -> _name.tabentry.offset];
    } else {
	strptr = ptr -> _name.name;
    }
    sct = (ptr -> styp & (~TVDEF));
    if ((sct > ABS) && (sct < EXTERN)) {		/* static symbols */
	/* ignore compiler generated labels */
	/*if (((strptr[0] != '.') && (strptr[0] != '(')) && ((strptr[0] != '@') || (strptr[1] != 'L'))) {*/
	if ((strptr[0] != '.') && (strptr[0] != '(')) {
	    if (getindx (strptr, C_STAT) < 0) {
		invent (ptr, ptr -> value, ptr -> sectnum, C_STAT);
	    }
	}
    } else {
	if (sct > EXTERN) {		/* global defined symbols */
	    if (getindx (strptr, C_EXT) < 0) {
		invent (ptr, ptr -> value, ptr -> sectnum, C_EXT);
	    }
	} else {
	    /* global undefined symbols */
	    if ((ptr -> styp == EXTERN) 
		|| (transvec && (ptr -> styp == (TVDEF | EXTERN)))) {
		    /* or tv defined symbols */
		    if (getindx (strptr, C_EXT) < 0) {
			undefcnt++;
			invent (ptr, ptr -> value, 0, C_EXT);
		    }
		}
	}
    }
    DBUG_VOID_RETURN;
}


/*
 *
 *	"symout" is the main routine for creating symbol table entries
 *	for which no "def"s appear in the source program.  At the time
 *	this function is called, the file whose descriptor appears in
 *	"fdsymb" should contain all entries from "def"s appearing in
 *	the text section, followed by all entries from "def"s for
 *	static symbols appearing in the data section.  The file whose
 *	descriptor appears in "fdgsymb" should contain all other "def"s
 *	from the data section.  "fdsymb" should be open for writing,
 *	and "fdgsymb" should be open for reading.  When this function
 *	returns, "fdsymb" will contain the entire symbol table.
 *
 *	This function first creates entries for the special section
 *	name symbols. These are defined as
 *	external static symbols to avoid conflict with similar defin-
 *	itions in other files.  The entries for these symbols are
 *	written to "fdsymb".
 *
 *	Following this, the file "fdgsymb" is copied to the end of
 *	"fdsymb".  This process insures that all global symbols follow
 *	all external static symbols.  The entries are examined as they
 *	are copied from one file to another, so that the correct indices
 *	for their positions in the symbol table can be determined.
 *
 *	The function "traverse" (from symbols.c) is called passing the
 *	function "outsyms" to create entries for all symbols declared
 *	"global" that have no "def" for them, any static symbols
 *	(user defined static symbols are generated only if a "file"
 *	entry has been produced), and all global undefined symbols.
 *	See the comments for "outsyms" for how this works."
 *
 */

symout ()
{
    register short index;
    register symbol *sym;
    register struct scnhdr *sect;
    char *symname;
    static codebuf statcbuf = {
	(long) C_STAT, 0, 0, 0
    };
    static codebuf nullcbuf = {
	0L, 0, 0, 0
    };

    DBUG_ENTER ("symout");
    dot -> styp = secdat[1].s_typ;
    dot -> sectnum = 1;

    /* 
     * define section name symbols in storage class C_STAT
     */
    for (index = 1, sect = &sectab[1]; index <= seccnt; index++, sect++) {
	sym = (*lookup (sect -> s_name, N_INSTALL, USRNAME)).stp;
	nullcbuf.cvalue = 0;
	define (sym, &nullcbuf);
	setval (sym, &nullcbuf);
	setscl (NULLSYM, &statcbuf);/* Storage Class C_STAT */
	dfaxent (sect -> s_size, sect -> s_nreloc, sect -> s_nlnno);
	endef (NULLSYM, &nullcbuf);
    }

    for (index = 0; index < gsymbent; index++) {
	/* We need a null-terminated name string to pass to	 */
	/* getindx.  If the name just fits within the eight	 */
	/* character name space of the syment, then that won't	 */
	/* work as the string.  This kludge gets around that	 */
	/* (efficiently) by copying the name to a location	 */
	/* big enough to handle the eight characters and a	 */
	/* null.  It is used whenever the name is in the	 */
	/* syment, rather than the string table.  WARNING!	 */
	/* Note that this kludge depends on the long == 4 char	 */
	/* identity currently found on all our machines.	 */

	union {
	    long l[3];
	    char c[12];
	} kludge;
	SYMENT sment;
	(void) inblock (&sment, SYMESZ, fdgsymb);
	outblock (&sment, SYMESZ, fdsymb);
	if (sment.n_zeroes != 0) {
	    /* Copy the name to a place that has room for a null. */
	    kludge.l[0] = sment.n_zeroes;
	    kludge.l[1] = sment.n_offset;
	    kludge.c[8] = '\0';
	    symname = kludge.c;
	} else {
	    symname = &strtab[sment.n_offset];
	}
	sym = (*lookup (symname, N_INSTALL, USRNAME)).stp;
	if (sym == NULLSYM) {
	    as_error (ERR_USST);
	}
	putindx (sym, sment.n_sclass, symbent);
	symbent++;
	/* Warning, following non-portable comparison assumes signed char's */
	if (sment.n_numaux > 0) {
	    AUXENT axent;
	    (void) inblock (&axent, AUXESZ, fdgsymb);
	    outblock (&axent, AUXESZ, fdsymb);
	    symbent++;
	    index++;
	}
    }
    traverse ((int (*)()) outsyms);
    DBUG_VOID_RETURN;
}

/*
 *
 *	"push", "pop", and "setsym" are procedures for producing
 *	the forward pointers for elements of the symbol table.
 *	"push" and "pop" maintain a stack that contains the symbol
 *	table index of the symbol that is receive the forward pointer
 *	and the symbol table index that is to become the forward
 *	pointer. "setsym" is called by "xform" (in addr.c) whenever
 *	a symbol is detected that can receive a forward pointer.
 *	It then determines the symbol table index of the auxiliary
 *	entry that is to receive the forward pointer and call "push"
 *	to enter that information onto the stack. "pop" is called
 *	by "xform" whenever the matching symbollic debugging entry
 *	is found to enter the current index plus one as the forward
 *	index.
 *
 */

static push (value)
register stent *value;
{
    DBUG_ENTER ("push");
    if (sttop == MAXSTK - 1) {
	as_error (ERR_STSOVF);
    }
    stack[sttop++] = value;
    DBUG_VOID_RETURN;
}

stent *pop ()
{
    register stent *rtnval = (stent *)NULL;

    DBUG_ENTER ("pop");
    if (sttop == 0) {
	as_error (ERR_USTE3);
    } else {
	rtnval = stack[--sttop];
    }
    DBUG_RETURN (rtnval);
}

void setsym (initval)
short initval;
{
    DBUG_ENTER ("setsym");
    currsym -> stindex = symbent + 1L;	/* point to auxiliary entry */
    /* -1 indicates this isn't a function entry */
    currsym -> fcnlen = (long) initval;
    if (symtail != currsym) {
	symtail -> stnext = currsym;
    }
    symtail = currsym;
    currsym = (stent *) calloc (1, sizeof (stent));
    push (symtail);
    DBUG_VOID_RETURN;
}

/*
 *
 *	"fixst" is a procedure to fix the symbol table by entering
 *	the forward symbol table indices into the auxiliary entries
 *	of entries that denote the beginning of a logical scope
 *	block. It does this by using "ftell" to determine the present
 *	location in the symbol table, then using the symbol table
 *	element stack (pointed to by symhead) fixst seeks to the
 *	auxiliary entry, and writes the forward symbol table index
 *	found in the stack entry. When fixst is finished, i.e. the
 *	stack is empty, it returns to the original position in the
 *	object file.
 *
 */

void fixst (fd)
register FILE *fd;
{
    register stent *symptr;
    long home;
    AUXENT axent;		/* dummy structure for address calculation */
    register long where;

    DBUG_ENTER ("fixst");
    symptr = symhead;
    if (symptr -> stindex > 0L) {
	home = ftell (fd);
	while (symptr != NULL) {
	    if (symptr -> fcnlen >= 0) {
		/* 
		 * a negative value indicates this isn't a
		 * function entry, so don't overwrite
		 * that field of the aux entry
		 */
		where = (symptr -> stindex * SYMESZ);
		/* add in offset from beginning of structure */
		where += ((char *) &axent.x_sym.x_misc.x_fsize - (char *) &axent);
		(void) fseek (fd, where, 0);
		(void) fwrite ((char *) (&(symptr -> fcnlen)),
			sizeof (symptr -> fcnlen), 1, fd);
	    }
	    where = (symptr -> stindex * SYMESZ);
	    where += ((char *) &axent.x_sym.x_fcnary.x_fcn.x_endndx
		    - (char *) &axent);
	    (void) fseek (fd, where, 0);
	    (void) fwrite ((char *) (&(symptr -> fwdindex)), 
			   sizeof (symptr -> fwdindex), 1, fd);
	    symptr = symptr -> stnext;
	}
	(void) fseek (fd, home, 0);
    }
    DBUG_VOID_RETURN;
}
