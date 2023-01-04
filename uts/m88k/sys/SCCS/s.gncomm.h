h52380
s 00045/00000/00000
d D 1.1 90/03/06 12:29:44 root 1 0
c date and time created 90/03/06 12:29:44 by root
e
u
U
t
T
I 1
#ifndef _SYS_GNCOMM_H_
#define _SYS_GNCOMM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	SID @(#)gncomm.h	1.8	*/

/*
 * Definitions for communication with PC gen type drivers
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* field definitions for unix minor device field */

#define UNITSHIFT 4  /* base of unit field */
#define SUBUNITMASK 0xf  /* mask for subunit field */

/* pre defined ioctl cmds */

#define IOCTL_OPEN 0xfffe  /* open call for a unit */
#define IOCTL_CLOSE 0xffff  /* close call for a unit */
#define	IOCTL_PBUF 0xfffd /* address of physical buffer */

/* field definitions for the ioctl cmd word */

#define IOCTLCMDMASK 0xffff  /* mask for ioctl command codes */
#define IOCTLCMDSHIFT 0  /* start of ioctl command code field */
#define IOCTLLENMASK 0xffff  /* mask for ioctl data length */
#define IOCTLLENSHIFT 16  /* start of ioctl data length field */

/* field definitions for the status word returned from a PC_STAT call */

#define STATUMASK	0xf  /* mask for unit part of status */
#define STATLOGERRS	0x10  /* errors are to be logged by unix */
#define STATBLKTYPE	0x20  /* allow block device access */
#define STATBREAKUP	0x40  /* can break up a request */
#define	STATSELECT	0x80  /* driver support bsd select */
#define STATPBUF	0x100 /* allocate a physical buffer in DMA VA*/

#endif	/* ! _SYS_GNCOMM_H_ */
E 1
