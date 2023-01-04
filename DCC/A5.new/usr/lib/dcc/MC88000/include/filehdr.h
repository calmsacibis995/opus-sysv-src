#ifndef __Ifilehdr
#define __Ifilehdr

struct filehdr {
	unsigned short	f_magic;	/* magic		*/
	unsigned short	f_nscns;	/* number of sections	*/
	long		f_timdat;	/* date stamp		*/
	long		f_symptr;	/* fileptr to symtab	*/
	long		f_nsyms;	/* symtab count		*/
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags		*/
};


/*
 *   Bits for f_flags:
 *
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable  (i.e. no unresolved
 *				externel references)
 *	F_LNNO		line nunbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_MINMAL	this is a minimal object file (".m") output of fextract
 *	F_UPDATE	this is a fully bound update file, output of ogen
 *	F_SWABD		this file has had its bytes swabbed (in names)
 *	F_AR16WR	this file has the byte ordering of an AR16WR (e.g. 11/70) machine
 *				(it was created there, or was produced by conv)
 *	F_AR32WR	this file has the byte ordering of an AR32WR machine(e.g. vax)
 *	F_AR32W		this file has the byte ordering of an AR32W machine (e.g. 3b,maxi)
 *	F_PATCH		file contains "patch" list in optional header
 *	F_NODF		(minimal file only) no decision functions for
 *				replaced functions
 */

#define  F_RELFLG	0000001		/* reloc info stripped		*/
#define  F_EXEC		0000002		/* file is executable		*/
#define  F_LNNO		0000004		/* lnno stripped		*/
#define  F_LSYMS	0000010		/* locals stripped		*/
#define  F_MINMAL	0000020
#define  F_UPDATE	0000040
#define  F_SWABD	0000100
#define  F_AR16WR	0000200
#define  F_AR32WR	0000400		/* VAX byte ordering		*/
#define  F_AR32W	0001000		/* Motorola byte ordering	*/
#define  F_PATCH	0002000
#define  F_NODF		0002000


/*	Magic Numbers	*/


/*	Motorola 68000	*/
#define	MC68MAGIC	0520
#define	MC68TVMAGIC	0521
#define MC68WRMAGIC	0620			
#define MC68ROMAGIC	0625
#define MC68KWRMAGIC	0620			
#define MC68KPGMAGIC	0620			
#define MC68KROMAGIC	0625
#define	MC68020MAGIC	0630
#define	M68MAGIC	0210
#define	M68TVMAGIC	0211

/*	Motorola 88k	*/
#define MC88MAGIC	0555	/* BCS july 88 */

#define MC88MAGOLD	0540


#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)

#endif
