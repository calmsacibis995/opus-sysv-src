h49259
s 00008/00003/00633
d D 1.2 90/05/02 08:40:22 root 2 1
c use kdebug 0x8000 to not cache frame buffer
e
s 00636/00000/00000
d D 1.1 90/03/06 12:28:44 root 1 0
c date and time created 90/03/06 12:28:44 by root
e
u
U
t
T
I 1
/* SID @(#)gn.c	1.16 */

/*
 * General driver for character special devices
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
#include "sys/sysconf.h"
#if SVR3
#include "sys/immu.h"
#else
#include "sys/page.h"
#endif
#include "sys/compat.h"
#include "sys/gncomm.h"
#if m88k
#include "sys/region.h"
#endif
#include "sys/proc.h"
#include "sys/inode.h"
#include "sys/conf.h"
#include "sys/file.h"

extern int selwait;

/* unit flags */

#define AVAIL	1  /* unit is available for use */
#define LOGERRS 2  /* do normal unix error handling for this unit */
#define BLKTYPE	4  /* allow block i/o */
#define BREAKUP 8  /* requests may be broken up */
#define GNBUSY	0x10	/* busy flag */
#define GNWANT	0x20	/* want flag */
#if !pm100
#define	GNRCOLL	0x40	/* select read collision */
#define	GNWCOLL	0x80	/* select write collision */
#define GNRWAKEUP	0x100
#define GNWWAKEUP	0x200
#endif

extern int dmamaxpg;
#define NPAGE (dmamaxpg - 1)

struct iotime gnstat;

struct iobuf gntab = tabinit(GN0, &gnstat.ios);

struct gn_info {
	int flags;  /* state flags */
	int dix;  /* device index for this unit */
	struct io_gn *iorb;  /* control block for data transfers */
#if !pm100
	struct proc *rsel;
	struct proc *wsel;
	struct iorb *riorb;
	struct iorb *wiorb;
	paddr_t buffer;
#endif
} gn_info[GNMAX];

int gnprint();

gnopenb(dev, flag)
minor_t dev;
int flag;
{

	gnopencom(dev, flag, 0);
}

gnopenc(dev, flag)
minor_t dev;
int flag;
{

	gnopencom(dev, flag, 1);
}

gnopencom(dev, flag, type)
int dev;
int flag;
{
	int status;
	register int unit;
	register int err;

	unit = dev >> UNITSHIFT;
	if (unit >= GNMAX ||
	    (gn_info[unit].flags & AVAIL) == 0 ||
	    ((gn_info[unit].flags & BLKTYPE) == 0 && type == 0)) {
		u.u_error = ENXIO;
		return;
	}
	err = gnlcmd(dev, IOCTL_OPEN, type, flag, &status);
	if (err) {
		u.u_error = (err == E_UERROR) ? status : ENXIO;
	}
}

gncloseb(dev, flag)
minor_t dev;
int flag;
{

	gnclosecom(dev, flag, 0);
}

gnclosec(dev, flag)
minor_t dev;
int flag;
{

	gnclosecom(dev, flag, 1);
}

gnclosecom(dev, flag, type)
int dev;
int flag;
int type;
{
	int status;
	register int err;

	err = gnlcmd(dev, IOCTL_CLOSE, type, flag, &status);
	if (err) {
		u.u_error = (err == E_UERROR) ? status : ENXIO;
	}
}

gnstrategy(bp)
register struct buf *bp;
{
	register struct gn_info *cp;

	bp->b_raddr = dmamapbuf(bp);
	bp->b_rcount = bp->b_bcount;
	cp = &gn_info[minor(bp->b_dev) >> UNITSHIFT];
	if (cp->flags & BLKTYPE) {
		bp->b_rblkno = bp->b_blkno;
	} else {
		bp->b_rblkno = u.u_offset;
	}
	bp->av_forw = NULL;
	gnstat.io_cnt++;
	gnstat.io_bcnt += BTOC(bp->b_bcount);
	bp->b_start = lbolt; 
	spl7();
	if (gntab.b_actf == NULL) {
		gntab.b_actf = bp;
	} else {
		gntab.b_actl->av_forw = bp;
	}
	gntab.b_actl = bp;
	if (gntab.b_active == 0) {
		while ( cp->flags & GNBUSY ) {
			cp->flags |= GNWANT;
			sleep((caddr_t) cp, PRIBIO);
		}
		cp->flags |= GNBUSY;
		gnstart(cp);
	}
	spl0();
}

gnstart(cp)
struct gn_info *cp;
{
	register struct io_gn *IORB;
	register struct buf *bp;

	if ((bp = gntab.b_actf) == NULL) {
		cp->flags &= ~GNBUSY;
		if (cp->flags & GNWANT) {
			cp->flags &= ~GNWANT;
			wakeup((caddr_t) cp);
		}
		return;
	}
	if (gntab.b_active == 0) {
		gntab.io_start = lbolt;
	}
	gntab.b_active = 1;
	blkacty |= (1 << GN0);
 	IORB = gn_info[minor(bp->b_dev) >> UNITSHIFT].iorb;
	IORB->IBUF = bp->b_raddr;
	IORB->io_bcount = bp->b_rcount;
	IORB->io_blknum = bp->b_rblkno;
#if pm100
	IORB->io_hblknum = bp->b_rblkno >> 16;
#endif
	IORB->io_cmd = bp->b_flags & B_READ ? PC_READ : PC_WRITE;
	IORB->io_dev = minor(bp->b_dev);
	IORB->io_status = S_GO;
	pcstrategy(IORB);
}

gnintr(IORB)
register struct io_gn *IORB;
{
	register struct buf *bp;
	register struct gn_info *cp;
	register misc;
	
	misc = indtab[IORB->io_index];
	cp = &gn_info[misc];

	if ( IORB->io_cmd == IOCTL ) {
		wakeup((caddr_t) IORB);
		return;
	}
	if ((bp = gntab.b_actf) == NULL) {
		cp->flags &= ~GNBUSY;
		if (cp->flags & GNWANT) {
			cp->flags &= ~GNWANT;
			wakeup((caddr_t) cp);
		}
		return;
	}
	blkacty &= ~(1 << GN0);
	misc = IORB->io_error;
	if (misc != E_OK) {
		bp->b_flags |= B_ERROR;
		if (misc == E_UERROR) {
			bp->b_error = IORB->io_devstat;
		}
		if (cp->flags & LOGERRS) {
			gntab.io_addr = (PADDR)IORB;
			gntab.io_nreg = sizeof(struct io_gn) / sizeof(short);
			fmtberr(&gntab, 0);
			prcom(gnprint, bp, misc, IORB->io_devstat);
		}
	}
	misc = IORB->io_bcount;
	bp->b_raddr += misc;
	bp->b_rcount -= misc;
	if (cp->flags & BLKTYPE) {
		bp->b_rblkno += (misc + BMASK) >> BSHIFT;
	} else {
		bp->b_rblkno += misc;
	}
	if (gntab.io_erec) {
		logberr(&gntab, bp->b_flags & B_ERROR);
	}
	gntab.b_actf = bp->av_forw;
	gntab.b_active = 0;
	bp->b_resid = bp->b_rcount;
	dmaunmapbuf(bp);
	gnstat.io_resp += lbolt - bp->b_start;
	gnstat.io_act += lbolt - gntab.io_start;
	iodone(bp);
	gnstart(cp);
}

gnread(dev)
minor_t dev;
{
	if ( !( gn_info[minor(dev) >> UNITSHIFT].flags & BREAKUP ) ) {
		/* if request does not fit, don't break it up */
		if ( u.u_count > CTOB(NPAGE) ) {
			u.u_error = EINVAL;
			return;
		}
	}
	physio(gnstrategy, 0, dev, B_READ);
}

gnwrite(dev)
minor_t dev;
{
	if ( !( gn_info[minor(dev) >> UNITSHIFT].flags & BREAKUP ) ) {
		/* if request does not fit, don't break it up */
		if ( u.u_count > CTOB(NPAGE) ) {
			u.u_error = EINVAL;
			return;
		}
	}
	physio(gnstrategy, 0, dev, B_WRITE);
}

/*
 * Get status for dix.
 */
gnstatus(dix, status, cnt)
int dix;
int *status;
unsigned *cnt;
{
	struct io_gn IORB;

	IORB.io_cmd = PC_STAT;
	IORB.io_index = dix;
	IORB.io_status = S_GO;
	IORB.io_error = E_OK;
	pcwait(&IORB, sizeof(struct io_gn));
	*status = IORB.io_devstat;
#if !pm100
	*cnt = IORB.cnt;
#endif
	return(IORB.io_error);
}

/*
 * Issue and ioctl command to dev and wait for completion.
 */
gnlcmd(dev, cmd, len, adx, status)
int dev;
int cmd;
int len;
int adx;
int *status;
{
	register struct io_gn *IORB;
	register struct gn_info *cp;
	int err, s;

	s = spl7();
	cp = &gn_info[minor(dev) >> UNITSHIFT];
	while ( cp->flags & GNBUSY ) {
		cp->flags |= GNWANT;
		/* have to prevent this guy from starvation */
		sleep((caddr_t) cp, PRIBIO - 1);
	}
	cp->flags |= GNBUSY;
	IORB = cp->iorb; 
	IORB->io_cmd = IOCTL;
	IORB->io_index = cp->dix;
#if pm100
	IORB->io_blknum = (ushort)cmd;
	IORB->io_hblknum = (ushort)(cmd >> 16);
	IORB->io_memaddr = (ushort)adx;
	IORB->io_bcount = (ushort)len;
#else
	IORB->blk = cmd;
	IORB->cnt = len;
	IORB->buf = (len << 16) | (adx & 0xffff); /* compatibility */
	IORB->val = adx;
#endif
	IORB->io_dev = dev;
	IORB->io_status = S_GO;
	IORB->io_error = E_OK;
	pcstrategy(IORB);
	while ( IORB->io_status != S_IDLE )
		sleep((caddr_t) IORB, PRIBIO );
	*status = IORB->io_devstat;
	err = IORB->io_error;
	cp->flags &= ~GNBUSY;
	if ( cp->flags & GNWANT ) {
		cp->flags &= ~GNWANT;
		wakeup((caddr_t) cp);
	}
	splx(s);
	return(err);
}

/*
 * Print out an error report.
 */
gnprint(dev, str)
int dev;
char *str;
{
	printf("%s on GN unit %d\n", str, dev);
}

#if !pm100
/*	gnwakeup() is called by pcintr() when opmon flags a select
*/
gnwakeup(seliorb)
struct iorb *seliorb;
{
	struct gn_info *cp;
	int coll, wake;
	struct proc **pp;

	cp = (struct gn_info *)(seliorb->a);
	if (seliorb == cp->riorb) {
		coll = GNRCOLL;
		wake = GNRWAKEUP;
		pp = &cp->rsel;
	} else {
		coll = GNWCOLL;
		wake = GNWWAKEUP;
		pp = &cp->wsel;
	}
	if (*pp) {
		selwakeup(*pp, cp->flags&coll);
		*pp = NULL;
		cp->flags &= ~coll;
		cp->flags |= wake;
	}
}

/*
**  handle select()
*/
gnselect(fp, ip, flag)
register struct inode *ip;
{
	int s;
	int unit;
	register struct gn_info *cp;
	register struct iorb *iorb, *seliorb;
	int coll, wake;
	struct proc **pp;

	unit = minor(ip->i_rdev) >> UNITSHIFT;
	cp = &gn_info[unit];

	if (flag == 0)
		return 0;

	if (flag & FREAD) {
		seliorb = cp->riorb;
		coll = GNRCOLL;
		wake = GNRWAKEUP;
		pp = &cp->rsel;
	} else {
		seliorb = cp->wiorb;
		coll = GNWCOLL;
		wake = GNWWAKEUP;
		pp = &cp->wsel;
	}
	if (seliorb == NULL)
		return 1;

	s = splhi();
	if (cp->flags & wake) {
		cp->flags &= ~wake;
		splx(s);
		return 1;
	}
	if (*pp && (*pp)->p_wchan == (caddr_t)&selwait)
		cp->flags |= coll;
	else
		*pp = u.u_procp;

	if (seliorb->st == S_IDLE)
		pcstrategy(seliorb);
	splx(s); /* select pending to opmon */
	return 0;
}
#endif /* pm100 */

/*
 * Initialize driver state.
 */
gninit()
{
	register dix;
	register int unit;
	register struct gn_info *cp;
	int status;
	unsigned cnt;

#if !pm100
	int maj = 0;
	for (unit = 0; unit < cdevcnt ; unit++)
		if (cdevsw[unit].d_open == gnopenc) {
			maj = unit;
			break;
		}
	if (maj == 0)
		panic("gn: cannot find gnopen");
	cdevsw[maj].d_select = gnselect;
#endif

	DIXLOOP(dix) {
		if (DIX(dix) == GEN) {
			gnstatus(dix, &status, &cnt);
			unit = status & STATUMASK;
			indtab[dix] = unit;
			cp = &gn_info[unit];
			cp->iorb = (struct io_gn *)
				dmastatic(sizeof(struct io_gn));
			cp->dix = cp->iorb->io_index = dix;
			cp->flags |= AVAIL;
			if (status & STATLOGERRS) {
				cp->flags |= LOGERRS;
			}
			if (status & STATBLKTYPE) {
				cp->flags |= BLKTYPE;
			}
			if (status & STATBREAKUP) {
				cp->flags |= BREAKUP;
			}
#if !pm100
			if (status & STATSELECT) {
				cp->riorb = (struct iorb *)
					dmastatic(2*sizeof(struct iorb));
				cp->wiorb = &cp->riorb[1];

				cp->riorb->dix = dix;
				cp->riorb->st = S_IDLE;
				cp->riorb->cmd = PC_SELECT;
				cp->riorb->f = gnwakeup;
				cp->riorb->a = (long)cp;
				cp->riorb->val = FREAD;

				cp->wiorb->dix = dix;
				cp->wiorb->st = S_IDLE;
				cp->wiorb->cmd = PC_SELECT;
				cp->wiorb->f = gnwakeup;
				cp->wiorb->a = (long)cp;
				cp->wiorb->val = FWRITE;
			} else {
				cp->riorb = cp->wiorb = NULL;
			}
			if ((status & STATPBUF) && cnt) {
				extern pde_t *dmapte;
				register int i, j;
				register pde_t *map;
				dmaalign(NBPP);
				cnt = ctob(btoc(cnt));
				i = dmastatic(cnt);
				i = btoct(i - DMASPACE);
#if m88k
				map = &dmapte[i];
				cp->buffer = map->pgi.pg_spde&~POFFMASK;
				for ( j = 0; j < btoc(cnt) ; j++, map++ ) {
D 2
					map->pgi.pg_spde |= PG_WT;	
					map->pgm.pg_ci = 0;
					kvtopde(map->pgi.pg_spde)->pgm.pg_wt=1;
E 2
I 2
					if ((kdebug & 0x8000) == 0) {
						map->pgi.pg_spde |= PG_WT;
						map->pgm.pg_ci = 0;
						kvtopde(map->pgi.pg_spde)->pgm.pg_wt=1;
					}
					else {
						kvtopde(map->pgi.pg_spde)->pgm.pg_ci=1;
					}
E 2
				}
				cp->buffer =
					dmapte[i].pgi.pg_spde&~POFFMASK;
#else
				cp->buffer =
					dmapte[i].pgi.pg_pde&~POFFMASK;
#endif
				/* begin sanity check */
				for (j=i+1; j<i+btoc(cnt); j++)
					if ((dmapte[j-1].pgm.pg_pfn + 1) 
						!= dmapte[j].pgm.pg_pfn) {
					printf("gn: noncontiguous space: %d 0x%x 0x%x\n", j, dmapte[j-1].pgm.pg_pfn, dmapte[j].pgm.pg_pfn);
					printf("dmapte=0x%x\n", dmapte);
					panic("gn: noncontiguous space");
				}
				/* end sanity check */
			} else {
				cp->buffer = NULL;
			}
#endif /* pm100 */
		}
	}
}

/*
 * Execute an ioctl.
 */
gnioctl(dev, cmd, arg, mode)
minor_t dev;
int cmd;
int arg;
int mode;
{
	caddr_t comm;  /* address of local buffer */
	int commlen;  /* lenght of local buffer */
	int dmaadx;  /* address of local buffer in dma space */
	int err;  /* error temp */
	int status;  /* status returned from a command */
	struct gn_info *cp;

	commlen = (cmd >> IOCTLLENSHIFT) & IOCTLLENMASK;
	comm = 0;
	dmaadx = 0;

	if (commlen > 0) {
		comm = (caddr_t)SPTALLOC(BTOC(commlen));
		if (comm == 0) {
			u.u_error = ENOMEM;
			goto out;
		}
		if (copyin(arg, comm, commlen) < 0) {
			u.u_error = EFAULT;
			goto out;
		}
		dmaadx = mapdma(comm, commlen);
	}

	switch ((cmd >> IOCTLCMDSHIFT) & IOCTLCMDMASK) {

		case IOCTL_OPEN:
		case IOCTL_CLOSE:
		u.u_error = ENXIO;
		break;

#if !pm100
		case IOCTL_PBUF:
		cp = &gn_info[dev >> UNITSHIFT];
		if (cp->buffer)
			u.u_rval1 = cp->buffer;
		else
			u.u_error = ENXIO;
		break;
#endif

		default:
		if (comm != 0) {
			err = gnlcmd(dev, cmd, commlen, dmaadx, &status);
		} else {
			err = gnlcmd(dev, cmd, ((unsigned)arg) >> 16,
				(ushort)arg, &status);
		}
		if (err != E_OK) {
			u.u_error = (err == E_UERROR) ? status : EIO;
		}
		break;	
	}

	out:
	if (comm != 0) {
		if (u.u_error == 0) {
			if (copyout((caddr_t)comm, arg, commlen) < 0) {
				u.u_error = EFAULT;
			}
		}
		if (dmaadx != NULL) {
			relsdma(dmaadx, commlen);
		}
		sptfree(comm, BTOC(commlen), 1);
	}
}
E 1
