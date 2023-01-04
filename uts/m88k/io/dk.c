/* SID @(#)dk.c	1.8 */

/*
 * PC Hard Disk Driver
 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/buf.h"
#include "sys/elog.h"
#include "sys/iobuf.h"
#include "sys/dev32k.h"
#include "sys/pc32k.h"
#include "sys/systm.h"
#include "sys/cb.h"
#if SVR3
#include "sys/immu.h"
#else
#include "sys/page.h"
#endif
#include "sys/compat.h"
#include "sys/sysconf.h"
#include "sys/dkcomm.h"
#include "sys/debug.h"
#include "sys/tuneable.h"

#define	physbp(bp)	(bp->b_flags & (B_PHYS|B_KPHYS))

#define NSECT 8  /* number of sections per drive */
#define MSECT 0x7  /* mask for section part */
#define SDRIVE 3  /* start of drive field */
#define DEFSECTSIZE 4096  /* default spacing for section starting blk number */
#define SECTORSIZE 512  /* number of bytes in a sector */
#define SECTORMASK 0x1ff  /* mask for byte in sector */
#define SECTORSHIFT 9  /* shift to get sector number from a byte count */
#define MDRIVENUM 0xf  /* mask for drive number field in disk status request */
#define FNAMESIZE 128  /* max length of the dos file name for a drive */

/* flags */

#define AVAIL	1  /* drive is available for use */
#define CLOSING 2  /* drive is in the process of being closed */
#define WCLOSE	4  /* an open for the drive is waiting for close completion */

/* command codes for dkcmd */

#define DRIVEOPEN 0  /* open a drive */
#define DRIVECLOSE 1  /* close a drive */
#define DRIVESTATUS 2  /* get status for a drive */
#define DRIVEGETFNAME 3  /* get the dos file name for a drive */
#define DRIVESETFNAME 4  /* set the dos file name for a drive */

/*
	At start up time the system sets aside a 4k window in the DMA address
	space. This page (usually at address DMASPACE+0x1000) is used for all
	block disk i/o.

	There are two reasons for this ..
	1. The mapping required is simpler and faster.
	2. File systems are unlikely to get thrashed  when new device
	   drivers are being developed.
	
	When this driver is called from physio, the dmamapbuf & dmaunmapbuf
	routines are used.

	The variables below are initialized at startup.
*/


#if SVR3
#if pm100
extern pde_t *dkmappde;	/* pointer to pte for block disk io page */
extern long dkadx;		/* KVA of block disk io page */

#define dkmapbuf(bp)	 \
	{	\
	*dkmappde = kptbl[pnum(paddr(bp))] ; \
	invsatb(SYSATB, dkadx); \
	}

#define dkunmapbuf(bp)	\
	{	\
	dkmappde->pgi.pg_pde = (uint) zeropage; \
	dkmappde->pgi.pg_pde2 = (uint) zeropage; \
	invsatb(SYSATB, dkadx); \
	}
#endif /* pm100 */
#if pm200
extern	int	dkadx;
extern	pde_t	*dkpte;
dkmapbuf(bp)
register struct buf *bp;
{
	register p;
	p = (int) paddr(bp);
	*dkpte = kptbl[pnum(p)];
	return((dkadx&~POFFMASK)|poff(p));
}
#define	dkunmapbuf(bp)
#endif /* pm200 */

#if m88k
extern	int	dkadx;
extern	pde_t	*dkpte;
struct buf dkbuf, *dkcoalesce(); 
static struct buf *incompletebp, *iobp;
caddr_t dkpage;
#define	DKPGS		4
#define	MAXDKPGS	8
int ndkpgs;
#define	GRAN		1024
#define	GRANSHFT	10
struct cachestat 
{
	unsigned reads, rhits, writes, whits, 
	rsize[1+MAXDKPGS*(NBPP/GRAN)],
	wsize[1+MAXDKPGS*(NBPP/GRAN)];
} cachestat;

dkmapbuf(bp)
register struct buf *bp;
{
	register p, i, maxi;
	register pde_t *kp;


	if (bp->b_rcount > ctob(ndkpgs)) {
		printf("cannot map %d bytes, bp = 0x%x\n", bp->b_rcount, bp);
		dkpanic("dkmapbuf");
	}

	p = (int) paddr(bp);
	maxi = btoc(bp->b_rcount);

	for (i = 0; i < maxi ; i++, p += ctob(1)) {
		kp = kvtopde(p);
		dkpte[i] = *kp;

		/* 
		 * if page is not cache inhibited,
		 * force it to be cache inhibited
		 */
		if (!pg_tstci(kp)) {
			pg_setci(dkpte);	/* will flush cache also */
		}
	}

	return(dkadx|poff(paddr(bp)));
}
#define	dkunmapbuf(bp)
#endif /* m88k */

#else /* 5.2 */

extern spte_t *dkmappde;	/* pointer to pte for block disk io page */
extern long dkadx;		/* KVA of block disk io page */
extern int dmapteprot;		/* unmapped state */

#define dkmapbuf(bp) \
	{	\
	*dkmappde = kpte[svtoc(paddr(bp))];	\
	invsatb(SYSATB, dkadx);	\
	}

#define dkunmapbuf(bp) \
	{	\
	dkmappde->pgi.pg_pte = dmapteprot; \
	dkmappde->pgi.pgx_pte = dmapteprot; \
	invsatb(SYSATB, dkadx);	\
	}

#endif /* SVR3 */

struct iotime dkstat;

struct iobuf dktab = tabinit(DK0, &dkstat.ios);
struct iobuf dktab1;

struct dk_info {
	int flags;  /* state flags */
	unsigned short dix;  /* device index for this drive */
	unsigned short inuse;  /* 1 bit for each section of blk and raw */
	struct iorb *cb;  /* control block for data transfers */
	struct logvol logvol[NSECT];  /* section info */
	int nspare;  /* number of active spares */
	struct spare *spare;  /* spare table */
	int sparesize;  /* size (in clicks) of the spare table */
	daddr_t	sparetbl;
} dk_info[DKMAX];

int dkprint();

dkopenb(dev)
minor_t dev;
{
	dkopencom(dev, 0);
}

dkopenc(dev)
minor_t dev;
{
	dkopencom(dev, 1);
}

dkopencom(dev, bc)
{
	register int drive;
	register int sect;
	register struct dk_info *cp;
	register int s;

	drive = dev >> SDRIVE;
	sect = dev & MSECT;
	cp = &dk_info[drive];
	if (drive >= DKMAX || (cp->flags & AVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	s = spl7();
	if (cp->inuse == 0) {
		while (cp->flags & CLOSING) {
			cp->flags |= WCLOSE;
			sleep((caddr_t)cp, PRIBIO);
		}
		cp->flags &= ~WCLOSE;
		if (!dkconnect(drive, 0)) {
			dkdisconnect(drive);
			u.u_error = ENXIO;
			splx(s);
			return;
		}
	}
	cp->inuse |= 1 << (sect + (NSECT * bc));
	splx(s);
}

dkcloseb(dev)
minor_t dev;
{
	if (minor(pipedev) != dev &&
	    minor(rootdev) != dev &&
	    minor(swapdev) != dev) {
		dkclosecom(dev, 0);
	}
}

dkclosec(dev)
minor_t dev;
{
	dkclosecom(dev, 1);
}

dkclosecom(dev, bc)
{
	register int drive;
	register int sect;
	register struct dk_info *cp;
	register int s;

	drive = dev >> SDRIVE;
	sect = dev & MSECT;
	cp = &dk_info[drive];
	s = spl7();
	cp->inuse &= ~(1 << (sect + (NSECT * bc)));
	if (cp->inuse == 0) {
		while (!dkqempty(dev)) {
			cp->flags |= CLOSING;
			sleep((caddr_t)cp, PRIBIO);
		}
		cp->flags &= ~CLOSING;
		if (cp->flags & WCLOSE) {
			wakeup((caddr_t)cp);
		}
		dkdisconnect(drive);
	}
	splx(s);
}

dkstrategy(bp)
register struct buf *bp;
{
	register int sect;
	register struct dk_info *cp;

	cp = &dk_info[minor(bp->b_dev)>>SDRIVE];
	sect = minor(bp->b_dev)&MSECT;
	bp->b_rblkno = bp->b_blkno + cp->logvol[sect].start;
	if (cp->inuse == 0 ||
	    bp->b_blkno < 0 ||
	    bp->b_blkno >= cp->logvol[sect].len ||
	    ((bp->b_dev & MSECT)!=(NSECT-1) && bp->b_rblkno>=cp->sparetbl)) {
		/* error if write; eof if read */
		if (!(bp->b_flags & B_READ)) {
			bp->b_flags |= B_ERROR;
			bp->b_error = ENXIO;
			printf("WARNING: write bad disk address: dev=%d,%d, blkno=%d (%d+%d)\n",
				major(bp->b_dev), minor(bp->b_dev),
				bp->b_rblkno, cp->logvol[sect].start, bp->b_blkno);
		}
		bp->b_resid = bp->b_bcount;
		iodone(bp);
		return;
	}
	if ( physbp(bp) )
		bp->b_raddr = dmamapbuf(bp);
	else
		bp->b_raddr = NULL;
	bp->b_rcount = bp->b_bcount;
	bp->av_forw = NULL;
	dkstat.io_cnt++;
	dkstat.io_bcnt += BTOC(bp->b_bcount);
	bp->b_start = lbolt; 
	sect = spl7();
	if ((bp->b_flags & B_LOPRI) && (kdebug & (0x400|0x800)) == 0xc00) {
		bp->b_flags &= ~B_LOPRI;
		bp->b_flags |= B_LOPRIQ;
		dksort(&dktab1, bp);
	}
	else
	{
		dksort(&dktab, bp);
	}
	if (dktab.b_active == 0 && (bp->b_flags & B_WAIT) == 0) {
		dkstart();
	}
	bp->b_flags &= ~B_WAIT;
	splx(sect);
}

dkstart()
{
	register struct iorb *IORB;
	register struct buf *bp;
	register int drive, sect;
	register int len;
	int blk, bblk, nspare, nblk;
	int lo, mid, hi;
	char done;
	struct spare *spare;
	struct dk_info *cp;

	if ((bp = incompletebp) == NULL) {
		if ((bp = dkcoalesce()) == NULL) {
			return;
		}
	} else {
		incompletebp = NULL;
	}
	if (dktab.b_active == 0) {
		dktab.io_start = lbolt;
	}
	drive = minor(bp->b_dev)>>SDRIVE;
	sect = minor(bp->b_dev)&MSECT;
	cp = &dk_info[drive];
	dktab.b_active = 1;
	blkacty |= (1 << DK0);

	/* check for spared blocks */

	nspare = cp->nspare;
	spare = cp->spare;
	blk = bp->b_rblkno;
	len = bp->b_rcount;
	nblk = (len + SECTORMASK) >> SECTORSHIFT;
	if (nspare > 0 &&
	    sect < (NSECT - 1) &&
	    (blk + nblk) > spare[0].badblk &&
	    blk <= spare[nspare - 1].badblk) {

		/* search bad block table for nearest bad blk */

		lo = 0;
		hi = nspare - 1;
		while (1) {
			mid = (lo + hi) >> 1;
			done = (hi - lo) <= 1;
			bblk = spare[mid].badblk;
			if (bblk < blk) {
				lo = mid;
			} else if (bblk == mid) {
				lo = mid;
				break;
			} else {
				hi = mid;
			}
			if (done) {
				break;
			}
		}

		/* check for bad blk in request and split if necessary */

		spare += lo;
		if (spare[0].badblk == blk) {
			blk = spare[0].spareblk;
			len = min(len, SECTORSIZE);
		} else if (spare[1].badblk == blk) {
			blk = spare[1].spareblk;
			len = min(len, SECTORSIZE);
		} else {
			bblk = spare[0].badblk;
			if (bblk > blk && bblk < (blk + nblk)) {
				len = (bblk - blk) * SECTORSIZE;
			} else {
				bblk = spare[1].badblk;
				if (bblk > blk && bblk < (blk + nblk)) {
					len = (bblk - blk) * SECTORSIZE;
				}
			}
		}
	}
	IORB = cp->cb; 
	if ( bp->b_raddr == NULL ) {
#if pm100
		dkmapbuf(bp);
		bp->b_raddr = dkadx;
#endif
#if pm200 || m88k
		bp->b_raddr = dkmapbuf(bp);
#endif
	}
	IORB->IBUF = bp->b_raddr;
	IORB->io_bcount = len;
	IORB->io_blknum = blk;
	IORB->a = (long) bp;
#if pm100
	IORB->io_hblknum = blk >> 16; /* hi order bits of blk adx for DOS */
#endif
	IORB->io_cmd = bp->b_flags & B_READ ? PC_READ : PC_WRITE;
	IORB->io_status = S_GO;
#if pm100
	if (IORB->io_memaddr < 0x200 ||
	    (IORB->io_memaddr+IORB->io_bcount) > 0xf000) {
		panic("dkstart");
	}
#endif
	iobp = bp;
	pcstrategy(IORB);
}

dkintr(IORB)
register struct iorb *IORB;
{
	register struct buf *bp;
	register struct dk_info *cp;
	register misc;
	
	cp = &dk_info[indtab[IORB->io_index]];

	bp = (struct buf *) IORB->a;
	if (bp == NULL) {
		dkpanic("dkintr: NULL bp");
	}

	blkacty &= ~(1 << DK0);
	misc = IORB->io_error;
	if (misc > E_GOOD) {
		bp->b_flags |= B_ERROR;
		dktab.io_addr = (PADDR)IORB;
		dktab.io_nreg = sizeof(struct iorb) / sizeof(short);
		fmtberr(&dktab, 0);
		if (misc != E_DA)
			prcom(dkprint, bp, misc, IORB->io_devstat);
	}
	misc = IORB->io_bcount;
	bp->b_raddr += misc;
	bp->b_rcount -= misc;
	bp->b_rblkno += (misc + SECTORMASK) >> SECTORSHIFT;
	if ((bp->b_rcount == 0) ||
	    (misc == 0) ||
	    (bp->b_flags & B_ERROR)) {
		if (dktab.io_erec) {
			logberr(&dktab, bp->b_flags & B_ERROR);
		}
		dktab.b_actf = bp->av_forw;
		dktab.b_active = 0;
		bp->b_resid = bp->b_rcount;
		if ( physbp(bp) )
			dmaunmapbuf(bp);
		else
			dkunmapbuf(bp);
		bp->b_raddr = NULL;
		dkstat.io_resp += lbolt - bp->b_start;
		dkstat.io_act += lbolt - dktab.io_start;
		if (bp != &dkbuf) {
			iodone(bp);
		}
		if (cp->flags & CLOSING) {
			wakeup((caddr_t)cp);
		}
		bp = NULL;
	}
	incompletebp = bp;
	iobp = NULL;
	dkstart();
}

dkread(dev)
minor_t dev;
{
	physio(dkstrategy,0,dev,B_READ);
}

dkwrite(dev)
minor_t dev;
{
	physio(dkstrategy,0,dev,B_WRITE);
}

/*
 * Reads 'bcount' bytes starting at 'blknum' into virtual address 'memaddr'.
 * The command is done using waited i/o. Returns the number of bytes read
 * or -1 if there was an error.
 */
dklread(drive, blknum, bcount, memaddr)
{
	struct iorb IORB;
	int adx;
	int err;

	adx = mapdma(memaddr, bcount);
	IORB.io_index = dk_info[drive].dix;
	IORB.io_cmd = PC_READ;
	IORB.IBUF = adx;
	IORB.io_bcount = bcount;
	IORB.io_blknum = blknum;
#if pm100
	IORB.io_hblknum = blknum >> 16;
#endif
	IORB.io_status = S_GO;
	IORB.io_error = E_OK;
	pcwait(&IORB, sizeof(struct iorb));
	relsdma(adx, bcount);
	err = IORB.io_error;
	if (err > E_GOOD) {
		return(-1);
	}
	return(IORB.io_bcount);
}

/*
 * Do command 'cmd' to the drive connected to 'dix'
 * using waited i/o.
 */
dklcmd(dix, cmd, parm, status)
int dix;
int cmd;
int parm;
int *status;
{
	struct iorb IORB;

	switch (cmd) {

		case DRIVEOPEN:
		IORB.io_cmd = IOCTL;
		IORB.IVAL = 1;
		break;

		case DRIVECLOSE:
		IORB.io_cmd = IOCTL;
		IORB.IVAL = 2;
		break;

		case DRIVEGETFNAME:
		IORB.io_cmd = IOCTL;
		IORB.IVAL = 3;
		break;

		case DRIVESETFNAME:
		IORB.io_cmd = IOCTL;
		IORB.IVAL = 4;
		break;

		case DRIVESTATUS:
		IORB.io_cmd = PC_STAT;
		break;

		default:
		return(-1);
		break;
	}
	IORB.io_index = dix;
	IORB.io_bcount = parm;
	IORB.io_status = S_GO;
	IORB.io_error = E_OK;
	pcwait(&IORB, sizeof(struct iorb));
	if (status != NULL) {
		*status = IORB.io_devstat;
	}
	return(IORB.io_error);
}

/*
 * Print out an error report.
 */
dkprint(dev,str)
int dev;
char *str;
{
	printf("%s on DK drive %d, section %d\n", str,
	       (dev>>SDRIVE) & MDRIVENUM, dev&MSECT);
}

/*
 * Initialize driver state.
 */
dkinit()
{
	register dix;
	register int drive;
	register int blk0dix;
	register struct dk_info *cp;
	int status;
	extern struct confoverride confoverride;

	if (confoverride.flags & OVBLK0DIX) {
		blk0dix = confoverride.blk0dix;
	} else {
		blk0dix = -1;
	}
	DIXLOOP(dix) {
		if (DIX(dix) == DISK) {
			dklcmd(dix, DRIVESTATUS, 0, &status);
			drive = status & MDRIVENUM;
			drive = dkselectdrive(drive == 0 ? -1 : drive);
			if (drive == -1) {
				break;  /* drive number in use */
			}
			indtab[dix] = drive;
			cp = &dk_info[drive];
			cp->cb = (struct iorb *)
				dmastatic(sizeof(struct iorb));
			cp->dix = cp->cb->io_index = dix;
			cp->flags |= AVAIL;
			if (blk0dix == -1 && drive == 0) {
				blk0dix = dix;
			}
			if (dix == blk0dix) {
				dkconnect(drive, 1);
				dkdisconnect(drive);
			}
		}
	}
}

/*
 * Get the drive number for a dix. 'dn' is the initial guess at
 * the drive number. 'dn' should be set to -1 to select a default
 * drive number. The result is -1 if  'dn' has already been used
 * or if a default drive number was requested and there are none left.
 */
dkselectdrive(dn)
register int dn;
{
	register struct dk_info *d;
	register int i;

	if (dn == -1) {
		/* check through all drive numbers and return first free */
		for (i = 0; i < DKMAX; i++) {
			if ((dn = dkselectdrive(i)) != -1) {
				break;
			}
		}
	} else {
		for (d = dk_info; d < &dk_info[DKMAX]; d++) {
			if (d->flags & AVAIL) {
				if (indtab[d->dix] == dn) {
					dn = -1;
					break;
				}
			}
		}
	}
	return(dn);
}

/*
 * Obtain disk resident information.
 * Returns 1 if initialization completed sucessfully.
 * Returns 0 if unable to initialize unit.
 */
dkconnect(drive, blk0)
int drive;
int blk0;
{
	struct confinfo *uc;
	struct spareinfo *us;
	register int i, j;
	register int sect;
	int sparetblptr;
	int spareused;
	register struct spare *sp;
	register struct dk_info *cp;
	extern struct confinfo confinfo;
	extern struct confoverride confoverride;

	uc = (struct confinfo *)SPTALLOC(1);
	us = (struct spareinfo *)uc;  /* share memory */
	if (uc == 0) {
		return(0);
	}
	cp = &dk_info[drive];
	if (dklcmd(cp->dix, DRIVEOPEN, 0, NULL) > E_GOOD) {
		sptfree(uc, 1, 1);
		return(0);
	}
	dklread(drive, 0, sizeof(*uc), uc);
	if (chksumblk(uc, sizeof(*uc))) {
		for (sect = 0; sect < NSECT; sect++) {
			bcopy(&uc->logvol[sect], &cp->logvol[sect], sizeof(struct logvol));
		}
		sparetblptr = uc->sparetbl;
		spareused = uc->used;
		cp->sparetbl = uc->sparetbl;
		if (cp->sparetbl == NULL)
			cp->sparetbl = 0x7fffffff;
		if (blk0) {
			confinfo = *uc;  /* set up conf info */
		}
	} else {
		for (sect = 0; sect < (NSECT-1); sect++) {
			cp->logvol[sect].start = sect*DEFSECTSIZE;
			cp->logvol[sect].len = -1;  /* infinite */
		}
		cp->logvol[(NSECT-1)].start = 0;
		cp->logvol[(NSECT-1)].len = -1;  /* infinite */
		sparetblptr = NULL;
		spareused = 0;
		cp->sparetbl = 0x7fffffff;
		if (blk0) {
			dkpanic("invalid block 0");
		}
	}
	
	/* override section definition if needed */

	if (confoverride.flags & OVSECTDEF && confoverride.drive == drive) {
		cp->logvol[confoverride.sect].start = confoverride.sectstart;
		cp->logvol[confoverride.sect].len = confoverride.sectlen;
	}

	/* read in spares table */

	if (spareused > 0) {
		cp->sparesize = BTOC(sizeof(struct spare) * spareused);
		cp->spare = (struct spare *)
			    SPTALLOC(cp->sparesize);
		if (cp->spare == NULL) {
			goto nospares;
		}
		us->link = sparetblptr;
		i = NUMSPARE;
		sp = cp->spare;
		cp->nspare = 0;
		while (cp->nspare < spareused) {
			if (i >= NUMSPARE) {
				if (us->link == NULL) {
					break;
				}
				if (dklread(drive, us->link, sizeof(*us), us) != sizeof(*us) || !chksumblk(us, sizeof(*us))) {
					goto nospares;
				}
				i = 0;
			}
			if (us->spare[i].badblk == NULLBLKPTR ||
			    us->spare[i].badblk == 0) {
				break;
			}
			bcopy(&us->spare[i], sp++, sizeof(struct spare));
			i++;
			cp->nspare++;
		}
	} else {

		nospares:
		if (cp->spare != NULL) {
			sptfree(cp->spare, cp->sparesize, 1);
		}
		cp->sparesize = 0;
		cp->nspare = 0;
		cp->spare = NULL;
	}
	sptfree(uc, 1, 1);
	return(1);
}

/*
 * Give back resources used while a drive is connected.
 */
dkdisconnect(drive)
int drive;
{
	register struct dk_info *cp;

	cp = &dk_info[drive];
	dklcmd(cp->dix, DRIVECLOSE, 0, NULL);
	if (cp->spare != NULL) {
		sptfree(cp->spare, cp->sparesize, 1);
	}
}

/*
 * Execute an ioctl.
 */
dkioctl(dev, cmd, arg, mode)
minor_t dev;
{
	register struct dk_info *cp;
	struct diskcommand *comm;
	int dmaadx;
	int err;
	int status;
	int i;

	if (cmd == 0xf00) {
		dumpcachestats(arg);
		return;
	}
	comm = (struct diskcommand *)SPTALLOC(BTOC(sizeof(*comm)));
	if (comm == 0) {
		u.u_error = ENOMEM;
		goto out;
	}
	if (copyin(arg, (caddr_t)comm, sizeof(*comm)) < 0) {
		u.u_error = EFAULT;
		goto out;
	}
	cp = &dk_info[comm->drive];
	if ((unsigned) comm->drive >= DKMAX || (cp->flags & AVAIL) == 0) {
		u.u_error = ENXIO;
		goto out;
	}
	switch (cmd) {

		case DKGETFNAME:
		dmaadx = mapdma(comm->name, FNAMESIZE);
		err = dklcmd(cp->dix, DRIVEGETFNAME, dmaadx, &status);
		relsdma(dmaadx, FNAMESIZE);
		if (err > E_GOOD) {
			u.u_error = ENXIO;
			goto out;
		}
		break;

		case DKSETFNAME:
		dmaadx = mapdma(comm->name, FNAMESIZE);
		err = dklcmd(cp->dix, DRIVESETFNAME, dmaadx, &status);
		relsdma(dmaadx, FNAMESIZE);
		if (err > E_GOOD) {
			u.u_error = ENXIO;
			goto out;
		}
		break;

		case DKGETFPARM:
		err = dklcmd(cp->dix, DRIVESTATUS, 0, &comm->parm);
		if (err > E_GOOD) {
			u.u_error = ENXIO;
			goto out;
		}
		break;

		case DKDRAINDISK:
		for (i = 0; i < DKMAX; i++) {
			while (!dkqempty(i << SDRIVE)) {
			}
		}
		break;

		default:
		u.u_error = ENXIO;
		goto out;
		break;	
	}
	if (copyout((caddr_t)comm, arg, sizeof(*comm)) < 0) {
		u.u_error = EFAULT;
	}

	out:
	if (comm) {
		sptfree(comm, BTOC(sizeof(*comm)), 1);
	}
	return;
}

dkqempty(dev)
int dev;
{
	register struct buf *bp;
	register int drive;
	
	drive = dev >> SDRIVE;

	bp = dktab.b_actf;
	while (bp) {
		if ((minor(bp->b_dev) >> SDRIVE) == drive) {
			return(0);
		}
		bp = bp->av_forw;
	}

	bp = dktab1.b_actf;
	while (bp) {
		if ((minor(bp->b_dev) >> SDRIVE) == drive) {
			return(0);
		}
		bp = bp->av_forw;
	}
	return(1);
}

/*
 * Seek sort for disks.
 *
 * The argument iobuf structure holds a b_actf activity chain pointer
 * on which we keep two queues, sorted in ascending cylinder order.
 * The first queue holds those requests which are positioned after
 * the current block (in the first request); the second holds
 * requests which came in after their block number was passed.
 * Thus we implement a one way scan, retracting after reaching the
 * end of the drive to the first request on the second queue,
 * at which time it becomes the first queue.
 *
 * A one-way scan is natural because of the way UNIX read-ahead
 * blocks are allocated.
 * 
 * Slightly modified 4.3 BSD routine.
 */

static
dksort(dp, bp)
register struct iobuf *dp;
register struct buf *bp;
{
	register struct buf *ap;
	register daddr_t blkno;

	/*
	 * If nothing on the activity queue, then
	 * we become the only thing.
	 */
	ap = dp->b_actf;
	if(ap == NULL) {
		dp->b_actf = bp;
		dp->b_actl = bp;
		bp->av_forw = NULL;
		return;
	}
	else if (!(kdebug & 0x400)) {
		dp->b_actl->av_forw = bp;
		dp->b_actl = bp;
		bp->av_forw = NULL;
		return;
	}

	blkno = bp->b_rblkno;

	/*
	 * If we lie before the first (currently active)
	 * request, then we must locate the second request list
	 * and add ourselves to it.
	 */
	if (blkno < ap->b_rblkno) {
		while (ap->av_forw) {
			/*
			 * Check for an ``inversion'' in the
			 * normally ascending block numbers,
			 * indicating the start of the second request list.
			 */
			if (ap->av_forw->b_rblkno < ap->b_rblkno) {
				/*
				 * Search the second request list
				 * for the first request at a larger
				 * block number.  We go before that;
				 * if there is no such request, we go at end.
				 */
				do {
					if (blkno < ap->av_forw->b_rblkno)
						goto insert;
					ap = ap->av_forw;
				} while (ap->av_forw);
				goto insert;		/* after last */
			}
			ap = ap->av_forw;
		}
		/*
		 * No inversions... we will go after the last, and
		 * be the first request in the second request list.
		 */
		goto insert;
	}
	/*
	 * Request is at/after the current request...
	 * sort in the first request list.
	 */
	while (ap->av_forw) {
		/*
		 * We want to go after the current request
		 * if there is an inversion after it (i.e. it is
		 * the end of the first request list), or if
		 * the next request is a larger block than our request.
		 */
		if (ap->av_forw->b_rblkno < ap->b_rblkno ||
		    blkno < ap->av_forw->b_rblkno)
			goto insert;
		ap = ap->av_forw;
	}
	/*
	 * Neither a second list nor a larger
	 * request... we go at the end of the first list,
	 * which is the same as the end of the whole schebang.
	 */
insert:
	bp->av_forw = ap->av_forw;
	ap->av_forw = bp;
	if (ap == dp->b_actl)
		dp->b_actl = bp;
}

dkbufalloc(physclick)
{

	register i;
	register pde_t *pde;

	if (!(kdebug & 0x400))
		ndkpgs = 1;
	else
		ndkpgs = swaps(COMMPAGE0->dkra);

	if (ndkpgs <= 0 || ndkpgs > MAXDKPGS)
		ndkpgs = DKPGS;

	/* Maximum number of block read aheads  */
	tune.t_maxrdahead = (ctob(ndkpgs)/1024) - 1;

	dkpage = (caddr_t) ctob(physclick);

	/* initialize bottom level page table entries */
	for (i = 0; i < ndkpgs; i++) {
		pde = kvtopde(dkpage + ctob(i));
		pg_setci(pde);
		pg_setpfn(pde, pnum(dkpage + ctob(i)));
		pg_setvalid(pde);
	}
	return (physclick + ndkpgs);
}

#define	readbp(bp)	(bp->b_flags & B_READ)
#define	writbp(bp)	((bp->b_flags & B_READ) == 0)
#define flushcache()	cachevalid = 0
#define	setcache(dev, blk, cnt) \
		cachevalid = 1;	cachedev = dev; cacheblk = blk; cachecnt = cnt;

#define	cacheaddr(bp)	(dkpage + ((bp->b_rblkno - cacheblk) << SCTRSHFT))

#define	bcopyl(src, dst, count)		bcopy16(src, dst, count)

static int cachevalid ;
static int cachedev;
static int cacheblk;
static int cachecnt;

struct buf *
dkcoalesce()
{
	register struct iobuf *dp;
	register struct buf *bp, *nbp, *sbp;
	register struct cachestat *cp;
	register int rcount, maxcnt;
	int scount;

	dp = &dktab;
	if (!(kdebug & 0x400))
		return dp->b_actf;
loop:
	if (dp->b_actf == NULL && dktab1.b_actf != NULL)
		dkpromotesome();
	if ((bp = dp->b_actf) == NULL)
		return NULL;
	if (physbp(bp))
		return bp;

	cp = &cachestat;
	maxcnt = ctob(ndkpgs);

	if (readbp(bp)) {
		while (bp && readbp(bp) && ! physbp(bp) && incache(bp)) {

			cp->rhits ++;
			bcopyl(cacheaddr(bp), paddr(bp), bp->b_rcount);
			bp->b_flags |= (dkbuf.b_flags & B_ERROR);
			bp->b_resid = 0;

			dp->b_actf = bp->av_forw;
			iodone(bp);
			bp = dp->b_actf;
		}
		flushcache();
		if (!bp || !readbp(bp) || physbp(bp))
			goto loop;

		sbp = bp;
		nbp = bp->av_forw;
		scount = rcount = bp->b_rcount;
		while (nbp && readbp(nbp) && ! physbp(nbp) && 
				bp->b_dev == nbp->b_dev && 
				((rcount + nbp->b_rcount) <= maxcnt)) {
			if ((bp->b_rblkno + (bp->b_rcount >> SCTRSHFT)) !=
			    nbp->b_rblkno)
				break;
			rcount += nbp->b_rcount;
			bp = nbp;
			nbp = nbp->av_forw;
		}
		cp->reads ++;
		cp->rsize[rcount >> GRANSHFT] ++;
		if (rcount != scount) {
			bp = &dkbuf;
			bcopy (sbp, bp, sizeof(struct buf));
			bp->b_un.b_addr = dkpage;
			bp->b_rcount = rcount;
			bp->b_bcount = rcount;
			bp->b_raddr = NULL;
			setcache(bp->b_dev, bp->b_rblkno, bp->b_rcount);
			/* insert dkbuf at the head of the disk request queue */
			bp->av_forw = dp->b_actf;
			dp->b_actf = bp;
			return (bp);
		}
		return(sbp);
	}
	else {
		flushcache();
		sbp = bp;
		nbp = bp->av_forw;
		scount = rcount = bp->b_rcount;
		while (nbp && writbp(nbp) && ! physbp(nbp) && 
				bp->b_dev == nbp->b_dev && 
				((rcount + nbp->b_rcount) <= maxcnt)) {
			if ((bp->b_rblkno + (bp->b_rcount >> SCTRSHFT)) !=
			    nbp->b_rblkno)
				break;
			rcount += nbp->b_rcount;
			if (bp == sbp) {
				bcopyl(paddr(bp), dkpage, bp->b_rcount);
				cacheblk = bp->b_rblkno;
				bp->b_resid = 0;
				cp->whits ++;
				dp->b_actf = nbp;
				iodone(bp);
			}
			bcopyl(paddr(nbp), cacheaddr(nbp), nbp->b_rcount);
			nbp->b_resid = 0;
			cp->whits ++;
			bp = nbp;
			dp->b_actf = nbp = nbp->av_forw;
			iodone(bp);
		}
		if (rcount != scount) {
			bp = &dkbuf;
			bcopy (sbp, bp, sizeof(struct buf));
			bp->b_un.b_addr = dkpage;
			bp->b_rcount = rcount;
			bp->b_bcount = rcount;
			bp->b_raddr = NULL;
			bp->b_flags = B_WRITE;
			/* insert bp at the head of the disk request queue */
			bp->av_forw = dp->b_actf;
			dp->b_actf = bp;
			if (bp->av_forw == NULL)
				dp->b_actl = bp;
		}
		cp->writes ++;
		cp->wsize[rcount >> GRANSHFT] ++;
		return bp;
	}
}

incache(bp)
register struct buf *bp;
{
	if (!cachevalid)
		return 0;

	if (bp->b_dev != cachedev)
		return 0;

	if (bp->b_rblkno >= cacheblk && 
		(bp->b_rblkno + (bp->b_rcount >> SCTRSHFT)) <=
			(cacheblk + (cachecnt >> SCTRSHFT)))
		return (1);
	return (0);
}

/* dump cache statistics and reset counters */
dumpcachestats(arg)
{
	register struct cachestat *cp;
	register unsigned i, total;

	cp = &cachestat;
	printf ("&cachestat=0x%x &dktab=0x%x &dktab1=0x%x\n",
		cp, &dktab, &dktab1);

	for (i = 0, total = 0; i < sizeof(cp->rsize)/sizeof(int) ; i++) {
		total += (cp->rsize[i] * (i << GRANSHFT));
	}
	if (cp->reads)
		printf("read stats: %d reads; %d hits; average size=%d\n", 
			cp->reads, cp->rhits, total / cp->reads);
	
	for (i = 0, total = 0; i < sizeof(cp->wsize)/sizeof(int) ; i++) {
		total += (cp->wsize[i] * (i << GRANSHFT));
	}
	if (cp->writes)
		printf("write stats: %d writes; %d hits; average size=%d\n", 
			cp->writes, cp->whits, total / cp->writes);

	if (arg)
		bzero(cp, sizeof(struct cachestat));
}

dkpromote(bp)				/* interrupts off */
register struct buf *bp;
{
	register struct buf *lbp;

	lbp = (struct buf *)&dktab1;
	while (lbp->av_forw && lbp->av_forw != bp)
		lbp = lbp->av_forw;

	lbp->av_forw = bp->av_forw;	/* remove from low pri queue */
	bp->b_flags &= ~B_LOPRIQ;
	bp->av_forw = NULL;
	dksort(&dktab, bp);		/* add to high pri queue */
}

dkpromoteall()				/* interrupts off */
{
	register struct buf *bp, *nbp;

	bp = dktab1.b_actf;
	while (bp) {
		nbp = bp->av_forw;
		dkpromote(bp);
		bp = nbp;
	}
}

dkpromotesome()		/* interrupts off; dktab1.b_actf != NULL */
{
	register struct buf *bp, *nbp;

	bp = dktab1.b_actf;
	nbp = bp->av_forw;
	dkpromote(bp);
	while (nbp && bp->b_dev == nbp->b_dev && 
		((bp->b_rblkno + (bp->b_rcount>>SCTRSHFT)) == nbp->b_rblkno)) {
		bp = nbp;
		nbp = bp->av_forw;
		dkpromote(bp);
	}
}

extern int noPanicSync;

/*VARARGS1*/
dkpanic(fmt, ARGS)
char *fmt;
int ARGS;
{
	register int i;

	noPanicSync = 1;	/* tell panic() not to sync */
	panic(fmt, ARGS);
}
