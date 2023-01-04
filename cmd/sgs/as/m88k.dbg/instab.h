/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: instab.h 2.2 88/04/08 14:05:59 root Exp $ RISC Source Base"

/*
 *	Bits for special processing as used in ops.in; end up in
 *	tag field of mnemonic structure.
 */

#define O_N		0x0001

/* Other stuff */

#define SHFT_MSK 	(0x1F)
#define INST_MSK 	(0x3f)
#define	DUMMY		(0x01)

#if FLOAT
#    define INSTRS	(0x0e)
#    define INSTRD	(0x0f)
#    define INSTRX	(0x10)
#endif

#if FLOAT
#    define INSTRSD	(0x1b)
#    define INSTRSX	(0x1c)
#    define INSTRDS	(0x1d)
#    define INSTRDX	(0x1e)
#    define INSTRXS	(0x1f)
#    define INSTRXD	(0x20)
#endif

#define CREGDFMD	(0x5)	/* register deferred mode */
#define CFPDSPMD	(0x6)	/* FP displacement mode */
#define CAPDSPMD	(0x7)	/* AP displacement mode */
#define CDSPMD		(0x8)	/* displacement mode */
#define CDSPDFMD	(0x9)	/* displacement deferred mode */
#define CHDSPMD		(0xA)	/* halfword displacement mode */
#define CHDSPDFMD	(0xB)	/* halfword displacement deferred mode */
#define CBDSPMD		(0xC)	/* byte displacement mode */
#define CBDSPDFMD	(0xD)	/* byte displacement deferred mode */
#define CEXPAND		(0xE)	/* expand byte */
#define CABSMD		(0x7F)	/* absolute address mode */
#define CABSDFMD	(0xEF)	/* absolute address deferred mode */

#if FLOAT
#    define FPIMMD	(0x13)	/* floating point immediate mode */
#    define DECIMMD	(0x14)	/* floating point immediate mode */
#    define CDIMMD	(0xCF)	/* floating point double immediate mode */
#endif

#define FPREG		(0x9)	/* FP register number */
#define APREG		(0xA)	/* AP register number */
#define	PSWREG		(0xB)	/* PSW register number */
#define SPREG		(0xC)	/* SP register number */
#define	PCREG		(0xF)	/* PC register number */

#define UBYTE		(0x3)
#define SBYTE		(0x7)
#define UHALF		(0x2)
#define SHALF		(0x6)
#define UWORD		(0x0)
#define SWORD		(0x4)
#define NOTYPE		(0xF)	/* no new type (i.e. {...}) specified */

#define JMPOPCD 	(0x24L)

#define	NULLSPEC	(0x00)
#define BYTESPEC	(0x01)
#define	HALFSPEC	(0x02)
#define	WORDSPEC	(0x03)
#define	LITERALSPEC	(0x04)
#define	SHORTSPEC	(0x05)
