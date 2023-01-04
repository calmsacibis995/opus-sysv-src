h16432
s 00173/00000/00000
d D 1.1 90/03/06 12:28:57 root 1 0
c date and time created 90/03/06 12:28:57 by root
e
u
U
t
T
I 1
/* SID @(#)pc.c	1.2 */

/* 
 * PC low level driver
 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/iorb.h"
#include "sys/compat.h"
#include "sys/cb.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/sysmacros.h"

#define PC_BUSY 1  /* local request in progress */
#define PC_WANT 2  /* there are local requesters waiting */

#define swapiorb(iorb) \
	swapsa(&iorb->dix, 2); \
	swapla(&iorb->dst, 10);

int pc_flags;
struct iorb *pc_cb;
unsigned short cpmsk, rqmsk;

pcstrategy(ptr)
register struct iorb *ptr;
{
	register s;
	register i;
	register long put,get,mask;
	ptr->st = S_GO;
	ptr->flg = 0;
	ptr->err = IORB_MAGIC;
	swapiorb(ptr);
	s=spl7();
	put = swaps(COMMPAGE->rqput);
	get = swaps(COMMPAGE->rqget);
	mask = rqmsk;
	if (((get - put) == 1) || ((put == mask) && (get == 0)))
		panic("rqq overrun");
	COMMPAGE->rqq[put] = (long) swapl(ptr);
	put = (put+1) & mask;
	COMMPAGE->rqput = swaps(put);

	SETATINTR();
	splx(s);
}

pcintr(pc,ps)
caddr_t pc;
unsigned int ps;
{
	register struct iorb *ptr;
	register int i;
	extern int (*intdevsw[])();
	extern nulldev();
	extern int intdevcnt;
	unsigned devtype;
	unsigned dix;

	while (COMMPAGE->cpget != COMMPAGE->cpput) {
		i = swaps(COMMPAGE->cpget);
		ptr = (struct iorb *) swapl(COMMPAGE->cpq[i]);
		i = (i+1) & cpmsk;
		COMMPAGE->cpget = swaps(i);
		SETATINTR();
		swapiorb(ptr);
		if ((dix = ptr->dix) >= COMMPAGE->dixsiz) {
			SCOPE_SYNC();
			printf("pcintr: iorb=0x%x: bad dix=%d, then %d\n",
				ptr, dix, ptr->dix);
			panic("pcintr");
		}
		devtype = COMMPAGE->dix[dix];
		if (devtype >= intdevcnt) {
			SCOPE_SYNC();
			printf("pcintr: iorb=0x%x: bad devtype=%d dix=%d, then %d,%d\n",
				ptr, devtype, dix, COMMPAGE->dix[ptr->dix], ptr->dix);
			panic("pcintr");
		}
		if (ptr->st != S_DONE) {
			SCOPE_SYNC();
			printf("pcintr: iorb=0x%x: bad status %d\n", ptr, ptr->st);
			panic("pcintr");
		}
		if (ptr->err == E_SYNC) {
			SCOPE_SYNC();
			printf("pcintr: iorb=0x%x: opmon sync error: was %d s/b %d\n",
					ptr, ptr->dst & 0xff, (ptr->dst >> 8) & 0xff);
			panic("pcintr");
		}
		if (ptr->err == E_DIX) {
			SCOPE_SYNC();
			printf("pcintr: iorb=0x%x: opmon dix error: %d\n",
					ptr, dix);
			panic("pcintr");
		}
		ptr->st = S_IDLE;
		if (ptr == pc_cb)
			wakeup((caddr_t)pc_cb);
		else if (ptr->f)
			(*ptr->f)(ptr,pc,ps);
		else
			(*intdevsw[devtype])(ptr,pc,ps);
	}
}

/*
**  trap.s got PC interrupt with current interrupt level >0
*/
badspl(spl)
int spl;
{
	printf("Interrupt received with spl=%d\n", spl);
	panic("badspl");
}

/*
 * Initialize driver state.
 */
pcinit()
{
	pc_cb = (struct iorb *)dmastatic(sizeof(struct iorb));
	cpmsk = swaps(COMMPAGE->cpmsk);
	rqmsk = swaps(COMMPAGE->rqmsk);
}

/*
 * Issue a local request and wait for it to finish.
 */
pcwait(IORB, len)
struct iorb *IORB;
int len;
{
	int s;
	extern int clkrunning;

	s = spl7();
	while (pc_flags & PC_BUSY) {
		pc_flags |= PC_WANT;
		sleep((caddr_t)&pc_flags, PRIBIO);
	}
	pc_flags |= PC_BUSY;
	bcopy(IORB, pc_cb, len);
	pc_cb->st = S_GO;
	pcstrategy(pc_cb);
	if (clkrunning) {
		while (pc_cb->st != S_IDLE) {
			sleep((caddr_t)pc_cb, PRIBIO);
		}
	} else {
		register struct iorb *i;
		i = pc_cb;
		while (i->st != S_IDLE) {
			if (COMMPAGE->cpget != COMMPAGE->cpput) {
				/* simulate interrupt */
				pcintr((caddr_t)pcwait, 0);
			}
		}
	}
	bcopy(pc_cb, IORB, len);
	pc_flags &= ~PC_BUSY;
	if (pc_flags & PC_WANT) {
		pc_flags &= ~PC_WANT;
		wakeup((caddr_t)&pc_flags);
	}
	splx(s);
}

E 1
