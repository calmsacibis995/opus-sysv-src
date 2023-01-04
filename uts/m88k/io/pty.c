/* SID @(#)pty.c	1.3 */

/*
 *	Pseudo tty driver
 */
#include "sys/param.h"
#include "sys/types.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/cb.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/ioctl.h"
#include "sys/tty.h"
#include "sys/termio.h"
#include "sys/buf.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/sysinfo.h"
#include "sys/sysmacros.h"
#include "sys/inode.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/ttold.h"

extern int	ptmax;
struct	tty *PT_tty;

struct PT_info {
	int flags;
	struct proc *rsel, *wsel;
} *PT_info;

int	PTproc();

#define PF_CTRLROPEN	0x100		/* controller pty is open */
#define	PF_NBIO		0x1		/* FIONBIO */
#define	PF_RCOLL	0x2
#define	PF_WCOLL	0x4

int PTcopen(), PTcselect();

npttys()
{
	return(ptmax);
}

PTsopen(dev, flag)
int dev;
int flag;
{
	register struct tty *tp;
	register struct PT_info *cp;
	register i;
	struct winsize ws;


	dev = minor(dev);
	if (dev >= ptmax) {
		u.u_error = ENXIO;
		return;
	}
	tp = &PT_tty[dev];
	cp = &PT_info[dev];
	if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
		ttinit(tp);
		tp->t_proc = PTproc;
		tp->t_cflag &= ~CBAUD;
		tp->t_cflag |= B9600;
		tp->t_iflag |= ICRNL|IXON|IXANY|ISTRIP;
		tp->t_oflag |= OPOST|ONLCR;
		tp->t_lflag |= ISIG|ICANON|ECHO|ECHOK;
	}

	if (cp->flags & PF_CTRLROPEN) {
		tp->t_state |= CARR_ON;
	}

	while ((tp->t_state & CARR_ON) == 0) {
		tp->t_state |= WOPEN;
		sleep((caddr_t)&tp->t_rawq, TTIPRI);
	}

	ws = tp->t_winsize;	/* preserve window size for reopens */
	(*linesw[tp->t_line].l_open)(tp, flag);
	tp->t_winsize = ws;
	PTcwakeup(tp, FREAD|FWRITE);
}

PTsclose(dev)
int dev;
{
	register struct tty *tp;
	struct PT_info *cp;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];

	if ((cp->flags & PF_CTRLROPEN) == 0) {
		ttyflush(tp, (FREAD|FWRITE));
	}
	(*linesw[tp->t_line].l_close)(tp);
	PTcwakeup(tp, FREAD|FWRITE);
}

PTsread(dev)
int dev;
{
	register struct tty *tp;
	register struct PT_info *cp;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];

	if (cp->flags & PF_CTRLROPEN) {
		(*linesw[tp->t_line].l_read)(tp);
	}
	PTcwakeup(tp, FWRITE);
}

/*
 * Write to pseudo-tty.
 * Wakeups of controlling tty will happen
 * indirectly, when tty driver calls PTproc.
 */
PTswrite(dev)
int dev;
{
	register struct tty *tp;
	register struct PT_info *cp;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];

	if (cp->flags & PF_CTRLROPEN) {
		(*linesw[tp->t_line].l_write)(tp);
	} else {
		u.u_error = EIO;
	}
}

PTcopen(dev, flag)
int dev;
int flag;
{
	register struct tty *tp;
	struct PT_info *cp;
	register i;

	dev = minor(dev);
	if (dev >= ptmax) {
		u.u_error = ENXIO;
		return;
	}
	tp = &PT_tty[dev];
	cp = &PT_info[dev];
	if (cp->flags & PF_CTRLROPEN) {
		u.u_error = EBUSY;
		return;
	}
	tp->t_proc = PTproc;
	if (tp->t_state & WOPEN) {
		wakeup((caddr_t)&tp->t_rawq);
	}
	bzero((caddr_t)&tp->t_winsize, sizeof(tp->t_winsize));
	tp->t_state |= CARR_ON;
	cp->flags = PF_CTRLROPEN;
}

PTcclose(dev)
int dev;
{
	register struct tty *tp;
	struct PT_info *cp;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];

	ttyflush(tp, (FREAD|FWRITE));
	if (tp->t_state & ISOPEN) {
		signal(tp->t_pgrp, SIGHUP);
		tp->t_pgrp = 0;
	}
	(*linesw[tp->t_line].l_close)(tp);
	tp->t_state &= ~CARR_ON;
	cp->flags &= ~PF_CTRLROPEN;
}

PTcread(dev)
int dev;
{
	register struct tty *tp;
	struct PT_info *cp;
	register struct ccblock *tbuf;
	int cc;


	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];
	tbuf = &tp->t_tbuf;

	/*
	 * We want to block until the slave
	 * is open, and there's something to read;
	 * but if we lost the slave,
	 * then return the appropriate error instead.
	 */
	for (;;) {
#ifdef huh  /* this dosn't work cause PTsclose turnes off ISOPEN before
	     * waiting for output queue to drain and thus can get stuck
	     * because the test below dosn't let this routine read the
	     * chars. I took out the ISOPEN check for now to see if this
	     * fixes the problem.
	     */
		if (tp->t_state & ISOPEN) {
			if ((tp->t_outq.c_cc || tbuf->c_count) &&
			    (tp->t_state & TTSTOP) == 0) {
				break;
			}
		}
#else
		if ((tp->t_outq.c_cc || tbuf->c_count) &&
		    (tp->t_state & TTSTOP) == 0) {
			break;
		}
#endif
		if ((tp->t_state & CARR_ON) == 0) {
			u.u_error = EIO;
			return;
		}
		if (u.u_fmode & FNDELAY) {
			return;
		}
		if (u.u_fmode & FNONB) {
			u.u_error = EAGAIN;
			return;
		}
		if (cp->flags & PF_NBIO) {
			u.u_error = ewouldblock();
			return;
		}
		sleep((caddr_t)&tp->t_outq.c_cf, TTIPRI);
	}


	/* transfer characters from the tty output queue into user space */
	while (u.u_count != 0 && u.u_error == 0) {
		if (tbuf->c_ptr == NULL || tbuf->c_count == 0) {
			if (tbuf->c_ptr) {
				tbuf->c_ptr -= tbuf->c_size - tbuf->c_count;
			}
			if (!(CPRES & (*linesw[tp->t_line].l_output)(tp))) {
				break;
			}
		}
		cc = u.u_count > tbuf->c_count ? tbuf->c_count : u.u_count;


		if (copyout(tbuf->c_ptr, u.u_base, cc)) {
			u.u_error = EFAULT;
		}
		u.u_base += cc;
		u.u_count -= cc;
		tbuf->c_count -= cc;
		tbuf->c_ptr += cc;
	}
	if (tp->t_state & TBLOCK) {
		if (tp->t_rawq.c_cc < TTXOLO) {
			(*tp->t_proc)(tp, T_UNBLOCK);
		}
	}
	if (tp->t_wsel)
		ttwakeup(tp, TS_WCOLL);
}

PTcwrite(dev)
int dev;
{
	register struct tty *tp;
	struct PT_info *cp;
	register int c;
	register int ch;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];

again:
	while (u.u_count != 0) {
		if (tp->t_state & TBLOCK) {
			goto block;
		}
		if ((tp->t_state & ISOPEN) == 0) {
			goto block;
		}
		if (tp->t_rbuf.c_ptr == NULL) {
			goto block;
		}
		c = fubyte(u.u_base++);
		ch = c & 0177;
		if (c < 0) {
			u.u_error = EFAULT;
			break;
		}
		u.u_count--;
		if (tp->t_iflag & IXON) {
			if (tp->t_state & TTSTOP) {
				if (ch == CSTART || tp->t_iflag & IXANY) {
					(*tp->t_proc)(tp, T_RESUME);
				}
			} else {
				if (ch == CSTOP) {
					(*tp->t_proc)(tp, T_SUSPEND);
				}
			}
			if (ch == CSTART || ch == CSTOP) {
				continue;
			}
		}
		if (tp->t_iflag & ISTRIP) {
			c &= 0177;
		} else {
			c &= 0377;
		}
		*tp->t_rbuf.c_ptr = c;
		tp->t_rbuf.c_count--;
		(*linesw[tp->t_line].l_input)(tp, L_BUF);
	}
	return;

block:
	/*
	 * Come here to wait for slave to open, for space
         * in outq, or space in rawq.
         */
	if ((tp->t_state & CARR_ON) == 0) {
		u.u_error = EIO;
		return;
	}
	if (u.u_fmode & FNDELAY) {
		return;
	}
	if (u.u_fmode & FNONB) {
		u.u_error = EAGAIN;
		return;
	}
	if (cp->flags & PF_NBIO) {
		u.u_error = ewouldblock();
		return;
	}
	sleep((caddr_t)&tp->t_rawq.c_cf, TTOPRI);
	goto again;
}

PTsioctl(dev, cmd, arg, flag)
int dev;
int cmd;
int arg;
int flag;
{

	PTioctl(0, dev, cmd, arg, flag);
}

PTcioctl(dev, cmd, arg, flag)
int dev;
int cmd;
int arg;
int flag;
{

	PTioctl(1, dev, cmd, arg, flag);
}

PTioctl(ctrlr, dev, cmd, arg, flag)
int ctrlr;
int dev;
int cmd;
int arg;
int flag;
{
	register struct tty *tp;
	struct PT_info *cp;
	int temp;

	dev = minor(dev);
	tp = &PT_tty[dev];
	cp = &PT_info[dev];
	switch (cmd) {

		case FIONBIO:
			if (!ctrlr)
				goto comm;
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
				break;
			}
			if (temp)
				cp->flags |= PF_NBIO;
			else
				cp->flags &= ~PF_NBIO;
			break;

		case FIONREAD:
			if (ctrlr)
				temp = PTcreadable(dev);
			else
				temp = ttnread(tp);
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCSETP:
			/*
	 		* IF CONTROLLER STTY THEN MUST FLUSH TO PREVENT A HANG.
	 		* ttywait will hang if there are characters in the outq.
			*/
			if (ctrlr) {
				while (getc(&tp->t_outq) >= 0)
					;
			}
			/* FALL THRU */

		default:
		comm:
			ttiocom(tp, cmd, arg, flag);
		break;
	}
}

PTproc(tp, cmd)
register struct tty *tp;
int cmd;
{


	switch (cmd) {

		case T_TIME:
		tp->t_state &= ~TIMEOUT;
		goto start;

		case T_WFLUSH:
		tp->t_state &= ~TTSTOP;
		goto start;
		
		case T_CTRESUME:
		case T_RESUME:
		tp->t_state &= ~TTSTOP;
		goto start;

		case T_OUTPUT:
		
		start:
		if (tp->t_state & (BUSY|TTSTOP)) {
			break;
		}
		PTcwakeup(tp, FREAD);
		break;

		case T_CTSUSPEND:
		case T_SUSPEND:
		tp->t_state |= TTSTOP;
		break;

		case T_CTBLOCK:
		case T_BLOCK:
		tp->t_state |= TBLOCK;
		break;

		case T_RFLUSH:
		if (!(tp->t_state&TBLOCK)) {
			break;
		}
		goto startin;

		case T_CTUNBLOCK:
		case T_UNBLOCK:
		
		startin:
		tp->t_state &= ~TBLOCK;
		PTcwakeup(tp, FWRITE);
		break;
		
	}
}

PTcwakeup(tp, flag)
struct tty *tp;
int flag;
{
	int dev;
	register struct PT_info *cp;

	dev = tp - PT_tty;
	cp = &PT_info[dev];

	if (flag & FREAD) {
		if (cp->rsel) {
			selwakeup(cp->rsel, cp->flags&PF_RCOLL);
			cp->rsel = 0;
			cp->flags &= ~PF_RCOLL;
		}
		wakeup((caddr_t)&tp->t_outq.c_cf);
	}
	if (flag & FWRITE) {
		if (cp->wsel) {
			selwakeup(cp->wsel, cp->flags&PF_WCOLL);
			cp->wsel = 0;
			cp->flags &= ~PF_WCOLL;
		}
		wakeup((caddr_t)&tp->t_rawq.c_cf);
	}
}

PTcreadable(dev)
int dev;
{
	register struct tty *tp;
	register cnt;

	if (dev < 0 || dev >= ptmax) {
		u.u_error = ENXIO;
		return(0);
	}
	tp = &PT_tty[dev];
	if ((cnt = tp->t_outq.c_cc + tp->t_tbuf.c_count) &&
	    (tp->t_state & TTSTOP) == 0) {
		return(cnt);
	}
	return(0);
}

PTcselect(fp, ip, flag)
register struct inode *ip;
{
	int dev;
	register struct PT_info *cp;
	register struct tty *tp;

	dev = minor(ip->i_rdev);
	cp = &PT_info[dev];
	tp = &PT_tty[dev];

	switch (flag) 
	{
		case FREAD:
			if (PTcreadable(dev))
				return (1);
		case 0:
			if (cp->rsel && cp->rsel->p_wchan == (caddr_t)&selwait)
				cp->flags |= PF_RCOLL;
			else
				cp->rsel = u.u_procp;
			return 0;
		case FWRITE:
			if ((tp->t_rawq.c_cc + tp->t_canq.c_cc) < TTYHOG - 2)
				return(1);
			if (cp->wsel && cp->wsel->p_wchan == (caddr_t)&selwait)
				cp->flags |= PF_WCOLL;
			else
				cp->wsel = u.u_procp;
			return 0;
	}
}

ptinit()
{
	register i, cd;
	for (i = 0; i < cdevcnt; i++)
		if (cdevsw[i].d_open == PTcopen)
			cdevsw[i].d_select = PTcselect;

	i = (sizeof (struct tty) + sizeof (struct PT_info)) * ptmax;
	i = btoc(i);
	PT_tty = (struct tty *) sptalloc (i, 1, 0);
	PT_info = (struct PT_info *) (PT_tty + ptmax);

	for (i = 0; i < cdevcnt; i++)
		if (cdevsw[i].d_open == PTsopen)
			cdevsw[i].d_ttys = PT_tty;
}
