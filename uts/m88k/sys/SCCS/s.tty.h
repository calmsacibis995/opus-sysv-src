h01531
s 00002/00002/00230
d D 1.3 91/04/11 18:02:54 root 3 2
c Expanded control char array to Posix size
e
s 00003/00000/00229
d D 1.2 90/11/28 16:49:59 root 2 1
c added mapchan to the tty structure
e
s 00229/00000/00000
d D 1.1 90/03/06 12:30:41 root 1 0
c date and time created 90/03/06 12:30:41 by root
e
u
U
t
T
I 1
#ifndef _SYS_TTY_H_
#define _SYS_TTY_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * A clist structure is the head of a linked list queue of characters.
 * The routines getc* and putc* manipulate these structures.
 */

struct clist {
	int	c_cc;		/* character count */
	struct cblock *c_cf;	/* pointer to first */
	struct cblock *c_cl;	/* pointer to last */
};

/* Macro to find clist structure given pointer into it	*/
#define CMATCH(pointer)		(char *)(cfree + (pointer - cfree))

/* Character control block for interrupt level control	*/

struct ccblock {
	caddr_t	c_ptr;		/* buffer address	*/
	ushort	c_count;	/* character count	*/
	ushort	c_size;		/* buffer size		*/
};

#ifndef _WINSIZE_
#define	_WINSIZE_
struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};
#endif

I 2
/* Needed for mapchan */
#include "sys/mapchan.h"
E 2
/*
 * A tty structure is needed for each UNIX character device that
 * is used for normal terminal IO.
 */
D 3
#define	NCC	8
E 3
I 3
#define	NCCS	19
E 3
struct tty {
	struct	clist t_rawq;	/* raw input queue */
	struct	clist t_canq;	/* canonical queue */
	struct	clist t_outq;	/* output queue */
	struct	ccblock	t_tbuf;	/* tx control block */
	struct	ccblock t_rbuf;	/* rx control block */
	int	(* t_proc)();	/* routine for device functions */
	struct	proc *t_rsel;	/* process who selected this device for read */
	struct	proc *t_wsel;	/* process who selected this device for write */
	ushort	t_xstate;	/* bsd select support */
	unchar	t_minor;	/* minor number */
	unchar	t_pad;		/* pad */
	ushort	t_iflag;	/* input modes */
	ushort	t_oflag;	/* output modes */
	ushort	t_cflag;	/* control modes */
	ushort	t_lflag;	/* line discipline modes */
	ulong	t_state;	/* internal state */
	short	t_pgrp;		/* process group name */
	char	t_line;		/* line discipline */
	char	t_delct;	/* delimiter count */
	char	t_term;		/* terminal type */	
	char	t_tmflag;	/* terminal flags */
	char	t_col;		/* current column */
	char	t_row;		/* current row */
	char	t_vrow;		/* variable row */
	char	t_lrow;		/* last physical row */
	char	t_hqcnt;	/* no. high queue packets on t_outq */	
	char	t_dstat;	/* used by terminal handlers
						and line disciplines */
	struct	winsize	t_winsize;
	ulong	t_brktime;
D 3
	unsigned char	t_cc[NCC];	/* settable control chars */
E 3
I 3
	unsigned char	t_cc[NCCS];	/* settable control chars */
E 3
I 2
	struct	mctbl	t_mctbl;	/* per channel mapchan info */
E 2
};

/*
 * The structure of a clist block
 */
#define	CLSIZE	64
struct cblock {
	struct cblock *c_next;
	char	c_first;
	char	c_last;
	char	c_data[CLSIZE];
};

#if 1
/* opus clists are dynamically allocated at boot time */
extern struct cblock	*cfree;
#else
extern struct cblock	cfree[];
#endif
extern struct cblock	*getcb();
extern struct cblock	*getcf();
extern struct clist	ttnulq;
extern int		cfreecnt;

struct chead {
	struct cblock *c_next;
	int	c_size;
	int	c_flag;
};
extern struct chead cfreelist;

struct inter {
	int	cnt;
};

#define	QESC	0200	/* queue escape */
#define	HQEND	01	/* high queue end */

#define	TTIPRI	28
#define	TTOPRI	29
#define	TTECHI	80

/* limits */
extern int ttlowat[], tthiwat[];
#define	TTYHOG	256
#define	TTXOLO	132
#define	TTXOHI	180

/* Hardware bits */
#define	DONE	0200
#define	IENABLE	0100
#define	OVERRUN	040000
#define	FRERROR	020000
#define	PERROR	010000

/* Internal state */
#define	TIMEOUT	0x01		/* Delay timeout in progress */
#define	WOPEN	0x02		/* Waiting for open to complete */
#define	ISOPEN	0x04		/* Device is open */
#define	TBLOCK	0x08
#define	CARR_ON	0x10		/* Software copy of carrier-present */
#define	BUSY	0x20		/* Output in progress */
#define	OASLP	0x40		/* Wakeup when output done */
#define	IASLP	0x80		/* Wakeup when input done */
#define	TTSTOP	0x100		/* Output stopped by ctl-s */
#define	EXTPROC	0x200		/* External processing */
#define	TACT	0x400
#define	CLESC	0x800		/* Last char escape */
#define	RTO	0x1000		/* Raw Timeout */
#define	TTIOW	0x2000		/* wait for I/O to complete */
#define	TTXON	0x4000		/* XON state */
#define	TTXOFF	0x8000		/* XOFF state */
#define	RCOLL	0x10000		/* collision in read select */
#define	WCOLL	0x20000		/* collision in write select */

/* tty_state flags. These are flags that should be in t_state but there is
   no room left and the tty structure must not be expanded at this time. */
#define SCON	0x01		/* system console */
#define DCON	0x02		/* dual console */
#define DCCLOSE	0x04		/* close port when dual console turned off */ 
#define TTDCD	0x08		/* DCD state */
#define TTBRK	0x10		/* BREAK state */
#define TTCTS	0x20		/*  CTS state */
#define TTWCLOSE 0x40		/*  Wait for close to complete */

/* l_output status */
#define	CPRES	0100000

/* device commands */
#define	T_OUTPUT	0
#define	T_TIME		1
#define	T_SUSPEND	2
#define	T_RESUME	3
#define	T_BLOCK		4
#define	T_UNBLOCK	5
#define	T_RFLUSH	6
#define	T_WFLUSH	7
#define	T_BREAK		8
#define	T_INPUT		9
#define T_DISCONNECT	10
#define	T_PARM		11
#define	T_SWTCH		12
#define	T_CTSUSPEND	13	/* opus codes */
#define	T_CTRESUME	14
#define	T_CTBLOCK	15
#define	T_CTUNBLOCK	16
#define	T_DRAINOUT	17
#define	T_PXBLOCK	18
#define	T_PXUNBLOCK	19

/*
 * Terminal flags (set in t_tmflgs).
 */

#define SNL	1		/* non-standard new-line needed */
#define ANL	2		/* automatic new-line */
#define LCF	4		/* Special treatment of last col, row */
#define TERM_CTLECHO	010	/* Echo terminal control characters */
#define TERM_INVIS	020	/* do not send escape sequences to user */
#define QLOCKB		040	/* high queue locked for base level */
#define QLOCKI		0100	/* high queue locked for interrupts */
#define	TERM_BIT 0200		/* Bit reserved for terminal drivers. */
				/* Usually used to indicate that an esc*/
				/* character has arrived and that the  */
				/* next character is special.          */
				/* This bit is the same as the TM_SET  */
				/* bit which may never be set by a user*/
/*
 *	device reports
 */
#define	L_BUF		0
#define	L_BREAK		3

/*
 *	xstate flags
 */
#define	TS_RCOLL	1
#define	TS_NONBIO	2
#define	TS_ASYNC	4
#define	TS_WCOLL	8

#endif	/* ! _SYS_TTY_H_ */
E 1
