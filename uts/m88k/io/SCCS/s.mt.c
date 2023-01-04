h43446
s 00002/00001/00750
d D 1.2 90/03/14 15:22:23 root 2 1
c MTWEOF must check mt_count
e
s 00751/00000/00000
d D 1.1 90/03/06 12:28:51 root 1 0
c date and time created 90/03/06 12:28:51 by root
e
u
U
t
T
I 1
 /* SID @(#)mt.c	1.1 */

/*
 * PC mag tape driver
 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/buf.h"
#if SVR3
#include "sys/immu.h"
#else
#include "sys/page.h"
#endif
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/elog.h"
#include "sys/iobuf.h"
#include "sys/pc32k.h"
#include "sys/dev32k.h"
#include "sys/cb.h"
#include "sys/mtcomm.h"
#include "sys/mtio.h"
#include "sys/region.h"
#include "sys/compat.h"


static int	index;

struct	iotime	mtstat[MTMAX];
struct	iobuf	mttab = tabinit(MT0, 0);
#if SVR3
extern	pde_t *dmapte;  /* kernel virtual address of dma space map */
#define BTOCT	btoct
#else
extern	spte_t *dmapte;  /* kernel virtual address of dma space map */
#define BTOCT	btotp
#endif

/* misc */

#define	INF	1000000
#define	CART_BLK	512		/* cartridge block size */
#define min(a,b)	(((a) > (b)) ? (b) : (a))

/* flags fields */

#define	AVAIL	1	/* unit is available */
#define INUSE	2	/* device is in use */
#define CART	4	/* cart tape drive */
#define	RZERO	8
#define	WFULL	16
#define	IOCTLDONE	32

/* driver state */

#define SIO	1	/* doing a read or write */
#define SSFOR	2	/* forward space record */
#define SSREV	3	/* back space record */
#define SCOM	4	/* command */
#define SOK	5	/* ok */

struct mt_info {
	int flags;
	int dix;
	struct io_mt *cb;
	int eot;
	int blkno;
	int nxrec;
} mt_info[MTMAX];

extern mtprint();

mtopenb(dev, flag)
int dev;
int flag;
{
	
	mtopencom(dev, flag, 0);
}

mtopenc(dev, flag)
int dev;
int flag;
{
	
	mtopencom(dev, flag, 1);
}

mtopencom(dev, flag, type)
int dev;
int flag;
int type;
{
	register unit;
	register struct mt_info *cp;
	int ds;

	unit = dev & UNITMSK;
	cp = &mt_info[unit];
	if (unit >= MTMAX || (cp->flags & AVAIL) == 0 ||
	    ((cp->flags & CART) && type == 0)) {
		u.u_error = ENXIO;
		return;

	}
	if (cp->flags&INUSE) {
		u.u_error = EBUSY;
		return;
	}
	cp->blkno = 0;
	cp->nxrec = INF;
	mtcommand(dev, MTIOCTL_NOP, 0, &ds);
	if (ds & (S_TO|S_ERR)) {
		mtcommand(dev, MTIOCTL_RESET, 0, &ds);
		mtcommand(dev, MTIOCTL_NOP, 0, &ds);
	}

/*
 * The following code is in the standard tape drivers.
 * I think its best to catch these errors when a command is started
 * so that ioctl status calls can be made even when the tape
 * is offline
 */
#ifdef nothanks
	if ((ds & (S_OFFLINE|S_TO|S_ILL|S_ERR)) ||
	    ((flag & FWRITE) && (ds & S_WPROT))) {
		u.u_error = ENXIO;
		return;
	}
#endif
	if (flag & FAPPEND) {
		if (cp->flags & CART) {
			u.u_error = ENXIO;
			return;
		}
		mtcommand(dev, MTIOCTL_FSF, 0, &ds);
		mtcommand(dev, MTIOCTL_BSF, 0, &ds);
	}
	if ((cp->flags & CART) == 0) {
		mtcommand(dev, MTIOCTL_DEN,
			  (dev & DENMSK) ? BPI1600 : BPI800, &ds);
	}
	cp->flags |= INUSE;
}

mtclose(dev, flag)
int dev;
int flag;
{
	register unit;
	register struct mt_info *cp;
	int ds;

	unit = dev & UNITMSK;
	cp = &mt_info[unit];
	if (dev & REWMSK) {
		/* no-rewind device */
		if (flag & FWRITE) {
			mtcommand(dev, MTIOCTL_WFM, 0, &ds);
#if 0
			mtcommand(dev, MTIOCTL_WFM, 0, &ds);
			mtcommand(dev, MTIOCTL_BSF, 0, &ds);
#endif
		}
		else {
			if ((cp->flags & (IOCTLDONE|RZERO)) == 0)
				mtcommand(dev, MTIOCTL_FSF, 0, &ds);
		}
	} else {
		if (flag & FWRITE) {
			mtcommand(dev, MTIOCTL_WFM, 0, &ds);
			mtcommand(dev, MTIOCTL_WFM, 0, &ds);
/*
			if (((cp->flags & CART) == 0) || ((ds & S_EOT) == 0))
				mtcommand(dev, MTIOCTL_WFM, 0, &ds);
			else
				mtcommand(dev, MTIOCTL_RESET, 0, &ds);
*/
		}
		mtcommand(dev, MTIOCTL_REW, 0, &ds);
	}
	cp->flags &= ~(INUSE | RZERO | WFULL | IOCTLDONE);
}


mtstrategy(bp)
register struct buf *bp;
{
	register daddr_t *p;
	register unit;
	register struct mt_info *cp;

	unit = bp->b_dev & UNITMSK;
	cp = &mt_info[unit];

#if SVR3
	bp->b_rblkno = (daddr_t) vtopreg(u.u_procp, u.u_base);
	if ( bp->b_rblkno == NULL ) {
		bp->b_flags |= B_ERROR;
		bp->b_error = EFAULT;
		iodone(bp);
		return;
	}
#else
	bp->b_rblkno = (daddr_t) findreg(u.u_procp, u.u_base);
#endif

	index = 0;								/* start index at 0 */
	bp->b_resid = bp->b_bcount;		/* remaining count	*/

	p = &cp->nxrec;
	if (bp->b_blkno > *p) {
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		iodone(bp);
		return;
	}
	if (bp->b_blkno == *p && bp->b_flags&B_READ) {
		if ((bp->b_flags & B_PHYS) == 0)
			clrbuf(bp);
		bp->b_resid = bp->b_bcount;
		iodone(bp);
		return;
	}
	if ((bp->b_flags&B_READ)==0) {
		*p = bp->b_blkno + 1;
	}
	bp->b_start = lbolt;
	mtstat[unit].io_bcnt += BTOC(bp->b_bcount);
	if (cp->flags & CART)			/* only map 4 pages */
		bp->b_bcount = 4*NBPP;
	bp->b_raddr = dmamapbuf(bp);
	bp->b_bcount = bp->b_resid;		/* put back original count */
	if ( cp->flags & CART ) {
		mt_mapnext(bp);			/* map first two blocks */
		if (bp->b_bcount > CART_BLK)
			mt_mapnext(bp);					
	}

	bp->av_forw = NULL;
	spl5();
	if (mttab.b_actf == NULL) {
		mttab.b_actf = bp;
	} else {
		mttab.b_actl->av_forw = bp;
	}
	mttab.b_actl = bp;
	if (mttab.b_active==0) {
		mtstart();
	}
	spl0();
}

mtstart()
{
	register struct buf *bp;
	register unit;
	daddr_t blkno;
	register struct mt_info *cp;
	register struct io_mt *IORB;

	loop:
	if ((bp = mttab.b_actf) == NULL) {
		return;
	}
	unit = bp->b_dev & UNITMSK;
	cp = &mt_info[unit];
	blkno = cp->blkno;
	if (bp->b_blkno > cp->nxrec) {
		goto abort;
	}
	if (mttab.io_start == 0) {
		mttab.io_start = lbolt;
	}
	if (blkno == bp->b_blkno) {
		if (cp->eot > 8) {
			goto abort;
		}
		if ((bp->b_flags&B_READ)==0 && cp->eot && blkno > cp->eot) {
			bp->b_error = ENOSPC;
			goto abort;
		}
		mttab.b_active = SIO;
		mtstat[unit].io_cnt++;
		blkacty |= (1 << MT0);
		IORB = cp->cb; 
		IORB->IBUF = bp->b_raddr;
		IORB->io_bcount = bp->b_bcount;	/* store count as 16-bit field*/
		IORB->io_blknum = bp->b_bcount;	/* store count as 32-bit field*/
#if pm100
		IORB->io_hblknum = bp->b_bcount>>16;
#endif
		IORB->io_cmd = bp->b_flags & B_READ ? PC_READ : PC_WRITE;
		IORB->io_status = S_GO;
		pcstrategy(IORB);
	} else {
		mtstat[unit].ios.io_misc++;
		IORB = cp->cb; 
		if (blkno < bp->b_blkno) {
			mttab.b_active = SSFOR;
			IORB->io_blknum = MTIOCTL_FSR;
			IORB->IBUF = bp->b_blkno - blkno;
		} else {
			mttab.b_active = SSREV;
			IORB->io_blknum = MTIOCTL_BSR;
			IORB->IBUF = blkno - bp->b_blkno;
		}
		IORB->IBUF = blkno - bp->b_blkno;
#if pm100
		IORB->io_hblknum = 0;
#endif
		IORB->io_bcount = 0;
		IORB->io_cmd = IOCTL;
		IORB->io_status = S_GO;
		pcstrategy(IORB);
	}
	return;

	abort:
	bp->b_flags |= B_ERROR;

	next:
	mttab.b_actf = bp->av_forw;
	iodone(bp);
	goto loop;
}

mtintr(IORB, pc, ps)
register struct io_mt *IORB;
int pc;
int ps;
{
	register struct buf *bp;
	struct mt_info *cp;
	int i, unit, ds;
	int	ret_cnt, res_blk;

	if ((bp = mttab.b_actf) == NULL) {
		return;
	}

	unit = indtab[IORB->io_index];
	cp = &mt_info[unit];

	ds = IORB->io_devstat;
	if (IORB->io_error > E_CE) {
		ds |= S_ERR;
	}
	if (ds & (S_ILL|S_ERR|S_TO|S_EOF|S_OFFLINE|S_WPROT|S_EOT)) {
		if ((ds & S_EOF) || (ds & S_EOT) ||
			((ds & S_WPROT) && (IORB->io_cmd == PC_READ))) {
			cp->nxrec = bp->b_blkno;
			mttab.b_active = SOK;
		}
		if (mttab.b_active != SOK) {
			mttab.io_addr = (PADDR)IORB;
			mttab.io_nreg = sizeof(struct io_mt) / sizeof(short);
			fmtberr(&mttab, 0);
			prcom(mtprint, bp, IORB->io_error, IORB->io_devstat);
			bp->b_flags |= B_ERROR;
			mttab.b_active = SIO;
		}
	}
	switch (mttab.b_active) {

		case SIO:
		case SOK:
		case SCOM:

		if (cp->flags & CART) {		/* only for cartridge	*/
#if pm100
		  	ret_cnt = (IORB->io_blknum & 0xffff) + 
					(IORB->io_hblknum<<16);
#endif
#if pm200 || m88k
			ret_cnt = IORB->blk;
#endif
			if (ret_cnt > 0) {
				res_blk = min(bp->b_resid, CART_BLK);
		  		if ( ((bp->b_resid -= res_blk) > 0) &&
					(bp->b_bcount == ret_cnt) ) {
	
						if (bp->b_resid > CART_BLK)
							mt_mapnext( bp );

						IORB->io_status = S_GO;
						pcstrategy(IORB);
						return;
				}
				bp->b_resid = bp->b_bcount - ret_cnt;
			}
		}
		else
			bp->b_resid -= IORB->io_bcount;
done:
		if (ds & S_EOT) {
			cp->eot++;
		} else {
			cp->eot = 0;
		}
		if (mttab.io_erec) {
			logberr(&mttab, bp->b_flags & B_ERROR);
		}
		mttab.b_errcnt = 0;
		mttab.b_actf = bp->av_forw;
		mttab.b_active = 0;
		dmaunmapbuf(bp);
		iodone(bp);

		mtstat[unit].io_resp += lbolt - bp->b_start;
		mtstat[unit].io_act += lbolt - mttab.io_start;
		mttab.io_start = 0;
		break;

		case SSFOR:
		case SSREV:
		if (ds & S_EOF) {
			if (mttab.b_active == SSREV) {
				cp->blkno = bp->b_blkno - IORB->io_bcount;
				cp->nxrec = cp->blkno;
			} else {
				cp->blkno = bp->b_blkno + IORB->io_bcount;
				cp->nxrec = cp->blkno - 1;
			}
		} else {
			cp->blkno = bp->b_blkno;
		}
		break;

		default:
		return;
		break;
	}
	mtstart();
}

#if pm100
mt_mapnext(bp)
struct buf *bp;
{
	int i, dmaadx, endpage;
	preg_t *prp;
	register pte;
	register addr;
	reg_t *rp;
#if SVR3
	register pde_t *map;
	register pde_t *dmap;
#else
	register pte_t *map;
	register spte_t *dmap;
#endif

	dmaadx = DMASPACE + CTOB(bp->b_dma+index);
	dmap = &dmapte[bp->b_dma+index];
	endpage = BTOC(poff(paddr(bp))+CART_BLK);

	prp = (preg_t *)bp->b_rblkno;
	addr = BTOCT((int)(paddr(bp)) - (int)prp->p_regva);
	rp = prp->p_reg;


	for (i=0; i<endpage; i++,dmap++,dmaadx+=NBPP,addr+=BTOCT(NBPP)) {
		map = &rp->r_list[ctost(addr)][addr&(NPGPT-1)];
#if SVR3
		pte = map->pgi.pg_pde | PG_ALL;
		pg_setpde(dmap, pte);
#else
		pte = (map->pgi.pg_pte & ~PG_PROT) | (PG_KV|PG_KW);
		dmap->pgi.pg_pte = pte;
		dmap->pgi.pgx_pte = pte | PGX_PFN;
#endif
		invsatb(SYSATB, dmaadx);
	}
	index = (index+2) & 0x03;
	bp->b_un.b_addr += CART_BLK;
}
#endif
#if pm200 || m88k
mt_mapnext(bp)
struct buf *bp;
{
	preg_t *prp;
	register addr;
	reg_t *rp;
	extern	pde_t *dmapte;
	register pde_t *dmap, *endpage;

	dmap = &dmapte[bp->b_dma+index];
	endpage = dmap + BTOC(poff(paddr(bp))+CART_BLK);

	prp = (preg_t *)bp->b_rblkno;
	addr = BTOCT((int)(paddr(bp)) - (int)prp->p_regva);
	rp = prp->p_reg;


	for (; dmap<endpage; dmap++,addr++) {
		*dmap = rp->r_list[ctost(addr)][addr&(NPGPT-1)];
	}
	index = (index+2) & 0x03;
	bp->b_un.b_addr += CART_BLK;
}
#endif


mtread(dev)
int dev;
{
	int count;

	mt_info[dev & UNITMSK].flags &= ~IOCTLDONE;
	if (mt_info[dev & UNITMSK].flags & RZERO) {
		u.u_error = ENXIO;
		return;
	}
	count = u.u_count;
	mtphys(dev);
	physio(mtstrategy, 0, dev, B_READ);
	if (count == u.u_count)
		mt_info[dev & UNITMSK].flags |= RZERO;
}

mtwrite(dev)
int dev;
{
	mt_info[dev & UNITMSK].flags &= ~IOCTLDONE;
	if (mt_info[dev & UNITMSK].flags & WFULL) {
		u.u_error = ENOSPC;
		return;
	}
	mtphys(dev);
	physio(mtstrategy, 0, dev, B_WRITE);
	if (u.u_count)
		mt_info[dev & UNITMSK].flags |= WFULL;
}

mtphys(dev)
int dev;
{
	register struct mt_info *cp;
	daddr_t a;

	cp = &mt_info[dev & UNITMSK];
	a = u.u_offset >> BSHIFT;
	cp->blkno = a;
	cp->nxrec = a + 1;
}

/*
 * Execute an ioctl.
 */
mtioctl(dev, cmd, arg, mode)
int dev;
int cmd;
int arg;
int mode;
{
	int ds;
#ifdef BCS
	if (cmd == 0x80086d01) {
		pxmtioctl(dev, cmd, arg, mode);
		return;
	}
#endif

	mt_info[dev & UNITMSK].flags |= IOCTLDONE;
	if ( copyin((caddr_t) arg, (caddr_t) &ds, sizeof(ds) ) ) {
		u.u_error = EFAULT;
		return;
	}

	if (mtcommand(dev, cmd, ds, &ds) <= E_CE) {
		if (copyout((caddr_t)&ds, arg, sizeof(ds)) < 0) {
			u.u_error = EFAULT;
		}
	} else {
		u.u_error = EIO;
	}
}

#ifdef BCS
#define	MTIOCTL_ERR 	-1

char opmtmap[] = {
	MTIOCTL_WFM,		/* MTWEOF */
	MTIOCTL_FSF,		/* MTFSF */
	MTIOCTL_BSF,		/* MTBSF */
	MTIOCTL_FSR,		/* MTFSR */
	MTIOCTL_BSR,		/* MTBSR */
	MTIOCTL_REW,		/* MTREW */
	MTIOCTL_REW,		/* MTOFFL */
	MTIOCTL_NOP,		/* MTNOP */
	MTIOCTL_ERR,		/* MTCACHE */
	MTIOCTL_ERR,		/* MTNOCACHE */
	MTIOCTL_ERR,		/* MTFSS */
	MTIOCTL_ERR,		/* MTBSS */
	MTIOCTL_ERASE,		/* MTERA */
	MTIOCTL_ERR,		/* MTEND */
	MTIOCTL_TENS,		/* MTTEN */
};

pxmtioctl(dev, cmd, arg, mode)
int dev;
int cmd;
int arg;
int mode;
{
	struct mtop ds;
	int status;
	register opmoncmd;

	if ( copyin((caddr_t) arg, (caddr_t) &ds, sizeof(ds) ) ) {
		u.u_error = EFAULT;
		return;
	}

	if (begintrace(54))
		printf("mtioctl op = %d count = %d\n", ds.mt_op, ds.mt_count);

	switch(ds.mt_op) {
		case MTCACHE:
		case MTNOCACHE:
		case MTTEN:
		case MTERA:
			break;
		default:
			mt_info[dev & UNITMSK].flags |= IOCTLDONE;
			break;
	}

	if (ds.mt_count == 0)
		return;
	if (((unsigned) ds.mt_op) > 14) {
		u.u_error = EINVAL;
		return;
	}

	opmoncmd = opmtmap[ds.mt_op];
	if (opmoncmd == MTIOCTL_ERR) {
		u.u_error = EINVAL;
		return;
	}

	status = 0;
	if (opmoncmd == MTIOCTL_FSR || opmoncmd == MTIOCTL_BSR) {
		status = ds.mt_count;
		ds.mt_count = 1;
	}
D 2
	else if (opmoncmd != MTIOCTL_FSF && opmoncmd != MTIOCTL_BSF) {
E 2
I 2
	else if (opmoncmd != MTIOCTL_FSF && opmoncmd != MTIOCTL_BSF &&
                 opmoncmd != MTIOCTL_WFM) {
E 2
		ds.mt_count = 1;
	}

	while (ds.mt_count) {
		if (mtcommand(dev, opmoncmd, status, &status) > E_CE) {
			u.u_error = EIO;
			break;
		}
		if (status & S_ILL) {
			u.u_error = EINVAL;
			break;
		}
		ds.mt_count -= 1;
	}
}
#endif

mtprint(dev, str)
int dev;
char *str;
{
	printf("%s on MT unit %d\n", str, dev & UNITMSK);
}

mtinit()
{
	register dix;
	register int unit;
	register struct mt_info *cp;
	int ds;

	DIXLOOP(dix) {
		if (DIX(dix) == TAPE) {
			mtstatus(dix, &ds);
			unit = ds & STATUNIT;
			indtab[dix] = unit;
			cp = &mt_info[unit];
			cp->cb = (struct io_mt *)
				dmastatic(sizeof(struct io_mt));
			cp->dix = cp->cb->io_index = dix;
			cp->flags |= AVAIL;
			if (ds & STATCART) {
				cp->flags |= CART;
			}
		}
	}
}

/*
 * Get status for dix.
 */
mtstatus(dix, status)
int dix;
int *status;
{
	struct io_mt IORB;

	IORB.io_cmd = PC_STAT;
	IORB.io_index = dix;
	IORB.io_status = S_GO;
	IORB.io_error = E_OK;
	pcwait(&IORB, sizeof(struct io_mt));
	*status = IORB.io_devstat;
	return(IORB.io_error);
}

/*
 * Issue and ioctl command to dev and wait for completion.
 */
mtcommand(dev, cmd, arg, status)
int dev;
int cmd;
int arg;
int *status;
{
	struct io_mt IORB;

	IORB.io_cmd = IOCTL;
	IORB.io_index = mt_info[dev & UNITMSK].dix;
#if pm100
	IORB.io_blknum = cmd;
	IORB.io_hblknum = cmd >> 16;
	IORB.io_memaddr = arg;
	IORB.io_bcount = arg >> 16;
#endif
#if pm200 || m88k
	IORB.val = cmd;
	IORB.arg = arg;
#endif
	IORB.io_status = S_GO;
	IORB.io_error = E_OK;
	pcwait(&IORB, sizeof(struct io_mt));
	*status = IORB.io_devstat;
	return(IORB.io_error);
}
E 1
