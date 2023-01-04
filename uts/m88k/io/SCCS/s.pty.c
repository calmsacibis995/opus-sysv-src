h31316
s 00001/00001/00589
d D 1.7 90/06/29 16:33:33 root 7 6
c ptmax is now an extern
e
s 00024/00019/00566
d D 1.6 90/06/28 02:17:28 root 6 5
c pty's can now be specified from the boot line.
e
s 00002/00001/00583
d D 1.5 90/06/20 00:45:15 root 5 4
c FIONBIO wasbeing ignored for the slave side, now we call ttiocom.
e
s 00002/00002/00582
d D 1.4 90/04/24 16:53:24 root 4 3
c preserve window size for reopens
e
s 00001/00001/00583
d D 1.3 90/04/16 14:42:32 root 3 2
c modified FIONREAD to call ttnread, instead of looking at the canonical queue only
e
s 00004/00000/00580
d D 1.2 90/03/22 17:14:12 root 2 1
c preserve window size slave open, zero window size info in master open.
e
s 00580/00000/00000
d D 1.1 90/03/06 12:28:58 root 1 0
c date and time created 90/03/06 12:28:58 by root
e
u
U
t
T
I 1
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

D 6
#undef	PTMAX
#define	PTMAX	64
struct	tty PT_tty[PTMAX];
E 6
I 6
D 7
int	ptmax	=  64;
E 7
I 7
extern int	ptmax;
E 7
struct	tty *PT_tty;
E 6

struct PT_info {
	int flags;
	struct proc *rsel, *wsel;
D 6
} PT_info[PTMAX];
E 6
I 6
} *PT_info;
E 6

int	PTproc();

#define PF_CTRLROPEN	0x100		/* controller pty is open */
#define	PF_NBIO		0x1		/* FIONBIO */
#define	PF_RCOLL	0x2
#define	PF_WCOLL	0x4

int PTcopen(), PTcselect();
D 6
int PTinited;
E 6

npttys()
{
D 6
	return(PTMAX);
E 6
I 6
	return(ptmax);
E 6
}

PTsopen(dev, flag)
int dev;
int flag;
{
	register struct tty *tp;
	register struct PT_info *cp;
	register i;
I 2
	struct winsize ws;
E 2


	dev = minor(dev);
D 6
	if (dev >= PTMAX) {
E 6
I 6
	if (dev >= ptmax) {
E 6
		u.u_error = ENXIO;
		return;
	}
D 6
	if (!PTinited)
		for (i = 0; i < cdevcnt; i++)
			if (cdevsw[i].d_open == PTcopen)
				cdevsw[i].d_select = PTcselect;
	PTinited = 1;
E 6
	tp = &PT_tty[dev];
	cp = &PT_info[dev];
	if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
I 2
D 4
		ws = tp->t_winsize;
E 4
E 2
		ttinit(tp);
I 2
D 4
		tp->t_winsize = ws;
E 4
E 2
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

I 4
	ws = tp->t_winsize;	/* preserve window size for reopens */
E 4
	(*linesw[tp->t_line].l_open)(tp, flag);
I 4
	tp->t_winsize = ws;
E 4
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
D 6
	if (dev >= PTMAX) {
E 6
I 6
	if (dev >= ptmax) {
E 6
		u.u_error = ENXIO;
		return;
	}
D 6
	if (!PTinited)
		for (i = 0; i < cdevcnt; i++)
			if (cdevsw[i].d_open == PTcopen)
				cdevsw[i].d_select = PTcselect;
	PTinited = 1;
E 6
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
I 2
	bzero((caddr_t)&tp->t_winsize, sizeof(tp->t_winsize));
E 2
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
D 5
				break;
E 5
I 5
				goto comm;
E 5
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
D 3
				temp = tp->t_canq.c_cc;
E 3
I 3
				temp = ttnread(tp);
E 3
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
I 5
		comm:
E 5
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

D 6
	if (dev < 0 || dev >= PTMAX) {
E 6
I 6
	if (dev < 0 || dev >= ptmax) {
E 6
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
I 6
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
E 6
}
E 1
