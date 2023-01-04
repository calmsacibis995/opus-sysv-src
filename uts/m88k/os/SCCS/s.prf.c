h23069
s 00006/00003/00371
d D 1.5 90/04/20 12:47:33 root 5 4
c limit write to tty structures to 2048 bytes
e
s 00060/00052/00314
d D 1.4 90/04/18 16:53:48 root 4 3
c added syslogd support
e
s 00062/00001/00304
d D 1.3 90/04/05 17:44:02 root 3 2
c kmesgread stuff
e
s 00010/00000/00295
d D 1.2 90/03/26 17:16:32 root 2 1
c more diagnostics in nomemmsg
e
s 00295/00000/00000
d D 1.1 90/03/06 12:28:07 root 1 0
c date and time created 90/03/06 12:28:07 by root
e
u
U
t
T
I 1
/*
 *	Copyright (c) 1987 Motorola Inc
 *	All Rights Reserved
 *		@(#)prf.c	10.1
 */
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/prf.c	10.8"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/cmn_err.h"
#include "sys/varargs.h"
I 2
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
I 3
#include "sys/errno.h"
I 4
#include "sys/tty.h"
#include "sys/msgbuf.h"
E 4
E 3
E 2

/* general argument list for cmn_err and panic */

# define ARGS	a0,a1,a2,a3,a4,a5

/*	Save output in a buffer where we can look at it
**	with demon or crash.  If the message begins with
**	a '!', then only put it in the buffer, not out
**	to the console.
*/

D 4
extern	char	putbuf[];
extern	int	putbufsz;
int		putbufndx;
I 3
#define	putbufput	putbufndx
int		putbufget;
E 3
short		prt_where;
E 4
I 4
#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4
E 4

I 4
short		prt_where = PRW_BUF | PRW_CONS;
D 5
static		outputc();
E 5
struct tty *constty;

E 4
#define	output(c) \
	if (prt_where & PRW_BUF) { \
D 3
		putbuf[putbufndx++ % putbufsz] = c; \
E 3
I 3
D 4
		putbuf[putbufndx] = c; \
		putbufndx = (putbufndx + 1) % putbufsz; \
E 4
I 4
		outputc(c, TOLOG, (struct tty *) 0); \
E 4
E 3
	} \
	if (prt_where & PRW_CONS) \
D 4
		putchar(c);
E 4
I 4
		outputc(c, TOCONS, (struct tty *) 0);
E 4

char	*panicstr ;

/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much suspended.
 * Printf should not be used for chit-chat.
 */

printf(fmt, va_alist)
register char *fmt;
va_dcl
{
	va_list ap;
	register int	c;
	register char	*s;
	register int	opri;

	va_start(ap);	/* must be first statement */

	opri = splhi();

loop:
	while ((c = *fmt++) != '%') {
		if (c == '\0') {
			splx(opri);
D 4
			wakeup(putbuf);
E 4
I 4
			logwakeup();
E 4
			return;
		}
		output(c);
	}
	c = *fmt++;
	if (c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn(va_arg(ap, long), c=='o'? 8: (c=='x'? 16:10));
	else if (c == 's') {
		s = va_arg(ap, char *);
		while (c = *s++) {
			output(c);
		}
	} else if (c == 'D') {
		printn(va_arg(ap, long), 10);
	}
	goto loop;
}

printn(n, b)
long n;
register b;
{
	register i, nd, c;
	int	flag;
	int	plmax;
	char d[12];

	c = 1;
	flag = n < 0;
	if (flag)
		n = (-n);
	if (b==8)
		plmax = 11;
	else if (b==10)
		plmax = 10;
	else if (b==16)
		plmax = 8;
	if (flag && b==10) {
		flag = 0;
		output('-');
	}
	for (i=0;i<plmax;i++) {
		nd = n%b;
		if (flag) {
			nd = (b - 1) - nd + c;
			if (nd >= b) {
				nd -= b;
				c = 1;
			} else
				c = 0;
		}
		d[i] = nd;
		n = n/b;
		if ((n==0) && (flag==0))
			break;
	}
	if (i==plmax)
		i--;
	for (;i>=0;i--) {
		output("0123456789ABCDEF"[d[i]]);
	}
}

/*
 * Panic is called on unresolvable fatal errors.
 * Change name to force unresolve references.
 * All routines should use cmn_err.
 * NOTE: Nothing is printed here.
 *	 To print a message use cmn_err.
 */

int noPanicSync = 0;

xpanic(fmt, ARGS)
char	*fmt;
int	ARGS ;
{
	extern breakpoint();

	dcacheflush(0);
I 5
	constty = 0;
E 5
	printf("panic: ");
	printf(fmt, ARGS);
	if (!panicstr && fmt) {
		panicstr = fmt;		/* save panic string */
		if (!noPanicSync)
			update();	/* "sync" */
D 4
/*		unixcore(); 		/* write unix core image */
E 4
	}

D 4
#if ! defined(ASPEN) && ! defined(ANGEL) && ! defined(opus)
	sysfail();			/* generate VME SYSFAIL signal */
#endif

/*#ifdef	DEBUG*/		/* Always breakpoint for now. */
E 4
	breakpoint();
D 4
/*#endif*/
E 4
	for(;;) {
	}
}

/*
 * prdev prints a warning message.
 * dev is a block special device argument.
 */

prdev(str, dev)
char *str;
dev_t dev;
{
	register maj;

	maj = bmajor(dev);
	if (maj >= bdevcnt) {
		cmn_err(CE_WARN,"%s on bad dev 0x%x\n", str, dev);
		return;
	}
	(*bdevsw[maj].d_print)(dev, str);
}

/*
 * prcom prints a diagnostic from a device driver.
 * prt is device dependent print routine.
 */
prcom(prt, bp, er1, er2)
int (*prt)();
register struct buf *bp;
{
	(*prt)(bp->b_dev, "\nDevice error");
	cmn_err(CE_NOTE,"bn = %D er = 0x%x,0x%x\n", bp->b_blkno, er1, er2);
}

cmn_err(level, fmt, ARGS)
register int	level ;
char		*fmt;
int		ARGS ;
{
	static	int	panic_level = 0;
	int	pri;

	/*	Set up to print to putbuf, console, or both
	**	as indicated by the first character of the
	**	format.
	*/

	pri = splhi();
	if (*fmt == '!') {
		prt_where = PRW_BUF;
		fmt++;
	} else if (*fmt == '^') {
		prt_where = PRW_CONS;
		fmt++;
	} else {
		prt_where = PRW_BUF | PRW_CONS;
	}

	switch (level) {
		case CE_CONT :
			printf(fmt, ARGS) ;
			break ;

		case CE_NOTE :
			printf("\nNOTICE: ") ;
			printf(fmt, ARGS) ;
			printf("\n") ;
			break ;

		case CE_WARN :
			printf("\nWARNING: ") ;
			printf(fmt, ARGS) ;
			printf("\n") ;
			break ;

		case CE_PANIC :
			prt_where = PRW_CONS | PRW_BUF;
			panic_level++ ;
I 5
			constty = 0;
E 5
			printf("\nPANIC (LEVEL %d): ",panic_level) ;
			printf(fmt, ARGS) ;
			printf("\n") ;
			splx(pri);
			xpanic(fmt, ARGS) ;

		default :
			cmn_err(CE_PANIC,
			  "unknown level in cmn_err (level=%d, msg=\"%s\")",
			  level, fmt, ARGS) ;
	}
	splx(pri);
}

/*	The following is an interface routine for the drivers.
*/

dri_printf(ARGS)
{
	cmn_err(CE_CONT, ARGS);
}

/*	Called by the ASSERT macro in debug.h when an
**	assertion fails.
*/

assfail(a, f, l)
register char *a, *f;
{
	cmn_err(CE_PANIC, "assertion failed: %s, file: %s, line: %d", a, f, l);
}


nomemmsg(func, count, contflg, lockflg)
register char	*func;
register int	count;
register int	contflg;
register int	lockflg;
{
I 2
	extern int	availrmem;
	extern int	availsmem;

E 2
	cmn_err(CE_NOTE,
		"%s - Insufficient memory to%s %d%s page%s - %s",
		func, (lockflg ? " lock" : " allocate"),
		count, (contflg ? " contiguous" : ""),
		(count == 1 ? "" : "s"),
		"system call failed");
I 2

	procname(0);
	printf("System call #%d, rmem %d, smem %d", 
		u.u_syscall, availrmem, availsmem);
I 3
}

D 4
kmsgread()
E 4
I 4
prfsys()
E 4
{
	register struct a {
		int	cmd;
D 4
		char	*cbuf;
		int	count;
E 4
I 4
		int	val;
E 4
	} *uap;
D 4
	register cnt;
E 4

	if (!suser())
		return;

	uap = (struct a *)u.u_ap;

	if (uap->cmd == 1) {
		u.u_rval1 = prt_where;
D 4
		prt_where = (uap->count & (PRW_BUF|PRW_CONS));
E 4
I 4
		prt_where = (uap->val & (PRW_BUF|PRW_CONS));
E 4
		return;
	}
D 4
	if (uap->cmd == 2) {
		u.u_rval1 = putbufsz;
		return;
E 4
I 4

	u.u_error = EINVAL;
}

/*
 * Print a character on console or users terminal.
 * If destination is console then the last MSGBUFS characters
 * are saved in msgbuf for inspection later.
 */
/*ARGSUSED*/
D 5
static
E 5
outputc(c, flags, tp)
	register int c;
	struct tty *tp;
{
	extern char *panicstr;

	if (panicstr)
		constty = 0;
	if ((flags & TOCONS) && tp == 0 && constty) {
		tp = constty;
		flags |= TOTTY;
E 4
	}
I 4
	if (flags & TOTTY) {
		register s = spltty();
E 4

D 4
	if (uap->count < putbufsz) {
		u.u_error = ENOSPC;
		return;
E 4
I 4
D 5
		if (tp && (tp->t_state & (CARR_ON | ISOPEN)) ==
E 5
I 5
		if (tp->t_outq.c_cc > 2048) {
			putchar(c);
		}
		else if (tp && (tp->t_state & (CARR_ON | ISOPEN)) ==
E 5
		    (CARR_ON | ISOPEN)) {
			if (c == '\n')
				(void) putc('\r', &tp->t_outq);
			(void) putc(c, &tp->t_outq);
			(*tp->t_proc)(tp, T_OUTPUT);
		} else if ((flags & TOCONS) && tp == constty)
			constty = 0;
		splx(s);
E 4
	}
I 4
	if ((flags & TOLOG) && c != '\0' && c != '\r' && c != 0177) {
		if (msgbuf.msg_magic != MSG_MAGIC) {
			register int i;
E 4

D 4
	u.u_rval1 = 0;
	while (1) {
		while ((putbufget == putbufput) && (u.u_rval1 == 0))
			sleep (putbuf, PUSER);
		if (putbufget <= putbufput) {
			cnt = putbufput - putbufget;
			if (cnt == 0)
				return;
			if (copyout(&putbuf[putbufget], uap->cbuf, cnt))
				u.u_error = EFAULT;
			else {
				putbufget = putbufput; 
				u.u_rval1 += cnt;
			}
			return;
E 4
I 4
			msgbuf.msg_magic = MSG_MAGIC;
			msgbuf.msg_bufx = msgbuf.msg_bufr = 0;
			for (i=0; i < MSG_BSIZE; i++)
				msgbuf.msg_bufc[i] = 0;
E 4
		}
D 4
		cnt = putbufsz - putbufget;
		if (cnt && copyout(&putbuf[putbufget], uap->cbuf, cnt)) {
			u.u_error = EFAULT;
			return;
		}
		u.u_rval1 = cnt;
		putbufget = 0;
		uap->cbuf += cnt;
		uap->count -= cnt;
E 4
I 4
		msgbuf.msg_bufc[msgbuf.msg_bufx++] = c;
		if (msgbuf.msg_bufx < 0 || msgbuf.msg_bufx >= MSG_BSIZE)
			msgbuf.msg_bufx = 0;
E 4
	}
I 4
	if ((flags & TOCONS) && constty == 0 && c != '\0')
		putchar(c);
E 4
E 3
E 2
}
E 1
