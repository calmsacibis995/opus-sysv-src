h31005
s 00141/00000/00000
d D 1.1 90/03/06 12:29:31 root 1 0
c date and time created 90/03/06 12:29:31 by root
e
u
U
t
T
I 1
#ifndef _SYS_CP_H_
#define _SYS_CP_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)cp.h	1.1	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#if pm100
#define MODTAB		0x20
#define R_WAIT          0xfff000        /* I/O reg for wait */
#define R_STAT          0xfff200        /* I/O reg for status, dma reset */
#define R_C_ACK         0xfff400        /* I/O reg for resetting pc int */
#define R_C_IRQ         0xfff600        /* I/O reg for interrupting PC */
#define R_B_DMA		0x10		/* dma err bit */
#define R_B_PAR		0x20		/* parity err bit */
#define TMPVAD          0xfe0000        /* TMPVAD for kernel mapping */
#define KERNELBASE	0xf00000	/* base of mapped kernel */
#endif

#if pm200 || pm300 || m88k
/*	CR0 values
 */
#define CR0_NOP		0x00	/* nop */
#define NMI_CP		0x01	/* nmi to milo */
#define RST_CP		0x02	/* reset milo */
#define GO_CP		0x03	/* release milo reset */
#define CLR_CP		0x04	/* clear milo errors */
#define ENB_AT		0x05	/* enable interrupt to AT */
#define CLR_AT		0x06	/* clear AT errors */
#define DSB_AT		0x07	/* disable interrupt to AT */
#define DMA_AT		0x08	/* start DMA transfer */

#define SET		1
#define ACK		0
#define CLR		0

#define INT_CP		0x10	/* interrupt milo */
#define INT_AT		0x12	/* interrupt AT */
#define F_CP		0x14	/* flag milo */
#define F_AT		0x16	/* flag AT */
#define WIDG_CS		0x18	/* chip select */
#define WIDG_CK		0x1a	/* clock */
#define WIDG_WD		0x1c	/* write data */
#define PUD_CP		0x1e	/* power-up diagnostic */
#define X_ENB		0x1e	/* XBus interrupt enable/disable */

/*	SR0 bits
 */
#define CP_GE		0x80	/* general error */
#define CP_INT		0x40	/* interrupt pending to milo */
#define CP_F		0x20	/* milo flag set */
#define CP_RST		0x10	/* milo reset (not running) */
#define AT_TE		0x08	/* AT transfer error */
#define AT_INT		0x04	/* interrupt pending to AT */
#define AT_F		0x02	/* AT flag set */
#define WIDG_RD		0x01	/* read data */

/*	SR1 bits
 */
#define PE_B3		0x80	/* parity error - byte 3 */
#define PE_B2		0x40	/* parity error - byte 2 */
#define PE_B1		0x20	/* parity error - byte 1 */
#define PE_B0		0x10	/* parity error - byte 0 */
#define PE_BANK		0x0c	/* parity error - bank 0-3 */
#define CP_TYPE		0x03	/* coprocessor type 0-3 */

#define CP_PM100	0xff	/* coprocessor type ns-32000 */
#define CP_PM300	0x00	/* coprocessor type clipper */
#define CP_PM200	0x01	/* coprocessor type binkley */

/*	SR2 bits
 */
#define CP_PUD		0x80	/* power-up diagnostic status */
#define CP_URF		0x80	/* unrecoverable fault status */
#define CP_BE		0x40	/* bus error */
#define CP_PE		0x20	/* parity error on milo access */
				/* spare */
#define CP_TO		0x08	/* coprocessor timeout */
#define AT_DMA		0x04	/* DMA in progress */
#define AT_TO		0x02	/* AT timeout error */
#define AT_PE		0x01	/* parity error on AT access */

/*	SR3 bits
*/
#define CP_TYPEX	0x0f	/* type extension field */
#define CP_XIRQ		0x10	/* XBus interrupt request */

#endif /* pm200 || pm300 || m88k */

#if pm300 
/*	System Tags
 */
#define ST_PWT  0	/* private write-through */
#define ST_SWT  1	/* shared write-through */
#define ST_PCB  2	/* private copy-back */
#define ST_NC   3	/* noncacheable */
#define ST_IO   4	/* I/O space */
#define ST_BOOT 5	/* boot ROM */
#define ST_PUR  6	/* cache purge */
#define ST_SLV	7	/* slave I/O */

#define PN_COMM 2			/* comm page physical page number */
#define ST_COMM ST_NC		/* comm page system tags */

/*  hardware page
 */
#define PN_HARD 0x5ffff
#define ST_HARD ST_NC

#ifndef ASSEM
struct hardpage {
	char x0[0xff0];
	unsigned short Data;
	short x1;
	unsigned long Addr;
	unsigned char sr0;
	unsigned char sr1;
	unsigned char sr2;
	char x2;
	unsigned char cr0;
	char x3[3];
};
#endif

/*  AT Bus Master spaces
 */
#define PN_ATIO  0x60000
#define ST_ATIO  ST_NC

#define PN_ATMEM 0x80000
#define ST_ATMEM ST_NC

#endif /* pm300 */

#endif	/* ! _SYS_CP_H_ */
E 1
