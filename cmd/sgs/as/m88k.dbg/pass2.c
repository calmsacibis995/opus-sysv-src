/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/pass2.c	1.18" */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include "scnhdr.h"
#include "filehdr.h"
#include "systems.h"
#include "gendefs.h"
#include "symbols.h"
#include "codeout.h"
#include "section.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

/*
 *
 *	"pass2.c" is a file that contains the main routine for the
 *	final pass of the assembler.  This includes the primary control
 *	(function "main.c") and "getargs", the flag handling routine.
 *	This pass of the assembler is invoked with the command:
 *
 *		as2 [flags] ifile ofile t2 t3 t4 t5 t6 ... t[NSECTIONS+5]
 *
 *	where [flags] are optional flags passed from pass 1,
 *	"ifile" is the name of the assembly language source
 *	program, "t2" ... are the names of temporary files,
 *	and "ofile" is the name of the file where the object program
 *	is to be written.  Pass2 uses the files as follows:
 *
 *	name	internal	mode	use
 *
 *	ifile	filenames[0]		name used in error messages
 *	ofile	filenames[1]	write	assembly output
 *	t2	filenames[2]	read*	section table
 *	t3	filenames[3]	read*	flexnames string table
 *	t4	filenames[4]	read*	symbol table
 *	t5	filenames[5]	read*	span dependent instruction table
 *	t6	filenames[6]	read	temp for section 1
 *	...
 *	tk	filenames[k]	read	temp for section k-5
 *	
 *  '*' after mode indicates files used only for multiple process implementations
 *
 *	Since all the information about the
 *	program is contained in temporary files at the time that the
 *	second pass is executed, the name "ifile" is only used in
 *	printing error messages.  The intermediate form of the 
 *	sections appears in the temporary file t6 through t[5+n], where n is
 *	the number of sections. t2 contains the section information, including
 *	the total number of sections, as determined by pass 1.
 *	See the commentary in the function "main" for the exact sequence of
 *	events. "ofile" is used as the output from this pass.
 *
 */

/*
 *
 *	The following are declarations for the major global variables
 *	used in the second pass of the assembler.  The variables and
 *	their meainings are as follows:
 *
 *	fdout	This is used to contain the file descriptor for the
 *		object file.
 *
 *	fdsect	This is the file descriptor where temporary section
 *		information is to be written.  This will refer to
 *		several different temporary files throughout execution
 *		of this pass.
 *
 *	fdline	This is the file descriptor where the completed line
 *		number entries are always written. Both the line number
 *		entries for all sections  are placed in this file. 
 *		t4 is used to store this information.
 *
 *	fdrel	This is the file descriptor where preliminary relocation
 *		entries are to be written.  As it happens, this will
 *		always refer to the temporary file "t5" during execution
 *		of this program.
 *
 *	fdsymb	This is the file descriptor where symbol table entries
 *		will be written.  These are symbol table entries from
 *		".def" pseudo operations, not to be confused with the
 *		internal symbol table entries that are read from "t4"
 *		when this pass begins execution.  This file descriptor
 *		will refer to the temporary file "t3" during execution
 *		of this program.
 *
 *	fdgsymb	This is the file descriptor where symbol table entries
 *		for global symbols whose declarations appear in
 *		the intermediate file for the "DAT" sections will be
 *		written.  These are written to a file other than that
 *		referred to by "fdsymb" so that declarations for
 *		external static symbols can find their way ahead of the
 *		global symbol declarations. t4 contains this info.
 *
 *	filenames[0]
 *		This is a pointer to the string which is the input file
 *		name ("ifile", as described at the beginning of this
 *		file).  This will be used if necessary to report the
 *		file name in an error message (see "errors.c").
 *
 *	filhead	This is a structure that is used to build the header for
 *		the object file.  It is of type "FILHDR", defined in
 *		"filehdr.h".
 *
 *
 *	symhead	This is a structure that is used to build the dummy
 *		section header whose offset points to the symbol table.
 *		It is of type "SCNHDR", defined in "scnhdr.h".
 *
 *	symbent	A count of the number of entries in the symbol table
 *		whose file is referred to be "fdsymb".
 *
 *	gsymbent A count of the number of entries in the symbol table
 *		whose file is referred to by "fdgsymb".
 *
 *	seccnt	A count of the number of sections. The first
 *		section is numbered 1.
 *
 *	sectab	The array of section headers that will be written 
 *		after the file header in the output file.  Indexed
 *		from 1 through seccnt.
 *
 *	secdat	An internal array of section attributes including
 *		the type of the section, the input/output file
 *		descriptor and the output buffer.
 *
 */

extern short passnbr;
extern unsigned short sdicnt;

extern int seccnt;
extern struct scnhdr sectab[];
extern struct scninfo secdat[];

extern char *filenames[];

extern char *malloc ();
extern char *strcpy ();
extern FILE *fopen ();

extern short anyerrs;

extern unsigned short lineent;

extern void as_error ();
extern void perfreport2 ();
extern void perfsetup ();
extern void copysect ();
extern void reloc ();
extern void headers ();
extern void fixst ();
extern void codout ();
extern void fixstab ();
extern void codgen ();
extern void delexit ();
extern void deltemps ();
extern int flags ();
extern void exit ();

extern upsymins *lookup ();

extern long currindex;
extern char *strtab;

BYTE *longsdi;

extern char islongsdi[];

unsigned short relent;

extern short transvec;

short uflag = NO;
short dlflag = NO;

/*
 *	The following variables are for the .expr section kluge and
 *	allow us to independently seek/read/write the output file, inside
 *	the .expr section raw data, keep track of the current offset, and
 *	keep track of the base offset to compute offsets within the
 *	section.
 */

#ifdef EXPR
FILE *fdexpr; 			/* Stream to access .expr raw data */
long exprhome;		/* Offset to start of .expr section */
#endif

FILE *fdout;
FILE *fdsect;
FILE *fdline;
FILE *fdrel;
FILE *fdsymb;
FILE *fdgsymb;

long symbent = 0L;
long gsymbent = 0L;
long hdrptr;

short keeptmps = NO;

static long rel;

static FILE *fopenw ();		/* Open a temp file for writing */
static FILE *fopenr ();		/* Open a temp file for reading */
static void tclose ();		/* Flush and close a temporary file */
static void wrtsects ();	/* Write out section data */
static void sctalign ();	/* Round section sizes up to SCTALIGN */
static void finishobj ();	/* Write end of obj file after data sects */
static void lnno_chk ();	/* Suppress line number info if desired */
static void open_ofile ();	/* Open output file and other stuff */

/*
 *
 *	"main" is the main function for the second pass of the assembler.
 *	It is invoked as described at the beginning of this file.  It
 *	performs initialization by setting the variable "file" to point
 *	to the source file name, setting "arglist" to point to the
 *	argument list (this is so temporary files can be removed easily
 *	in case of error), and checking the argument count.  It
 *	then performs the following sequence of steps:
 *
 *	     1.	The section lengths are determined from sectab[].
 *		If necessary, the section sizes are rounded up to
 *		even sizes and the "s_up" flag is set in the 
 *		corresponding secdat[] entry. "siz" keeps a
 *		running total of the section sizes and is used
 *		as an argument to fixstab() to convert the offsets
 *		in each section to addresses.
 *
 *	     2. The following file descriptors are opened:
 *
 *		fdsect	This is opened for writing on the output file
 *			ofile.  The section information for the each
 *			section will be written to this file.
 *
 *		fdsymb	This is opened for writing on the temporary file
 *			t3.  The symbol table entries will be written
 *			to this file.
 *
 *		fdline	This is opened for writing on the temporary file
 *			t4.  The preliminary line number entries for
 *			the text section will be written to this file.
 *
 *		fdrel	This is opened for writing on the temporary file
 *			t5.  The preliminary relocation information for
 *			the text section will be written to this file.
 *
 *		The following occurs for each section S in sectab[]:
 *
 *		lineent	This is used to count the number of line number
 *			entries written to "fdline".  The value of this
 *			variable will be copied to sectab[S].s_nlnno.
 *
 *		relent	This is used to count the number of relocation
 *			entries written to "fdrel".  The value of this
 *			variable will be copied to sectab[S].s_nreloc.
 *
 *		symbent	This is used to keep count of the number of
 *			symbol table entries written to "fdsymb".
 *
 *		gsymbent This is used to keep count of the number of
 *			symbol table entries written to "fdgsymb".  This
 *			will not be used until the data section is
 *			processed.
 *
 *		The function "codout" (from codout.c) is called to
 *		process the "TXT" and "DAT" sections.  If the flag 
 *		"s_up" is set, an extra byte having the value hex "f4" is 
 *		written to the end of "fdsect".  This rounds the section out
 *		to an even number of bytes by adding a "hlt"
 *		instruction.
 *
 *	     3. The file descriptor "fdout" is initialized and is
 *		set to point to the beginning of the object file to
 *		be created.  The function "headers" is called to
 *		create the file headers and all section headers for
 *		the object file.
 *
 *	     4. The file descriptor "fdrel" is opened to read from t5,
 *		and "reloc" is called to process the text section
 *		and data section relocation entries and write them
 *		to the object file.
 *
 *	     5. The file t4 containing the line number entries is
 *		appended to the object file by calling "copysect".
 *
 *	     6. The file t3 containing the complete symbol table is
 *		copied to the object file using "copysect".  Following
 *		this, the file descriptor "fdout" can be closed, since
 *		the object file is complete.
 *
 *	     7. If the assembler has not been called for testing
 *		the temporary file are removed by calling "deltemps".
 *
 */

aspass2 () 
{
    register long home;
    
    DBUG_ENTER ("aspass2");
    passnbr = 2;
    perfsetup ();
    longsdi = (BYTE *) islongsdi;
    sctalign ();
    open_ofile ();
    printf("filenames[2] is %s\n", filenames[2]);
    printf("filenames[3] is %s\n", filenames[3]);
    printf("filenames[4] is %s\n", filenames[4]);
    printf("filenames[5] is %s\n", filenames[5]);
    fdgsymb = fopenw (filenames[2]);
    fdsymb = fopenw (filenames[3]);
    fdline = fopenw (filenames[4]);
    fdrel = fopenw (filenames[5]);
    wrtsects ();
    DBUG_PRINT ("code", ("raw code for all sections now complete"));
    fixst (fdsymb);
    tclose (fdline);
    tclose (fdrel);
    tclose (fdgsymb);
    fdgsymb = fopenr (filenames[2]);
    symout ();
    (void) fclose (7, fdgsymb);	/* preliminary global symbols are processed */
    tclose (fdsymb);
    home = ftell (fdout);	/* remember where we are */
    (void) fseek (fdout, 0L, 0);
    lnno_chk ();
    headers ();
    (void) fseek (fdout, home, 0);	/* return for more work */
    fdrel = fopenr (filenames[5]);
    reloc (rel);
    (void) fclose (8, fdrel);
    DBUG_PRINT ("rel", ("relocation info is complete and appended"));
    perfreport2 ();
    if (anyerrs) {
	delexit ();
    } else {
	finishobj ();
    }
    DBUG_RETURN (0);
}

/*
 *	Open a temporary file for writing.  Any failure here will
 *	be immediately fatal, via action in as_error().
 */

static FILE *fopenw (name)
char *name;
{
    register FILE *fp;

    DBUG_ENTER ("fopenw");
    if ((fp = fopen (name, "w")) == NULL) {
	fprintf(stderr, "can't open %s\n", name);
	as_error (ERR_COTW, name);
    }
fprintf(stderr, "fp is %#x\n", fp);
    DBUG_RETURN (fp);
}

/*
 *	Open a temporary file for reading.  Any failure here will
 *	be immediately fatal, via action in as_error().
 */

static FILE *fopenr (name)
char *name;
{
    register FILE *fp;

    DBUG_ENTER ("fopenr");
    if ((fp = fopen (name, "r")) == NULL) {
	as_error (ERR_COTR, name);
    }
    DBUG_RETURN (fp);
}

/*
 *	Flush a buffered temporary file, check for problems, and then close 
 *	it.  Any errors are immediately fatal via action in as_errors().
 */

static void tclose (fp)
FILE *fp;
{
    DBUG_ENTER ("tclose");
    (void) fflush (fp);
    if (ferror (fp)) {
	as_error (ERR_TCL);
    }
    (void) fclose (9, fp);
    DBUG_VOID_RETURN;
}

/*
 *	Scan through the sections, rounding the sizes up to the next
 *	multiple of SCTALIGN.
 *
 *	Note that the s_up flag is set in the corresponding secdat[] entry,
 *	and "siz" keeps a running total of the section sizes and is used
 *	as an argument to fixstab() to convert the offsets in each
 *	section to addresses.
 */

static void sctalign ()
{
    register int i;
    register struct scnhdr *sect;
    register struct scninfo *seci;
    register symbol *ptr;
    register long siz;

    DBUG_ENTER ("sctalign");
    DBUG_PRINT ("p2", ("compute section sizes, aligning to SCTALIGN"));
    siz = 0;
    for (i = 1, sect = &sectab[1], seci = &secdat[1]; i <= seccnt; i++, sect++, seci++) {
	ptr = (*lookup (sect -> s_name, N_INSTALL, USRNAME)).stp;
	ptr -> value = 0L;	/* has s_flags, needed for pass1; clear
				   before fixstab() below */
	if (seci -> s_up = sect -> s_size % SCTALIGN) {
	    seci -> s_up = SCTALIGN - (seci -> s_up);
	    sect -> s_size += seci -> s_up;
	}
	if (i > 1) {
	    fixstab (0L, siz, i);
	}
	siz += sect -> s_size;
    }
    DBUG_VOID_RETURN;
}

/*
 *		The following occurs for each section S in sectab[]:
 *
 *		lineent	This is used to count the number of line number
 *			entries written to "fdline".  The value of this
 *			variable will be copied to sectab[S].s_nlnno.
 *
 *		relent	This is used to count the number of relocation
 *			entries written to "fdrel".  The value of this
 *			variable will be copied to sectab[S].s_nreloc.
 *
 *		symbent	This is used to keep count of the number of
 *			symbol table entries written to "fdsymb".
 *
 *		gsymbent This is used to keep count of the number of
 *			symbol table entries written to "fdgsymb".  This
 *			will not be used until the data section is
 *			processed.
 *
 *		The function "codout" (from codout.c) is called to
 *		process the "TXT" and "DAT" sections.  If the flag 
 *		"s_up" is set, an extra byte having the value hex "f4" is 
 *		written to the end of "fdsect".  This rounds the section out
 *		to an even number of bytes by adding a "hlt"
 *		instruction.
 *
 */

static void wrtsects ()
{
    register int i;
    register struct scnhdr *sect;
    register struct scninfo *seci;
    long siz;

    DBUG_ENTER ("wrtsects");
    rel = 0;
    siz = 0;
    for (i = 1, sect = &sectab[1], seci = &secdat[1]; i <= seccnt; i++, sect++, seci++) {
	/* The following is for the .expr section kluge */
#ifdef EXPR
	if (strncmp (sect -> s_name, _EXPR, 8) == 0) {
	    DBUG_PRINT ("expr", ("found %s section is sect %d", _EXPR, i));
	    exprhome = ftell (fdout);
	    (void) fseek (fdexpr, exprhome, 0);
	    DBUG_PRINT ("expr", ("at offset %ld in output file", exprhome));
	} 
#endif
	lineent = 0;
	relent = 0;
	codout (filenames[i + 5], siz, i);
	if (seci -> s_typ == TXT) {
	    while (seci -> s_up >= 4) {
		codgen (BITSPBY * 4, TXTNOP);
		seci -> s_up -= 4;
	    }
	    while (seci -> s_up--) {
		codgen (BITSPBY, TXTFILL);
	    }
	} else if (seci -> s_typ != BSS) {
	    while (seci -> s_up--) {
		codgen (BITSPBY, FILL);
	    }
	}
	sect -> s_nlnno = lineent;
	sect -> s_nreloc = relent;
	rel += sect -> s_nreloc;
	siz += sect -> s_size;
    }
    DBUG_VOID_RETURN;
}

/*
 *	Finish building the object file after writing out the
 *	data sections.
 */

static void finishobj ()
{
    register int i;

    DBUG_ENTER ("finishobj");
    if (!dlflag) {
	DBUG_PRINT ("linno", ("append line number information"));
	copysect (filenames[4]);
    }
#if AR32W || AR32WR
    /* 
     * The following code removes all unused symbol
     * table entries.  It also will detect errors in
     * COFF.
     */
    DBUG_PRINT ("unsym", ("remove all unused symbol table entries"));
    (void) fflush (fdout);	/* make sure everything is written out */
    if (uflag) {
	if ((i = unused (filenames[3], filenames[1])) <= -1000) {
	    as_error (ERR_IOFE);
	} else if (i < 0) {
	    as_error (ERR_CRSYMTBL);
	    DBUG_PRINT ("symtab", ("append symbol table"));
	    copysect (filenames[3]);
	} else if (i == 0) {
	    /* no symbols to remove */
	    DBUG_PRINT ("symtab", ("append symbol table"));
	    copysect (filenames[3]);
	}
    } else {
	DBUG_PRINT ("symtab", ("append symbol table"));
	copysect (filenames[3]);
    }
    /* 
     * End of removal code
     */
#else
    DBUG_PRINT ("symtab", ("append symbol table"));
    copysect (filenames[7]);
#endif
    if (currindex > 4) {
	/* If string table exists, write at end of symbol table */
	DBUG_PRINT ("strtab", ("write string table at end of symbol table"));
	(void) fseek (fdout, 0L, 2);
	(void) fwrite ((char *) &currindex, sizeof (long), sizeof (char), fdout);
	(void) fwrite (&strtab[4], sizeof (char), (int) (currindex - 4), fdout);
    }
    tclose (fdout);
    DBUG_PRINT ("fini", ("finished with object file, closed"));
    if (!keeptmps) {
	deltemps ();
    }
    DBUG_VOID_RETURN;
}

/*
 *	If line number information in the object file is to be
 *	suppressed (-dl option) then zero out s_nlnno for each
 *	section.
 */

static void lnno_chk ()
{
    register int i;

    DBUG_ENTER ("lnno_chk");
    if (dlflag) {
	for (i = 1; i <= seccnt; i++) {
	    sectab[i].s_nlnno = 0;
	}
    }
    DBUG_VOID_RETURN;
}

static void open_ofile ()
{
    DBUG_ENTER ("open_ofile");
    DBUG_PRINT ("ofile", ("open output file '%s' for writing", filenames[1]));
    if ((fdsect = fopen (filenames[1], "w+")) == NULL) {
	(void) unlink (filenames[1]);
	if ((fdsect = fopen (filenames[1], "w+")) == NULL) {
	    as_error (ERR_COOF, filenames[1]);
	}
    }
#ifdef EXPR
    if ((fdexpr = fopen (filenames[1], "r+")) == NULL) {
	as_error (ERR_COOF, filenames[1]);
    }
#endif
    fdout = fdsect;
    hdrptr = ftell (fdout);
    DBUG_PRINT ("hdr", ("position pointer past area for headers"));
#if 0
    (void) fseek (fdout, (long) (hdrptr + FILHSZ + seccnt * SCNHSZ), 0);
#else	/* dc/jkl/ccf:  align properly */
    (void) fseek (fdout, (long)
	    (((hdrptr + FILHSZ + seccnt * SCNHSZ)+(SCTALIGN-1)) & -SCTALIGN),
	    0);
#endif
    DBUG_VOID_RETURN;
}
