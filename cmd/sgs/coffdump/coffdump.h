#ident "@(#) $Header: coffdump.h 4.1 88/04/26 17:02:41 root Exp $ RISC Source Base"
/*
 *	Configuration parameters, can be tuned if desired.
 */
 
#define BYTES_PER_LINE	16	/* Bytes dumped per screen line */

/*
 *	Defines for built in debugger macros.
 *	If the macro based debugger package (dbug) is not available
 *	then define NO_DBUG at compilation time.  I.E.:
 *
 *		cc -DNO_DBUG -c *.c
 *
 */
 
#ifndef NO_DBUG
#    include <dbug.h>
#else
#    define DBUG_ENTER(a1)
#    define DBUG_RETURN(a1) return(a1)
#    define DBUG_VOID_RETURN return
#    define DBUG_EXECUTE(keyword,a1)
#    define DBUG_2(keyword,format)
#    define DBUG_3(keyword,format,a1)
#    define DBUG_4(keyword,format,a1,a2)
#    define DBUG_5(keyword,format,a1,a2,a3)
#    define DBUG_PUSH(a1)
#    define DBUG_POP()
#    define DBUG_PROCESS(a1)
#    define DBUG_FILE (stderr)
#endif

/*
 *	Error codes.
 */
 
#define ERR_LDOPEN	1		/* Can't open file */
#define ERR_SECTHEAD	2		/* Can't read section header */
#define ERR_RELOC	3		/* Can't read relocation */
#define ERR_LNNO	4		/* Can't read line number entry */
#define ERR_SYMR	5		/* Can't read a symbol table entry */


/*
 *	Typedefs to make things more portable and clearer.
 */
 
#define VOID void		/* Typedef bug? */

typedef int BOOLEAN;

typedef char CHAR;
typedef long LONG;
typedef unsigned long ULONG;
typedef int INT;
typedef unsigned int UINT;
typedef short SHORT;
typedef unsigned short USHORT;

/*
 *	Manifest constants.
 */

#define EOS '\000'		/* End of string marker */

#define FALSE	0
#define TRUE	1
#define BRIEF	2
#define VERBOSE	3


/*
 *	Some globally useful macros.
 */
 
#define TRANSPAIR(x) "x",x		/* Name and value pair */

/*
 *	Command line flags are grouped into a single structure.
 */

struct cmdflags {
    BOOLEAN cflag;		/* Dump the string table */
    BOOLEAN fflag;		/* Dump each file header */
    BOOLEAN hflag;		/* Dump section headers */
    BOOLEAN lflag;		/* Dump line number information */
    BOOLEAN oflag;		/* Dump each optional header */
    BOOLEAN rflag;		/* Dump relocation information */
    BOOLEAN sflag;		/* Dump section contents */
    BOOLEAN tflag;		/* Dump symbol table entries */
    BOOLEAN Hflag;		/* Give help info */
    BOOLEAN Dflag;		/* Output will be used in diff */
};

extern struct cmdflags flags;	/* Used practically everywhere */
