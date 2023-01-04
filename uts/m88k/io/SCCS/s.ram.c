h15011
s 00002/00012/00125
d D 1.3 90/05/31 20:06:55 root 3 2
c general clean up
e
s 00004/00029/00133
d D 1.2 90/05/16 09:58:31 root 2 1
c made MAXPHYSCL 96MB
e
s 00162/00000/00000
d D 1.1 90/03/06 12:28:58 root 1 0
c date and time created 90/03/06 12:28:58 by root
e
u
U
t
T
I 1
D 3
/*		Copyright (c) 1985 AT&T		*/
/*		All Rights Reserved		*/
/* @(#)ram.c	6.2	   */
E 3

#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/var.h"
D 3
#ifdef	ASPEN
#include "sys/aspen.h"
#endif
E 3
#include "sys/ram.h"
#include "sys/immu.h"
#include "sys/systm.h"
D 3
#ifdef opus
#include "sys/commpage.h"
#endif
E 3

extern struct ram ram_tbl[];	/* ram configuration table */
extern unsigned ram_nbr;	/* number of entries in ram_tbl[] */
D 3
/*extern caddr_t bsmear();	/* zeros a block of ram, catches buserr */
#define	bsmear	bzero
E 3
extern int bprobe();		/* probes a byte of ram, catches buserr */
extern struct probe probe_tbl[];/* probe initialization table */
/*
 * RAM configuration assumptions.
 * 	Noncontiguous blocks of memory expected.
 *	Minimum granuality of memory assumed to be 1 click.
D 2
 *	It is found in the address range 0-64Mb.
E 2
I 2
 *	It is found in the address range 0-96Mb.
E 2
 */
D 2
#define MAXPHYSCL	(64*1024*1024)
E 2
I 2
#define MAXPHYSCL	(96*1024*1024)
E 2

/*
 * raminit(physclick) - initializes memory.
 *	Zeros memory beginning at physclick and then higher
 *	-- according to ram_tbl[].
 *	Initializes troublesome memory by probing it.
 *	Sets physmem to the total number of clicks of physical memory.
 *	Returns click number of last physical click found.
 */

int
raminit(physclick)
register int physclick;	/* first click to test and zero */
{
	register struct ram *ram = ram_tbl;
	register unsigned nbr = ram_nbr;
	register unsigned low, ram_end, end, top, lastclick, physize;

I 3
	/* ram_tbl[0] is reserverd */
E 3
	ram->ram_sz = (uint) getmemsize();		/* get it first */
	ram->ram_sz = btoc(ram->ram_sz);		/* make it clicks */
	ram->ram_hi = ram->ram_sz - 1;

	lastclick = physclick - 1;
	physize = physclick;
	/*
		Don't go beyond the end of "ram" memory
		as defined in dfile - maxpmem (v.v_maxpmem),
		as it will clear the area that should be
		loaded with the ram disk image.  v.v_maxpmem
		is set to a nonzero value if a ram disk is
		to be installed.  This will define the end
		of user/kernel memory and the start of ram
		disk memory.
	*/
	if (v.v_maxpmem)
		ram_end = v.v_maxpmem;
	else
		ram_end = MAXPHYSCL;
    	/*
	 * locate and initialize ram,
	 * all addresses and sizes from ram_tbl in clicks
	 */
	for ( ; nbr ; nbr--, ram++ ) {

		if ( ram->ram_flg & RAM_PRIV )
			continue;

		low = ram->ram_lo;

		if ( ram->ram_hi < ram_end )
			end = ram->ram_hi + 1;
		else
			end = ram_end;

		if ( low < physclick )	/* start with physclick */
			low = physclick;

		for ( ; low < end; low++ ) {
			
D 2
#if 0
E 2
I 2
#if probingmemory
E 2
			/* probe chunk of memory */
			if ( ( ram->ram_flg & (RAM_INIT | RAM_HUNT ) )
			    == RAM_HUNT ) {
				if ( bprobe ( ctob ( low ), -1 ) )
					break;
				else
					goto exists;
			}
#endif
	
			/* zero chunk of memory */
			if ( ram->ram_flg & RAM_INIT ) {
D 2
#ifndef SIMULATOR
E 2
D 3
				if ( low == btoct( bsmear(ctob( low ),
E 3
I 3
				if ( low == btoct( bzero(ctob( low ),
E 3
				    ctob( 1 ))))
					break;
D 2
#endif
E 2
			}
exists:	
			ram->ram_flg |= RAM_FOUND;
			physize++;
			top = low;
		}
		/*
		 * Adjust ram_hi and ram_sz according to what
		 * ram actually exists
		 */
		if ( ram->ram_flg & RAM_FOUND ) {
			if ( lastclick < top )
				lastclick = top;
			ram->ram_hi = top;
			ram->ram_sz = top - ram->ram_lo + 1;
		}
	}

	physmem = physize;

D 2
#if 0
E 2
I 2
#if probingmemory
E 2
	{ /* probe any required bytes */
	register struct probe *probe = probe_tbl;

	for( ; probe->probe_addr ; probe++ )
		(void) bprobe( probe->probe_addr, probe->probe_byte );
	}
#endif

	return( lastclick );
}
D 2

#ifdef	ASPEN     /* aspen specific cache flush routines */
/* Cache flush subroutine. */
cacheflush()
{
	*FUDC=1;	/* Flush user data cache. */
	*FUCC=1;	/* Flush user code cache. */
	*FSDC=1;	/* Flush kernel data cache. */
/* Flushing kernel code space is not needed. */
}
scacheflush()
/* Flush kernel data cache */
{
	*FSDC=1;	/* Flush kernel data cache. */
/* Flushing kernel code space is not needed. */
}
ucacheflush()
/* Flush user cache */
{
	*FUDC=1;	/* Flush user data cache. */
	*FUCC=1;	/* Flush user code cache. */
}
#endif
E 2
E 1
