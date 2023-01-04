h10371
s 00060/00000/00000
d D 1.1 90/03/06 12:29:59 root 1 0
c date and time created 90/03/06 12:29:59 by root
e
u
U
t
T
I 1
#ifndef _SYS_MTCOMM_H_
#define _SYS_MTCOMM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	SID @(#)mtcomm.h	1.4	*/

/*
 * Definitions for communication with the PC TAPE drivers
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* field definitions for unix minor device */

#define UNITMSK		3	/* mask for unit field */
#define REWMSK		4	/* mask for rewind/no rewind field */
#define DENMSK		8	/* mask for density select field */

/* field definitions for the status word returned from a PC_STAT call */

#define STATUNIT	3	/* mask for unit field */
#define STATCART	4	/* mask for drive type */

/* field definitions for status returned from a device error or MTIOCTL_NOP */

#define	S_WPROT		0x1	/* write protect */
#define	S_OFFLINE	0x2	/* offline */
#define	S_BOT		0x4	/* beginning of tape */
#define	S_EOT		0x8	/* end of tape */
#define	S_EOF		0x10	/* end of file */
#define	S_ERR		0x20	/* error */
#define S_ILL		0x40	/* illegal command */
#define	S_TO		0x80	/* time out */

/* ioctl commands */

#define MTIOCTL_NOP	0	/* no operation, returns status */
#define MTIOCTL_RESET	1	/* reset the tape controller */
#define	MTIOCTL_REW	2	/* rewind */
#define	MTIOCTL_TENS	3	/* retension tape */
#define	MTIOCTL_WFM	4	/* write file mark */
#define	MTIOCTL_FSF	5	/* forward space past file mark */
#define MTIOCTL_ERASE	6	/* erase tape */
#define MTIOCTL_BSF	7	/* back space past file mark */
#define MTIOCTL_DEN	8	/* select density */
#define	MTIOCTL_FSR	9	/* forward space past record */
#define MTIOCTL_BSR	10	/* back space past record */
#define MTIOCTL_UNL	11	/* unload tape 9-track only */

/* densities */

#define	BPI800		0	/* 800 bpi */
#define	BPI1600		1	/* 1600 bpi */

#endif	/* ! _SYS_MTCOMM_H_ */
E 1
