h32487
s 00000/00004/00178
d D 1.2 90/05/05 06:42:40 root 2 1
c removed mototola references
e
s 00182/00000/00000
d D 1.1 90/03/06 12:29:37 root 1 0
c date and time created 90/03/06 12:29:37 by root
e
u
U
t
T
I 1
#ifndef _SYS_EREC_H_
#define _SYS_EREC_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

D 2
/*		Copyright (c) 1986,1987 Motorola		*/
/*		All Rights Reserved				*/
/*		@(#)erec.h	10.3		*/

E 2
#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Every error record has a header as follows.
 */

struct errhdr {
	short	e_type;		/* record type */
	short	e_len;		/* bytes in record (with header) */
	time_t	e_time;		/* time of day */
};

/*
 * Error record types
 */

#define E_GOTS	010		/* Start for UNIX/TS */
#define E_GORT	011		/* Start for UNIX/RT */
#define E_STOP	012		/* Stop */
#define E_TCHG	013		/* Time change */
#define E_CCHG	014		/* Configuration change */
#define E_BLK	020		/* Block device error */
#define E_STRAY	030		/* Stray interrupt */
#define E_PRTY	031		/* Memory parity */

/*
 * Error logging startup record. One of these is
 * sent to the logging daemon when logging is
 * first activated.
 */


struct estart {
	long	e_cpu;		/* cpu type */
	struct  utsname e_name;	/* system names */
	short	e_mmr3;		/* contents of mem mgmt reg 3 (11/70 and 11/45) */
	long	e_syssize;	/* system memory size (11/70 only) */
	short	e_bconf;	/* block device configuration */
};

/*
 * Error logging termination record that is sent to the daemon
 * when it stops error logging.
 */

#define eend errhdr

/*
 * A time change record is sent to the daemon whenever
 * the system's time of day is changed.
 */

struct etimchg {
	time_t	e_ntime;	/* new time */
};

/*
 * A configuration change message is sent to
 * the error logging daemon whenever a block device driver
 * is attached or detached (MERT only).
 */

struct econfchg {
	char	e_trudev;	/* "true" major device number */
	char	e_cflag;	/* driver attached or detached */
};

#define E_ATCH	1
#define E_DTCH	0


#if defined(m68k) || defined(m88k)
/*
 * Struct for Disk-specific errors.
*/

#define E_DIOCTL	02		/* General disk ioctl cmd */
#define	E_DCHGSZ	04		/* Chg disk sector size */

struct edisk {
	short	ed_request;		/* Non-IO Disk request (see above) */
	char	ed_type;		/* Disk/cntroller error type */
	char	ed_code;		/* Disk/cntroller error code */
};

#endif /* m68k */
/*
 * Template for the error record that is logged by block devices.
 */

struct eblock {
	dev_t	e_dev;		/* "true" major + minor dev number */
	physadr	e_regloc;	/* controller address */
	short	e_bacty;	/* other block I/O activity */
	struct iostat e_stats;	/* unit I/O statistics */
	short	e_bflags;	/* read/write, error, etc */
#if defined(m68k) || defined(m88k)
	long	e_cyloff;	/* logical dev start cyl */
#else
	short	e_cyloff;	/* logical dev start cyl */
#endif
	daddr_t	e_bnum;		/* logical block number */
	unsigned e_bytes;	/* number of bytes to transfer */
	paddr_t	e_memadd;	/* buffer memory address */
	ushort e_rtry;		/* number of retries */
	short	e_nreg;		/* number of device registers */
#ifdef vax
	struct	mba_regs e_mba;	/* mass bus adaptor registers */
#endif
#if defined(m68k) || defined(m88k)
	daddr_t	e_baseblk;	/* logical dev start block */
	struct  edisk e_disk;	/* Disk-specific info */
#endif /* m68k */
};

/*
 * Flags (selected subset of flags in buffer header)
 */

#define E_WRITE	0
#define E_READ	1
#define E_NOIO	02		/* May imply E_DCLOSE or E_DFMT */
#define E_PHYS	04
#if defined(m68k) || defined(m88k)
#define E_FORMAT 010
#else
#define E_MAP	010
#endif
#define E_ERROR	020

/*
 * Template for the stray interrupt record that is logged
 * every time an unexpected interrupt occurs.
 */

struct estray {
	physadr	e_saddr;	/* stray loc or device addr */
	short	e_sbacty;	/* active block devices */
#if defined(m68k) || defined(m88k)
	int	e_vector;	/* 0 - 255 */
#endif /* m68k */
};

/*
 * Memory parity record that is logged whenever one
 * of those things is detected (status regs scanned periodically).
 */

#if defined(m68k) || defined(m88k)
struct ememory {
	short	e_memtyp;	/* corrected=0; uncorrectable=1 */
	char	*e_memcsr;	/* memory board csr address */
	int	e_memsyn;	/* memory board syndrome bits */
};

#define E_SBME 0		/* single-bit memory error (corrected) */
#define E_UME  1		/* uncorrectable memory error */
struct eparity {
	short	e_parreg[4];	/* memory subsystem registers */
};
#ifdef vax
struct ememory {
	int	e_sbier;
	int	e_memcad;
};
#endif /* vax */
#endif /* m68k */

#endif	/* ! _SYS_EREC_H_ */
E 1
