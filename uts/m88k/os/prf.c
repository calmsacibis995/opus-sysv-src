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
#include "sys/signal.h"
#include "sys/fs/s5dir.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/tty.h"
#include "sys/msgbuf.h"

/* general argument list for cmn_err and panic */

# define ARGS	a0,a1,a2,a3,a4,a5

/*	Save output in a buffer where we can look at it
**	with demon or crash.  If the message begins with
**	a '!', then only put it in the buffer, not out
**	to the console.
*/

#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4

short		prt_where = PRW_BUF | PRW_CONS;
struct tty *constty;

#define	output(c) \
	if (prt_where & PRW_BUF) { \
		outputc(c, TOLOG, (struct tty *) 0); \
	} \
	if (prt_where & PRW_CONS) \
		outputc(c, TOCONS, (struct tty *) 0);

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
			logwakeup();
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
	constty = 0;
	printf("panic: ");
	printf(fmt, ARGS);
	if (!panicstr && fmt) {
		panicstr = fmt;		/* save panic string */
		if (!noPanicSync)
			update();	/* "sync" */
	}

	breakpoint();
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
			constty = 0;
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
	extern int	availrmem;
	extern int	availsmem;

	cmn_err(CE_NOTE,
		"%s - Insufficient memory to%s %d%s page%s - %s",
		func, (lockflg ? " lock" : " allocate"),
		count, (contflg ? " contiguous" : ""),
		(count == 1 ? "" : "s"),
		"system call failed");

	procname(0);
	printf("System call #%d, rmem %d, smem %d", 
		u.u_syscall, availrmem, availsmem);
}

prfsys()
{
	register struct a {
		int	cmd;
		int	val;
	} *uap;

	if (!suser())
		return;

	uap = (struct a *)u.u_ap;

	if (uap->cmd == 1) {
		u.u_rval1 = prt_where;
		prt_where = (uap->val & (PRW_BUF|PRW_CONS));
		return;
	}

	u.u_error = EINVAL;
}

/*
 * Print a character on console or users terminal.
 * If destination is console then the last MSGBUFS characters
 * are saved in msgbuf for inspection later.
 */
/*ARGSUSED*/
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
	}
	if (flags & TOTTY) {
		register s = spltty();

		if (tp->t_outq.c_cc > 2048) {
			putchar(c);
		}
		else if (tp && (tp->t_state & (CARR_ON | ISOPEN)) ==
		    (CARR_ON | ISOPEN)) {
			if (c == '\n')
				(void) putc('\r', &tp->t_outq);
			(void) putc(c, &tp->t_outq);
			(*tp->t_proc)(tp, T_OUTPUT);
		} else if ((flags & TOCONS) && tp == constty)
			constty = 0;
		splx(s);
	}
	if ((flags & TOLOG) && c != '\0' && c != '\r' && c != 0177) {
		if (msgbuf.msg_magic != MSG_MAGIC) {
			register int i;

			msgbuf.msg_magic = MSG_MAGIC;
			msgbuf.msg_bufx = msgbuf.msg_bufr = 0;
			for (i=0; i < MSG_BSIZE; i++)
				msgbuf.msg_bufc[i] = 0;
		}
		msgbuf.msg_bufc[msgbuf.msg_bufx++] = c;
		if (msgbuf.msg_bufx < 0 || msgbuf.msg_bufx >= MSG_BSIZE)
			msgbuf.msg_bufx = 0;
	}
	if ((flags & TOCONS) && constty == 0 && c != '\0')
		putchar(c);
}
