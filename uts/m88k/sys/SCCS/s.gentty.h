h11674
s 00032/00000/00000
d D 1.1 90/03/06 12:29:43 root 1 0
c date and time created 90/03/06 12:29:43 by root
e
u
U
t
T
I 1
#ifndef _SYS_GENTTY_H_
#define _SYS_GENTTY_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* dual console related ioctl calls */
#define CONCURRENT	('C'<<8)
#define CONC		(CONCURRENT)
#define CONC_ON		(CONCURRENT|1)  /* turn on dual console mode */
#define CONC_OFF	(CONCURRENT|2)  /* turn off dual console mode */
#define CONC_BOOT	(CONCURRENT|3)	/* system booted in dual console mode,
					   maintain the port settings */
#define CONC_DEBUG	(CONCURRENT|4)
#define CONC_PUTC	(CONCURRENT|5)  /* putc command */
#define CONC_IPUTC	(CONCURRENT|6)  /* inputc command */

#define ON 1
#define OFF 0

/* dual console receive status */
#define DC_BREAK  0x01		/* break */
#define DC_FRERR  0x02		/* frame error */
#define DC_PARERR 0x04		/* parity error */
#define DC_ROVRN  0x08		/* receiver overrun */

#endif	/* ! _SYS_GENTTY_H_ */
E 1
