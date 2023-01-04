#ifndef _SYS_IORB_H_
#define _SYS_IORB_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)iorb.h	1.1	*/

/*	iorb.h
 *
 *	12/09/86	initial version
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*	IORB
 *	I/O request block format
 */

#ifndef ASSEM
#if pm100
struct	io_clk {
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_ticks;
};

struct	io_conin {		/* keyboard input command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_char	;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;

};

struct	io_conout {		/* crt output command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_conctl {		/* crt control command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_hdisk {		/* hard disk io command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_fdisk {		/* floppy disk io command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_gn {		/* general driver */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
	unsigned short	io_dev;
};

struct	io_mt {		/* mag tape io command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_serin {		/* serial in command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_char;
	unsigned short	io_bcount;
};

struct	io_serout {		/* serial out command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
};

struct	io_serctl {		/* serial control command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
};

struct	io_lpt {		/* lpt command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
};

struct	io_vdi {		/* vdi command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
};

struct	io_Xin {
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_Xout {
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};

struct	io_Xctl {
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};
#endif /* pm100 */

#if pm200 || pm300 || m88k
struct iorb {
	unsigned char cmd;		/* command code */
	unsigned char flg;		/* command flag */
	unsigned char st;		/* iorb status */
	unsigned char err;		/* error return */
	unsigned short dix;		/* device index */
	unsigned short dev;		/* subdevice number */
	unsigned long dst;		/* device status */
	unsigned long buf;		/* dma address of buffer */
	unsigned long cnt;		/* byte count */
	unsigned long blk;		/* device address */
	unsigned long val;		/* value */
	unsigned long arg;		/* address of argument */
	int (*f)();			/* interrupt service routine */
	unsigned long a;		/* interrupt service argument */
	unsigned long self;		/* address of iorb */
	unsigned long extra;		/* extra iorb length */
};

#define IORB_MAGIC 0x49			/* iorb->magic */

#endif /* pm200 || pm300 || m88k */

/* common old iorbs, still used */

#define MAXIOARGS	8
struct	io_gen {		/* non-specific command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;
	unsigned short	io_args[MAXIOARGS];	
};

struct	io_dos {		/* dos io command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_ax;
	unsigned short	io_bx;
	unsigned short	io_cx;
	unsigned short	io_dx;
	unsigned short	io_si;
	unsigned short	io_di;
	unsigned short	io_ds;		/* return value only */
};

struct	io_int {		/* int io command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned short	io_ax;
	unsigned short	io_bx;
	unsigned short	io_cx;
	unsigned short	io_dx;
	unsigned short	io_si;
	unsigned short	io_di;
	unsigned short	io_intdesc; /* interrupt # or'd with interrupt flags */
};

struct	io_time {
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;	
	unsigned char	io_usec;
	unsigned char	io_sec;
	unsigned char	io_min;
	unsigned char	io_hour;
	unsigned char	io_day;
	unsigned char	io_month;
	unsigned short	io_year;
};

struct	io_sys {		/* system command block */
	char		io_index;
	char		io_cmd;
	char		io_status;
	char		io_error;
	unsigned short	io_devstat;
	unsigned short	io_memaddr;
	unsigned short	io_bcount;
	unsigned short	io_blknum;
	unsigned short	io_hblknum;
};
#endif /* ASSEM */

/*	iorb->cmd
 */
#if pm100
#define PC_NOP		0
#define PC_RST		1
#define PC_STAT		2
#define PC_READ		3
#define PC_WRITE	4
#define PC_NWREAD	5
#define PC_IOCTL	6
#endif

#if pm200 || pm300 || m88k
#define PC_NOP		0	/* nop */
#define PC_RST		1	/* reset */
#define PC_STAT		2	/* status */
#define PC_OPEN		3	/* open */
#define PC_CLOSE	4	/* close */
#define	PC_READ		5	/* read */
#define PC_WRITE	6	/* write */
#define	PC_IOCTL	7	/* i/o control */
#define PC_DVCTL	8	/* device control */
#define PC_NWREAD	9	/* */
#define	PC_SELECT	10

#define FLG_LOCAL	0x04	/* local request */
#define FLG_CALL	0x04	/*   call f(a) when done */
#define FLG_HIQ		0x05	/*   queue f(a) on hiq when done */
#define FLG_SVQ		0x06	/*   queue f(a) on svq when done */
#define FLG_LOQ		0x07	/*   queue f(a) on loq when done */
#endif /* pm200 || pm300 || m88k */

/*	iorb->st
 */
#define S_IDLE		0
#define S_GO		1
#define S_BUSY		2
#define S_DONE		4

/*	iorb->err
 */
#define E_OK		0	/* okay */
#define E_RT		1	/* okay after retry */
#define E_CE		2	/* okay after error correction */
#define E_GOOD		2	/* <= E_GOOD was ok */
#define E_CMD		3	/* illegal command */
#define E_DA	        4	/* bad address according to device */
#define E_MA		5	/* bad memory address */
#define E_NA		6	/* not available */
#define E_RO	        7	/* read only */
#define E_DE		8	/* data error */
#define E_DEV		9	/* device error */
#define E_DEVICE	E_DEV
#define E_DIX		10	/* device index */
#define E_CANCEL	11	/* cancel request */
#define E_NOIN		12	/* no input available */
#define E_DMA		13	/* dma abort during io */
#define E_PARITY	14	/* parity error during io */
#define E_CRIT		15	/* MS/DOS critical error */
#define E_TO		16	/* device timeout */
#define E_SYNC		17	/* iorb request with iorb status != GO */
#define E_BADB		18	/* flagged bad block detected */
#define E_UERROR	19	/* use devstat as unix u.u_error on return */
#define E_EOF		20	/* end of file detected */
#define E_MAP		21	/* error in mapping */

#ifdef OPMON
/*	iorb->dst for (iorb->err == E_UERROR)
 *	selected u_error codes
 *	see intro(2)
 */
#define EIO			5	/* I/O error */
#define ENXIO		6	/* no such device or address */
#define ENOMEM		12	/* not enough space */
#define EFAULT		14	/* bad address */
#define EBUSY		16	/* device or resource busy */
#define ENODEV		19	/* no such device */
#define EINVAL		22	/* invalid argument */
#define ENOSPC		28	/* no space left on device */
#endif

#endif	/* ! _SYS_IORB_H_ */
