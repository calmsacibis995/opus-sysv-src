h04431
s 00209/00000/00000
d D 1.1 90/03/06 12:28:11 root 1 0
c date and time created 90/03/06 12:28:11 by root
e
u
U
t
T
I 1

/*		Copyright (c) 1987 Motorola Inc.		*/
/*		All Rights Reserved				*/
/*	%Q%	%W%	*/

/************************************************************************/
/* NAME									*/
/*	smpgalloc(), smpgfree(), imapalloc(), imapfree()		*/
/*									*/
/* DESCRIPTION								*/
/*	Smpg (alloc and free) manipulate chunks of memory smaller	*/
/*	than a system page.  Memory can be allocated (and freed)	*/
/*	as multiple chunks.  Multiple chunks are virtually and 		*/
/*	physically contiguous and may not be larger than a system page.	*/
/*	Smpgalloc() assumes that the allocation map structure, smpgmap,	*/
/*	has been zeroed on initialization.				*/
/*									*/
/*	smpgalloc() returns the virtual address of the beginning	*/
/*	of the allocated chunk(s) of memory.				*/
/*									*/
/*	imap (alloc and free) supply and retrieve virtually mapped	*/
/*	memory for lists of long word pointers. If a list fits in	*/
/*	a partial page, it is allocated a partial page.			*/
/*									*/
/************************************************************************/

#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/smpgmap.h"
#include "sys/var.h"
#include "sys/cmn_err.h"
#include "sys/debug.h"

#define SMPGWANTED	2
#define SMPGLOCKED	1
#define ALLCHKSFREE	0xf		/* All four chunks free */

static int smpgflag;	/* lock/request flags insure one at a time */
static struct smpgmap *smpgtop = smpgmap;
static struct smpgmap *smpgend = &smpgmap[-1];


caddr_t
smpgalloc(nchks)
register nchks;
{
	register struct smpgmap *smpg;
	register k;
	register caddr_t blk;
	caddr_t retaddr;
	register freechks;

	ASSERT((nchks > 0) && (nchks <= (NBPP/SMPGSIZE)));

loop:
	for(smpg = smpgtop; smpg < &smpgmap[v.v_smpgmapsz]; smpg++) {

		/*
		 * Find a page with free chunks.
		 */

		if ((blk = smpg->smp_addr) == NULL) {
			if (smpgflag & SMPGLOCKED) {
				smpgflag |= SMPGWANTED;
				sleep((caddr_t)smpgmap, PMEM);
			} else {
				/* allocate a virtual page with physical mem */
				smpgflag |= SMPGLOCKED;
				if ((blk = (caddr_t) sptalloc(1, PG_VALID, 0)) != 0) {
					smpgend++;
					ASSERT(smpg == smpgend);
					smpgend->smp_addr = blk;
					smpgend->smp_bmap = ALLCHKSFREE;
				}
				if (smpgflag & SMPGWANTED)
					wakeup((caddr_t)smpgmap);
				smpgflag &= ~(SMPGWANTED | SMPGLOCKED);
				if (blk == NULL)
					return( NULL );
			}
			goto loop;	/* start over, much may have changed */
		} 

		if ( ! smpg->smp_bmap) {	/* no free chunks ? */
			if (smpg == smpgtop)
				++smpgtop;
			continue;		/* try another */
		}

		/*
		 * Try to satisfy request.
		 */

		freechks = 0;
		for(k = 1; k <= smpg->smp_bmap; k <<= 1, blk += SMPGSIZE) {
			if (smpg->smp_bmap & k) {
				if (freechks++ == 0)	/* if first free chk */
					retaddr = blk;	/* remember addr */
			} else
				freechks = 0;	/* start again, too few chks */
			if (freechks == nchks) {
				while(--freechks)
					k |= (k >> 1);
				smpg->smp_bmap &= ~k;
				return( retaddr );
			}
		}

		/*
		 * This page won't due, try next one.
		 */
	}

	/*
	 * Not enough map structures for pages holding tables.
	 */
	cmn_err(CE_PANIC,"Small page table allocation map is full!");
	/* NOT REACHED */
	return (NULL);
}



smpgfree(vaddr, nchks)
register caddr_t vaddr;		/* virt addr of first chunk to free */
register nchks;			/* number of chunks to free */
{
	register struct smpgmap *smpg;
	register struct smpgmap *nsmpg;
	register 	mask;
	register caddr_t pg_addr;

	ASSERT((nchks > 0) && (nchks <= (NBPP/SMPGSIZE)));
	nsmpg = NULL;

	pg_addr = (caddr_t) ctob(btoct(vaddr));		/* virt addr of page */

	/*
	 * Find map for this page.
	 */

	for(smpg = smpgend; smpg >= smpgmap; smpg--) {
		if (smpg->smp_addr == pg_addr) {	/* found ? */
			mask = 1 << (poff(vaddr) / SMPGSIZE);
			while(--nchks)
				mask |= (mask << 1);
			ASSERT((smpg->smp_bmap & mask) == 0);
			smpg->smp_bmap |= mask;		/* mark chunks free */
			smpgtop = (struct smpgmap *) min( smpgtop, smpg );
			if (smpgflag & SMPGWANTED) {
				smpgflag &= ~SMPGWANTED;
				wakeup((caddr_t)smpgmap);
			}
			if((smpg->smp_bmap != ALLCHKSFREE)
				||(smpgflag & SMPGLOCKED))
				return;

			/*
			 * Could make the followin optional
			 * if smpg < tuneable smpgmap level.
			 */
			sptfree(smpg->smp_addr, 1, 1);	/* free whole page */
			nsmpg = smpg + 1;
			smpgend--;
			break;
		}
	}

	ASSERT(nsmpg != NULL);	/* Must find table entry for free chunk */

	/*
	 * Move all the remaining map entries down.
	 */
	for( ; nsmpg < &smpgmap[v.v_smpgmapsz]; smpg++, nsmpg++) {
		if ((smpg->smp_addr = nsmpg->smp_addr) == 0)
			return;
		smpg->smp_bmap = nsmpg->smp_bmap;
	}
	smpg->smp_addr = 0;
}


int *
imapalloc( maplen)
int maplen;		/* number of long word pointers in map */
{
	int mapsiz = maplen * NBPW;

	if( btopt( mapsiz) >= NPTPP )
		return((int *)sptalloc( btoc(mapsiz), PG_VALID, 0));
	/* else */
		return((int *)smpgalloc( pttosmpg( btopt(mapsiz))));
}


imapfree( vaddr, maplen)
caddr_t vaddr;		/* map virtual location */
int maplen;		/* number of long word pointers in map */
{
	int mapsiz = maplen * NBPW;

	if( btopt( mapsiz ) >= NPTPP)
		sptfree( vaddr, btoc( mapsiz), 1);
	else
		smpgfree( vaddr, pttosmpg( btopt( mapsiz)));
	
}
E 1
