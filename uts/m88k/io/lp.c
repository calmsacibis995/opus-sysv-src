/* SID @(#)lp.c	1.5 */

/*
 *  PC line printer driver
 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/dev32k.h"
#include "sys/pc32k.h"
#include "sys/cb.h"
#if SVR3
#include "sys/fs/s5dir.h"
#else
#include "sys/dir.h"
#endif
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/tty.h"
#include "sys/lprio.h"
#include "sys/compat.h"

#if !SVR3
extern int cfreedmaoffset;
#endif

#define	LPPRI	(PZERO+8)
#define	LPLOWAT	40
#define	LPHIWAT	100

struct lp_info {
	struct	io_lpt *cb;  /* PC request block pointer */
	struct	clist l_outq;
	struct	cblock *activecb;
	char	flag;
	char	ind;
	char	retry;
	char	avail;
	int	ccc;
	int	mcc;
	int	mlc;
	int	line;
	int	col;
} lp_info[LPMAX];


#define	OPEN	010
#define	CAP	020
#define	NOCR	040
#define	RAW	0100
#define	ASLP	0200

#define	FORM	014

lpopen(dev, mode)
minor_t dev;
{
	register unit;
	register struct lp_info *lp;

	unit = dev&07;
	lp = &lp_info[unit];
	if (unit >= LPMAX || !lp->avail) {
		u.u_error = ENXIO;
		return;
	}
	if (lp->flag) {	/* already open */
		u.u_error = EBUSY;
		return;
	}
	lp->flag = (dev&0177)|OPEN;
	lp->retry = 0;
	if (lp->col == 0) {	/* first time only */
		lp->ind = 0;
		lp->col = 2000000;
		lp->line = 66;
	}
	if ((lp->flag & RAW) == 0) {
		lpoutput(lp, FORM);
	}
}

lpclose(dev)
minor_t dev;
{
	register unit;
	register struct lp_info *lp;

	unit = dev&07;
	lp = &lp_info[unit];
	if ((lp->flag & RAW) == 0) {
		lpoutput(lp, FORM);
	}
	lp->flag = 0;
}

lpwrite(dev)
minor_t dev;
{
	register c;
	register struct lp_info *lp;

	lp = &lp_info[dev & 07];
	if (!lp->avail) {
		u.u_error = ENXIO;
		return;
	}
	if ( lp->flag & RAW ) {	/* treat raw lp more efficiently */
		rlpwrite(lp);
		return;
	}
	while (u.u_count) {
		spl4();
		while(lp->l_outq.c_cc > LPHIWAT) {
			lpintr(lp->cb);
			lp->flag |= ASLP;
			sleep(lp, LPPRI);
		}
		spl0();
		c = fubyte(u.u_base++);
		if (c < 0) {
			u.u_error = EFAULT;
			break;
		}
		u.u_count--;
		lpoutput(lp, c);
	}
	spl4();
	lpintr(lp->cb);
	spl0();
}

rlpwrite(lp)
register struct lp_info *lp;
{
	register n;
	register struct cblock *cp;

	while (u.u_count) {
		while ((cp = getcf()) == NULL) {
			cfreelist.c_flag = 1;
			sleep(&cfreelist, LPPRI);
		}
		n = min( cp->c_last, u.u_count );
		if (copyin( u.u_base, cp->c_data, n )) {
			putcf(cp);
			u.u_error = EFAULT;
			return;
		}
		cp->c_last = n;
		u.u_base += n;
		u.u_offset += n;
		u.u_count -= n;

		putcb(cp, &lp->l_outq);
		spl4();
		while(lp->l_outq.c_cc > LPHIWAT) {
			lpintr(lp->cb);
			lp->flag |= ASLP;
			sleep(lp, LPPRI);
		}
		spl0();
	}
	spl4();
	lpintr(lp->cb);
	spl0();
}

lpoutput(lp, c)
register struct lp_info *lp;
register c;
{
	register struct io_lpt *IORB;

	IORB = lp->cb;
	if (lp->flag&RAW) {
		putc(c, &lp->l_outq);
		return;
	}
	if(lp->flag&CAP) {
		if(c>='a' && c<='z')
			c += 'A'-'a'; 
		switch(c) {
		case '{':
			c = '(';
			goto esc;
		case '}':
			c = ')';
			goto esc;
		case '`':
			c = '\'';
			goto esc;
		case '|':
			c = '!';
			goto esc;
		case '~':
			c = '^';
		esc:
			lpoutput(lp, c);
			lp->ccc--;
			c = '-';
		}
	}
	switch(c) {
	case '\t':
		lp->ccc = ((lp->ccc+8-lp->ind) & ~7) + lp->ind;
		return;
	case '\n':
		lp->mlc++;
		if(lp->mlc >= lp->line )
			c = FORM;
	case FORM:
		lp->mcc = 0;
		if (lp->mlc) {
			putc(c, &lp->l_outq);
			if(c == FORM)
				lp->mlc = 0;
		}
	case '\r':
		lp->ccc = lp->ind;
		spl4();
		lpintr(IORB);
		spl0();
		return;
	case 010:
		if(lp->ccc > lp->ind)
			lp->ccc--;
		return;
	case ' ':
		lp->ccc++;
		return;
	default:
		if(lp->ccc < lp->mcc) {
			if (lp->flag&NOCR) {
				lp->ccc++;
				return;
			}
			putc('\r', &lp->l_outq);
			lp->mcc = 0;
		}
		if(lp->ccc < lp->col) {
			while(lp->ccc > lp->mcc) {
				putc(' ', &lp->l_outq);
				lp->mcc++;
			}
			putc(c, &lp->l_outq);
			lp->mcc++;
		}
		lp->ccc++;
	}
}

lptimeout(IORB)
struct io_lpt *IORB;
{

	lpintr(IORB);
}

lpintr(IORB)
struct io_lpt *IORB;
{
	register struct lp_info *lp;
	register c;
	register dev;
	register struct cblock *bp;

	dev = indtab[IORB->io_index];
	lp = &lp_info[dev];

	if (IORB->io_status == S_IDLE) {
		if (lp->retry) {
			lp->retry = 0;
			IORB->io_status = S_GO;
			pcstrategy(IORB);
		} else if (IORB->io_error == E_DEV) {
			lp->retry = 1;
			timeout(lptimeout, IORB, 5 * HZ);
		} else {
			if (lp->activecb) {
				putcf(lp->activecb);
				lp->activecb = 0;
			}
			if (IORB->io_error != E_OK && lp->avail) {
				printf("LP%d: error 0x%x, 0x%x\n",
				       dev, IORB->io_error, IORB->io_devstat);
				/* flush out queue */
				while ((bp = getcb(&lp->l_outq)) != NULL) {
					putcf(bp);
				}
			}
			if ((bp = getcb(&lp->l_outq)) != NULL) {
				lp->activecb = bp;
				IORB->io_bcount = bp->c_last - bp->c_first;
				IORB->IBUF =
#if pm100
					(ushort) (&bp->c_data[bp->c_first]);
#endif
#if pm200 || m88k
					(unsigned) (&bp->c_data[bp->c_first]);
#endif
				IORB->io_cmd = PC_WRITE;
				IORB->io_status = S_GO;
				pcstrategy(IORB);
			}
		}
	}
	if (lp->l_outq.c_cc <= LPLOWAT && lp->flag&ASLP) {
		lp->flag &= ~ASLP;
		wakeup(lp);
	}
}

lpioctl(dev, cmd, arg, mode)
minor_t dev;
{
	register struct lp_info *lp;
	struct lprio lpr;

	lp = &lp_info[dev];
	if (!lp->avail) {
		u.u_error = ENXIO;
		return;
	}
	switch(cmd) {
	case LPRGET:
		lpr.ind = lp->ind;
		lpr.col = lp->col;
		lpr.line = lp->line;
		if (copyout(&lpr, arg, sizeof lpr))
			u.u_error = EFAULT;
		break;
	case LPRSET:
		if (copyin(arg, &lpr, sizeof lpr)) {
			u.u_error = EFAULT;
			break;
		}
		lp->ind = lpr.ind;
		lp->col = lpr.col;
		lp->line = lpr.line;
		break;
	default:
		u.u_error = EINVAL;
	}
}

/*
 * Initialize driver state.
 */
lpinit()
{
	register int i;
	register int lp_cnt;

	lp_cnt = 0;
	DIXLOOP(i) {
		if (DIX(i) == LPT) {
			indtab[i] = lp_cnt;
			lp_info[lp_cnt].cb = (struct io_lpt *)
				dmastatic(sizeof(struct io_lpt));
			lp_info[lp_cnt].cb->io_index = i;
			lp_info[lp_cnt].avail = 1;
			lp_cnt++;
		}
	}
}
