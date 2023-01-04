h47221
s 00001/00001/00707
d D 1.4 90/06/28 02:16:13 root 5 4
c PT_tty is now a pointer
e
s 00010/00001/00698
d D 1.3 90/05/31 20:37:31 root 4 3
c added iscons()
e
s 00002/00000/00697
d D 1.2 90/03/26 17:17:36 root 3 1
c added tty window size default for console
e
s 00002/00000/00697
d R 1.2 90/03/22 10:45:27 root 2 1
c initialized terminal size to 24 X 80
e
s 00697/00000/00000
d D 1.1 90/03/06 12:28:38 root 1 0
c date and time created 90/03/06 12:28:38 by root
e
u
U
t
T
I 1
/* SID @(#)cons.c	1.7 */

/*
 *	Console interface
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
#include "sys/tty.h"
#include "sys/termio.h"
#include "sys/buf.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/sysinfo.h"
#include "sys/ascomm.h"
#include "sys/sysmacros.h"

#if CONMAX != 4
/* wrong cb.h, contp below has to change */
#include "CONMAX != 4"
#endif

struct	tty con_tty[CONMAX];
struct tty *contp[] =
{
	&con_tty[0],
	&con_tty[1],
	&con_tty[2],
	&con_tty[3]
};


int	conproc();
int	ttrstrt();

#define cport(x) ((x) & 0xff)
#define	CONSOLE	0  /* the only legal device */

#define CONFLAG_BUSY 0x1  /* a status update cmd is being sent to the pc */
#define CONFLAG_CTLWU 0x2  /* wakeup processes awaiting control completion */
#define CONFLAG_XBSY 0x4	/* transmit iorb busy */

struct con_info {
	uint update;  /* 1 bit for each part of state that needs update */
	ushort avail;	/* available flag */
	ushort flags;  /* current state of control block */
	ushort iflag;  /* new iflags */
	ushort cflag;  /* new cflags */
	struct io_conin *incb;  /* request block pointer for incoming data */
	struct io_conout *outcb;  /* request block pointer for outgoing data */
	struct io_conctl *ctlcb;  /* request block pointer for control info */
} con_info[CONMAX];


conopen(dev, flag)
dev_t dev;
int flag;
{
	register struct tty *tp;
	register l;
	
	l = cport(dev);
	if ((l < CONMAX) && con_info[l].avail) {
	
		tp = contp[l];
		tp->t_proc = conproc;
		tp->t_minor = l;
		ATOMIC (tp->t_state |= WOPEN);
		if (tp->t_rbuf.c_ptr == NULL)	/* have this setup for conin */
			ttioctl(tp, LDOPEN, 0, 0);
		spl7();
		if (con_info[l].incb->io_status == S_IDLE) {
			conin(l);
		}
		spl0();
		if ((tp->t_state&ISOPEN) == 0) {
			ttinit(tp);
			ATOMIC (tp->t_state = CARR_ON);
			(*linesw[tp->t_line].l_open)(tp, flag);
			tp->t_cflag &= ~CBAUD;
			tp->t_cflag |= B9600;
			tp->t_iflag |= ICRNL|IXON|IXANY|ISTRIP;
			tp->t_oflag |= OPOST|ONLCR;
			tp->t_lflag |= ISIG|ICANON|ECHO|ECHOK;
		} else {
			(*linesw[tp->t_line].l_open)(tp, flag);
		}
I 3
		tp->t_winsize.ws_row = 25;
		tp->t_winsize.ws_col = 80;
E 3
		/* ttopen will do the next line for us */
		/*ATOMIC (tp->t_state &= ~WOPEN);*/
	} else
		u.u_error = ENXIO;
}

conclose(dev)
dev_t dev;
{
	register struct tty *tp;
	register l;
	
	l = cport(dev);
	if ((l < CONMAX) && con_info[l].avail) {
		tp = contp[l];
		(*linesw[tp->t_line].l_close)(tp);
		l = spl7();
		/* if somebody else did an open during this close,
		 * don't mess up their flags. if no one else did an
		 * open zero t_state.
		 */
		if ((tp->t_state & (ISOPEN|WOPEN)) == 0) {
			tp->t_state = 0;
		}
		splx(l);
	} 
}

conread(dev)
dev_t dev;
{
	register l;
	register struct tty *tp;

	l = cport(dev);
	tp = contp[l];

	if ((l < CONMAX) && con_info[l].avail) {
		(*linesw[tp->t_line].l_read)(tp);
	}
}

conwrite(dev)
dev_t dev;
{
	register l;
	register struct tty *tp;

	l = cport(dev);
	tp = contp[l];

	if ((l < CONMAX) && con_info[l].avail) {
		(*linesw[tp->t_line].l_write)(tp);
	}
}

conxint(IORB)
struct io_conout *IORB;
{
	register struct tty *tp;
	register struct con_info *cp;

	cp = &con_info[indtab[IORB->io_index]];
	sysinfo.xmtint++;
	tp = contp[indtab[IORB->io_index]];
	cp->flags &= ~CONFLAG_XBSY;
	conproc(tp, T_OUTPUT);
	if (tp->t_wsel)
		ttwakeup(tp, TS_WCOLL);
}

conrint(IORB)
struct io_conin *IORB;
{
	register struct tty *tp;
	register dev;

	sysinfo.rcvint++;
	dev = indtab[IORB->io_index];
	tp = contp[dev];
	if (!(tp->t_state & ISOPEN))
		goto out;
	if (tp->t_rbuf.c_ptr == NULL)
		goto out;

	tp->t_rbuf.c_count -= IORB->io_bcount;
	(*linesw[tp->t_line].l_input)(tp, L_BUF);
	
	out:
	if (tp->t_state & ISOPEN) {
		conin(dev);
	} else {
		IORB->io_status = S_IDLE;
	}
}

/*
 * Completion interrupt for control commands.
 */
conctlint(IORB)
struct io_conctl *IORB;
{
	register struct con_info *cp;

	cp = &con_info[indtab[IORB->io_index]];
	if (cp->flags & CONFLAG_BUSY) {
		cp->flags &= ~CONFLAG_BUSY;
		consetstate(cp);  /* output next state change */
	} else {
		panic("conctlint");
	}
}

/*
 * Ioctl command for 'dev'.
 */
conioctl(dev, cmd, arg, mode)
int dev;
int cmd;
int arg;
int mode;
{
	if (cport(dev) < CONMAX) {
		switch(cmd) {
			struct tty *tp;
	
			default:
			tp = contp[cport(dev)];
			if (ttiocom(tp, cmd, arg, mode)) {
				conparam(tp);
			}
			break;
		}
	}
	conwaitctl(cport(dev));
}

/*
 * Set state variables for control of console i/o.
 */
conparam(tp)
register struct tty *tp;
{
	register int cflags, iflags;

	cflags = tp->t_cflag;
	iflags = tp->t_iflag;
	conctl(tp->t_minor,ASCTL_IFLAG, iflags);
	conctl(tp->t_minor,ASCTL_CFLAG, cflags);
}
	
conproc(tp, cmd)
register struct tty *tp;
{
	register struct ccblock *tbuf;
	register dev = tp->t_minor;
	register struct con_info *cp = &con_info[dev];

	switch (cmd) {

		case T_DRAINOUT:
		conctl(dev,ASCTL_DRAINOUT, 0);
		break;

		case T_TIME:
		ATOMIC (tp->t_state &= ~TIMEOUT);
		goto start;

		case T_WFLUSH:
		ATOMIC (tp->t_state &= ~TTSTOP);
		conctl(dev,ASCTL_FLUSHOUT, 0);
		goto start;
		
		case T_CTRESUME:
		case T_RESUME:
		ATOMIC (tp->t_state &= ~TTSTOP);
		goto start;

		case T_OUTPUT:
		
		start:
		tbuf = &tp->t_tbuf;
		if (tp->t_state & TTSTOP) {
			break;
		}
		if (cp->flags & CONFLAG_XBSY)
			break;
		if (!(CPRES & (*linesw[tp->t_line].l_output)(tp))) {
			break;
		}
		ATOMIC (cp->flags |= CONFLAG_XBSY);
#if pm100
		conoutstr(dev,(ushort)tbuf->c_ptr, tbuf->c_count);
#endif
#if pm200 || m88k
		conoutstr(dev,(unsigned)tbuf->c_ptr, tbuf->c_count);
#endif
		tbuf->c_count = 0;
		break;

		case T_CTSUSPEND:
		case T_SUSPEND:
		ATOMIC (tp->t_state |= TTSTOP);
		break;

		case T_CTBLOCK:
		case T_BLOCK:
		ATOMIC (tp->t_state |= TBLOCK);
		conctl(dev,ASCTL_SUSPENDIN, 0);
		break;

		case T_RFLUSH:
		if (!(tp->t_state&TBLOCK)) {
			break;
		}
		conctl(dev,ASCTL_FLUSHIN, 0);
		goto startin;

		case T_CTUNBLOCK:
		case T_UNBLOCK:
		
		startin:
		ATOMIC (tp->t_state &= ~TBLOCK);
		conctl(dev,ASCTL_RESUMEIN, 0);
		break;
		
		case T_PARM:
		conparam(tp);
		break;
	}
}

/*
 * Send out a request to the pc to output
 * 'len' characters from the string 'adx'.
 */
conoutstr(dev,adx,len)
register int dev;
int adx;
int len;
{
	register struct io_conout *IORB;
	
	IORB = con_info[dev].outcb;
	IORB->io_error = IORB_MAGIC;
	IORB->io_cmd = PC_WRITE;
	IORB->IBUF = adx;
	IORB->io_bcount = len;
	IORB->io_status = S_GO;
	pcstrategy(IORB);
}

/*
 * Request the pc to read in a character
 */
conin(dev)
{
	register struct io_conin *IORB;
	register struct tty *tp;
	
	tp = contp[dev];
	
	IORB = con_info[dev].incb;
	IORB->io_error = IORB_MAGIC;
	IORB->io_cmd = PC_READ;
	IORB->io_status = S_GO;
#if pm100
	IORB->io_char = (ushort) tp->t_rbuf.c_ptr;
#endif
#if pm200 || m88k
	IORB->buf = (unsigned) tp->t_rbuf.c_ptr;
#endif
	IORB->io_bcount = tp->t_rbuf.c_count - 1;
	pcstrategy(IORB);
}

/*
 * Change the state of the console port. This procedure logs
 * the requested new state and starts a command to change the state
 * if one is not already in progress. Note that, upon return from 
 * this procedure, the state will probably not have been changed yet
 * due to the buffered nature of the pc interface.
 */
conctl(dev,cmd, data)
register int dev;
unsigned short cmd;
unsigned short data;
{
	register int s;
	register struct con_info *cp;
	
	cp = &con_info[dev];
	
	/* install state change data */
	
	s = spl7();
	cp->update |= (1 << cmd);
	switch (cmd) {
		
		case ASCTL_IFLAG:
		cp->iflag = data;
		break;
		
		case ASCTL_CFLAG:
		cp->cflag = data;
		break;
	}
	consetstate(cp);  /* send state change to pc */
	splx(s);
}

/*
 * Start a command to update the console port state if one is not currently
 * in progress. Must be called under console interrupt level.
 */
consetstate(cp)
register struct con_info *cp;
{
	register struct io_conctl *IORB;
	register int cmd;
	register int data;
	register int update;
	
	IORB = cp->ctlcb;

	/* check for command block busy or no pending commands */
	
	if ((cp->flags & CONFLAG_BUSY) || cp->update == 0) {
		if ((cp->flags & CONFLAG_BUSY) == 0 &&
		    (cp->flags & CONFLAG_CTLWU) == CONFLAG_CTLWU) {
			cp->flags &= ~CONFLAG_CTLWU;
			wakeup((caddr_t)cp);
		}
		return;
	}
	
	/* pick the next command */
	
	update = cp->update;
	for (cmd = 0; cmd < 32; cmd++) {
		if (update & 1) {
			switch (cmd) {

				case ASCTL_IFLAG:
				data = cp->iflag;
				break;

				case ASCTL_CFLAG:
				data = cp->cflag;
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
	
	cp->flags |= CONFLAG_BUSY;
	IORB->io_error = IORB_MAGIC;
#if pm100
	IORB->io_cmd = PC_IOCTL;
	IORB->io_memaddr = cmd;
#endif
#if pm200 || m88k
	IORB->cmd = PC_DVCTL;
	IORB->val = cmd;
#endif
	IORB->io_bcount = data;
	IORB->io_status = S_GO;
	pcstrategy(IORB);
}

/*
 * Sleep until all pending controls are completed.
 */
conwaitctl(dev)
{
	register struct con_info *cp;
	register s;

	cp = &con_info[dev];
	s = spl7();
	if ((cp->flags & CONFLAG_BUSY) || cp->update != 0) {
		cp->flags |= CONFLAG_CTLWU;
		while ((cp->flags & CONFLAG_CTLWU) == CONFLAG_CTLWU) {
			if (sleep((caddr_t)cp, (PZERO+1)|PCATCH)) {
				cp->update = 0;		/* forget it */
				u.u_error = EINTR;
				break;
			}
		}
	}
	splx(s);
}

#if pm100
#define	KCHAR	PCCMD->pc_kchar
#define	KFLAG	PCCMD->pc_kflag
#define	SCHAR	PCCMD->pc_schar
#define	SFLAG	PCCMD->pc_sflag
#endif
#if pm200 || m88k
#define	KCHAR	COMMPAGE->kchar
#define	KFLAG	COMMPAGE->kflag
#define	SCHAR	COMMPAGE->schar
#define	SFLAG	COMMPAGE->sflag
#endif

putchar(c)
register c;
{
	register s;

#ifdef SIMULATOR
	asm("	tb0	0,r0,509");
#else
	s = spl7();
	while ((SFLAG == S_GO) || (SFLAG == S_BUSY)) {
	}
	if (c == 0) {
		return;
	}
	SCHAR = c;
	SFLAG = S_GO;
	SETATINTR();
	if (c == '\n') {
		putchar('\r');
	}
	putchar(0);	/* wait for char to go out */
	SFLAG = S_IDLE;
	splx(s);
#endif
}

waitchar()
{
	register c;

	if (KFLAG == S_DONE) {
		return(1);
	}
	KFLAG = S_GO;
	SETATINTR();
	while (KFLAG == S_GO) {
	}
	if (KFLAG == S_DONE) {
		return(1);
	} else {
		return(0);
	}
}

getchar()
{
	register c;

#ifdef SIMULATOR
	asm("	tb0	0,r0,510");
#else
	if (KFLAG == S_DONE) {
		c = KCHAR & 0x7f;
		KFLAG = S_BUSY;
		return(c);
	}
	do {
		KFLAG = S_GO;
		SETATINTR();
		while (KFLAG == S_GO) {
		}
	} while (KFLAG != S_DONE);
	c = KCHAR & 0x7f;
	KFLAG = S_BUSY;
	if (c == '\r') {
		c = '\n';
	}
	putchar(c);
	return(c);
#endif
}

enterdebugmode()
{

	KFLAG = S_BUSY;
}


exitdebugmode()
{

	KFLAG = S_IDLE;
}

/*
 * Initialize driver state.
 */
coninit()
{
	register i;
	register con_cnt, conoutnum, conctlnum;
	register unit;
	char u_in[CONMAX], u_out[CONMAX], u_ctl[CONMAX];

	for (i=0; i<CONMAX; ++i) {
		u_in[i] = u_out[i] = u_ctl[i] = 0;
	}
	

	DIXLOOP(i) {
		switch (DIX(i)) {
		
		case CONIN:
			if ((unit = conunit(i, u_in)) != -1) {
				indtab[i] = unit;
				con_info[unit].incb = (struct io_conin *)
					dmastatic(sizeof(struct io_conin));
				con_info[unit].avail = 1;
				con_info[unit].incb->io_index = i;
			}
			break;
			
		case CONOUT:
			if ((unit = conunit(i, u_out)) != -1) {
				indtab[i] = unit;		
				con_info[unit].outcb = (struct io_conout *)
					dmastatic(sizeof(struct io_conout));
				con_info[unit].outcb->io_index = i;
			}
			break;
			
		case CONCTL:
			if ((unit = conunit(i, u_ctl)) != -1) {
				indtab[i] = unit;
				con_info[unit].ctlcb = (struct io_conctl *)
					dmastatic(sizeof(struct io_conctl));
				con_info[unit].ctlcb->io_index = i;
			}
			break;
		}
	}
}

/*	conunit(dix)
 *
 *	issue NOP with waited I/O to get console unit number
 *	map unit into table; use next available >= returned unit
 */
conunit(dix, tbl)
int dix;
char tbl[];
{
	struct io_conctl IORB;
	register unit;
	
	IORB.io_index = dix;
	IORB.io_status = S_GO;
	IORB.io_cmd = PC_NOP;
	IORB.io_error = IORB_MAGIC;
	IORB.io_devstat = 0;
	pcwait(&IORB, sizeof(struct io_conctl));
	for (unit=IORB.io_devstat&0x7f; unit<CONMAX; ++unit)
		if (tbl[unit] == 0) {
			tbl[unit] = 1;
			return(unit);
		}
	return(-1);
}

dumptty()
{
	register struct a {
		unsigned	dmaj;
		unsigned	dmin;
		caddr_t		addr;
	} *uap;
D 5
	extern struct	tty PT_tty[];
E 5
I 5
	extern struct	tty *PT_tty;
E 5
	register struct tty *tp;

	uap = (struct a *)u.u_ap;
	if (uap->dmaj > cdevcnt || uap->dmin > 256) {
		u.u_error = EINVAL;
		return;
	}
	if ((tp = cdevsw[uap->dmaj].d_ttys) == NULL) {
		if (uap->dmaj == 37 || uap->dmaj == 38)
			tp = PT_tty;
		else {
			u.u_error = ENXIO;
			return;
		}
	}
	tp += uap->dmin;
D 4
	printf("tty address (%d, %d) =  %x\n", uap->dmaj, uap->dmin, tp);
E 4
	if (copyout((caddr_t) tp, uap->addr, sizeof(struct tty)))
		u.u_error = EFAULT;
I 4
}

iscons()
{
	if (u.u_ttyp)
		/* if on a terminal is it a console */
		return ((*cdevsw[major(u.u_ttyd)].d_open) == conopen);
	else
		/* pretend it is ok! */
		return (1);
E 4
}
E 1
