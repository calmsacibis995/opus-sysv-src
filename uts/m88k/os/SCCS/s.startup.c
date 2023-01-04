h00447
s 00006/00006/00343
d D 1.6 90/06/28 22:34:18 root 6 5
c double table limit sizes
e
s 00003/00000/00346
d D 1.5 90/06/22 01:06:16 root 5 4
c flox is now settable from the boot line
e
s 00005/00005/00341
d D 1.4 90/06/20 19:01:03 root 4 3
c multiplied space allocations in mapvar by a factor of 5.
e
s 00001/00001/00345
d D 1.3 90/04/24 14:48:18 root 3 2
c changed strcpy to kstrcpy for dcc
e
s 00059/00000/00287
d D 1.2 90/03/27 17:06:07 root 2 1
c dynamically allocate proc, inode, s5inode, file and region arrays
e
s 00287/00000/00000
d D 1.1 90/03/06 12:28:11 root 1 0
c date and time created 90/03/06 12:28:11 by root
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

#ident	"@(#)kern-port:os/startup.c	10.10"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/psw.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/pfdat.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/map.h"
#include "sys/buf.h"
#include "sys/reg.h"
#include "sys/tty.h"
#include "sys/var.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
I 2
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/fs/s5inode.h"
I 5
#include "sys/flock.h"
E 5
E 2

extern user_t	*winuvad[];
extern pde_t	*winupde[];

/* Routine called before main, to initialize various data structures.
** The argument is:
**	physaddr - The address of the first available byte of memory.
*/

mlsetup(physaddr)
int	physaddr;
{
	register unsigned	nextclick;
	register unsigned	physclick;
	register unsigned	i;

	physclick = btoc(physaddr);

	/*
	 *	Initialize according to CPU board type.
	 */
	physclick = archinit0(physclick);

	/* 	Size and zero memory starting at the first 
	 *	available page.
	 */
	i = raminit(physclick);

	/*	Set up memory parameters.
	*/
	if (v.v_maxpmem  &&  (v.v_maxpmem < i))
		i = v.v_maxpmem;
	/* physmem = i; Now set in ram.c */
	maxclick = i;

	ASSERT(physclick < maxclick);

	/*	Initialize memory mapping for sysseg,
	**	the segment from which the kernel
	**	dynamically allocates space for itself.
	**	Also initialize virtual address space 
	**	table and turn on MMU.
	*/

	nextclick = mmuinit(physclick);

I 2
	/*	Initialize proc, region, file, inode, s5inode
	*/
	nextclick = mapvars(nextclick);

E 2
	/*	Initialize the kernel window segments.
	*/

	for (i = 0; i < NUBLK; ++i) {
		winuvad[i] = (struct user *)sptalloc(USIZE, 0, -1);
		winupde[i] = (pde_t *)kvtokptbl(winuvad[i]);
	}

	/*	Note that reginit must be called before
	**	mktables because reginit calculates the
	**	value of pregpp which is used by mktables.
	*/

	reginit();
	
	/*	Allocate some kernel tables.
	*/

	nextclick = mktables(nextclick);

	/*	Initialize the physical memory manager.
	*/

	kpbase = nextclick;
	meminit( nextclick );

	/*	Initialize process 0.
	*/

	p0init();
}

/*
 * Create system space to hold page allocation and
 * buffer mapping structures and hash tables
 */

mktables(nextfree)
{
	register int	m;
	register int	i;
	register preg_t	*prp;
	extern int	pregpp;

	
	/*
	 *      Allocate space for buffer headers, both
	 *      regular and physical.
	 */
	i = btoc((v.v_buf + v.v_pbuf) * sizeof(struct buf));
	bufz = (buf_t *)ctob(nextfree);
	pbufz = bufz + v.v_buf;
	nextfree += i;

	/*
	 *      Allocate space for disk buffers.
	 */
		bufferz = (char *)ctob(nextfree);
		nextfree += (v.v_buf + SBPP -1) / SBPP;
#ifdef CMMU
	/* cache inhibit disk buffers */
		i = btoc(bufz);
		while(i < nextfree) {
			pg_setci(kvtopde(ctob(i)));
			i++;
		}
#endif

	/*	Compute the smallest power of two larger than
	 *	the size of physical memory.
	 */

	m = physmem;
	while (m & (m - 1))
		 m = (m | (m - 1)) + 1;
	phashmask = (m>>3) - 1;

	/*	Allocate space for the page hash bucket
	 *	headers.
	 */

	i = btoc((m >> 3) * sizeof(*phash));
	phash = (struct pfdat **) sptalloc(i, PG_VALID, nextfree);
	nextfree +=i;

	for (i = 0; i <= phashmask; ++i)
		phash[i] = (struct pfdat *)&phash[i];

	/*	Allocate space for the pregion tables for each process
	 *	and link them to the process table entries.
	 *	The maximum number of regions allowed for is process is
	 *	3 for text, data, and stack plus the maximum number
	 *	of shared memory regions allowed.
	 */
	
	i = btoc(pregpp * sizeof(preg_t) * v.v_proc);
	prp = (preg_t *)sptalloc(i, PG_VALID, nextfree);
	nextfree +=i;
	for (i = 0  ;  i < v.v_proc  ;  i++, prp += pregpp)
		proc[i].p_region = prp;
	
	/*	Allocate space for the pfdat.
	*/

	i = btoc((maxclick - nextfree + 1) * sizeof(struct pfdat));
	pfdat = (struct pfdat *) sptalloc(i, PG_VALID, nextfree);
	pfdat = &pfdat[-nextfree];
	nextfree +=i;
	/*	Return next available physical page.
	*/

	return(nextfree);
}

/*	Setup proc 0
*/

extern	pde_t	*mmuupde;

p0init()
{
	register int	i;
	register pde_t	*upde = mmuupde;
	register pde_t  *pt;
	extern 	 int	cdlimit;

	/* Allocate the proc[0]'s segment table
	*/

#ifndef CMMU
	if ((proc[0].p_pttbl = (sde_t*)smpgalloc(pttosmpg(STBLSZ))) == NULL)
#else /* CMMU */
	if ((proc[0].p_pttbl = (sde_t*)sptalloc(SEGSIZE, PG_VALID, 0)) == NULL)
#endif
		cmn_err( CE_PANIC, "Can't allocate proc[0]'s ptr table");
#ifdef CMMU
	/* all page tables must be cache inhibited */
	pg_setci((pde_t *)kvtokptbl(proc[0].p_pttbl));
#endif
	bzero((char*)proc[0].p_pttbl, pttob(STBLSZ));

	/*	Setup proc 0's ublock
	*/

	if (sptmemall(proc[0].p_ubpgtbl, USIZE, PG_VALID) < 0)
		cmn_err( CE_PANIC, "Can't allocate proc[0]'s ublock");

	pt = &proc[0].p_ubpgtbl[0];
	pg_setref(pt);			/* avoid pmmu rmc cycles */
	pg_setmod(pt);			/* by presetting U and M bits */

	for (i=0; i<USIZE; ++i, pt += NPDEPP, upde += NPDEPP) {
		pg_pdecp(pt, upde);
	}

	/*	Initialize proc 0's ublock
	*/

	u.u_tsize   = 0;
	u.u_dsize   = 0;
	u.u_ssize   = 0;
	u.u_procp   = &proc[0];
	u.u_cmask   = CMASK;
	u.u_limit   = cdlimit;

	/*	initialize process data structures
	*/

	curproc = &proc[0];
	curpri = 0;

	proc[0].p_size = USIZE;
	proc[0].p_stat = SRUN;
	proc[0].p_flag = SLOAD | SSYS;
	proc[0].p_nice = NZERO;
}

/*	Machine-dependent startup code
*/

startup()
{
	extern int	nofiles_min;
	extern int	nofiles_max;
	extern int	nofiles_cfg;

	nofiles_cfg = v.v_nofiles;
	v.v_nofiles = min(v.v_nofiles, nofiles_max);
	v.v_nofiles = max(v.v_nofiles, nofiles_min);
	u.u_pofile  = (char*)&u.u_ofile[v.v_nofiles];

	/*	Check to see if the configured value of NOFILES
	**	was illegal and changed.  If so, print a message.
	*/

	if (nofiles_cfg != v.v_nofiles) {
	    if (nofiles_cfg < nofiles_min)
		   cmn_err(CE_NOTE,
		      "Configured value of NOFILES (%d) is less than min (%d)",
			  nofiles_cfg, nofiles_min);
	    else
		  cmn_err(CE_NOTE,
		    "Configured value of NOFILES (%d) is greater than max (%d)",
			 nofiles_cfg, nofiles_max);
	    cmn_err(CE_CONT, "        NOFILES set to %d.", v.v_nofiles);
	}

	/*	The following string is used to designate the end of the
	**	u-structure and helps determine stack over runs.
	*/

D 3
	strcpy( (char*)&u.u_pofile[v.v_nofiles], "This string marks the end of the u-structure.  The kernel stack should not extend beyond this string.");
E 3
I 3
	kstrcpy( (char*)&u.u_pofile[v.v_nofiles], "This string marks the end of the u-structure.  The kernel stack should not extend beyond this string.");
E 3
I 2
}

extern struct ksym {
	char 	*name;
	long 	val;
} ksyms[];

#define MAPV( V, E, T, MAXSZ ) \
	V = ctob(btoc(V*sizeof(struct T)))/sizeof(struct T); \
	if ( V*sizeof(struct T) > ctob(MAXSZ) ) \
		V = ctob(MAXSZ)/sizeof(struct T); \
	nextfree += mapvar((long *) &(E), V*sizeof(struct T), nextfree);

mapvars(nextfree) 
{
D 4
	MAPV( v.v_proc, proc, proc, 16 );
E 4
I 4
D 6
	MAPV( v.v_proc, proc, proc, 80 );
E 6
I 6
	MAPV( v.v_proc, proc, proc, 2*80 );
E 6
E 4
	v.ve_proc = (char *) &proc[v.v_proc];

D 4
	MAPV( v.v_inode, inode, inode, 16 );
E 4
I 4
D 6
	MAPV( v.v_inode, inode, inode, 80 );
E 6
I 6
	MAPV( v.v_inode, inode, inode, 2*80 );
E 6
E 4
	v.ve_inode = (char *) &inode[v.v_inode];

	if (v.v_region < (2 * v.v_proc))
		v.v_region = 2 * v.v_proc;
D 4
	MAPV( v.v_region, region, region, 16 );
E 4
I 4
D 6
	MAPV( v.v_region, region, region, 80 );
E 6
I 6
	MAPV( v.v_region, region, region, 2*80 );
E 6
E 4

D 4
	MAPV( v.v_file, file, file, 4);
E 4
I 4
D 6
	MAPV( v.v_file, file, file, 20);
E 6
I 6
	MAPV( v.v_file, file, file, 2*20);
E 6
E 4
	v.ve_file = (char *) &file[v.v_file];

	if (v.v_s5inode < ((4 * v.v_inode) / 5))
		v.v_s5inode = (4 * v.v_inode) / 5;
D 4
	MAPV( v.v_s5inode, s5inode, s5inode, 16 );
E 4
I 4
D 6
	MAPV( v.v_s5inode, s5inode, s5inode, 80 );
E 6
I 6
	MAPV( v.v_s5inode, s5inode, s5inode, 2*80 );
E 6
I 5

D 6
	MAPV( flckinfo.recs, flox, filock, 20);
E 6
I 6
	MAPV( flckinfo.recs, flox, filock, 2*20);
E 6
E 5
E 4

	return (nextfree);
}

mapvar( kva, size, next )
register long *kva;
register size, next;
{
	register i;

	size = btoc(size);

	*kva = ctob(next);	/* set new address */

	for (i = 0; ksyms[i].name ; i++) {	/* update symbol table */
		if (ksyms[i].val == (long) kva) {
			ksyms[i].val = *kva;
			break;
		}
	}
	return size;
E 2
}
E 1
