/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "@(#) $Header: unused.c 6.5 88/09/21 16:28:46 root Exp $ RISC Source Base"

#if AR32W || AR32WR
/*
 * The routines in this file implement an algorithm
 * to removed unused symbols from a COFF symbol table.
 * Unused symbols are defined to be structure, union,
 * enumeration tags, and typedefs which are not referernced
 * by any other symbol table entry.
 *
 * The algorithm is two pass where the first pass collects
 * data about the symbols and the second pass removes the
 * unused symbols and writes out the reduced symbol table.
 * The file header, relocation entries, and line number entries
 * are adjusted appropriately
*/

#include <stdio.h>
#include "symbols2.h"
#include "filehdr.h"
#include "linenum.h"
#include "reloc.h"
#include "scnhdr.h"
#include "syms.h"
#include "dbug.h"

#define TABLESZE	NSYMS
#define ENDNDX		x_sym.x_fcnary.x_fcn.x_endndx
#define TAGNDX		x_sym.x_tagndx
#define LNNOPTR		x_sym.x_fcnary.x_fcn.x_lnnoptr
#define ITEM		struct tag_data
#define LINK		struct link_data
#define EQUAL(a,b,l)	(strncmp((a),(b),(l)) == 0)
#define ERROR		(-1000)
#define ALG_ERR		(-100)

/*
 * LINKs are used to link tag ITEMs
 * which are referenced by tag members
 * to the referencing tag
 */

LINK {
    LINK *l_next;
    ITEM *l_link;
};

/*
 * ITEMs store the basic information needed
 * for each potential unused symbol
 */

ITEM {
    long t_index;
    long t_new_index;
    LINK *t_link;
    unsigned int t_refd:1;
    int t_size:15;
};
#ifdef EXPR
static ITEM *elookup ();
#else
static ITEM *lookup ();
#endif

extern short uflag;				/* Flags we need to access R.I. */
/*extern short striplocstat; */

/*
 * unused is the only entry point for this file.
 * The return values from unused indicate the
 * following:
 *		ret == ERROR	hard assembler error (system call,COFF bug,etc)
 *		ret == ALG_ERR  0	algorithm error
 *		ret == 0	no unused symbols (yes, miracles do happen)
 *		ret >  0	everything is OK
 *
 * Inputs are the pathname for the symbol table file (symbols)
 */

int unused (symbols, output)
char *symbols;
char *output;
{
    extern int analysis ();
    extern int reduce ();
    int ret;

    DBUG_ENTER ("unused");
    if ((ret = analysis (symbols)) > 0) {
	ret = reduce (symbols, output);
    }
    DBUG_RETURN (ret);
}

/*
 * anaylsis collects all the necessary information
 * about tags.  Input is the pathname to the
 * file containing the symbol table
 *
 * The algorithm is as follows:
 *	initialize data structure
 *	open input for reading
 *	for each symbol table entry in the input
 *		if aux then read in aux entry
 *		switch on storage class of symbol
 *			tag:
 *				insert into data structure as unreferenced
 *				remember tag inserted in case links are needed
 *				break
 *			typdef:
 *				insert into data structure
 *				break
 *			static:
 *			extern:
 *				insert into data structure as referenced
 *				if static and .text or .data or .bss
 *					do not process aux entry (break)
 *			C_variable:
 *				if aux entry exists then check tagndx
 *					lookup tagndx and set to referenced
 *					set all linked tags to referenced
 *				break
 *			tag_member:
 *				if aux entry exists then check tagndx
 *					link the lookup tag to the current tag
 *				break
 *			eos:
 *				forget current tag index
 *				break
 *		end switch
 *		read through all aux entries (increment index)
 *	end for
 *	close input
 *	if unreferenced tags
 *		indicate further work to be done
 */

static analysis (symbols)
char *symbols;
{
    /* EXTERNS */
    extern short insert ();
    extern void set_refd ();
    extern int fread ();
    extern int link ();
    extern int markup ();
    extern int setup ();
    extern int issect ();
    extern FILE *fopen ();
    extern SCNHDR sectab[]; /* For L% fix R.I. */
	

    /* AUTOMATICS */
    FILE *fd;			/* symbol table file descriptor */
    SYMENT sym;			/* symbol table entry */
    AUXENT aux;			/* auxilary symbol table entry */
    int i;			/* loop controller */
    long tag = -1;		/* tag rememberer */
    register long index;	/* symbol table index */
    register ITEM *entry;	/* lookup ITEM pointer */

    DBUG_ENTER ("analysis");
    if (setup () < 0) {
	DBUG_RETURN (ERROR);
    }
    if ((fd = fopen (symbols, "r")) == NULL) {
	DBUG_RETURN (ERROR);
    }
    for (index = 0; fread ((char *) & sym, SYMESZ, 1, fd) == 1; index++) {
	if (sym.n_numaux != 0) {
	    if (fread ((char *) & aux, AUXESZ, 1, fd) != 1) {
		DBUG_RETURN (ERROR);
	    }
	}
	switch ((int) sym.n_sclass) {
	    case C_STRTAG: 
	    case C_UNTAG: 
	    case C_ENTAG: 
		tag = insert (index, (long) (aux.ENDNDX - index), 0);
		if (tag < 0) {
		    DBUG_RETURN (ALG_ERR);
		}
		break;
	    case C_TPDEF: 
		if (insert (index, (long) (sym.n_numaux + 1), 0) < 0) {
		    DBUG_RETURN (ALG_ERR);
		}
		break;
	    case C_STAT: 
	    case C_EXT: 
	/*
 	 * If striplocstat is set we mark local statics as unreferenced 
	 * provided there are no relocation entries for the symbol R.I.
	 */
		if(/* striplocstat && */ (sym.n_sclass == C_STAT) &&
				( ((sym.n_name[0] == 'L') &&
				(sym.n_name[1] == '%')) ||
				((sym.n_name[0] == '@') &&
				(sym.n_name[1] == 'L')) ) &&
				(strcmp(sectab[sym.n_scnum].s_name,_TEXT) == 0) ){
			DBUG_PRINT("info",("Marking %s as unused",sym.n_name));
			if (insert (index, (long)(sym.n_numaux + 1), 0) < 0) {
		    	DBUG_RETURN (ALG_ERR);
			}
		} else {
		/* R.I. */
			if (insert (index, 0L, 1) < 0) {
		    	DBUG_RETURN (ALG_ERR);
			}
			if ((sym.n_sclass == C_STAT) && (sym.n_zeroes != 0L) &&
				issect (sym.n_name)) {
		    	break;
			}
		} /* R.I. */
	    case C_AUTO: 
	    case C_REG: 
	    case C_ARG: 
	    case C_REGPARM: 
		if (sym.n_numaux == 0) {
		    break;
		} else {
		    /* 0 == the NULL tag pointer but * could be legal if
		       the first entry * is not a .file!! */
#ifdef EXPR
		    if ((entry = elookup (aux.TAGNDX)) != NULL) {
#else
		    if ((entry = lookup (aux.TAGNDX)) != NULL) {
#endif
			if (entry -> t_refd != 1) {
			    entry -> t_refd = 1;
			    set_refd (entry -> t_link);
			}	/* else it was already marked */
		    } else if (aux.TAGNDX != 0) {
			DBUG_RETURN (ALG_ERR);
		    }
		}
		break;
	    case C_MOS: 
	    case C_MOU: 
	    case C_FIELD: 
		if (sym.n_numaux == 0) {
		    break;
		} else {
#ifdef EXPR
		    if ((entry = elookup (aux.TAGNDX)) != NULL) {
#else
		    if ((entry = lookup (aux.TAGNDX)) != NULL) {
#endif
			if (link ((short) tag, entry) < 0) {
			    DBUG_RETURN (ALG_ERR);
			}
		    } else if (aux.TAGNDX != 0) {
			DBUG_RETURN (ALG_ERR);
		    }
		}
		break;
	    case C_EOS: 
		tag = -1;
	    default: 
		break;
	}
	if (sym.n_numaux != 0) {
	    index++;
	    for (i = 1; i < sym.n_numaux; i++) {
		index++;
		(void) fread ((char *) & aux, AUXESZ, 1, fd);
	    }
	}
    }
    if (ferror (fd) != 0) {
	DBUG_RETURN (ERROR);
    }
    (void) fclose (14, fd);
	touch_relocs();				/* Mark any relocs as referenced */
    DBUG_RETURN (markup ());
}

/*
 * reduce produces the final object file
 * Input is the pathname to the symbol table
 * file and the ultimate object file
 *
 * algorithm:
 *	open the input files
 *	read object file file header
 *	seek to symbol table in object file
 *	set symbol entry remove counter to 0
 *	for each symbol in the symbol table file
 *		lookup current symbol
 *		if lookup successful and symbol is not referenced
 *			increment removed count
 *			remove symbols
 *		else
 *			write out symbol to object file
 *			if aux entry exists
 *				if symbol is a function
 *					remember file offset
 *					remember inside of function
 *				read aux
 *			switch on storage class
 *				tag:
 *					adjust aux.endndx
 *					break
 *				static:
 *					if name of section
 *						do nothing - break
 *				C_variables:
 *				tag_member:
 *					if aux entry
 *						lookup aux.tagndx symbol
 *						if successful
 *							adjust aux.tagndx
 *						if symbol is a function
 *							save aux entry
 *							save removed count
 *							adjust aux.endndx
 *					break
 *				eos:
 *					adjust aux.endndx
 *					break
 *				.ef:
 *					if not inside of function
 *						error
 *					else if symbols removed from inside fcn
 *						remember current file offset
 *						adjust saved aux entry
 *						seek to function aux offset
 *						write saved aux
 *						seek back to current offset
 *						mark outside of function
 *			end switch
 *			if aux entry exists
 *				write entry to object file
 *				process additional aux entries
 *	end for
 *	adjust object file sections
 *	close input
 */

static reduce (symbols, output)
char *symbols;
char *output;
{
    /* EXTERNS */
#ifdef EXPR
    static ITEM *elookup ();
#else
    static ITEM *lookup ();
#endif
    extern long ftell ();
    extern int fread ();
    extern int fseek ();
    extern int fwrite ();
    extern int issect ();
    extern  FILE *fopen ();
    extern  FILE *fdout;
    extern long sect_lnnoptrs[];
    extern short dlflag;

    /* AUTOMATICS */
    FILE *fd;			/* symbol table file descriptor */
    FILE *fdtmp;		/* line number entry file descripter */
    register long index;	/* symbol table index */
    register long i;		/* loop counter */
    register long removed;	/* removed symbol entry counter */
    register ITEM *entry;	/* lookup symbol pointer */
    int in_fcn = 0;		/* inside function flag */
    long f_offset;		/* file offset for function aux entry */
    long f_removed;		/* # entries removed at start of fcn */
    SYMENT sym;			/* symbol table entry */
    AUXENT aux;			/* aux entry */
    AUXENT f_aux;		/* saved function aux entry */

    DBUG_ENTER ("reduce");
    if ((fd = fopen (symbols, "r")) == NULL) {
	DBUG_RETURN (ERROR);
    }
    if ((fdtmp = fopen (output, "r+")) == NULL) {
	DBUG_RETURN (ERROR);
    }
    removed = 0;
    for (index = 0; fread ((char *) & sym, SYMESZ, 1, fd) == 1; index++) {
#ifdef EXPR
	if (((entry = elookup (index)) != NULL) && (entry -> t_refd == 0)) {
#else
	if (((entry = lookup (index)) != NULL) && (entry -> t_refd == 0)) {
#endif
	    removed += entry -> t_size;
	    for (i = 1; i < entry -> t_size; i++) {
		index++;
		(void) fread ((char *) & sym, SYMESZ, 1, fd);
	    }
	} else {
	    (void) fwrite ((char *) & sym, SYMESZ, 1, fdout);
	    if (sym.n_numaux != 0) {
		if (ISFCN (sym.n_type) 
			&& (sym.n_sclass == C_EXT
			|| sym.n_sclass == C_STAT)) {
		    in_fcn = 1;
		    f_offset = ftell (fdout);
		}
		(void) fread ((char *) & aux, AUXESZ, 1, fd);
	    }
	    switch ((int) sym.n_sclass) {
		case C_STRTAG: 
		case C_UNTAG: 
		case C_ENTAG: 
		    aux.ENDNDX -= removed;
		    break;
		case C_STAT: 
		    if ((sym.n_sclass == C_STAT) &&
			    (sym.n_zeroes != 0L) &&
			    issect (sym.n_name)) {
			break;
		    }
		case C_EXT: 
		case C_AUTO: 
		case C_REG: 
		case C_ARG: 
		case C_REGPARM: 
		case C_MOS: 
		case C_MOU: 
		case C_FIELD: 
		    if (sym.n_numaux != 0) {
#ifdef EXPR
			if ((entry = elookup (aux.TAGNDX)) != NULL) {
#else
			if ((entry = lookup (aux.TAGNDX)) != NULL) {
#endif
			    aux.TAGNDX = entry -> t_new_index;
			} else if (aux.TAGNDX != 0) {
			    DBUG_RETURN (ALG_ERR);
			}
			if (ISFCN (sym.n_type)
				&& (sym.n_sclass == C_EXT || sym.n_sclass == C_STAT)) {
			    long symndx;

		DBUG_PRINT("info",("Adjust code for sym %s",sym.n_name));
		DBUG_PRINT("info",("Section 0x%x, Type 0x%x",sym.n_scnum,sym.n_type));
			    f_aux = aux;
			    f_removed = removed;
			    aux.ENDNDX -= removed;
			    if (!dlflag) {
				/*
				 * Nooo! not (-1)! R.I.
		** aux.LNNOPTR += sect_lnnoptrs[sym.n_scnum - 1]; **
				 */
				aux.LNNOPTR += sect_lnnoptrs[sym.n_scnum];
				if (fseek (fdtmp, aux.LNNOPTR, 0) != 0) {
				    DBUG_RETURN (ERROR);
				}
				symndx = index - removed;
				(void) fwrite ((char *) & symndx, sizeof (long), 1, fdtmp);
			    }
			}
		    }
		    break;
		case C_EOS: 
		    aux.TAGNDX -= removed;
		    break;
		case C_FCN: 
		    if ((sym.n_zeroes != 0L) &&
			    EQUAL (sym.n_name, ".ef", SYMNMLEN)) {
			if (in_fcn == 0) {
			    DBUG_RETURN (ERROR);
			} else if (f_removed != removed) {
			    f_aux.ENDNDX -= removed;
			    if (fseek (fdtmp, f_offset, 0) != 0) {
				DBUG_RETURN (ERROR);
			    }
			    (void) fwrite ((char *) &f_aux, AUXESZ, 1, fdtmp);
			    in_fcn = 0;
			}
		    }
		    break;
		default: 
		    break;
	    }
	    if (sym.n_numaux != 0) {
		index++;
		(void) fwrite ((char *) & aux, AUXESZ, 1, fdout);
		for (i = 1; i < sym.n_numaux; i++) {
		    index++;
		    (void) fread ((char *) & aux, AUXESZ, 1, fd);
		    (void) fwrite ((char *) & aux, AUXESZ, 1, fdout);
		}
	    }
	}
    }
    if ((ferror (fd) != 0) || (ferror (fdtmp) != 0)) {
	DBUG_RETURN (ERROR);
    }
    (void) fclose (15,fd);
    (void) fclose (16, fdtmp);
    if (adjust_obj ((long) (index - removed)) < 0) {
	DBUG_RETURN (ERROR);
    }
    DBUG_RETURN (1);
}

/*
 * adjust_obj adjusts the relocation, line number,
 * and file header info in the ultimate object file
 * Input is the object file file descriptor and the
 * new symbol count
 *
 * algorithm:
 *	read file header
 *	adjust number of symbols entry
 *	write file header
 *	seek to the section headers
 *	for each section
 *		if line numbers
 *			for each line number entry
 *				if line number == 0
 *					adjust line number (symbol table index)
 *				write out line number entry
 *			end for
 *		if relocation
 *			for each reloc entry
 *				adjust symbol index
 *				write reloc entry
 *			end for
 *	end for
 */

static adjust_obj (nsyms)
long nsyms;
{
    /* EXTERNS */
#ifdef EXPR
    static ITEM *elookup ();
#else
    static ITEM *lookup ();
#endif
    extern int fseek ();
    extern int fwrite ();
    extern FILE *fdout;
    extern int seccnt;
    extern SCNHDR sectab[];

    /* AUTOMATICS */
    FILHDR filhdr;
    RELOC reloc;		/* relocation entry */
    ITEM *entry;		/* lookup ITEM pointer */
    register SCNHDR *scnptr;	/* loop controller */
    register unsigned short i;
    register unsigned long n;	/* loop controller */ /* Was ushort R.I. */

    DBUG_ENTER ("adjust_obj");
    if (fseek (fdout, 0L, 0) != 0) {
	DBUG_RETURN (ERROR);
    }
    if (fread ((char *) & filhdr, FILHSZ, 1, fdout) != 1) {
	DBUG_RETURN (ERROR);
    }
    filhdr.f_nsyms = nsyms;
    if (fseek (fdout, 0L, 0) != 0) {
	DBUG_RETURN (ERROR);
    }
    (void) fwrite ((char *) & filhdr, FILHSZ, 1, fdout);

    /* 
     * The optional header always has zero
     * length during an assembly
     */

    if (fseek (fdout, (long) FILHSZ, 0) != 0) {
	DBUG_RETURN (ERROR);
    }
	/* OLD line was......
	if (fread ((char *) sectab, SCNHSZ, seccnt, fdout) != seccnt) {
	this seems to be wrong, I made it.... R.I. */
    if (fread ((char *) &sectab[1], SCNHSZ, seccnt, fdout) != seccnt) {
	DBUG_RETURN (ERROR);
    }
    for (scnptr = (&sectab[1]); scnptr <= &sectab[seccnt]; scnptr++) {
	DBUG_PRINT("info",("Adjusting %s",scnptr->s_name) );
	if (n = scnptr -> s_relptr) {
	    for (i = 0; i < scnptr -> s_nreloc; i++, n += RELSZ) {
		if (fseek (fdout, n, 0) != 0) {
		    DBUG_RETURN (ERROR);
		}
		if (fread ((char *) & reloc, RELSZ, 1, fdout) != 1) {
		    DBUG_RETURN (ERROR);
		}
#ifdef EXPR
		if ((entry = elookup (reloc.r_symndx)) == NULL) {
#else
		if ((entry = lookup (reloc.r_symndx)) == NULL) {
#endif
		    DBUG_RETURN (ERROR);
		}
		DBUG_PRINT("info",("Old reloc = 0x%x 0x%x",
			reloc.r_vaddr,reloc.r_symndx) );
		reloc.r_symndx = entry -> t_new_index;
		if (fseek (fdout, (long) n, 0) != 0) {
		    DBUG_RETURN (ERROR);
		}
		DBUG_PRINT("info",("New reloc = 0x%x 0x%x",
			reloc.r_vaddr,reloc.r_symndx) );
		(void) fwrite ((char *) & reloc, RELSZ, 1, fdout);
	    }
	}
    }
    DBUG_RETURN (1);
}

/*
 * The following routines implement the
 * symbol manipulating routines
 * Most of the details were hidden from
 * the above programs to allow for future
 * improvements
 */

static ITEM *table;		/* master table of ITEMs */
static long entries = 0;	/* number of entries in table */

/*
 * markup calculates the new indices for
 * all table entries that are not removed
 */

static markup () 
{
    extern ITEM *table;
    extern long entries;
    register ITEM *entry;
    register int i;
    long removed;

    DBUG_ENTER ("markup");
    entry = table;
    removed = 0;
    for (i = 0; i < entries; i++) {
	if (entry -> t_refd == 0) {
	    removed += entry -> t_size;
	} else {
	    entry -> t_new_index = entry -> t_index - removed;
	}
	entry++;
    }
	DBUG_PRINT("info",("markup found %d syms unused",removed));
    DBUG_RETURN (removed);
}

/*
 * Touch_relocs was added to double check that all symbols
 * needed by relocation entries are marked as 'used' and not
 * dropped by reduce(). The code is basically the same loop
 * that adjust_obj() uses. R.I.
 */
touch_relocs()
{
    /* EXTERNS */
#ifdef EXPR
    static ITEM *elookup ();
#else
    static ITEM *lookup ();
#endif
    extern int fseek ();
    extern int fwrite ();
    extern FILE *fdout;
    extern int seccnt;
    extern SCNHDR sectab[];

    /* AUTOMATICS */
    FILHDR filhdr;
    RELOC reloc;		/* relocation entry */
    ITEM *entry;		/* lookup ITEM pointer */
    register SCNHDR *scnptr;	/* loop controller */
    register unsigned short i;
    register unsigned long n;	/* loop controller */ /* Was ushort R.I. */
	long savloc;

    DBUG_ENTER ("touch_relocs");
    savloc = ftell(fdout);
    if (fseek (fdout, 0L, 0) != 0) {
		DBUG_RETURN (ERROR);
    }
    if (fread ((char *) &filhdr, FILHSZ, 1, fdout) != 1) {
		DBUG_RETURN (ERROR);
    }
    if (fread ((char *) &sectab[1], SCNHSZ, seccnt, fdout) != seccnt) {
		DBUG_RETURN (ERROR);
    }
    for (scnptr = (&sectab[1]); scnptr <= &sectab[seccnt]; scnptr++) {
	DBUG_PRINT("info",("Touching reloc syments for %s",scnptr->s_name) );
	if (n = scnptr -> s_relptr) {
	    for (i = 0; i < scnptr -> s_nreloc; i++, n += RELSZ) {
			if (fseek (fdout, n, 0) != 0) {
		    	DBUG_RETURN (ERROR);
			}
			if (fread ((char *) & reloc, RELSZ, 1, fdout) != 1) {
		    	DBUG_RETURN (ERROR);
			}
#ifdef EXPR
			if ((entry = elookup (reloc.r_symndx)) != NULL) 
#else
			if ((entry = lookup (reloc.r_symndx)) != NULL) 
#endif
			{
				if( entry->t_refd == 0 ){
			    		entry -> t_refd = 1;
			    		set_refd (entry -> t_link);
	DBUG_PRINT("info",("Reclaimed symbol %d as reloc symb",entry->t_index));
				}
			}
    		}
		}
	}
    if (fseek (fdout, savloc, 0) != 0) {
	DBUG_RETURN (ERROR);
    }
    DBUG_RETURN (1);
}

/*
 * setup allocates the incore instance
 * of table
 */

static setup ()
{
    extern ITEM *table;
    extern char *calloc ();

    DBUG_ENTER ("setup");
    if ((table = (ITEM *) calloc (TABLESZE, sizeof (*table))) == NULL) {
	DBUG_RETURN (-1);
    }
    DBUG_RETURN (1);
}

/*
 * insert puts an entry into the table
 * statics and externs are inserted with
 * the refd bit set so that markup wil
 * not remove them
 */

static short insert (index, size, refd)
long index;
long size;
int refd;
{
    extern ITEM *table;
    extern long entries;
    register ITEM *next;

    DBUG_ENTER ("insert");
    if (entries < TABLESZE) {
	next = table + entries;
	next -> t_index = index;
	next -> t_new_index = 0;
	next -> t_link = NULL;
	next -> t_refd = refd;
	next -> t_size = size;
	next++;
	DBUG_RETURN (entries++);
    }
    DBUG_RETURN ((short) (-1));
}

#ifdef EXPR
/*
 * elookup uses binary search to find the
 * desired entry.  This algorithm can be
 * used since symbol table indices are
 * monotonically nondecreasing
 */

static ITEM *elookup (index)
long index;
{
    extern ITEM *table;
    extern long entries;
    register ITEM *start;
    register ITEM *end;
    register ITEM *mid;

    DBUG_ENTER ("elookup");
    if (entries > 0) {
	start = table;
	end = table + (entries - 1);
	while (start <= end) {
	    mid = ((end - start) / 2) + start;
	    if (index < mid -> t_index) {
		end = mid - 1;
	    } else if (index > mid -> t_index) {
		start = mid + 1;
	    } else {
		DBUG_RETURN (mid);
	    }
	}
    }
    DBUG_RETURN ((ITEM *)NULL);
}
#else
static ITEM*
lookup(index)
	long 	index;
{
	extern ITEM	*table;
	extern long	entries;
	register ITEM	*start,
			*end,
			*mid;

	if (entries > 0)
	{
		start = table;
		end = table + (entries-1);
		while(start <= end)
		{
			mid = ((end - start) / 2) + start;
			if (index < mid->t_index)
				end = mid - 1;
			else if (index > mid->t_index)
				start = mid + 1;
			else
				return(mid);
		}
	}
	return(NULL);
}
#endif

/*
 * link creates a linked list of
 * tags which are members of the
 * linker
 *
 * the special case of a tag being
 * recursively referenced by one of
 * its members is handled by not
 * creating a link
 */

static link (linker, linkee)
short linker;
ITEM *linkee;
{
    extern ITEM *table;
    extern char *calloc ();
    register LINK **ptr;

    DBUG_ENTER ("link");
    if ((linker < 0) || (linker >= entries)) {
	DBUG_RETURN (-1);
    }
    if ((table + linker) != linkee) {
	for (ptr = (LINK **) & (table + linker) -> t_link; *ptr != NULL; ptr = (LINK **) & (*ptr) -> l_next) {;}
	if ((*ptr = (LINK *) calloc (1, sizeof (**ptr))) == NULL) {
	    DBUG_RETURN (-2);
	}
	(*ptr) -> l_next = NULL;
	(*ptr) -> l_link = linkee;
    } /* else it is a recursive reference */
    DBUG_RETURN (1);
}

/*
 * set_refd sets the referenced bit then
 * recursively set the referenced bit in
 * all ITEMs linked to the input ITEM
 */

static void set_refd (entry)
LINK *entry;
{
    register LINK *ptr;

    DBUG_ENTER ("set_refd");
    for (ptr = entry; ptr != NULL; ptr = ptr -> l_next) {
	ptr -> l_link -> t_refd = 1;
	set_refd (ptr -> l_link -> t_link);
    }
    DBUG_VOID_RETURN;
}

static int issect (s)
register char *s;
{
    /* EXTERNS */
    extern int seccnt;
    extern SCNHDR sectab[];
    register int i;

    DBUG_ENTER ("issect");
    for (i = 1; i <= seccnt; i++) {
	if (EQUAL (sectab[i].s_name, s, SYMNMLEN)) {
	    DBUG_RETURN (1);
	}
    }
    DBUG_RETURN (0);
}

#endif	/* AR32W || AR32WR */
