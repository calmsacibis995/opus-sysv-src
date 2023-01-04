h39559
s 00004/00004/00736
d D 1.5 91/04/11 18:01:12 root 5 4
c Expanded control chars in tty structure to Posix size.
e
s 00084/00000/00656
d D 1.4 90/11/28 16:47:40 root 4 3
c added mapchan ioctls
e
s 00001/00004/00655
d D 1.3 90/06/05 23:07:45 root 3 2
c TC_PX_GETPGRP returns ENOSYS
e
s 00003/00003/00656
d D 1.2 90/03/14 15:26:36 root 2 1
c comments
e
s 00659/00000/00000
d D 1.1 90/03/06 12:29:06 root 1 0
c date and time created 90/03/06 12:29:06 by root
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


/* #ident	"@(#)kern-port:io/tty.c	10.7" */

/*
 * general TTY subroutines
 */
#include "sys/types.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/psw.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/tty.h"
#include "sys/ttold.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/termio.h"
#ifdef BCS
#include "sys/termios.h"
#endif
#include "sys/sysinfo.h"
#include "sys/time.h"
#include "sys/sysmacros.h"

I 4
/* defined in as.c */
extern struct mapchan *mc_array;
extern int mc_max;

E 4
/*
 * tty low and high water marks
 * high < TTYHOG
 */
int	tthiwat[16] = {
	0, 60, 60, 60,
	60, 60, 60, 120,
	120, 180, 180, 240,
	240, 240, 100, 100,
};
int	ttlowat[16] = {
	0, 20, 20, 20,
	20, 20, 20, 40,
	40, 60, 60, 80,
	80, 80, 50, 50,
};

D 5
char	ttcchar[NCC] = {
E 5
I 5
char	ttcchar[NCCS] = {
E 5
	CINTR,
	CQUIT,
	CERASE,
	CKILL,
	CEOF,
	0,
	0,
	0
};

#define	MTCGETA		_IOR(T,1,struct termio)
#define	MTCSETA		_IOW(T,2,struct termio)
#define	MTCSETAW	_IOW(T,3,struct termio)
#define	MTCSETAF	_IOW(T,4,struct termio)

/* null clist header */
struct clist ttnulq;

/* canon buffer */
char	canonb[CANBSIZ];
/*
 * Input mapping table-- if an entry is non-zero, when the
 * corresponding character is typed preceded by "\" the escape
 * sequence is replaced by the table value.  Mostly used for
 * upper-case only terminals.
 */
char	maptab[] = {
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,'|',000,000,000,000,000,'`',
	'{','}',000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,'~',000,
	000,'A','B','C','D','E','F','G',
	'H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W',
	'X','Y','Z',000,000,000,000,000,
};

/*
 * common ioctl tty code
 */
ttiocom(tp, cmd, arg, mode)
register struct tty *tp;
{
	register flag;
	struct termio cb;
	struct sgttyb tb;
I 4
	struct mapchan mc;
E 4

#ifdef BCS
	switch(cmd) {
		case TC_PX_GETATTR:
		case TC_PX_SETATTR:
		case TC_PX_SETATTRD:
		case TC_PX_SETATTRF:
		case TC_PX_DRAIN:
		case TC_PX_GETPGRP:
		case TC_PX_SETPGRP:
		case TIOCSWINSZ:
		case TIOCGWINSZ:
			return(pxttiocom(tp, cmd, arg, mode));

		default:
			break;
	}
#endif

	switch (cmd) {
	case IOCTYPE:
		u.u_rval1 = TIOC;
		break;

	case TCSETAW:
	case TCSETAF:
	case MTCSETAW:
	case MTCSETAF:
		ttywait(tp);
		if (cmd == TCSETAF)
			ttyflush(tp, (FREAD|FWRITE));
		if (cmd == MTCSETAF)
			ttyflush(tp, (FREAD|FWRITE));
	case TCSETA:
	case MTCSETA:
		if (copyin(arg, &cb, sizeof cb)) {
			u.u_error = EFAULT;
			break;
		}
		if (tp->t_line != cb.c_line) {
			if (cb.c_line < 0 || cb.c_line >= linecnt) {
				u.u_error = EINVAL;
				break;
			}
			(*linesw[tp->t_line].l_ioctl)(tp, LDCLOSE, 0, mode);
		}
		flag = tp->t_lflag;
		tp->t_iflag = cb.c_iflag;
		tp->t_oflag = cb.c_oflag;
		tp->t_cflag = cb.c_cflag;
		tp->t_lflag = cb.c_lflag;
		bcopy(cb.c_cc, tp->t_cc, NCC);
		if (tp->t_line != cb.c_line) {
			tp->t_line = cb.c_line;
			(*linesw[tp->t_line].l_ioctl)(tp, LDOPEN, 0, mode);
		} else if (tp->t_lflag != flag) {
			(*linesw[tp->t_line].l_ioctl)(tp, LDCHG, flag, mode);
		}
		return(1);

	case TCGETA:
	case MTCGETA:
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;
		cb.c_cflag = tp->t_cflag;
		cb.c_lflag = tp->t_lflag;
		cb.c_line = tp->t_line;
		bcopy(tp->t_cc, cb.c_cc, NCC);
		if (copyout(&cb, arg, sizeof cb))
			u.u_error = EFAULT;
		break;

	case TCSBRK:
	case TC_PX_BREAK:
		ttywait(tp);
		if ((tp->t_brktime = arg) == 0)
			(*tp->t_proc)(tp, T_BREAK);
		else {
			if (cmd == TC_PX_BREAK) {
				tp->t_brktime = timerround(arg);
				(*tp->t_proc)(tp, T_BREAK);
			}
		}
		break;

	case TCXONC:
	case TC_PX_FLOW:
		switch (arg) {
		case 0:
			(*tp->t_proc)(tp, T_SUSPEND);
			break;
		case 1:
			(*tp->t_proc)(tp, T_RESUME);
			break;
		case 2:
			(*tp->t_proc)(tp, cmd == TCXONC ? T_BLOCK:T_PXBLOCK);
			break;
		case 3:
			(*tp->t_proc)(tp, cmd == TCXONC ? T_UNBLOCK:T_PXUNBLOCK);
			break;
		default:
			u.u_error = EINVAL;
		}
		break;

	case TCFLSH:
	case TC_PX_FLUSH:
		switch (arg) {
		case 0:
		case 1:
		case 2:
			ttyflush(tp, (arg - FOPEN)&(FREAD|FWRITE));
			break;

		default:
			u.u_error = EINVAL;
		}
		break;

/* conversion aide only */
	case TIOCSETP:
		ttywait(tp);
		ttyflush(tp, (FREAD|FWRITE));
		if (copyin(arg, &tb, sizeof(tb))) {
			u.u_error = EFAULT;
			break;
		}
		tp->t_iflag = 0;
		tp->t_oflag = 0;
		tp->t_lflag = 0;
		tp->t_cflag = (tb.sg_ispeed&CBAUD)|CREAD;
		if ((tb.sg_ispeed&CBAUD)==B110)
			tp->t_cflag |= CSTOPB;
		if (tb.sg_erase != -1)
			tp->t_cc[VERASE] = tb.sg_erase;
		if (tb.sg_kill != -1)
			tp->t_cc[VKILL] = tb.sg_kill;
		flag = tb.sg_flags;
		if (flag&O_HUPCL)
			tp->t_cflag |= HUPCL;
		if (flag&O_XTABS)
			tp->t_oflag |= TAB3;
		else if (flag&O_TBDELAY)
			tp->t_oflag |= TAB1;
		if (flag&O_LCASE) {
			tp->t_iflag |= IUCLC;
			tp->t_oflag |= OLCUC;
			tp->t_lflag |= XCASE;
		}
		if (flag&O_ECHO)
			tp->t_lflag |= (ECHO|ECHOE);
		if (!(flag&O_NOAL))
			tp->t_lflag |= ECHOK;
		if (flag&O_CRMOD) {
			tp->t_iflag |= ICRNL;
			tp->t_oflag |= ONLCR;
			if (flag&O_CR1)
				tp->t_oflag |= CR1;
			if (flag&O_CR2)
				tp->t_oflag |= ONOCR|CR2;
		} else {
			tp->t_oflag |= ONLRET;
			if (flag&O_NL1)
				tp->t_oflag |= CR1;
			if (flag&O_NL2)
				tp->t_oflag |= CR2;
		}
		if (flag&O_RAW) {
			tp->t_cc[VTIME] = 1;
			tp->t_cc[VMIN] = 1;
			tp->t_iflag &= ~(ICRNL|IUCLC);
			tp->t_cflag |= CS8;
		} else {
			tp->t_cc[VEOF] = CEOF;
			tp->t_cc[VEOL] = 0;
			tp->t_cc[VEOL2] = 0;
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON|IXANY;
			tp->t_oflag |= OPOST;
			tp->t_cflag |= CS7|PARENB;
			tp->t_lflag |= ICANON|ISIG;
		}
		tp->t_iflag |= INPCK;
		if (flag&O_ODDP)
			if (flag&O_EVENP)
				tp->t_iflag &= ~INPCK;
			else
				tp->t_cflag |= PARODD;
		if (flag&O_VTDELAY)
			tp->t_oflag |= FFDLY;
		if (flag&O_BSDELAY)
			tp->t_oflag |= BSDLY;
		return(1);

	case TIOCGETP:
		tb.sg_ispeed = tp->t_cflag&CBAUD;
		tb.sg_ospeed = tb.sg_ispeed;
		tb.sg_erase = tp->t_cc[VERASE];
		tb.sg_kill = tp->t_cc[VKILL];
		flag = 0;
		if (tp->t_cflag&HUPCL)
			flag |= O_HUPCL;
		if (!(tp->t_lflag&ICANON))
			flag |= O_RAW;
		if (tp->t_lflag&XCASE)
			flag |= O_LCASE;
		if (tp->t_lflag&ECHO)
			flag |= O_ECHO;
		if (!(tp->t_lflag&ECHOK))
			flag |= O_NOAL;
		if (tp->t_cflag&PARODD)
			flag |= O_ODDP;
		else if (tp->t_iflag&INPCK)
			flag |= O_EVENP;
		else
			flag |= O_ODDP|O_EVENP;
		if (tp->t_oflag&ONLCR) {
			flag |= O_CRMOD;
			if (tp->t_oflag&CR1)
				flag |= O_CR1;
			if (tp->t_oflag&CR2)
				flag |= O_CR2;
		} else {
			if (tp->t_oflag&CR1)
				flag |= O_NL1;
			if (tp->t_oflag&CR2)
				flag |= O_NL2;
		}
		if ((tp->t_oflag&TABDLY)==TAB3)
			flag |= O_XTABS;
		else if (tp->t_oflag&TAB1)
			flag |= O_TBDELAY;
		if (tp->t_oflag&FFDLY)
			flag |= O_VTDELAY;
		if (tp->t_oflag&BSDLY)
			flag |= O_BSDELAY;
		tb.sg_flags = flag;
		if (copyout(&tb, arg, sizeof(tb)))
			u.u_error = EFAULT;
I 4
		break;

	case TCGETMC:
		if (!tp->t_mctbl.mapping) {
			u.u_error = EINVAL;
			break;
		}
		if (copyout(tp->t_mctbl.mapchan_p, arg, sizeof mc))
			u.u_error = EFAULT;
		break;

	case TCSETMC:
		{
		int i;
	
		if (u.u_procp->p_pgrp != tp->t_pgrp && !suser()) {
			u.u_error = EPERM;
			break;
		}
		if (tp->t_mctbl.mapping) {
			u.u_error = EEXIST;
			break;
		}

		if (copyin(arg, &mc, sizeof mc)) {
			u.u_error = EFAULT;
			break;
		}

		for (i=0; i<mc_max; i++) {
			int j;
			int *ip1, *ip2;

			if (!mc_array[i].hdr.refcount)
				continue;
			ip1 = &mc_array[i].hdr.version;
			ip2 = &mc.hdr.version;
			for (j = (sizeof(mc) - sizeof(mc.hdr.refcount)) / sizeof(int); j; j--)
				if (*ip1++ != *ip2++)
					break;
			if (j == 0)
				break;
		}
		if (i == mc_max) {
			for (i=0; i<mc_max; i++) {
				if (!mc_array[i].hdr.refcount)
					break;
			}
		}

		/* need array check here */

		if (i < mc_max) {
			if (!mc_array[i].hdr.refcount)
				bcopy(&mc, &mc_array[i], sizeof (mc));
			tp->t_mctbl.mapchan_p = &mc_array[i];
			mc_array[i].hdr.refcount++;
			tp->t_mctbl.output_state = MC_OUTPUT_STATE_IDLE;
			tp->t_mctbl.input_state[0] = MC_INPUT_STATE_IDLE;
			tp->t_mctbl.input_state[1] = MC_INPUT_STATE_IDLE;
			tp->t_mctbl.mapping = 1;
		} else {
			u.u_error = ENOSPC;
		}
		}
		break;

	case TCSTOPMC:
		if (u.u_procp->p_pgrp != tp->t_pgrp && !suser()) {
			u.u_error = EPERM;
			break;
		}
		if (!tp->t_mctbl.mapping) {
			u.u_error = EINVAL;
		} else {
			tp->t_mctbl.mapping = 0;
			if (--tp->t_mctbl.mapchan_p->hdr.refcount < 0)
				printf("TCSTOPMC: refcount underflow\n");
		}
E 4
		break;

	default:
		if (bsdttiocom(tp, cmd, arg, mode))
			break;
		if ((cmd&IOCTYPE) == LDIOC)
			(*linesw[tp->t_line].l_ioctl)(tp, cmd, arg, mode);
		else
			u.u_error = EINVAL;
		break;
	}
	return(0);
}

#ifdef BCS
/* posix ttiocom */
pxttiocom(tp, cmd, arg, mode)
register struct tty *tp;
{
	register flag;
	struct termios cb;

	switch (cmd) {

	case TC_PX_SETATTRD: /* TCSETAW */
	case TC_PX_SETATTRF: /* TCSETAF */
		ttywait(tp);
		if (cmd == TC_PX_SETATTRF)
			ttyflush(tp, (FREAD|FWRITE));
	case TC_PX_SETATTR:  /* TCSETA */
		if (copyin(arg, &cb, sizeof cb)) {
			u.u_error = EFAULT;
			break;
		}
		if (tp->t_line != cb.c_line) {
			if (cb.c_line < 0 || cb.c_line >= linecnt) {
				u.u_error = EINVAL;
				break;
			}
			(*linesw[tp->t_line].l_ioctl)(tp, LDCLOSE, 0, mode);
		}

		flag = (cb.c_cflag >> 16) & 0xf; /* use output baud rate */
		flag |= (cb.c_cflag & 0xfff0);
		cb.c_cflag = flag;

		flag = tp->t_lflag;
		tp->t_iflag = cb.c_iflag;
		tp->t_oflag = cb.c_oflag;
		tp->t_cflag = cb.c_cflag;
		tp->t_lflag = cb.c_lflag;
D 5
		bcopy(cb.c_cc, tp->t_cc, NCC);
E 5
I 5
		bcopy(cb.c_cc, tp->t_cc, NCCS);
E 5
		if (tp->t_line != cb.c_line) {
			tp->t_line = cb.c_line;
			(*linesw[tp->t_line].l_ioctl)(tp, LDOPEN, 0, mode);
		} else if (tp->t_lflag != flag) {
			(*linesw[tp->t_line].l_ioctl)(tp, LDCHG, flag, mode);
		}
		return(1);

	case TC_PX_GETATTR: /* TCGETA */
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;

		flag = tp->t_cflag;
		flag &= 0xf;
		flag = (flag << 16) | (flag << 24);
		cb.c_cflag = (tp->t_cflag & 0xfff0);
		cb.c_cflag |= flag;

		cb.c_lflag = tp->t_lflag;
		cb.c_line = tp->t_line;
D 5
		bcopy(tp->t_cc, cb.c_cc, NCC);
E 5
I 5
		bcopy(tp->t_cc, cb.c_cc, NCCS);
E 5
		cb.c_cc[VSTART] = CSTART;	/* read only fields */
		cb.c_cc[VSTOP] = CSTOP;
		if (copyout(&cb, arg, sizeof cb))
			u.u_error = EFAULT;
		break;

	case TC_PX_DRAIN:
		ttywait(tp);
		break;

I 3
	case TC_PX_GETPGRP:
E 3
	case TC_PX_SETPGRP:
		u.u_error = ENOSYS;
D 3
		break;

	case TC_PX_GETPGRP:
		u.u_rval1 = tp->t_pgrp;
E 3
		break;

	case TIOCSWINSZ:
	case TIOCGWINSZ:
		bsdttiocom(tp, cmd, arg, mode);
		break;

	default:
		u.u_error = EINVAL;
		break;
	}
	return(0);
}
#endif

ttinit(tp)
register struct tty *tp;
{
	tp->t_line = 0;
	tp->t_iflag = 0;
	tp->t_oflag = 0;
	tp->t_cflag = SSPEED|CS8|CREAD|HUPCL;
	tp->t_lflag = 0;
D 5
	bcopy(ttcchar, tp->t_cc, NCC);
E 5
I 5
	bcopy(ttcchar, tp->t_cc, NCCS);
E 5
}

ttywait(tp)
register struct tty *tp;
{
	register int	oldpri;
	static	int	rate[] =
	{
		HZ+1 ,	/* avoid divide-by-zero, as well as unnecessary delay */
		50 ,
		75 ,
		110 ,
		134 ,
		150 ,
		200 ,
		300 ,
		600 ,
		1200 ,
		1800 ,
		2400 ,
		4800 ,
		9600 ,
		19200 ,
		38400 ,
	} ;

	oldpri = spltty();
	while (tp->t_outq.c_cc || (tp->t_state&(BUSY|TIMEOUT))) {
D 2
		tp->t_state |= TTIOW;
E 2
I 2
		tp->t_state |= TTIOW;	/* ATOMIC (spl) */
E 2
		sleep((caddr_t)&tp->t_oflag, TTOPRI);
	}
	splx(oldpri);
				/* delay 11 bit times to allow uart to empty */
				/* add one to allow for truncation */
				/* add one to allow for partial clock tick */
	if (usedelaytable()) {
		delay(1+1+11*HZ/rate[tp->t_cflag&CBAUD]) ;
	}
	else {
		delay(HZ/15);
		(*tp->t_proc)(tp, T_DRAINOUT);
	}
}

/*
 * flush TTY queues
 */
ttyflush(tp, cmd)
register struct tty *tp;
{
	register struct cblock *cp;
	register s;

	if (cmd&FWRITE) {
		while ((cp = getcb(&tp->t_outq)) != NULL)
			putcf(cp);
		(*tp->t_proc)(tp, T_WFLUSH);
		if (tp->t_state&OASLP) {
			ATOMIC (tp->t_state &= ~OASLP);
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_wsel)
			ttwakeup(tp, TS_WCOLL);
		if (tp->t_state&TTIOW) {
			ATOMIC (tp->t_state &= ~TTIOW);
			wakeup((caddr_t)&tp->t_oflag);
		}
	}
	if (cmd&FREAD) {
		while ((cp = getcb(&tp->t_canq)) != NULL)
			putcf(cp);
		s = spltty();
		while ((cp = getcb(&tp->t_rawq)) != NULL)
			putcf(cp);
		tp->t_delct = 0;
		splx(s);
		(*tp->t_proc)(tp, T_RFLUSH);
		if (tp->t_state&IASLP) {
			ATOMIC (tp->t_state &= ~IASLP);
			wakeup((caddr_t)&tp->t_rawq);
		}
		if (tp->t_rsel)
			ttwakeup(tp, TS_RCOLL);
	}
}

/*
 * Transfer raw input list to canonical list,
 * doing erase-kill processing and handling escapes.
 */
canon(tp)
register struct tty *tp;
{
	register char *bp;
	register c, esc;
	register int	oldpri;
	register struct cblock *cb;

	oldpri = spltty();
	/* If the character count on the raw queue is 0, make
	   the delimeter count 0. */
	if (tp->t_rawq.c_cc == 0)
		tp->t_delct = 0;
	/*
	  If we don't have any delimeters in the raw queue (t_delct==0),
	  we can't do any canonical processing.
	*/
	while (tp->t_delct == 0) {
		/* If we have no carrier, or NO DELAY was specified, just 
		   restore the execution priority and return (0). */
		if (!(tp->t_state&CARR_ON) || (u.u_fmode&FNDELAY)) {
			splx(oldpri);
			return;
		}
		if (u.u_fmode & FNONB) {
			u.u_error = EAGAIN;
			splx(oldpri);
			return;
		}
		if (tp->t_xstate&TS_NONBIO) {
			u.u_error = ewouldblock();
			splx(oldpri);
			return;
		}
		if (!(tp->t_lflag&ICANON) && tp->t_cc[VMIN]==0) {
			if (tp->t_cc[VTIME]==0)
				break;
D 2
			tp->t_state &= ~RTO;
E 2
I 2
			tp->t_state &= ~RTO;	/* ATOMIC (spl) */
E 2
			if (!(tp->t_state&TACT))
				tttimeo(tp);
		}
D 2
		tp->t_state |= IASLP;
E 2
I 2
		tp->t_state |= IASLP;	/* ATOMIC (spl) */
E 2
		sleep((caddr_t)&tp->t_rawq, TTIPRI);
	}
	if (!(tp->t_lflag&ICANON)) {
		while (cb = getcb(&tp->t_rawq))
			putcb(cb, &tp->t_canq);
		tp->t_delct = 0;
		splx(oldpri);
		return;
	}
	splx(oldpri);
	bp = canonb;
	esc = 0;
	while ((c=getc(&tp->t_rawq)) >= 0) {
		if (!esc) {
			if (c == '\\') {
				esc++;
			} else if (c == tp->t_cc[VERASE]) {
				if (bp > canonb)
					bp--;
				continue;
			} else if (c == tp->t_cc[VKILL]) {
				bp = canonb;
				continue;
			} else if (c == tp->t_cc[VEOF]) {
				break;
			}
		} else {
			esc = 0;
			if (c == tp->t_cc[VERASE] ||
			    c == tp->t_cc[VKILL] ||
			    c == tp->t_cc[VEOF])
				bp--;
			else if (tp->t_lflag&XCASE) {
				if ((c < 0200) && maptab[c]) {
					bp--;
					c = maptab[c];
				} else if (c == '\\')
					continue;
			} else if (c == '\\')
				esc++;
		}
		*bp++ = c;
		if (c == '\n' || c == tp->t_cc[VEOL] || c == tp->t_cc[VEOL2])
			break;
		if (bp >= &canonb[CANBSIZ])
			bp--;
	}
	tp->t_delct--;
	c = bp - canonb;
	sysinfo.canch += c;
	bp = canonb;
/* faster copy ? */
	while (c--)
		putc(*bp++, &tp->t_canq);
	return;
}

/*
 * Restart typewriter output following a delay timeout.
 * The name of the routine is passed to the timeout
 * subroutine and it is called during a clock interrupt.
 */
ttrstrt(tp)
register struct tty *tp;
{

	(*tp->t_proc)(tp, T_TIME);
}
E 1
