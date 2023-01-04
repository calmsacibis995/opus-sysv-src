h28194
s 00175/00000/00000
d D 1.1 90/03/06 12:30:17 root 1 0
c date and time created 90/03/06 12:30:17 by root
e
u
U
t
T
I 1
#ifndef _SYS_SIGNAL_H_
#define _SYS_SIGNAL_H_



/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt (rubout) */
#define	SIGQUIT	3	/* quit (ASCII FS) */
#define	SIGILL	4	/* illegal instruction (not reset when caught)*/
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define SIGABRT 6	/* used by abort, replace SIGIOT in the  future */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* death of a child */
#define	SIGPWR	19	/* power-fail restart */
#define SIGWINCH	20
#define SIGPOLL 22	/* pollable event occured */

#define SIGSTOP 23
#define	SIGTSTP	24
#define SIGCONT 25
#define	SIGTTIN	26
#define SIGTTOU 27

#define SIGURG	33
#define SIGIO	34
#define SIGXCPU	35
#define SIGXFSZ	36
#define SIGVTALRM	37
#define SIGPROF	38
#define SIGLOST	40

#define SIGIOT	SIGABRT
#define	SIGCHLD	SIGCLD

#define	NSIG	65	/* The valid signal number is from 1 to NSIG-1 */
#define MAXSIG	64	/* size of u_signal[], NSIG-1 <= MAXSIG*/
			/* MAXSIG is larger than we need now. */
			/* In the future, we can add more signal */
			/* number without changing user.h */

#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

#if lint
#define SIG_ERR (void(*)())0
#else
#define SIG_ERR	(void(*)())-1
#endif
#define	SIG_DFL	(void(*)())0
#if lint
#define	SIG_IGN	(void (*)())0
#else
#define	SIG_IGN	(void (*)())1
#endif
#if lint
#define SIG_HOLD (void(*)())0
#else
#define SIG_HOLD (void(*)())2
#endif

#ifndef _SIGSET_T
#define	_SIGSET_T
typedef struct sigset { unsigned long s[2]; } sigset_t;
#endif

struct sigaction {
	void		(*sa_handler) ();
	sigset_t	sa_mask;
	int		sa_flags;
};

#define SA_NOCLDSTOP	1

#define	sigmask(s)	(1L << (31  - (((s) - 1) & 0x1f)))
#define	sigoff(s)	(((s) - 1) >> 5)

#ifndef _SIGFRAME_
#define	_SIGFRAME_
struct sigframe {
	long	sf_r31;
	long	sf_id;
	long	sf_sxip;
	long	sf_snip;
	long	sf_sfip;
	long	sf_sigsource;
	long	sf_signum;
	long	sf_numexceptblock;
	long	sf_exceptblock[1];
};

struct exceptblock {
	long eb_numarg;
	long eb_arg[1];
};

struct siginfo {
	long	si_version,		/* will be 1 for this structure */
		si_r1,
		si_r2,
		si_r3,
		si_r4,
		si_r5,
		si_r6,
		si_r7,
		si_r8,
		si_r9,
		si_r10,
		si_r11,
		si_r12,
		si_r13,
		si_r14,
		si_r15,
		si_r16,
		si_r17,
		si_r18,
		si_r19,
		si_r20,
		si_r21,
		si_r22,
		si_r23,
		si_r24,
		si_r25,
		si_r26,
		si_r27,
		si_r28,
		si_r29,
		si_r30,
		si_psr,
		si_fpsr,
		si_fpcr;
	struct	sigframe	*si_sigframe;
};
#endif

#ifndef __88000_OCS_DEFINED

/* The following definitions are only used by kernel (for 3b2 only) */
/* These defines are also needed by the libraries */
#define SIGNO_MASK	0xff
#define SIGDEFER	0x100
#define SIGHOLD		0x200
#define SIGRELSE	0x400
#define SIGIGNORE	0x800
#define SIGPAUSE	0x1000

/* SIGFPE codes */

#define	KINTOVF	0x80000001	/* integer overflow */
#define	KINTDIV	0x80000002	/* integer divide by zero */
#define	KSUBRNG	0x80000003	/* subscript range */

#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_SIGNAL_H_ */
E 1
