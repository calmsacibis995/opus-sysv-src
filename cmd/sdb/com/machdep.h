/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:com/machdep.h	1.19"

#ifndef MACHINCL
#define MACHINCL

#include	<sys/param.h>
#include	<sys/types.h>
#include	<signal.h>
#if vax || u3b || clipper || ns32000
#include	<sys/page.h>
#endif
#if clipper || ns32000
#include	<sys/sbd.h>
#endif

#if u3b
#include	<sys/macro.h>	/* has stob, sys/seg.h below (also has ptob) */
#else
#if vax || u3b5 || u3b15 || u3b2 || clipper || ns32000 || m88k
#include	<sys/sysmacros.h>
#endif
#endif

#include	<sys/fs/s5dir.h>

#if u3b
#include	<sys/seg.h>	/* needed for USRSTACK */
#include	<sys/istk.h>	/* needed for istk structure in struct user */
#else
#if u3b2
#define USRSTACK	0xc0020000
#define TXTSTART	0x80800000
#else
#if u3b5 || u3b15
#define USRSTACK	0xf00000
#define TXTSTART	0x800000
#else
#if ns32000
#define SELF_P_082	123
#define NCPS_082	64
#define	stoc_082(x)	((x) * NCPS_082)
#define MAXUMEM_082	(stoc_082(SELF_P_082*2))
#define NBPC_082	1024
#define UVSTACK_082	((unsigned)MAXUMEM_082*NBPC_082)
#define USRSTACK	(mmu_382 ? UVSTACK : UVSTACK_082)
#define TXTSTART	0x0
#else
#if clipper
#define USRSTACK	0x0c0000000	/* PORT should be in sys/param.h */
#define TXTSTART	0x0
#endif
#endif
#endif
#endif
#endif

#if u3b2 || clipper || ns32000 || m88k
#include	<sys/immu.h>
#include	<sys/psw.h>
#if !defined(SWAPPING) && !defined(m88k)
#include	<sys/pcb.h>
#endif
#endif

#if m88k
#ifndef OPUSVENDORID
#define OPUSVENDORID 0xf00
#endif
#include	<sys/ptrace.h>
#else
#include	<sys/user.h>
#endif
#include	<sys/errno.h>
#include	<a.out.h>
#include	<sys/reg.h>
#include	<sys/stat.h>

#if u3b || u3b5 || u3b15 || u3b2
#define SUBCALL		(isubcall(dot,ISP))	/* subroutine call instruction*/
#if u3b
#define SUBRET		get(SDBREG(SP)-8, DSP)	/* subroutine ret instruction */
#else
extern int regvals[];
extern int fpregvals[];
extern int asrval;
extern int drval[];

#define SUBRET		get(regvals[12]-2*WORDSIZE,DSP)
#endif
#define RETREG		0			/* register with ret value    */

/* Given the current fp (frame), these are the next frame pc, ap, fp */
#if u3b
#define XTOB(x) (ptob(x))

#define NEXTCALLPC (frame - (13*WORDSIZE))
#define NEXTARGP   (frame - (12*WORDSIZE))
#define NEXTFRAME  (frame - (11*WORDSIZE))
#ifndef	USEG
#define	USEG	0x5
#endif

#define TXTRNDSIZ	0x20000L	/* = 128K = (1L<<17)	*/

/* Address of UBLOCK (absolute) is beginning of segment 5 */
#define ADDR_U ((unsigned) (USEG * TXTRNDSIZ))

#define BKOFFSET   10	/* offset from beginning of proc to bkpt (no -O) */

#define APNO	9	/* argument pointer register number */
#define FPNO	10	/* frame pointer register number */
#define SAVEDREGS	13	/* max # of registers saved by 'SAVE'  */
#else
#define XTOB(x) (ctob(x))

#define NEXTCALLPC	(frame-(9*WORDSIZE))
#define NEXTARGP	(frame-(8*WORDSIZE))
#define NEXTFRAME	(frame-(7*WORDSIZE))

#define TXTRNDSIZ	0x80000L

#if u3b
#define BKOFFSET	9	/* offset from start of proc to bkpt */
#else
#if u3b5 || u3b15 || u3b2
#define BKOFFSET	0	/* offset from start of proc to bkpt */
#define SAVEDREGS	9	/* max # of registers saved by 'SAVE'  */
#endif
#endif


#define APNO		10
#define FPNO		9
#endif
#define	PROCOFFSET	0	/* offset from label to first instruction */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define NUMREGLS 16	/* number of "registers" in reglist[] */

#define ISREGVAR(regno)	(3<=(regno) && (regno) <= 8)

#else
#if vax
#define XTOB(x) (ctob(x))
#define SUBCALL		((get(dot,ISP) & 0xff) == 0xfb) /* subroutine call */
#define SUBRET		get(SDBREG(SP)+16, DSP)	/* subroutine ret instruction */

#define RETREG		0			/* register with ret value */

/* Given the current fp (frame) and ap (arg ptr), these are the next */
/*  frame pc, ap, fp */

#define TXTRNDSIZ 512L

#define ADDR_U ((unsigned) 0x7ffff800)	/* absolute address of UBLOCK */

#define	PROCOFFSET	2	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define APNO	12	/* argument pointer register number */
#define FPNO	13	/* frame pointer register number */
#define NUMREGLS 17	/* number of "registers" in reglist[] */

#define ISREGVAR(regno) (6 <= (regno) && (regno) < 12)
#endif
#if ns32000
#define XTOB(x) (ctob(x))
							 /* jsr or bsr */
#define SUBCALL	(((get(dot,ISP)&0x7ff)==0x67f) || ((get(dot,ISP)&0xff)==2))
#define SUBRET		get(SDBREG(SP), DSP)	/* sub ret instruction */
#define BRANCH	((get(dot,ISP)&0xff)==0xea)
#define JUMP	((get(dot,ISP)&0x7ff)==0x27f)
#define ENTER	((get(dot,ISP)&0xff)==0x82)

#define RETREG		0			/* register with ret value */

/* Given the current fp (frame), these are the next frame pc, ap, fp */
#define NEXTCALLPC (frame + (1*WORDSIZE))
#define NEXTARGP   (frame)
#define NEXTFRAME  (frame)

#define TXTRNDSIZ 512L

extern INT mmu_382;
#define SNUMSHFT_082	16
#define stob_082(x)	((((unsigned)(x)) & 0x1fff) << SNUMSHFT_082)
#define U1_SEG_082	128
#define UDOT_P_082	(SELF_P_082+1)	
#define UPTBL_082	((unsigned)stob_082(U1_SEG_082))	/* 0x800000 */
#define NBPPT_082	512
#define UBLK_082	((unsigned)UPTBL_082+(UDOT_P_082*2*NBPPT_082) )
#define ADDR_U (mmu_382 ? UBLK : UBLK_082)	/* absolute address of UBLOCK */

#define	PROCOFFSET	0	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define APNO	FP	/* argument pointer register number */
#define FPNO	FP	/* frame pointer register number */
#define NUMREGLS 12	/* number of "registers" in reglist[] */
#define NUMFREGS 9	/* number of floating point "registers" */
#define STACKVAR	0xC00000

#define ISREGVAR(regno) (4 <= (regno) && (regno) < 7)

#define SAVEDREGS	13	/* max # of registers saved by 'SAVE'  */
#endif
#if clipper
#define XTOB(x) (ctob(x))
#define SUBCALL ((((get(dot-2,ISP) & 0xffff) == 0x14f5) && ((get(dot,ISP) & 0xfe00) == 0x4800)) || ((get(dot,ISP) & 0xff) == 0x44))    /* subroutine call */
#define BRANCH ((get(dot,ISP) & 0xffff) == 0x4990)
#define SUBRET		get(SDBREG(SP), DSP)	/* subroutine ret addr on stk */

#define RETREG		0			/* register with ret value */

/* Given the current fp (frame), these are the next frame pc, fp */
#define NEXTCALLPC (frame + (1*WORDSIZE))
#define NEXTARGP  (frame + (0*WORDSIZE))
#define NEXTFRAME  (frame + (0*WORDSIZE))

#define ADDR_U	((unsigned) 0xf0000000)	/* absolute address of UBLOCK */

#define	PROCOFFSET  0	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define	R0NO	0	/* R0 register */
#define	R1NO	1	/* R1 register */
/* #define APNO	13 */	/* No argument pointer register on CLIPPER */
#define FPNO	14	/* Frame pointer register */
#define SPNO	15	/* Stack pointer register */
#define	F0NO	16	/* F0 register */
#define	F0xNO	17	/* F0x register */
#define	F1NO	18	/* F1 register */
#define	F1xNO	19	/* F1x register */
#define SSWNO	32	/* SSW register */
#define	PSWNO	33	/* PSW register */
#define PCNO	34	/* PC register */

#define NUMREGLS 35	/* number of "registers" in reglist[] */
#define ISREGVAR(reg)	((0 <= (reg) && (reg) <= 14) ||\
			(16 <= (reg) && (reg) <= 31))
#define STACKVAR	0xC0000000

#define SAVEDREGS	13	/* max # of registers saved by 'SAVE'  */
#endif
#if m88k
#define XTOB(x) (ctob(x))
#define SUBCALL (((get(dot,ISP) & 0xf8000000) == 0xc8000000) /* bsr, bsr.n */ \
	|| ((get(dot,ISP) & 0xfc00fbe0) == 0xf400c800))      /* jsr */
#define BRANCH (((get(dot,ISP) & 0xfc000000) == 0xc0000000)  /* br */ \
	|| ((get(dot,ISP) & 0xffffffe0) == 0xf400c000) /* jmp */ \
	|| ((get(dot,ISP) & 0xf4000000) == 0xd0000000) /* bb0, bb1 */ \
	|| ((get(dot,ISP) & 0xf4000000) == 0xe8000000))	/* bcnd */
#define BRANCHPLUS (((get(dot,ISP) & 0xfc000000) == 0xc4000000) /* br.n */ \
	|| ((get(dot,ISP) & 0xffffffe0) == 0xf400c400) /* jmp.n */ \
	|| ((get(dot,ISP) & 0xf4000000) == 0xd4000000)  /* bb0.n, bb1.n */ \
	|| ((get(dot,ISP) & 0xf4000000) == 0xec000000))	/* bcnd.n */
#define SUBRET		SDBREG(R1NO)	/* subroutine ret addr in r1 */

#define RETREG		R2NO			/* register with ret value */

/* Given the current argp (arg ptr), these are the next argp pc, ap, fp */
#define NEXTCALLPC ((((unsigned)get(argp-2*WORDSIZE,DSP)) && \
		((unsigned)get(argp-2*WORDSIZE,DSP)<(unsigned)0xe0000000)) ? \
		argp - (2*WORDSIZE) : argp - (1*WORDSIZE))
#define NEXTARGP ((((unsigned)get(argp-2*WORDSIZE,DSP)) && \
		((unsigned)get(argp-2*WORDSIZE,DSP)<(unsigned)0xe0000000)) ? \
		argp - (3*WORDSIZE) : argp - (2*WORDSIZE))
#define NEXTFRAME argp	/* must be called before NEXTARGP */

#define	PROCOFFSET  0	/* offset from label to first instruction */
#define BKOFFSET    0	/* offset from beginning of proc to bkpt */
#define ALTBKOFFSET 0	/* offset from beginning of proc to bkpt */

#define	R0NO	0	/* R0 register */
#define	R1NO	1	/* R1 register */
#define	R2NO	2	/* R2 register */
#define APNO	30 	/* Argument pointer register */
#define FPNO	31	/* Frame pointer register */
#define SPNO	31	/* Stack pointer register */
#define SRNO	32	/* PS register */
#define	PSNO	32	/* PS register */
#undef SCIP
#undef SNIP
#undef SFIP
#undef PC
#define SCIP	252	/* Current instruction pointer register */
#define PCNO	253	/* PC register */
#define PC	253	/* PC register */
#define SNIP	253	/* Next instruction pointer register */
#define SFIP	254	/* Fetch instruction pointer register */

#define NUMREGLS 36	/* number of "registers" in reglist[] */
#define ISREGVAR(reg)	((0 <= (reg) && (reg) <= 31))
#define STACKVAR	0xf0000000

#define SAVEDREGS	ERROR	/* max # of registers saved by 'SAVE'  */

/*#define NOBACKUP 1		/* set to 1 if machine does not back up */
			        /* to previous instruction at exception */
#endif
#endif

extern char uu[XTOB(USIZE)];

extern ADDR	callpc, frame, argp;	/* current stack frame */

#define WORDSIZE (sizeof(int))	/* wordsize in bytes on this machine */
#define REGSIZE WORDSIZE	/* register size in bytes on this machine */

#ifndef NOBACKUP
#define NOBACKUP 0		/* set to 1 if machine does not back up */
#endif
			        /* to previous instruction at exception */
#define ADDRTYPE	"d"	/* type of address for getval */

#if u3b2
#define MAXPOS	0xC005FFFF
#else
#if m88k
#define MAXPOS	0xf0000000		/* maximum address */
#else
#define MAXPOS	0x7ffffff		/* maximum address */
#endif
#endif

/*  two extra numbers to be printed with regs; in optab.c */
/*  removed because these are not offsets from R0; can't use SDBREG */
/*
#define VAL1	((unsigned)&(((struct user *) 0)->u_rval1)
#define VAL2	((unsigned)&(((struct user *) 0)->u_rval2)
*/

/* ptracew modes */
#define	SETTRC	0
#define	RDUSER	2
#define	RIUSER	1
#define	WDUSER	5
#define WIUSER	4
#define	RUREGS	3
#define	WUREGS	6
#define	CONTIN	7
#define	EXIT	8
#define SINGLE	9

extern REGLIST reglist [];

#if vax || u3b || clipper || ns32000
#define SDBREG(A) (((struct user *)uu)->u_ar0[A])
#define SYSREG(A) ((int) (((char *) (&SDBREG(A)) - ((int) uu))))
#endif

#if m88k
#define user ptrace_user
#define u_tsize pt_o_tsize
#define u_dsize pt_dsize
#define u_ssize pt_ssize
#define SDBREG(A) (((long *)(&((struct user *)uu)->pt_r0))[A])
#define SYSREG(A) ((int) (((char *) (&SDBREG(A)) - ((int) uu))))
#endif

#if ns32000
#define SDBFREG(A) (((struct user *)uu)->u_fps[A])
#define FPREG(A)  ((int) (((char *) (&SDBFREG(A)) - ((int) uu))))
#endif

#if m88k || clipper
#define NUMREGS 32
#else
#define NUMREGS 16	/* number of general purpose registers */
#endif

#if vax || u3b || clipper || ns32000 || m88k
#define ISREGN(reg)	(0<= (reg) && (reg) < NUMREGS)
#else
#if u3b5 || u3b15 || u3b2
#define ISREGN(reg)	(0 <= (reg) && (reg) < NUMREGS && (reg) != 13 && (reg) != 14)
#endif
#endif

#if vax || u3b || clipper || ns32000
#define USERPC  SDBREG(PC)
#else
#if m88k
#define USERPC  (SDBREG(PC)&~3)
#else
#if u3b5 || u3b15 || u3b2
#define USERPC	regvals[15]
#endif
#endif
#endif

union word {
	char c[WORDSIZE]; 
	unsigned char cu[WORDSIZE];
	short s[WORDSIZE/2];
	unsigned short su[WORDSIZE/2];
	int w;
	unsigned int iu;
	long l;
	float f;
};
union dbl {
	struct {
		int w1, w2;
	} ww;
	double dd;
	int i;
	float f;
	char cc[WORDSIZE*2];
};
#endif /* MACHINCL */
