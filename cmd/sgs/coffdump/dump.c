#ident "@(#) $Header: dump.c 4.1 88/04/26 17:02:47 root Exp $ RISC Source Base"
#include <stdio.h>
#include <a.out.h>

/*
 *	On System V machines, <a.out.h> automatically includes the
 *	following files.  If this is a BSD host instead, then 
 *	explicitly include them and hope that our include path
 *	points somewhere where they will be found.
 */

#ifndef FILHDR
#include "filehdr.h"
#include "aouthdr.h"
#include "scnhdr.h"
#include "reloc.h"
#include "linenum.h"
#undef N_ABS			/* Incompatibly defined in BSD's a.out.h */
#include "syms.h"
#endif

#include <ar.h>
#include <ldfcn.h>

#include "coffdump.h"

extern CHAR *ctime ();

static VOID dump_arhdr ();	/* Dump archive header for module */
static VOID dump_filehdr ();	/* Dump file header */
static VOID dump_auxhdr ();	/* Dump the Unix header */
static VOID dump_allsec ();	/* Dump each section in turn */
static VOID dump_shdr ();	/* Dump the section headers */
static VOID dump_sdata ();	/* Dump a section in "raw" format */
static VOID dump_sreloc ();	/* Dump section relocation */
static VOID dump_lnno ();	/* Dump line numbers */
static VOID dump_syms ();	/* Dump symbol table */
static VOID PrtSyment ();	/* Print a normal symbol table entry */

extern int AuxType ();		/* Return auxiliary type for syment */
extern VOID PrtAuxent ();	/* Print an auxiliary entry */
extern CHAR *TransReloc ();	/* Translate relocation type to symbolic */
extern CHAR *TransMagic ();	/* Translate f_magic to symbolic */
extern CHAR *TransFlags ();	/* Translate f_flags to symbolic */
extern CHAR *TransSflags ();	/* Translate s_flags to symbolic */
extern CHAR *TransSections ();	/* Translate section numbers */
extern CHAR *TransClasses ();	/* Translate storage classes */
extern CHAR *LongTuple ();	/* Build decimal/octal/hex ascii string */
extern CHAR *IntTuple ();	/* Build decimal/octal/hex ascii string */
extern CHAR *UIntTuple ();	/* Build decimal/octal/hex ascii string */

/*
 *  FUNCTION
 *
 *	dump    dump a single common object module file
 *
 *  SYNOPSIS
 *
 *	VOID dump (ldptr)
 *	LDFILE *ldptr;
 *
 *  DESCRIPTION
 *
 *	Dump the contents of a single common object module.
 *
 */

VOID dump (ldptr)
LDFILE *ldptr;
{
    DBUG_ENTER ("dump");
    dump_arhdr (ldptr);
    if (flags.fflag) {
	dump_filehdr (ldptr);
    }
    if (flags.oflag) {
	dump_auxhdr (ldptr);
    }
    dump_allsec (ldptr);
    if (flags.tflag) {
	dump_syms (ldptr);
    }
    DBUG_VOID_RETURN;
}

static VOID dump_arhdr (ldptr)
LDFILE *ldptr;
{
    auto ARCHDR arhead;
    
    DBUG_ENTER ("dump_arhdr");
    if (ldahread (ldptr, &arhead) == SUCCESS) {
	DBUG_2 ("ldahread", "read archive header");
	printf ("Archive file member \"%s\" header:\n\n", arhead.ar_name);
	printf ("\tar_name:\t%12s\n", arhead.ar_name);
	printf ("\tar_date:\t%12ld\t", arhead.ar_date);
	printf ("<%24.24s>\n", ctime (&arhead.ar_date));
	printf ("\tar_uid:\t\t%s\n", IntTuple (arhead.ar_uid));
	printf ("\tar_gid:\t\t%s\n", IntTuple (arhead.ar_gid));
	printf ("\tar_mode:\t%#12lo\n", arhead.ar_mode);
	printf ("\tar_size:\t%s\n", LongTuple (arhead.ar_size));
	printf ("\n");
    }
    DBUG_VOID_RETURN;
}

static VOID dump_filehdr (ldptr)
LDFILE *ldptr;
{
    DBUG_ENTER ("dump_filehdr");
    printf ("File header:\n\n");
    printf ("\tf_magic:\t%#12o\t", HEADER(ldptr).f_magic);
    printf ("<%s>\n", TransMagic (HEADER(ldptr).f_magic));
    printf ("\tf_nscns:\t%s\n", UIntTuple (HEADER(ldptr).f_nscns));
    printf ("\tf_timdat:\t%12ld\t", HEADER(ldptr).f_timdat);
    printf ("<%24.24s>\n", ctime (&HEADER(ldptr).f_timdat));
    printf ("\tf_symptr:\t%s\n", LongTuple (HEADER(ldptr).f_symptr));
    printf ("\tf_nsyms:\t%s\n", LongTuple (HEADER(ldptr).f_nsyms));
    printf ("\tf_opthdr:\t%s\n", UIntTuple (HEADER(ldptr).f_opthdr));
    printf ("\tf_flags:\t%#12o\t", HEADER(ldptr).f_flags);
    printf ("<%s>\n", TransFlags (HEADER(ldptr).f_flags));
    printf ("\n");
    DBUG_VOID_RETURN;
}


static VOID dump_auxhdr (ldptr)
LDFILE *ldptr;
{
    auto AOUTHDR auxhead;
    
    DBUG_ENTER ("dump_auxhdr");
    if (ldohseek (ldptr) == SUCCESS) {
	DBUG_2 ("ldohseek", "found auxilliary header");
	if (FREAD (&auxhead, sizeof (auxhead), 1, ldptr) == 1) {
	    DBUG_2 ("ldfhread", "read auxilliary header");
	    printf ("Auxiliary file header:\n\n");
	    printf ("\tmagic:\t\t%s\n", IntTuple (auxhead.magic));
	    printf ("\tvstamp:\t\t%s\n", IntTuple (auxhead.vstamp));
	    printf ("\ttsize:\t\t%s\n", LongTuple (auxhead.tsize));
	    printf ("\tdsize:\t\t%s\n", LongTuple (auxhead.dsize));
	    printf ("\tbsize:\t\t%s\n", LongTuple (auxhead.bsize));
	    printf ("\tentry:\t\t%s\n", LongTuple (auxhead.entry));
	    printf ("\ttext_start:\t%s\n", LongTuple (auxhead.text_start));
	    printf ("\tdata_start:\t%s\n", LongTuple (auxhead.data_start));
	    printf ("\n");
	}
    }
    DBUG_VOID_RETURN;
}


static VOID dump_allsec (ldptr)
LDFILE *ldptr;
{
    register int sectindx;
    auto SCNHDR secthead;
    
    DBUG_ENTER ("dump_allsec");
    for (sectindx = 1; sectindx <= HEADER(ldptr).f_nscns; sectindx++) {
	if (ldshread (ldptr, sectindx, &secthead) == FAILURE) {
	    cd_error (ERR_SECTHEAD, sectindx);
	} else {
	    if (flags.hflag) {
		dump_shdr (ldptr, &secthead, sectindx);
	    }
	    if (flags.sflag) {
		dump_sdata (ldptr, &secthead, sectindx);
	    }
	    if (flags.rflag) {
		dump_sreloc (ldptr, &secthead, sectindx);
	    }
	    if (flags.lflag) {
		dump_lnno (ldptr, &secthead, sectindx);
	    }
	}
    }
    DBUG_VOID_RETURN;
}


static VOID dump_shdr (ldptr, secthead, sectindx)
LDFILE *ldptr;
SCNHDR *secthead;
INT sectindx;
{
    DBUG_ENTER ("dump_shdr");
    printf ("Section \"%s\" header:\n\n", secthead -> s_name);
    printf ("\ts_name:\t\t%12s\n", secthead -> s_name);
    printf ("\ts_paddr:\t%s\n", LongTuple (secthead -> s_paddr));
    printf ("\ts_vaddr:\t%s\n", LongTuple (secthead -> s_vaddr));
    printf ("\ts_size:\t\t%s\n", LongTuple (secthead -> s_size));
    printf ("\ts_scnptr:\t%s\n", LongTuple (secthead -> s_scnptr));
    printf ("\ts_relptr:\t%s\n", LongTuple (secthead -> s_relptr));
    printf ("\ts_lnnoptr:\t%s\n", LongTuple (secthead -> s_lnnoptr));
    printf ("\ts_nreloc:\t%s\n", UIntTuple (secthead -> s_nreloc));
    printf ("\ts_nlnno:\t%s\n", UIntTuple (secthead -> s_nlnno));
    printf ("\ts_flags:\t%12lo", secthead -> s_flags);
    printf ("\t<%s>\n", TransSflags (secthead -> s_flags));
    printf ("\n");
    DBUG_VOID_RETURN;
}


static VOID dump_sdata (ldptr, secthead, sectindx)
LDFILE *ldptr;
SCNHDR *secthead;
INT sectindx;
{
    register ULONG count;
    
    DBUG_ENTER ("dump_sdata");
    if (secthead -> s_scnptr != 0) {
	if (FSEEK (ldptr, secthead -> s_scnptr, BEGINNING) == OKFSEEK) {
	    printf ("Section \"%s\" data:\n", secthead -> s_name);
	    for (count = 0L; count < secthead -> s_size; count++) {
		if (count % BYTES_PER_LINE == 0) {
		    printf ("\n\t0x%8.8x:  ", count);
		}
		printf ("%3.2x", GETC (ldptr));
	    }
	    printf ("\n\n");
	}
    }
    DBUG_VOID_RETURN;
}


static VOID dump_sreloc (ldptr, secthead, sectindx)
LDFILE *ldptr;
SCNHDR *secthead;
INT sectindx;
{
    register ULONG count;
    auto struct reloc rbuf;
    
    DBUG_ENTER ("dump_sreloc");
    if (secthead -> s_nreloc != 0) {
	if (FSEEK (ldptr, secthead -> s_relptr, BEGINNING) == OKFSEEK) {
	    printf ("Section \"%s\" relocation:\n\n", secthead -> s_name);
	    for (count = 0; count < secthead -> s_nreloc; count++) {
		if (FREAD (&rbuf, sizeof (rbuf), 1, ldptr) != 1) {
		    cd_error (ERR_RELOC, count);
		} else {
		    printf ("\tindex:\t\t%s\n", LongTuple (count));
		    printf ("\tr_vaddr:\t%s\n", LongTuple (rbuf.r_vaddr));
		    printf ("\tr_symndx:\t%s\n", LongTuple (rbuf.r_symndx));
		    printf ("\tr_type:\t\t%12d\t", rbuf.r_type);
		    printf ("<%s>\n", TransReloc (rbuf.r_type));
		    printf ("\n");
		}
	    }
	    printf ("\n");
	}
    }
    DBUG_VOID_RETURN;
}


static VOID dump_lnno (ldptr, secthead, sectindx)
LDFILE *ldptr;
SCNHDR *secthead;
INT sectindx;
{
    register ULONG count;
    auto LINENO linent;
    
    DBUG_ENTER ("dump_lnno");
    if (secthead -> s_nlnno != 0) {
	if (FSEEK (ldptr, secthead -> s_lnnoptr, BEGINNING) == OKFSEEK) {
	    printf ("Section \"%s\" line numbers:\n\n", secthead -> s_name);
	    for (count = 0; count < secthead -> s_nlnno; count++) {
		if (FREAD (&linent, sizeof (linent), 1, ldptr) != 1) {
		    cd_error (ERR_LNNO, count);
		} else {
		    printf ("\tindex:\t\t%s\n", LongTuple (count));
		    if (linent.l_lnno == 0) {
			printf ("\tl_symndx:");
			printf ("\t%s\n", LongTuple (linent.l_addr.l_symndx));
		    } else {
			printf ("\tl_paddr:");
			printf ("\t%s\n", LongTuple (linent.l_addr.l_paddr));
		    }
		    printf ("\tl_lnno:\t\t%s\n", UIntTuple (linent.l_lnno));
		    printf ("\n");
		}
	    }
	    printf ("\n");
	}
    }
    DBUG_VOID_RETURN;
}


static VOID dump_syms (ldptr)
LDFILE *ldptr;
{
    register LONG symindex;
    auto SYMENT symbol;
    auto int auxent;
    auto int auxtype;
    
    DBUG_ENTER ("dump_syms");
    if (ldtbseek (ldptr) == SUCCESS) {
	printf ("Symbol table:\n\n");
	auxent = 0;
	for (symindex = 0; symindex < HEADER(ldptr).f_nsyms; symindex++) {
	    if (ldtbread (ldptr, symindex, &symbol) == FAILURE) {
		cd_error (ERR_SYMR, symindex);
	    } else {
		if (auxent) {
		    PrtAuxent (auxtype, symindex, (AUXENT *) &symbol);
		    auxent = 0;
		} else {		 
		    PrtSyment (symindex, &symbol);
		    auxent = symbol.n_numaux;
		    if (auxent) {
			auxtype = AuxType (&symbol);
		    }
		}
	    }
	}
	printf ("\n");
    }
    DBUG_VOID_RETURN;
}

static VOID PrtSyment (symindex, symp)
LONG symindex;
SYMENT *symp;
{
    printf ("\tindex:\t\t%s\n", LongTuple (symindex));
    if (symp -> n_zeroes == 0) {
	printf ("\t_n_name:\t%12s\n", "<strtab string>");
	printf ("\t_n_offset:\t%s\n", LongTuple (symp -> n_offset));
    } else {
	printf ("\t_n_name:\t%12s\n", symp -> n_name);
    }
    printf ("\tn_value:\t%s\n", LongTuple (symp -> n_value));
    printf ("\tn_scnum:\t%12d\t", symp -> n_scnum);
    printf ("<%s>\n", TransSection (symp -> n_scnum));
    printf ("\tn_type:\t\t%s\n",UIntTuple (symp -> n_type));
    printf ("\tn_sclass:\t%12d\t", symp -> n_sclass);
    printf ("<%s>\n", TransClass (symp -> n_sclass));
    printf ("\tn_numaux:\t%12d\n", symp -> n_numaux);
    printf ("\n");
}
