h44644
s 00011/00001/00733
d D 1.5 90/11/28 16:46:26 root 5 4
c added mapchan support
e
s 00040/00008/00694
d D 1.4 90/06/20 02:18:27 root 4 3
c more debugging code
e
s 00021/00000/00681
d D 1.3 90/06/14 22:22:09 root 3 2
c tty trace logic, keyed off kdebug
e
s 00066/00156/00615
d D 1.2 90/03/14 15:24:19 root 2 1
c ioctls are now sequential.
e
s 00771/00000/00000
d D 1.1 90/03/06 12:28:34 root 1 0
c date and time created 90/03/06 12:28:34 by root
e
u
U
t
T
I 1
/* SID @(#)as.c	1.12 */

/*
 *	async input/output driver
 */
#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/fs/s5dir.h"
#include "sys/compat.h"
#include "sys/signal.h"
#include "sys/dev32k.h"
#include "sys/pc32k.h"
#include "sys/cb.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "sys/termio.h"
#include "sys/buf.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/sysinfo.h"
#include "sys/debug.h"
#include "sys/ascomm.h"
#include "sys/sysmacros.h"

I 5
/* for mapchan */
struct mapchan *mc_array;
int mc_max;

E 5
struct tty *as_tty;
int asmax;
struct tty *as_tp[ASMAX];	/* cache for tty pointers */
extern int ttrstrt();
extern char partab[];

int asproc();

D 2
#define ASFLAG_BUSY 0x1  /* a status update cmd is being sent to the pc */
#define ASFLAG_CTLWU 0x2  /* wakeup processes waiting control completion */
#define ASFLAG_LCL 0x4  /* the port is opened in local mode */
#define ASFLAG_AVAIL 0x8  /* the port is available for use */
#define ASFLAG_CD 0x10	/* last carrier detect status */
E 2
I 2
#define ASFLAG_WANT	0x1  /* wakeup processes waiting control startup */
#define ASFLAG_BUSY	0x2  /* wakeup processes waiting control completion */
#define ASFLAG_LCL	0x4  /* the port is opened in local mode */
#define ASFLAG_AVAIL	0x8  /* the port is available for use */
#define ASFLAG_CD	0x10	/* last carrier detect status */
#define ASFLAG_INTR	0x20	/* in interrupt service routine */
E 2

#define ON 1  /* set modem control lines to enable communications */
#define OFF 0  /* set modem control lines to disable communications */

#define LOCAL 0x80  /* set for 'local' type access to port */

I 2
#define ASQSIZE 5

E 2
struct as_info {
D 2
	uint update;  /* 1 bit for each part of state that needs update */
	ushort flags;  /* current state of control block */
	ushort iflag;  /* new iflags */
	ushort cflag;  /* new cflags */
	ushort modemcontrol;  /* modem control bits */
	ushort status;  /* latest async port status */
	struct io_serin *incb;  /* control block for incoming data */
	struct io_serout *outcb;  /* control block for outgoing data */
	struct io_serctl *ctlcb;  /* control block for control info */
	ulong  brktime;	/* microsecond duration of break */
} as_info[ASMAX];
E 2
I 2
	struct io_serin *incb;		/*  0: read IORB */
	struct io_serout *outcb;	/*  4: write IORB */
	struct io_serctl *ctlcb;	/*  8: control IORB */
	unsigned char flags;		/*  c: state info */
	unsigned char qget, qput;	/*  d: queue pointers */
	unsigned char cmd[ASQSIZE];	/*  f: ioctl cmd queue */
	ushort status;			/* 14: latest line status */
	unsigned short data[ASQSIZE];	/* 16: ioctl data queue */
} as_info[ASMAX];			/* 20: next as_info block */
I 3
D 4
#define	TRTPSYS(tp, s)	if (kdebug & 0x10000) asprstate(tp, s)
#define	TRTPINTR(tp, s)	if (kdebug & 0x20000) asprstate(tp, s)
E 4
I 4
#define	TRTPSYS(tp, s)	if (kdebug & 0x10000) asprstate(tp, s, 0)
#define	TRTPSYSA(tp, s, arg)	if (kdebug & 0x10000) asprstate(tp, s, arg)
#define	TRTPINTR(tp, s)	if (kdebug & 0x20000) asprstate(tp, s, 0)
#define TRTPDATA(tp, base, data) if (kdebug & 0x40000) asprdata(tp, base, data)
E 4
E 3
E 2

asopen(dev, flag)
minor_t dev;
int flag;
{
	register struct tty *tp;
	register struct io_serin *IORB;
	register int local;
	register struct as_info *cp;

	local = dev & LOCAL;
	dev &= ~LOCAL;
	cp = &as_info[dev];
	if (dev >= asmax || (cp->flags & ASFLAG_AVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	tp = as_tp[dev];
	IORB = cp->incb;
	if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
D 2
		cp->flags &= ~ASFLAG_CD;
E 2
I 2
		ATOMIC(cp->flags &= ~ASFLAG_CD);
E 2
		ttinit(tp);
		tp->t_proc = asproc;
		tp->t_minor = dev;
		asparam(dev);
	}
	if (tp->t_rbuf.c_ptr == NULL)	/* have this setup asin */
		ttioctl(tp, LDOPEN, 0, 0);
	spl7();
	if (IORB->io_status == S_IDLE) {
		asin(dev);
	}
	if ((tp->t_state&ISOPEN)==0 && local) {
D 2
		cp->flags |= ASFLAG_LCL;
E 2
I 2
		cp->flags |= ASFLAG_LCL;	/* ATOMIC (spl) */
E 2
	}
	asmodem(dev, ON);
D 2
	aswaitctl(dev);  /* wait for completion */
E 2
	if (tp->t_cflag&CLOCAL || cp->status&ASTAT_CD || local) {
D 2
		tp->t_state |= CARR_ON;
E 2
I 2
		tp->t_state |= CARR_ON;		/* ATOMIC (spl) */
E 2
	} else {
D 2
		tp->t_state &= ~CARR_ON;
E 2
I 2
		tp->t_state &= ~CARR_ON;	/* ATOMIC (spl) */
E 2
	}
#if m88k
	if (!(flag&(FNDELAY|FNONB))) {
#else
	if (!(flag&FNDELAY)) {
#endif
		while ((tp->t_state&CARR_ON)==0 ||
		       (cp->flags&ASFLAG_LCL && !local)) {
			if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
				asmodem(dev, ON);
D 2
				tp->t_state |= WOPEN;
E 2
I 2
				tp->t_state |= WOPEN;	/* ATOMIC (spl) */
E 2
				if (IORB->io_status == S_IDLE) {
					asin(dev);
				}
			}
			sleep((caddr_t)&tp->t_state, TTIPRI);
			if ((cp->flags & ASFLAG_AVAIL) == 0) {
				u.u_error = ENXIO;
				spl0();
				return;
			}
		}
	}
	if ((cp->flags&ASFLAG_LCL)==0 && local) {
		u.u_error = EBUSY;
		spl0();
		return;
	}
#if m88k
	(*linesw[tp->t_line].l_open)(tp, flag);
#else
	(*linesw[tp->t_line].l_open)(tp);
#endif
	if (IORB->io_status == S_IDLE) {
		asin(dev);
	}
	spl0();
I 3
	TRTPSYS(tp, "OP");
E 3
}

asclose(dev)
minor_t dev;
{
	register struct tty *tp;
	register int local;
	register struct as_info *cp;
	register int s;

	local = dev & LOCAL;
	dev &= ~LOCAL;
	tp = as_tp[dev];
	cp = &as_info[dev];
	(*linesw[tp->t_line].l_close)(tp);
	if ((tp->t_cflag&HUPCL || local) && !(tp->t_state&ISOPEN)) {
		asmodem(dev, OFF);
D 2
		aswaitctl(dev);  /* wait for completion */
E 2
		if (cp->flags&ASFLAG_LCL && local) {
			if (cp->status & ASTAT_CD) {
				delay(HZ/2);
			}
			s = spl7();
D 2
			cp->flags &= ~ASFLAG_LCL;
E 2
I 2
			cp->flags &= ~ASFLAG_LCL;	/* ATOMIC (spl) */
E 2
			if (cp->status&ASTAT_CD) {
D 2
				tp->t_state |= CARR_ON;
E 2
I 2
				tp->t_state |= CARR_ON;	/* ATOMIC (spl) */
E 2
			} else {
D 2
				tp->t_state &= ~CARR_ON;
E 2
I 2
				tp->t_state &= ~CARR_ON; /* ATOMIC (spl) */
E 2
			}
			wakeup((caddr_t)&tp->t_state);
			splx(s);
		}
	}
I 3
	TRTPSYS(tp, "CL");
E 3
}

asread(dev)
minor_t dev;
{
	register struct tty *tp;
	register struct as_info *cp;
D 4
	
E 4
I 4
	register cnt = u.u_count;
	register char *base = u.u_base;

E 4
	tp = as_tp[dev & ~LOCAL];
	cp = &as_info[dev & ~LOCAL];
	if ((cp->flags & ASFLAG_AVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	(*linesw[tp->t_line].l_read)(tp);
I 3
D 4
	TRTPSYS(tp, "RD");
E 4
I 4
	TRTPSYSA(tp, "RD", cnt);
	TRTPDATA(tp, base, cnt - u.u_count);
E 4
E 3
}

aswrite(dev)
minor_t dev;
{
	register struct tty *tp;
	register struct as_info *cp;
I 4
	register cnt = u.u_count;
	register char *base = u.u_base;
E 4
	
	tp = as_tp[dev & ~LOCAL];
	cp = &as_info[dev & ~LOCAL];
	if ((cp->flags & ASFLAG_AVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	(*linesw[tp->t_line].l_write)(tp);
I 3
D 4
	TRTPSYS(tp, "WR");
E 4
I 4
	TRTPSYSA(tp, "WR", cnt);
	TRTPDATA(tp, base, cnt);
E 4
E 3
}

asioint(IORB)
register struct io_serctl *IORB;
{
	if (IORB->io_cmd == PC_READ)
		asrint((struct io_serin *)IORB);
	else if (IORB->io_cmd == PC_WRITE)
		asxint((struct io_serout *)IORB);
	else
		asctlint(IORB);
}

asxint(IORB)
struct io_serout *IORB;
{
	register struct tty *tp;
	register dev_t dev;
I 2
	register struct as_info *cp;
E 2

	sysinfo.xmtint++;
	dev = indtab[IORB->io_index];
I 2
	cp = &as_info[dev];
	cp->flags |= ASFLAG_INTR;
E 2
	tp = as_tp[dev];
	ATOMIC (tp->t_state &= ~BUSY);
	asproc(tp, T_OUTPUT);
	if (tp->t_wsel)
		ttwakeup(tp, TS_WCOLL);
I 2
	cp->flags &= ~ASFLAG_INTR;
I 3
	TRTPINTR(tp, "XI");
E 3
E 2
}

ascarrier(dev, tp, cp)
register dev_t dev;
register struct tty *tp;
register struct as_info *cp;
{
	if (tp->t_cflag&CLOCAL || cp->status&ASTAT_CD) {
		if ((tp->t_state&CARR_ON) == 0) {
			wakeup((caddr_t)&tp->t_state);
			ATOMIC (tp->t_state |= CARR_ON);
		}
	} else {
		if (tp->t_state&CARR_ON && cp->flags&ASFLAG_CD) {
			signal(tp->t_pgrp, SIGHUP);
			tp->t_pgrp = 0;
			asmodem(dev, OFF);
			ATOMIC (tp->t_state &= ~CARR_ON);
			ttyflush(tp, (FREAD|FWRITE));
		}
	}
	if (cp->status & ASTAT_CD) 
D 2
		cp->flags |= ASFLAG_CD;
E 2
I 2
		cp->flags |= ASFLAG_CD;	/* ATOMIC (isr) */
E 2
	else
D 2
		cp->flags &= ~ASFLAG_CD;
E 2
I 2
		cp->flags &= ~ASFLAG_CD;	/* ATOMIC (isr) */
E 2
}

asrint(IORB)
register struct io_serin *IORB;
{
	register struct tty *tp;
	register dev_t dev;
	register struct as_info *cp;
	
	sysinfo.rcvint++;
	IORB->io_status = S_DONE;
	dev = indtab[IORB->io_index];
	tp = as_tp[dev];
	cp = &as_info[dev];
I 2
	cp->flags |= ASFLAG_INTR;
E 2
	if (IORB->io_error == E_DEV) {
		cp->status = IORB->io_devstat;
		ascarrier(dev, tp, cp);
	}
	if (!(tp->t_state & (ISOPEN|WOPEN)))
		goto out;
	if (IORB->io_error == E_DEV) {
		if (cp->status&ASTAT_BREAK || cp->status&ASTAT_FRAMEERR) {
			if ((tp->t_iflag&IGNBRK) == 0) {
				if (tp->t_iflag&BRKINT) {
					(*linesw[tp->t_line].l_input)(tp, L_BREAK);
				} else {
					IORB->io_error = E_OK;
				}
			}
		}
	}
	if (IORB->io_error != E_OK) {
		sysinfo.rcvint--;  /* no character actually received */
		if ((cp->flags & ASFLAG_AVAIL) && IORB->io_error != E_DEV) {
			printf("AS%d disabled: error 0x%x,0x%x\n",
			       dev, IORB->io_error, IORB->io_devstat);
D 2
			cp->flags &= ~ASFLAG_AVAIL;
E 2
I 2
			cp->flags &= ~ASFLAG_AVAIL;	/* ATOMIC (isr) */
E 2
			wakeup((caddr_t)&tp->t_state);
		}
		goto out;
	}
	if (tp->t_rbuf.c_ptr == NULL)
		goto out;

	tp->t_rbuf.c_count -= IORB->io_bcount;
	(*linesw[tp->t_line].l_input)(tp, L_BUF);
	
	out:
	if ((cp->flags & ASFLAG_AVAIL)
	   && tp->t_state&(ISOPEN|WOPEN)
	   && !(tp->t_state&TBLOCK)) {
		asin(dev);
	} else {
		IORB->io_status = S_IDLE;
	}
I 2
	cp->flags &= ~ASFLAG_INTR;
I 3
	TRTPINTR(tp, "RI");
E 3
E 2
}

/*
 * Completion interrupt for control commands.
 */
asctlint(IORB)
struct io_serctl *IORB;
{
	register dev_t dev;
	register struct as_info *cp;

	dev = indtab[IORB->io_index];
	cp = &as_info[dev];
D 2
	if (cp->flags & ASFLAG_BUSY) {
		cp->flags &= ~ASFLAG_BUSY;
		if (IORB->io_cmd == PC_STAT) {
			cp->status = IORB->io_devstat;
			ascarrier(dev, as_tp[dev], cp);
		}
		assetstate(dev);  /* output next state change */
	} else {
		panic("asctlint");
E 2
I 2
	cp->flags |= ASFLAG_INTR;
	cp->flags &= ~ASFLAG_BUSY;
	wakeup((caddr_t)cp);
	if (IORB->io_cmd == PC_STAT) {
		cp->status = IORB->io_devstat;
		ascarrier(dev, as_tp[dev], cp);	/* update carrier status */
E 2
	}
I 2
	if (!(cp->flags & ASFLAG_BUSY))
		asctlstart(cp);
	cp->flags &= ~ASFLAG_INTR;
I 3
	TRTPINTR(as_tp[dev], "CI");
E 3
E 2
}

/*
 * Ioctl command for 'dev'.
 */
asioctl(dev, cmd, arg, mode)
minor_t dev;
int cmd;
int arg;
int mode;
{
	
	dev &= ~LOCAL;
	if ((as_info[dev].flags & ASFLAG_AVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	if (ttiocom(as_tp[dev], cmd, arg, mode)) {
		asparam(dev);
	}
I 3
D 4
	TRTPSYS(as_tp[dev], "IO");
E 4
I 4
	TRTPSYSA(as_tp[dev], "IO", cmd);
E 4
E 3
}

/*
 * Set state variables for control of serial i/o.
 */
asparam(dev)
register int dev;
{
	register struct tty *tp;
	register int cflags, iflags;

	tp = as_tp[dev];
	cflags = tp->t_cflag;
	iflags = tp->t_iflag;
	if ((cflags & CBAUD) == 0) {
		/* hang up line */
		asmodem(dev, OFF);
		return;
	}
D 2
	aswaitctl(dev);  /* wait for completion */
E 2
	asctl(dev, ASCTL_IFLAG, iflags);
	asctl(dev, ASCTL_CFLAG, cflags);
D 2
	aswaitctl(dev);  /* wait for completion */
E 2
	asctl(dev, ASCTL_NOP, 0);  /* get current state of port */	
D 2
	aswaitctl(dev);  /* wait for completion */
E 2
}	

asproc(tp, cmd)
register struct tty *tp;
int cmd;
{
	register dev;
	register s;
	register struct ccblock *tbuf;
	struct io_serin *IORB;

	dev = tp->t_minor;
	switch (cmd) {

		case T_TIME:
		ATOMIC (tp->t_state &= ~TIMEOUT);
		goto start;

		case T_WFLUSH:
		ATOMIC (tp->t_state &= ~TTSTOP);
		asctl(dev, ASCTL_FLUSHOUT, 0);
		goto start;

		case T_CTRESUME:
		case T_RESUME:
		ATOMIC (tp->t_state &= ~TTSTOP);
		goto start;

		case T_OUTPUT:
		
		start:
		tbuf = &tp->t_tbuf;
		if (tp->t_state & (BUSY|TTSTOP)) {
			break;
		}
		if (!(CPRES & (*linesw[tp->t_line].l_output)(tp))) {
			break;
		}
		ATOMIC (tp->t_state |= BUSY);
#if pm100
		asoutstr(dev, (ushort)tbuf->c_ptr, tbuf->c_count);
#else
		asoutstr(dev, (unsigned)tbuf->c_ptr, tbuf->c_count);
#endif
		tbuf->c_count = 0;
		break;

		case T_CTSUSPEND:
		case T_SUSPEND:
		ATOMIC (tp->t_state |= TTSTOP);
		break;

		case T_PXBLOCK:
		asctl(dev, ASCTL_SUSPENDIN, 0);
		break;

		case T_PXUNBLOCK:
		asctl(dev, ASCTL_RESUMEIN, 0);
		break;

		case T_CTBLOCK:
		case T_BLOCK:
		ATOMIC (tp->t_state |= TBLOCK);
		break;

		case T_RFLUSH:
		asctl(dev, ASCTL_FLUSHIN, 0);
		/* fall into UNBLOCK */

		case T_CTUNBLOCK:
		case T_UNBLOCK:
		s = spl7();
D 2
		tp->t_state &= ~TBLOCK;
E 2
I 2
		tp->t_state &= ~TBLOCK;	/* ATOMIC (spl) */
E 2
		IORB = as_info[dev].incb;
		if (IORB->io_status == S_IDLE) {
			asin(dev);
		}
		splx(s);
		break;
		
		case T_DRAINOUT:
D 2
		aswaitctl(dev);
E 2
		asctl(dev, ASCTL_DRAINOUT, 0);
D 2
		aswaitctl(dev);
E 2
		break;
		
		case T_BREAK:
		asctl(dev, ASCTL_BREAK, tp->t_brktime);
		break;
		
		case T_PARM:
		asparam(dev);
		break;
	}
}

/*
 * Turn DTR and RTS on if flag == ON else turn them off.
D 2
 * Returns true if DCD is on.
E 2
 */
asmodem(dev, flag)
int dev;
int flag;
{
	asctl(dev, ASCTL_MODEM, (flag == ON) ? (MODEM_DTR|MODEM_RTS) : 0);
}

/*
 * Send out a request to the pc to output
 * 'len' characters from the string 'adx'.
 */
asoutstr(dev,adx,len)
int dev;
int adx;
int len;
{
	register struct io_serout *IORB;
	
	IORB = as_info[dev].outcb;
	IORB->io_error = E_OK;
	IORB->io_cmd = PC_WRITE;
	IORB->IBUF = adx;
	IORB->io_bcount = len;
	IORB->io_status = S_GO;
	pcstrategy(IORB);
}

/*
 * Request the pc to do 'cmd' on the serial input control
 * block;
 */
asin(dev)
int dev;
{
	register struct io_serin *IORB;
	register struct tty *tp;
	
	tp = as_tp[dev];
	IORB = as_info[dev].incb;

	IORB->io_error = E_OK;
	IORB->io_cmd = PC_READ;
	IORB->io_status = S_GO;
#if pm100
	IORB->io_char = (ushort) (tp->t_rbuf.c_ptr);
#endif
#if pm200 || m88k
	IORB->buf = (unsigned) tp->t_rbuf.c_ptr;
#endif
	IORB->io_bcount = tp->t_rbuf.c_count - 1;
	pcstrategy(IORB);
}

/*
D 2
 * Change the state of the serial port 'dev'. This procedure logs
 * the requested new state and starts a command to change the state
 * if one is not already in progress. Note that, upon return from 
 * this procedure, the state will probably not have been changed yet
 * due to the buffered nature of the pc interface. A special case is
 * the command ASCTL_NOP which causes a status command to be issued.
E 2
I 2
 * send a device control command to port 'dev'
E 2
 */
asctl(dev, cmd, data)
int dev;
unsigned short cmd;
unsigned data;
{
	register int s;
I 2
	register int put;
E 2
	register struct as_info *cp;
	
	cp = &as_info[dev];
D 2
	
	/* install state change data */
	
E 2
	s = spl7();
D 2
	cp->update |= (1 << cmd);
	switch (cmd) {
		
		case ASCTL_IFLAG:
		cp->iflag = data;
		break;
		
		case ASCTL_CFLAG:
		cp->cflag = data;
		break;
		
		case ASCTL_MODEM:
		cp->modemcontrol = data;
		break;
E 2
I 2
	put = cp->qput;
	cp->cmd[put] = cmd;
	cp->data[put] = data;
	cp->qput = (put + 1) % ASQSIZE;
E 2

D 2
		case ASCTL_BREAK:
		cp->brktime = data;
		break;
	}
	assetstate(dev);  /* send state change to pc */
E 2
I 2
	if (!(cp->flags & ASFLAG_BUSY))
		asctlstart(cp);
	if (!(cp->flags & ASFLAG_INTR))
		while (cp->flags & ASFLAG_BUSY)
			sleep((caddr_t)cp, PZERO);
E 2
	splx(s);
}

D 2
/*
 * Start a command to update the async port state if one is not currently
 * in progress. Must be called under async interrupt level.
 */
assetstate(dev)
int dev;
E 2
I 2
asctlstart(cp)	/* ints disabled */
register struct as_info *cp;
E 2
{
	register struct io_serctl *IORB;
D 2
	register struct as_info *cp;
	register int cmd;
	register int data;
	register int update;
	
	cp = &as_info[dev];
	IORB = cp->ctlcb;
E 2
I 2
	register int get;
E 2

D 2
	/* check for command block busy or no pending commands */
	
	if ((cp->flags & ASFLAG_BUSY) || cp->update == 0) {
		if ((cp->flags & ASFLAG_BUSY) == 0 &&
		    (cp->flags & ASFLAG_CTLWU) == ASFLAG_CTLWU) {
			cp->flags &= ~ASFLAG_CTLWU;
			wakeup((caddr_t)cp);
		}
E 2
I 2
	if ((get = cp->qget) == cp->qput)
E 2
		return;
D 2
	}
E 2
I 2
	cp->flags |= ASFLAG_BUSY;	/* ATOMIC (spl) */
	IORB = cp->ctlcb;
E 2
	
D 2
	/* pick the next command */
E 2
I 2
	/* install state change data */
E 2
	
D 2
	update = cp->update;
	for (cmd = 0; cmd < 32; cmd++) {
		if (update & 1) {
			switch (cmd) {
			
				case ASCTL_NOP:
				data = 0;
				break;

				case ASCTL_IFLAG:
				data = cp->iflag;
				break;

				case ASCTL_CFLAG:
				data = cp->cflag;
				break;

				case ASCTL_MODEM:
				data = cp->modemcontrol;
				break;

				case ASCTL_BREAK:
				data = cp->brktime;
				break;
		
				default:
				data = 0;
				break;
			}
			cp->update &= ~(1 << cmd);
			break;
		}
		update >>= 1;
	}	
	
	/* start command */
	
	cp->flags |= ASFLAG_BUSY;
E 2
	IORB->io_error = E_OK;
D 2
	IORB->io_cmd = (cmd == ASCTL_NOP) ? PC_STAT : IOCTL;
	IORB->IVAL = cmd;
	IORB->io_bcount = data;
E 2
I 2
	IORB->io_cmd = (cp->cmd[get] == ASCTL_NOP) ? PC_STAT : IOCTL;
	IORB->IVAL = cp->cmd[get];
	IORB->io_bcount = cp->data[get];
E 2
	IORB->io_status = S_GO;
I 2
	cp->qget = (get + 1) % ASQSIZE;
E 2
	pcstrategy(IORB);
}

D 2
/*
 * Sleep until all pending controls are completed.
 */
aswaitctl(dev)
int dev;
{
	register struct as_info *cp;
	register s;

	cp = &as_info[dev];
	s = spl7();
	if ((cp->flags & ASFLAG_BUSY) || cp->update != 0) {
		cp->flags |= ASFLAG_CTLWU;
		while ((cp->flags & ASFLAG_CTLWU) == ASFLAG_CTLWU) {
			sleep((caddr_t)cp, PZERO);
		}
	}
	splx(s);
}
E 2
/*
 * Initialize driver state.
 */
asinit()
{
	register i;
	register as_cnt, asoutnum, asctlnum;
	register unit;
	char u_in[ASMAX], u_out[ASMAX], u_ctl[ASMAX];

	for (i=0; i<ASMAX; ++i) {
		u_in[i] = u_out[i] = u_ctl[i] = 0;
	}
	

	DIXLOOP(i) {
		switch(DIX(i)) {
		
		case SERIN:
			if ((unit = asunit(i, u_in)) != -1) {
				indtab[i] = unit;
				as_info[unit].incb = (struct io_serin *)
					dmastatic(sizeof(struct io_serin));
				as_info[unit].incb->io_index = i;
			}
			break;
			
		case SEROUT:
			if ((unit = asunit(i, u_out)) != -1) {
				indtab[i] = unit;		
				as_info[unit].outcb = (struct io_serout *)
					dmastatic(sizeof(struct io_serout));
				as_info[unit].outcb->io_index = i;
			}
			break;
			
		case SERCTL:
			if ((unit = asunit(i, u_ctl)) != -1) {
				indtab[i] = unit;
				as_info[unit].ctlcb = (struct io_serctl *)
					dmastatic(sizeof(struct io_serctl));
				as_info[unit].flags |= ASFLAG_AVAIL;
				as_info[unit].ctlcb->io_index = i;
				if (unit > asmax)
					asmax = unit;
			}
			break;

		case ASIO:
			if ((unit = asunit(i, u_ctl)) != -1) {
				indtab[i] = unit;
				u_in[unit] = 1;
				u_out[unit] = 1;
				as_info[unit].incb = (struct io_serin *)
					dmastatic(sizeof(struct io_serin));
				as_info[unit].outcb = (struct io_serout *)
					dmastatic(sizeof(struct io_serout));
				as_info[unit].ctlcb = (struct io_serctl *)
					dmastatic(sizeof(struct io_serctl));
				as_info[unit].outcb->io_index = i;
				as_info[unit].incb->io_index = i;
				as_info[unit].ctlcb->io_index = i;
				as_info[unit].flags |= ASFLAG_AVAIL;
				if (unit > asmax)
					asmax = unit;
			}
			break;
		}
	}

	/* allocate only as many tty structs as necessary */
	asmax++;
	as_tty = (struct tty *) dmastatic(asmax*sizeof(struct tty));
D 5
	for (i=0; i<asmax; ++i)
E 5
I 5
	for (i=0; i<asmax; ++i) {
E 5
		as_tp[i] = &as_tty[i];
I 5
		as_tty[i].t_mctbl.mapping = 0;
	}
E 5
	for (i=0; i<cdevcnt ; i++)
		if (cdevsw[i].d_open == asopen) {
			cdevsw[i].d_ttys = as_tty;
			break;
		}
I 5
	mc_max = 20;
	mc_array = (struct mapchan *) dmastatic(mc_max*sizeof(struct mapchan));	
	for (i=0; i<mc_max; i++)
		mc_array[i].hdr.refcount = 0;
E 5
}

/*	asunit(dix)
 *
 *	issue NOP with waited I/O to get tty unit number
 *	map unit into table; use next available >= returned unit
 */
asunit(dix, tbl)
int dix;
char tbl[];
{
	struct io_serctl IORB;
	register unit;
	
	IORB.io_index = dix;
	IORB.io_status = S_GO;
	IORB.io_cmd = PC_NOP;
	IORB.io_error = E_OK;
	IORB.io_devstat = 0;
	pcwait(&IORB, sizeof(struct io_serctl));
	for (unit=IORB.io_devstat&0x7f; unit<ASMAX; ++unit)
		if (tbl[unit] == 0) {
			tbl[unit] = 1;
			return(unit);
		}
	return(-1);
I 3
}

D 4
asprstate(tp, s)
E 4
I 4
asprstate(tp, s, arg)
E 4
register struct tty *tp;
char *s;
{
	printf("tty%d %s st %x flg(i %x, o %x, c %x, l %x)", 
		tp->t_minor, s, tp->t_state, 
		tp->t_iflag, tp->t_oflag, tp->t_cflag, tp->t_lflag);
D 4
	printf(" qsz(r %x, c %x, o %x)\n", 
E 4
I 4
	if (arg) {
		printf(" qsz(r %x, c %x, o %x) arg = %x\n", 
			tp->t_rawq.c_cc, tp->t_canq.c_cc, tp->t_outq.c_cc, arg);
	}
	else 
		printf(" qsz(r %x, c %x, o %x)\n", 
E 4
			tp->t_rawq.c_cc, tp->t_canq.c_cc, tp->t_outq.c_cc);
I 4
}

#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4
asprdata(tp, base, count)
struct tty *tp;
register unsigned char *base;
register count;
{
	register i;

	if (count > 60)
		count = 60;

	printf("tty%d [", tp->t_minor); 
	for (; count ; count--)
		outputc(fubyte (base++), TOCONS|TOLOG, (struct tty *) 0);
	printf("]\n"); 
E 4
E 3
}
E 1
