/*	inc/common/aouthdr.h: 1.2 12/13/82	*/

#if clipper
#define	CLICDFL	0	/* default caching strategy	*/
#define CLICNCA	1	/* noncacheable			*/
#define CLICCWT	2	/* shared, write-thru		*/
#define CLICPWT	3	/* private, write-thru		*/
#define CLICPCB	4	/* private, copy-back		*/
#endif

typedef	struct aouthdr {
	short	magic;		/* magic number				*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes, padded to FW bdry*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
#if ns32000
	long	msize;          /* size of module table                 */
	long	mod_start;      /* start address of module table        */
#endif
	long	entry;		/* entry pt.				*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
#if clipper
	unsigned long cliflags;	/* CLIPPER flags			*/
	unsigned char tcache;	/* text region caching policy		*/
	unsigned char dcache;	/* data region caching policy		*/
	unsigned char scache;	/* stack region caching policy		*/
#endif
#if ns32000
	unsigned short  entry_mod; /* module number of entry point      */
	unsigned short	flags;	/* flags */
#endif
} AOUTHDR;

#if ns32000
/*
 * Optional header flags
 *
 *	U_AL		section alignment
 *	U_AL_NONE	fullword   "
 *	U_AL_512	512 byte   "
 *	U_AL_1024	1K	   "
 *	U_AL_2048	2K	   "
 *	U_AL_4096	4K	   "
 *	U_AL_8192	8K	   "
 *	U_PR		section protections
 *	U_PR_DATA	data section
 *	U_PR_TEXT	text    "
 *	U_PR_MOD	module  "
 */

#define U_AL		0x07
#define U_AL_NONE	0x00
#define U_AL_512	0x01
#define U_AL_1024	0x02
#define U_AL_2048	0x03
#define U_AL_4096	0x04
#define U_AL_8192	0x05
#define U_PR		0x38
#define U_PR_DATA	0x08
#define U_PR_TEXT	0x10
#define U_PR_MOD	0x20
#define U_SYS_5   0x100 /* default file for system V */
#define U_SYS_4BSD 0x200 /* default file for Genix 4BSD coff */
#define U_SYS_42 U_SYS_4BSD	/* for historical reasons */



/*
 *	When a UNIX aout header is built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC:	     : writable text segment
 *		PAGEMAGIC  :	     : configured for paging
 *		LIBMAGIC   :         : configured for shared libraries
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407
#define PAGEMAGIC  0413
#define AOUT3MAGIC 0417 /* not default of either 4.2 or systm V */
#define LIBMAGIC   0443
#endif /* ns32000 */
