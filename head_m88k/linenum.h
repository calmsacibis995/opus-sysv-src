#ifndef _LINENUM_H_
#define _LINENUM_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

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
#if __ALPHA
	unsigned short	l_lnno ;	/* line number */
	char		l_pad1;		/* pad to 4 byte multiple */
	char		l_pad2;		/* pad to 4 byte multiple */
#else
	unsigned long	l_lnno ;	/* line number */
#endif
} ;

#define	LINENO	struct lineno
#define	LINESZ	8

#endif	/* ! _LINENUM_H_ */
