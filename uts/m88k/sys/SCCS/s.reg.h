h18865
s 00073/00000/00000
d D 1.1 90/03/06 12:30:12 root 1 0
c date and time created 90/03/06 12:30:12 by root
e
u
U
t
T
I 1
#ifndef _SYS_REG_H_
#define _SYS_REG_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Location of the users' stored registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define NREGS	37

#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
#define	R8	8
#define	R9	9
#define	R10	10
#define	R11	11
#define	R12	12
#define	R13	13
#define	R14	14
#define	R15	15
#define	R16	16
#define	R17	17
#define	R18	18
#define	R19	19
#define	R20	20
#define	R21	21
#define	R22	22
#define	R23	23
#define	R24	24
#define	R25	25
#define	R26	26
#define	R27	27
#define	R28	28
#define	R29	29
#define	R30	30
#define	FP	30
#define	R31	31
#define	SP	31
#define	SR	32
#define	PS	32
#define	PC	33
#define	SCIP	33
#define	SNIP	34
#define	SFIP	35
#define	VALIDIP	2
#define	EXCEPT_IP	1
#define	SSB	36

#define	NUMTRAN	3
#define	TRAN1	37
#define	ADDR1	38
#define	DATA1	39
#define	TRAN2	40
#define	ADDR2	41
#define	DATA2	42
#define	TRAN3	43
#define	ADDR3	44
#define	DATA3	45

#endif	/* ! _SYS_REG_H_ */
E 1
