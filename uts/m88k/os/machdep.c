/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/machdep.c	10.15"

#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/psw.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/user.h"
#include "sys/var.h"
#include "sys/reg.h"

/*
 *	Read time of day chip
 */
extern time_t time;

time_t clkread()
{
	return(time);
}

newsig()
{
	return ((kdebug & 0x80) ? 0 : 1);
}

/*
 * Send an interrupt to process
 */
sendsig(hdlr, signo, arg, mask0, mask1)
caddr_t hdlr;
register signo;
{
	register *usp, *regs;

	regs = u.u_ar0;
	usp = (int *)regs[SP];

#ifdef m88k
	if (kdebug & 8) {
		printf("pid %d signo %d pc  0x%x sp 0x%x arg = 0x%x\n", 
		u.u_procp->p_pid, signo, regs[PC], usp, arg );
	}

	if (grow((unsigned)(usp-20)) == -1) {		/* 20 integers */
		return 0;
	}

	/* Save registers to be restored later on the user's stack */
	if (newsig())
		suword((caddr_t)--usp, 0);		/* 8 byte align */
	suword((caddr_t)--usp, mask1);		/* return mask. */
	suword((caddr_t)--usp, mask0);		/* return mask. */
	if (newsig() == 0) {
	suword((caddr_t)--usp, regs[PS]);     /* Status register. */
	suword((caddr_t)--usp, regs[R1]);     /* parameter regs. */
	suword((caddr_t)--usp, regs[R2]);
	suword((caddr_t)--usp, regs[R3]);
	suword((caddr_t)--usp, regs[R4]);
	}
	suword((caddr_t)--usp, u.u_siginfo.args[3]);
	suword((caddr_t)--usp, u.u_siginfo.args[2]);
	suword((caddr_t)--usp, u.u_siginfo.args[1]);
	suword((caddr_t)--usp, u.u_siginfo.args[0]);
	if (u.u_siginfo.nwords)
		suword((caddr_t)--usp, u.u_siginfo.nwords);
	else
		suword((caddr_t)--usp, 1);
	u.u_siginfo.nwords = 0;
	suword((caddr_t)--usp, 1);		/* 1 exception block */
	suword((caddr_t)--usp, signo);		/* signal number */
	if (u.u_siginfo.src == 0) {
		if (signo == SIGBUS || signo == SIGSEGV || 
				signo == SIGILL || signo == SIGFPE)
			u.u_siginfo.src = 1;
		else
			u.u_siginfo.src = 2;
	}
	suword((caddr_t)--usp, u.u_siginfo.src);/* signal source */
	u.u_siginfo.src = 0;
	suword((caddr_t)--usp, regs[SFIP]);   /* interrupted PC. */
	suword((caddr_t)--usp, regs[SNIP]);   /* interrupted PC. */
	suword((caddr_t)--usp, regs[SCIP]);   /* interrupted PC. */
	suword((caddr_t)--usp, 1);		/* sig frame format id */
	suword((caddr_t)--usp, regs[SP]);     /* Stack pointer. */

	regs[SP] = (int)usp;
	if (newsig() == 0) {
	regs[R1] = regs[SNIP] + 1;
	regs[R2] = signo;
	regs[R3] = arg;
	regs[R4] = u.u_traptype;
	}
	regs[SFIP] = (int)hdlr | VALIDIP;
	regs[SNIP] &= ~VALIDIP;
	return (1);

#else

	  /* For M68881 exceptions */
	if(u.u_traptype == TRAPFPE) {
		   /* Save data and copy to u_fpu.excop */
		fsave(u.u_fpu.fsave);
		bcopy(&u.u_fpu.fsave[8],&u.u_fpu.excop,sizeof(u.u_fpu.excop));
		u.u_fpu.ustate = UFPSAVE;  /* u_fpu.excop has valid data */
		u.u_trapflag = 1;          /* Save stack frame */
	} else {
		if((u.u_ar0[18] & 0xF000) == 0x9000) {
			  /*  This is a co-processor mid-instruction      */
			  /*  frame.  We do not execute the signal        */
			  /*  handler, but save the signal and arg in     */
			  /*  the U-block, then force a trap by setting   */
			  /*  the trace bit.  On return, the instruction  */
			  /*  will complete, then trap.  At that time     */
			  /*  we will retrieve signal and arg and exe-    */
			  /*  cute the handler.                           */
			u.u_ar0[PS] |= PS_T1;           /* Force trap   */
			u.u_trapsig = signo;            /* Save signal  */
			u.u_trapcode = arg;             /* Save arg     */
			  /* Re-arm the signal */
			u.u_signal[signo - 1] = (void (*)())hdlr;
			return;                 /* Don't do handler */
		}
	}

	if (u.u_traptype) {
		regs[PC] |= 1;	/* make odd to prevent sucessful 'rtr' */
	}
	if (grow((unsigned)(usp-4)) == -1) {		/* 5 integers */
		return;
	}
	/* simulate an interrupt on the user's stack */
	suword((caddr_t)--usp, regs[PC]);
	suword((caddr_t)--usp, (regs[PS] & 0xffff) | (u.u_traptype << 16));
	suword((caddr_t)--usp, arg);
	suword((caddr_t)--usp, signo);

	regs[SP] = (int)usp;
	regs[PS] &= ~(PS_T1 | PS_T0);
	regs[PC] = (int)hdlr;
#endif
}

/*
 * Clear registers on exec
 */

setregs()
{
	register int	i;
	void (* *rp)();

#ifdef	m88k
	for (i = 0; i < 31; i++) 
		u.u_ar0[i] = 0;	/* ignore PS and PC */

	setcr62(0);
	setcr63(0);

		/* special register for 88100; initial pc */
		/* it will have 8 added upon return */
	u.u_ar0[SCIP] = u.u_exdata.ux_entloc - 8;
#else
	for (i = 0; i < 15; i++) 
		u.u_ar0[i] = 0;	/* ignore PS and PC */

		/* special registers for 68000; initial pc */
	u.u_ar0[PC] = u.u_exdata.ux_entloc;

		/* Initial stack region */
	u.u_rval1 = u.u_ar0[R8] = USRSTACK - ctob(u.u_ssize);
	u.u_rval2 = 0;
#endif

	/* Any pending signal remain held, so don't clear p_hold and
	p_sig */	

	/* If the action was to catch the signal, then the action
	must be reset to SIG_DFL */

	for (rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if (*rp != SIG_IGN)
			*rp = SIG_DFL;

	/* reset u_sigmask array since caught signals have changed to SIG_DFL */
	bzero (u.u_sigmask, sizeof (u.u_sigmask));

	for (i=0; i<v.v_nofiles; i++)
		if ((u.u_pofile[i] & EXCLOSE) && u.u_ofile[i] != NULL) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
		}
}


addupc(pc, ignored_arg, incr)
unsigned pc;
{
	register int   tmpval;
	register short *slot;
	register unsigned x;
	register struct a
	{
		short	*pr_base;
		unsigned pr_size;
		unsigned pr_off;
		unsigned pr_scale;
	} *p;

	p = (struct a *) &u.u_prof; 
	x = pc - p->pr_off;
	if (p->pr_scale == 0x8000)
		slot = &p->pr_base[x >>2];
	else {
		x = (x >> 16) * p->pr_scale
			+ (((x & 0xffff) * p->pr_scale) >> 16);
		slot = &p->pr_base[x >> 1];
	}
	if ((caddr_t)slot >= (caddr_t)(p->pr_base) &&
		(caddr_t)slot < (caddr_t)((unsigned)p->pr_base + p->pr_size))
		{
			tmpval = (int)fubyte((char *)slot);
			tmpval = (int)(tmpval<<8 | fubyte((char *)slot+1));
			if (tmpval == -1)
				u.u_prof.pr_scale = 0;	/* turn off */
			else
			{
				/*
				** Inc tmpval
				*/
				tmpval += incr;
				subyte((char *)slot, tmpval>>8);
				subyte((char *)slot+1, tmpval&0xff);
			}
		}
}
