/*
 * static char ID_lnumh[] = "inc/common/linenum.h: 1.1 1/7/82";
 */

/*  There is one line number entry for every 
    "breakpointable" source line in a section.
    Line numbers are grouped on a per function
    basis; the first entry in a function grouping
    will have l_lnno = 0 and in place of physical
    address will be the symbol table index of
    the function name.
*/
struct lineno
{
	union
	{
		long	l_symndx ;	/* sym. table index of function name
						iff l_lnno == 0      */
		long	l_paddr ;	/* (physical) address of line number */
	}		l_addr ;
	unsigned short	l_lnno ;	/* line number */
#if (defined(TARGET) && TARGET==m88k) || defined(m88k)
	char		l_pad1;		/* pad to 4 byte multiple */
	char		l_pad2;		/* pad to 4 byte multiple */
#endif
} ;

#define	LINENO	struct lineno
#define	LINESZ	6	/* sizeof(LINENO) */
