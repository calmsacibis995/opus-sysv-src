h04684
s 00003/00000/01257
d D 1.12 90/06/28 02:16:36 root 12 11
c added ptmax as a boot option
e
s 00003/00002/01254
d D 1.11 90/06/27 02:18:04 root 11 10
c more changes to limit dmaspace size to 16mb
e
s 00004/00001/01252
d D 1.10 90/06/22 01:00:11 root 10 9
c added modifiable flox
e
s 00013/00000/01240
d D 1.9 90/06/14 22:21:35 root 9 8
c put a max for dma space at 16 Mb
e
s 00017/00000/01223
d D 1.8 90/05/31 20:26:10 root 8 7
c added vendorfunc routine
e
s 00003/00001/01220
d D 1.7 90/05/16 11:32:00 root 7 6
c cachecheck preserves the memory it is clobbering for its test.
e
s 00001/00001/01220
d D 1.6 90/04/24 11:18:34 root 6 5
c added volatile in archinit2
e
s 00011/00005/01210
d D 1.5 90/04/20 12:46:32 root 5 4
c added dynamic print buffer sizing
e
s 00014/00013/01201
d D 1.4 90/04/19 09:20:32 root 4 3
c cleaned up flags
e
s 00042/00001/01172
d D 1.3 90/04/05 12:51:02 root 3 2
c added shared memory & message boot line parameters
e
s 00008/00003/01165
d D 1.2 90/04/03 14:21:28 root 2 1
c -S to unix sets init to single user mode
e
s 01168/00000/00000
d D 1.1 90/03/06 12:28:53 root 1 0
c date and time created 90/03/06 12:28:53 by root
e
u
U
t
T
I 1
/* SID @(#)opus.c	1.8 */
#define DEBUG	1
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
#include "sys/utsname.h"
#include "sys/tty.h"
#include "sys/var.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
#include "sys/tuneable.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/iorb.h"
#include "sys/fs/s5param.h"

int dmafree;  /* kernel virtual address of next free byte in dma space */
pde_t *dmapte;  /* kernel virtual address of dma space map */
int dmamaxpg;  /* max pages available for dma */
int availdmem;	/* available dma window virtual memory */
int dmaspace = DMASPACE;
int hwregaddr = HWREGADDR;
I 5
int prfbufsize = 1;	/* 1 page == 4096 bytes */
E 5
extern ndkpgs;

int tstsnoop;
extern	v_dmamap;
#define pg_setwt(P)	((P)->pgm.pg_wt = 1)
#define setci(P)	((P)->pgm.pg_ci = 1)
#define clrci(P)	((P)->pgm.pg_ci = 0)
#define tstci(P)	((P)->pgm.pg_ci)

#define		NUMDMAPG	npgs*NPGPT


/*
 * Initialize the dma mapping variables.
 * COMMPAGE                -> 0xD0000000
 * Disk Window             -> 0xD0001000
 * Available dynamic space -> 0xD0002000
 */
dmamapinit(base, npgs)
pde_t *base;
{
	dmapte = base;
	dmafree = 0;

	dmamaxpg = NUMDMAPG;
	availdmem = NUMDMAPG;
	mapinit(dmamap, v_dmamap);
	mfree(dmamap, NUMDMAPG, 1+ndkpgs);
	dkwindowinit();		/* allocate page for blk disk io */

	/* double map commpage in DMASPACE and make r/w */
	availdmem--;
	pg_setvalid(base);
	pg_setpfn(base, 0);
	pg_setci(base);
}

caddr_t vmspace[] = {
	(caddr_t) 0,		/* KERNEL STATIC SPACE */
	(caddr_t) SYSSEGS,	/* KERNEL DYNAMIC SPACE */
	(caddr_t) DMASPACE,	/* OPUS DMA SPACE */
	(caddr_t) 1		/* END OF LIST */
};

I 9
long vmsize[] = {	/* size of above address space 0: maps physmem size */
	0,		/* KERNEL STATIC SPACE SIZE */
	0,		/* KERNEL DYNAMIC SPACE SIZE */
	0,		/* OPUS DMA SPACE SIZE */
	1		/* END OF LIST */
};

#define	MAXDMASIZE (16*1024*1024)

E 9
archinit0(physclick)	/* called from mlsetup */
{
	register memsize;

	/* allocate 1/8 of physmem for buffers */
	memsize = swapl(COMMPAGE0->mem_size);
	v.v_buf = (memsize/8)/SBUFSIZE;
I 9

	/* limit dma space */
	if (memsize > MAXDMASIZE)
D 11
		vmsize[2] = MAXDMASIZE;
E 11
I 11
		memsize = MAXDMASIZE;
	vmsize[2] = memsize;
E 11
E 9
	
	unixparm();

I 5
	prfloginit (ctob(physclick), ctob(prfbufsize));
	physclick += prfbufsize;

E 5
	return (physclick);
}

archinit1(physclick)	/* called before mapping is turned on */
register physclick;
{
	register pde_t *pde;

	/* allocate disk physical buffer */
	physclick = dkbufalloc(physclick);

	/* initialize H/W top level page table entry */
	physclick = mkpgtbl(pnum(hwregaddr), 1, sstbl, physclick);

	/* initialize H/W bottom level page table entry */
	pde = kvtopde(hwregaddr);
	pg_setci(pde);
	pg_setpfn(pde, pnum(hwregaddr));
	pg_setvalid(pde);
	
	/* initialize opmon dma maps */
	pde = kvtopde(DMASPACE);
D 11
	dmamapinit(pde, ctost(physmem));
E 11
I 11
	dmamapinit(pde, ctost(vmsize[2]));
E 11

	return (physclick);
}

archinit2(physclick) 	/* called after mapping is turned on */
{
	unsigned short dix;
D 6
	register struct commpage *C;
E 6
I 6
	register volatile struct commpage *C;
E 6

	/* opmon hand shake */
	C = COMMPAGE;
	dix = swaps(DIXSIZ);
	C->map = swapl(dmapte);			/* paddr of dma map */
	C->map_size = swapl(dmamaxpg*sizeof(int));/* its size in bytes */
	C->dixsiz = dix;			/* swapped DIXSIZ */
	C->magic = COMMPAGE->type;		/* copy magic */
	*(unsigned char *)CNTRL0 = (F_AT|SET);	/* set flag */
#if !WS
	while (C->dixsiz == dix);		/* wait for opmon */
	C->dixsiz = swaps(C->dixsiz);		/* swap it for unix drivers */
#endif
	return (physclick);
}

/*
 * Allocate 'len' bytes of memory which is mapped into dma space.
 * Returns the kernel virtual address of the allocated space.
 * This routine is called only during initialization so a failure
 * to get memory or address space results in a panic.
 * The allocator will try to use space at the end of the last
 * page it mapped if 'len' will fit there, otherwise a new portion
 * of address space will be allocated.
 */
dmastatic(len)
int len;
{
	register int base;
	register npages;
	int pg,i;

	/* round length up to long boundary */
	len = (len+3) & (~3);

	/* allocate out of free area at end of current page if possible */
	if (poff(dmafree) &&
	    (NBPP - poff(dmafree)) >= len) {
		base = dmafree;
		dmafree += len;
		return(base);
	}
	
	/* allocate new pages */
	
	npages = btoc(len);
	if ((pg = malloc(dmamap, npages)) == 0) {
		cmn_err(CE_PANIC, "dmastatic: out of dma address space");
	}
	dmamaxpg -= npages;
	availsmem -= npages;
	availrmem -= npages;
	availdmem -= npages;
	reglock(&sysreg);
	/* memlock(); */
	if (npages > 4) {
		base = getcpages(npages, 1);
		if (base == NULL)
			cmn_err( CE_PANIC, "dmastatic : getcpages failed");
		for ( i = 0; i < npages ; i++ ) {
			dmapte[pg+i].pgi.pg_spde = base+(i*NBPP)+PG_VALID;
		}
	}
	else {
		if ( ptmemall(&sysreg, &dmapte[pg], npages, 1) )
			cmn_err( CE_PANIC, "dmastatic : ptmemall failed");
	}
	/* memunlock(); */
	regrele(&sysreg);
	base = DMASPACE + ctob(pg);
	for (i=0;i<npages;i++) {
		if (tstsnoop) {
			dmapte[pg+i].pgi.pg_spde |= tstsnoop;
		}
		else setci(&dmapte[pg+i]);
	}
	dcacheflush(0);
	ccacheflush(0);
	mmuflush();
	bzero(base, ctob(npages));
	/* use remainder of prior page if contiguous to new page */
	if ( poff(dmafree) && ((dmafree+(NBPP-1)) & ~(NBPP-1)) == base )
		base = dmafree;
	dmafree = base + len;
	return(base);
}

/* align the dma buffer for those who require it */
dmaalign(al)
register al;
{
	if ((al & ( al - 1)) == 0) {	/* must be a power of two */
		dmafree += (al - 1);
		dmafree &= ~(al - 1);
	}
}


xdmastatic(len)
{
}

/*
 * Map the system virtual address space starting at 'sysadx' for 'len'
 * bytes into dma space. The virtual address in dmaspace is returned.
 * The routine may sleep awaiting free dma space.
 */
mapdma(sysadx, len)
unsigned int sysadx, len;
{
	int npages;
	register int pg;
	register int i;
	register int pte;
	register int s;
	register pde_t *spde, *dpde;

	npages = btoc(len + poff(sysadx));
#if WS
	if (npages == 1) {
		/* single page. just use physical address */
		if ( sysadx > (int)&sysadx) {
			panic("cannot map stack space\n");
		}
		else {
			pte = (int) kvtophys(sysadx);
		}
		return(pte);
	}
#endif
	s = spl7();
	while ((pg = malloc(dmamap, npages)) == NULL) {
		cmn_err(CE_NOTE, "mapdma waiting %d pages", npages);
		mapwant(dmamap)++;
		sleep((caddr_t)dmamap,PSWP);
	}
	availdmem -= npages;
	splx(s);

	spde = kvtopde(sysadx);
	dpde = &dmapte[pg];
	for (i = 0; i < npages; i++, dpde++, spde++) {
		dpde->pgi.pg_spde  = spde->pgi.pg_spde | PG_CI ;
	}
	mmuflush();
	dcacheflush(0);
	return(DMASPACE + ctob(pg) + poff(sysadx));
}

/*
 * Release the dma address space starting at virtual address 'dmaadx'
 * for 'len' bytes. The address space is returned to the free space
 * pool.
 */
relsdma(dmaadx, len)
int dmaadx, len;
{
	register int s;
	register int pg;
	register int endpg;

#if WS
	if ((dmaadx&DMASPACE) != DMASPACE)
		return;
#endif
	pg = btoct(dmaadx - DMASPACE);
	endpg = pg + btoc(len + poff(dmaadx));
	s = spl7();
	for ( ; pg < endpg; ++pg) {
		dmapte[pg].pgi.pg_spde = (uint) 0;
	}
	mmuflush();
	dcacheflush(0);
	mfree(dmamap, btoc(len + poff(dmaadx)), btoct(dmaadx - DMASPACE));
	availdmem += btoc(len + poff(dmaadx));
	if (mapwant(dmamap)) {
		mapwant(dmamap)--;
		wakeup(dmamap);
	}
	splx(s);
}

/*
 * Map the data area for a buffer request into dma space.
 * Returns the address of the data area as a dma space relative
 * address.
 * This routine must be called in the context of user running, since
 * it calls uvtopde. This means it cannot be called on an interrupt.
 * For example the driver strategy routine must call it when invoked the
 * the first time.
 */
#if !WS
dmamapbuf(bp)
register struct buf *bp;
{
	int s;
	register pde_t *map, *dpte;
	register unsigned i, addr;
	
	/* allocate dma pages */
	bp->b_dpages = min(dmamaxpg, btoc(bp->b_bcount + poff(paddr(bp))));
	s = spl7();
	while ((bp->b_dma = malloc(dmamap, bp->b_dpages))==0) {
		cmn_err(CE_NOTE, "dmamapbuf waiting %d pages", bp->b_dpages);
		mapwant(dmamap)++;
		sleep((caddr_t)dmamap,PSWP);
	}
	availdmem -= bp->b_dpages;
	splx(s);

	dpte = &dmapte[bp->b_dma];
	for (i = 0; i<bp->b_dpages;i++, dpte++ ) {
		addr = paddr(bp) + ctob(i);
		if (bp->b_flags & B_PHYS)
			map = uvtopde(addr);
		else
			map = kvtopde(addr);
		dpte->pgi.pg_spde  = map->pgi.pg_spde | PG_CI ;
	}
	mmuflush();
	dcacheflush(0);

	return(DMASPACE | ctob(bp->b_dma) | poff(paddr(bp)));
}

/*
 * Free up the dma space used by request 'bp'.
 * This will wake up processes waiting for space.
 */
dmaunmapbuf(bp)
register struct buf *bp;
{
	int s;
	register pde_t *dpte;
	register i;

	dpte = &dmapte[bp->b_dma];
	s = spl7();
	for (i = 0; i<bp->b_dpages;i++, dpte++)
		dpte->pgi.pg_spde = 0;
	mfree(dmamap,bp->b_dpages,bp->b_dma);
	availdmem += bp->b_dpages;
	if (mapwant(dmamap)) {
		mapwant(dmamap)--;
		wakeup(dmamap);
	}
	mmuflush();
	splx(s);
}
#endif

dmamapuser(rw)
{
	register unsigned s;
	register pde_t *map, *dpte;
	register i;
	register dma, pages;
	
	/* allocate dma pages */
	pages = btoc(u.u_count + poff(u.u_base));
	s = spl7();
	if ((dma = malloc(dmamap, pages))==0) {
		splx(s);
		return(0);
	}
	availdmem -= pages;
	splx(s);

	/* lock user pages */
	if (userdma(u.u_base, u.u_count, rw) == 0) {
		mfree(dmamap, pages, dma);
		cmn_err(CE_WARN, "useracc failed call %x, base %x, cnt %x", 
				u.u_syscall, u.u_base, u.u_count);
		return (0);
	}
	s = (int) u.u_base;
	dpte = &dmapte[dma];
	for (i = 0; i<pages;i++, dpte++) {
		/* user map may not be contiguous */
		map = uvtopde(s);
		if (pg_tstvalid(map)) {
			dpte->pgi.pg_spde = map->pgi.pg_spde | PG_CI;
			if (!tstci(map)) {
				/* save cache inhibit bit */
				dpte->pgm.pg_ndref = 1;
				setci(map);
				setci(kvtopde(map->pgi.pg_spde));
			}
			else
				dpte->pgm.pg_ndref = 0;
		} else
			panic("useracc failure\n");
		s += ctob(1);
	}
	mmuflush();
	dcacheflush(0);

	return(DMASPACE | ctob(dma) | poff(u.u_base));
}

/*
 * Free up the dma space used by user request .
 * This will wake up processes waiting for space.
 */
dmaunmapuser(dma, rw)
{
	register unsigned s;
	register i;
	int pages;
	register pde_t *dpte, *map;

	dma = btoct(dma-DMASPACE);
	pages = btoc(u.u_count + poff(u.u_base));
	dpte = &dmapte[dma];
	s = (unsigned) u.u_base;
	for (i = 0; i<pages;i++, dpte++) {
		map = uvtopde(s);
		/* reset cache inhibit bit */
		if ((map->pgi.pg_spde&PG_VALID) &&
				(map->pgm.pg_pfn == dpte->pgm.pg_pfn)) {
			
			if (dpte->pgm.pg_ndref) {
				clrci(map);
				clrci(kvtopde(map->pgi.pg_spde));
			}
		}
		else
			cmn_err(CE_PANIC, "dmaunmapuser");
		dpte->pgi.pg_spde = 0;
		s += ctob(1);
	}
	mmuflush();
	dcacheflush(0);
	s = spl7();
	mfree(dmamap,pages,dma);
	availdmem += pages;
	if (mapwant(dmamap)) {
		mapwant(dmamap)--;
		wakeup(dmamap);
	}
	undma(u.u_base, u.u_count, rw);
	splx(s);
}
long dkadx;
pde_t *dkpte;

/* allocate a dma 4K window for block disk i/o */
dkwindowinit()
{
	dkpte = dmapte+1;
	availdmem -= ndkpgs;
	dkadx = DMASPACE+ctob(1);
}
	
cdlock(p)
struct proc *p;
{
	p->p_compat |= SCOMPAT; 
}


#include "sys/sysconf.h"
char indtab[DIXSIZ];
struct confinfo confinfo = {
	"opus",  /* node name */
	makedev(0, 0),  /* rootdev */
	makedev(0, 0),  /* pipedev */
	makedev(0, 120),  /* dumpdev */
	makedev(0, 120),  /* swapdev */
	0,  /* swaplo */
#if WS
	8*2048,  /* nswap */
#else
	2048,  /* nswap */
#endif
	"c:\opus\opsash",  /* boot program */
	"/unix",  /* unix */
	0,  /* sectors */
	0,  /* spares */
	0,  /* used */
	0,  /* entries */
	0,  /* sparetbl */
	0,  /* rootsects */
	3,  /* fstype */
	{ 0 },  /* pad1 */
	{ {0,-1},{4000,-1},{8000,-1},{12000,-1},
	  {16000,-1},{20000,-1},{24000,-1},{0,-1} },  /* logvol */
	{ {{0}}, '?', '7', {0}, "PST8PDT", "xt" },  /* dosinfo */
	{ 0 },  /* pad2 */
	64,  /* system buffers */
	0,  /* magic */
	0,  /* csum */
};
struct confoverride confoverride;
unsigned cfreedmaoffset;
int blkacty;
errinit() {}

fmtberr() {}

logberr() {}

int kdebug = 0xc00;
int knochange = 0xe80;	/* disk , paging, signals */
int printflg;
extern cdlimit;
int swapfirstfit;
D 2
int swaplim;
E 2
I 2
int defaultlevel = -1;
I 3
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/msg.h"
I 10
#include "sys/flock.h"
E 10
E 3
E 2

I 3
extern struct shminfo	shminfo;	/* shared memory info structure */
extern struct msginfo	msginfo;	/* message parameters */
I 12
extern ptmax;
E 12
D 4
struct miscinfo 
E 4
I 4
D 5
struct ipcinfo 
E 5
I 5
struct miscinfo 
E 5
E 4
{
	long	*shmmax,
		*shmmin, 
		*shmseg,
		*shmbrk,
		*shmall,
		*msgmax,
		*msgmnb,
		*msgssz,
D 4
		*msgtql,
		*deflvl
} miscinfo = {
E 4
I 4
D 5
		*msgtql;
} ipcinfo = {
E 5
I 5
		*msgtql,
I 10
		*flox,
I 12
		*ptmax,
E 12
E 10
		*prfsiz;
} miscinfo = {
E 5
E 4
		&shminfo.shmmax,
		&shminfo.shmmin, 
		&shminfo.shmseg,
		&shminfo.shmbrk,
		&shminfo.shmall,
		&msginfo.msgmax,
		&msginfo.msgmnb,
		&msginfo.msgssz,
		&msginfo.msgtql,
I 10
		&flckinfo.recs,
I 12
		&ptmax,
E 12
E 10
I 5
		&prfbufsize,
E 5
D 4
		&defaultlevel,
E 4
};

E 3
/*
 * Parse unix start up parameters.
 */
unixparm()
{
	register int i,j;
	register char *cp;
	long *p;
	extern struct confoverride confoverride;
	extern int freemem;
	char buf[128];
	char ch;
	int argc;
	char *args[32];
	register char **argv = args;

	cp = COMMPAGE0->cmdline;
	i = 0;
	while (((ch = *cp++) != 0) && (ch != ';'))  ;
	if (ch == 0) return;
	while ((ch = *cp++) != 0) {
		buf[i] = ch;
		if (++i >= 128) break;
	}
	argc = setargv(buf, argv, 32);

	while (argc > 0) {
		if (argv[0][0] == '-') {
			switch (argv[0][1]) {

				case 'd':
				kdebug = asciitoint(&argv[0][2]);
				if (kdebug == 0)
					kdebug = 1;
				break;

				case 'k':
				confoverride.dumpdev = asciitoint(argv[1]);
				confoverride.flags |= OVDUMPDEV;
				argc--; argv++;
				break;

				case 'r':
				confoverride.rootdev = asciitoint(argv[1]);
				confoverride.flags |= OVROOTDEV;
				argc--; argv++;
				break;

				case 'p':
				confoverride.pipedev = asciitoint(argv[1]);
				confoverride.flags |= OVPIPEDEV;
				argc--; argv++;
				break;

				case 's':
				confoverride.swapdev = asciitoint(argv[1]);
				confoverride.flags |= OVSWAPDEV;
				argc--; argv++;
				break;

				case 'l':
				confoverride.swplo = asciitoint(argv[1]);
				confoverride.flags |= OVSWPLO;
				argc--; argv++;
				break;

				case 'w':
				confoverride.nswap = asciitoint(argv[1]);
				confoverride.flags |= OVNSWAP;
				argc--; argv++;
				break;

				case 'b':
				if ((i = asciitoint(argv[1])) < 0) {
					i = (freemem * (-i)) / 100;
				}
#if m88k
				v.v_buf = (i < 1) ? 1 : i;
#else
				confoverride.nbuf = (i < 1) ? 1 : i;
				confoverride.flags |= OVNBUF;
#endif
				argc--; argv++;
				break;

				case 'n':
				for (i = 0; i < NODENAMESIZE; i++) {
					confoverride.nodename[i] = argv[1][i];
					if (argv[1][i] == 0) {
						break;
					}
				}
				confoverride.flags |= OVNODENAME;
				argc--; argv++;
				break;

				case 'i':
				confoverride.blk0dix = asciitoint(argv[1]);
				confoverride.flags |= OVBLK0DIX;
				argc--; argv++;
				break;

				case 'z':
				confoverride.drive = asciitoint(argv[1]);
				confoverride.sect = asciitoint(argv[2]);
				confoverride.sectstart = asciitoint(argv[3]);
				confoverride.sectlen = asciitoint(argv[4]);
				confoverride.flags |= OVSECTDEF;
				argv += 4; argc -= 4;
				break;

				case 'm':
				v.v_maxpmem = asciitoint(argv[1]);
				argc--; argv++;
				break;

				case 'q':
				printflg &= ~PRINT_STD;
				break;

				case 'Q':
				printflg &= ~(PRINT_STD|PRINT_COPYRIGHT);
				break;

				case 'v':
				printflg = printflg;  /* nothing for now */
				break;

				case 'a':
				swapfirstfit = 1;
				break;

				case 'u':
				i = asciitoint(argv[1]);
				if (i > cdlimit)
					cdlimit = i;
				argc--; argv++;
				break;

D 4
				case 'S':
D 2
				swaplim = asciitoint(argv[1]);
				argc--; argv++;
E 2
I 2
D 3
				defaultlevel = 7;
E 3
I 3
				p = (long *) &miscinfo;
				j = sizeof(struct miscinfo)/sizeof(int);
E 4
I 4
D 10
				case 'I':
E 10
I 10
				case 'M':
E 10
D 5
				p = (long *) &ipcinfo;
				j = sizeof(struct ipcinfo)/sizeof(int);
E 5
I 5
				p = (long *) &miscinfo;
				j = sizeof(struct miscinfo)/sizeof(int);
E 5
E 4
				if ( argv[0][2] >= '0' && argv[0][2] <= '9' ) {
					i = asciitoint(&argv[0][2]);
D 4
					if ( i >= 0 && i < j )
E 4
I 4
					if ( i >= 0 && i < j ) {
E 4
						p += i;
D 4
					p = (long *) *p;
					if (j == 9)
						*p = 7;	/* defaultlevel */
					else
E 4
I 4
						p = (long *) *p;
E 4
						*p = asciitoint(argv[1]);
I 4
					}
E 4
				}
I 4
				argc--; argv++;
				break;

				case 'S':
				defaultlevel = 7;
E 4
E 3
E 2
				break;

				case 'T':
				p = (long *) &tune;
				j = sizeof(struct tune)/sizeof(int);
				goto tunevar;

				case 'V':
				p = (long *) &v;
				j = sizeof(struct var)/sizeof(int);

				tunevar:
				if ( argv[0][2] >= '0' && argv[0][2] <= '9' ) {
					i = asciitoint(&argv[0][2]);
					if ( i >= 0 && i < j )
						p[i] = asciitoint(argv[1]);
				}
				argc--; argv++;
				break;

				case 'C':	/* cache modes */
					j = asciitoint(&argv[0][3]);
					cfgcache(argv[0][2], j);
					break;
			}
		}
		argc--; argv++;
	}
}

int cachepolicy = 0xff;
cfgcache(c, val)
register char c;
register val;
{
	extern ucacheinh, dcacheinh, ccacheinh, usebatc;
	extern long cmmuinfo[];
	switch(c) {
		case 'b':
		case 'B':
			usebatc = val;
			break;
		case 'a':
		case 'A':
			ccacheinh = val;
			dcacheinh = val;
			/* FALL THRU */
		case 'u':
		case 'U':
			ucacheinh = val;
			break;
		case 'd':
		case 'D':
			dcacheinh = val;
			break;
		case 'c':
		case 'C':
			ccacheinh = val;
			break;
		case 'p':
		case 'P':
			cachepolicy = val;
			break;
		case 't':
		case 'T':
			tstsnoop = val;
			cmmuinfo[2] |= 0x4000;	/* DATA CMMU SNOOP ONLY */
			break;
	}
}

/*
 * Check the data starting at 'adx' for 'len' bytes to see
 * if it checksums correctly. Return 1 if ok else return 0.
 */
chksumblk(adx, len)
register int *adx;
register int len;
{
	register int total;

	total = 0;
	len /= sizeof(int);
	while ((len--) > 0) {
		total += *adx++;
	}
	return(total == BLKCHKSUM);
}

/*
 * Do run time configuration.
 */
confinit()
{
	extern struct confinfo confinfo;
	extern struct confoverride confoverride;
	extern dev_t dumpdev;
	extern char hostname[];

	if (confoverride.flags & OVNODENAME) {
		bcopy(confoverride.nodename, utsname.nodename,
		      sizeof(utsname.nodename));
		bcopy(confoverride.nodename, hostname,
		      strlen(confoverride.nodename));
	} else {
		bcopy(confinfo.nodename, utsname.nodename,
		      sizeof(utsname.nodename));
		bcopy(confinfo.nodename, hostname,
		      strlen(confinfo.nodename));
	}
	dumpdev = confoverride.flags & OVDUMPDEV ?
		  confoverride.dumpdev : confinfo.dumpdev;
	pipedev = confoverride.flags & OVPIPEDEV ?
		  confoverride.pipedev : confinfo.pipedev;
	rootdev = confoverride.flags & OVROOTDEV ?
		  confoverride.rootdev : confinfo.rootdev;
	swapdev = confoverride.flags & OVSWAPDEV ?
		  confoverride.swapdev : confinfo.swapdev;
	swplo = confoverride.flags & OVSWPLO ?
		confoverride.swplo : confinfo.swplo;
	nswap = confoverride.flags & OVNSWAP ?
		confoverride.nswap : confinfo.nswap;

#if !m88k && !WS
	v.v_buf = confoverride.flags & OVNBUF ?
		  confoverride.nbuf : confinfo.nbuf;
#endif
}

/*
 * 'str' contains a number in ascii. The number is, by default, in base 10.
 * if '0x' or '0X' preceedes the number then it is in base 16 and 'A' - 'F'.
 * or 'a' - 'f' are used for the extra digits.
 * if a '0' precedes the number then it is in base 8.
 * Any character which is not '0' - '9', 'a' - 'f', or 'A' - 'F' is treated
 * as the end of the number string.
 */
asciitoint(str)
char *str;
{
	int val;
	int sign;
	int base;

	val = 0;
	sign = 1;
	base = 10;
	while(*str == ' ') {
		++str;
	}
	if (*str == '-') {
		++str;
		sign = -1;
	} else if (*str == '+') {
		++str;
	}
	if(*str == '0') {
		++str;
		if (*str == 'x' || *str == 'X') {
			++str;
			base = 16;
		} else if (*str == 'd' || *str == 'D') {
			++str;
			base = 10;
		} else {
			base = 8;
		}
	} else {
		base = 10;
	}

	while(1) {
		if (*str >= '0' && *str <= '9') {
			val = (base * val) + *str - '0';
		} else if (*str >= 'a' && *str <= 'f') {
			val = (base * val) + *str - 'a' + 10;
		} else if (*str >= 'A' && *str <= 'F') {
			val = (base * val) + *str - 'A' + 10;
		} else {
			break;
		}
		++str;
	}
	return(sign * val);
}

setargv (argbuf,argv,argmax)
register char *argbuf;
register char *argv[];
register int argmax;
{
	register int i;

	for (i=0; i<argmax; i++) {
		while (*argbuf == ' ' || *argbuf == '\t')
			argbuf++;
		if (*argbuf == 0)
			break;
		argv[i] = argbuf;
		while (*argbuf != ' ' && *argbuf != '\t' && *argbuf)
			argbuf++;
		if (*argbuf == 0)
			break;
		*argbuf++ = 0;
	}
	return(++i);
}

#define	UMAPSIZE	16

#if UMAPSIZE
struct umap {
	int		pid;
	caddr_t		uaddr;
	caddr_t		dmaddr;
	unsigned	len;
} umap[UMAPSIZE+1];


usermap(uaddr, len)
caddr_t	uaddr;
unsigned len;
{
	register struct umap *up;
	register caddr_t addr;
	
	up = umap;
	up[UMAPSIZE].pid = 0;
	while (up->pid)
		up++;

	if (up == &umap[UMAPSIZE]) {
		u.u_error = ENOMEM;
		return;
	}

	u.u_base = uaddr;
	u.u_count = len;
	if (addr = (caddr_t) dmamapuser(B_READ)) {
		up->pid = u.u_procp->p_pid;
		up->uaddr = uaddr;
		up->dmaddr = addr;
		u.u_rval1 = (int) addr;
		up->len = len;
	}
	else if (u.u_error == 0)
		u.u_error = ENOMEM;
}

userunmap(uaddr, dmaddr, len)
register caddr_t uaddr, dmaddr;
unsigned len;
{
	register struct umap *up;
	int pages, dma;
	
	up = umap;
	up[UMAPSIZE].uaddr = uaddr;
	up[UMAPSIZE].dmaddr = dmaddr;
	up[UMAPSIZE].len = len;
	while (up->dmaddr != dmaddr || up->uaddr != uaddr ||
		(len && len != up->len))
			++up;

	if (up == &umap[UMAPSIZE]) {
		u.u_error = ENXIO;
		return;
	}

	if (up->pid == u.u_procp->p_pid) {
		u.u_base = up->uaddr;
		u.u_count = up->len;
		dmaunmapuser(up->dmaddr, B_READ);
		up->pid = 0;
		up->dmaddr = 0;
		up->uaddr = 0;
		up->len = 0;
	}
	else
		u.u_error = EPERM;
}

unmappid()
{
	register struct umap *up;
	register pid;

	pid = u.u_procp->p_pid;

	for (up = umap; up < &umap[UMAPSIZE]; up++)
		if (up->pid == pid) {
			u.u_base = up->uaddr;
			u.u_count = up->len;
			dmaunmapuser(up->dmaddr, B_READ);
			up->pid = 0;
			up->dmaddr = 0;
			up->uaddr = 0;
			up->len = 0;
		}
#if XBus
	xbus_exit();
#endif
}
#endif

getmemsize()
{
	return(swapl(COMMPAGE0->mem_size));
}

cachecheck()
{
	register unsigned char *end, *beg;
D 7
	int opattern[4], ipattern[4];
E 7
I 7
	int opattern[4], ipattern[4], save[4];
E 7

	opattern[0] = 0x12345678;
	opattern[1] = 0x87654321;
	opattern[2] = 0x9abcdef0;
	opattern[3] = 0x0fedcba9;

	beg = (unsigned char *) 0x400000;
	end = (unsigned char *) getmemsize();
	for ( ; beg < end ; beg += 0x400000) {
I 7
		bcopy(beg, save, 16);
E 7
		bcopy(opattern, beg, 16);
		dcacheflush(0);
		bcopy(beg, ipattern, 16);
		if (ipattern[0] != 0x12345678 ||
			ipattern[1] != 0x87654321 ||
			ipattern[2] != 0x9abcdef0 ||
			ipattern[3] != 0x0fedcba9 ) {
				printf("mismatched expansion memory at 0x%x", 
								beg);
				panic("memory");
			}
I 7
		bcopy(save, beg, 16);
E 7
	}
}

freerbuf(tp)
register struct tty *tp;
{
	/* 
	   Should the receive buffer be released on the close ?
	   The code in tt1.c ttioctl (LDCLOSE) tests the following
		if (tp->t_rbuf.c_ptr && freerbuf(tp)) {
			putcf(CMATCH((struct cblock *)tp->t_rbuf.c_ptr));
			tp->t_rbuf.c_ptr = NULL;
		}
	*/
	/* Do not free because opmon still has an IORB for this request. */
	return 0;
}

usedelaytable()
{
	/*
	   Should the delay table in tty.c ttywait() be used?
	   The code in ttywait tests the following
		if (usedelaytable()) {
			delay(1+1+11*HZ/rate[tp->t_cflag&CBAUD]) ;
		}
		else {
			delay(HZ/15);
			(*tp->t_proc)(tp, T_DRAINOUT);
		}
	 */
	/* No. Opmon provides the delay */
	return 0;
}

#if m88kdebug
int cssp, ctp[4], cdp[4][4];
dumpset(addr)
register addr;
{
	register i, j, k;

	addr &= 0xff0;

	cssp = cdr(1, 0x880, addr);
	for ( i = 0, j = 0x840; i < 4 ; i++, j += 4 )
	{
		ctp[i] = cdr( 1, j, addr);
	}
	for ( k = 0; k < 4 ; k ++, addr += 4)  {
		for ( i = 0, j = 0x800; i < 4; i++, j += 4 ) {
			cdp[i][k] = cdr(1, j, addr);
		}
	}
	printf("&CSSP %x *CSSP %x\n", &cssp, cssp);
	for ( i = 0 ; i < 4 ; i ++ ) {
		printf("%x : %x %x %x %x\n", ctp[i], cdp[i][0], cdp[i][1],
				cdp[i][2], cdp[i][3]);
	}
}
#endif

#if m88k && XBus

/*	XBus support routines
**
**	xbus_ctl(cmd, signo)		sys_local() to set up signal connection
**	xbus_trap()			called from trap() to process XBus ints
**	xbus_exit()			called from exit() to clean up
*/

struct proc *xbus_p;
unsigned xbus_signo;

/*	xbus_ctl(cmd, signo)
**
**	request signal handling for XBus interrupt
**
**	cmd & 1:  request signal handling (cancel if signo == 0)
**	cmd & 2:  disable XBus interrupt
**	cmd & 4:  enable XBus interrupt
**
**	returns 1 if XBus interrupt pending; else 0
*/
void
xbus_ctl()
{
	register struct a {
		unsigned	cmd;
		unsigned	signo;
	} *uap;
	register s;

	uap = (struct a *)u.u_ap;

	if (xbus_p && xbus_p != u.u_procp) {
		u.u_error = EACCES;
		return;
	}

	if (uap->cmd > 5) {
		u.u_error = EINVAL;
		return;
	}

	s = spl7();

	if (uap->cmd & 1) {

		if (uap->signo >= NSIG || uap->signo && !_validsig(uap->signo)) {
			u.u_error = EINVAL;
			return;
		}

		if (uap->signo) {
			xbus_p = u.u_procp;
			xbus_signo = uap->signo;
		} else {
			xbus_signo = 0;
			xbus_p = 0;
		}
	}

	if (uap->cmd & 2)
		*CNTRL0 = X_ENB | CLR;
	else if (uap->cmd & 4)
		*CNTRL0 = X_ENB | SET;

	splx(s);
	u.u_rval1 = (*SR3 & CP_XIRQ) ? 1 : 0;
}

/*	called from trap handler
**	if XBus interrupt is active, disable it.
**	if signaling is requested, send signal
*/
void
xbus_trap()
{
	if (*SR3 & CP_XIRQ) {
		*CNTRL0 = X_ENB | CLR;
		if (xbus_p && xbus_signo)
			psignal(xbus_p, xbus_signo);
	}
}

/*	called from exit logic
**	if process requesting XBus signal is exiting, cancel it
*/
xbus_exit()
{
	if (u.u_procp == xbus_p) {
		xbus_p = 0;
		xbus_signo = 0;
	}
}
#endif

vendorbanner()
{
	printf("Copyright (c) 1989 Opus Systems\n");
	printf("	All Rights Reserved\n\n");
I 2
}

initlevel()
{
	u.u_rval1 = defaultlevel;
	defaultlevel = -1;	/* only the first time */
E 2
}
I 8

vendorfunc(vendor, func)
{
	/*	sample usage
	register struct a {
		long arg0, arg1, arg2, arg3, arg4, arg5;
	} *uap;
	uap = (struct a *)u.u_ap;

	if (vendor == YOURVENDORID) {
		if (func >= YOURMINFUNC && func < YOURMAXFUNC) 
			call_your_func(uap->arg0, uap->arg1, .... );
	}
	otherwise ..
	*/
	u.u_error = ENOSYS;
}
E 8
E 1
