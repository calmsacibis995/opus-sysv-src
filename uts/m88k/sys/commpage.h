#ifndef _SYS_COMMPAGE_H_
#define _SYS_COMMPAGE_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)commpage.h	1.1	*/
/*	comm.h
 *
 *	communication page structure and related values
 *
 *	12/09/86	initial version
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*	communication page
 *
 *	structure must be physically contiguous,
 *	starting at page boundary PN_COMM,
 *	with system tags ST_COMM
 */

#define COMMSIZ 248	/* bytes in comm head */
#define DIXSIZ 128	/* dix table size (multiple of 4 for alignment) */
#define DEVTABSIZE 128	/* dix table size (multiple of 4 for alignment) */
#define RQSIZ 256	/* request queue size (power of 2) */
#define CPSIZ 256	/* completion queue size (power of 2) */
#define DMAMAPSIZ 4	/* 4 4k map pages (maps 16 MB) */

#ifndef ASSEM
#if pm100
struct pc_cmd {
	unsigned char	unused1[6];
	unsigned short	pc_magic;
	unsigned short	pc_msg;
	unsigned short	pc_dix;
	unsigned char	pc_kflag;
	unsigned char	pc_kchar;
	unsigned char	pc_sflag;
	unsigned char	pc_schar;
	unsigned char	pc_rqflg;
	unsigned char	unused2;
	unsigned short	pc_rqblk;
	unsigned char	pc_cpflg;
	unsigned char	unused3;
	unsigned short	pc_cpblk;
	unsigned char	pc_syflg;
	unsigned char	pc_ckflg;
	unsigned short	pc_syblk;
	unsigned long	pc_ckcnt;
	unsigned long	mod_tab[4];
	unsigned char	deadzone[16];
	unsigned char	pc_devtab[DEVTABSIZE];
	struct io_gen	cmd_buf[1];
};
#endif /* pm100 */

#if pm200 || pm300 || m88k
volatile struct commpage {
	char x0[16];				/* space for startup code */
	unsigned short magic;		/* magic number for startup */
	unsigned short dixsiz;		/* size of dix array */
	unsigned long map;			/* physical map location */
	unsigned long map_size;		/* physical map size in bytes */
	unsigned long ckcnt;		/* clock tick count */
	unsigned char kflag, kchar;	/* secondary keyboard channel */
	unsigned char sflag, schar;	/* secondary display channel */
	unsigned char ckflg;		/* clock handshake */
	unsigned char tstlvl;		/* initial test level */
	unsigned char syflg;
	unsigned char nmiflg;		/* m88k nmi state */
	unsigned long syblk;		/* dvrsys requests */
	unsigned short rqget, rqput, rqmsk;	/* rqq control */
	unsigned short cpget, cpput, cpmsk;	/* cpq control */
	unsigned long mem_size;		/* physical memory size in bytes */
	char cmdline[128];		/* dos command line */
	unsigned short type;		/* board type */
	unsigned short dkra;		/* disk read ahead buffer */
	unsigned long cfgval[4];	/* config register values */
	unsigned long debug[8];		/* misc debug area */
	unsigned long splret;		/* last caller of spl */
	unsigned char spl;		/* current processor spl */
	char x1[3];			/* fill */
	char arena[4096 - COMMSIZ - DIXSIZ - RQSIZ*4 - CPSIZ*4];/* fill */
	unsigned char dix[DIXSIZ];	/* device index table */
	unsigned long rqq[RQSIZ];	/* request queue */
	unsigned long cpq[CPSIZ];	/* completion queue */
};

struct commcell
{
	unsigned char name[4];
	unsigned long *next;
	/* data */
};


#if pm200
extern	char	*SR0, *SR1, *SR2, *CNTRL0;
#define COMMPAGE ((struct commpage *)(0))
#endif
#if m88k
#define	SR0	(volatile unsigned char *) 0x5fffff20
#define	SR1	(volatile unsigned char *) 0x5fffff24
#define	SR2	(volatile unsigned char *) 0x5fffff28
#define	SR3	(volatile unsigned char *) 0x5fffff2C
#define	CNTRL0	(volatile unsigned char *) 0x5fffff30
#define	HWREGADDR (0x5fffff00)
#define COMMPAGE0 ((volatile struct commpage *)(0))	/* R/O Commpage */
#define COMMPAGE ((volatile struct commpage *)(DMASPACE))/* R/W Commpage */
#endif
#if pm300
#define COMMPAGE ((struct commpage *)(0x2000))

extern unsigned dmatags[];
#define mkoppde(pg) \
	(int)((dmapte[(pg)].pgi.pg_pde & PG_ADDR) | dmatags[dmapte[(pg)].pgm.pg_tag]);
#define mkopaddr(pg) \
	&((int *)(COMMPAGE->map & SDMA_MASK))[(pg)]
#endif

#endif /* pm200 || pm300 || m88k */
#endif /* ASSEM */

/*
 *	Kernel Virtual Address of dmaspace 
 */
#if pm200 || pm300 || m88k
#define	DMASPACE	0xD0000000
#define DMASEG		832		/* svtos(DMASPACE) */
#endif
#if pm100
#define DMASPACE	0xff0000
#define PCCMD		((struct pc_cmd *)DMASPACE)
#endif
/*
** magic definitions
**
*/
#define	MAGIC_PM100		0x5aa5
#define N32MAG			MAGIC_PM100
#define	MAGIC_PM200		0x0332
#define	MAGIC_PM200x		0xf332
#define	MAGIC_PM300x		0x1234
#define	MAGIC_PM300		0x4321
#define	MAGIC_PM88K		0xa55a


/*	device types
 *	dt->type and dix table entries
 */

#define CLOCK		1
#define CONIN		2
#define CONOUT		3
#define DISK		4
#define	FLPDISK		5
#define SERIN		6
#define SEROUT		7
#define DOS		8
#define LPT		9
#define SERCTL		10
#define CONCTL		11
#define VDI		12
#define	PCNET		13
#define GEN		14
#define TAPE		15
#define ETHER		16
#define	RES1		17	/* reserved for customer drivers */
#define RES2		18	/* reserved for customer drivers */
#define RES3		19	/* reserved for customer drivers */
#define XIN		20	/* X10.4 */
#define XCTL		21	/* X10.4 */
#define GENF		22
#define	EC		23
#define RES4		24	/* reserved for customer drivers */
#define RES5		25	/* reserved for customer drivers */
#define RES6		26	/* reserved for customer drivers */
#define	ASIO		27

#define DEVTYPES	28

#endif	/* ! _SYS_COMMPAGE_H_ */
