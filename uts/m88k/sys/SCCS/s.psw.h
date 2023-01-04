h13280
s 00057/00000/00000
d D 1.1 90/03/06 12:30:09 root 1 0
c date and time created 90/03/06 12:30:09 by root
e
u
U
t
T
I 1
#ifndef _SYS_PSW_H_
#define _SYS_PSW_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 *  	processor status register for 88000
 */

#define	PS_EXD	0x1		/* Exception disable */
#define	PS_IPL	0x2		/* Interrupt level/disable */
#define	PS_MXM	0x4		/* Misaligned exception mask */
#define	PS_DEXC	0x8000000	/* Data exception pending.*/
#define	PS_C	0x10000000	/* carry bit */
#define	PS_SER	0x20000000	/* serial mode bit */
#define	PS_BO	0x40000000	/* byte order bit */
#define	PS_CUR	0x80000000	/* current mode. Supervisor if set */
#define	PS_SUP	0x80000000	/* current mode. Supervisor if set */


/*
 * Special Status shadow registers containing fault information
 */

#define	TRN_MD	(1<<14)		/* 1 = supervisor */
#define	TRN_DB	(1<<13)		/* 1 = double load*/
#define	TRN_LK	(1<<12)		/* 1 = locked bus (from xmem) */
#define	TRN_REG	(0x1f<<7)	/* register destination*/
#define	TRN_SIGN (1<<6)		/* Signed/unsigned load */
#define	TRN_B3	(1<<5)		/* 1 Upper byte */
#define	TRN_B2	(1<<4)		/* 1  byte */
#define	TRN_B1	(1<<3)		/* 1  byte */
#define	TRN_B0	(1<<2)		/* 1 Lower byte */
#define	TRN_RW	(1<<1)		/* Read bit. 0= read */
#define	TRN_VALID	(1)	/* Valid transaction*/
#define	STROBES	(TRN_B0 | TRN_B1 | TRN_B2 |TRN_B3)
#define	TRN_WORD STROBES	/* Entire word */
#define	UPHALF	(TRN_B2 |TRN_B3)  /* Upper half. */
#define	LOHALF	(TRN_B0 |TRN_B1)  /* lower half. */
#define	LOBYTE	(TRN_B0)	/* lower byte. byte 0. */
#define	MLBYTE	(TRN_B1)	/* byte 1.*/
#define	MHBYTE	(TRN_B2)	/* byte 2. */
#define	HIBYTE	(TRN_B3)	/* most significant byte */

struct fltinfo {		/* structure of saved fault information */
	int	tran;		/* flags as defined above */
	char	*addr;		/* word address of transaction */
	int	data;		/* data for store. */
};

#endif	/* ! _SYS_PSW_H_ */
E 1
