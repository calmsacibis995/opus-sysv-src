#ifndef __Isignal
#define __Isignal

#include<sys/signal.h>

typedef struct sigset {
	unsigned long s[2];
} sigset_t;

struct sigaction {
	void	 (*sa_handler)();
	sigset_t sa_mask;
	int	 sa_flags;
};

#ifdef m88k
#define	sigmask(s)	(1L << (31  - (((s) - 1) & 0x1f)))
#define	sigoff(s)	(((s) - 1) >> 5)

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
	long	si_version,
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

typedef volatile int sig_atomic_t;

extern void (*signal())();
extern int raise(int);

#endif
