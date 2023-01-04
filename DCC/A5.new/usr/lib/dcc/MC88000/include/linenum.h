#ifndef __Ilinenum
#define __Ilinenum

#if m88k

struct lineno {				/* modified COFF		 */
	union {
		long	l_symndx;	/* l_lnno==0: symtab index of fn */
		long	l_paddr;	/* address of line number	 */
	}		l_addr;
	unsigned long	l_lnno;		/* line number			 */
};

#define	LINENO	struct lineno
#define	LINESZ	sizeof(LINENO)

#else

struct lineno {				/* standard COFF		 */
	union {
		long	l_symndx;	/* l_lnno==0: symtab index of fn */
		long	l_paddr;	/* address of line number	 */
	}		l_addr;
	unsigned short	l_lnno;		/* line number			 */
};

#define	LINENO	struct lineno
#define	LINESZ	6	/* sizeof(LINENO) (sometimes) */

#endif

#endif
