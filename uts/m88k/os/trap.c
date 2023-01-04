/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/trap.c	1.0"

#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/reg.h"
#include "sys/psw.h"
#include "sys/immu.h"
#include "sys/trap.h"
#include "sys/region.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/proc.h"
#include "sys/sysinfo.h"
#include "sys/cmn_err.h"

extern unsigned nofulow();
extern unsigned nofuhigh();
extern unsigned errret();

extern int bootstate;	/* rfs state: up or down */

int divcnt, aligncnt;
/*
 * Called from the trap handler when a processor trap occurs.
 */
trap(regptr,type)
	int *regptr;
	register type;
{
	int *old_ar0;		/* old ar0		*/
	register sig = 0;
	register struct proc *pp;
	time_t syst;
	register code;
	int inst, s1, s2;
	
	syst = u.u_stime;
	pp = u.u_procp;
	old_ar0 = u.u_ar0;	/* save old ar0 and restore on return	      */
	u.u_ar0 = regptr;
	code = u.u_ar0[PC];		/* pc is the most common use */
	u.u_traptype = TRAPNORM;
	u.u_siginfo.nwords = 0;
	sysinfo.trap++;

	if (USERMODE(u.u_ar0[PS]))
		type |= USER;
	switch (type) {

	/*
	 * Trap not expected.
	 * Usually a kernel mode bus error.
	 * If user mode, signal trap.
	 */
	default:
		if (USERMODE(u.u_ar0[PS])) {
			type &= ~USER;
			code = type;
			if (kdebug & 8) {
				procname(0);
				printf("trap number %d\n", type);
			}
			if (type == 503) {
				/* cpu divide by zero bug problem */
				sig = SIGFPE;
				code = KINTDIV;
			}
			else if (type >= 504 && type <= 511)
				sig = SIGTRAP;
			else
				sig = SIGSYS;
			break;
		}
		spl7();
		procname(regptr);
		cmn_err(CE_WARN,"   *** Unknown Trap Type = 0x%x ***", type);
		cmn_err(CE_WARN,"proc = 0x%x, pid = %d",pp,pp->p_pid);
		cmn_err(CE_PANIC,"pc   = 0x%x, ps   = 0x%x",code,u.u_ar0[PS]);


	case RESCHED + USER:	/* Allow process switch */
		sysinfo.resched++;
		qswtch();
		goto out;

	case SFU1PR+USER:      /* Precise floating point exception come here */
	case SFU1IMP+USER:
		sig = SIGFPE;
		code = u.u_sfu1data[0].fpecr;
		if (type == (SFU1PR + USER))
			break;
		u.u_siginfo.args[1] = u.u_sfu1data[0].fphs1_fprh;
		u.u_siginfo.args[2] = u.u_sfu1data[0].fpls1_fprl;
		u.u_siginfo.args[3] = u.u_sfu1data[0].fphs2_fpit;
		u.u_siginfo.nwords = 4;
		break;

	case ZDVDERR + USER:	/* zero divide check */
	case ZDVDERR:	/* zero divide check */
		
	t503:
	    divcnt++;
	    if (type == ZDVDERR)
		inst = * (long *) (u.u_ar0[SCIP] & ~3);
	    else
		inst = fuiword(u.u_ar0[SCIP] & ~3);
	    
		if((inst) > 0){  /* If literal*/
			s2 = inst & 0xFFFF;
/* literals are unsigned, so sign extension here is bogus. madan 12/21/88
			if (s2 & 0x8000)
				s2 |= 0xFFFF0000;
*/
		}
		else
			s2 = u.u_ar0[inst & 0x1F];

		s1 = u.u_ar0[(inst >> 16) & 0x1F];

		if( s2 != 0){		/* Signed divide exception. */
			if((s1 == 0x80000000) || (s2 == 0x80000000)){
				if(s2 == 0x80000000){
					u.u_ar0[(inst >> 21) & 0x1f] = (s1 == 0x80000000);
				}
				else{
					
					if(s2 > 0){
						u.u_ar0[(inst >> 21) & 0x1f] = ((unsigned)(s1)/(unsigned)(s2)) * -1;
					}
					else {
						if(s2 == -1){
							if(type == ZDVDERR){	/* If in kernel, panic. */
								cmn_err(CE_PANIC,"   *** Kernel Divide by zero pc: 0x%x ***", u.u_ar0[SCIP]);
							}
							sig = SIGFPE;
							code = KINTOVF;
							goto divzeroerr;
						}
						else{
							u.u_ar0[(inst >> 21) & 0x1f] = (unsigned)(s1)/(unsigned)(s2 * -1);
						}
					}
				}

			}
			else if(s2 > 0){
				s1 *= -1;
				s1 /= s2;
				u.u_ar0[(inst >> 21) & 0x1f] = s1 * -1;
			}
			else if (s1 > 0) {
				s2 *= -1;
				s1 /= s2;
				u.u_ar0[(inst >> 21) & 0x1f] = s1 * -1;
			}
			else{
				s1 *= -1;
				s2 *= -1;
				u.u_ar0[(inst >> 21) & 0x1f] = s1 / s2;
			}
			if(type == ZDVDERR){	/* If in kernel, return. */
				u.u_ar0 = old_ar0;
				return;
			}

			break;
		}
		else{ 		/* Zero divide error */
			if(type == ZDVDERR){	/* If in kernel, panic. */
				procname(regptr);
				cmn_err(CE_PANIC,"   *** Kernel Divide by zero pc: 0x%x ***", u.u_ar0[SCIP]);
			}
			sig = SIGFPE;
			code = KINTDIV;
			divzeroerr:
			procname(0);
			printf("Divide by zero problem PC = %x, instr = %x\n", 
					regptr[PC], inst);
		}
		break;

	case CHKTRAP + USER:	/* tbnd trap */
		sig = SIGFPE;
		code = KSUBRNG;
		break;

	case ADDRERR + USER:	/* address error */

		aligncnt++;
		if (fixalign(regptr))
			break;
		sig = SIGBUS;
		code = 1;
		u.u_traptype = TRAPADDR;
		break;

	case BPTTRAP + USER:	/* new bpt instruction fault */
	case BPTFLT + USER:	/* old bpt instruction fault */

		if (pp->p_flag & STRC) {
			u.u_ar0[SFIP] = u.u_ar0[SNIP];    /* Back up IPs */
			u.u_ar0[SNIP] = u.u_ar0[SCIP];	  

			if(u.u_trace[1].addr != -1){
				settrace(u.u_trace[1].addr, u.u_trace[1].inst);
				u.u_trace[1].addr = -1;
			}
			if(u.u_trace[0].addr != -1){
				settrace(u.u_trace[0].addr, u.u_trace[0].inst);
				u.u_trace[0].addr = -1;
			}
		}

		sig = SIGTRAP;
		code = type & ~USER;
		break;

	case INTOVF:	/* kernel fpe code, signal user */
		u.u_ar0[PS] &= ~PS_SUP;
		/* FALL THRU */

	case INTOVF + USER:	/* SIGFPE ?? */
		sig = SIGFPE;
		code = KINTOVF;
		break;

	case PRIVFLT + USER:	/* privileged instruction fault */
		sig = SIGILL;
		code = 2;
		break;
	case INSTERR + USER:	/* Illegal instruction */
		sig = SIGILL;
		code = 1;
		break;

	}

	if (sig && (kdebug & 8))
	{
		procname(0);
		printf("pc   = 0x%x, ps   = 0x%x",u.u_ar0[PC],u.u_ar0[PS]);
		printf("trap: psignal(%x, %x)\n", pp, sig);	/* DEBUG */
	}

	if (sig)
		psignal(pp, sig);

	if (runrun != 0)
		qswtch();
out:
	curpri = pp->p_pri = calcppri(pp);

	if ((pp->p_flag & SPRSTOP) || (isset(pp) && issig())) {
		u.u_siginfo.src = 1;
		u.u_siginfo.args[0] = code;
		psig(code);
		curpri = pp->p_pri = calcppri(pp);
	}

	if(u.u_prof.pr_scale)
		addupc((caddr_t)u.u_ar0[PC], &u.u_prof, (int)(u.u_stime-syst));
}

/* System call preprocessing */
syscall(arg1,arg2,arg3,arg4,arg5,arg6, regptr, code)
	int  *regptr;
	register unsigned code;
{

	register 		i;
	register struct sysent	*callp;
	register struct proc	*pp;
	time_t			syst;
	short			pid;
	extern 			nsysent;
	extern			netisr;

	syst = u.u_stime;
	pid = u.u_procp->p_pid;
	u.u_traptype = TRAPNORM;
	u.u_ar0 = regptr;
	sysinfo.syscall++;
	u.u_error = 0;
	
	if ((code & 256) == 0) {
		/* old style system call, disabled by default */
		/* allow old syslocal as X won't run */
		if (code != 50 && (kdebug & 0x20) == 0) {
			/* 256 was the start vector for the old syscalls */
			u.u_siginfo.args[0] = code + 256;
			procname(0);
			printf("Old system call # %d\n", code);
			code = 0;
		}
	}
	else 
		code -= 256;

	if (code >= nsysent)
		code = 0;
	u.u_syscall = code;
	callp = &sysent[code];

	u.u_arg[0] = arg1;
	u.u_arg[1] = arg2;
	u.u_arg[2] = arg3;
	u.u_arg[3] = arg4;
	u.u_arg[4] = arg5;
	u.u_arg[5] = arg6;

	u.u_dirp = (caddr_t)u.u_arg[0];
	u.u_rval1 = 0;
	u.u_rval2 = u.u_ar0[R3];
	u.u_ap = u.u_arg;

#define DEBUG
#ifdef DEBUG
	startsyscall();
#endif

	if ((bootstate || !callp->sy_setjmp) && setjmp(u.u_qsav)) {
		if (!(u.u_rflags & U_RSYS) && !u.u_error)
			u.u_error = EINTR;
		spl0();
	} else
		(*callp->sy_call)();

	u.u_rflags &= ~(U_RSYS | U_DOTDOT | U_LBIN);

#ifdef DEBUG
	endsyscall();
#endif

	if (u.u_error) {
		u.u_ar0[R2] = u.u_error;
		u.u_error = 0;
		if (++u.u_errcnt > 16) {
			u.u_errcnt = 0;
			runrun++;
		}
		if (u.u_nomemcnt > 32) {
			u.u_nomemcnt = 0;
			psignal(u.u_procp, SIGTERM);
			procname(0);
			printf("SIGTERM: out of memory\n");
		}
	} else {
		u.u_nomemcnt = 0;
		if (code == 108 || 
			(u.u_arg[0] == 2 && (code == 38 || code == 50))) {
			   /* Don't do this on sigret */
			u.u_ar0[R2] = u.u_rval1; /* setup return values */
			u.u_ar0[R3] = u.u_rval2;  /* Old register contents */
		}
		else{
			u.u_ar0[R2] = u.u_rval1; /* setup return values */
			u.u_ar0[R3] = u.u_rval2;
			u.u_ar0[SFIP] = (u.u_ar0[SCIP] + 8) | VALIDIP;
			u.u_ar0[SNIP] = (u.u_ar0[SCIP] + 8) & ~VALIDIP; /* bump pc */
		}
	}

	pp = u.u_procp;
	curpri = pp->p_pri = calcppri(pp);

	if (netisr)
		softnetint();

	if (runrun != 0)
		qswtch();

	if (!u.u_trapflag && ((pp->p_flag & SPRSTOP)||(isset(pp) && issig()))) {
		psig((fuiword(u.u_ar0[SCIP] & ~3) & 0x1FF) - 256);
		curpri = pp->p_pri = calcppri(pp);
	}

	/*	If pid != pp->p_pid, then we are the child
	**	returning from a fork system call.  In this
	**	case, ignore syst since our time was reset
	**	in fork.
	*/

	if (u.u_prof.pr_scale)
		addupc((caddr_t)u.u_ar0[PC], &u.u_prof, 
			pid == pp->p_pid ? (int)(u.u_stime - syst)
					 : (int)u.u_stime);
}

#ifdef DEBUG

char *
getusrstr(p)
char *p;
{
	static char buf[512];
	char *bufp = buf;

	while (*bufp++ = fubyte(p++))
		;
	*bufp = 0;
	return(buf);
}


startsyscall()
{
	register int a1 = u.u_arg[0];
	register int a2 = u.u_arg[1];
	register int a3 = u.u_arg[2];
	register int a4 = u.u_arg[3];
	register int i;

	if (begintrace(u.u_syscall) == 0)
		return;
	
	procname(0);
	printf("(%d)@%x,\t", u.u_procp->p_pid, u.u_ar0[R1]);
	switch (u.u_syscall) {
	case 0:
		printf("nosys()\n");
		break;
	case 1:
		printf("exit(%d)\n", a1);
		break;
	case 2:
		printf("fork()\n");
		break;
	case 3:
		printf("read(%d, 0x%x, %d)\n", a1, a2, a3);
		break;
	case 4:
		printf("write(%d, 0x%x, %d)\n", a1, a2, a3);
		break;
	case 5:
		printf("open(\"%s\", 0x%x, 0%o)\n", getusrstr(a1), a2, a3);
		break;
	case 6:
		printf("close(%d)\n", a1);
		break;
	case 7:
		printf("wait(0x%x)\n", a1);
		break;
	case 8:
		printf("creat(\"%s\", 0%o)\n", getusrstr(a1), a2);
		break;
	case 9:
		printf("link(\"%s\", \"%s\")\n", getusrstr(a1), getusrstr(a2));
		break;
	case 10:
		printf("unlink(\"%s\")\n", getusrstr(a1));
		break;
	case 11:
		printf("exec(\"%s\")\n", getusrstr(a1));
		break;
	case 12:
		printf("chdir(\"%s\")\n", getusrstr(a1));
		break;
	case 13:
		printf("time(0x%x)\n", a1);
		break;
	case 14:
		printf("mknod(\"%s\", 0%o, 0x%x)\n", getusrstr(a1), a2, a3);
		break;
	case 15:
		printf("chmod(\"%s\", 0%o)\n", getusrstr(a1), a2);
		break;
	case 16:
		printf("chown(\"%s\", %d, %d)\n", getusrstr(a1), a2, a3);
		break;
	case 17:
		printf("brk(0x%x)\n", a1);
		break;
	case 18:
		printf("stat(\"%s\", 0x%x)\n", getusrstr(a1), a2);
		break;
	case 19:
		printf("lseek(%d, 0x%x, %d)\n", a1, a2, a3);
		break;
	case 20:
		printf("getpid()\n");
		break;
	case 21:
		printf("mount(\"%s\", \"%s\", 0x%x, 0x%x)\n", getusrstr(a1), getusrstr(a2), a3, a4);
		break;
	case 22:
		printf("umount(\"%s\")\n", getusrstr(a1));
		break;
	case 23:
		printf("setuid(%d)\n", a1);
		break;
	case 24:
		printf("getuid()\n");
		break;
	case 25:
		printf("stime(0x%x)\n", a1);
		break;
	case 26:
		printf("ptrace(%d, %d, 0x%x, 0x%x)\n", a1, a2, a3, a4);
		break;
	case 27:
		printf("alarm(%d)\n", a1);
		break;
	case 28:
		printf("fstat(%d, 0x%x)\n", a1, a2);
		break;
	case 29:
		printf("pause()\n");
		break;
	case 30:
		printf("utime(\"%s\", 0x%x)\n", getusrstr(a1), a2);
		break;
	case 31:
		printf("stty()\n");
		break;
	case 32:
		printf("gtty()\n");
		break;
	case 33:
		printf("access(\"%s\", 0%o)\n", getusrstr(a1), a2);
		break;
	case 34:
		printf("nice(%d)\n", a1);
		break;
	case 35:
		printf("statfs(\"%s\", 0x%x, %d, %d)\n", getusrstr(a1), a2, a3, a4);
		break;
	case 36:
		printf("sync()\n");
		break;
	case 37:
		printf("kill(%d, %d)\n", a1, a2);
		break;
	case 38:
		printf("sys_local()\n");
		break;
	case 39:
		printf("setpgrp()\n");
		break;
	case 41:
		printf("dup(%d)\n", a1);
		break;
	case 42:
		printf("pipe()\n");
		break;
	case 43:
		printf("times(0x%x)\n", a1);
		break;
	case 44:
		printf("profil(0x%x, %d, %d, %d)\n", a1, a2, a3, a4);
		break;
	case 45:
		printf("plock(%d)\n", a1);
		break;
	case 46:
		printf("setgid()\n");
		break;
	case 47:
		printf("getgid()\n");
		break;
	case 48:
		printf("signal(%d, 0x%x)\n", a1, a2);
		break;
	case 49:
		printf("sysmsg(0x%x, 0x%x 0x%x)\n", a1, a2, a3);
		break;
	case 50:
		printf("sys_local(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 51:
		printf("acct(\"%s\")\n", getusrstr(a1));
		break;
	case 52:
		printf("shm(0x%x, 0x%x, 0x%x, 0x%x)\n", a1, a2, a3, a4);
		break;
	case 53:
		printf("sem(0x%x, 0x%x, 0x%x, 0x%x)\n", a1, a2, a3, a4);
		break;
	case 54:
		printf("ioctl(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 55:
		printf("uadmin(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 56:
		printf("locking()\n");
		break;
	case 57:
		printf("utssys()\n");
		break;
	case 59:
		printf("exece(\"%s\", %x, %x)\n", getusrstr(a1), a2, a3);
		break;
	case 60:
		printf("umask(o%o)\n", a1);
		break;
	case 61:
		printf("chroot(\"%s\")\n", getusrstr(a1));
		break;
	case 62:
		printf("fcntl(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 63:
		printf("ulimit(%d, 0x%x)\n", a1, a2);
		break;
	
	case 70:
		printf("advfs()\n");
		break;
	case 71:
		printf("unadvfs()\n");
		break;
	case 72:
		printf("rmount()\n");
		break;
	case 73:
		printf("rumount()\n");
		break;
	case 74:
		printf("rfstart()\n");
		break;
	case 76:
		printf("rdebug()\n");
		break;
	case 77:
		printf("rfstop()\n");
		break;
	case 78:
		printf("rfsys()\n");
		break;
	case 79:
		printf("rmdir(\"%s\")\n", getusrstr(a1));
		break;
	case 80:
		printf("mkdir(\"%s\", 0%o)\n", getusrstr(a1), a2);
		break;
	case 81:
		printf("getdents(0x%x, 0x%x, %d)\n", a1, a2, a3);
		break;
	case 82:
		printf("fstatfs(%d, 0x%x, %d, %d)\n", a1, a2, a3, a4);
		break;
	case 84:
		printf("sysfs(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 85:
		printf("getmsg(%d, 0x%x, 0x%x, 0x%x)\n", a1, a2, a3, a4);
		break;
	case 86:
		printf("putmsg(%d, 0x%x, 0x%x, 0x%x)\n", a1, a2, a3, a4);
		break;
	case 87:
		printf("poll(0x%x, %d, %d)\n", a1, a2, a3);
		break;
	case 88:
		printf("getitimer()\n", a1, a2);
		break;
	case 89:
		printf("getpgrp()\n");
		break;
	case 90:
		printf("getpsr()\n");
		break;
	case 91:
		printf("rename(\"%s\", \"%s\")\n", getusrstr(a1),getusrstr(a2));
		break;
	case 92:
		printf("setitimer(%d, 0x%x, 0x%x)\n", a1, a2, a3);
		break;
	case 93:
		printf("setpsr(0x%x)\n", a1);
		break;
	case 94:
		printf("uname()\n", a1);
		break;
	case 95:
		printf("ustat()\n", a1, a2);
		break;
	case 96:
		printf("waitpid	(%d, 0x%x, %d)\n", a1, a2, a3);
		break;
	case 97:
		printf("pathconf()\n", a1, a2);
		break;
	case 98:
		printf("memctl()\n", a1, a2, a3);
		break;
	case 99:
		printf("setpgid()\n", a1, a2);
		break;
	case 100:
		printf("sysconf(%d)\n", a1);
		break;
	case 101:
		printf("fstat(%d, %x)\n", a1, a2);
		break;
	case 102:
		printf("stat(%s, %x)\n", getusrstr(a1), a2);
		break;
	case 103:
		printf("sigaction(%d, %x, %x)\n", a1, a2, a3);
		break;
	case 104:
		printf("sigprocmask(%d, %x, %x)\n", a1, a2, a3);
		break;
	case 105:
		printf("sigpending()\n", a1);
		break;
	case 106:
		printf("sigsuspend()\n", a1);
		break;
	case 107:
		printf("sigfillset()\n", a1);
		break;
	case 108:
		printf("sigret()\n");
		break;
	case 109:
		printf("setsid()\n");
		break;
	case 110:
		break;	/* sys_local */
	default:
		printf("sys #%d ( %d, %d, %d )\n", u.u_syscall, a1, a2, a3);
		break;
	}

}

endsyscall()
{
	register int e = u.u_error;
	register retval;
	register char *s;

	if (endtrace(u.u_syscall) == 0)
		return;

	procname(0);
	if (e) {
		retval = e;
		printf("(%d) err,\t", u.u_procp->p_pid);
	}
	else {
		retval = u.u_rval1;
		printf("(%d) end,\t", u.u_procp->p_pid);
	}

	s = 0;
	switch (u.u_syscall) {
	case 0:
		s = "nosys -> %d\n";
		break;
	case 1:
		s = "exit -> %d\n";
		break;
	case 2:
		s = "fork -> %d\n";
		break;
	case 3:
		s = "read -> %d\n";
		break;
	case 4:
		s = "write -> %d\n";
		break;
	case 5:
		s = "open -> %d\n";
		break;
	case 6:
		s = "close -> %d\n";
		break;
	case 7:
		s = "wait -> %d\n";
		break;
	case 8:
		s = "creat -> %d\n";
		break;
	case 9:
		s = "link -> %d\n";
		break;
	case 10:
		s = "unlink -> %d\n";
		break;
	case 11:
		s = "exec -> %d\n";
		break;
	case 12:
		s = "chdir -> %d\n";
		break;
	case 13:
		s = "time -> %d\n";
		break;
	case 14:
		s = "mknod -> %d\n";
		break;
	case 15:
		s = "chmod -> %d\n";
		break;
	case 16:
		s = "chown -> %d\n";
		break;
	case 17:
		s = "brk -> %d\n";
		break;
	case 18:
		s = "stat -> %d\n";
		break;
	case 19:
		s = "lseek -> %d\n";
		break;
	case 20:
		s = "getpid -> %d\n";
		break;
	case 21:
		s = "mount -> %d\n";
		break;
	case 22:
		s = "umount -> %d\n";
		break;
	case 23:
		s = "setuid -> %d\n";
		break;
	case 24:
		s = "getuid -> %d\n";
		break;
	case 25:
		s = "stime -> %d\n";
		break;
	case 26:
		s = "ptrace -> %d\n";
		break;
	case 27:
		s = "alarm -> %d\n";
		break;
	case 28:
		s = "fstat -> %d\n";
		break;
	case 29:
		s = "pause -> %d\n";
		break;
	case 30:
		s = "utime -> %d\n";
		break;
	case 31:
		s = "stty -> %d\n";
		break;
	case 32:
		s = "gtty -> %d\n";
		break;
	case 33:
		s = "access -> %d\n";
		break;
	case 34:
		s = "nice -> %d\n";
		break;
	case 35:
		s = "statfs -> %d\n";
		break;
	case 36:
		s = "sync -> %d\n";
		break;
	case 37:
		s = "kill -> %d\n";
		break;
	case 38:
		s = "sysm68k -> %d\n";
		break;
	case 39:
		s = "setpgrp -> %d\n";
		break;
	case 41:
		s = "dup -> %d\n";
		break;
	case 42:
		s = "pipe -> %d\n";
		break;
	case 43:
		s = "times -> %d\n";
		break;
	case 44:
		s = "profil -> %d\n";
		break;
	case 45:
		s = "plock -> %d\n";
		break;
	case 46:
		s = "setgid -> %d\n";
		break;
	case 47:
		s = "getgid -> %d\n";
		break;
	case 48:
		s = "signal -> %d\n";
		break;
	case 49:
		s = "sysmsg -> %d\n";
		break;
	case 50:
		s = "sysm68k -> %d\n";
		break;
	case 51:
		s = "acct -> %d\n";
		break;
	case 52:
		s = "shm -> %d\n";
		break;
	case 53:
		s = "sem -> %d\n";
		break;
	case 54:
		s = "ioctl -> %d\n";
		break;
	case 55:
		s = "uadmin -> %d\n";
		break;
	case 56:
		s = "locking -> %d\n";
		break;
	case 57:
		s = "utssys -> %d\n";
		break;
	case 59:
		s = "exece -> %d\n";
		break;
	case 60:
		s = "umask -> %d\n";
		break;
	case 61:
		s = "chroot -> %d\n";
		break;
	case 62:
		s = "fcntl -> %d\n";
		break;
	case 63:
		s = "ulimit -> %d\n";
		break;
	
	
	case 70:
		s = "advfs -> %d\n";
		break;
	case 71:
		s = "unadvfs -> %d\n";
		break;
	case 72:
		s = "rmount -> %d\n";
		break;
	case 73:
		s = "rumount -> %d\n";
		break;
	case 74:
		s = "rfstart -> %d\n";
		break;
	case 76:
		s = "rdebug -> %d\n";
		break;
	case 77:
		s = "rfstop -> %d\n";
		break;
	case 78:
		s = "rfsys -> %d\n";
		break;
	case 79:
		s = "rmdir -> %d\n";
		break;
	case 80:
		s = "mkdir -> %d\n";
		break;
	case 81:
		s = "getdents -> %d\n";
		break;
	case 82:
		s = "fstatfs -> %d\n";
		break;
	case 84:
		s = "sysfs -> %d\n";
		break;
	case 85:
		s = "getmsg -> %d\n";
		break;
	case 86:
		s = "putmsg -> %d\n";
		break;
	case 87:
		s = "poll -> %d\n";
		break;
	case 88:
		s = "getitimer -> %d\n";
		break;
	case 89:
		s = "getpgrp -> %d\n";
		break;
	case 90:
		s = "getpsr -> %d\n";
		break;
	case 91:
		s = "rename -> %d\n";
		break;
	case 92:
		s = "setitimer -> %d\n";
		break;
	case 93:
		s = "setpsr -> %d\n";
		break;
	case 94:
		s = "uname -> %d\n";
		break;
	case 95:
		s = "ustat -> %d\n";
		break;
	case 96:
		s = "waitpid	 -> %d\n";
		break;
	case 97:
		s = "pathconf -> %d\n";
		break;
	case 98:
		s = "memctl -> %d\n";
		break;
	case 99:
		s = "setpgid -> %d\n";
		break;
	case 100:
		s = "sysconf -> %d\n";
		break;
	case 101:
		s = "fstat -> %d\n";
		break;
	case 102:
		s = "stat -> %d\n";
		break;
	case 103:
		s = "sigaction -> %d\n";
		break;
	case 104:
		s = "sigprocmask -> %d\n";
		break;
	case 105:
		s = "sigpending -> %d\n";
		break;
	case 106:
		s = "sigsuspend -> %d\n";
		break;
	case 107:
		s = "sigfillset -> %d\n";
		break;
	case 108:
		s = "sigret -> %d\n";
		break;
	case 109:
		s = "setsid -> %d\n";
		break;
	case 110:
		break;		/* sys_local */
	default:
		s = "sys #%d -> %d\n";
		break;
	}

	if (s)
		printf(s, retval);
}
#endif




/*
 * Called from the trap handler when a processor bus error occurs.
 *	    return value		Meaning
 *	    	 0	   Faulting page has been fixed, continue inst.
 *	    	value	   Abort cycle.  Clean up stack frame.
 *				New pc has been assigned.
 */


buserr(regptr)
	register int *regptr;
{
	register typesig = 0;	/* signal type		*/
	register proc_t	*p;	/* faulting process	*/
	/* register int	ssw;	/* special status work	*/
	register int	vaddr;	/* faulting virt addr	*/
	/* register int	berrsrc;/* STAT2 bus error reg	*/
	/* register int	write;	/* write flag		*/
	register int	*ppc;	/* pointer to pc	*/
	register 	i;
	time_t syst;		/* time of fault	*/
	int *old_ar0;		/* old ar0		*/
	int old_hold;		/* old held signals	*/
	register struct fltinfo *fltptr;
	int inst;

	minfo.buserr++;
	p = u.u_procp;
	u.u_traptype = TRAPNORM;
	old_ar0 = u.u_ar0;	/* save old ar0 and restore on return	      */
	u.u_ar0 = regptr;	/* set up new ar0 for current regs	      */
	ppc = &u.u_ar0[SCIP];	/* address of pc - referenced very often      */

	/*
	 * Calculate the address of the bus error 
	 *	This is the logical address to the MMU
	 */

	syst = u.u_stime;

/*  Check for code faults. */
	if(*ppc & EXCEPT_IP){
		if(USERMODE(u.u_ar0[PS])){   /*  User code fault */
			regptr[SFIP] = regptr[SNIP] & ~EXCEPT_IP;
			regptr[SNIP] = *ppc & ~EXCEPT_IP;  /* Rerun fault*/
			if(typesig = fault(*ppc,p,0, 0)){
				u.u_trapflag = 1;	/* copy fault info. to u area */
				if (kdebug & 8)
				{
					procname(0);
					printf("iacc: pc = 0x%x, ps = 0x%x",
						regptr[PC],regptr[PS]);
					printf(": psignal(%x, %x)\n", 
						p->p_pid, typesig);
				}
				psignal(p, typesig);
				u.u_siginfo.src = 1;
				u.u_siginfo.nwords = 1;
				u.u_siginfo.args[0] = 1;
				u.u_oldsig = p->p_hold;
				p->p_flag |= SOMASK;
				p->p_hold.s[0] = ~( sigmask(SIGSEGV) | 
										sigmask(SIGKILL) |
				       					sigmask(SIGBUS) );
				p->p_hold.s[1] = ~0;
					/* except SIGBUS, SIGSEGV & SIGKILL */
				if ((p->p_flag & SPRSTOP) || (isset(p) && issig()))
					psig(*ppc);
				p->p_hold = u.u_oldsig;	/* reset held signals */
				p->p_flag &= ~SOMASK;
				goto out;
			}
		}
		else {  /* Kernel mode code fault. */
			procname(regptr);
			cmn_err(CE_WARN,"pc   = 0x%x, ps   = 0x%x",*ppc,u.u_ar0[PS]);
			cmn_err(CE_PANIC,"       *** Kernel Code fault *** ");
			
		}
	}

/* Check for data access faults. */
	if(u.u_ar0[PS] & PS_DEXC){  /* Fault? */

		/* Only TRAN1 is known to be faulted.  If its kernel, die */
		if(regptr[TRAN1] & TRN_MD) {   /* kernel data address fault? */
			if(*ppc >= (int) nofulow && *ppc <= (int) nofuhigh){
				/* Must be read of nomem from /dev/mem */
				regptr[SFIP] = (int)errret | VALIDIP;
				regptr[SNIP] = ~VALIDIP;
				if (u.u_caddrflt){
					regptr[SFIP] = u.u_caddrflt | VALIDIP;
					regptr[SNIP] = ~VALIDIP;
				}
				regptr[TRAN1] &= ~TRN_VALID;  /* Don't rerun. */
			}
			else{
				procname(regptr);
				cmn_err(CE_WARN,"pc= 0x%x, ps= 0x%x, vaddr= %x",
				*ppc, u.u_ar0[PS],regptr[ADDR1]);
				cmn_err(CE_PANIC,"       *** Kernel data fault *** ");
			}
		}
fltptr = (struct fltinfo *) &regptr[TRAN3];
if ((fltptr->tran & 3) == 3) {
	register int j, data, addr;
	inst = fuiword(regptr[PC] & ~3);
	if ((inst & 0xf0000000) == 0x20000000) { /* if store */
		j = (inst >> 26) & 0x3;	/* type */
		i = (inst >> 21) & 0x1f; /* source */
		addr = (inst >> 16) & 0x1f;
		addr = regptr[addr] + (inst & 0xffff);
		if (addr != (int) fltptr->addr)
			j = 0;
		else if (j == 3) {
			j = ((unchar) fltptr->data != (unchar) regptr[i]);
			data = (unchar) regptr[i];
			data |= (data << 24) | (data << 16) | (data << 8); 
		}
		else if (j == 2)  {
			j = ((ushort) fltptr->data != (ushort) regptr[i]);
			data = (ushort) regptr[i];
			data |= (data << 16); 
		}
		else if (j == 1) {
			j = ((ulong) fltptr->data != (ulong) regptr[i]);
			data = (ulong) regptr[i];
		}
		else {
			if ((fltptr->tran & TRN_DB) == 0)
				i++;
			j = ((ulong) fltptr->data != (ulong) regptr[i]);
			data = (ulong) regptr[i];
		}
			
		if ( j ) {
			procname(0);
			printf("pc = %x, inst = %x, r1 = %x, sp = %x, r%d = %x\n",
				regptr[PC], inst, regptr[R1], regptr[SP], i, regptr[i]);

			fltptr = (struct fltinfo *) &regptr[TRAN1];
			for ( j = 0; j < NUMTRAN ; j++, fltptr++ ) {
				printf("(%d) %x %x %x\n", j, fltptr->tran, 
					fltptr->addr, fltptr->data);
			}
			fltptr = (struct fltinfo *) &regptr[TRAN3];
			fltptr->data = data;
		}
	}
}
		u.u_ar0[PS] &= ~PS_DEXC;    /* clear it */
		for(i = 0, fltptr = (struct fltinfo *) &regptr[TRAN1]; i < NUMTRAN; i++, fltptr++){
			if(fltptr->tran & TRN_VALID) {    /* If not NOP */
				if(!(fltptr->tran & TRN_MD) && (typesig = fault(fltptr->addr, p,(fltptr ->tran & TRN_RW), i))) {  
					if(u.u_ar0[PS] & PS_SUP && *ppc >= (int) nofulow && *ppc <= (int) nofuhigh){
						/* fault during userio */
						regptr[SFIP] = (int)errret | VALIDIP;
						regptr[SNIP] = ~VALIDIP;
						if (u.u_caddrflt){
							regptr[SFIP] = u.u_caddrflt | VALIDIP;
							regptr[SNIP] = ~VALIDIP;
						}
						
					}
					else{
						/* User fault, send signal */
						
xmemins:
						u.u_traptype = TRAPBUS;	/* bus error trap type */
						u.u_trapflag = 1;	/* copy fault info. to u area */

						if (kdebug & 8)
						{
							procname(0);
							printf("dacc: pc = 0x%x, ps = 0x%x, tr = 0x%x, ad = 0x%x",
								regptr[PC],regptr[PS], fltptr->tran, fltptr->addr);
							printf(":psignal(0x%x, 0x%x)\n", p->p_pid, typesig);
						}
						psignal(p, typesig);	/* post bus error signal */
						u.u_siginfo.src = 1;
						u.u_siginfo.nwords = 4;
						u.u_siginfo.args[0] = 2;
						u.u_siginfo.args[1] = (long) fltptr->addr;
						u.u_siginfo.args[2] = fltptr->tran;
						u.u_siginfo.args[3] = fltptr->data;
					
						u.u_oldsig = p->p_hold;	/* Hold off all other signals */
						p->p_flag |= SOMASK;
						p->p_hold.s[0] = ~(sigmask(SIGSEGV) | 
											sigmask(SIGKILL) |
				       						sigmask(SIGBUS) );
						p->p_hold.s[1] = ~0;
						/* except SIGBUS, SIGSEGV & SIGKILL */
						if ((p->p_flag & SPRSTOP) || (isset(p) && issig()))
							psig(fltptr->addr);

						p->p_hold = u.u_oldsig;	/* reset held signals */
						p->p_flag &= ~SOMASK;
						goto out;
					}
				}
				else {  /* paged in or supervisor, complete the access */
#if 0
					if(fltptr->tran & TRN_LK /* If xmem */
					   && i!=2){	/* on stage 0 or 1, */
						/* combine into 1 transaction */
						fltptr->data=(fltptr+1)->data;
						(fltptr+1)->tran &= ~TRN_VALID;
						fltptr->tran &=
							 ~(TRN_RW|TRN_SIGN);
					}
#endif
					if(fltptr->tran & TRN_LK) /* If xmem */ {
						typesig = SIGSEGV;
						goto xmemins;
					}

					/* complete access in software */
					dotran(regptr,fltptr);
					if(fltptr->tran & TRN_DB && (i==2)){
						/* Double in first stage. */
						i--; /* back up count */
						fltptr->addr +=4;  /*bump addr*/
						fltptr->tran &= ~TRN_DB; 
						fltptr->tran |= TRN_VALID; 
						if(fltptr->tran & TRN_RW){
							/* st.d */
							if(USERMODE(u.u_ar0[PS])){
								inst=fuiword(*ppc & ~0x3);
							}
							else {
								inst= *(int *)(*ppc & ~0x3);
							}
							fltptr->data = u.u_ar0[((inst >> 21) + 1) & 0x1F];
						}
						else{
							/* ld.d */
							fltptr->tran = (fltptr->tran & ~0xF80) | (((fltptr->tran & 0xF80) + (1<<7)) & 0xF80);
						}
						fltptr--;
					}
				}

			}
		}
	}

out:
	if(USERMODE(u.u_ar0[PS])){
		if (runrun != 0)
			qswtch();

		if (u.u_prof.pr_scale)
		  	addupc((caddr_t)*ppc ,&u.u_prof, (int)(u.u_stime-syst));
	}

	

	u.u_ar0 = old_ar0;
	return(typesig);
}

struct fltin {
	struct fltinfo flt;
	int pc, pid, pd;
} fltin[32];
int fltindex;
dotran(regptr,fltptr)
/* Complete a memory transaction */
/*  This subroutine will complete a pending memory transaction based
   on the information present in the shadow transaction registers. 
   The register image on the stack is corrected if neccessary */

/*  This is a heavily used subroutine.  Candidate for ml. */

	register struct fltinfo *fltptr;
	register int *regptr;
{
	int size;
	int write;
	register int i;

/*if (kdebug)*/ {
register struct fltin *fp;
extern lastpd;
i = fltindex;
fp = &fltin[i];
fp->flt = *fltptr;
fp->pc = regptr[PC];
fp->pid = u.u_procp->p_pid;
fp->pd = lastpd;
fltindex = (i + 1) & 31;
}

	size = fltptr->tran & STROBES;
	write = (fltptr->tran & TRN_RW);   
	fltptr->tran &= ~TRN_VALID;   /* Mark transaction complete. */

	if(write){  /* Complete a st */
		if (kdebug & 0x10)
			printf("st %x %x %x\n", 
				fltptr->tran, fltptr->addr, fltptr->data);
		switch (size){

		case TRN_WORD:
		if(fltptr->tran & TRN_MD)
			*(int *)(fltptr->addr) = fltptr->data;
		else
			suword(fltptr->addr,fltptr->data);
			
		break;

		case UPHALF:
		if(fltptr->tran & TRN_MD)
			*(short *)(fltptr->addr) = fltptr->data;
		else
			suhalf(fltptr->addr,fltptr->data);
		break;

		case LOHALF:
		if(fltptr->tran & TRN_MD)
			*(short *)(fltptr->addr+2) = fltptr->data;
		else
			suhalf(fltptr->addr+2,fltptr->data);
		break;

		case HIBYTE:
		if(fltptr->tran & TRN_MD)
			*(fltptr->addr) = fltptr->data;
		else
			subyte(fltptr->addr,fltptr->data);
		break;

		case MHBYTE:
		if(fltptr->tran & TRN_MD)
			*(fltptr->addr+1) = fltptr->data;
		else
			subyte(fltptr->addr+1,fltptr->data);
		break;

		case MLBYTE:
		if(fltptr->tran & TRN_MD)
			*(fltptr->addr+2) = fltptr->data;
		else
			subyte(fltptr->addr+2,fltptr->data);
		break;

		case LOBYTE:
		if(fltptr->tran & TRN_MD)
			*(fltptr->addr+3) = fltptr->data;
		else
			subyte(fltptr->addr+3,fltptr->data);
		break;

		}
		
	}
	else {  /* Complete ld */
		switch (size){

		case TRN_WORD:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(int *)(fltptr->addr);
		else{
			if(fltptr->tran & TRN_LK)    /* If xmem word */
				fltptr->data = xmem(fltptr->addr, fltptr->data, 1);
			else{
				fltptr->data = fuword(fltptr->addr);
			}
		}
		break;

		case UPHALF:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(short *)(fltptr->addr) & 0xFFFF;
		else
			fltptr->data=fuhalf(fltptr->addr);
		break;

		case LOHALF:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(short *)(fltptr->addr+2) & 0xFFFF;
		else
			fltptr->data=fuhalf(fltptr->addr+2);
		break;

		case HIBYTE:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(fltptr->addr) & 0xFF;
		else{
			if(fltptr->tran & TRN_LK)    /* If xmem word */
				fltptr->data = xmem(fltptr->addr, fltptr->data, 0);
			else
				fltptr->data=fubyte(fltptr->addr);
		}
		break;

		case MHBYTE:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(fltptr->addr+1) & 0xFF;
		else{
			if(fltptr->tran & TRN_LK)    /* If xmem word */
				fltptr->data = xmem(fltptr->addr+1, fltptr->data, 0);
			else
				fltptr->data=fubyte(fltptr->addr+1);
		}
		break;

		case MLBYTE:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(fltptr->addr+2) & 0xFF;
		else
		{
			if(fltptr->tran & TRN_LK)    /* If xmem word */
				fltptr->data = xmem(fltptr->addr+2, fltptr->data, 0);
			else
				fltptr->data=fubyte(fltptr->addr+2);
		}
		break;

		case LOBYTE:
		if(fltptr->tran & TRN_MD)
			fltptr->data = *(fltptr->addr+3) & 0xFF;
		else
		{
			if(fltptr->tran & TRN_LK)    /* If xmem word */
				fltptr->data = xmem(fltptr->addr+3, fltptr->data, 0);
			else
				fltptr->data=fubyte(fltptr->addr+3);
		}
		break;

		}
		if(fltptr -> tran & TRN_SIGN){
			switch (size){
			case UPHALF:
			case LOHALF:
				if(fltptr->data & 0x8000)
					fltptr->data |= 0xFFFF0000;
			break;
			case HIBYTE:
			case MHBYTE:
			case MLBYTE:
			case LOBYTE:
				if(fltptr->data & 0x80)
					fltptr->data |= 0xFFFFFF00;
			break;
			}
		}
		/* Load register with result and clear scoreboard bit */
		i = R0+((fltptr->tran & TRN_REG)>>7);
		regptr[i] = fltptr->data;
		if (kdebug & 0x10) {
			printf("ld %x %x %x", 
				fltptr->tran, fltptr->addr, fltptr->data);
		}

	}
}

procname(regptr)
register long *regptr;
{
	register unsigned char *p;
	register c, n;

	if (regptr) {
		printf("register dump LA: %x, PA: %x\n", 
			regptr, kvtophys(regptr));
		for ( n = 0; n < 46; n += 4, regptr += 4 ) {
			printf("R%d: %x:%x:%x:%x\n",
				n, regptr[0], regptr[1], regptr[2], regptr[3]);
		}
	}
	p = (unsigned char *) u.u_psargs;
	n = 0;
	while (c = *p) {
		if (c == ' ' || c  == '\0')
			break;
		if (n >= 16)
			break;
		n++;
		p++;
	}
	*p = 0;
	printf("%s: ", u.u_psargs);
	*p = c;
}


fixalign(regptr)
register long *regptr;
{
	register unsigned inst, i, kva, uva, sz;
	int ld, issigned;
	static unsigned char alsztab[] = { 8, 4, 2, 0 };

	inst = fuiword(regptr[PC] & ~0x3);
	i = (inst >> 26) & 0x3f;
	if (i == 0 || i == 1)		/* xmem */
		goto badret;
	kva = (int) &regptr[(inst >> 21) & 0x1F];
	i = (inst >> 28) & 0xf;
	if (i < 3) {
		/* immediate mode */
		uva = regptr[(inst >> 16) & 0x1f] + (inst & 0xffff);
		sz = alsztab[(inst >> 26) & 3];
		if (sz == 0)
			goto badret;
		ld = ( i != 2);	
		issigned = (i == 1);

	}
	else if (i == 0xf) {
		/* register indexed mode */
		i = (inst >> 10) & 0x3f;
		if (i == 0 || i == 1)		/* xmem */
			goto badret;
		i = (inst >> 12) & 0xf;
		if (i < 3) {
			sz = alsztab[(inst >> 10) & 3];
			if (sz == 0)
				goto badret;
			ld = ( i != 2);	
			issigned = (i == 1);
			if (inst & 0x200) {
				/* scaled */
				uva = regptr[(inst >> 16) & 0x1f] + 
						(sz * regptr[inst & 0x1f]);
			}
			else {
				/* unscaled */
				uva = regptr[(inst >> 16) & 0x1f] + 
						regptr[inst & 0x1f];
			}
		}
		else
			goto badret;
	}
	else 
		goto badret;
	if ((u.u_procp->p_compat & SALIGN) == 0)
		goto badret;
	if (u.u_procp->p_compat & SRECALN) {
		register long *p;
		p = (long *) u.u_filler;
		i = p[2];
		if (i >= p[0] && i < p[1]) {
			suword(i, regptr[PC] & ~3);
			i += 4;
			p[2] = i;
		}
	}
	/* do the operation */
	if (ld) {
		if (copyin((caddr_t)uva, (caddr_t)kva, sz))
			goto badret;
		if (sz == 2) {
			if(issigned)
				*(long *) kva = *(short *) kva;
			else
				*(long *) kva = *(ushort *) kva;
		}
		return 1;
	}
	else {
		if (sz == 2)
			kva += 2;
		if (copyout((caddr_t)kva, (caddr_t)uva, sz))
			goto badret;
		return 1;
	}
badret:
	procname(0);
	printf("Alignment problem PC = %x, instr = %x, uva = %x, R1 = %x\n", 
			regptr[PC], inst, uva, regptr[R1]);
	return 0;
}

/*
 * nonexistent system call-- signal bad system call.
 */
nosys()
{
	u.u_siginfo.src = 2;
	u.u_siginfo.nwords = 1;
	psignal(u.u_procp, SIGSYS);
}

parbuserr(berrsrc, vaddr, pc, Status)
{
	cmn_err(CE_WARN,"Kernel Parity Error (PC=0x%x, PS=0x%x)",pc,Status);
/*	switch (berrsrc & (STAT2_VMEBERR | STAT2_VMXBERR | STAT2_LBTO)) {
	case STAT2_VMEBERR:
		cmn_err(CE_PANIC,"VME131: VME bus error at 0x%x",vaddr);
	case STAT2_VMXBERR:
		cmn_err(CE_PANIC,"VME131: VMX bus error at 0x%x",vaddr);
	case STAT2_LBTO:
		cmn_err(CE_PANIC,"VME131: Local bus time out at 0x%x",vaddr);
	default:
	cmn_err(CE_PANIC,"VME131: Unknown bus error (0x%x) at 0x%x",berrsrc,vaddr);
	}
*/
	cmn_err(CE_PANIC,"VME131: Unknown bus error (0x%x) at 0x%x",berrsrc,vaddr);
}

fpui()
/* Process a floating point imprecise exception */
{
	return(0);
}

fpup()
/* Process a floating point precise exception */
{
	return(0);
}

stray(vector)
int vector;
{
	cmn_err(CE_WARN, " ***** Stray interrupt,  SSR = 0x%x *****\n", vector);
}
