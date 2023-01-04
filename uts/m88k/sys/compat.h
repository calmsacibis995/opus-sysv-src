#ifndef _SYS_COMPAT_H_
#define _SYS_COMPAT_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)compat.h	1.3	*/
/* compatiblity file for Unix rel 5.2.2 and 5.3 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#if SVR3

#if pm100 || pm200
# define SPTALLOC(size) sptalloc((size),  PG_ALL, 0, NOSLEEP)
#endif
#if pm300
# define SPTALLOC(size) sptalloc((size),  PM_RWE, 0, NOSLEEP)
#endif
#if m88k
# define SPTALLOC(size) sptalloc((size),  1, 0)
#endif
# define BTOC	btoc
# define CTOB	ctob
# define PADDR	paddr_t
# define minor_t	unsigned char

#else

# define SPTALLOC(size) sptalloc((size),  PG_V|PG_KW, 0)
# define BTOC	btop
# define CTOB	ptob
# define PADDR	physadr
# define minor_t	int

#endif
#if pm100
#define	SETATINTR() *(char *)R_C_IRQ = 1
#define	CLRATINTR() *(char *)R_C_IRQ = 0
#define SCOPE_SYNC() {*(char *)(R_WAIT+4)=1;*(char *)(R_WAIT+4)=0;}
#define	DIXLOOP(i)	for (i = 1; i <= PCCMD->pc_devtab[0]; i++)
#define	DIX(i)		PCCMD->pc_devtab[i]
#define	IBUF		io_memaddr
#define	IVAL		io_memaddr
#define	IOCTL		PC_IOCTL
#define	IORB_MAGIC	E_OK
#endif
#if pm200 || m88k
#define	SETATINTR() *(char *)CNTRL0 = INT_AT|SET
/*#define	CLRATINTR() *(char *)CNTRL0 = INT_AT|CLR*/
#define SCOPE_SYNC() {*CNTRL0 = WIDG_CK|SET; *CNTRL0 = WIDG_CK|CLR;}

#define	io_index	dix
#define	io_devstat	dst
#define io_cmd		cmd
#define	io_error	err
#define	io_bcount	cnt
#define	io_status	st
#define	io_blknum	blk
#define	io_dev		dev
#define io_inuse	st
#define io_klp		val

#define	io_serin	iorb
#define	io_serout	iorb
#define	io_serctl	iorb
#define	io_conin	iorb
#define	io_conout	iorb
#define	io_conctl	iorb
#define	io_hdisk	iorb
#define	io_gn		iorb
#define	io_lpt		iorb
#define	io_mt		iorb
#define	io_Xin		iorb
#define	io_Xout		iorb
#define	io_Xctl		iorb
#define io_eth		iorb

#define	DIXLOOP(i)	for (i = 0; i < COMMPAGE->dixsiz; i++)
#define	DIX(i)		COMMPAGE->dix[i]
#define	IBUF		buf
#define	IVAL		val
#define	IOCTL		PC_DVCTL
#endif

#endif	/* ! _SYS_COMPAT_H_ */
