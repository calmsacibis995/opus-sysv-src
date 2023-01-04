struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of line number entries */
	long		s_flags;	/* flags */
#if ns32000
	long            s_symptr;	/* file pointer to symbol table entries*/
	unsigned short  s_modno;        /* module number of module associated
					   with this section  */
	short		s_pad;		/* padding to 4 byte multiple */
#endif /* ns32000 */
	};

/* the number of shared libraries in a .lib section in an absolute output
 * file is put in the s_paddr field of the .lib section header, the 
 * following define allows it to be referenced as s_nlib.
 */

#define s_nlib  s_paddr
#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)



#if clipper

/*
 * Define constants for names of "special" sections
 */

#define _TEXT ".text"
#define _DATA ".data"
#define _BSS  ".bss"
#define _TV  ".tv"
#define _INIT ".init"
#define _FINI ".fini"
#define _EXPR ".expr"
#define _COMMENT ".comment"

/*
 * The low 2 bytes of s_flags is used as a section "type"
 */

#define STYP_REG	0x00		/* "regular" section:
						allocated, relocated, loaded */
#define STYP_DSECT	0x01		/* "dummy" section:
						not allocated, relocated,
						not loaded */
#define STYP_NOLOAD	0x02		/* "noload" section:
						allocated, relocated,
						 not loaded */
#define STYP_GROUP	0x04		/* "grouped" section:
						formed of input sections */
#define STYP_PAD	0x08		/* "padding" section:
						not allocated, not relocated,
						 loaded */
#define STYP_COPY	0x10		/* "copy" section:
						for decision function used
						by field update;  not
						allocated, not relocated,
						loaded;  reloc & lineno
						entries processed normally */
#define STYP_INFO	0x200		/* comment section : not allocated
						not relocated, not loaded */
#define STYP_LIB	0x800		/* for .lib section : same as INFO */
#define STYP_OVER	0x400		/* overlay section : relocated
						not allocated or loaded */
#define	STYP_TEXT	0x20		/* section contains text only */
#define STYP_DATA	0x40		/* section contains data only */
#define STYP_BSS	0x80		/* section contains bss only */

/*
 *  In a minimal file or an update file, a new function
 *  (as compared with a replaced function) is indicated by S_NEWFCN
 */

#define S_NEWFCN  0x100

/*
 * In 3b Update Files (output of ogen), sections which appear in SHARED
 * segments of the Pfile will have the S_SHRSEG flag set by ogen, to inform
 * dufr that updating 1 copy of the proc. will update all process invocations.
 */

#define S_SHRSEG	0x20

#else

/*
 * Define constants for names of "special" sections
 */

#define _TEXT ".text"
#define _DATA ".data"
#define _BSS  ".bss"
#define _LINK ".link"
#define _MOD  ".mod"
#define _TV ".tv"
#define _SB ".sb"
#define _STATIC ".static"
#define _INIT ".init"
#define _FINI ".fini"


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
#define STYP_MOD        0x100           /* section contains module table only*/
#define STYP_LINK       0x200           /* section contains link table only  */
#define STYP_RELOC	0x400		/* relocatable, cannot be melded (linked
					  	linked using NS16000 modular
					   	software features            */
#define STYP_COLLAPSE   0x800         /* modules can/cannot be collapsed
						when processed by linker     */

#define STYP_PROT       0x3000          /* protection assigned section when
					   loaded into memory                */
#define STYP_INFO       0x4000           /* comment section : not allocated
						not relocated, not loaded    */
#define STYP_OVER       0x8000           /* overlay section : relocated
						not allocated or loaded      */
#define STYP_LIB        0x10000           /* for .lib section : same as INFO
						when processed by linker     */


/*
 *  In a minimal file or an update file, a new function
 *  (as compared with a replaced function) is indicated by S_NEWFCN
 */

#define S_NEWFCN  0x10

#endif /* clipper */
