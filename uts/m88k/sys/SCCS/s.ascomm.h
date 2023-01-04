h61957
s 00047/00000/00000
d D 1.1 90/03/06 12:29:25 root 1 0
c date and time created 90/03/06 12:29:25 by root
e
u
U
t
T
I 1
#ifndef _SYS_ASCOMM_H_
#define _SYS_ASCOMM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* async ioctl cmds */

#define ASCTL_NOP	0	/* nop */
#define	ASCTL_SUSPENDIN	1	/* suspend character input from pc */
#define ASCTL_RESUMEIN	2	/* resume character input from pc */
#define ASCTL_DRAINOUT	3	/* drain output buffer, return when done */
#define	ASCTL_BREAK	4	/* send break */
#define	ASCTL_MODEM	5	/* set modem control bits */
#define	ASCTL_FLUSHIN	6	/* flush input buffer in pc */
#define	ASCTL_FLUSHOUT	7	/* flush output buffer in pc */
#define ASCTL_IFLAG	8	/* send iflags to pc */
#define ASCTL_CFLAG	9	/* send cflags to pc

/* modem control bits for ASCTL_MODEM */

#define	MODEM_DTR	1	/* dtr control */
#define MODEM_RTS	2	/* rts control */

/* async status fields */

#define	ASTAT_BREAK	0x0001	/* incoming break detected */
#define	ASTAT_PARITYERR	0x0002	/* parity error */
#define	ASTAT_FRAMEERR	0x0004	/* framing error */
#define	ASTAT_HOV	0x0008	/* hardware overrun */
#define	ASTAT_SOV	0x0010	/* software overrun */
#define	ASTAT_MODEM	0x0080	/* modem status line change */
#define	ASTAT_DTR	0x0100	/* current setting of data terminal ready */
#define	ASTAT_RTS	0x0200	/* current setting of request to send */
#define	ASTAT_DSR	0x1000	/* data set ready status */
#define	ASTAT_CTS	0x2000	/* clear to send status */
#define	ASTAT_CD	0x4000	/* carrier detect status */
#define	ASTAT_RI	0x8000	/* ring indicator status */



#endif	/* ! _SYS_ASCOMM_H_ */
E 1
