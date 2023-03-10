/*
 *	The following two declarations appear in the IH versions of
 *	"stdio.h" but do not appear in the normal 1.2 versions.
 */

long ftell();
char *fgets();

#ifndef LDFILE
struct	ldfile {
	int	_fnum_;		/* so each instance of an LDFILE is unique */
	FILE	*ioptr;		/* system I/O pointer value */
	long	offset;		/* absolute offset to the start of the file */
	FILHDR	header;		/* the file header of the opened file */
	unsigned short	type;		/* indicator of the type of the file */
};


/*
	provide a structure "type" definition, and the associated
	"attributes"
*/

#define	LDFILE		struct ldfile
#define IOPTR(x)	x->ioptr
#define OFFSET(x)	x->offset
#define TYPE(x)		x->type
#define	HEADER(x)	x->header
#define LDFSZ		sizeof(LDFILE)

/*
	define various values of TYPE(ldptr)
*/

#define USH_ARTYPE	(unsigned short) ARTYPE
#define ARTYPE 	0177545

#if CCF
#define ISARCHIVE(x)	((((unsigned short)x)==(unsigned short)ARTYPE))
#define BADMAGIC(x)	((((x)>>8) < 7) && !ISMAGIC(x) && !ISARCHIVE(x))
#endif

/*
	define symbolic positioning information for FSEEK (and fseek)
*/

#define BEGINNING	0
#define CURRENT		1
#define END		2

/*
	define a structure "type" for an archive header
*/

typedef struct
{
	char ar_name[16];
	long ar_date;
	int ar_uid;
	int ar_gid;
	long ar_mode;
	long ar_size;
} archdr;

#define	ARCHDR	archdr
#define ARCHSZ	sizeof(ARCHDR)


/*
	define some useful symbolic constants
*/

#define SYMTBL	0	/* section nnumber and/or section name of the Symbol Table */

#define	SUCCESS	 1
#define	CLOSED	 1
#define	FAILURE	 0
#define	NOCLOSE	 0
#define	BADINDEX	-1L

#define	OKFSEEK	0

/*
	define macros to permit the direct use of LDFILE pointers with the
	standard I/O library procedures
*/

LDFILE *ldopen();
LDFILE *ldaopen();

#define GETC(ldptr)	getc(IOPTR(ldptr))
#define GETW(ldptr)	getw(IOPTR(ldptr))
#define FEOF(ldptr)	feof(IOPTR(ldptr))
#define FERROR(ldptr)	ferror(IOPTR(ldptr))
#define FGETC(ldptr)	fgetc(IOPTR(ldptr))
#define FGETS(s,n,ldptr)	fgets(s,n,IOPTR(ldptr))
#define FILENO(ldptr)	fileno(IOPTR(ldptr))
/* changed from FREAD to avoid a conflict in <sys/file.h>	rwu */
#define LDFREAD(p,s,n,ldptr)	fread(p,s,n,IOPTR(ldptr))
#if clipper || ns32000 || m88k
#define FREAD(p,s,n,ldptr)	fread(p,s,n,IOPTR(ldptr))
#endif
#define FSEEK(ldptr,o,p)	fseek(IOPTR(ldptr),(p==BEGINNING)?(OFFSET(ldptr)+o):o,p)
#define FTELL(ldptr)	ftell(IOPTR(ldptr))
/* changed from FWRITE to avoid a conflict in <sys/file.h>	rwu */
#define LDFWRITE(p,s,n,ldptr)       fwrite(p,s,n,IOPTR(ldptr))
#if clipper || ns32000 || m88k
#define FWRITE(p,s,n,ldptr)       fwrite(p,s,n,IOPTR(ldptr))
#endif
#define REWIND(ldptr)	rewind(IOPTR(ldptr))
#define SETBUF(ldptr,b)	setbuf(IOPTR(ldptr),b)
#define UNGETC(c,ldptr)		ungetc(c,IOPTR(ldptr))
#define STROFFSET(ldptr)	(HEADER(ldptr).f_symptr + HEADER(ldptr).f_nsyms * SYMESZ) /* 20 == SYMESZ */
#endif
