#ifndef _SYS_IMMU_H_
#define _SYS_IMMU_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Page Descriptor Entry Definitions
 */

typedef union cpde {	/* CMMU pde */
/*         short page descriptor entry
 *
 *  +---------------------+---+-----+--+--+--+--+--+---+---+--+--+--+
 *  |      Page Address   |lck|ndref|wt|sp| g|ci| r|mod|ref|wp| r| v|
 *  +---------------------+---+-----+--+--+--+--+--+---+---+--+--+--+
 *             20           1   1     1  1  1  1  1  1   1   1  1  1
 */
	struct {
		uint	pg_pfn  : 20,	/* Physical page frame (hardware) */
			pg_lock	:  1,	/* Lock in core      (software)	*/
			pg_ndref:  1,	/* Need a reference  (software)	*/
			pg_wt   :  1,	/* Write through cac (hardware) */
			pg_sp   :  1,	/* Supervisor Only   (hardware) */
			pg_g	:  1,	/* Global page       (hardware)	*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_r2	:  1,	/* Reserved			*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_r1	:  1,	/* Reserved			*/
			pg_val	:  1;	/* Valid Page desc.  (hardware) */
	} pgm;

	struct {
		uint	pg_spde; /* CMMU page descriptor (table) entry */
	} pgi;
} cpde_t;	/* CMMU Page Descriptor */

#define pde_t	cpde_t			/* all pde's are CMMU type */
#define	NPDEPP	1			/* number of PDE's per page */
#define NPGPT		1024		/* Nbr of pages per page table (seg). */

/*
 *	Page Table
 */

typedef union ptbl {
	int page[NPGPT*NPDEPP];
} ptbl_t;

/* Page descriptor (table) entry dependent constants */

#define	NBPP		4096		/* Number of bytes per page */
#define	NBPPT		4096		/* Number of bytes per page table */
#define	BPTSHFT		12 		/* LOG2(NBPPT) if exact */
#define NDPP		8		/* Number of disk blocks per page */
#define DPPSHFT		3		/* Shift for disk blocks per page. */

#define	FILESHFT	0		/* Shift for file disk blocks per page*/
					/* 1KFS specific - Not true for 512 FS*/
					/* Dependent on page and file size    */

#define PNUMSHFT	12		/* Shift for page number from addr. */
#define POFFMASK        0xFFF		/* Mask for offset into page. */
#define PNUMMASK	0x3FF		/* Mask for page number in segment. */
#define	NPDEPPT		1024		/* Nbr of descriptors per page table */

#define	PG_VALID	1		/* bit indep. valid flag */

#define PG_INV		0		/* Invalid descriptor */
#define PG_VPD		1		/* Valid page descriptor */

/* byte addr to virtual page */

#define pnum(X)   ((uint)(X) >> PNUMSHFT) 

/* page offset */

#define poff(X)   ((uint)(X) & POFFMASK)

/* byte address to page within segment */

#define psnum(X)   (((uint)(X) >> PNUMSHFT) & PNUMMASK)

/*	Disk blocks (sectors) and pages.
*/

#define	ptod(PP)	((PP) << DPPSHFT)
#define	dtop(DD)	(((DD) + NDPP - 1) >> DPPSHFT)
#define dtopt(DD)	((DD) >> DPPSHFT)

/*	Disk blocks (sectors) and bytes.
*/

#define	dtob(DD)	((DD) << SCTRSHFT)
#define	btod(BB)	(((BB) + NBPSCTR - 1) >> SCTRSHFT)
#define	btodt(BB)	((BB) >> SCTRSHFT)

/*	Page tables (256 entries == 1024 bytes) to pages.
 *	May be considered a "standard" table size since a few of 
 *	these macros are used for segment, domain and rlist table management.
 */

/*	The CMMU page and segment tables are 4K bytes == 1K entries.
 *	r_list size is still 1K ( provides 256 page table == 4Meg * 256 = 1Gig )
 *	so only r_lists use small page alloc for the CMMU.
 */ 
#define	NPTPP		1		/* Nbr of page tables per page.	*/
#define	NPTPPSHFT	0		/* Shift for NPTPP. */

#define	PTBLSZ		1		/* Nbr of std tbl units per page tbl */
#define	PT_PAIR		2		/* Std tbl units per pde/dbd tbl pair */

#define	pgtopt(X)	((uint)(X) << NPTPPSHFT)
#define	pttopgs(X)	(((uint)(X) + NPTPP - 1) >> NPTPPSHFT)
#define	pttob(X)	((uint)(X) << BPTSHFT)
#define	btopt(X)	(((uint)(X) + NBPPT - 1) >> BPTSHFT)

#define BRLSHFT	10	/* rlists are 1K */
#define NBPRL	1024
#define	rltob(X)	((uint)(X) << BRLSHFT)
#define	btorl(X)	(((uint)(X) + NBPRL - 1) >> BRLSHFT)

/*	Tables to small-subpage-chunks. (see smallpage.c)
*/

#define NSMPGCPT	1	/* Nbr small-subpage-chunks per table */ 
#define pttosmpg(X)	((uint)(X)*NSMPGCPT)

/*	NOTE: Page, segment and r_list tables are all the same size at present
 *	so pttosmpg, pttob and btopt can be used when managing all of
 *	these tables.  -psr
 */

#define NULLADDR	0

/*
 * The following macros help access the disk block descriptors.
 * They are always located immediately following the page table.
 */

#define	DBDOFF	1				/* offset between dbd's */
#define	db_getnxt(dbd)	((dbd) + DBDOFF)	/* assumes dbd_t *dbd */

#define	Pde_t	pde_t

#define BITSET		1
#define BITCLR		0
#define	PG_REF		0x00000008
#define PG_WP		0x00000004
#define PG_MOD		0x00000010
#define PG_CI		0x00000040
#define PG_G		0x00000080
#define PG_SP		0x00000100
#define	PG_WT		0x00000200
#define	PG_NDREF	0x00000400
#define PG_LOCK		0x00000800

extern	void	pg_setvalid();
extern	void	pg_clrvalid();
extern	int	pg_tstvalid();
extern	void	pg_setlock();
extern	void	pg_clrlock();
extern	int	pg_tstlock();
extern	void	pg_setmod();
extern	void	pg_clrmod();
extern	int	pg_tstmod();
extern	void	pg_setcw();
extern	void	pg_clrcw();
extern	int	pg_tstcw();
extern	void	pg_setref();
extern	void	pg_clrref();
extern	int	pg_tstref();
extern	void	pg_setndref();
extern	void	pg_clrndref();
extern	int	pg_tstndref();
extern	void	pg_setci();
extern	void	pg_clrci();
extern	int	pg_tstci();
extern	void	pg_setpfn();
extern	int	pg_getpfn();
extern	void	pg_setpde();
extern	int	pg_getpde();
extern	int	pg_getaddr();
extern	int	pg_gettfn();
extern	int	pg_chkref();

/*
 * Segment Descriptor Entry Definitions
 */

typedef union csde {	/* CMMU sde */
/*         CMMU segment descriptor entry
 *
 *  +---------------------+----+--+--+--+--+-----+--+--+--+
 *  |   Table   Address   | res|wt|sp| g|ci| res |wp| r| v|
 *  +---------------------+----+--+--+--+--+-----+--+--+--+
 *             20            2   1  1  1  1   3    1  1  1
 */
	struct {
		uint	pt_tpfn  : 20,	/* Physical Table pg (hardware) */
			pt_r3	:  2,	/* Reserved			*/
			pt_wt   :  1,	/* Write through cac (hardware) */
			pt_sp   :  1,	/* Supervisor Only   (hardware) */
			pt_g	:  1,	/* Global page       (hardware)	*/
			pt_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pt_r2	:  3,	/* Reserved			*/
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_r1	:  1,	/* Reserved			*/
			pt_val	:  1;	/* Valid Table desc. (hardware) */
/* temp for debugging on PMMU */
/*			pt_val	:  1,	/* Valid short desc. (hardware) */
/*			pt_r1	:  1;	/* Reserved			*/
	} ptm;

	struct {
		uint	pt_ssde; /* CMMU segment descrpt. (table) entry */
	} pti;
} csde_t;	/* CMMU Segment Descriptor */

#define sde_t	csde_t			/* all sde's are CMMU type */

#define PT_INV		0		/* Invalid pointer */

#define PT_VLD		1		/* Valid pointer to CMMU descr. */

/*	The following macros are used to set, clear, and test
 *	the value of the bits in a segment descriptor (table) entry 
 */

#define pt_setvalid(P,V) ((P)->ptm.pt_val = (V))	/* Set valid bit. */
#define pt_clrvalid(P)	 ((P)->ptm.pt_val = PT_INV)	/* Clear valid bit. */
#define pt_tstvalid(P,V) ((P)->ptm.pt_val == (V))	/* Test valid bit. */

#define pt_setref(P)	((P)->ptm.pt_ref = BITSET) 	/* Set ref bit.	*/
#define pt_clrref(P)	((P)->ptm.pt_ref = BITCLR)	/* Clear ref bit. */
#define pt_tstref(P)	((P)->ptm.pt_ref)		/* Test ref bit. */

#define pt_setprot(P)	((P)->ptm.pt_wp = BITSET)	/* Set wrt prot bit. */
#define pt_clrprot(P)	((P)->ptm.pt_wp = BITCLR)	/* Clear wrt prot bit */
#define pt_tstprot(P)	((P)->ptm.pt_wp)		/* Test wrt prot bit. */

#define pt_settfn(P,TFN) ((P)->ptm.pt_tpfn = TFN) /* Set Table frame no. */
#define	pt_gettfn(P)	((P)->ptm.pt_tpfn)	/* Get Table frame no. */
#define	pt_getaddr(P)	((caddr_t)pttob(pt_gettfn(P)))	/* Get table address. */

/*	Segment descriptor (table) dependent constants.	*/

#define NBPS		0x400000	/* Number of bytes per segment */
#define SNUMSHFT	22	/* Shift for segment number from address. */
#define SNUMMASK	0x3FF	/* Mask for segment number after shift. */
#define SOFFMASK	0x3FFFFF	/* Mask for offset into segment. */

#define	SEG_RW		0		/* user permissions read/write */
#define	SEG_RO		1		/* user permissions read-only */

#define snum(X)   (((uint)(X) >> SNUMSHFT) & SNUMMASK)
#define soff(X)   ((uint)(X) & SOFFMASK)

#define	SEGSIZE		btoc(NBPPT)	/* seg tbl size in clicks (rounded up)*/
#define	STBLSZ		1		/* Nbr of std tbl units per seg tbl */

extern	mmbstore();		/* Routine to set MMB register */

extern cmmu_store();	/* code in ML to store CMMU regs	*/
extern cmmuflush();	/* code in ML to flush ATCI		*/

#define CMMU_TCR 0	/* used for pmmu simulation of CMMU	*/
#define CMMU_URP 4	/* store CPU root pointer (USER)	*/
#define CMMU_SRP 8	/* store SUPV root pointer		*/

/*	The following variables describe most memory managed by
**	the kernel.  Only certain I/O areas are not included.
*/

extern pde_t	*kptbl;		/* Kernel page table.  Used to	*/
				/* map a contiguous block of	*/
				/* kernel virtual space based	*/
				/* at SYSSEGS.			*/
extern int	kpbase;		/* The address of the start of	*/
				/* the first physical page of	*/
				/* memory above the kernel.	*/
				/* Physical memory from here to	*/
				/* the end of physical memory	*/
				/* is represented in the pfdat.	*/
extern int	maxmem;		/* Maximum available free	*/
				/* memory.			*/
extern int	freemem;	/* Current free memory.		*/
extern int	availrmem;	/* Available resident (not	*/
				/* swapable) memory in pages.	*/
extern int	availsmem;	/* Available swapable memory in	*/
				/* pages.			*/


/*
**	Implementation specific constants and variables.
*/

#define SYSSEGS 0x10000000	/* Kernel virtual address of syssegs area */
#define ALLFFFS 0xffffffff	/* Last 32 bit address */

extern	sde_t *ustbl;		/* user segment table 		*/
extern	sde_t *sstbl;		/* system segment table 	*/

/*	Conversion macros
*/

/*	Get page number from system virtual address.  */

#define	svtop(X)	((uint)(X) >> PNUMSHFT) 

/*	Get system virtual address from page number.  */

#define	ptosv(X)	((uint)(X) << PNUMSHFT)


/*	These macros are used to map between kernel virtual
**	and physical address.  Note that all physical memory
**	within the same contiguous block as the kernel
**	is mapped into kernel virtual address space
**	at the actual physical address.
*/

#define kvtophys(kva) (((caddr_t)pg_getaddr(kvtopde(kva)))+poff(kva))
#define phystokv(paddr) (paddr)


/*	Between kernel virtual address and physical page frame number.
*/

#define pfntokv(pfn)   ((pfn) << PNUMSHFT)


/*	Between kernel virtual addresses and the kernel
**	segment table entry.
*/

#define	kvtosde(kva) ((sde_t *)&(sstbl[snum(kva)]))


/*	Between kernel virtual addresses and the kernel
**	page table entry.
*/

#define	kvtopde(kva) ((pde_t *)&((pde_t *)pt_getaddr(kvtosde(kva)))[psnum(kva)*NPDEPP])


/*	Between kernel virtual addresses and the kernel syssegs page table. 
**	Only valid for addresses mapped by kptbl.
*/

#define	kvtokptbl(kva)	(&kptbl[btopt(((uint)(kva) - SYSSEGS))])


/*	Between kernel virtual address and its physical page frame.
**	Only valid for addresses mapped by kptbl.
*/

#define	kvtotfn(kva)	(pg_gettfn((pde_t *)kvtokptbl(kva)))


/*	Between current user virtual addresses and the current user
**	segment table entry.
*/

#define	uvtosde(uva) ((sde_t *)&(ustbl[snum(uva)]))

/*	Between current user virtual addresses and the current user page
**	table.
*/

#define	uvtopde(uva) ((pde_t *)&((pde_t *)pt_getaddr(uvtosde(uva)))[psnum(uva)*NPDEPP])


/*	The following routines are involved with the pfdat
**	table described in pfdat.h
*/

#define	pdetopfdat(pde)	(&pfdat[pg_getpfn(pde)])
#define	pfntopfdat(pfn)	(&pfdat[pfn])
#define	pfdattopfn(pfd)	(pfd - pfdat)
#define	pdetpdbd(pde)	(dpe + (NPGPT * NPDEPP))


/* flags used in ptmemall() call
*/

#define PHYSCONTIG 02
#define NOSLEEP    01

#endif	/* ! _SYS_IMMU_H_ */
