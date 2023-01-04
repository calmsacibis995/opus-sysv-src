h43430
s 00014/00002/00992
d D 1.2 90/03/16 18:30:37 root 2 1
c settrace checks for NULL pointers
e
s 00994/00000/00000
d D 1.1 90/03/06 12:28:09 root 1 0
c date and time created 90/03/06 12:28:09 by root
e
u
U
t
T
I 1
#define NPTRACE 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/sig.c	10.15"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/pfdat.h"
#include "sys/region.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/fstyp.h"
#include "sys/inode.h"
#include "sys/nami.h"
#include "sys/file.h"
#include "sys/psw.h"
#include "sys/reg.h"
#include "sys/var.h"
#include "sys/message.h"
#include "sys/conf.h"
#include "sys/ptrace.h"
#include "sys/m88kbcs.h"
#include "sys/trap.h"

extern         fuerror;

/*
 * Priority for tracing
 */

#define	IPCPRI	PZERO


/*
 * Tracing variables.
 * Used to pass trace command from
 * parent to child being traced.
 * This data base cannot be
 * shared and is locked
 * per user.
 */

struct
{
	int	ip_lock;
	int	ip_req;
	int	*ip_addr;
	int	ip_data;
} ipc;

/*
 * Send the specified signal to
 * all processes with 'pgrp' as
 * process group.
 * Called by tty.c for quits and
 * interrupts.
 */

signal(pgrp, sig)
register pgrp;
{
	register struct proc *p;

	if (pgrp == 0)
		return;
	for (p = &proc[1]; p < (struct proc *)v.ve_proc; p++)
		if (p->p_pgrp == pgrp)
			psignal(p, sig);
}

/*
 * Send the specified signal to
 * the specified process.
 */

psignal(p, sig)
register struct proc *p;
register sig;
{
	register s = sig - 1;	/* s is used as index into the
				signal array */
	register unsigned off, smask;
	if (s < 0 || s >= NSIG-1)
		return;
	smask = sigmask(sig);
	off = sigoff(sig);
	p->p_sig.s[off] |= smask;
	if (p->p_stat == SSLEEP && ((p->p_flag & SNWAKE) == 0)) {
		if(!(p->p_hold.s[off] & smask))
			setrun(p);
	} else if (p->p_stat == SSTOP) {
		if (sig == SIGKILL)
			setrun(p);
		else if (p->p_wchan && ((p->p_flag & SNWAKE) == 0))
			/*
			 * If process is in the sleep queue at an
			 * interruptible priority but is stopped,
			 * remove it from the sleep queue but don't
			 * set it running yet. The signal will be
			 * noticed when the process is continued.
			 */
			unsleep(p);
	}
}

/*
 * Returns true if the current
 * process has a signal to process,
 * and the signal is not held.
 * This is asked at least once
 * each time a process enters the
 * system.
 * A signal does not do anything
 * directly to a process; it sets
 * a flag that asks the process to
 * do something to itself.
 */

issig()
{
	register n;
	register struct proc *p, *q;
	register unsigned off, smask;

	p = u.u_procp;
	if(!(p->p_sig.s[0] & ~p->p_hold.s[0]) && 
			!(p->p_sig.s[1] & ~p->p_hold.s[1]))
		return(0);
	while (n=fsig(p)) {
		smask = sigmask(n);
		off = sigoff(n);
		if (n == SIGURG || n == SIGIO || n == SIGWINCH) {
			/* don't bother unless explicitly handled */
			if (u.u_signal[n-1] != SIG_IGN &&
				u.u_signal[n-1] != SIG_DFL)
				return(n);
		} else if (n == SIGCLD) {
			if (u.u_signal[SIGCLD-1] == SIG_IGN) {
				for (q = p->p_child; q; q = q->p_sibling)
					if (q->p_stat == SZOMB)
						freeproc(q, 0);
			} else if (u.u_signal[SIGCLD-1])
				return(n);
		} else if (n == SIGPWR) {
			if (u.u_signal[SIGPWR-1] && 
				(u.u_signal[SIGPWR-1] != SIG_IGN))
				return(n);
		} else if ((u.u_signal[n-1] != SIG_IGN) || 
			(p->p_flag&STRC))
			return(n);
		p->p_sig.s[off] &= ~smask;
	}
	return(0);
}
/*
 * Put the argument process into the stopped state and notify the
 * parent and other interested parties via wakeup and/or signal.
 * 
 */

stop(p)
struct proc *p;
{
	register struct proc *cp, *pp;

	cp = p;
	pp = cp->p_parent;
	if (cp->p_ppid == 1  ||  cp->p_ppid != pp->p_pid)
		exit(fsig(cp));
	cp->p_stat = SSTOP;
	cp->p_flag &= ~SWTED;
	wakeup((caddr_t)pp);
}

/*
 * Perform the action specified by
 * the current signal.
 * The usual sequence is:
 *	if (issig())
 *		psig();
 */

psig(arg)
{
	register n, mask;
	void(*p)();
	register struct proc *rp = u.u_procp;
	register unsigned off;
	sigset_t retmask;

	if (rp->p_flag & SPRSTOP) {
		rp->p_flag &= ~SPRSTOP;
		stop(rp);
		swtch();
	}
	if ((rp->p_flag & STRC)
	  || ((rp->p_flag & SPROCTR)
	    && (n = fsig(rp)) && 
			(mask = sigmask(n), off = sigoff(n),
			 rp->p_sigmask.s[off] & (mask)))) {
		/*
		 * If traced, always stop, and stay
		 * stopped until released by parent or tracer.
		 * If tracing via /proc, do not call procxmt.
		 */
		do {
			stop(rp);
			swtch();
		} while ((rp->p_flag&SPROCTR) == 0
		  && !procxmt() && (rp->p_flag & STRC));
	} 
	if ((n = fsig(rp)) == 0)
		return;
	mask = sigmask(n);
	off = sigoff(n);
	rp->p_sig.s[off] &= ~mask;
	if (rp->p_flag & SKILL) {
		u.u_siginfo.src = 3;
		rp->p_flag &= ~SKILL;
	}
	if ((p = u.u_signal[n-1]) != SIG_DFL) {
		if (p == SIG_IGN)
			return;
		u.u_error = 0;
		if (rp->p_flag & SOMASK) {
			retmask = u.u_oldsig;
			rp->p_flag &= ~SOMASK;
		}
		else
			retmask = rp->p_hold;
		/* if it is sigset, turn on p_hold bit */
		if(rp->p_chold.s[off] & mask)
			rp->p_hold.s[off] |= mask;
		else
			if(n != SIGILL && n != SIGTRAP && n != SIGPWR)
				u.u_signal[n-1] = SIG_DFL;
		rp->p_hold.s[0] |= u.u_sigmask[n-1].s[0];
		rp->p_hold.s[1] |= u.u_sigmask[n-1].s[1];
		if (sendsig(p, n, arg, retmask.s[0], retmask.s[1]))
			return;
		procname(0);
		printf(" Out of stack space signal %d\n", n);
	}
	switch (n) {

	case SIGQUIT:
	case SIGILL:
	case SIGTRAP:
	case SIGIOT:
	case SIGEMT:
	case SIGFPE:
	case SIGBUS:
	case SIGSEGV:
	case SIGSYS:
		u.u_abort = n;     /* save signal causing abort */
#if opus
		if (!(rp->p_compat&SCOMPAT) && core(n))
#else
		if (core())
#endif
			n += 0200;
	}
	exit(n);
}

/*
 * find the unhold signal in bit-position
 * representation in p_sig.
 */

fsig(p)
struct proc *p;
{
	register i;
	register unsigned n;

	n = p->p_sig.s[0] & ~p->p_hold.s[0];
	for (i=1; n; i++) {
		if (n & 0x80000000)
			return(i);
		n <<= 1;
	}
	n = p->p_sig.s[1] & ~p->p_hold.s[1];
	for (i=33; n; i++) {
		if (n & 0x80000000)
			return(i);
		n <<= 1;
	}
	return(0);
}

/*
 * Create a core image on the file "core"
 *
 * It writes USIZE block of the
 * user.h area followed by the entire
 * data+stack segments.
 */


core(signo)
{
	register struct inode *ip;
	struct argnamei nmarg;
	extern spath();
	register preg_t	*prp;
	register proc_t *pp;
	register int	gap;
#if NPTRACE
	struct ptrace_user ptrbuf;
#endif

	if (u.u_uid != u.u_ruid)
		return(0);
	if (u.u_procp->p_compat & SNOCORE)
		return(0);

	u.u_error = 0;
	u.u_dirp = "core";
	u.u_syscall = DUCOREDUMP;
	nmarg.cmd = NI_CREAT;
	nmarg.mode = ((IREAD|IWRITE)>>6)|((IREAD|IWRITE)>>3)|(IREAD|IWRITE);
	nmarg.ftype = 0;
	ip = namei(spath, &nmarg);
	if (u.u_error)
		return(0);

	if (!FS_ACCESS(ip, IWRITE) && ip->i_ftype == IFREG) {

		/*	Put the region sizes into the u-block for the
		 *	dump.
		 */
		pp = u.u_procp;
		if (prp = findpreg(pp, PT_TEXT))
			u.u_tsize = prp->p_reg->r_pgsz;
		else
			u.u_tsize = 0;
		
		/*	In the following, we do not want to write
		**	out the gap but just the actual data.  The
		**	calculation mirrors that in loadreg and
		**	mapreg which allocates the gap and the
		**	actual space separately.  We have to watch
		**	out for the case where the entire data region
		**	was given away by a brk(0).
		*/

		if (prp = findpreg(pp, PT_DATA)) {
			u.u_dsize = prp->p_reg->r_pgsz;
			gap=btoct((caddr_t)u.u_exdata.ux_datorg - prp->p_regva);
			if (u.u_dsize > gap)
				u.u_dsize -= gap;
			else
				u.u_dsize = 0;
		} else {
			u.u_dsize = 0;
		}

		if (prp = findpreg(pp, PT_STACK)) {
			u.u_ssize = prp->p_reg->r_pgsz;
		} else {
			u.u_ssize = 0;
		}

		/*	Check the sizes against the current ulimit and
		**	don't write a file bigger than ulimit.  If we
		**	can't write everything, we would prefer to
		**	write the stack and not the data rather than
		**	the other way around.
		*/

		if (USIZE + u.u_dsize + u.u_ssize > (uint)dtop(u.u_limit)) {
			u.u_dsize = 0;
			if (USIZE + u.u_ssize > (uint)dtop(u.u_limit))
			u.u_ssize = 0;
		}

#if !m88k
		/*
		 * Save FPU context into U-block
		 */
		if(do_fpu)
			fpu_save();
#endif

		FS_ITRUNC(ip);
		u.u_offset = 0;
#if NPTRACE
		getptuser(&ptrbuf, signo);
		u.u_base = (caddr_t) &ptrbuf;
		u.u_count = sizeof(ptrbuf);
		u.u_segflg = 1;
		u.u_fmode = FWRITE;
		FS_WRITEI(ip);

		bzero(&ptrbuf, sizeof (ptrbuf));
		u.u_offset = sizeof(ptrbuf);
		u.u_base = (caddr_t) &ptrbuf;
		u.u_count = 0x800 - sizeof(ptrbuf);
		u.u_segflg = 1;
		u.u_fmode = FWRITE;
		FS_WRITEI(ip);
#else
		u.u_base = (caddr_t)&u;
		u.u_count = ctob(USIZE);
		u.u_segflg = 1;
		u.u_fmode = FWRITE;
		FS_WRITEI(ip);
#endif

		/*	Write the data and stack to the dump file.
		 */
		
		u.u_segflg = 0;
		if (u.u_dsize) {
			u.u_base = (caddr_t)u.u_exdata.ux_datorg;
			u.u_count = ctob(u.u_dsize) - poff(u.u_base);
			FS_WRITEI(ip);
#if !NPTRACE
			u.u_offset = ctob(btoc(u.u_offset));
#endif
		}
		if (u.u_ssize) {
			u.u_count = ctob(u.u_ssize);
			u.u_base = (caddr_t)(USRSTACK - u.u_count);
			FS_WRITEI(ip);
		}

	} else {
		u.u_error = EACCES;
	}
	iput(ip);
	return(u.u_error==0);
}

/*
 * sys-trace system call.
 */

ptrace()
{
	register struct proc *p;
	register struct a {
		int	req;
		int	pid;
		int	*addr;
		int	data;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (uap->req <= 0) {
		u.u_procp->p_flag |= STRC;
		return;
	}
	for (p = u.u_procp->p_child; p; p = p->p_sibling)
		if (p->p_stat == SSTOP
#if opus
		 && ! (p->p_compat & SCOMPAT)
#endif
		 && p->p_pid == uap->pid)
			goto found;
	u.u_error = ESRCH;
	return;

    found:
	while (ipc.ip_lock)
		sleep((caddr_t)&ipc, IPCPRI);
	ipc.ip_lock = p->p_pid;
	ipc.ip_data = uap->data;
	ipc.ip_addr = uap->addr;
	ipc.ip_req = uap->req;
	p->p_flag &= ~SWTED;
	setrun(p);
	while (ipc.ip_req > 0)
		sleep((caddr_t)&ipc, IPCPRI);
	if (ipc.ip_req < 0) {
		u.u_error = EIO;
	} else
		u.u_rval1 = ipc.ip_data;
	ipc.ip_lock = 0;
	wakeup((caddr_t)&ipc);
}


/*
 * Code that the child process
 * executes to implement the command
 * of the parent process in tracing.
 */

#ifdef	m88k

#define	TRAP	0xF000D000+BPTTRAP		/* tb0  0,r0,BPTTRAP */
#if !NPTRACE
#define UADDR(u,i)	((int *)&((int *)&u)[i>>2])	/* longword alignment */
int	ipcreg[] = {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, R16,
R17, R18, R19,R20, R21, R22, R23, R24, R25, R26, R27, R28, R29, R30, SP, SNIP, PS};
#endif

#else

#define UADDR(u,i)	((int *)&((short *)&u)[i>>1])	/* halfword alignment */
int	ipcreg[] = {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, SP, PC};

#endif

procxmt()
{
	register int i;
	int	pswsave;
	register *p;
	int inst,addr,snip;

	i = ipc.ip_req;
	ipc.ip_req = 0;
	switch (i) {

	case 1: /* read user I */
	case 2: /* read user D */

		fuerror = 0;
		ipc.ip_data = fuword((caddr_t)ipc.ip_addr);
		if(fuerror)
			goto error;
		break;

	case 3: /* read u */

		i = (int)ipc.ip_addr;
#if NPTRACE
		if ((i >= sizeof(struct ptrace_user)) || (i < 0) ||
				(i & 3) )
			goto error;
		ipc.ip_data = getptuseroff(i);
		break;
#else
		if (i >= 0  &&  i < ctob(USIZE)) {
			p = UADDR(u,i);
			ipc.ip_data = *p;
			if (p == &u.u_ar0[SNIP]) {
				/* Reading pc. */
				ipc.ip_data &= ~(EXCEPT_IP|VALIDIP);
			}
			break;
		}

		p = (int *)i;
		for (i = 0  ;  i < sizeof(ipcreg)/sizeof(ipcreg[0])  ; i++) {
			if (p == &u.u_ar0[ipcreg[i]]) {
				ipc.ip_data = *p;
				if (p == &u.u_ar0[SNIP]) {
					/* Reading pc. */
					ipc.ip_data &= ~(EXCEPT_IP|VALIDIP);
				}
				goto out;
			}
		}
		goto error;
#endif	/* NPTRACE */

	case 4: /* write user I */
	case 5: /* write user D */
	{
		register preg_t *prp;
		register reg_t *rp;
		int rdonly = 0;
		preg_t *vtopreg(), *xdup();

		if ((prp = vtopreg(u.u_procp, ipc.ip_addr)) == NULL)
			goto error;
		rp = prp->p_reg;
		if ((rp->r_type == RT_STEXT) && (u.u_magic == 0407))
			if ((int)ipc.ip_addr < rp->r_filesz)
				goto error; /* Don't write to 407 text pages */
		if ((rp->r_type == RT_STEXT) && (u.u_magic != 0407)) {
			if (rp->r_iptr)
				plock(rp->r_iptr);
			reglock(rp);
			if ((prp = xdup(u.u_procp, prp)) == NULL)
				goto error;
			rp = prp->p_reg;
			if (rp->r_iptr)
				prele(rp->r_iptr);
			regrele(rp);
		}
		if (prp->p_flags & PF_RDONLY) {
			rdonly++;
			reglock(rp);
			chgprot(prp, SEG_RW);
			regrele(rp);
		}
		/*
		 * Zapped address must be on a word boundary; no explicit
		 * check here because the hardware enforces it and suword
		 * will fail on a non-word address.
		 */
		i = suword((caddr_t)ipc.ip_addr, ipc.ip_data);
#ifdef	CMMU
		dcacheflush(0);
		ccacheflush(0);
#endif
		if (rdonly) {
			reglock(rp);
			chgprot(prp, SEG_RO);
			regrele(rp);
		}
		if (i < 0)
			goto error;
		break;
	}
	case 6: /* write u */
#if NPTRACE
		i = (int)ipc.ip_addr;
		if ((i >= sizeof(struct ptrace_user)) || (i < 0) ||
				(i & 3) )
			goto error;
		if (setptuseroff(i, ipc.ip_data) == -1)
			goto error;
		break;
#else
		p = UADDR(u, (int)ipc.ip_addr);
		for (i=0; i<sizeof(ipcreg)/sizeof(ipcreg[0]); i++)
			if (p == &u.u_ar0[ipcreg[i]])
				goto ok6;
#if !m88k
		if (p == &u.u_ar0[PS]) {
			/* Only allow carry bit to be changed. */
			/* don't touch bits: user mode & ipl 0 & system stack */
			ipc.ip_data &= ~(PS_CUR|PS_IPL);
			ipc.ip_data |= PS_M;
			goto ok6;
		}
#endif

#if !m88k
		if(do_fpu && fpu_wrt_ok(p)) {
			/* legal u-block fpu value to update */
			u.u_fpu.ustate |= UFPRWRT;
			goto ok6;
		}
#endif

		goto error;

	ok6:
#if m88k
		if (p == &u.u_ar0[PS]) {
			/* Only allow carry bit to be changed. */
			*p &= ~PS_C;
			ipc.ip_data &= PS_C;
			ipc.ip_data |= *p;
		}
		if (p == &u.u_ar0[SNIP]) {	/* Changing pc. */
			ipc.ip_data &= ~EXCEPT_IP;
			ipc.ip_data |= VALIDIP;
			u.u_ar0[SFIP] = ipc.ip_data + 4;
		}
#endif
		*p = ipc.ip_data;
		break;
#endif		/* NPRTACE */

	/* set signal and continue */
	/* one version causes a trace-trap */
#ifdef	m88k
	case 9:
	case 7:
		if ((int)ipc.ip_addr != 1){
			u.u_ar0[SNIP] = (int)ipc.ip_addr | VALIDIP;
			u.u_ar0[SFIP] = u.u_ar0[SNIP] + 4;
		}
		snip = u.u_ar0[SNIP] & ~3;
		if(i == 9){	/* Gotta trace */
			if((addr = jmporbr(snip)) != -1){   /* If branch type */
				if(addr & 1){	/* If .n instruction */
					/* br.n self, detection goes here. */
					u.u_trace[0].addr = snip + 8;
					u.u_trace[0].inst = fuiword(snip + 8);
				}
				else{		/* Plain branch. */
					u.u_trace[0].addr = snip + 4;
					u.u_trace[0].inst = fuiword(snip + 4);
				}
				settrace(u.u_trace[0].addr, TRAP);
I 2
				if (ipc.ip_req == -1)
					goto nobrk;
E 2
				addr &= 0xFFFFFFFC;	/* mask off flag bits */
				u.u_trace[1].addr = addr;
				u.u_trace[1].inst = fuiword(addr);
				settrace(u.u_trace[1].addr, TRAP);
I 2
				if (ipc.ip_req == -1)
					goto nobrk;
E 2
			}
			else{
				u.u_trace[0].addr = snip + 4;
				u.u_trace[0].inst = fuiword(snip + 4);
				settrace(u.u_trace[0].addr, TRAP);
I 2
				if (ipc.ip_req == -1)
					goto nobrk;
E 2
				u.u_trace[1].addr = -1;  /* indicate single trap */
			}
		}
		else {
I 2
			nobrk:
E 2
			u.u_trace[0].addr = -1;
			u.u_trace[1].addr = -1;
		}
#else

	case 9:
		u.u_ar0[PS] |= PS_T;
	case 7:
		if ((int)ipc.ip_addr != 1)
			u.u_ar0[PC] = (int)ipc.ip_addr;
#endif
		clrsig(u.u_procp, p_sig);
		if (ipc.ip_data)
			psignal(u.u_procp, ipc.ip_data);
		wakeup(&ipc);
		return(1);

	case 8: /* force exit */

		wakeup(&ipc);
		exit(fsig(u.u_procp));

	default:
	error:
		ipc.ip_req = -1;
	}
out:
	wakeup(&ipc);
	return(0);
}
#ifdef	m88k
/*  
    Determine if the instruction pointed to by 'addr' is a jump or
branch type of instruction.  We also must check if the instruction is
a system call instruction as it will modify IP as well.

    Returns:
	-1 if not a jump or branch.
        else returns the destination address of the jump or branch
 	instruction.  If bit 0 of this address is 1, then the .n
 	option is in effect on the jump or branch instruction.

*/
jmporbr(addr)
	int *addr;	/* pointer to instruction of interest */
{
	int inst;	/* The instruction */
	int offset;

	if((inst = fuiword(addr)) == -1){
		ipc.ip_req = -1;
	}
	switch(inst & 0xF8000000){

	/* 16 bit offset branches */
	case 0xD0000000:	/* bb0 */
	case 0xD8000000:	/* bb1 */
	case 0xE8000000:	/* bcnd */
		offset = inst & 0xFFFF;
		if(offset & 0x8000)
			offset |= 0xFFFF0000;
		return(((int)&addr[offset]) | ((inst & 0x04000000) >> 26));

	/* 26 bit offset branches */
	case 0xC0000000:	/* br */
	case 0xC8000000:	/* bsr */
		offset = inst & 0x3FFFFFF;
		if(offset & 0x2000000)
			offset |= 0xFC000000;
		return(((int)&addr[offset]) | ((inst & 0x04000000) >> 26));

	default:
		switch(inst & 0xFC00C800){
		/* Register addressing jumps */
		case 0xF400C000:	/* jmp */
		case 0xF400C800:	/* jsr */
			offset = u.u_ar0[inst & 0x1F];
			return(offset | ((inst & 0x400) >>10));
		case 0xF000C000:	/* Possible tb0 */
			if((inst & 0xFC1FFF00) == 0xF000D100){ /* System call */
				return((int) (addr+2));
			}
		default:
			return(-1);
		}
	}
	return(-1);   /* Never gets here, just for compiler */
}

/*  This subroutine will set a trace instruction in the users
    code space at location addr.
*/
settrace(addr,data)
	int *addr;
	int data;
{
	int i;
	register preg_t *prp;
	register reg_t *rp;
	int rdonly = 0;
	preg_t *vtopreg(), *xdup();

	addr = (int *)((int)addr & ~3);  /* Round down to nearest instruction */
D 2
	if ((prp = vtopreg(u.u_procp, addr)) == NULL)
E 2
I 2
	if ((prp = vtopreg(u.u_procp, addr)) == NULL) {
E 2
		ipc.ip_req = -1;
I 2
		return;
	}
E 2
	rp = prp->p_reg;
	if (rp->r_type == RT_STEXT) {
		if (rp->r_iptr)
			plock(rp->r_iptr);
		reglock(rp);
D 2
		if ((prp = xdup(u.u_procp, prp)) == NULL)
E 2
I 2
		if ((prp = xdup(u.u_procp, prp)) == NULL) {
E 2
			ipc.ip_req = -1;
I 2
			regrele(rp);
			return;
		}
E 2
		rp = prp->p_reg;
		if (rp->r_iptr)
			prele(rp->r_iptr);
		regrele(rp);
	}
	if (prp->p_flags & PF_RDONLY) {
		rdonly++;
		reglock(rp);
		chgprot(prp, SEG_RW);
		regrele(rp);
	}
	/*
		* Zapped address must be on a word boundary; no explicit
		* check here because the hardware enforces it and suword
		* will fail on a non-word address.
		*/
	i = suword((caddr_t)addr, data);
#ifdef	CMMU
	dcacheflush(0);
	ccacheflush(0);
#endif
	if (rdonly) {
		reglock(rp);
		chgprot(prp, SEG_RO);
		regrele(rp);
	}
	if (i < 0)
		ipc.ip_req = -1;
}

#ifndef OPUSVENDORID
#define	OPUSVENDORID	0xf00
#endif

getptuseroff(off)
{
	struct ptrace_user ptu;
	register i;

	getptuser(&ptu, 0);

	i = ((int) &ptu) + off;

	if (off == 1024|| off == 1028 || off == 1032) {	
		/* reading sxip, snip, sfip */
		i = * (long *) i;
		return (i & ~(EXCEPT_IP|VALIDIP));
	}
	return ( *(long *) i);
}

getptuser(ptp, signo)
register struct ptrace_user *ptp;
{
	register i;
	register preg_t *prp;
	register struct sigframe *sp;
	int gap;

	bzero(ptp, sizeof (struct ptrace_user));

	ptp->pt_magic = 0x88000;
	ptp->pt_rev = 1;
	ptp->pt_timdat = u.u_pttimdat;
	ptp->pt_vendor = OPUSVENDORID;

	bcopy((caddr_t) u.u_ar0, (caddr_t) &ptp->pt_r0, 
		(caddr_t) &ptp->pt_psr - (caddr_t) &ptp->pt_r0);
	ptp->pt_psr = psrcom(0, 0);
	ptp->pt_fpsr = getcr62();
	ptp->pt_fpcr = getcr63();

	ptp->pt_ppid = u.u_procp->p_ppid;

	bcopy((caddr_t) u.u_psargs, (caddr_t) ptp->pt_comm, PSARGSZ);
	for (i = 0; i < sizeof(ptp->pt_comm); i++) {
		if (ptp->pt_comm[i] == 0) {
			i--;
			if (ptp->pt_comm[i] == ' ')
				ptp->pt_comm[i] = '\0';
			break;
		}
	}
	
	ptp->pt_local[0] = u.u_syscall;		/* usefull */
	for (i = 0; i < 6 ; i++)
		ptp->pt_arg[i] = u.u_arg[i];

	if (signo == 0) {
		if (prp = findpreg(u.u_procp, PT_DATA)) {
			i = prp->p_reg->r_pgsz;
			gap=btoct((caddr_t)u.u_exdata.ux_datorg - prp->p_regva);
			if (i > gap)
				i -= gap;
			else 
				i = 0;
			ptp->pt_dsize = ctob(i);
		}
		if (prp = findpreg(u.u_procp, PT_STACK)) {
			i = prp->p_reg->r_pgsz;
			ptp->pt_ssize = ctob(i);
		}
	}
	else {
		i = (int) u.u_exdata.ux_datorg;
		ptp->pt_dsize = ctob(u.u_dsize) - poff(i);
		ptp->pt_ssize = ctob(u.u_ssize);
	}
	bcopy((caddr_t) u.u_signal, (caddr_t) ptp->pt_sigtbl, 
			sizeof(ptp->pt_sigtbl));
	bcopy((caddr_t) &u.u_pt, (caddr_t) &ptp->pt_o_magic, sizeof(u.u_pt));
	if (signo)
		ptp->pt_dataptr = (char *) 0x800;	/* always */
	sp = (struct sigframe *) ptp->pt_sigframe;
	sp->sf_r31 = u.u_ar0[SP];
	sp->sf_id = 1;
	sp->sf_sxip = u.u_ar0[PC];
	sp->sf_snip = u.u_ar0[SNIP];
	sp->sf_sfip = u.u_ar0[SFIP];

	/* if not core dumping, any pending sigs */
	if (signo == 0)
		signo = issig();

	if (signo) {
		sp->sf_sigsource = u.u_siginfo.src;
		sp->sf_signum = signo;
		sp->sf_numexceptblock = 1;
		sp->sf_exceptblock[0] = u.u_siginfo.nwords;
		for ( i = 0; i < 4 ; i++ )
			sp->sf_exceptblock[i+1] = u.u_siginfo.args[i];
	}
}

setptuseroff(off, data)
register unsigned off;
{
	if (off  >= 16 && off < 144) {		 /* r0 - r31 */ 
		off -= 16;
		off += (int) u.u_ar0;
	}
	else if (off == 144) {			/* psr */
		psrcom(1, data);
		return (0);
	}
	else if (off == 148) {
		setcr62(data);			/* fpsr */
		return (0);
	}
	else if (off == 152) {
		setcr63(data);			/* fpcr */
		return (0);
	}
	else if (off >= 464 && off < 720) {	/* sigtbl */
		off -= 464;
		off += (int) u.u_signal;
	}
	else if (off == 1016)			/* r31 again */
		off = (int) &u.u_ar0[R31];
	else if (off == 1024)			/* sxip */
		off = (int) &u.u_ar0[SCIP];
	else if (off == 1028) {			/* snip */
		data &= ~EXCEPT_IP;
		data |= VALIDIP;
		u.u_ar0[SFIP] = data + 4;
		off = (int) &u.u_ar0[SNIP];
	}
	else if (off == 1032)			/* sfip */
		off = (int) &u.u_ar0[SFIP];
	else
		return (-1);
	* (long *) off = data;
	return (0);
}
#endif
E 1
