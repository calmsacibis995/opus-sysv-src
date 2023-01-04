h47645
s 00006/00000/00962
d D 1.6 90/11/30 09:08:15 root 6 5
c fixed mapchan beep on bad characters
e
s 00265/00006/00697
d D 1.5 90/11/28 16:48:21 root 5 4
c added mapchan translations
e
s 00003/00000/00700
d D 1.4 90/04/18 11:54:53 root 4 3
c added constty check in ttclose
e
s 00002/00000/00698
d D 1.3 90/03/27 13:25:48 root 3 2
c generate SIGIO on asynchronous io
e
s 00003/00003/00695
d D 1.2 90/03/14 15:25:28 root 2 1
c comments
e
s 00698/00000/00000
d D 1.1 90/03/06 12:29:05 root 1 0
c date and time created 90/03/06 12:29:05 by root
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


/* #ident	"@(#)kern-port:io/tt1.c	10.4" */

/*
 * Line discipline 0
 * No Virtual Terminal Handling
 */

#include "sys/types.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/conf.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/psw.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "sys/termio.h"
#include "sys/sysinfo.h"
#include "sys/fcntl.h"
#include "sys/sysmacros.h"

extern char partab[];

/*
 * routine called on first teletype open.
 * establishes a process group for distribution
 * of quits and interrupts from the tty.
 */
ttopen(tp, flag)
register struct tty *tp;
{
	register struct proc *pp;

	pp = u.u_procp;
	if ((pp->p_pid == pp->p_pgrp)
	 && ((flag & O_NOCTTY) == 0)
	 && (u.u_ttyp == NULL)
	 && (tp->t_pgrp == 0)) {
		u.u_ttyp = &tp->t_pgrp;
		tp->t_pgrp = pp->p_pgrp;
	}
	ttioctl(tp, LDOPEN, 0, 0);
	intoff();
D 2
	tp->t_state |= ISOPEN;
	tp->t_state &= ~WOPEN;
E 2
I 2
	tp->t_state |= ISOPEN;	/* ATOMIC (intoff()) */
	tp->t_state &= ~WOPEN;	/* ATOMIC (intoff()) */
E 2
	inton();
	bzero((caddr_t)&tp->t_winsize, sizeof(tp->t_winsize));
}

ttclose(tp)
register struct tty *tp;
{
I 4
	extern struct tty *constty;	/* temporary virtual console */
	if (constty == tp)
		constty = NULL;
E 4
	if ((tp->t_state&ISOPEN) == 0)
		return;
	ATOMIC (tp->t_state &= ~ISOPEN);
	ttioctl(tp, LDCLOSE, 0, 0);
	tp->t_pgrp = 0;
	tp->t_xstate = 0;
}

/*
 * Called from device's read routine after it has
 * calculated the tty-structure given as argument.
 */
ttread(tp)
register struct tty *tp;
{
	register struct clist *tq;

	tq = &tp->t_canq;
	if (tq->c_cc == 0)
		canon(tp);
	while (u.u_count!=0 && u.u_error==0) {
		if (u.u_count >= CLSIZE) {
			register n;
			register struct cblock *cp;

			if ((cp = getcb(tq)) == NULL)
				break;
			n = min(u.u_count, cp->c_last - cp->c_first);
			if (copyout(&cp->c_data[cp->c_first], u.u_base, n))
				u.u_error = EFAULT;
			putcf(cp);
			u.u_base += n;
			u.u_count -= n;
		} else {
			register c;

			if ((c = getc(tq)) < 0)
				break;
			if (subyte(u.u_base++, c))
				u.u_error = EFAULT;
			u.u_count--;
		}
	}
	if (tp->t_state&TBLOCK) {
		if (tp->t_rawq.c_cc<TTXOLO) {
			(*tp->t_proc)(tp, T_UNBLOCK);
		}
	}
}

/*
 * Called from device's write routine after it has
 * calculated the tty-structure given as argument.
 */
ttwrite(tp)
register struct tty *tp;
{
	register int	oldpri, ndone;

	if (!(tp->t_state&CARR_ON))
		return;
	ndone = 0;
	while (u.u_count) {
		if (tp->t_outq.c_cc > tthiwat[tp->t_cflag&CBAUD]) {
			oldpri = spltty();
			while (tp->t_outq.c_cc > tthiwat[tp->t_cflag&CBAUD]) {
				(*tp->t_proc)(tp, T_OUTPUT);
				if (u.u_fmode & FNONB) {
					if (ndone == 0)
						u.u_error = EAGAIN;
					splx(oldpri);
					return;
				}
D 2
				tp->t_state |= OASLP;
E 2
I 2
				tp->t_state |= OASLP;	/* ATOMIC (spl) */
E 2
				sleep((caddr_t)&tp->t_outq, TTOPRI);
			}
			splx(oldpri);
		}
D 5
		if (u.u_count >= (CLSIZE/2)) {
E 5
I 5
		if (tp->t_mctbl.mapping) {
			register c;

			if (tp->t_mctbl.output_state == MC_OUTPUT_STATE_IDLE) {
				c = fubyte(u.u_base++);
				if (c<0) {
					u.u_error = EFAULT;
					break;
				}
				c = mc_do_output(&tp->t_mctbl, c & 0x00ff);
				u.u_count--;
				ndone++;
				ttxput(tp, c, 0);
			} else {	/* MC_OUTPUT_STATE_BUSY */
				c = mc_do_output(&tp->t_mctbl, 0);
				ndone++;
				ttxput(tp, c, 0);
			}
		}
		else if (u.u_count >= (CLSIZE/2)) {
E 5
			register n;
			register struct cblock *cp;

			if ((cp = getcf()) == NULL)
				break;
			n = min(u.u_count, cp->c_last);
			if (copyin(u.u_base, cp->c_data, n)) {
				u.u_error = EFAULT;
				putcf(cp);
				break;
			}
			u.u_base += n;
			u.u_count -= n;
			cp->c_last = n;
			ndone += n;
			ttxput(tp, cp, n);
		} else {
			register c;

			c = fubyte(u.u_base++);
			if (c<0) {
				u.u_error = EFAULT;
				break;
			}
			u.u_count--;
			ndone++;
			ttxput(tp, c, 0);
		}
	}
out:
	oldpri = spltty();
	(*tp->t_proc)(tp, T_OUTPUT);
	splx(oldpri);
}

/*
 * Place a character on raw TTY input queue, putting in delimiters
 * and waking up top half as needed.
 * Also echo if required.
 */
ttin(tp, code)
register struct tty *tp;
{
	register c;
	register flg;
	register char *cp;
	ushort nchar, nc;

	if (code == L_BREAK) {
		signal(tp->t_pgrp, SIGINT);
		ttyflush(tp, (FREAD|FWRITE));
		return;
	}
	nchar = tp->t_rbuf.c_size - tp->t_rbuf.c_count;
	/* reinit rx control block */
	tp->t_rbuf.c_count = tp->t_rbuf.c_size;
	if (nchar==0)
		return;
	flg = tp->t_iflag;
	/* KMC does all but IXOFF */
	if (tp->t_state&EXTPROC)
		flg &= IXOFF;
	nc = nchar;
	cp = tp->t_rbuf.c_ptr;
D 5
	if (nc < cfreelist.c_size || (flg & (INLCR|IGNCR|ICRNL|IUCLC))) {
E 5
I 5
	if (tp->t_mctbl.mapping) {
E 5
			/* must do per character processing */
		for (;nc--; cp++) {
			c = *cp;
I 5
			if ((c = mc_do_input(tp, c & 0x00ff, 0)) == -1)
				continue;
E 5
			if (c == '\n' && flg&INLCR)
				*cp = c = '\r';
			else if (c == '\r')
				if (flg&IGNCR)
					continue;
				else if (flg&ICRNL)
					*cp = c = '\n';
			if (flg&IUCLC && 'A' <= c && c <= 'Z')
				c += 'a' - 'A';
			if (putc(c, &tp->t_rawq))
				continue;
			sysinfo.rawch++;
		}
		cp = tp->t_rbuf.c_ptr;
I 5
	} else if (nc < cfreelist.c_size || (flg & (INLCR|IGNCR|ICRNL|IUCLC))) {
			/* must do per character processing */
		for (;nc--; cp++) {
			c = *cp;
			if (c == '\n' && flg&INLCR)
				*cp = c = '\r';
			else if (c == '\r')
				if (flg&IGNCR)
					continue;
				else if (flg&ICRNL)
					*cp = c = '\n';
			if (flg&IUCLC && 'A' <= c && c <= 'Z')
				c += 'a' - 'A';
			if (putc(c, &tp->t_rawq))
				continue;
			sysinfo.rawch++;
		}
		cp = tp->t_rbuf.c_ptr;
E 5
	} else {
		/* may do block processing */
		putcb(CMATCH((struct cblock *)cp), &tp->t_rawq);
		sysinfo.rawch += nc;
		/* allocate new rx buffer */
		if ((tp->t_rbuf.c_ptr = getcf()->c_data)
			== ((struct cblock *)NULL)->c_data) {
			tp->t_rbuf.c_ptr = NULL;
			return;
		}
		tp->t_rbuf.c_count = cfreelist.c_size;
		tp->t_rbuf.c_size = cfreelist.c_size;
	}


	if (tp->t_rawq.c_cc > TTXOHI) {
		if (flg&IXOFF && !(tp->t_state&TBLOCK))
			(*tp->t_proc)(tp, T_BLOCK);
		if (tp->t_rawq.c_cc > TTYHOG) {
			ttyflush(tp, FREAD);
			return;
		}
	}
	flg = lobyte(tp->t_lflag);
	if (flg) while (nchar--) {
		c = *cp++;
I 5
		if (tp->t_mctbl.mapping)
			if ((c = mc_do_input(tp, c & 0x00ff, 1)) == -1)
				continue;
E 5
		if (flg&ISIG) {
			if (c == tp->t_cc[VINTR]) {
				signal(tp->t_pgrp, SIGINT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				continue;
			}
			if (c == tp->t_cc[VQUIT]) {
				signal(tp->t_pgrp, SIGQUIT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				continue;
			}
			if (c == tp->t_cc[VSWTCH]) {
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				(*tp->t_proc)(tp, T_SWTCH);
				continue;
			}
		}
		if (flg&ICANON) {
			if (c == '\n') {
				if (flg&ECHONL)
					flg |= ECHO;
				tp->t_delct++;
			} else if (c == tp->t_cc[VEOL] || c == tp->t_cc[VEOL2])
				tp->t_delct++;
			if (!(tp->t_state&CLESC)) {
				if (c == '\\')
					tp->t_state |= CLESC;
				if (c == tp->t_cc[VERASE] && flg&ECHOE) {
					if (flg&ECHO)
D 5
						ttxput(tp, '\b', 0);
E 5
I 5
						if (tp->t_mctbl.mapping)
							mc_do_echo(tp,c);
						else
							ttxput(tp, '\b', 0);
E 5
					flg |= ECHO;
D 5
					ttxput(tp, ' ', 0);
E 5
I 5
					if (tp->t_mctbl.mapping)
						mc_do_echo(tp, ' ');
					else
						ttxput(tp, ' ', 0);
E 5
					c = '\b';
				} else if (c == tp->t_cc[VKILL] && flg&ECHOK) {
					if (flg&ECHO)
D 5
						ttxput(tp, c, 0);
E 5
I 5
						if (tp->t_mctbl.mapping)
							mc_do_echo(tp, c);
						else
							ttxput(tp, c, 0);
E 5
					flg |= ECHO;
					c = '\n';
				} else if (c == tp->t_cc[VEOF]) {
					flg &= ~ECHO;
					tp->t_delct++;
				}
			} else {
				if (c != '\\' || (flg&XCASE))
					tp->t_state &= ~CLESC;
			}
		}
		if (flg&ECHO) {
D 5
			ttxput(tp, c, 0);
E 5
I 5
			if (tp->t_mctbl.mapping)
				mc_do_echo(tp, c);
			else
				ttxput(tp, c, 0);
E 5
			(*tp->t_proc)(tp, T_OUTPUT);
		}
	}
	if (!(flg&ICANON)) {
		tp->t_state &= ~RTO;
		if (tp->t_rawq.c_cc >= tp->t_cc[VMIN])
			tp->t_delct = 1;
		else if (tp->t_cc[VTIME]) {
			if (!(tp->t_state&TACT))
				tttimeo(tp);
		}
	}
	if (tp->t_delct && (tp->t_state&IASLP)) {
		tp->t_state &= ~IASLP;
		wakeup((caddr_t)&tp->t_rawq);
	}
	if (tp->t_delct && tp->t_rsel)
		ttwakeup(tp, TS_RCOLL);
I 3
	if (tp->t_delct && (tp->t_xstate & TS_ASYNC))
		signal(tp->t_pgrp, SIGIO);
E 3
}

/*
 * Put character(s) on TTY output queue, adding delays,
 * expanding tabs, and handling the CR/NL bit.
 * It is called both from the base level for output, and from
 * interrupt level for echoing.
 */
#ifdef m88k
ttxput(tp, ul, ncode)
register struct tty *tp;
#else
ttxput(tp, ucp, ncode)
register struct tty *tp;
union {
	ushort	ch;
	struct cblock *ptr;
} ucp;
#endif
{
	register c;
	register flg;
	register unsigned char *cp;
	register char *colp;
	int ctype;
	int cs;
	struct cblock *scf;
#ifdef m88k
	union {
		ushort	ch;
		struct cblock *ptr;
	} ucp;
	ucp.ptr = (struct cblock *) ul;
#endif
	if (tp->t_state&EXTPROC) {
		if (tp->t_lflag&XCASE)
			flg = OPOST;
		else
			flg = 0;
	} else
		flg = tp->t_oflag;
	if (ncode == 0) {
		ncode++;
#ifdef	m88k
		cp = (unsigned char *)&ul + 3; /* char is in least significant byte of first param */
#endif
#if	u3b2 || u3b5 || u3b15
		cp = (unsigned char *)&ucp.ch + 3; /* char is in least significant byte of first param */
#endif
#ifdef	ns32000
		cp = (unsigned char *)&ucp.ch;
#endif
		if (!(flg&OPOST)) {
			sysinfo.outch++;
			c = *cp;
			putc(c, &tp->t_outq);
			return;
		}
		scf = NULL;
	} else {
		if (!(flg&OPOST)) {
			sysinfo.outch += ncode;
			putcb(ucp.ptr, &tp->t_outq);
			return;
		}
		cp = (unsigned char *)&ucp.ptr->c_data[ucp.ptr->c_first];
		scf = ucp.ptr;
	}
	while (ncode--) {
		c = *cp++;
		if (c >= 0200 && !(tp->t_state&EXTPROC)) {
	/* spl5-0 */
			if (c == QESC)
				putc(QESC, &tp->t_outq);
			sysinfo.outch++;
			putc(c, &tp->t_outq);
			continue;
		}
		/*
		 * Generate escapes for upper-case-only terminals.
		 */
		if (tp->t_lflag&XCASE) {
			colp = "({)}!|^~'`\\\0";
			while (*colp++)
				if (c == *colp++) {
					ttxput(tp, '\\', 0);
					c = colp[-2];
					break;
				}
			if ('A' <= c && c <= 'Z')
				ttxput(tp, '\\', 0);
		}
		if (flg&OLCUC && 'a' <= c && c <= 'z')
			c += 'A' - 'a';
		cs = c;
		/*
		 * Calculate delays.
		 * The numbers here represent clock ticks
		 * and are not necessarily optimal for all terminals.
		 * The delays are indicated by characters above 0200.
		 */
		ctype = partab[c];
		colp = &tp->t_col;
		c = 0;
		switch (ctype&077) {

		case 0:	/* ordinary */
			(*colp)++;

		case 1:	/* non-printing */
			break;

		case 2:	/* backspace */
			if (flg&BSDLY)
				c = 2;
			if (*colp)
				(*colp)--;
			break;

		case 3:	/* line feed */
			if (flg&ONLRET)
				goto cr;
			if (flg&ONLCR) {
				if (!(flg&ONOCR && *colp==0)) {
					sysinfo.outch++;
					putc('\r', &tp->t_outq);
				}
				goto cr;
			}
		nl:
			if (flg&NLDLY)
				c = 5;
			break;

		case 4:	/* tab */
			c = 8 - ((*colp)&07);
			*colp += c;
			ctype = flg&TABDLY;
			if (ctype == TAB0) {
				c = 0;
			} else if (ctype == TAB1) {
				if (c < 5)
					c = 0;
			} else if (ctype == TAB2) {
				c = 2;
			} else if (ctype == TAB3) {
				sysinfo.outch += c;
				do
					putc(' ', &tp->t_outq);
				while (--c);
				continue;
			}
			break;

		case 5:	/* vertical tab */
			if (flg&VTDLY)
				c = 0177;
			break;

		case 6:	/* carriage return */
			if (flg&OCRNL) {
				cs = '\n';
				goto nl;
			}
			if (flg&ONOCR && *colp == 0)
				continue;
		cr:
			ctype = flg&CRDLY;
			if (ctype == CR1) {
				if (*colp)
					c = max((*colp>>4) + 3, 6);
			} else if (ctype == CR2) {
				c = 5;
			} else if (ctype == CR3) {
				c = 9;
			}
			*colp = 0;
			break;

		case 7:	/* form feed */
			if (flg&FFDLY)
				c = 0177;
			break;
		}
		sysinfo.outch++;
		putc(cs, &tp->t_outq);
		if (c) {
			if ((c < 32) && flg&OFILL) {
				if (flg&OFDEL)
					cs = 0177;
				else
					cs = 0;
				putc(cs, &tp->t_outq);
				if (c > 3)
					putc(cs, &tp->t_outq);
			} else {
				putc(QESC, &tp->t_outq);
				putc(c|0200, &tp->t_outq);
			}
		}

	}
	if (scf != NULL)
		putcf(scf);
}

/*
 * Get next packet from output queue.
 * Called from xmit interrupt complete.
 */

ttout(tp)
register struct tty *tp;
{
	register struct ccblock *tbuf;
	register c;
	register char *cptr;
	register retval;
	extern ttrstrt();

	if (tp->t_state&TTIOW && tp->t_outq.c_cc==0) {
		tp->t_state &= ~TTIOW;
		wakeup((caddr_t)&tp->t_oflag);
	}

delay:
	tbuf = &tp->t_tbuf;
	if (hibyte(tp->t_lflag)) {
		if (tbuf->c_ptr) {
			putcf(CMATCH((struct cblock *)tbuf->c_ptr));
			tbuf->c_ptr = NULL;
		}
		tp->t_state |= TIMEOUT;
		timeout(ttrstrt, tp, (hibyte(tp->t_lflag)&0177)+6);
		hibyte(tp->t_lflag) = 0;
		return(0);
	}
	retval = 0;

	if ((tp->t_state&EXTPROC) || (!(tp->t_oflag&OPOST))) {
		if (tbuf->c_ptr)
			putcf(CMATCH((struct cblock *)tbuf->c_ptr));
		if ((tbuf->c_ptr = (char *)getcb(&tp->t_outq)) == NULL)
			return(0);
		tbuf->c_count = ((struct cblock *)tbuf->c_ptr)->c_last -
				((struct cblock *)tbuf->c_ptr)->c_first;
		tbuf->c_size = tbuf->c_count;
		tbuf->c_ptr = &((struct cblock *)tbuf->c_ptr)->c_data
				[((struct cblock *)tbuf->c_ptr)->c_first];
		retval = CPRES;
	} else {		/* watch for timing	*/
		if (tbuf->c_ptr == NULL) {
			if ((tbuf->c_ptr = getcf()->c_data)
				== ((struct cblock *)NULL)->c_data) {
				tbuf->c_ptr = NULL;
				return(0);	/* Add restart? */
			}
		}
		tbuf->c_count = 0;
		cptr = tbuf->c_ptr;
		while ((c=getc(&tp->t_outq)) >= 0) {
			if (c == QESC) {
				if ((c = getc(&tp->t_outq)) < 0)
					break;
				if (c > 0200) {
					hibyte(tp->t_lflag) = c;
					if (!retval)
						goto delay;
					break;
				}
			}
			retval = CPRES;
			*cptr++ = c;
			tbuf->c_count++;
			if (tbuf->c_count >= cfreelist.c_size)
				break;
		}
		tbuf->c_size = tbuf->c_count;
	}

out:
	if (tp->t_state&OASLP &&
		tp->t_outq.c_cc<=ttlowat[tp->t_cflag&CBAUD]) {
		tp->t_state &= ~OASLP;
		wakeup((caddr_t)&tp->t_outq);
		if (tp->t_wsel)
			ttwakeup(tp, TS_WCOLL);
	}
	return(retval);
}

tttimeo(tp)
register struct tty *tp;
{
	ATOMIC (tp->t_state &= ~TACT);
	if (tp->t_lflag&ICANON || tp->t_cc[VTIME] == 0)
		return;
	if (tp->t_rawq.c_cc == 0 && tp->t_cc[VMIN])
		return;
	if (tp->t_state&RTO) {
		tp->t_delct = 1;
		if (tp->t_state&IASLP) {
			ATOMIC (tp->t_state &= ~IASLP);
			wakeup((caddr_t)&tp->t_rawq);
		}
		if (tp->t_rsel)
			ttwakeup(tp, TS_RCOLL);
	} else {
		ATOMIC (tp->t_state |= RTO|TACT);
		timeout(tttimeo, tp, tp->t_cc[VTIME]*(HZ/10));
	}
}

/*
 * I/O control interface
 */
ttioctl(tp, cmd, arg, mode)
register struct tty *tp;
{
	ushort	chg;
	register int	oldpri;

	switch (cmd) {
	case LDOPEN:
		if (tp->t_rbuf.c_ptr == NULL) {
			/* allocate RX buffer */
			while ((tp->t_rbuf.c_ptr = getcf()->c_data)
				== ((struct cblock *)NULL)->c_data) {
				tp->t_rbuf.c_ptr = NULL;
				cfreelist.c_flag = 1;
				sleep(&cfreelist, TTOPRI);
			}
			tp->t_rbuf.c_count = cfreelist.c_size;
			tp->t_rbuf.c_size  = cfreelist.c_size;
			(*tp->t_proc)(tp, T_INPUT);
		}
		break;

	case LDCLOSE:
		oldpri = spltty();
		(*tp->t_proc)(tp, T_RESUME);
		splx(oldpri);
		ttywait(tp);
		ttyflush(tp, FREAD);
		if (tp->t_tbuf.c_ptr) {
			putcf(CMATCH((struct cblock *)tp->t_tbuf.c_ptr));
			tp->t_tbuf.c_ptr = NULL;
		}
		if (tp->t_rbuf.c_ptr && freerbuf(tp)) {
			putcf(CMATCH((struct cblock *)tp->t_rbuf.c_ptr));
			tp->t_rbuf.c_ptr = NULL;
		}
		break;

	case LDCHG:
		chg = tp->t_lflag^arg;
		if (!(chg&ICANON))
			break;
		oldpri = spltty();
		if (tp->t_canq.c_cc) {
			if (tp->t_rawq.c_cc) {
				tp->t_canq.c_cc += tp->t_rawq.c_cc;
				tp->t_canq.c_cl->c_next = tp->t_rawq.c_cf;
				tp->t_canq.c_cl = tp->t_rawq.c_cl;
			}
			tp->t_rawq = tp->t_canq;
			tp->t_canq = ttnulq;
		}
		tp->t_delct = tp->t_rawq.c_cc;
		splx(oldpri);
		break;

	default:
		break;
I 5
	}
}

static void
mc_do_beep(tp)
struct tty *tp;
{
	mc_do_echo(tp, '\a');
I 6
	/*
	 * Beep is called on bad character input, so the
	 * echo probably wont happen at the end of ttin.
	 * Fire off a driver request here.
	*/
	(*tp->t_proc)(tp, T_OUTPUT);
E 6
}

/* 
 * do_input
 *
 * return value:
 * 		-1 - throw character away
 *	   (0-255) - use character (may be translated)
*/
static int
mc_do_input(tp, c, echo)
struct tty *tp;
register unsigned c;
int echo;
{
	int i;
	struct mctbl *mctbl_p = &tp->t_mctbl;
	struct mapchan *m= mctbl_p->mapchan_p;
	struct mcinput *mci;
	struct mcdead *mcd;
	struct mcdeadkeys *mcdk;
	struct mccompose *mcc;

	/* all input chars must go through input translation table */
	for (i = m->hdr.ninput, mci = GET_INPUT_PTR(m); i != 0; i--, mci++) {
		if (mci->typed == c) {
			c = mci->xlate;
			break;
		}
	}

	switch (mctbl_p->input_state[echo]) {

	case MC_INPUT_STATE_IDLE:

		/* test for compose key */
		if (m->hdr.ncompose && m->hdr.compose_char == c) {
			mctbl_p->input_state[echo] = MC_INPUT_STATE_COMP;
			return (-1);
		}

		/* test for dead key */
		for (i = m->hdr.ndead, mcd = GET_DEAD_PTR(m); i != 0; i--) {
			if (mcd->deadkey == c) {
				mctbl_p->lastchar[echo] = c;
				mctbl_p->input_state[echo] = MC_INPUT_STATE_DEAD;
				return (-1);
			} else {
				mcd = (struct mcdead *) (((char *)mcd) +
				 sizeof(struct mcdead) + 
				 (sizeof(struct mcdeadkeys)*(mcd->count - 1)));
			}
		}

		return(c & 0x00ff);
		break;

	case MC_INPUT_STATE_DEAD:

		for (i = m->hdr.ndead, mcd = GET_DEAD_PTR(m); i != 0; i--) {
			if (mcd->deadkey == mctbl_p->lastchar[echo]) {
				break;
			} else {
				mcd = (struct mcdead *) (((char *)mcd) +
				 sizeof(struct mcdead) + 
				 (sizeof(struct mcdeadkeys)*(mcd->count - 1)));
			}
		}
#if MC_DEBUG
		if (!i) {
			printf("dead key table walk error, key = %c\n",
				mctbl_p->lastchar[echo]);
			mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
			return (-1);
		}
#endif
		for (i = mcd->count, mcdk = mcd->mcdeadkeys; i != 0; i--,mcdk++) {
			if (mcdk->typed == c) {
				mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
				return(mcdk->xlate & 0x00ff);
			}
		}

		/* if we got here, the second key was not in the dead table */
		mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
		if (m->hdr.beep)
			mc_do_beep(tp);
		return(-1);

		break;

	case MC_INPUT_STATE_COMP:

		for (i = m->hdr.ncompose, mcc = GET_COMPOSE_PTR(m); i != 0; i--, mcc++) {
			if (mcc->typed1 == c) {
				mctbl_p->lastchar[echo] = c;
				mctbl_p->input_state[echo] = MC_INPUT_STATE_CMP1;
				return(-1);
			}
		} 
		
		/* if we got here, the first key wasn't in the table */
		mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
		if (m->hdr.beep)
			mc_do_beep(tp);
		return(-1);

		break;

	case MC_INPUT_STATE_CMP1:

		for (i = m->hdr.ncompose, mcc = GET_COMPOSE_PTR(m); i != 0; i--, mcc++) {
			if (mcc->typed1 == mctbl_p->lastchar[echo] && mcc->typed2 == c) {
				mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
				return(mcc->xlate & 0x00ff);
			}
		} 
		
		/* if we got here, the second key wasn't in the table */
		mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
		if (m->hdr.beep)
			mc_do_beep(tp);
		return(-1);

		break;

	default:
		printf("mc_do_input: bad input_state = 0x%x\n",mctbl_p->input_state[echo]);
		mctbl_p->input_state[echo] = MC_INPUT_STATE_IDLE;
		return(-1);
	}
}

static int 
mc_do_echo(tp, c)
register struct tty *tp;
unsigned c;
{
	struct mctbl *mctbl_p;
	struct mapchan *m;
	struct mcoutput *mco;
	int i;

	mctbl_p = &tp->t_mctbl;
	m = mctbl_p->mapchan_p;
	c &= 0x00ff;
	for (i = m->hdr.noutput, mco = GET_OUTPUT_PTR(m); i != 0; i--) {
		if (mco->outchar == c) {
			char *p;

			p = mco->xlate;
			while (*p) {
				ttxput(tp, *p++, 0);
			}
			return;
		} else {
			mco = (struct mcoutput *) (((char *)mco) +
			 (sizeof(struct mcoutput) + strlen(mco->xlate)));
		}
	}
	ttxput(tp, c, 0);	
}

static int 
mc_do_output(mctbl_p,c)
struct mctbl *mctbl_p;
unsigned c;
{
	struct mapchan *m = mctbl_p->mapchan_p;
	struct mcoutput *mco;
	int i;

	if (mctbl_p->output_state == MC_OUTPUT_STATE_IDLE) {
		c &= 0x00ff;
		for (i = m->hdr.noutput, mco = GET_OUTPUT_PTR(m); i != 0; i--) {
			if (mco->outchar == c) {
				if (mco->xlate[1]) {
					mctbl_p->output_state = MC_OUTPUT_STATE_BUSY;
					mctbl_p->next_out_xlate = &mco->xlate[1];
				}
				return(mco->xlate[0] & 0x00ff);
			} else {
				mco = (struct mcoutput *) (((char *)mco) +
				 (sizeof(struct mcoutput) + strlen(mco->xlate)));
			}
		}
		return(c);
	} else {		/* MC_OUTPUT_STATE_BUSY */
		char *p;

		p = mctbl_p->next_out_xlate;
		if (p[1]) {
			mctbl_p->next_out_xlate = &p[1];
		} else {
			mctbl_p->output_state = MC_OUTPUT_STATE_IDLE;
		}
		return(*p & 0x00ff);
E 5
	}
}
E 1
