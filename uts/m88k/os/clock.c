/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/clock.c	10.8"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/tuneable.h"
#include "sys/psw.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/callo.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/immu.h"
#include "sys/user.h"
#include "sys/conf.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/var.h"
#include "sys/cmn_err.h"
#include "sys/swap.h"

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	reprime clock
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	alarm clock signals
 *	jab the scheduler
 */

unsigned	prfstat;
extern struct sit cicsit;
extern int	vhand();
extern int	bdflush();
extern int	switching;
struct scanitimer {
	struct proc *first,*last;
} itproc;


time_t	time,	/* time in seconds since 1970 */
	lbolt;	/* time in HZ since last boot */
int	lticks;

int	one_sec = 1;
int	vhandcnt;	/* counter for t_vhandr				*/
int	bdflushcnt;	/* counter for t_bdflushr			*/
uint	sxbrkcnt;	/* count of procs whose current stat is SXBRK	*/

clock(pc, ps, ticks)
caddr_t pc;
int	ps;
register ticks;
{
	register struct proc *pp;
	register struct proc *lp;
	register a;
	register int	retval = 0;
	static rqlen, sqlen;
	extern char waitloc;
	extern char waitloc2;

	/*
	 * if panic stop clock
	 */

	if (panicstr) {
		clkreld(0);
		return(retval);
	} else {
		if (clkreld(1))
			return(retval);
	}

	/*
	 * if any callout active, update first non-zero time
	 */

	if (callout[0].c_func != NULL) {
		register struct callo *cp;
		register int tdole;
		cp = &callout[0];
		while (cp->c_time<=0 && cp->c_func!=NULL)
			cp++;
		tdole = ticks;
		while(cp->c_time<tdole && cp->c_func!=NULL) {
			tdole -= cp->c_time;
			cp->c_time = 0;
			cp++;
		}
		cp->c_time -= tdole;
	}
	if (callout[0].c_time <= 0)
		timepoke();
	if (prfstat & PRF_INT)	/* ON and using INTernal system clock */
		prfintr(pc, ps);
	if (USERMODE(ps)) {
		a = CPU_USER;
		if (u.u_stime)
			u.u_utime += ticks;
		else {
			u.u_utime += (ticks - 1);
			u.u_stime = 1;
		}
		if (u.u_prof.pr_scale)
			retval = ticks;
	} else {
		if (pc >= &waitloc && pc < &waitloc2) {
			if (syswait.iowait+syswait.swap+syswait.physio) {
				a = CPU_WAIT;
				if (syswait.iowait)
					sysinfo.wait[W_IO]++;
				if (syswait.swap)
					sysinfo.wait[W_SWAP]++;
				if (syswait.physio)
					sysinfo.wait[W_PIO]++;
			} else
				if (sxbrkcnt)
					a = CPU_SXBRK;
				else
					a = CPU_IDLE;
		} else {
			a = CPU_KERNEL;
			if (!switching)
				if (u.u_utime)
					u.u_stime += ticks;
				else {
					u.u_stime += (ticks - 1);
					u.u_utime = 1;
				}
			if (server())
				 dinfo.serve++;	/* ticks servicing remote */
		}
	}
	sysinfo.cpu[a]++;
	pp = u.u_procp;

	/* update memory usage for the currently running process */

	if (pp->p_stat==SRUN) {
		register preg_t	*prp;
		register reg_t	*rp;

		for (prp = pp->p_region; rp = prp->p_reg; prp++)
			if (rp->r_refcnt > 1)
				u.u_mem += rp->r_nvalid/rp->r_refcnt;
			else
				u.u_mem += rp->r_nvalid;
	}

	if (!switching  &&  pp->p_cpu < 80)
		pp->p_cpu++;
	lbolt += ticks;	/* time in ticks */
	if ((lticks -= ticks) <= 0)
		runrun++;

	/* "double" long arithmetic for minfo.freemem */
	{	unsigned long ofrmem;

		ofrmem = minfo.freemem[0];
		minfo.freemem[0] += freemem;
		if (minfo.freemem[0] < ofrmem)
			minfo.freemem[1]++;
	}
#define SEC pp->p_itimer.it_value.tv_sec
#define USEC pp->p_itimer.it_value.tv_usec

	lp = itproc.last;
	if (pp = itproc.first) for (; pp <= lp; pp++)
		if (pp->p_stat && (SEC || USEC)) {
			if (ticks > (0x7fffffff/USEC_PER_TICK)) {
				USEC = 0;
				if ((SEC -= ticks/HZ) < 0)
					SEC = 0;
			} else {
				USEC -= ticks * USEC_PER_TICK;
				while (USEC < 0) {
					USEC += timerround(1000000);
					if ((SEC -= 1) < 0) {
						SEC = 0;
						USEC = 0;
					}
				}
			}
			if (SEC == 0 && USEC == 0) {
				psignal(pp, SIGALRM);
				runrun++;
				pp->p_itimer.it_value = pp->p_itimer.it_interval;
				if (SEC == 0 && USEC == 0)
					scanitimer();
			}
		}

	if ((one_sec -= ticks) <= 0)
	{

		if (BASEPRI(ps))
			return(retval);

		one_sec += HZ;

		minfo.freeswap = 0;
		{	int	i;
			swpt_t	*st;
			for (i=0, st=swaptab; i<MSFILES; i++, st++)
			{	if (st->st_ucnt == NULL)
					continue;
				minfo.freeswap += st->st_nfpgs << DPPSHFT;
			}
		}
		++time;
		runrun++;
		rqlen = 0;
		sqlen = 0;
		for (pp = &proc[0]; pp < (struct proc *)v.ve_proc; pp++)
			if (pp->p_stat) {
				if (pp->p_time != 127)
					pp->p_time++;
				pp->p_cpu >>= 1;
				if (pp->p_pri >= (PUSER-NZERO))
					pp->p_pri = calcppri(pp);

				if (pp->p_stat == SRUN)
					if (pp->p_flag & SLOAD)
						rqlen++;
					else
						sqlen++;
			}

		if (rqlen) {
			sysinfo.runque += rqlen;
			sysinfo.runocc++;
		}
		if (sqlen) {
			sysinfo.swpque += sqlen;
			sysinfo.swpocc++;
		}

		dinfo.nservers += nservers;	/* cumul. tally of servers */
		if (idleserver) {
			dinfo.srv_que += idleserver;	/* idle servers in Q */
			dinfo.srv_occ++;		/* sec's srv Q occ'd */
		}
		if (msglistcnt) {
			dinfo.rcv_que += msglistcnt;	/* receive descr in Q */
			dinfo.rcv_occ++;		/* sec's rcv Q occ'd */
		}


		/* wake up page ageing proc every t_vhandr secs
		 * unless memory is available
		 */

		if (--vhandcnt <= 0) {
			vhandcnt = tune.t_vhandr;
			if (freemem < tune.t_vhandl)
				wakeup(vhand);
		}

		/*	Wake up bdflush to write out DELWRI
		**	buffers which have been around too long.
		*/

		if (--bdflushcnt <= 0) {
			bdflushcnt = tune.t_bdflushr;
			wakeup(bdflush);
		}

		/*	Wake up sched if it is sleeping for
		**	one second to let things settle.
		*/

		if (runin != 0) {
			runin = 0;
			setrun(&proc[0]);
		}
	}
	return(retval);
}

/*
 * timeout is called to arrange that fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout structure.
 * The time in each structure entry is the number of HZ's more
 * than the previous entry. In this way, decrementing the
 * first entry has the effect of updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */

int	timeid;

timeout(fun, arg, tim)
int (*fun)();
caddr_t arg;
{
	register struct callo *p1, *p2;
	register int t;
	register int id;
	int s;

	t = tim;
	p1 = &callout[0];
	s = spl7();
	if (callout[v.v_call-2].c_func)
		cmn_err(CE_PANIC,"Timeout table overflow");
	while (p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	p1->c_time -= t;
	p2 = p1;
	while (p2->c_func != 0)
		p2++;
	while (p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		(p2+1)->c_id = p2->c_id;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	p1->c_id = id =  ++timeid;
	splx(s);
	return(id);
}

untimeout(id)
register id;
{
	register struct callo *p1, *p2;
	register s;

	s = splhi();
	for (p1 = callout; p1->c_func != NULL; p1++)
		if (p1->c_id == id) {
			p2 = p1 + 1;
			p2->c_time += p1->c_time;
			do
				*p1++ = *p2++;
			while (p1->c_func != NULL);
			break;
		}
	splx(s);
}

/*
 * timein() is called via a PIR9 which was set by timepoke()
 * in clock()
 */
timein()
{
	register struct callo *p1, *p2;
	register s;
	struct callo svcall;

	if (callout[0].c_func == NULL)
		return;
	s = splhi();
	if (callout[0].c_time <= 0) {
		p1 = p2 = &callout[0];
		while (p1->c_func != 0 && p1->c_time <= 0) {
			svcall = *p1;
			/* move up entries first so untimeout won't find us */
			do {
				/* *p2 = *++p1; */
				p1++;
				*p2 = *p1;
			}
			while (p2++->c_func != NULL);

			(*svcall.c_func)(svcall.c_arg);
			/* raise mask again to allow functions in callout */
			/* to drop it if they need to and still protect	*/
			/* callout table structure from clock interrupts */
			(void) splhi();
			p1 = p2 = &callout[0];
		}
	}
	splx(s);
}

#define	PDELAY	(PZERO-1)
delay(ticks)
{
	int s;
	extern wakeup();

	if (ticks<=0)
		return;
	s = splhi();
	timeout(wakeup, (caddr_t)u.u_procp+1, ticks);
	sleep((caddr_t)u.u_procp+1, PDELAY);
	splx(s);
}

scanitimer()		/* must be called with ints off */
{
	register struct proc *pp, *ep;
	register struct scanitimer *sp;

	sp = &itproc;
	sp->first = NULL;
	sp->last = NULL;
	ep = (struct proc *) v.ve_proc;
	for (pp = &proc[0]; pp < ep; pp++)
		if (pp->p_stat && timerisset(&(pp->p_itimer.it_value))) {
			if (sp->first == NULL)
				sp->first = pp;
			sp->last = pp;
		}
}
