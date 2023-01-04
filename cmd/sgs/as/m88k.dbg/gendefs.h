/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:m88k/gendefs.h	1.10" */

#define  LESS		(-1)
#define  EQUAL    	(0)
#define  GREATER  	(1)

#define  NO       	(0)
#define  YES      	(1)

#define NCPS		(8)		/* number of characters per symbol */
#define BITSPBY		(8)
#define BITSPOW		(8)
#define BITSPW		(32)
#define OUTWTYPE	char

/*
 *	if the previous line changes the following line should be change to 
 *	look like
 * 	#define	OUT(a,b)	fwrite((char *)(&a), sizeof(OUTWTYPE), 1, b)
 */

#define	OUT(a,b)	putc((a), (b))
#define	MAX(a,b)	((a) > (b)) ? (a) : (b)

#define NBPW		(32)

#define SCTALIGN 	(16L)		/* byte alignment for sections */
#define TXTNOP		(0xF4005800)	/* m88k "or r0,r0,r0" instruction */
#define TXTFILL		(0L)
#define FILL 		(0L)
#define NULLVAL 	(0L)
#define NULLSYM 	((symbol *)NULL)

/* constants used in testing and flag parsing */

#define ARGERR		"Illegal argument - ignored\n"
#define TESTVAL		(-2)
#define NSECTIONS	(9)

/* number of section files + other files */

#define NFILES		(NSECTIONS + 6)

/* index of action routines in modes array */

#define	NOACTION	(0)
#define	DEFINE		(1)
#define	SETVAL		(2)
#define	SETSCL		(3)
#define	SETTYP		(4)
#define	SETTAG		(5)
#define	SETLNO		(6)
#define	SETSIZ		(7)
#define	SETDIM1		(8)
#define	SETDIM2		(9)
#define	ENDEF		(10)
#define	LLINENO		(11)
#define	LLINENUM	(12)
#define	LLINEVAL	(13)
#define	NEWSTMT		(14)
#define	SETFILE		(15)
#define	LVRT16		(16)	/* Direct referrence, low 16 bits of 32 */
#define	RESABS		(17)
#define	RELPC8		(18)
#define	UNUSED_19	(19)	/* Unused */
#define	UNUSED_20	(20)	/* Unused */
#define UNUSED_21	(21)	/* Unused */
#define FNDBRLEN	(22)
#define	SHIFTVAL	(23)
#define	VRT32		(24)	/* Direct reference, 32 bits */
#define	PCR16L		(25)	/* PC relative, 16 bits, long word aligned */
#define SETEXPAND	(26)
#define	DOTZERO		(27)
#define PCR26L		(28)	/* PC relative, 26 bits, long word aligned */
#define VRT16		(29)	/* Direct reference, 16 bits */
#define HVRT16		(30)	/* Direct references, high 16 bits of 32 */
#define UNUSED_31	(31)	/* Unused */

#ifdef CALLPCREL
#    define	CALLNOPT	(32)
#    define	NACTION		(32)
#else
#    define	NACTION		(31)
#endif
