h12523
s 00003/00001/00400
d D 1.3 90/06/27 02:18:27 root 3 2
c more changes to limit dmaspace size to 16mb
e
s 00006/00003/00395
d D 1.2 90/06/14 22:22:58 root 2 1
c added logic to support a size for an address space.
e
s 00398/00000/00000
d D 1.1 90/03/06 12:28:06 root 1 0
c date and time created 90/03/06 12:28:06 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/pmmu.c	10.7"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/sysmacros.h"
#include "sys/pfdat.h"
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/map.h"
#include "sys/var.h"
#include "sys/ram.h"
#include "sys/cmn_err.h"
#include "sys/debug.h"

/*
 * File: pmmu.c
 */

/*
 * System memory management variables
 */

pde_t *mmuupde;	/* pointer to table of pde's mapping the U-Vector */

extern struct map sptmap[];  /* map for dynamically allocated virtual space */
extern etext;
extern end;
extern M88Kvec[];

sde_t *fcltbl;		/* function lookup table */
/*
 * System table structures 
 */
sde_t *ustbl;		/* user segment table */
sde_t *sstbl;		/* system segment table */
pde_t *ksptbl;		/* kernel static page tables */


/*
 * mmuinit(physclick)
 *
 * Sets up the system space for the MMU.
 * Enter this routine running in physical space and exit it running
 * mapped as follows:
 *
 *	AREA			SIZE(CLICKS)	COMMENT
 *
 * 00000000 Kernel Static       256 MEG          mapped 1:1
 *
 * 10000000 U-Vector		4k
 * 10001000 Kernel Dynamic      256 MEG + 4k
 *
 * d0000000 32 Bit Device I/O   Configurable    mapped 1:1
 *
 * physclick is the first physical click available for tables.
 */
#ifdef CMMU	/* this should be in mmu.h file */
typedef union batc {
	struct {
	unsigned long	lba	:	13,	/* logical block addr	*/
			pba	:	13,	/* physical block addr	*/
			supv	:	1,	/* supervisor LA 	*/
			wt	:	1,	/* write through 	*/
			global	:	1,	/* global block 	*/
			ci	:	1,	/* cache inhibit 	*/
			wp	:	1,	/* write protect 	*/
			val	:	1;	/* valid block discrip 	*/
	} batc_b;
	struct {
	unsigned long	word;
	} batc_w;
} batc_t;
#define	BWP0		0x400	/* base plus this = batc write port */
#define BLKSIZE		0x80000	/* each batc maps 512K block */
#define BLKSHFT		19	/* bits to shift to divide by 512K */
#define btoblk(x)	((x) >> BLKSHFT)
#define pg_setwt(P)	((P)->pgm.pg_wt = 1)
#define pg_clrwt(P)	((P)->pgm.pg_wt = 0)
#define pg_setg(P)	((P)->pgm.pg_g = 1)
#define pg_clrg(P)	((P)->pgm.pg_g = 0)
#endif

extern ccacheinh, dcacheinh, ucacheinh;
int usebatc;

int mmuinit(physclick)
int physclick;
{
	extern struct ram ram_tbl[];
	extern unsigned ram_nbr;
	register sde_t *sde;
	register sde_t *stbl;
	register pde_t *pde;
	register unsigned int addr, endaddr;
	register struct ram *ram = ram_tbl;
	register unsigned int nbr = ram_nbr;
	register int vindex;
	unsigned int firstpt = ctob(physclick);
	extern caddr_t vmspace[];
I 2
	extern long vmsize[];
E 2
	extern int DCMMUBASE, CCMMUBASE;

	if (usebatc)
	{
	/* load block address translation for kernel text now */
	batc_t batc;	/* working batc */
	int i = 0;
	addr = 0;	/* starting at text at zero */
	/* there are only 8 BATC entries in each CMMU */
	while( addr < (unsigned int)&etext && i < (sizeof(long)*8) ) {	
		batc.batc_w.word = 0;
		batc.batc_b.lba = batc.batc_b.pba = btoblk(addr);
		batc.batc_b.supv = 1;
		batc.batc_b.val = 1;
		if (ccacheinh & PG_CI)
			batc.batc_b.ci = 1;

		cmmu_store(CCMMUBASE + BWP0 + i, batc.batc_w.word);
		i += sizeof(long);
		addr += BLKSIZE;
	}
	i = 0;
	/* this will only load data BATC for the 512K blocks before end of bss
	the last partial block from end of bss to end of last block is 
	not in BATC because the page tables start on a page boundry
	If the start of free mem was rounded up to the next block then
	all the data and bss could be in BATC */
	while(addr < (unsigned int)&end && i < (sizeof(long)*8)) {	
		/* set the previous 512K block in data BATC */
		batc.batc_w.word = 0;
		batc.batc_b.lba = batc.batc_b.pba = btoblk(addr - BLKSIZE);
		batc.batc_b.supv = 1;
		batc.batc_b.val = 1;
		if (dcacheinh & PG_WT)
			batc.batc_b.wt = 1;
		if (dcacheinh & PG_CI)
			batc.batc_b.ci = 1;

		cmmu_store(DCMMUBASE + BWP0 + i, batc.batc_w.word);
		i += sizeof(long);
		addr += BLKSIZE;
	}
	}	/* usebatc */

	/*
	 * Set up MMU translation tree for all kernel virtual space.
	 */

	sstbl = (sde_t *)ctob(physclick);
	physclick++;
	ksptbl = (pde_t *) ctob(physclick);
	ustbl = (sde_t *)0;	/* user segment table is set during switch */

	/* 
	 * Build domain for static, dynamic and opus dma kernel virtual space
	 */
	for ( vindex = 0 ; vmspace[vindex] != (caddr_t) 1 ; vindex++ ) {
		register unsigned int size = 0;

D 2
		for ( addr = (unsigned) vmspace[vindex], endaddr = addr + ctob( physmem );
		    addr < endaddr ;
		    addr += size ) {
E 2
I 2
		addr = (unsigned) vmspace[vindex];
		if ((endaddr = vmsize[vindex]) == 0)
D 3
			endaddr = addr + ctob( physmem );
E 3
I 3
			endaddr = ctob( physmem );

		endaddr += addr;
E 3

		for ( ; addr < endaddr ; addr += size ) {
E 2
			register unsigned int dindex;
	
			size = endaddr - addr;
			physclick = mkpgtbl( pnum(addr), btoc(size),
			    sstbl, physclick );
		}
	}


	for ( ; nbr ; nbr--, ram++ ) {
		register unsigned int size, flg, ci;

		flg = ram->ram_flg;
		if ( !( flg & ( RAM_FOUND | RAM_PRIV ) ) )
			continue;

		size = ram->ram_sz;
		addr = ctob(ram->ram_lo);
		ci = flg & RAM_CI;

		for ( ; size ; size-- ) {
			physclick = mkpgtbl(pnum(addr), 1, sstbl,
			    physclick);
			pde = kvtopde(addr);
			pg_setpfn(pde, pnum(addr));
			/* because kernel page tables must be cache inhibited */
			if( (addr >= firstpt && addr < ctob(physclick) ) || 
			    (addr == 0) ||
			    ((kdebug & 1) && addr >= 0 && addr < 0x3000) ||
			    (ci))
				pg_setci(pde);
			pg_setvalid(pde);
			addr += ctob(1);
		}
	}

	kptbl = kvtopde(SYSSEGS);	/* set table address */
	mmuupde = kptbl;		/* set U-Vector pde table address */

	physclick = archinit1(physclick);

	/* Initialize the map used to dynamically allocate
	 * kernel virtual space. Skip room for UBLOCK.
	 */

	mapinit(sptmap, v.v_sptmap);
	mfree(sptmap, physmem-USIZE, btoc(SYSSEGS)+USIZE);

	/* 
	 * Enable mapping
	 */

	cmmu_store(CMMU_SRP, (unsigned long)sstbl);
	physclick = archinit2(physclick);
	return( physclick);
}

/*
 * loadstbl(up, prp, change)
 *
 * Change the content of a process's segment descriptor entries (pointer tbl):
 *    	change > 0 -> load sdt table using the affected entries of prp->r_list.
 *    	change = 0 -> invalidate all sdt table entries for this prp.
 *	change < 0 -> invalidate the affected sdt table entries. 
 *
 * Region data always represents the new state when loadstbl() is called. 
 *  Exception: Special case when (change == 0), then invalidate all entries.
 *   In this case, region has not change to new state or size.
 */

loadstbl(up, prp, change)
user_t		*up;
preg_t		*prp;
register int	change;
{
	register reg_t	*rp;
	register sde_t	*st;
	register proc_t *p;
	int	seg;	/* first segment to be modified (pttbl index) */
	int	rseg;	/* first segment to be modified (rlist index) */
	int	lim;	/* first segment not to be modified */
	int	newend, oldend, lowend, highend;
	int	osize;

	p = up->u_procp;
	rp = prp->p_reg;

	osize = btoct(prp->p_regva) + rp->r_pgoff;

	if (prp->p_type != PT_STACK) {
		newend = osize + rp->r_pgsz;
		if (change)
			oldend = newend - change;
		else
			oldend = osize;
	} else {
		newend = osize;
		if (change)
			oldend = newend + change;
		else
			oldend = osize + rp->r_pgsz;
	}

	lowend  = min(newend, oldend);
	highend = max(newend, oldend);

	lim = ctos(highend);			/* round up */

	if (change <= 0) {	/* shrinking region,  clear sde's */
		seg = ctos(lowend - rp->r_pgoff);	/* round up */
		st = &p->p_pttbl[seg];		/* first seg to change */
		for (; seg < lim; seg++, st++) {	
			pt_clrvalid(st);	
			pt_settfn(st, 0);
		}
	} else {			/* expanding region, set up sde's */
		seg  = ctost(lowend - rp->r_pgoff);	/* round down */
		st = &p->p_pttbl[seg];		/* first seg to change */
		rseg = ctost(lowend - rp->r_pgoff - btoc(prp->p_regva) );
		for (; seg < lim; seg++, rseg++, st++) {    
			pt_settfn(st, kvtotfn(rp->r_list[rseg]));
			pt_setvalid(st,PT_VLD);
			if (prp->p_flags & PF_RDONLY)
				pt_setprot(st);
			else
	/* this was done by default in pt_setup, not used in CMMU */
				pt_clrprot(st);
		}
	}
	mmuflush();
}


/*
 * mmucswtch(p)
 *
 * Sets up the MMU to switch to process p.
 * Requires a change of the single descriptor in the 
 * user domain table (udtbl).
 * The MMU must be properly flushed after the change.
 */
mmucswtch(p)
register proc_t	*p;
{
	register int	ipl;

	ASSERT(p->p_pttbl != NULL);
	ustbl = (sde_t *)pttob(kvtotfn(p->p_pttbl));
	cmmu_store(CMMU_URP, (long)ustbl);	/* load the USER(CPU) RP */
}

/*	The one and only MMB flush option is to flush the entire ATC
 *	by rewriting the root pointer.
 *	Note that kvtophys(fcltbl) would be appropriate but we know
 *	the kernel is mapped 1:1 so the kvtophys() is skipped
 *	...we have enough overhead already.
 */

/*
 * mmuflush() flushes entire MMU.
 * Probably wouldn't need to flush supervisor program space.
 */
mmuflush()
{
	cmmuflush();
}

/*
 * mkpgtbl( firstclick, size, stbl, nextfree )
 *
 * Makes page tables as required to map the specified 
 * virtual address range from firstclick for size clicks 
 * within a domain whose segment table is pointed to by stbl,
 * taking clicks for tables from a zeroed click heap beginning at nextfree. 
 *
 * Returns nextfree, the next free click for tables.
 * The new page table is invalidated by default since the heap is zeroed.
 *
 * Note: We make the assumption here that NPTPP page tables fit exactly
 * in one page and that clicksize is the same as pagesize.
 */
int
mkpgtbl( firstclick, size, stbl, nextfree )
int	firstclick;	/* click offset into domain */
int	size;
sde_t	*stbl;		/* segment table */
int	nextfree;
{
	sde_t *sde, *lastsde, *tempsde;

	ASSERT( size > 0 );

	/* For each segment descriptor within the range */

	lastsde = stbl + ctost(firstclick + size -1);
	for ( sde = stbl + ctost(firstclick); sde <= lastsde; sde++ ) {

		if ( !pt_tstvalid(sde,PT_VLD) ) { /* if the sde is not valid */

			/* allocate and attach page table */
			tempsde = (sde_t *) ctob(nextfree);
			++nextfree;
			setsde(sde, 0, btopt(tempsde), PT_VLD);
		}
	}

	return( nextfree );
}

/*
 * setsde(sde, lim, tfn, val )
 *
 * Sets default values, limit and tfn (table frame no) for specified sde.
 * If val is set, validates the sde using the value of val.
 */
setsde(sde, lim, tfn, val )
sde_t	*sde;
int	lim;
int     tfn;
int	val;
{
	pt_settfn(sde,tfn);
	if( val )
		pt_setvalid(sde,val);
}
E 1
