/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)dis:common/dis_main.c	1.26"


#include	<stdio.h>
#include	<ar.h>
#include	"dis.h"
#include	"filehdr.h"
#include	"ldfcn.h"
#include	"sgs.h"
#include	"scnhdr.h"
#include	"paths.h"
#include	"syms.h"
#include	"linenum.h"
#include	"structs.h"
#ifdef OLIST
#include	"olist_defs.h"
#else
#define TOOL	"dis"
#endif

/* this header is to contain the archive header information */
static	ARCHDR	archhdr;

/*
 *	To process the '-l string' option, the library name will be
 *	built by concatenating  LIBDIR  from 'paths.h' (where the library is),
 *	LIB from '_main.c' (/lib), the user supplied string
 *	(will probably be 'a' for liba.a) and '.a', thus building
 *	the whole name.  Code for this is in the 'command_line' function.
 */

#define		LIB	"/lib"

/*
 *	the number of files that may be disassembled.
 */

#define		FILES	20
#define		FILNMLEN	14


/*
 * filename	contains the names of the files to be disassembled
 * files	contains the number of entries in the above array
 */

static	char	*filename[FILES];
static	int	files = -1;

static	int	status;		/* for check after return from I/O routines	*/
extern char mneu[];


/*
 *	main (argc, argv)
 *
 *	this routine deals with the disassembly only on a per-file basis.
 *	Upon determining that a file is valid (by a magic number) it then
 *	gives control to a subordinate routine (disassembly) to proceed
 *	with the processing.
 */

main(argc, argv)
int	argc;
char	*argv[];
{
	extern	LDFILE	*f_ptr;		/* in _extn.c */
	extern	char	*fname;		/* in _extn.c */
	extern  int 	Sflag;		/* in _extn.c */
	extern	int	lflag;		/* in _extn.c */
	extern	int	Lflag;		/* in _extn.c */
	extern int	save_Lflag;	/* in _extn.c */
#ifdef OLIST
	extern	int	isarch;
	extern	char	membernm[];
	extern	char	*tmpfil;
#endif	

	int	c;
	unsigned short	type;
	static char	buffer[BUFSIZ];
#ifdef OLIST
	char	buf[BUFSIZ/2];
#endif

	if (argc == 1) {
		fprintf(stderr,"%s%s: Usage is %s%s [flags] file1 ...\n",SGS,TOOL,SGS,TOOL);
		exit(1);
	}

	setbuf(stdout, buffer);

	/* process the command line	*/

	command_line(argc, argv);

#ifdef OLIST
	/* make temporary files */
	tmpfil=tempnam(TMPDIR,"listp");
#else
#if M88K
	if (files > 0)
#endif
	printf("\t\t****  %s DISASSEMBLER  ****\n\n",SGS);
#endif

	/* verify each named file as it is opened for disassembly	*/
	for (c = 0; c <= files; c++) {	/* for each named file */
		fname = filename[c];
#ifdef OLIST
		printf("\n\n\t\t\t3B20D OBJECT LISTING\n\t\t\t\t%s\n\n", fname);
#else
#if M88K
		if (!Sflag)
#endif
			printf("\ndisassembly for %s\n",fname);
#endif

		/*
		 * The do while loop is the construct used to process both
		 * archive and regular files.  The library routine 'ldclose'
		 * will recognize the fact that an archive is being processed
		 * and will return a true value only when asked to close
		 * what was the last member of an archive file.
		 */
		do {
#ifdef OLIST
			isarch = FALSE;
#endif
			f_ptr = ldopen(fname, f_ptr);
			type = TYPE(f_ptr);
			fflush(stdout);
			if (f_ptr == NULL)
				fprintf(stderr,"\n%s%s: %s: cannot open\n",SGS,TOOL,fname);
			/* CCFelse if ((!ISARCHIVE(type)) && (!ISMAGIC(type))) {*/
			else if (0) {
				fprintf(stderr,"\n%s%s: %s: bad magic %o\n",SGS,TOOL,fname,type);
				ldclose(f_ptr);
				break;	/* to next file	*/
			}

			else  {	/* successful open so process */
				/* if the file was an archive, read	*/
				/* and verify its header		*/
				if (type == USH_ARTYPE) {
					status = ldahread(f_ptr, &archhdr);
#if OLIST
					isarch = TRUE;
					sprintf(membernm, "%s", archhdr.ar_name);
					membernm[FILNMLEN] = NULL;
#else
					printf("\narchive member\t\t%s\n",
						archhdr.ar_name);
#endif

					if (status == FAIL) {
						printf("\n%s%s: %s:  %s:",SGS,TOOL,
							fname,archhdr.ar_name);
						fprintf(stderr," bad archive header\n");
						continue;

					}
				}

				disassembly();
			}
		}
		while (!ldclose(f_ptr));
		if (Lflag == -1)
			Lflag = save_Lflag;
	}

	if (lflag > 0)
		lib_free();	/* free space allocated for library names */
#ifdef OLIST
	sprintf(buf, "/bin/rm -f %s\n", tmpfil);
	system(buf);	/* remove temp files */
#endif

	/* this statement is to assure that the return code will be 0 */
	exit(0);
}
/*
 *	disassembly ()
 *
 *	For each file that is disassembled, disassembly opens the
 *	necessary file pointers, builds the list of section
 *	headers, and if necessary, lists of functions and labels.
 * 	It then calls text_sections or dis_data to disassemble
 *	the sections.
 */

disassembly()
{
	extern	LDFILE	*ldaopen( );
	extern	SCNLIST	*build_sections();

	extern	LDFILE	*f_ptr;
	extern	LDFILE	*symb;
	extern	LDFILE	*symb_ptr;
	extern	LDFILE	*l_ptr;
	extern	LDFILE	*t_ptr;
	extern	int	Lflag;
	extern	char	*fname;
	extern	char	*sname;
	extern	FILHDR	filhdr;
	extern	FUNCLIST	*next_function;
	extern	SCNHDR	scnhdr;
	extern	int	sflag;
	extern	int	nosyms;
#if OLIST
	extern	int	minmal;
	extern	struct	files	*curfile;
	extern	SCNLIST	*curr_section;
#else
	extern	int	fflag;
#endif
	extern	SCNLIST	*sclist;
#if M88K
	extern	int	Aflag;
#endif

	SCNLIST		*sectp;
	int		i;

	filhdr = HEADER( f_ptr );
	symb = ldaopen( fname, f_ptr );
	/* nosyms will TRUE only if sflag is on and the last file had no symbol table */
	if (nosyms) {
		nosyms = FALSE;
		sflag = 1;
	}
	/* check to see if the current file has a symbol table; if it doesn't
	   turn off the symbolic disassembly */
	if ((ldtbseek( symb ) == FAILURE) && sflag) {
		nosyms = TRUE;
		sflag = 0;
		fprintf(stderr,"\nNo symbol table in %s: symbolic disassembly cannot be performed.\n",fname);
	}
	symb_ptr = ldaopen( fname, f_ptr );
	ldtbseek( symb_ptr );
	l_ptr = ldaopen( fname, f_ptr );
	t_ptr = ldaopen(fname, f_ptr);

	/* if labels are requested, make a linked list of the labels */
	if ( Lflag > 0 )
		build_labels();

#if VAX || N3B || U3B || M32 || CLIPPER || NS32000 || M88K
#if M88K
	if (!nosyms) {
#else
	if (sflag) {
#endif
		init_eshash();
		setup_lochash();
	}
#endif

#if OLIST
	minmal = (filhdr.f_flags & F_MINMAL);
#else
	/* disassemble only the names functions */
	if (fflag)
	{
		build_sections();
		build_funcs();
		search_table();
		dis_funcs();

		if (Lflag > 0)
			label_free();
		ldaclose( symb );
		ldaclose( symb_ptr );
		ldaclose( l_ptr );
		ldaclose( t_ptr );
		return;
	}
#endif

	/* make a list of the sections to be disassembled */
	build_sections();

	/* make lists of the files and functions from the symbol table */
#if OLIST
	bld_files();
	if (minmal)
		build_funcs();
#else
#if !AR16WR
	build_funcs();
#endif
#endif

	for ( sectp = sclist; sectp; sectp = sectp->snext )
		if (sectp->stype == TEXT)
		{
			FSEEK( l_ptr, sectp->shdr.s_lnnoptr, BEGINNING );
#if M88K
			if (Aflag && (Aflag - sectp->shdr.s_paddr > 0 && 
					sectp->shdr.s_paddr +
					sectp->shdr.s_size - Aflag > 0))
				FSEEK( t_ptr, Aflag-sectp->shdr.s_paddr+
					sectp->shdr.s_scnptr, BEGINNING );
			else
#endif
			FSEEK( t_ptr, sectp->shdr.s_scnptr, BEGINNING );
			sprintf( sname, "%.8s", sectp->shdr.s_name );
#if OLIST
			curr_section = sectp;
#else
#if M88K
			if (Sflag)
				printf("\n%s\n", &sname[1] );
			else
#endif
				printf("\nsection\t%s\n", sname );
#endif
			next_function = sectp->funcs;
			scnhdr = sectp->shdr;
			dis_text();
		}
		else
			dis_data( sectp );

	/* Now free up all the storage that was allocated for	*/
	/* the lookup of labels, files, functions, and sections */
#if OLIST
	file_free();
#endif
	if (Lflag > 0)
		label_free();
	section_free();
	ldaclose( symb );
	ldaclose( symb_ptr );
	ldaclose( l_ptr );
	ldaclose( t_ptr );

#if OLIST
	putchar( '\014' );
	curfile = NULL;
#endif
}

/*
 *	command_line (argc, argv)
 *
 *	This routine processes the command line received by the disassembler.
 *	Flags and filenames may be in any order.
 */


command_line(argc, argv)
int	argc;
char	*argv[];
{
	/* the following external variables are all from _extn.c */
	extern	int	oflag;
	extern	int	sflag;
	extern	int	trace;
#if !OLIST
	extern	int	fflag;
	extern	NFUNC	ffunction[];
#endif
#if M88K
	extern int	Aflag;
	extern int	Sflag;
#endif
#if M32
	extern int	Sflag;
#endif
	extern	int	Lflag;
	extern	int	lflag;
#ifdef VAX
	extern	int	jflag;
#endif
	extern	char	*libs[];
	extern	short	aflag;
#ifdef OLIST
	extern	char	Sfile[];
#endif
	extern	char	*namedsec[];
	extern	int	namedtype[];
	extern	int	nsecs;

	char	*calloc();
	char	z;
	int	str_1, str_2, str_len;

	char	*p;
	char 	*newp;
	int c, i;
	for (c = 1; c < argc; c++){	/* begin processing command line */
		p = argv[c];
		if (*p == '-'){		/* have one or more flags together */
			for (i = 1; p[i]; i++){ /* more than one flag possible*/
				switch(p[i]){
				case 'T':
					trace++;
					continue;
				case 'o':
					oflag++;
					continue;
#if VAX || N3B || U3B || M32 || NS32000 || M88K
				case 's':
					sflag++;
					continue;
#endif
#if M32 || M88K
				case 'S':
					Sflag++;
					continue;
#endif
				case 'L':
					Lflag++;
					continue;
				case 'l':
					fprintf(stderr,"-%c will be eliminated in the next release\n",p[i]);
					lflag++;
					c++;
					str_1 = sizeof(LIBDIR) - 1 + strlen(LIB);
					str_2 = strlen(argv[c]);
					/* add 2 for '.a'	*/
					str_len = str_1 + str_2 +2;
					if ((libs[lflag] = calloc(str_len+1,sizeof(z))) == NULL) {
						fprintf(stderr, "memory allocation failure on call to calloc");
						exit(0);
					}
					sprintf(libs[lflag],"%s%s%s.a", LIBDIR,LIB,argv[c]);
					if (trace > 0)
						printf("\nlib is {%s}\n", libs[lflag]);
					files++;
					filename[files] = libs[lflag];
					break;
				case 'd':	/* disassemble as a data section flag */
					if (p[i+1] == 'a') {
					/* print addresses of data rather than offsets */
						i++;
						aflag++;
					}
					nsecs++;
					c++;
					namedsec[nsecs] = argv[c];
					namedtype[nsecs] = DATA;
					break;
				case 't':
					nsecs++;
					c++;
					namedsec[nsecs] = argv[c];
					namedtype[nsecs] = TEXT;
					break;
				case 'V':	/* version flag */
					fprintf(stderr,"%s%s: %s\n",SGS,TOOL,
						RELEASE);
					break;
#if !OLIST
				case 'F':
					if (argc < (c + 3)) {
						fprintf(stderr,"Usage is: %sdis ",SGS);
						fprintf(stderr,"-F function file\n");
						exit(2);
					}
					if (fflag == FFUNC)
					{
						fprintf( stderr, "too many -F otpions\n");
						break;
					}
					fflag++;
					c++;
					ffunction[fflag - 1].funcnm = argv[c];
					break;
#endif
#if M88K
				case 'A':
					c++;
					Aflag = (strtol(argv[c],&newp,16));
					i = newp - p - 1;
					break;
				case 'I':
					c++;
					idis(strtol(argv[c],&newp,16));
					i = newp - p - 1;
					printline();
					break;
#endif
#ifdef VAX
				case 'j':
					jflag++;
					break;
#endif
				default:
					fprintf(stderr,"\n%s%s: bad flag %c\n",SGS,TOOL,p[i]);
					break;
				}	/* end of switch */
			}	/* end of multi flag processing */
		}
		else {		/* must be a file name */
#ifdef OLIST
			if ( argv[c][strlen( argv[c] ) -2] == '.' &&
					argv[c][strlen( argv[c] ) - 1] == 'S' ) 
				strcpy( Sfile, argv[c] );
			else
#endif
			{
				files++;
				filename[files] = argv[c];
			}
		}	/* end of processing for this parameter */
	}	/* the entire command line is now processed */
}

#if !OLIST
/*
 * search the symbol table for the named functions and fill in the
 * information in the ffunction array
 */

search_table()
{
	extern	char	*ldgetname();

	extern	LDFILE	*symb;
	extern	FILHDR	filhdr;
	extern	int	fflag;
	extern	NFUNC	ffunction[];

	long	i;
	int	j;
	SYMENT	symbol;
	AUXENT	auxent;
	char	*name;
	FUNCLIST	*funcp;
	SCNLIST		*sectp;

	for ( i = 0; i < filhdr.f_nsyms; i++ )
	{
		if (ldtbread( symb, i, &symbol ) == FAILURE)
			fatal( "cannot read symbol table" );
		if ((name = ldgetname( symb, &symbol )) == NULL)
			fatal( "ldgetname returned NULL" );

		for ( j = 0; j < fflag; j++ ) {
			if (strcmp( name, ffunction[j].funcnm ) == 0)
				break;
#if VAX || CLIPPER || NS32000 || M88K
			else if ((name[0] == '_') &&
				(strcmp(name+1,ffunction[j].funcnm) == 0))
				break;
#endif
		}

		if (j != fflag) {
			for (sectp = sclist; sectp; sectp=sectp->snext)
				if (sectp->scnum == symbol.n_scnum)
					break;
			if ((ISFCN(symbol.n_type) && (symbol.n_numaux > 0)) ||
				((symbol.n_sclass==C_EXT || 
				symbol.n_sclass==C_STAT) &&
				(strncmp(sectp->shdr.s_name,".text",8) == 0) &&
				(strncmp(symbol.n_name,".text",8) != 0) &&
				(strncmp(symbol.n_name,".init",8) != 0))) {

				ffunction[j].faddr = symbol.n_value;
				ffunction[j].fcnindex = i;
				ffunction[j].fnumaux = symbol.n_numaux;
				ffunction[j].found = 1;
				ffunction[j].fscnum = symbol.n_scnum;

				for (funcp=sectp->funcs; funcp; funcp=funcp->nextfunc) {
					if (strcmp(name, funcp->funcnm) == 0)
						break;
				}
				if (funcp->nextfunc != NULL)
					ffunction[j].fsize=funcp->nextfunc->faddr - funcp->faddr;
				else
					ffunction[j].fsize=(sectp->shdr.s_paddr+
				     	    sectp->shdr.s_size) - funcp->faddr;
			}
		}

		i += symbol.n_numaux;
	}
}

/*
 * disassemble the functions in the ffunction array
 */
dis_funcs()
{
	extern	NFUNC	ffunction[];
	extern	int	fflag;
	extern	char	*fname;
	extern	LDFILE	*f_ptr;
	extern	LDFILE	*t_ptr;
	extern	SCNHDR	scnhdr;
	extern	char	*sname;
	extern	FUNCLIST	*next_function;

	int		i;
	FUNCLIST	func;

	for ( i = 0; i < fflag; i++ )
	{
		if (!ffunction[i].found)
		{
			fprintf( stderr, "%sdis: %s: function %s not found\n",
				SGS, fname, ffunction[i].funcnm );
			continue;
		}

		/* find the right section header */
		if (ldshread( f_ptr, ffunction[i].fscnum, &scnhdr ) == FAILURE)
			fatal( "cannot read section headers" );
		sprintf( sname, "%.8s", scnhdr.s_name );
#if M88K	
		if (Sflag)
			printf( "\n%s\n", sname+1 );
		else
#endif
			printf( "\nsection\t%s\n", sname );

		/*
		 * seek to the start of the function, and change the section
		 * header to fake out dis_text (make it think that the section
		 * contains just that one function
		 */
		FSEEK( t_ptr, scnhdr.s_scnptr + (ffunction[i].faddr - scnhdr.s_paddr), BEGINNING );
		scnhdr.s_scnptr += (ffunction[i].faddr - scnhdr.s_paddr);
		scnhdr.s_paddr = ffunction[i].faddr;
		scnhdr.s_size = ffunction[i].fsize;
		if ((ldlinit(f_ptr,ffunction[i].fcnindex)) == FAILURE) {
			scnhdr.s_lnnoptr = NULL;
			scnhdr.s_nlnno = 0;
		}

		func.funcnm = ffunction[i].funcnm;
		func.faddr = ffunction[i].faddr;
		func.fcnindex = ffunction[i].fcnindex;
		func.fnumaux = ffunction[i].fnumaux;
		func.nextfunc = NULL;
		next_function = &func;
		dis_text();
	}
}
#endif
