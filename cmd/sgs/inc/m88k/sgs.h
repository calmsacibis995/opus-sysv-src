#ident "@(#) $Header: sgs.h 4.1 88/04/26 17:11:15 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 */

#define	SGS	""

/*	The MAGIC symbol is defined in filehdr.h	*/

#define MAGIC	MC88MAGIC
#define TVMAGIC (MAGIC+1)

#define ISMAGIC(x)	(x ==  MAGIC)


#ifdef ARTYPE
#define	ISARCHIVE(x)	( x ==  ARTYPE)
#define BADMAGIC(x)	((((x) >> 8) < 7) && !ISMAGIC(x) && !ISARCHIVE(x))
#endif


/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default
 *		PAGEMAGIC  : configured for paging
 */

#define AOUT1MAGIC 0407
#define AOUT2MAGIC 0410
#define PAGEMAGIC  0413
#define PAGEMAGIC2 0414	/* Paged split I&D file (address zero unmapped) */
#define PAGEMAGIC3 0415	/* Paged split I&D file (address zero mapped) */
#define LIBMAGIC   0443



/*
 * MAXINT defines the maximum integer for printf.c
 */

#define MAXINT	2147483647

#define	SGSNAME	""
#define	RELEASE	"C Compilation System, Issue 4.0 (11.0) 1/3/86"
