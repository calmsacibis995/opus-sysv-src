#define ASSEM
#include "stand.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/psw.h"
#include "sys/page.h"

#define NFILES	4
#if pm100
#define PAGESIZE 512
#endif

.globl __main
.globl __opus
.globl __initstr
.globl __memtest
.globl __memerr
.globl _edata
.globl _end
.globl __end
.globl __exit
.globl __memsize
.globl __mapmemtest
.globl __memmap
.globl __getpaddr
.globl __ptrtab
.globl __dmaslot
.globl __zero
.globl __jptoit
.globl __mapped
.globl _argc
.globl _environ

#ifndef RESTART
#define RESTART		start
#endif
#define STACKPTR 	0xe800
#define FRAMEPTR	0x0
#define PSW_I		0x800

.globl __mode
.text
__zero:
	jump	start

.align	0x6
magic:	.word	0x0		
__msg:	.word	__initstr,0,0,0

.double 0,0,0,0,0,0,0,0,0,0,0,0		 	# 0x10-0x40
.double 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0	 	# 0x40-0x80
.double 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0	 	# 0x80-0xc0
.double 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  	# 0xc0-0x100
.space	0xf00

__memerr:
	.double	0
savesp:	.double 0
savefp:	.double 0
_argc:	.double 0
env00:	.double	0
_environ:
	.double	env00

__ptrtab:	.double 0
__kptrtab:	.double 0
__monmem:	.double 0
__tlpagetab:	.double 0
__regpage:	.double 0
__lastbank:	.double 0
__parerrs:	.double 0

start:
	movw	$N32MAG,@magic	# magic number signifies start
	lprd	fp,$FRAMEPTR	# set up null frame pointer
	lprd	sp,$STACKPTR	# set stack pointer to end of visible mem
	setcfg	[m,f]		# enable mmu and fpu instructions

	addr	@__inttab,r0	# set up interrupts and traps for inttab
	lprd	intbase,r0

	addr	@MODTAB,r0	# setup modtab for interrupts
	addr	@__zero,0(r0)
	addr	@0,4(r0)
	addr	@__zero,8(r0)
	addr	@0,12(r0)
	lprw	mod,r0
	movqw	$0,@__mapped

	addr	@_end,@__end	# end used by sbrk
	addr	@_edata,r0	# clear memory past data
clrbss:	movb	$0,0(r0)
	addqd	$1,r0
	cmpd	r0,$DMASIZE
	blt	clrbss

	jsr	@__memsize	# size memory (sets __memmap, __maxmem)
	jsr	@__mapmemtest	# test memory (modifies __memmap, __maxmem)
	movd	r0,@__memerr	# keep track of memory errors

findtop:
	movd	$NUMCHUNK,r0	# calculate highest physical memory
nomemyet:
	addqd	$-1,r0
	movd	r0,@__lastbank	# save in case of error to make invalid
	tbitd	r0,@__memmap	# bit set in memmap means valid bank
	bfc	nomemyet
	addqd	$1,r0	
	muld	$MEMINCR,r0	# multiply by banksize to get location
	
	movd	$KPTRTAB,@__kptrtab
	addd	r0,@__kptrtab
	movd	$PTRTAB,@__ptrtab
	addd	r0,@__ptrtab
	movd	$MONMEM,@__monmem
	addd	r0,@__monmem
	movd	$TLPAGETAB,@__tlpagetab
	addd	r0,@__tlpagetab
	movd	$REGPAGE,@__regpage
	addd	r0,@__regpage

	movd	@__tlpagetab,r1	# clear top level page table
	movd	r1,r2
	addd	$1024,r2	# number of top level entries in 16MB
tlpgzero:
	movqd	$0,0(r1)
	addqd	$4,r1
	cmpd	r1,r2
	blt	tlpgzero

	movd	@__ptrtab,r0	# set up top level page maps 
	addqd	$3,r0		# (set valid, write bits)
	movd	@__tlpagetab,r1	#  to point to pointer tables for 4MB
	movd	r1,r2
	addd	$256,r2		# number of top level entries in 4MB
tlpgloop:
	movd	r0,0(r1)
	addd	$PAGESIZE,r0
	addqd	$4,r1
	cmpd	r1,r2
	blt	tlpgloop

	movd	@__ptrtab,r0
	addqd	$3,r0
	movd	@__tlpagetab,r1
	movd	r0,0x3fc(r1)	# last 64k is mapped the same as the first

	movd	$MINMEM+3,r0	# setup bottom level page maps (valid,
				#  write) to point to memory above 512k
	movd	@__ptrtab,r1
	addd	$0x1000,r1
hiptrloop:
	movd	r0,0(r1)
	addd	$PAGESIZE,r0
	addqd	$4,r1
	cmpd	r0,$MAXPHYSMEM
	blt	hiptrloop

	movd	@__monmem,r0	# point low logical memory 
	addqd	$3,r0		#  (with valid and write bits)
	movd	@__monmem,r2
	addd	$DMASIZE,r2
	movd	@__ptrtab,r1	#  to relocated monitor
loptrloop:
	movd	r0,0(r1)
	addd	$PAGESIZE,r0
	addqd	$4,r1
	cmpd	r0,r2
	blt	loptrloop

	movd	$DMASIZE+3,r0	# point mid logical memory to physical
midptrloop:			#  r1 still has correct ptrtab from above
	movd	r0,0(r1)
	addd	$PAGESIZE,r0
	addqd	$4,r1
	cmpd	r0,$MAXPHYSMEM
	blt	midptrloop

	movd	$0,r0		# copy monitor from low phys to high phys
	movd	@__monmem,r2
memloop:
	movd	0(r0),0(r2)
	addqd	$4,r0
	addqd	$4,r2
	cmpd	r0,$STACKPTR
	blt	memloop

	movd	@__errptblk+4,@__parerrs
	movd	$0,r0		# read monitor, looking for parity errors
	movd	@__monmem,r2
rdloop:
	movd	0(r2),r1
	addqd	$4,r0
	addqd	$4,r2
	cmpd	r0,$STACKPTR
	blt	rdloop
	
	cmpd	@__errptblk+4,@__parerrs # see if we got a parity error
	beq	mapit			# we didn't get a parity error
	cbitd	@__lastbank,@__memmap	# invalidate bank in bank map
	sbitd	@__lastbank,@__badmemmap
	br	findtop

				# map in physical 0 to new program location
mapit:
	movd	@__kptrtab,r0
	addqd	$3,r0
	movd	@__tlpagetab,r1	
	movqd	$0,r2
maploop:
	movd	r0,0x3c0(r1)[r2:d] # fill first 14 top level spots (for 900KB)
	addd	$0x200,r0
	addqd	$1,r2
	cmpd	$14,r2
	bne	maploop

	movd	$0,r5		# map in first 900KB to logical 0xf00000
	movd	@__kptrtab,r7
	movd	r7,r6
	addd	$0x1c00,r6	# enough room for 900KB
kernloop:
	movd	@__memmap,tos
	movd	r5,tos
	jsr	@__getpaddr
	adjspb	$-8
	addqd	$3,r0
	movd	r0,0(r7)
	addd	$PAGESIZE,r5
	addqd	$4,r7
	cmpd	r7,r6
	blt	kernloop

	movd	@__regpage,r1
	movd	$0,-4(r1)		# last page is invalid
	movd	$PR_WAIT+3,0(r1)	# WAIT on READ, MISCOMPARE on write
	movd	$PR_STAT+3,4(r1)	# STAT on read, C.RSTE on write
	movd	$PR_C_ACK+3,8(r1)	# C.ACK on write
	movd	$PR_C_IRQ+3,0xc(r1)	# C.IRQ on write

	lmr	ptb0,@__tlpagetab # setup system and user top level page table
	nop			# allow dma activity

	lmr	msr,$B_SR_MAPON	# allow mapping via map status register
	bispsrw	$PSW_I		# turn on interrupts
	movqw	$1,@__mapped	# global flag to signify mapping is on

	movb	$0,@R_C_IRQ	# bring pc back to life

	jsr	@__opus		# initialize communications with pc
	sprd	sp,@savesp	# save sp and fp for <del> handling
	sprd	fp,@savefp
	jsr	@__main		# execute main program and never return back
	br	.		# should never get here

__jptoit:			# jptoit(base,sp,ptb0)
	sprd	sp,@savesp
	sprd	fp,@savefp
	bicpsrw	$PS_I>>16	# turn off interrupts
	movb	$0,@R_C_ACK	# reset interrupt from PC
	movd	4(sp),r0
	movd	8(sp),r1
	movd	12(sp),r2
	lprd	fp,$0		# reset frame pointer
	lprd	sp,r1		# set stack pointer
	lmr	ptb0,r2		# invalidate maps
	jsr	0(r0)		# jump to specified location
	lmr	ptb0,@__tlpagetab # bring back top level maps
	setcfg	[m,f]		# enable mmu and fpu instructions
	lprd	sp,@savesp	# retrieve saved sp and fp
	lprd	fp,@savefp
	addr	@__inttab,r0	# set up interrupts and traps for inttab
	lprd	intbase,r0
	addr	@MODTAB,r0	# setup modtab for interrupts
	addr	@__zero,0(r0)
	addr	@0,4(r0)
	addr	@__zero,8(r0)
	addr	@0,12(r0)
	lprw	mod,r0
	bispsrw	$PS_I>>16	# turn on interrupts
	movd	@__exitval,r0
	ret	$0

__exit:				# _main will exit to DOS when done
	lprd	sp,@savesp	# retrieve saved sp and fp
	lprd	fp,@savefp
	movd	$NFILES-1,r7
exitloop:
	movd	r7,tos
#ifdef OPSASH
	jsr	@_close
#endif
	adjspb	$-4
	addqd	$-1,r7
	cmpd	r7,$3
	bge	exitloop
	jsr	@__main		# try again
	br	.		# should never get here

.data
__dmaslot:			# this should be on a 512 byte boundary
	.space	512
__modtab:
	.double	__zero,0,__zero,0

#include "asm.pm100.h"
