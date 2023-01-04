#ifndef _SCNHDR_H_
#define _SCNHDR_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
	unsigned long	s_nreloc;	/* number of relocation entries */
	unsigned long	s_nlnno;	/* number of line number entries */
	long		s_flags;	/* flags */
	};

/*
 * The low 20 bits of s_flags is used as a section "type"
 */

#define STYP_REG	0x00		/* "regular" section:
						allocated, relocated, loaded */
#define STYP_DSECT	0x01		/* "dummy" section:
						not allocated, relocated,
						not loaded                   */
#define STYP_NOLOAD	0x02		/* "noload" section:
						allocated, relocated,
						 not loaded                  */
#define STYP_GROUP	0x04		/* "grouped" section:
						formed of input sections     */
#define STYP_PAD	0x08		/* "padding" section:
						not allocated, not relocated,
						 loaded                      */
#define STYP_COPY	0x10		/* "copy" section:
						for decision function used by
						field update;  not allocated,
						not relocated, loaded;
						relocation & line number
						entries processed normally   */
#define	STYP_TEXT	0x20		/* section contains text only        */
#define STYP_DATA	0x40		/* section contains data only        */
#define STYP_BSS	0x80		/* section contains bss only         */
#define STYP_INFO       0x200          /* comment section : not allocated
						not relocated, not loaded    */
#define STYP_OVER       0x400           /* overlay section : relocated
						not allocated or loaded      */
#define STYP_LIB        0x800           /* for .lib section : same as INFO
						when processed by linker     */
#define	STYP_VENDOR	0x1000

#ifndef __88000_OCS_DEFINED

/* the number of shared libraries in a .lib section in an absolute output
 * file is put in the s_paddr field of the .lib section header, the 
 * following define allows it to be referenced as s_nlib.
 */

#define s_nlib  s_paddr
#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

/*
 * Define constants for names of "special" sections
 */

#define _TEXT ".text"
#define _DATA ".data"
#define _BSS  ".bss"
#define _INIT ".init"
#define _FINI ".fini"
#define _VENDOR ".vendor"

/*
 *  In a minimal file or an update file, a new function
 *  (as compared with a replaced function) is indicated by S_NEWFCN
 */

#define S_NEWFCN  	0x10

#define S_SHRSEG	0x20

#define s_vendor	s_nlnno
#define _TV ".tv"
#define _SB ".sb"
#define _STATIC ".static"
#define _COMMENT ".comment"
#endif	/* !__88000_OCS_DEFINED */

#endif	/* ! _SCNHDR_H_ */
