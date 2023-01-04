h07551
s 00000/00543/00405
d D 1.2 90/05/05 06:31:52 root 2 1
c removed CMMU defines
e
s 00948/00000/00000
d D 1.1 90/03/06 12:29:47 root 1 0
c date and time created 90/03/06 12:29:47 by root
e
u
U
t
T
I 1
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

D 2
#ifndef CMMU
#define	CMMU
#endif
E 2
/*
 * Page Descriptor Entry Definitions
 */

D 2
#ifdef CMMU
E 2
typedef union cpde {	/* CMMU pde */
/*         short page descriptor entry
 *
 *  +---------------------+---+-----+--+--+--+--+--+---+---+--+--+--+
 *  |      Page Address   |lck|ndref|wt|sp| g|ci| r|mod|ref|wp| r| v|
 *  +---------------------+---+-----+--+--+--+--+--+---+---+--+--+--+
 *             20           1   1     1  1  1  1  1  1   1   1  1  1
 */
D 2
#ifdef	AUSTINCC
E 2
	struct {
D 2
		uint	
			pg_val	:  1,	/* Valid Page desc.  (hardware) */
			pg_r1	:  1,	/* Reserved			*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_r2	:  1,	/* Reserved			*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_g	:  1,	/* Global page       (hardware)	*/
			pg_sp   :  1,	/* Supervisor Only   (hardware) */
			pg_wt   :  1,	/* Write through cac (hardware) */
			pg_ndref:  1,	/* Need a reference  (software)	*/
			pg_lock	:  1,	/* Lock in core      (software)	*/
			pg_pfn  : 20;	/* Physical page frame (hardware) */
	} pgm;
#else
	struct {
E 2
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
D 2
#endif
E 2

	struct {
		uint	pg_spde; /* CMMU page descriptor (table) entry */
	} pgi;
} cpde_t;	/* CMMU Page Descriptor */
D 2
#else
typedef union spde {
/*         short page descriptor entry
 *
 *  +---------------------+---+-----+--+--+--+---+---+--+--+
 *  |      Page Address   |lck|ndref| g|ci| l|mod|ref|wp|dt|
 *  +---------------------+---+-----+--+--+--+---+---+--+--+
 *             22           1   1     1  1  1  1   1   1  2
 */
#ifdef	AUSTINCC
	struct {
		uint	pg_dt	:  2,	/* Descriptor type   (hardware)	*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_l	:  1,	/* Cache desc. lock  (hardware)	*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_g	:  1,	/* Gate page         (hardware)	*/
			pg_ndref:  1,	/* Need a reference  (software)	*/
			pg_lock	:  1,	/* Lock in core      (software)	*/
			pg_pfn  : 22;	/* Physical page frame (hardware) */
	} pgm;
#else
	struct {
		uint	pg_pfn  : 22,	/* Physical page frame (hardware) */
			pg_lock	:  1,	/* Lock in core      (software)	*/
			pg_ndref:  1,	/* Need a reference  (software)	*/
			pg_g	:  1,	/* Gate page         (hardware)	*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_l	:  1,	/* Cache desc. lock  (hardware)	*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_dt	:  2;	/* Descriptor type   (hardware)	*/
	} pgm;
#endif
E 2

D 2
	struct {
		uint	pg_spde; /* short page descriptor (table) entry */
	} pgi;
} spde_t;

typedef union lpde {
/*                    long page descriptor entry	(* not used *)
 *
 * +---+-----+---+---+--+-+-+--+-+---+---+--+--+     +---------+-+-----+----+--+
 * |l/u|limit|ral|wal|sg|s|g|ci|l|mod|ref|wp|dt|     |Page Addr|-|ndref|lock|--|
 * +---+-----+---+---+--+-+-+--+-+---+---+--+--+     +---------+-+-----+----+--+
 *   1    15   3   3  1  1 1 1  1  1   1  1   2          24     1    1    1   5
 */
#ifdef	AUSTINCC
	struct {
		uint	pg_dt	:  2,	/* Descriptor type   (hardware)	*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_l	:  1,	/* Cache desc. lock  (hardware)	*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_g	:  1,	/* Gate page         (hardware)	*/
			pg_s	:  1,	/* Supervisor page   (hardware)	*/
			pg_sg	:  1,	/* Shared globally   (hardware)	*/
			pg_wal	:  3,	/* Write access level(hardware)	*/
			pg_ral	:  3,	/* Read access level (hardware)	*/
			pg_limit: 15,	/* Table limit       (hardware)	*/
			pg_lu	:  1;	/* Lower / upper     (hardware)	*/

		uint		:  5,	/* Unused/available  (software)	*/
			pg_lock	:  1,	/* Lock in core      (software)	*/
			pg_ndref:  1,	/* Needs reference   (software)	*/
				:  3,	/* Unused/available  (software)	*/
			pg_pfn  : 22;	/* Physical page frame (hardware) */
	} pgm;
#else
	struct {
		uint	pg_lu	:  1,	/* Lower / upper     (hardware)	*/
			pg_limit: 15,	/* Table limit       (hardware)	*/
			pg_ral	:  3,	/* Read access level (hardware)	*/
			pg_wal	:  3,	/* Write access level(hardware)	*/
			pg_sg	:  1,	/* Shared globally   (hardware)	*/
			pg_s	:  1,	/* Supervisor page   (hardware)	*/
			pg_g	:  1,	/* Gate page         (hardware)	*/
			pg_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pg_l	:  1,	/* Cache desc. lock  (hardware)	*/
			pg_mod	:  1,	/* Page modified     (hardware)	*/
			pg_ref	:  1,	/* Page referenced   (hardware)	*/
			pg_wp	:  1,	/* Write protect     (hardware) */
			pg_dt	:  2;	/* Descriptor type   (hardware)	*/

		uint	pg_pfn  : 22,	/* Physical page frame (hardware) */
				:  3,	/* Unused/available  (software)	*/
			pg_ndref:  1,	/* Needs reference   (software)	*/
			pg_lock	:  1,	/* Lock in core      (software)	*/
				:  5;	/* Unused/available  (software)	*/
	} pgm;
#endif

	struct {
		uint	pg_hlpde; /* hardware long page descriptor entry */
		uint	pg_slpde; /* software long page descriptor entry */
	} pgi;
} lpde_t;
#endif	/* def CMMU */

#ifdef CMMU
E 2
#define pde_t	cpde_t			/* all pde's are CMMU type */
#define	NPDEPP	1			/* number of PDE's per page */
#define NPGPT		1024		/* Nbr of pages per page table (seg). */
D 2
#else
#define pde_t	spde_t			/* all pde's are short for MMB */
#define	NPDEPP	4			/* number of PDE's per page */
#define NPGPT		64		/* Nbr of pages per page table (seg). */
#endif
E 2

/*
 *	Page Table
 */

typedef union ptbl {
	int page[NPGPT*NPDEPP];
} ptbl_t;

/* Page descriptor (table) entry dependent constants */

#define	NBPP		4096		/* Number of bytes per page */
D 2
#ifdef CMMU
E 2
#define	NBPPT		4096		/* Number of bytes per page table */
#define	BPTSHFT		12 		/* LOG2(NBPPT) if exact */
D 2
#else
#define	NBPPT		1024		/* Number of bytes per page table */
#define	BPTSHFT		10 		/* LOG2(NBPPT) if exact */
#endif
E 2
#define NDPP		8		/* Number of disk blocks per page */
#define DPPSHFT		3		/* Shift for disk blocks per page. */

#define	FILESHFT	0		/* Shift for file disk blocks per page*/
					/* 1KFS specific - Not true for 512 FS*/
					/* Dependent on page and file size    */

#define PNUMSHFT	12		/* Shift for page number from addr. */
#define POFFMASK        0xFFF		/* Mask for offset into page. */
D 2
#ifdef CMMU
E 2
#define PNUMMASK	0x3FF		/* Mask for page number in segment. */
#define	NPDEPPT		1024		/* Nbr of descriptors per page table */
D 2
#else
#define PNUMMASK	0x3F		/* Mask for page number in segment. */
#define	NPDEPPT		256		/* Nbr of descriptors per page table */
#endif
E 2

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

D 2
#ifdef CMMU
E 2
/*	The CMMU page and segment tables are 4K bytes == 1K entries.
 *	r_list size is still 1K ( provides 256 page table == 4Meg * 256 = 1Gig )
 *	so only r_lists use small page alloc for the CMMU.
 */ 
#define	NPTPP		1		/* Nbr of page tables per page.	*/
#define	NPTPPSHFT	0		/* Shift for NPTPP. */
D 2
#else
#define	NPTPP		4		/* Nbr of page tables per page.	*/
#define	NPTPPSHFT	2		/* Shift for NPTPP. */
#endif
E 2

#define	PTBLSZ		1		/* Nbr of std tbl units per page tbl */
#define	PT_PAIR		2		/* Std tbl units per pde/dbd tbl pair */

#define	pgtopt(X)	((uint)(X) << NPTPPSHFT)
#define	pttopgs(X)	(((uint)(X) + NPTPP - 1) >> NPTPPSHFT)
#define	pttob(X)	((uint)(X) << BPTSHFT)
#define	btopt(X)	(((uint)(X) + NBPPT - 1) >> BPTSHFT)

D 2
#ifdef CMMU	/* rlists are smaller than page tables */
E 2
#define BRLSHFT	10	/* rlists are 1K */
#define NBPRL	1024
#define	rltob(X)	((uint)(X) << BRLSHFT)
#define	btorl(X)	(((uint)(X) + NBPRL - 1) >> BRLSHFT)
D 2
#else		/* rlists are the same size as page tables */
#define btorl(X)	btopt(X)
#define rltob(X)	pttob(X)
#endif
E 2

/*	Tables to small-subpage-chunks. (see smallpage.c)
*/

#define NSMPGCPT	1	/* Nbr small-subpage-chunks per table */ 
#define pttosmpg(X)	((uint)(X)*NSMPGCPT)

/*	NOTE: Page, segment and r_list tables are all the same size at present
 *	so pttosmpg, pttob and btopt can be used when managing all of
 *	these tables.  -psr
 */

D 2

/* Minimum address to be mapped for ROM and IO */
#define IO_32_MIN 0xE0000000

E 2
#define NULLADDR	0

/*
 * The following macros help access the disk block descriptors.
 * They are always located immediately following the page table.
 */

D 2
#ifdef CMMU
E 2
#define	DBDOFF	1				/* offset between dbd's */
D 2
#else
#define	DBDOFF	4				/* offset between dbd's */
#endif
E 2
#define	db_getnxt(dbd)	((dbd) + DBDOFF)	/* assumes dbd_t *dbd */

D 2
/*
 * A collection of 4 (1K) page descriptors that define a 4K page
 */

#ifndef CMMU /* dont use or need this for CMMU */
typedef	struct	Pde {
			pde_t a;
			pde_t b;
			pde_t c;
			pde_t d;
} Pde_t;
#else
E 2
#define	Pde_t	pde_t
D 2
#endif
E 2

#define BITSET		1
#define BITCLR		0
#define	PG_REF		0x00000008
D 2
#ifdef CMMU
E 2
#define PG_WP		0x00000004
#define PG_MOD		0x00000010
#define PG_CI		0x00000040
#define PG_G		0x00000080
#define PG_SP		0x00000100
#define	PG_WT		0x00000200
#define	PG_NDREF	0x00000400
#define PG_LOCK		0x00000800
D 2
#else
#define	PG_NDREF	0x00000100
#endif
E 2

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

D 2
#ifdef CMMU
E 2
typedef union csde {	/* CMMU sde */
/*         CMMU segment descriptor entry
 *
 *  +---------------------+----+--+--+--+--+-----+--+--+--+
 *  |   Table   Address   | res|wt|sp| g|ci| res |wp| r| v|
 *  +---------------------+----+--+--+--+--+-----+--+--+--+
 *             20            2   1  1  1  1   3    1  1  1
 */
D 2
#ifdef	AUSTINCC
E 2
	struct {
D 2
		uint	
			pt_val	:  1,	/* Valid Table desc.  (hardware) */
			pt_r1	:  1,	/* Reserved			*/
/* temp for debugging on PMMU */
/*			pt_r1	:  1,	/* Reserved			*/
/*			pt_val	:  1,	/* Valid Table desc.  (hardware) */
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_r2	:  3,	/* Reserved			*/
			pt_ci	:  1,	/* Cache inhibit     (hardware)	*/
			pt_g	:  1,	/* Global page       (hardware)	*/
			pt_sp   :  1,	/* Supervisor Only   (hardware) */
			pt_wt   :  1,	/* Write through cac (hardware) */
			pt_r3	:  2,	/* Reserved			*/
			pt_tpfn  : 20;	/* Physical Table pg (hardware) */
	} ptm;
#else
	struct {
E 2
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
D 2
#endif
E 2

	struct {
		uint	pt_ssde; /* CMMU segment descrpt. (table) entry */
	} pti;
} csde_t;	/* CMMU Segment Descriptor */

#define sde_t	csde_t			/* all sde's are CMMU type */

D 2
#else /* ndef CMMU */
typedef union ssde {
/*         short segment descriptor entry	(* not used *)
 *
 *      +---------------------+---+--+--+
 *      |    Table  Address   |ref|wp|dt|
 *      +---------------------+---+--+--+
 *                 22+6           1   1  2
 */

#ifdef	AUSTINCC
	struct {
		uint	pt_dt	:  2,	/* Descriptor type   (hardware)	*/
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_ref	:  1,	/* Page referenced   (hardware)	*/
				:  6,	/* Zeros for table address (hardware) */
			pt_tpfn : 22;	/* Physical table pfn (hardware) */
	} ptm;
#else
	struct {
		uint	pt_tpfn : 22,	/* Physical table pfn (hardware) */
				:  6,	/* Zeros for table address (hardware) */
			pt_ref	:  1,	/* Page referenced   (hardware)	*/
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_dt	:  2;	/* Descriptor type   (hardware)	*/
	} ptm;
#endif

	struct {
		uint	pt_ssde; /* short page descriptor (table) entry */
	} pti;
} ssde_t;


typedef union lsde {
/*                    long segment descriptor entry
 *
 * +---+-----+---+---+--+-+-+-+-+-+---+--+--+ +-------------+-+-+-+-+
 * |l/u|limit|ral|wal|sg|s|-|-|-|-|ref|wp|dt| |Table Address|-|-|-|-|
 * +---+-----+---+---+--+-+-+-+-+-+---+--+--+ +-------------+-+-+-+-+
 *   1    15   3   3  1  1 1 1 1 1  1   1  2         22+6      1 1 1 1
 */

#ifdef	AUSTINCC
	struct {
		uint	pt_dt	:  2,	/* Descriptor type   (hardware)	*/
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_ref	:  1,	/* Page referenced   (hardware)	*/
			pt_zero	:  4,	/* Unused zero bits  (hardware) */
			pt_s	:  1,	/* Supervisor page   (hardware)	*/
			pt_sg	:  1,	/* Shared globally   (hardware)	*/
			pt_wal	:  3,	/* Write access level(hardware)	*/
			pt_ral	:  3,	/* Read access level (hardware)	*/
			pt_limit: 15,	/* Table limit       (hardware)	*/
			pt_lu	:  1;	/* Lower / upper     (hardware)	*/

			uint	:  4,	/* Unused/available  (software) */
				:  6,	/* Zeros for table address (hardware) */
			pt_tpfn : 22;	/* Physical table pfn (hardware) */
	} ptm;
#else
	struct {
		uint	pt_lu	:  1,	/* Lower / upper     (hardware)	*/
			pt_limit: 15,	/* Table limit       (hardware)	*/
			pt_ral	:  3,	/* Read access level (hardware)	*/
			pt_wal	:  3,	/* Write access level(hardware)	*/
			pt_sg	:  1,	/* Shared globally   (hardware)	*/
			pt_s	:  1,	/* Supervisor page   (hardware)	*/
			pt_zero	:  4,	/* Unused zero bits  (hardware) */
			pt_ref	:  1,	/* Page referenced   (hardware)	*/
			pt_wp	:  1,	/* Write protect     (hardware) */
			pt_dt	:  2;	/* Descriptor type   (hardware)	*/

		uint	pt_tpfn : 22,	/* Physical table pfn (hardware) */
				:  6,	/* Zeros for table address (hardware) */
			        :  4;	/* Unused/available  (software) */
	} ptm;
#endif

	struct {
		uint	pt_hlsde; /* hardware long segment descriptor entry */
		uint	pt_slsde; /* software long segment descriptor entry */
	} pti;
} lsde_t;

#define sde_t	lsde_t			/* all sde's are long for MMB */

#define PT_VSD		2		/* Valid pointer to short descr. */
#define PT_VLD		3		/* Valid pointer to long descr. */

#endif /* CMMU */

E 2
#define PT_INV		0		/* Invalid pointer */

D 2
#ifdef CMMU
E 2
#define PT_VLD		1		/* Valid pointer to CMMU descr. */
D 2
#endif
E 2

/*	The following macros are used to set, clear, and test
 *	the value of the bits in a segment descriptor (table) entry 
 */

D 2
#ifdef CMMU
E 2
#define pt_setvalid(P,V) ((P)->ptm.pt_val = (V))	/* Set valid bit. */
#define pt_clrvalid(P)	 ((P)->ptm.pt_val = PT_INV)	/* Clear valid bit. */
#define pt_tstvalid(P,V) ((P)->ptm.pt_val == (V))	/* Test valid bit. */
D 2
#else
#define pt_setvalid(P,V) ((P)->ptm.pt_dt = (V))		/* Set valid bit. */
#define pt_clrvalid(P)	 ((P)->ptm.pt_dt = PT_INV)	/* Clear valid bit. */
#define pt_tstvalid(P,V) ((P)->ptm.pt_dt == (V))	/* Test valid bit. */
E 2

D 2
#define pt_setlim(P,L)	((P)->ptm.pt_limit = L)		/* Set index limit. */
#endif

E 2
#define pt_setref(P)	((P)->ptm.pt_ref = BITSET) 	/* Set ref bit.	*/
#define pt_clrref(P)	((P)->ptm.pt_ref = BITCLR)	/* Clear ref bit. */
#define pt_tstref(P)	((P)->ptm.pt_ref)		/* Test ref bit. */

#define pt_setprot(P)	((P)->ptm.pt_wp = BITSET)	/* Set wrt prot bit. */
#define pt_clrprot(P)	((P)->ptm.pt_wp = BITCLR)	/* Clear wrt prot bit */
#define pt_tstprot(P)	((P)->ptm.pt_wp)		/* Test wrt prot bit. */

#define pt_settfn(P,TFN) ((P)->ptm.pt_tpfn = TFN) /* Set Table frame no. */
#define	pt_gettfn(P)	((P)->ptm.pt_tpfn)	/* Get Table frame no. */
#define	pt_getaddr(P)	((caddr_t)pttob(pt_gettfn(P)))	/* Get table address. */

D 2
#ifndef CMMU
/*	The following macro sets up several pointer descriptor 
 *	sets up fields with default values.  Individual fields 
 *	may be changed after setup but most are fixed for the port.
 *
 *	Setup modifies only the upper 28 bits of the long descriptor.
 *	Software bits, valid and write protect and address fields are
 *	not touched.
 *
 *	Default setups for the MMB:
 *		L/U = L = 0		(* always *)
 *		seven bits = 0		(* always *)
 *		limit = 255		(*  255 is for page table *)
 *		ral = 7			(* always *)
 *		wal = 7			(* always *)
 *		next six bits = 0	(* always *)
 */
#define SDEFGP0  0x00fffc00	/* setup value for MMB pointer table entries */
#define pt_setup(P) ((P)->pti.pt_hlsde = SDEFGP0)


E 2
/*	Segment descriptor (table) dependent constants.	*/

D 2
#define NBPS		0x40000 /* Number of bytes per segment */
#define SNUMSHFT	18	/* Shift for segment number from address. */
#define SNUMMASK	0x7F	/* Mask for segment number after shift. */
#define SOFFMASK	0x3FFFF	/* Mask for offset into segment. */

#define	SEG_RW		1		/* user permissions read/write */
#define	SEG_RO		2		/* user permissions read-only */

#else /* def CMMU */

/*	Segment descriptor (table) dependent constants.	*/

#ifndef NBPS
E 2
#define NBPS		0x400000	/* Number of bytes per segment */
D 2
#endif
E 2
#define SNUMSHFT	22	/* Shift for segment number from address. */
#define SNUMMASK	0x3FF	/* Mask for segment number after shift. */
#define SOFFMASK	0x3FFFFF	/* Mask for offset into segment. */

#define	SEG_RW		0		/* user permissions read/write */
#define	SEG_RO		1		/* user permissions read-only */

D 2
#endif /* CMMU */

E 2
#define snum(X)   (((uint)(X) >> SNUMSHFT) & SNUMMASK)
#define soff(X)   ((uint)(X) & SOFFMASK)

#define	SEGSIZE		btoc(NBPPT)	/* seg tbl size in clicks (rounded up)*/
#define	STBLSZ		1		/* Nbr of std tbl units per seg tbl */

D 2
#ifndef CMMU
/*
 * Domain Descriptor Entry Definitions
 *
 * The domain descriptors are the same format as the segment descriptors
 * in the current implementation.
 */
#define dde_t sde_t

/*	Domain descriptor (table) dependent constants.	*/

#define DNUMSHFT	25		/* Shift for domain number from addr. */
#define DNUMMASK	0x7F		/* Mask for domain number after shft. */
#define DOFFMASK	0x1FFFFFF	/* Mask for offset into domain. */

#define dnum(X)   (((uint)(X) >> DNUMSHFT) & DNUMMASK)
#define doff(X)   ((uint)(X) & DOFFMASK)

/*
 * Memory Management Unit Definitions
 */

typedef struct _VAR {	/*  virtual address     */
	uint v_ssl	: 14,	/*  segment number      */
	     v_psl	:  6,	/*  page number         */
	     v_byte	: 12;	/*  offset within page  */
} VAR;

/*  masks to extract portions of a VAR  */

#define MSK_IDXSEG  0x3ffff  /*  lower 18 bits == PSL || BYTE  */


/* 
 * MMB control interface definitions
 *
 * Two 32 bit registers provide the MMB programmer interface.
 * They appear in the CPU address space at address 0x10000.
 *
 * The root pointer is set to the base of the first translation table
 * used in a table search.
 * The translation control register is defined below.
 *
 *		+---------------------+
 *	0x10000	|     root pointer    |	 R/W
 *		+---------------------+
 *	0x10004	| translation control |  R/W
 *		+---------------------+
 */

typedef union tcr {

/*         translation control register
 *
 *  +-+-----+-----+-----+----+----+-----+-----+-----+-----+
 *  |e| zero| sre | fcl | ps | is | tia | tib | tic | tid |
 *  +-+-----+-----+-----+----+----+-----+-----+-----+-----+
 *   1   5     1     1    4     4    4     4     4     4
 */
#ifdef	AUSTINCC
	struct {
		uint	tc_tid	:  4,	/* Table index D */
			tc_tic	:  4,	/* Table index C */
			tc_tib	:  4,	/* Table index B */
			tc_tia	:  4,	/* Table index A */
			tc_is	:  4,	/* Initial shift */
			tc_ps	:  4,	/* Page size = (2**tc_ps) */
			tc_fcl	:  1,	/* Function code lookup enable */
			tc_sre	:  1,	/* Supervisor root pointer enable */
				:  5,	/* Unused, taken to be zero */
			tc_e	:  1;	/* Enable address translation */
	} tcm;
#else
	struct {
		uint	tc_e	:  1,	/* Enable address translation */
				:  5,	/* Unused, taken to be zero */
			tc_sre	:  1,	/* Supervisor root pointer enable */
			tc_fcl	:  1,	/* Function code lookup enable */
			tc_ps	:  4,	/* Page size = (2**tc_ps) */
			tc_is	:  4,	/* Initial shift */
			tc_tia	:  4,	/* Table index A */
			tc_tib	:  4,	/* Table index B */
			tc_tic	:  4,	/* Table index C */
			tc_tid	:  4;	/* Table index D */
	} tcm;
#endif

	struct {
		uint	tc_tcr;	/* translation control register */
	} tci;
} tcr_t;


/*  MMB-specific routines  */

E 2
extern	mmbstore();		/* Routine to set MMB register */

D 2
/*  MMB-specific constants  */

#define MMB_RP		0	/* Root pointer */
#define MMB_TCR		4	/* Translation Control Register */

/*	The following constants are the values the MMB expects/allows
 *	for the associated tcr field assuming a 32 bit logical address.
 */

#define MMB_SRE		0	/* Supervisor root pointer not enabled */
#define MMB_FCL		1	/* Function code lookup required */
#define MMB_PS		0xa	/* Page size is 1K */
#define MMB_IS32	0	/* Logical address must be 32 bits */
#define MMB_TIA		7	/* Table index A */
#define MMB_TIB		7	/* Table index B */
#define MMB_TIC		8	/* Table index C */
#define MMB_TID		0	/* Table index D */

#define MMB_ENBL	0x80000000	/* enable MMB */

/* The fixed values for 32 bit operation are summed up in the macro below. */
#define MMB_FIXED	0x1a07780	/* Fixed values for 32 bit addresses */

#define mmb_enable()	mmbstore(MMB_TCR,(MMB_ENBL|MMB_FIXED))
#define mmb_settcr(T)	mmbstore(MMB_TCR,T)
#define mmb_setrp(RP)	mmbstore(MMB_RP,RP)

#define mmu_enable()	mmb_enable()
#define mmu_setup()	mmb_setrp(fcltbl)

#else /* CMMU */

/* #define P_CMMU_FIXED	0x82C0AA00	/* ena, no funcd, sre, 4K, is = 0 */

E 2
extern cmmu_store();	/* code in ML to store CMMU regs	*/
extern cmmuflush();	/* code in ML to flush ATCI		*/

#define CMMU_TCR 0	/* used for pmmu simulation of CMMU	*/
#define CMMU_URP 4	/* store CPU root pointer (USER)	*/
#define CMMU_SRP 8	/* store SUPV root pointer		*/

D 2
#endif

E 2
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

D 2
#ifndef CMMU
#define FCLTSZ	8		/* size of function code lookup table */
#define UDTLIM	0		/* User domain table index limit */
#define SDTLIM	127		/* System domain table index limit */
#define USTLIM	127		/* User segment table index limit */
#define SSTLIM	127		/* System segment table index limit */
#define PGTLIM	255		/* Page table limit (default in pt_setup(P) ) */

/*	The following variables are used in tree walks of the
**	MMU translation tree.
*/

extern	sde_t *udtbl;		/* user domain table 		*/
extern	sde_t *sdtbl;		/* system domain table 		*/

#else /* CMMU */

E 2
extern	sde_t *ustbl;		/* user segment table 		*/
extern	sde_t *sstbl;		/* system segment table 	*/

D 2
#endif

E 2
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

D 2
#ifndef CMMU
E 2

/*	Between kernel virtual addresses and the kernel
D 2
**	domain table entry.
*/

#define	kvtodde(kva)  ((sde_t *)&(sdtbl[dnum(kva)]))


/*	Between kernel virtual addresses and the kernel
E 2
**	segment table entry.
*/

D 2
#define	kvtosde(kva) ((sde_t *)&((sde_t *)pt_getaddr(kvtodde(kva)))[snum(kva)])

#else

/*	Between kernel virtual addresses and the kernel
**	segment table entry.
*/

E 2
#define	kvtosde(kva) ((sde_t *)&(sstbl[snum(kva)]))

D 2
#endif
E 2

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


D 2
#ifndef CMMU
E 2
/*	Between current user virtual addresses and the current user
D 2
**	domain table entry.
*/

#define	uvtodde(uva)  (udtbl)	/* There is only one */


/*	Between current user virtual addresses and the current user
E 2
**	segment table entry.
*/

D 2
#define	uvtosde(uva) ((sde_t *)&((sde_t *)pt_getaddr(uvtodde(uva)))[snum(uva)])

#else

/*	Between current user virtual addresses and the current user
**	segment table entry.
*/

E 2
#define	uvtosde(uva) ((sde_t *)&(ustbl[snum(uva)]))

D 2
#endif

E 2
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
D 2

/* CPU function codes
*/
#define USERDATA	1
#define USERPROG	2
#define SUPVDATA	5
#define SUPVPROG	6
#define CPUSPACE	7

/*
 *	PMMU Status Register (PSR)
 */

#define	P_LVL	0x0007		/* number of levels */
#define	P_GSH	0x0080		/* globally sharable */
#define	P_GATE	0x0100		/* gate */
#define	P_MOD	0x0200		/* modified */
#define	P_INV	0x0400		/* invalid */
#define	P_WP	0x0800		/* write protected */
#define	P_ALV	0x1000		/* access level violation */
#define	P_SUP	0x2000		/* supervisor only */
#define	P_LIM	0x4000		/* limit violation */
#define	P_BE	0x8000		/* bus error */
E 2

#endif	/* ! _SYS_IMMU_H_ */
E 1
