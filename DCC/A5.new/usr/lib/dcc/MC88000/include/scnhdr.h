#ifndef __Iscnhdr
#define __Iscnhdr

#if m88k

struct scnhdr {				/* modified COFF	*/
	char		s_name[8];	/* section name		*/
	long		s_paddr;	/* physical address	*/
	long		s_vaddr;	/* virtual address	*/
	long		s_size;		/* size of section	*/
	long		s_scnptr;	/* fileptr to raw data	*/
	long		s_relptr;	/* fileptr to reloc	*/
	long		s_lnnoptr;	/* fileptr to lineno	*/
	unsigned long	s_nreloc;	/* reloc count		*/
	unsigned long	s_nlnno;	/* lnno count		*/
	long		s_flags;	/* flags		*/
};

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

#else

struct scnhdr {				/* standard COFF	*/
	char		s_name[8];	/* section name		*/
	long		s_paddr;	/* physical address	*/
	long		s_vaddr;	/* virtual address	*/
	long		s_size;		/* size of section	*/
	long		s_scnptr;	/* fileptr to raw data	*/
	long		s_relptr;	/* fileptr to reloc	*/
	long		s_lnnoptr;	/* fileptr to lineno	*/
	unsigned short	s_nreloc;	/* reloc count		*/
	unsigned short	s_nlnno;	/* lnno count		*/
	long		s_flags;	/* flags		*/
};

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

#endif

/*	The low 4 bits of s_flags is used as a section "type"	*/

#define STYP_REG	0x00		/* "regular" section	*/
#define STYP_DSECT	0x01		/* "dummy" section	*/
#define STYP_NOLOAD	0x02		/* "noload" section	*/
#define STYP_GROUP	0x04		/* "grouped" section	*/
#define STYP_PAD	0x08		/* "padding" section	*/
#define STYP_COPY	0x10		/* "copy" section	*/
#define	STYP_TEXT	0x20		/* section contains text*/
#define STYP_DATA	0x40		/* section contains data*/
#define STYP_BSS	0x80		/* section contains bss	*/
#define STYP_INFO	0x200
#define STYP_OVER	0x400
#define STYP_LIB	0x800
#define STYP_VENDOR	0x1000

#define S_NEWFCN	0x10
#define S_SHRSEG	0x20

#endif
