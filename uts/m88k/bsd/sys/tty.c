#include "param.h"
#include "systm.h"
#include "dir.h"
#include "user.h"
#include "proc.h"
#include "file.h"
#include "ioctl.h"
#include "termio.h"
#include "tty.h"
#include "ttold.h"
#include "var.h"

struct tchars {
	char	t_intrc;	/* interrupt */
	char	t_quitc;	/* quit */
	char	t_startc;	/* start output */
	char	t_stopc;	/* stop output */
	char	t_eofc;		/* end-of-file */
	char	t_brkc;		/* input delimiter (like nl) */
};
struct ltchars {
	char	t_suspc;	/* stop process signal */
	char	t_dsuspc;	/* delayed stop process signal */
	char	t_rprntc;	/* reprint line */
	char	t_flushc;	/* flush output (toggles) */
	char	t_werasc;	/* word erase */
	char	t_lnextc;	/* literal next character */
};

/* 
	called from ttiocom:
	return 0	if not a valid bsd ioctl call
	return 1	if valid
*/

struct ltchars ltchars = { -1, -1, -1, -1, -1, -1 };
int tiocwarn = 0;
struct proc *procfind();
extern struct tty *constty;		/* temporary virtual console */

bsdttiocom(tp, cmd, arg, mode)
register struct tty *tp;
caddr_t arg;
{
	int temp, retval;
	struct a {
		int	fdes;
		int	cmd;
		int	arg;
	} *uap;
	struct proc *p;
	register i;
	struct tchars tc;
	struct winsize newwin;

	retval = 1;		/* assume a bsd cmd */
	uap = (struct a *)u.u_ap;
	i = 0;
	switch (cmd) {
		case FIONBIO:
			i = TS_NONBIO;
		case FIOASYNC:
			if (i == 0)
				i = TS_ASYNC;
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			if (temp)
				tp->t_xstate |= i;
			else
				tp->t_xstate &= ~i;
			break;

		case FIONREAD:
			temp = ttnread(tp);
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case FIOGETOWN:
			temp = - (tp->t_pgrp);
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case FIOSETOWN:
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
				break;
			}
			if (temp > 0) {
				p = procfind(temp, 0);	/* search by pid */
				if (p == NULL) {
					u.u_error = ESRCH;
					break;
				}
				temp = p->p_pgrp;
			}
			else if (temp < 0) {
				temp = -temp;
				p = procfind(temp, 1);	/* search by pgrp */
				if (p == NULL) {
					u.u_error = ESRCH;
					break;
				}
			}
			tp->t_pgrp = temp;
			break;

		case TIOCSTART:
			i = spl7();
			(*tp->t_proc)(tp, T_RESUME);
			splx(i);
			break;

		case TIOCSTOP:
			i = spl7();
			(*tp->t_proc)(tp, T_SUSPEND);
			splx(i);
			break;

		case TIOCSPGRP:
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
				break;
			}
			if (procfind(temp, 1))
				tp->t_pgrp = temp;
			else
				u.u_error = EPERM;
			break;
	
		case TIOCGPGRP:
			temp = tp->t_pgrp;
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCGETC:
			tc.t_intrc = tp->t_cc[VINTR];
			tc.t_quitc = tp->t_cc[VQUIT];
			tc.t_startc = CSTART;
			tc.t_stopc = CSTOP;
			tc.t_eofc = tp->t_cc[VEOF];
			tc.t_brkc = -1;
			if (copyout(&tc, arg, sizeof(tc))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCSETC:
			if (copyin(arg, &tc, sizeof(tc))) {
				u.u_error = EFAULT;
				break;
			}
			if (tc.t_intrc != -1)
				tp->t_cc[VINTR] = tc.t_intrc;
			if (tc.t_quitc != -1)
				tp->t_cc[VQUIT] = tc.t_quitc;
			if (tc.t_eofc != -1)
				tp->t_cc[VEOF] = tc.t_eofc;
			break;

		case TIOCGLTC:
			if (copyout(&ltchars, arg, sizeof(ltchars))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCNOTTY:
			/* setpgrp(2) code */
			p = u.u_procp;
			if (p->p_pgrp != p->p_pid) {
				u.u_ttyp = NULL;
				u.u_ttyip = NULL;
			}
			p->p_pgrp = p->p_pid;
			break;

		case TIOCFLUSH:
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
				break;
			}
			if (temp == 0)
				temp = FREAD|FWRITE;
			else
				temp &= (FREAD|FWRITE);
			ttyflush(tp, temp);
			break;

		case TIOCGETD:
			temp = tp->t_line;
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCHPCL:
			tp->t_cflag |= HUPCL;
			break;

		case TIOCOUTQ:
			temp = tp->t_outq.c_cc;
			if (copyout(&temp, arg, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCSWINSZ:
			if (copyin((caddr_t) arg, (caddr_t) &newwin,
					sizeof(struct winsize))) {
				u.u_error = EFAULT;
				break;
			}
			if (newwin.ws_row != tp->t_winsize.ws_row ||
				newwin.ws_col != tp->t_winsize.ws_col ||
				newwin.ws_xpixel != tp->t_winsize.ws_xpixel ||
				newwin.ws_ypixel != tp->t_winsize.ws_ypixel) {
				tp->t_winsize = newwin;
				signal(tp->t_pgrp, SIGWINCH);
			}
			break;

		case TIOCGWINSZ:
			if (copyout((caddr_t) &tp->t_winsize, (caddr_t) arg, 
					sizeof(struct winsize))) {
				u.u_error = EFAULT;
			}
			break;

		case TIOCSETN:
			/* recursion */
			ttiocom(tp, TIOCSETP, arg, mode);
			break;

		case TIOCCONS:
			if (copyin(arg, &temp, sizeof(temp))) {
				u.u_error = EFAULT;
			}
			if (temp) {
				if (constty != NULL) {
					u.u_error = EBUSY;
					break;
				}
				if (!suser())
					break;
				constty = tp;
			} else if (tp == constty)
				constty = NULL;
			break;

		case TIOCSLTC:
		case TIOCSETD:
		case TIOCLSET:
		case TIOCLGET:
		case TIOCSTI:
		case TIOCLBIS:
		case TIOCLBIC:
			if (tiocwarn)
				printf("No TIOC(%x) support\n", cmd);
			break;

		default:
			retval = 0;
			break;
	}
	return(retval);
}

ttnread(tp)
register struct tty *tp;
{
	register nread = 0;
	register i;

	if (tp->t_delct && tp->t_rawq.c_cc && (tp->t_lflag & ICANON))
		canon(tp);
	nread = tp->t_canq.c_cc;
	if ((i = tp->t_rawq.c_cc) && ((tp->t_lflag&ICANON) == 0))
		if (tp->t_cc[VMIN]) {
			if (i >= tp->t_cc[VMIN])
				nread += i;
		}
		else
			nread += i;
	return (nread);
}

ttselect(tp, rw)
register struct tty *tp;
{
	int nread;
	int s = spl7();

	switch (rw) {

	case FREAD:
		nread = ttnread(tp);
		if (nread > 0)
			goto win;
		if (tp->t_rsel && tp->t_rsel->p_wchan == (caddr_t)&selwait)
			tp->t_xstate |= TS_RCOLL;
		else
			tp->t_rsel = u.u_procp;
		break;

	case FWRITE:
#if TS_WCOLL
#define	TTLOWAT(tp)	ttlowat[(tp)->t_cflag&CBAUD]
		if (tp->t_outq.c_cc <= TTLOWAT(tp))
			goto win;
		if (tp->t_wsel && tp->t_wsel->p_wchan == (caddr_t)&selwait)
			tp->t_state |= TS_WCOLL;
		else
			tp->t_wsel = u.u_procp;
		break;
#else
		goto win;
#endif
	}
	splx(s);
	return (0);
win:
	splx(s);
	return (1);
}

ttwakeup(tp, flag)
register struct tty *tp;
register flag;
{
	if (flag == TS_RCOLL && tp->t_rsel) {
		selwakeup (tp->t_rsel, tp->t_xstate&TS_RCOLL);
		tp->t_rsel = 0;
	}
	if (flag == TS_WCOLL && tp->t_wsel) {
		selwakeup (tp->t_wsel, tp->t_xstate&TS_WCOLL);
		tp->t_wsel = 0;
	}
	tp->t_xstate &= ~flag;
}

/*
pgrp == 0 : search by pid
pgrp == 1 : search by pgrp
*/
struct proc *
procfind(pidorpgrp, pgrp)
register pidorpgrp, pgrp;
{
	register struct proc *p, *ep;

	if (pidorpgrp == 0)
		return NULL;
	ep = (struct proc *) v.ve_proc;
	for (p = &proc[1]; p < ep; p++)
		if (pgrp) {
			if (p->p_pgrp == pidorpgrp)
				return(p);
		}
		else {
			if (p->p_pid == pidorpgrp)
				return(p);
		}

	return(NULL);
}

ewouldblock()
{
#if pm200
	u.u_error = EWOULDBLOCK;
#endif
#if m88k
	return((kdebug & 0x4000) == 0 ? EAGAIN : EDEADLK);
#endif
}
