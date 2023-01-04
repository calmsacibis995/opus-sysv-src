
#define ASSEM
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/param.h"
#include "sys/page.h"
#include "sys/psw.h"

#define PSW_I		0x800
#define B_SR_MAPON	0x20000
#define	PAGESIZE	0x200
#define TWOPAGES	0x400

#define MODTAB		0x20
#define STACKOFF	(0x60000) /* sa always guaranteed this much */

	.set	NFILES,6

	.globl	start
	.globl	_edata
	.globl	_end
	.globl	__end
	.globl	_environ
	.globl	__opus
	.globl	__init
	.globl	__main
	.globl	__reinit
	.globl	_exit
	.globl	__exit
	.globl	__cleanup
	.globl	__devsw
	.globl	_close
	.globl	__dmapage
	.globl	__progpages
	.globl	_gd_boot
	.globl	__memmap

	.globl __initstr
	.globl __r
	.globl __trap
	.globl __eia
.text
zero:
	br	start
	.align	8
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0			# 0x08-0x40
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		# 0x40-0x80
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		# 0x80-0xc0
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		# 0xc0-0x100
	.space	0xf00

 # dump to tape (loc 800 hex)
 #	.align	0x200
 #	jsr	@_tpdump
 #	wait

 # restart program (808 hex)
 #	.align	0x208
 #	lprd	fp,@savefp
 #	lprd	sp,@savesp
 #	br	L0

.align	4
__r:
	.space	200
__memerr:
	.double 0

savefp:	.double	0			# save area for fp
savesp:	.double	0			# save area for sp
__nextpage: .double 0			# save area for next available page
__tlpages:  .double 0			# top level page table pointers
__tmpvad:   .double 0			# kernel TMPVAD page
__dmapage:  .double 0			# dma page
__progpages: .double 0			# program pointer tables
	.globl cerror
cerror: .double 0			# used by libc
.byte	0

start:					# start(nextpage,memmap,drive)
	movqd	$0,r0
	movqd	$0,r1
	movqd	$0,r2
	movqd	$0,r3
	movqd	$0,r4
	movqd	$0,r5
	movqd	$0,r6
	movqd	$0,r7
	movw	$N32MAG,@DMASPACE+6
	sprd	fp,@savefp 		# save fp and sp
	sprd	sp,@savesp
	movd	4(sp),@__nextpage
	movd	8(sp),@__memmap
	movd	12(sp),@argc
	movd	16(sp),@argv
	addr	@_end,r0		# find address of top level page table
	addd	$TWOPAGES-1,r0		# at first 1024 byte boundary past end
	andd	$0xfffffc00,r0
	movd	r0,@__tlpages		# save top level page table address
	addd	$PAGESIZE,r0		
	addd	$PAGESIZE,r0		
	movd	r0,@__tmpvad		# save TMPVAD page address
	addd	$PAGESIZE,r0
	movd	r0,@__dmapage		# save dma page address
	addd	$PAGESIZE,r0
	movd	r0,@__progpages		# save program pointer tables
	movd	@__nextpage,r0		# caculate end from value passed in
	muld	$PAGESIZE,r0		#  equal to base address plus
	addr	@zero,r1		#  base address
	addd	r1,r0
 	movd	r0,@__end		# set start of usable mem for sbrk

	addr	@zero,r0		# set stack to top of mapped memory
	addd	$STACKOFF,r0
	lprd	sp,r0			# move stack pointer to high memory

L0:	addr	@_edata,r1		# calculate bss size
	addr	@_end,r0
	subd	r1,r0
L7:	movqd	$0,0(r1)
	addqd	$4,r1
	acbd	$-4,r0,L7

	movd	@__end,r1		# clear sbrk and stack area
	sprd	sp,r0			#  from __end to stack pointer
	subd	r1,r0
L8:	movqd	$0,0(r1)
	addqd	$4,r1
	acbd	$-4,r0,L8
	
	addr	@_environ,tos		# environment
	cmpd	$0x12345678,8(sp)
	bne	L1
	movd	16(sp),tos
	movd	12(sp),tos
	br	L2
L1:	addr	@argv0,tos
	movqd	$1,tos
L2:	setcfg	[m,f]		# enable mmu and fpu instructions
	movqd	$0,r0
	movqd	$0,r1
	movdl	r0,f0
	movl	f0,f2
	movl	f0,f4
	movl	f0,f6

	addr	@__inttab,r0	# set up interrupts and traps for inttab
	lprd	intbase,r0

	movd	$MODTAB,r0	# setup modtab for interrupts
	addr	@zero,0(r0)
	addr	@0,4(r0)
	addr	@zero,8(r0)
	addr	@0,12(r0)
	lprw	mod,r0

	lmr	msr,$B_SR_MAPON	# allow mapping via map status register

	bispsrw	$PSW_I		# turn on interrupts
	movb	$0,@R_C_IRQ	# bring pc back to life

	jsr	@__opus		# initialize pc-related functionality
	movd	@argv,tos
	movd	@argc,tos
	jsr	@__init
	jsr	@_main		# execute main program
	jsr	@_exit
	br	L4

__exit:
L4:
	movd	$NFILES-1,r2
L5:
	movd	r2,tos			# save a copy
	movd	r2,tos
	jsr	@_close
	movd	tos,r2
	movd	tos,r2			# retrieve saved copy
	acbd	$-1,r2,L5
 #	jsr	@__reinit
	lprd	fp,@savefp 		# restore fp and sp
	lprd	sp,@savesp
	ret	$0

argc:	.double 0
argv:	.double	0
argv0:	.double	argv00
argv00:	.double	0

env00:	.double	0
_environ:
	.double	env00

__abt:	movd	$0x61,tos
	br	trapit
__fpu:	movd	$0x66,tos
	br	trapit
__ill:	movd	$0x69,tos
	br	trapit
__svc:	movd	$0x73,tos
	br	trapit
__dvz:	movd	$0x64,tos
	br	trapit
__flg:	movd	$0x67,tos
	br	trapit
__bpt:	movd	$0x62,tos
	br	trapit
__trc:	movd	$0x74,tos
	br	trapit
__und:	movd	$0x75,tos
	br	trapit
__nmi:	movd	$0x6e,tos
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
	movd	@R_STAT,tos

	enter	[r0,r1,r2,r3,r4,r5,r6,r7],$0
	jsr	@__trap		# handle trap appropriately
	exit	[r0,r1,r2,r3,r4,r5,r6,r7]
	adjspb	$-8
	movb	$0,@R_STAT	# Reset parity and dma abort
	rett	$0

restart:
	lprd	fp,@savefp
	lprd	sp,@savesp
	jsr	@__main
	br	restart

__nvi:
	movb	$0,@R_C_ACK	# Acknowledge (reset) interrupt from PC
	rett	$0

__eia:
	lmr	eia,4(sp)	# Invalidate old page at this spot
	ret	$0

.data
	.align	4

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
