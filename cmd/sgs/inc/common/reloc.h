/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)head:reloc.h	2.6"
struct reloc {
	long	r_vaddr;	/* (virtual) address of reference */
	long	r_symndx;	/* index into symbol table */
	unsigned short	r_type;		/* relocation type */
#if (defined(TARGET) && TARGET==m88k) || defined(m88k)
	unsigned short r_offset;	/* hi16 bits of constant in hi16 or */
#endif					 /* lo16 expression. */
	};


/*
 *   relocation types for all products and generics
 */

/*
 * All generics
 *	reloc. already performed to symbol in the same section
 */
#define  R_ABS		0

/*
 * X86 generic
 *	8-bit offset reference in 8-bits
 *	8-bit offset reference in 16-bits 
 *	12-bit segment reference
 *	auxiliary relocation entry
 */
#define	R_OFF8		07
#define R_OFF16		010
#define	R_SEG12		011
#define	R_AUX		013

/*
 * B16 and X86 generics
 *	16-bit direct reference
 *	16-bit "relative" reference
 *	16-bit "indirect" (TV) reference
 */
#define  R_DIR16	01
#define  R_REL16	02
#define  R_IND16	03

/*
 * 3B generic
 *	24-bit direct reference
 *	24-bit "relative" reference
 *	16-bit optimized "indirect" TV reference
 *	24-bit "indirect" TV reference
 *	32-bit "indirect" TV reference
 */
#define  R_DIR24	04
#define  R_REL24	05
#define  R_OPT16	014
#define  R_IND24	015
#define  R_IND32	016

/*
 * 3B and M32 || u3b15 || u3b5 || u3b2 generics
 *	32-bit direct reference
 */
#define  R_DIR32	06

/*
 * M32 || u3b15 || u3b5 || u3b2 generic
 *	32-bit direct reference with bytes swapped
 */
#define  R_DIR32S	012

/*
 * XL generics
 *	10-bit direct reference
 *	10-bit "relative" reference
 *	32-bit "relative" reference
 */
#define	R_DIR10	025
#define	R_REL10	026
#define	R_REL32	027

/*
 *  GENIX environment for sys16 
 */

#define  R_ADDRTYPE    0x000f      /* type of relocation item               */
#define   R_NOTHING      0x0000    /* ignore                                */
#define   R_ADDRESS      0x0001    /* take address of symbol                */
#define   R_LINKENTRY    0x0002    /* take symbol's link entry index        */

#define  R_RELTO       0x00f0      /* action to take when relocating        */
#if nsc
#define   R_ABS          0x0000    /* keep symbol's address as such         */
#endif
#define   R_PCREL        0x0010    /* subtract the pc address of hole       */
#define   R_SBREL        0x0020    /* subtract the static base start of this*/
				   /* section's module                      */

#define  R_FORMAT      0x0f00      /* relocation item data format           */
#define   R_NUMBER       0x0000    /* retain as two's complement value      */
#define   R_DISPL        0x0100    /* convert to NS16000 displacement       */
#define   R_PROCDES      0x0200    /* convert to NS16000 procedure descriptor*/
#define   R_IMMED        0X0300    /* for NS16000 immediate addressing mode */

#define  R_SIZESP      0xf000      /* relocation item size                  */
#define   R_S_08         0x0000    /* relocate 8 bits                       */
#define   R_S_16         0x1000    /* relocate 16 bits                      */
#define   R_S_32         0x2000    /* relocate 32 bits                      */


/* Musking for the relocated holes */

#define   R_HBYTE       0x00       /* relocated filed is one byte           */
#define   R_HWORD       0x8000     /* relocated field is one word           */
#define   R_HLONG       0xc0000000 /* relocated field is long              */


/*
 * DEC Processors  VAX 11/780 and VAX 11/750
 *
 */

#define R_RELBYTE	017
#define R_RELWORD	020
#define R_RELLONG	021
#define R_PCRBYTE	022
#define R_PCRWORD	023
#define R_PCRLONG	024

/*
 * Motorola 68000
 *
 * ... uses R_RELBYTE, R_RELWORD, R_RELLONG, R_PCRBYTE and R_PCRWORD as for
 * DEC machines above.
 */

/*
 * Motorola 88000 (Risc architecture)
 *
 * Since AT&T has no "hot-line" to call to request reservation of new
 * relocation types, these numbers were pulled out of thin air.  Also,
 * since there is no definitive specification about what the existing
 * values mean we do not use any of them.  The syntax of the entries
 * is:
 *
 *	R_[H|L]<VRT|PCR><8|16|26|32>[L]
 *
 * where:
 *
 *	[]	means optional fields
 *	<>	mandatory fields, select one
 *	|	alternation, select one
 *	H	use the high 16 bits of symbol's virtual address
 *	L	use the low 16 bits of symbol's virtual address
 *	VRT	use the symbol's virtual address
 *	PCR	use symbol's virtual address minus value of PC
 *
 * Currently used relocation types are:
 *
 * R_PCR16L
 *
 *	PC relative, 16 bits of the 16 bit field at offset r_vaddr, longword
 *	aligned.  I.E., the 16 bits represent the top 16 bits of an 18 bit
 *	byte offset from the current PC.
 *
 * R_PCR26L
 *
 *	PC relative, lower 26 bits of the 32 bit field at offset r_vaddr,
 *	longword aligned.  I.E., the 26 bits represent the top 26 bits of
 *	a 28 bit byte offset from the current PC.
 *
 * R_VRT16
 *
 *	Reference to a symbol at an absolute address in the virtual address
 *	space, 16 bits of the 16 bit field at offset r_vaddr.  If the
 *	relocation value will not fit in 16 bits an error or warning
 *	is generated and zero is used as the relocation value.
 *
 * R_HVRT16
 *
 *	Same as R_VRT16 except that the high 16 bits of the absolute address
 *	are used as the relocation value.
 *
 * R_LVRT16
 *
 *	Same as R_VRT16 except that the low 16 bits of the absolute address
 *	are used as the relocation value and it is not an error to have
 *	overflow.
 *	
 * R_VRT32
 *
 *	Reference to a symbol at an absolute address in the virtual address
 *	space, 32 bits of the 32 bit field at offset r_vaddr.
 *
 */

#define R_PCR16L	128
#define R_PCR26L	129
#define R_VRT16		130
#define R_HVRT16	131
#define R_LVRT16	132
#define R_VRT32		133

#define	RELOC	struct reloc
#define	RELSZ	10	/* sizeof(RELOC) */

	/* Definition of a "TV" relocation type */

#if N3B
#define ISTVRELOC(x)	((x==R_OPT16)||(x==R_IND24)||(x==R_IND32))
#endif
#if B16 || X86
#define ISTVRELOC(x)	(x==R_IND16)
#endif
#if M32 || u3b15 || u3b5 || u3b2 || clipper || ns32000 || m88k
#define ISTVRELOC(x)	(x!=x)	/* never the case */
#endif
