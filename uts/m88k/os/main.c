/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/main.c	10.7"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/conf.h"
#include "sys/utsname.h"
#include "sys/cmn_err.h"

int	physmem;	/* Physical memory size in clicks.	*/
int	maxmem;		/* Maximum available memory in clicks.	*/
int	freemem;	/* Current available memory in clicks.	*/

extern int	icode[];
extern int	szicode;

/*
 *	Initialization code.
 *	fork	- process 0 to loop forever in sched
 *		- process 1 to loop forever exec'ing /etc/init
 *		- process 2 to loop forever in vhand
 *		- process 3 to loop forever in bdflush
 */

main()
{
	register int	(**initptr)();
	extern int	(*io_init[])();
	extern int	(*init_tbl[])();
	extern int	(*io_start[])();
	extern int	sched();
	extern int	vhand();
	extern int	bdflush();

	prt_where = PRW_CONS | PRW_BUF;

	/*	Call all system initialization functions.
	*/

	for (initptr= &io_init[0];  *initptr; initptr++) (**initptr)();
	for (initptr= &init_tbl[0]; *initptr; initptr++) (**initptr)();
	for (initptr= &io_start[0]; *initptr; initptr++) (**initptr)();

	printf( "\nUNIX System V.3 Release %s %s Version %s\n",
		utsname.release, utsname.machine, utsname.version);
	printf("Total real memory  = %d\n",  ctob(physmem) );
	printf("Available memory   = %d\n\n", ctob(freemem) );

	prt_where = PRW_CONS;
	printf("***********************************************************************\n\n");
	printf("Copyright (c) 1984 AT&T - All Rights Reserved\n\n");
	printf("THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T INC.\n");
	printf("The copyright notice above does not evidence any actual or\n");
	printf("intended publication of such source code.\n\n");
	vendorbanner();
	printf("***********************************************************************\n\n");
	prt_where = PRW_CONS | PRW_BUF;
	u.u_start = time;

	/*	This call of swapadd must come after devinit in case
	 *	swap is moved by devinit.  It must also come after
	 *	dskinit so that the disk is don'ed.  This call should
	 *	return 0 since the first slot of swaptab should be used.
	 */

	if (swapdev != NODEV)
		if (swapadd(swapdev, swplo, nswap) != 0)
			cmn_err(CE_PANIC, "main - swapadd failed");

	/*
	 * Make processes init, vhand, and bdflush
	 * and enter scheduling loop.
	 */

	if (newproc(0)) {
		register preg_t	*prp;
		register reg_t	*rp;
		u.u_cstime = u.u_stime = u.u_cutime = u.u_utime = 0;

		/*	Set up the text region to do an exec
		**	of /etc/init.  The "icode" is in misc.s.
		*/

		rp = allocreg(NULL, RT_PRIVATE, 0);
		prp = attachreg(rp, &u, 0, PT_DATA, SEG_RW);
		growreg(prp, btoc(szicode), DBD_DZERO);
		regrele(rp);

		if (copyout((caddr_t)icode, (caddr_t)(0), szicode))
			cmn_err(CE_PANIC, "main - copyout of icode failed");
#ifdef CMMU
		dcacheflush(0);	/* because its a write back cache */
		ccacheflush(0);	/* to invalidate old code from this page */
#endif

		return(0);	/* return to user */
	}

	if (newproc(0)) {
		u.u_procp->p_flag |= SLOAD|SSYS;
		u.u_cstime = u.u_stime = u.u_cutime = u.u_utime = 0;
		bcopy("vhand", u.u_psargs, 6);
		bcopy("vhand", u.u_comm, 5);
		vhand();	/* never returns */
	}

	if (newproc(0)) {
		u.u_procp->p_flag |= SLOAD|SSYS;
		u.u_cstime = u.u_stime = u.u_cutime = u.u_utime = 0;
		bcopy("bdflush", u.u_psargs, 8);
		bcopy("bdflush", u.u_comm, 7);
		bdflush();	/* never returns */
	}

	bcopy("sched", u.u_psargs, 6);
	bcopy("sched", u.u_comm, 5);
	sched();		/* never returns */
	/* NOT REACHED */
	return (0);
}
