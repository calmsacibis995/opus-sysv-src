
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/var.h"
#include "sys/ram.h"
#include "sys/immu.h"
#include "sys/systm.h"

extern struct ram ram_tbl[];	/* ram configuration table */
extern unsigned ram_nbr;	/* number of entries in ram_tbl[] */
extern int bprobe();		/* probes a byte of ram, catches buserr */
extern struct probe probe_tbl[];/* probe initialization table */
/*
 * RAM configuration assumptions.
 * 	Noncontiguous blocks of memory expected.
 *	Minimum granuality of memory assumed to be 1 click.
 *	It is found in the address range 0-96Mb.
 */
#define MAXPHYSCL	(96*1024*1024)

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

	/* ram_tbl[0] is reserverd */
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
			
#if probingmemory
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
				if ( low == btoct( bzero(ctob( low ),
				    ctob( 1 ))))
					break;
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

#if probingmemory
	{ /* probe any required bytes */
	register struct probe *probe = probe_tbl;

	for( ; probe->probe_addr ; probe++ )
		(void) bprobe( probe->probe_addr, probe->probe_byte );
	}
#endif

	return( lastclick );
}
