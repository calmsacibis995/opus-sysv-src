h45617
s 00131/00000/00000
d D 1.1 90/03/06 12:29:35 root 1 0
c date and time created 90/03/06 12:29:35 by root
e
u
U
t
T
I 1
#ifndef _SYS_DOSCOMM_H_
#define _SYS_DOSCOMM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	%Q% %W%	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Definitions for communication with the PC DOS driver.
 */

/* ioctl commands */

#define DOSCMD		1
#define SYSCMD		2
#define INTCMD		3

/* data structure for DOSCALL ioctl */

struct	dos_ctrl {
	struct	io_gen *dptr;  /* dos command request block */
	int	reloc;  /* regs to be relocated */
	char	*buf;  /* data buffer for dos command */
	int	len;  /* length of 'buf' */
};

/* dos_ctrl.reloc bit definitions */

#define RELOCAX 0x01  /* relocate ax register */
#define RELOCBX 0x02  /* relocate bx register */
#define RELOCCX 0x04  /* relocate cx register */
#define RELOCDX 0x08  /* relocate dx register */
#define RELOCSI 0x10  /* relocate si register */
#define RELOCDI 0x20  /* relocate di register */
#define RELOCDS 0x40  /* relocate ds register */


/* data structure for INTCALL ioctl */

struct	int_ctrl {
	struct	io_gen *dptr;  /* dos command request block */
	int	reloc;  /* regs to be relocated */
	char	*buf;  /* data buffer for dos command */
	int	len;  /* length of 'buf' */
};

#define RD_INT		0x0100		/* interrupt needs a read transfer */
#define WR_INT		0x0200		/* interrupt needs a write transfer */
#define NOWAIT_INT	0x4000		/* post done before int call */
#define NODOS_INT 	0x8000		/* target interrupt will not call DOS */

/* data structure for SYSCMD ioctl */

struct	sys_ctrl {
	struct	io_gen *dptr;  /* sys command request block */
	char	*buf;  /* data buffer for dos command */
	int	len;  /* length of 'buf' */
};

/* MS/DOS commands */

#define DOS_CREAT	0x3c
#define DOS_OPEN	0x3d
#define DOS_CLOSE	0x3e
#define DOS_READ	0x3f
#define DOS_WRITE	0x40
#define DOS_LSEEK	0x42

/* DOS file attributes
 */
#define ATTR_READONLY	0x01
#define ATTR_HIDDEN	0x02
#define ATTR_SYSTEM	0x04
#define ATTR_LABEL	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20

/* structure for find first/next matching file
 */
struct dos_find {
	char path[256];		/* wild-card pathname */
	char fill[2];		/* make size aligned */
	char dta[21];		/* reserved for DOS */
	char attr;		/* input to findfirst; out for first/next */
	short time;		/* binary encoded modify time */
	short date;		/* binary encoded modify date */
	long size;		/* file size in bytes */
	char name[13];		/* file name ("name.exe\0") */
};

/* system commands */

#define PC_SNOP		0	/* nop */
#define PC_DOSRTN	1	/* return to MS/DOS */
#define PC_SSTAT	2	/* system status */
#define PC_PAUSE	3	/* pause for MS/DOS */
#define PC_CANCEL	4	/* cancel specific request */
#define PC_REINIT	5	/* reinitialize device table */
#define PC_CMDLINE	6	/* read initial command line */
#define PC_CMDCHAR	7	/* read cmd char */
#define PC_SYNC		8	/* flush disk buffers in pc memory */
#define PC_DEVS		9	/* fetch root and swap dix */
#define PC_TOD		10	/* access PC tod clock (obsolete) */
#define PC_EXEC		11	/* lattice c fork */
#define PC_READIO	12	/* read a PC i/o byte */
#define PC_WRITEIO	13	/* write a PC i/o byte */
#define PC_READMEM	14	/* read PC memory */
#define PC_WRITEMEM	15	/* write PC memory */
#define PC_GTOD		16	/* get PC time of day */
#define PC_STOD		17	/* set PC time of day */
#define PC_OPVER	18	/* get opmon version */
#define PC_GETENV	19	/* get DOS environment variable */
#define PC_PUTENV	20	/* put DOS environment variable */
#define	PC_PRFSTART	21	/* start profiling */
#define	PC_PRFSTOP	22	/* stop profiling */
#define	PC_PRFSAVE	23	/* save profiling */
#define PC_READIOW	24	/* read a PC i/o word */
#define PC_WRITEIOW	25	/* write a PC i/o word */

#define PC_READIOBS	128	/* read a PC i/o byte string */
#define PC_READIOWS	129	/* read a PC i/o word string */
#define PC_WRITEIOBS	130	/* write a PC i/o byte string */
#define PC_WRITEIOWS	131	/* write a PC i/o word string */

#endif	/* ! _SYS_DOSCOMM_H_ */
E 1
