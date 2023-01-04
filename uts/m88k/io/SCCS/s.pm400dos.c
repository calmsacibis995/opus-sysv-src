h65069
s 00477/00000/00000
d D 1.1 90/03/06 12:28:56 root 1 0
c date and time created 90/03/06 12:28:56 by root
e
u
U
t
T
I 1
/* SID @(#)dos.c	1.7 */

/* 
 * PC DOS command driver
 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#if SVR3
#include "sys/immu.h"
#else
#include "sys/page.h"
#endif
#include "sys/errno.h"
#include "sys/buf.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/elog.h"
#include "sys/iobuf.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/compat.h"
#include "sys/iorb.h"
#include "sys/systm.h"
#include "sys/cb.h"
#include "sys/doscomm.h"

int dosprint();

struct iorb *sys_cb;  /* control block pointer for system commands */
struct iorb *time_cb;  /* control block pointer for tod commands */

struct dos_info {
	int flags;  /* state flags for driver */
	int ccmd;  /* current command being executed */
	union {  /* copy of user request */
		struct dos_ctrl dctrl;
		struct sys_ctrl sctrl;
	} cmd;
	struct iorb *cb;  /* control block pointer for commands */
	int dosdix;  /* device table index for dos device */
	struct buf *bp;  /* buf struct used in physio call */
} dos_info;

/* dos_info.flags definitons */

#define DOSAVAIL	0x01	/* dos driver available */
#define DOSINUSE	0x02	/* dos cmd execution in progress */
#define DOSWANT		0x04	/* processes waiting for execution to finish */

/*
 * Open the dos driver.
 */
dosopen(dev)
minor_t dev;
{
	if (dev > 0 || (dos_info.flags & DOSAVAIL) == 0) {
		u.u_error = ENXIO;
		return;
	}
}

/*
 * Close the dos driver.
 */
dosclose(dev)
minor_t dev;
{

}

/*
 * Start the execution of a command.
 */
dosstrategy(bp)
register struct buf *bp;
{
	register struct io_dos *doscmd;
	register struct iorb *IORB;
	register struct io_gen *IOGEN;
	register struct dos_ctrl *dcp;
	register struct sys_ctrl *scp;
	register int base;
	register int reloc;
	register int s;

	dos_info.bp = bp;  /* save for interrupt routine */
	bp->b_raddr = (paddr_t)paddr(bp);
	bp->b_rcount = bp->b_bcount;
	base = dmamapbuf(bp);
	switch (dos_info.ccmd) {

		case DOSCMD:
		case INTCMD:
		IORB = (struct iorb *)dos_info.cb;
		doscmd = (struct io_dos *)IORB->arg;
		dcp = &dos_info.cmd.dctrl;
		IORB->val = dcp->reloc;
		IORB->cnt = dcp->len;
		IORB->buf = base;
		if (dos_info.ccmd == INTCMD) {
			IORB->cmd = PC_IOCTL;
		} else {
			IORB->cmd = PC_DVCTL;
		}
		s = spl7();
		pcstrategy(IORB);
		splx(s);
		/* dma will be freed in interrupt routine */
		break;

		case SYSCMD:
		IORB = (struct iorb *)dos_info.cb;
		IOGEN = (struct io_gen *)IORB->arg;
		scp = &dos_info.cmd.sctrl;
		IORB->cmd = PC_DVCTL;
		if (IOGEN->cmd == PC_CMDLINE ||
		    IOGEN->cmd == PC_OPVER ||
		    IOGEN->cmd == PC_READMEM ||
		    IOGEN->cmd == PC_GETENV ||
		    IOGEN->cmd == PC_PUTENV ||
		    (IOGEN->cmd & 0x80) ||
		    IOGEN->cmd == PC_WRITEMEM) {
			IORB->cnt = scp->len;
			IORB->buf = base;
		}
		s = spl7();
		executesyscmd(IORB);
		splx(s);
		if (IOGEN->cmd == PC_PAUSE && IOGEN->dst != 0xffff) {
			time = timeofday();
		}
		bp->b_flags |= B_DONE;  /* so physio will complete */
		dmaunmapbuf(bp);  /* execution complete, free dma now */
		break;
	}
}

#if m88k
#define SWAPIORB(type, iorb) \
	swapsa(&iorb->io_devstat, (sizeof(struct type)-(4*sizeof(char)))/2)
#else
#define SWAPIORB(type, iorb)
#endif
/*
 * Execute an ioctl to the dos driver.
 */
dosioctl(dev, cmd, arg, mode)
minor_t dev;
{

	register struct iorb *IORB;
	register struct io_dos *doscmd;
	register struct dos_ctrl *dcp;
	register struct sys_ctrl *scp;
	register struct io_gen *IOGEN;
	int index;
	int s;
	extern int dmamaxpg;

	/* only let one command execute at a time */
	while (dos_info.flags & DOSINUSE) {
		dos_info.flags |= DOSWANT;
		sleep((caddr_t)&dos_info, PSLEP);
	}
	dos_info.flags |= DOSINUSE;

	dos_info.ccmd = cmd;
	switch (cmd) {

		case DOSCMD:
		case INTCMD:
		IORB = (struct iorb *)dos_info.cb;
		doscmd = (struct io_dos *)IORB->arg;
		dcp = &dos_info.cmd.dctrl;
		if (copyin(arg, dcp, sizeof(struct dos_ctrl)) < 0) {
			u.u_error = EFAULT;
			goto out;
		}
		if (copyin(dcp->dptr, doscmd, sizeof(struct io_dos)) < 0) {
			u.u_error = EFAULT;
			goto out;
		}	
		IORB->dix = dos_info.dosdix;
		if ((poff(dcp->buf) + dcp->len) > CTOB(dmamaxpg)) {
			u.u_error = EFAULT;
			goto out;
		}
		SWAPIORB(io_dos, doscmd);
		if (dcp->len == 0) {
			dosphys(dev, 0, NBPP, B_WRITE);  /* placebo */
		} else {
			dosphys(dev, dcp->buf, dcp->len, B_READ);
		}
		SWAPIORB(io_dos, doscmd);
		if (copyout(doscmd, dcp->dptr, sizeof(struct io_dos)) < 0) {
			u.u_error = EFAULT;
		}
		break;

		case SYSCMD:
		IORB = (struct iorb *)dos_info.cb;
		IOGEN = (struct io_gen *)IORB->arg;
		scp = &dos_info.cmd.sctrl;
		if (copyin(arg, scp, sizeof(struct sys_ctrl)) < 0) {
			u.u_error = EFAULT;
			goto out;
		}
		if (copyin(scp->dptr, IOGEN, sizeof(struct io_gen)) < 0) {
			u.u_error = EFAULT;
			goto out;
		}	
		if (IOGEN->cmd == PC_CANCEL ||
		    IOGEN->cmd == PC_REINIT) {
			u.u_error = ENXIO;
			goto out;
		}
		IORB->dix = 0;
		SWAPIORB(io_gen, IOGEN);
		if ((IOGEN->cmd == PC_CMDLINE ||
		     IOGEN->cmd == PC_OPVER ||
		     IOGEN->cmd == PC_READMEM ||
		     IOGEN->cmd == PC_GETENV ||
		     IOGEN->cmd == PC_PUTENV ||
		     (IOGEN->cmd & 0x80) ||
		     IOGEN->cmd == PC_WRITEMEM) && scp->len != 0) {
			if ((poff(scp->buf) + scp->len) > CTOB(dmamaxpg)) {
				u.u_error = EFAULT;
				goto out;
			}
			dosphys(dev, scp->buf, scp->len, B_READ);
		} else {
			dosphys(dev, 0, NBPP, B_WRITE);
		}
		SWAPIORB(io_gen, IOGEN);
		if (copyout(IOGEN, scp->dptr, sizeof(struct io_gen)) < 0) {
			u.u_error = EFAULT;
		}
		break;

		default:
		u.u_error = ENXIO;
		break;	
	}

	out:
	dos_info.flags &= ~DOSINUSE;
	if (dos_info.flags & DOSWANT) {
		dos_info.flags &= ~DOSWANT;
		wakeup((caddr_t)&dos_info);
	}
}

/*
 * Interrupt handler for the dos driver.
 */
dosintr()
{
	register struct iorb *IORD;
	register struct buf *bp;
	register err;

	IORD = (struct iorb *)dos_info.cb;
	bp = dos_info.bp;
	err = IORD->err;
	if (err != E_OK && err != E_DEVICE) {
		bp->b_flags |= B_ERROR;
	}
	bp->b_flags |= B_DONE;
	dmaunmapbuf(bp);
	wakeup((caddr_t)bp);
}

/*
 * Print out an error report.
 */
dosprint(dev,str)
int dev;
char *str;
{
	printf("%s on DOS interface %d\n", str, minor(dev));
}

/*
 * Execute the system command 'cmd'. Returns the dst
 * from the completed request.
 */
syscmd(cmd,arg)
{
	register struct iorb *IORB = (struct iorb *)sys_cb;
	register struct io_sys *io_sys = (struct io_sys *) IORB->arg;
	register s;
	register devstat;

	s = spl7();
	IORB->cmd = PC_DVCTL;
	io_sys->io_cmd = cmd;
	io_sys->io_memaddr = swaps(arg);
	executesyscmd(IORB);
	if (IORB->err != E_OK) {
		printf("syscmd err=%x\n", IORB->err);
		panic("syscmd");
	}
	devstat = IORB->dst;
	splx(s);
	return(devstat);
}

/*
 * Get the current time of day from the pc.
 */
time_t 
timeofday()
{
#ifdef SIMULATOR
	return 0;
#else
	register s,t,i;
	register struct iorb *IORB = (struct iorb *)time_cb;
	register struct io_time *timecmd;
	static char dmsize[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	
	timecmd = (struct io_time *)IORB->arg;
	s = spl7();
	IORB->cmd = PC_DVCTL;
	timecmd->io_cmd = PC_TOD;
	executesyscmd(IORB);
	if (timecmd->io_error != E_OK) {
		printf("timeofday: err=%x\n", timecmd->io_error);
		panic("timeofday");
	}
	splx(s);
	t = 0;
	swapsp(&timecmd->io_year);
	if (timecmd->io_month == 0 || timecmd->io_day == 0) {
		goto badclk;
	}
	for (i=1970; i < timecmd->io_year; i++) {
		t += dysize(i);
	}
	if ((dysize(timecmd->io_year) == 366) & (timecmd->io_month >= 3)){
		t++;
	}
	while (--timecmd->io_month) {
		t += dmsize[timecmd->io_month - 1];
	}
	t += timecmd->io_day - 1;
	t = (24 * t) + timecmd->io_hour;
	t = (60 * t) + timecmd->io_min;
	t = (60 * t) + timecmd->io_sec;
	
	badclk:
	return(t);
#endif
}

/*
 * Number of days in the year 'year'.
 */
dysize(year)
{

	return((year % 4) == 0 ? 366 : 365);  /* dosn't work after year 2100 */
}

/*
 * Set the current time of day in the PC
 */
void
wtodc(t)
time_t t;
{
#ifdef SIMULATOR
	return;
#else
	register s;
	register struct iorb *IORB = (struct iorb *)sys_cb;

	s = spl7();
	IORB->cmd = PC_STOD;
	IORB->val = t;
	executesyscmd(IORB);
	if (IORB->err != E_OK) {
		printf("setpctime: err=%x\n", IORB->err);
		u.u_error = EINVAL;
	}
	splx(s);
#endif
}

#if m88k
#define swapiorb(iorb) \
	swapsa(&(iorb)->dix, 2); \
	swapla(&(iorb)->dst, 10);
#endif

/*
 * Execute the system request pointed to by IORB.
 * This is done under high interrupt level and does not return until
 * the command is complete. This routine should be called under high
 * interrupt level to protect COMMPAGE->syblk.
 */
executesyscmd(IORB)
register struct iorb *IORB;
{
register arg;

	IORB->err = E_OK;
	IORB->st = S_GO;
#if m88k
	swapiorb(IORB);
	COMMPAGE->syblk = swapl(IORB);
#else
	COMMPAGE->syblk = (unsigned long)IORB;
#endif
	COMMPAGE->syflg = S_GO;
	SETATINTR();
	while (COMMPAGE->syflg != S_DONE) {
	}
	COMMPAGE->syflg = S_IDLE;
#if m88k
	swapiorb(IORB);
#endif
}

/*
 * Execute a physio call to get 'buf' for 'len' bytes of user
 * space mapped and locked.
 */
dosphys(dev, buf, len, direction)
caddr_t buf;
int len;
int direction;
{

	u.u_base = buf;
	u.u_count = len;
	u.u_offset = 0;
	u.u_segflg = 0;
	physio(dosstrategy, 0, dev, direction);
}

/*
 *
 * Initialize driver state.
 */
dosinit()
{
	register int i;
	
	/* allocate request block for system commands */
	sys_cb = (struct iorb *)dmastatic(sizeof(struct iorb));
	sys_cb->dix = 0;
	sys_cb->arg = (unsigned long)dmastatic(sizeof(struct io_gen));

	/* allocate request block for tod commands */
	time_cb = (struct iorb *)dmastatic(sizeof(struct iorb));
	time_cb->dix = 0;
	time_cb->arg = (unsigned long)dmastatic(sizeof(struct io_time));

	/* allocate request block for dos commands */
	for (i = 0; i < COMMPAGE->dixsiz; i++) {
		if (COMMPAGE->dix[i] == DOS) {
			dos_info.cb = (struct iorb *)
				dmastatic(sizeof(struct iorb));
			dos_info.dosdix = i;
			dos_info.flags |= DOSAVAIL;
			dos_info.cb->arg = (unsigned long)
				dmastatic(sizeof(struct io_dos));
		}
	}
}
E 1
