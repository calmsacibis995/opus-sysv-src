#ident "@(#) $Header: coffcmp.h 4.1 88/04/26 17:02:15 root Exp $ RISC Source Base"
#include <filehdr.h>

# ifndef NO_DBUG
#    include <dbug.h>
# else
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
# endif

/*
 *	Error codes.
 */
 
#define ERR_OPEN	1		/* Can't open file */
#define ERR_SEEK	2		/* Can't seek in file */
#define ERR_DIFF	3		/* Found a difference */


/*
 *	Typedefs to make things more portable and clearer.
 */
 
#define VOID void		/* Typedef bug? */

typedef char CHAR;
typedef long LONG;
typedef unsigned long ULONG;
typedef int INT;
typedef unsigned int UINT;
typedef short SHORT;
typedef unsigned short USHORT;

typedef INT BOOLEAN;			/* TRUE or FALSE */
typedef LONG OFFSET;			/* Offset into a file */

/*
 *	Manifest constants.
 */

#define BEGINNING	(0)		/* Seek from beginning */
#define SEEK_OK		(0)		/* Fseek return */
#define EOS		'\000'		/* End of string marker */

#define FALSE	0
#define TRUE	1
#define BRIEF	2
#define VERBOSE	3

/*
 *	Error exit codes.
 */

#define OK_EXIT		(0)	/* No differences or errors */
#define ERROR_EXIT	(1)	/* Error or different */

/*
 *	Offsets.
 */

#define MAX_OFFSET	(0x7FFFFFFF)	/* Maximum possible offset */
#define TO_EOF		(-1L)		/* Go to end of file */

#define LOW1	0L
#define HIGH1	(OFFSET)(sizeof(dummy.f_magic) + sizeof(dummy.f_nscns))
#define LOW2	(OFFSET)(HIGH1 + sizeof(dummy.f_timdat))
#define HIGH2	TO_EOF

/*
 *	Structure for maintaining file information.
 */

struct finfo {
    FILE *fp;			/* Stream pointer for I/O */
    CHAR *fname;		/* Name of file */
};

