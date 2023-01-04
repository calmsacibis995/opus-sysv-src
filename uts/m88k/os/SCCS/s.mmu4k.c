h29567
s 00386/00000/00000
d D 1.1 90/03/06 12:28:03 root 1 0
c date and time created 90/03/06 12:28:03 by root
e
u
U
t
T
I 1
/*	%W%	*/

/*
 *	This code is specific for processors that
 *	have the page size equal to 4K.
 */

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"

/*
 * "valid bit" manipulating routines
 */

void
pg_setvalid(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_dt = PG_VPD; p++;
	p->pgm.pg_dt = PG_VPD; p++;
	p->pgm.pg_dt = PG_VPD; p++;
	p->pgm.pg_dt = PG_VPD;
#else /* CMMU */
	/*p->pgm.pg_val = PG_VPD;*/
	p->pgi.pg_spde |= PG_VPD;
#endif
}

void
pg_clrvalid(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_dt = PG_INV; p++;
	p->pgm.pg_dt = PG_INV; p++;
	p->pgm.pg_dt = PG_INV; p++;
	p->pgm.pg_dt = PG_INV;
#else /* CMMU */
	/* p->pgm.pg_val = PG_INV; */
	p->pgi.pg_spde &= ~PG_VPD;
#endif
}

pg_tstvalid(p)
pde_t *p;
{
#ifndef CMMU
	return(p->pgm.pg_dt == PG_VPD);
#else /* CMMU */
	/*return(p->pgm.pg_val == PG_VPD);*/
	return(p->pgi.pg_spde & PG_VPD);
#endif
}

/*
 * "lock bit" manipulating routines
 */

void
pg_setlock(p)
pde_t *p;
{
	/*p->pgm.pg_lock = BITSET;*/
	p->pgi.pg_spde |= PG_LOCK;
}

void
pg_clrlock(p)
pde_t *p;
{
	/*p->pgm.pg_lock = BITCLR;*/
	p->pgi.pg_spde &= ~PG_LOCK;
}

pg_tstlock(p)
pde_t *p;
{
	/*return(p->pgm.pg_lock);*/
	return(p->pgi.pg_spde&PG_LOCK);
}

/*
 * "modify bit" manipulating routines
 */

void
pg_setmod(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_mod = BITSET; p++;
	p->pgm.pg_mod = BITSET; p++;
	p->pgm.pg_mod = BITSET; p++;
#endif
	/*p->pgm.pg_mod = BITSET;*/
	p->pgi.pg_spde |= PG_MOD;
}

void
pg_clrmod(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_mod = BITCLR; p++;
	p->pgm.pg_mod = BITCLR; p++;
	p->pgm.pg_mod = BITCLR; p++;
#endif
	/*p->pgm.pg_mod = BITCLR;*/
	p->pgi.pg_spde &= ~PG_MOD;
}

pg_tstmod(p)
pde_t *p;
{
#ifndef CMMU
	return ((p++)->pgm.pg_mod ||
		(p++)->pgm.pg_mod ||
		(p++)->pgm.pg_mod ||
		(p)->pgm.pg_mod);
#else /* CMMU */
	/*return((p)->pgm.pg_mod);*/
	return(p->pgi.pg_spde&PG_MOD);
#endif
}

/*
 * "write protect bit" manipulating routines
 */

void
pg_setcw(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_wp = BITSET; p++;
	p->pgm.pg_wp = BITSET; p++;
	p->pgm.pg_wp = BITSET; p++;
#endif
	/*p->pgm.pg_wp = BITSET;*/
	p->pgi.pg_spde |= PG_WP;
}

void
pg_clrcw(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_wp = BITCLR; p++;
	p->pgm.pg_wp = BITCLR; p++;
	p->pgm.pg_wp = BITCLR; p++;
#endif
	/*p->pgm.pg_wp = BITCLR;*/
	p->pgi.pg_spde &= ~PG_WP;
}

pg_tstcw(p)
pde_t *p;
{
	return(p->pgi.pg_spde&PG_WP);
	/*return(p->pgm.pg_wp);*/
}

/*
 * "reference bit" manipulating routines
 */

void
pg_setref(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_ref = BITSET; p++;
	p->pgm.pg_ref = BITSET; p++;
	p->pgm.pg_ref = BITSET; p++;
#endif
	/*p->pgm.pg_ref = BITSET;*/
	p->pgi.pg_spde |= PG_REF;
}

void
pg_clrref(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_ref = BITCLR; p++;
	p->pgm.pg_ref = BITCLR; p++;
	p->pgm.pg_ref = BITCLR; p++;
#endif
	/*p->pgm.pg_ref = BITCLR;*/
	p->pgi.pg_spde &= ~PG_REF;
}

pg_tstref(p)
pde_t *p;
{
#ifndef CMMU
	return ((p++)->pgm.pg_ref ||
		(p++)->pgm.pg_ref ||
		(p++)->pgm.pg_ref ||
		(p)->pgm.pg_ref);
#else /* CMMU */
	return(p->pgi.pg_spde&PG_REF);
	/*return ((p)->pgm.pg_ref);*/
#endif
}

/*
 * "need reference bit" manipulating routines
 */

void
pg_setndref(p)
pde_t *p;
{
	/*p->pgm.pg_ndref = BITSET;*/
	p->pgi.pg_spde |= PG_NDREF;
}

void
pg_clrndref(p)
pde_t *p;
{
	/*p->pgm.pg_ndref = BITCLR;*/
	p->pgi.pg_spde &= ~PG_NDREF;
}

pg_tstndref(p)
pde_t *p;
{
	return(p->pgi.pg_spde&PG_NDREF);
	/*return(p->pgm.pg_ndref);*/
}

/*
 * "cache inhibit bit" manipulating routines
 */

void
pg_setci(p)
register pde_t *p;
{
#ifdef opus
	dcacheflush(p->pgi.pg_spde);
	ccacheflush(p->pgi.pg_spde);
#endif
#ifndef CMMU
	p->pgm.pg_ci = BITSET; p++;
	p->pgm.pg_ci = BITSET; p++;
	p->pgm.pg_ci = BITSET; p++;
#endif
	/*p->pgm.pg_ci = BITSET;*/
	p->pgi.pg_spde |= PG_CI;
#if opus
	/* clear any mmu atc entries for this page */
	cmmuflush();
#endif
}

void
pg_clrci(p)
pde_t *p;
{
#ifndef CMMU
	p->pgm.pg_ci = BITCLR; p++;
	p->pgm.pg_ci = BITCLR; p++;
	p->pgm.pg_ci = BITCLR; p++;
#endif
	/*p->pgm.pg_ci = BITCLR;*/
	p->pgi.pg_spde &= ~PG_CI;
}

pg_tstci(p)
pde_t *p;
{
	return(p->pgi.pg_spde&PG_CI);
	/*return(p->pgm.pg_ci);*/
}

/*
 * set & get page frame number
 */

void
pg_setpfn(p,pfn)
pde_t *p;
int pfn;
{
#ifndef CMMU
	p->pgm.pg_pfn = (pfn * 4);     p++;
	p->pgm.pg_pfn = (pfn * 4) + 1; p++;
	p->pgm.pg_pfn = (pfn * 4) + 2; p++;
	p->pgm.pg_pfn = (pfn * 4) + 3;
#else /* CMMU */
	p->pgm.pg_pfn = (pfn);
#endif
}

pg_getpfn(p)
pde_t *p;
{
#ifndef CMMU
	return((p->pgm.pg_pfn) / 4);
#else /* CMMU */
	return((p)->pgm.pg_pfn);
#endif
}

/*
 * set & get entire pde
 */

void
pg_setpde(p,pde)
pde_t *p;
int pde;
{
#ifndef CMMU
	p->pgi.pg_spde = pde; p++;
	p->pgi.pg_spde = pde; p++;
	p->pgi.pg_spde = pde; p++;
#endif
	p->pgi.pg_spde = pde;
}

pg_getpde(p)
pde_t *p;
{
	return(p->pgi.pg_spde);
}

/*
 * get page address
 */

pg_getaddr(p)
pde_t *p;
{
	return(ctob(pg_getpfn(p)));
}

/*
 * get table frame number
 */

pg_gettfn(p)
pde_t *p;
{
	return(p->pgm.pg_pfn);
}

/*
 * copy a pde to another pde
 */

void
pg_pdecp(pde1,pde2)
pde_t *pde1, *pde2;
{
#ifndef CMMU
	int i;

	for (i = 0; i < NPDEPP; i++, pde1++, pde2++)
#endif
		*pde2 = *pde1;
}

/*
 * check if any 1K page of the 4K set has been referenced
 */

pg_chkref(p,mask)
pde_t *p;
int mask;
{
#ifndef CMMU
	return (((p++)->pgi.pg_spde & mask) ||
		((p++)->pgi.pg_spde & mask) ||
		((p++)->pgi.pg_spde & mask) ||
		((p)->pgi.pg_spde & mask));
#else /* CMMU */
	return ((p)->pgi.pg_spde & mask);
#endif
}
E 1
