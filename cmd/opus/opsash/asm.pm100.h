#define ASSEM
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"

.globl __r
.globl __trap
.globl __eia
.globl __newptb0
.globl __inttab

.text
__abt:	movd	$'a',tos
	br	trapit
__fpu:	movd	$'f',tos
	br	trapit
__ill:	movd	$'i',tos
	br	trapit
__svc:	movd	$'s',tos
	br	trapit
__dvz:	movd	$'d',tos
	br	trapit
__flg:	movd	$'g',tos
	br	trapit
__bpt:	movd	$'b',tos
	br	trapit
__trc:	movd	$'t',tos
	br	trapit
__und:	movd	$'u',tos
	br	trapit
__nmi:	movd	$'n',tos
	br	trapit

trapit:
	movd	r0,@__r
	movd	r1,@__r+4
	movd	r2,@__r+8
	movd	r3,@__r+12
	movd	r4,@__r+16
	movd	r5,@__r+20
	movd	r6,@__r+24
	movd	r7,@__r+28
	movd	4(sp),@__r+32
	sprd	sb,@__r+36
	sprd	fp,@__r+40
	sprd	sp,@__r+44
	sprd	intbase,@__r+48
	movd	8(sp),@__r+52
	smr	bpr0,@__r+56
	smr	bpr1,@__r+60
	smr	pf0,@__r+64
	smr	pf1,@__r+68
	smr	sc,@__r+72
	smr	msr,@__r+76
	smr	bcnt,@__r+80
	smr	ptb0,@__r+84
	smr	ptb1,@__r+88
	smr	eia,@__r+92
	cmpqd	$0,@__mapped
	beq	mapoff
	movd	@R_STAT,tos	/* save current status (mapped address) */
	br	mapon
mapoff:
	movd	@PR_STAT,tos	/* save current status (unmapped address) */
mapon:
	enter	[r0,r1,r2,r3,r4,r5,r6,r7],$0
	jsr	__trap		/* handle trap as appropriate */
	exit	[r0,r1,r2,r3,r4,r5,r6,r7]
	adjspb	$-8
	rett	$0		/* return to offending program */

__nvi:
	cmpqd	$0,@__mapped
	beq	physack
	movb	$0,@R_C_ACK	/* acknowledge (reset) interrupt from PC */
	rett	$0
physack:
	movb	$0,@PR_C_ACK	/* acknowledge (reset) interrupt from PC */
	rett	$0

__eia:
	lmr	eia,4(sp)	/* invalidate old page at this spot */
	ret	$0

__newptb0:
	lmr	ptb0,4(sp)	/* force use of new top level page table */
	ret	$0

__inttab:
t_nvi:	.word	MODTAB,__nvi
t_nmi:	.word	MODTAB,__nmi
t_abt:	.word	MODTAB,__abt
t_fpu:	.word	MODTAB,__fpu
t_ill:	.word	MODTAB,__ill
t_svc:	.word	MODTAB,__svc
t_dvz:	.word	MODTAB,__dvz
t_flg:	.word	MODTAB,__flg
t_bpt:	.word	MODTAB,__bpt
t_trc:	.word	MODTAB,__trc
t_und:	.word	MODTAB,__und

/*  									*/
/* 	udiv - unsigned division for ns32000 				*/
/*  									*/
/* 	arguments: dividend, divisor. 					*/
/* 	result: quotient. 						*/
/* 	uses r0-r2 							*/
/*  									*/
/* 	If 1 < divisor <= 2147483647, zero-extend the dividend 		*/
/* 	to 64 bits and let ediv do the work.  If the divisor is 1, 	*/
/* 	ediv will overflow if bit 31 of the dividend is on, so 		*/
/* 	just return the dividend unchanged.  If the divisor is 0, 	*/
/* 	do the ediv also, so it will generate the proper exception. 	*/
/* 	All other values of the divisor have bit 31 on: in this case 	*/
/* 	the quotient must be 0 if divisor > dividend, and 1 otherwise, 	*/
/* 	provided that the comparison is made as unsigned. 		*/
/* 	All the Above comments not applicable for the ns32000. 		*/
	.text
	.globl	udiv
udiv:
	movd	4(sp),r0	/*  Dividend */
	movd	8(sp),r2	/*  Divisor */
	movqd	$0,r1		/*  Clear Hi register */
	deid	r2,r0		/*  extended divide */
	movd	r1,r0		/*  quotient */
	ret	$0

/*									*/
/* 	urem - unsigned remainder for ns32000 				*/
/*									*/
/* 	arguments: dividend, divisor 					*/
/* 	result: remainder 						*/
/* 	uses r0-r2 							*/
/*  									*/
/* 	if 1 < divisor <= 2147483647, zero-extend the dividend 		*/
/* 	to 64 bits and let ediv do the work.  If the divisor is 1, 	*/
/* 	ediv will overflow if bit 31 of the dividend is on, so 		*/
/* 	just return 0.  If the divisor is 0, do the ediv also, 		*/
/* 	so it will generate the proper exception.  All other values 	*/
/* 	of the divisor have bit 31 on: in this case the remainder 	*/
/* 	must be the dividend if divisor > dividend, and the dividend 	*/
/* 	minus the divisor otherwise.  The comparison must be unsigned. 	*/
/* 	All above comments not applicable to the ns32000		*/
	.text
	.align	2
	.globl	urem
urem:
	movd	4(sp),r0	/*  Dividend */
	movd	8(sp),r2	/*  Divisor */
	movqd	$0,r1		/*  Clear Hi register */
	deid	r2,r0		/*  Remainder in r0 */
	ret	$0	
