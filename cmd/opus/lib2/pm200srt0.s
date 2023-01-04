# @(#)pm200srt0.s	1.2 10/31/86
# Startup code for standalone programs

# Page table entry definitions
	.set	PG_V,0x1		# Page Valid
	.set	PG_RW,0x6		# Read, Write
	.set	PG_PF,0			# Invalid

	.set	MAPINITWORD,0x7		# Page Valid, Read, Write
	.set	AT_MEM_SPACE,0x80000000	
	.set	AT_IO_SPACE,0x60000000

# Opus specific
	.set	COMMPAGE,0x0000		# communications page
	.set	MODTAB,0x1000
	.set	MAGIC_PM200,0xf332
	.set	HPAGE,0x5ffff000	# ppn of hardware registers i/o style
	.set	F_AT,0x16
	.set	CLR_AT,6
	.set	CLR_CP,4
	.set	CR0,0x3ffc
	.set	SET,1
	.set	CFGREGADDR,0xc0	# address of CFGREG
	.set	MC_TS,2			# translate supv bit

	.globl	start
	.globl	_edata
	.globl	_end
	.globl	_main
	.globl	__init
	.globl	_exit
	.globl	__exit
	.globl	_trap
	.globl	__r
	.globl	__opus
	.globl	_environ

	.text

# PAGE 0
#	The commpage

commpage:
	jump	@start			# branch to startup code
	.align	4
# zero communication area and skip to next page
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.double	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.align	0x1000
__tmpstack:


# PAGE 1
# startup code
	.double	0,0,0,0		# module table
vector:
	.word	MODTAB		# mod
	.word	trap		# non-vectored interrupt
	.word	MODTAB		# mod
	.word	trap		# non-maskable interrupt
	.word	MODTAB		# mod
	.word	trap		# abort
	.word	MODTAB		# mod
	.word	trap		# FPU trap
	.word	MODTAB		# mod
	.word	trap		# illegal operation trap
	.word	MODTAB		# mod
	.word	trap		# supervisor call trap
	.word	MODTAB		# mod
	.word	trap		# divide by zero trap
	.word	MODTAB		# mod
	.word	trap		# flag trap
	.word	MODTAB		# mod
	.word	trap		# breakpoint trap
	.word	MODTAB		# mod
	.word	trap		# trace trap
	.word	MODTAB		# mod
	.word	trap		# UNDEFINED INSTRUCTION TRAP
	.word	MODTAB		# mod
	.word	trap		# reserved
	.word	MODTAB		# mod
	.word	trap		# reserved
	.word	MODTAB		# mod
	.word	trap		# reserved
	.word	MODTAB		# mod
	.word	trap		# reserved
	.word	MODTAB		# mod
	.word	trap		# reserved
trap:
	movd	r0,@__r
	movd	r1,@__r+4
	movd	r2,@__r+8
	movd	r3,@__r+12
	movd	r4,@__r+16
	movd	r5,@__r+20
	movd	r6,@__r+24
	movd	r7,@__r+28
	movd	0(sp),__r+32
	sprd	sb,__r+36
	sprd	fp,__r+40
	sprd	sp,__r+44
	sprd	intbase,__r+48
	movd	4(sp),__r+52		# psr/mod
	smr	msr,__r+76
	smr	ptb0,__r+84
	smr	ptb1,__r+88
	smr	tear,__r+92
	jsr	_trap
	.align	4
	rett	$0

	.align	4
start:
	cmpqd	$0,@CFGREGADDR
	beq	havfpu1			# old opmon
	tbitb	$1,@CFGREGADDR
	bfc	nofpu1
havfpu1:
	addr	@0x26,@CFGREGADDR	# FM | M | F
	setcfg	[m,f,fm]
	br	done1
nofpu1:
	setcfg	[m]			# 532 MMU only
done1:
	movqd	$0,r0
	movqd	$0,r1
	movqd	$0,r2
	movqd	$0,r3
	movqd	$0,r4
	movqd	$0,r5
	movqd	$0,r6
	movqd	$0,r7
	tbitb	$1,@CFGREGADDR
	bfc	nofpu2
	movdl	r0,f0
	movl	f0,f2
	movl	f0,f4
	movl	f0,f6
nofpu2:
# set up registers
	movd	$(0x2000+PG_RW+PG_V),@0x2ffc	# interrupt vector,
					# see pm200start.c
	addr	@0x2000,r1		# Top level page table 
	lmr	ptb0,r1
	movd	$MC_TS,r1		# Translate supervisor space
	lmr	mcr,r1
	bicpsrw	$0xf00			# ints off, system stack, system mode
	lprw	mod,$MODTAB		# set mod register to mod table
	movd	$__tmpstack,r5		# set tmp int. stack at end of commpage
	lprd	sp,r5
	lprd	fp,r5
	movd	$vector,r5
	lprd	intbase,r5
	movqd	$0,@0			# Zero address zero.

# size memory first
	jsr	__memsize
# clear bss
	addr	_edata,r1
	addr	4(r1),r2
	#addr	_end,r0		# already in r0
	addqd	$-4,r0		# madan says so
	subd	r2,r0
	ashd	$-2,r0		# r0 = count of double words for speed
	movqd	0,0(r1)		# zero first double word for this to work
	movsd			# clear memory

# size memory
	jsr	__memsize	# size memory
	lprd	sp,r0		# set stack to end of usable memory
	lprd	fp,r0

# bring pc back to life
	movd	$paget,@COMMPAGE+0x14
	movd	$0x100000,@COMMPAGE+0x18
	movw	@COMMPAGE+0xbc,@COMMPAGE+0x10
	movd	@_S_CR0,r0
	movb	$CLR_AT,0(r0)
	movb	$CLR_CP,0(r0)
	movb	$(F_AT+SET),0(r0)

# call opus intialization routines
	jsr	__opus

# call device init routine
	jsr	__init

# Set up arguments to main()
	movd	@_environ,tos
	movd	@__argv0,tos	# Get **argv
	movd	@__argc,tos	# and argc
	jsr	_main		# Call main()
	adjspb	$-12

# Here at _exit, either by call, or by falling through from return
# from main(). 

_exit:
	# Put any desired cleanup here
__exit:
	movqd	$-1,tos
	jsr	__pause

	.data
	.align	4
	.globl cerror
cerror: .double 0			# used by libc
	.globl	_S_CR0
_S_CR0:
	.double	CR0
	.globl	__argc
	.globl	__argv0
	.globl	__argv00
__argc: .double		0
__argv0: .double	__argv00
__argv00: .double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
	.double	0
env00:	.double	0
_environ:
	.double	env00
	.text

	.align 0x1000

# PAGE 2
# 1st level page table entries
segt:
	.double	0x4000+PG_RW+PG_V	# segment valid (0-4MB)
	.double	0x5000+PG_RW+PG_V	# segment valid (4-8MB)
	.double	0x6000+PG_RW+PG_V	# segment valid (8-12MB)
	.double	0x7000+PG_RW+PG_V	# segment valid (12-16MB)
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	0x3000+PG_RW+PG_V	# PC BUS-MASTER I/O MEMORY (0xa0000000)
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	0x8000+PG_RW+PG_V	# PC BUS-MASTER MEMORY (0xb0000000)
	.double	0x9000+PG_RW+PG_V	# PC BUS-MASTER MEMORY (4-8MB)
	.double	0xa000+PG_RW+PG_V	# PC BUS-MASTER MEMORY (8-12MB)
	.double	0xb000+PG_RW+PG_V	# PC BUS-MASTER MEMORY (12-16MB)
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	PG_PF			# invalid segment
	.double	0x2000+PG_RW+PG_V	# interrupt vector

	.align	0x1000

# PAGE 3
# BUS-MASTER I/O 2nd level page table entries
# NOT AVAILABLE FROM LOGICAL MEMORY (HARDWARE PAGE)
pagebm:
	.double	0x0000+AT_IO_SPACE+MAPINITWORD
	.space	0xffc

# PAGE 4
# opmon dma map entries and 2nd level page table entries
paget:
	.double	0x0000+PG_RW+PG_V	# commpage
	.double	0x1000+PG_RW+PG_V	# startup code
	.double	0x2000+PG_RW+PG_V	# 1st level page table entries
hpage:
	.double	HPAGE+PG_RW+PG_V	# hardware page (also I/O page)
	.double	0x4000+PG_RW+PG_V	# 2nd level page table entries
	.double	0x5000+PG_RW+PG_V	# 2nd level page table entries
	.double	0x6000+PG_RW+PG_V	# 2nd level page table entries
	.double	0x7000+PG_RW+PG_V	# 2nd level page table entries
	.double	0x8000+PG_RW+PG_V	# 2nd level bus-master entries
	.double	0x9000+PG_RW+PG_V	# 2nd level bus-master entries
	.double	0xa000+PG_RW+PG_V	# 2nd level bus-master entries
	.double	0xb000+PG_RW+PG_V	# 2nd level bus-master entries
	.double	0xc000+PG_RW+PG_V
	.double	0xd000+PG_RW+PG_V
	.double	0xe000+PG_RW+PG_V
	.double	0xf000+PG_RW+PG_V
	.double	0x10000+PG_RW+PG_V
	.double	0x11000+PG_RW+PG_V
	.double	0x12000+PG_RW+PG_V
	.double	0x13000+PG_RW+PG_V
	.double	0x14000+PG_RW+PG_V
	.double	0x15000+PG_RW+PG_V
	.double	0x16000+PG_RW+PG_V
	.double	0x17000+PG_RW+PG_V
	.double	0x18000+PG_RW+PG_V
	.double	0x19000+PG_RW+PG_V
	.double	0x1a000+PG_RW+PG_V
	.double	0x1b000+PG_RW+PG_V
	.double	0x1c000+PG_RW+PG_V
	.double	0x1d000+PG_RW+PG_V
	.double	0x1e000+PG_RW+PG_V
	.double	0x1f000+PG_RW+PG_V
	.double	0x20000+PG_RW+PG_V
	.double	0x21000+PG_RW+PG_V
	.double	0x22000+PG_RW+PG_V
	.double	0x23000+PG_RW+PG_V
	.double	0x24000+PG_RW+PG_V
	.double	0x25000+PG_RW+PG_V
	.double	0x26000+PG_RW+PG_V
	.double	0x27000+PG_RW+PG_V
	.double	0x28000+PG_RW+PG_V
	.double	0x29000+PG_RW+PG_V
	.double	0x2a000+PG_RW+PG_V
	.double	0x2b000+PG_RW+PG_V
	.double	0x2c000+PG_RW+PG_V
	.double	0x2d000+PG_RW+PG_V
	.double	0x2e000+PG_RW+PG_V
	.double	0x2f000+PG_RW+PG_V
	.double	0x30000+PG_RW+PG_V
	.double	0x31000+PG_RW+PG_V
	.double	0x32000+PG_RW+PG_V
	.double	0x33000+PG_RW+PG_V
	.double	0x34000+PG_RW+PG_V
	.double	0x35000+PG_RW+PG_V
	.double	0x36000+PG_RW+PG_V
	.double	0x37000+PG_RW+PG_V
	.double	0x38000+PG_RW+PG_V
	.double	0x39000+PG_RW+PG_V
	.double	0x3a000+PG_RW+PG_V
	.double	0x3b000+PG_RW+PG_V
	.double	0x3c000+PG_RW+PG_V
	.double	0x3d000+PG_RW+PG_V
	.double	0x3e000+PG_RW+PG_V
	.double	0x3f000+PG_RW+PG_V
	.double	0x40000+PG_RW+PG_V
	.double	0x41000+PG_RW+PG_V
	.double	0x42000+PG_RW+PG_V
	.double	0x43000+PG_RW+PG_V
	.double	0x44000+PG_RW+PG_V
	.double	0x45000+PG_RW+PG_V
	.double	0x46000+PG_RW+PG_V
	.double	0x47000+PG_RW+PG_V
	.double	0x48000+PG_RW+PG_V
	.double	0x49000+PG_RW+PG_V
	.double	0x4a000+PG_RW+PG_V
	.double	0x4b000+PG_RW+PG_V
	.double	0x4c000+PG_RW+PG_V
	.double	0x4d000+PG_RW+PG_V
	.double	0x4e000+PG_RW+PG_V
	.double	0x4f000+PG_RW+PG_V
	.double	0x50000+PG_RW+PG_V
	.double	0x51000+PG_RW+PG_V
	.double	0x52000+PG_RW+PG_V
	.double	0x53000+PG_RW+PG_V
	.double	0x54000+PG_RW+PG_V
	.double	0x55000+PG_RW+PG_V
	.double	0x56000+PG_RW+PG_V
	.double	0x57000+PG_RW+PG_V
	.double	0x58000+PG_RW+PG_V
	.double	0x59000+PG_RW+PG_V
	.double	0x5a000+PG_RW+PG_V
	.double	0x5b000+PG_RW+PG_V
	.double	0x5c000+PG_RW+PG_V
	.double	0x5d000+PG_RW+PG_V
	.double	0x5e000+PG_RW+PG_V
	.double	0x5f000+PG_RW+PG_V
	.double	0x60000+PG_RW+PG_V
	.double	0x61000+PG_RW+PG_V
	.double	0x62000+PG_RW+PG_V
	.double	0x63000+PG_RW+PG_V
	.double	0x64000+PG_RW+PG_V
	.double	0x65000+PG_RW+PG_V
	.double	0x66000+PG_RW+PG_V
	.double	0x67000+PG_RW+PG_V
	.double	0x68000+PG_RW+PG_V
	.double	0x69000+PG_RW+PG_V
	.double	0x6a000+PG_RW+PG_V
	.double	0x6b000+PG_RW+PG_V
	.double	0x6c000+PG_RW+PG_V
	.double	0x6d000+PG_RW+PG_V
	.double	0x6e000+PG_RW+PG_V
	.double	0x6f000+PG_RW+PG_V
	.double	0x70000+PG_RW+PG_V
	.double	0x71000+PG_RW+PG_V
	.double	0x72000+PG_RW+PG_V
	.double	0x73000+PG_RW+PG_V
	.double	0x74000+PG_RW+PG_V
	.double	0x75000+PG_RW+PG_V
	.double	0x76000+PG_RW+PG_V
	.double	0x77000+PG_RW+PG_V
	.double	0x78000+PG_RW+PG_V
	.double	0x79000+PG_RW+PG_V
	.double	0x7a000+PG_RW+PG_V
	.double	0x7b000+PG_RW+PG_V
	.double	0x7c000+PG_RW+PG_V
	.double	0x7d000+PG_RW+PG_V
	.double	0x7e000+PG_RW+PG_V
	.double	0x7f000+PG_RW+PG_V
	.double	0x80000+PG_RW+PG_V
	.double	0x81000+PG_RW+PG_V
	.double	0x82000+PG_RW+PG_V
	.double	0x83000+PG_RW+PG_V
	.double	0x84000+PG_RW+PG_V
	.double	0x85000+PG_RW+PG_V
	.double	0x86000+PG_RW+PG_V
	.double	0x87000+PG_RW+PG_V
	.double	0x88000+PG_RW+PG_V
	.double	0x89000+PG_RW+PG_V
	.double	0x8a000+PG_RW+PG_V
	.double	0x8b000+PG_RW+PG_V
	.double	0x8c000+PG_RW+PG_V
	.double	0x8d000+PG_RW+PG_V
	.double	0x8e000+PG_RW+PG_V
	.double	0x8f000+PG_RW+PG_V
	.double	0x90000+PG_RW+PG_V
	.double	0x91000+PG_RW+PG_V
	.double	0x92000+PG_RW+PG_V
	.double	0x93000+PG_RW+PG_V
	.double	0x94000+PG_RW+PG_V
	.double	0x95000+PG_RW+PG_V
	.double	0x96000+PG_RW+PG_V
	.double	0x97000+PG_RW+PG_V
	.double	0x98000+PG_RW+PG_V
	.double	0x99000+PG_RW+PG_V
	.double	0x9a000+PG_RW+PG_V
	.double	0x9b000+PG_RW+PG_V
	.double	0x9c000+PG_RW+PG_V
	.double	0x9d000+PG_RW+PG_V
	.double	0x9e000+PG_RW+PG_V
	.double	0x9f000+PG_RW+PG_V
	.double	0xa0000+PG_RW+PG_V
	.double	0xa1000+PG_RW+PG_V
	.double	0xa2000+PG_RW+PG_V
	.double	0xa3000+PG_RW+PG_V
	.double	0xa4000+PG_RW+PG_V
	.double	0xa5000+PG_RW+PG_V
	.double	0xa6000+PG_RW+PG_V
	.double	0xa7000+PG_RW+PG_V
	.double	0xa8000+PG_RW+PG_V
	.double	0xa9000+PG_RW+PG_V
	.double	0xaa000+PG_RW+PG_V
	.double	0xab000+PG_RW+PG_V
	.double	0xac000+PG_RW+PG_V
	.double	0xad000+PG_RW+PG_V
	.double	0xae000+PG_RW+PG_V
	.double	0xaf000+PG_RW+PG_V
	.double	0xb0000+PG_RW+PG_V
	.double	0xb1000+PG_RW+PG_V
	.double	0xb2000+PG_RW+PG_V
	.double	0xb3000+PG_RW+PG_V
	.double	0xb4000+PG_RW+PG_V
	.double	0xb5000+PG_RW+PG_V
	.double	0xb6000+PG_RW+PG_V
	.double	0xb7000+PG_RW+PG_V
	.double	0xb8000+PG_RW+PG_V
	.double	0xb9000+PG_RW+PG_V
	.double	0xba000+PG_RW+PG_V
	.double	0xbb000+PG_RW+PG_V
	.double	0xbc000+PG_RW+PG_V
	.double	0xbd000+PG_RW+PG_V
	.double	0xbe000+PG_RW+PG_V
	.double	0xbf000+PG_RW+PG_V
	.double	0xc0000+PG_RW+PG_V
	.double	0xc1000+PG_RW+PG_V
	.double	0xc2000+PG_RW+PG_V
	.double	0xc3000+PG_RW+PG_V
	.double	0xc4000+PG_RW+PG_V
	.double	0xc5000+PG_RW+PG_V
	.double	0xc6000+PG_RW+PG_V
	.double	0xc7000+PG_RW+PG_V
	.double	0xc8000+PG_RW+PG_V
	.double	0xc9000+PG_RW+PG_V
	.double	0xca000+PG_RW+PG_V
	.double	0xcb000+PG_RW+PG_V
	.double	0xcc000+PG_RW+PG_V
	.double	0xcd000+PG_RW+PG_V
	.double	0xce000+PG_RW+PG_V
	.double	0xcf000+PG_RW+PG_V
	.double	0xd0000+PG_RW+PG_V
	.double	0xd1000+PG_RW+PG_V
	.double	0xd2000+PG_RW+PG_V
	.double	0xd3000+PG_RW+PG_V
	.double	0xd4000+PG_RW+PG_V
	.double	0xd5000+PG_RW+PG_V
	.double	0xd6000+PG_RW+PG_V
	.double	0xd7000+PG_RW+PG_V
	.double	0xd8000+PG_RW+PG_V
	.double	0xd9000+PG_RW+PG_V
	.double	0xda000+PG_RW+PG_V
	.double	0xdb000+PG_RW+PG_V
	.double	0xdc000+PG_RW+PG_V
	.double	0xdd000+PG_RW+PG_V
	.double	0xde000+PG_RW+PG_V
	.double	0xdf000+PG_RW+PG_V
	.double	0xe0000+PG_RW+PG_V
	.double	0xe1000+PG_RW+PG_V
	.double	0xe2000+PG_RW+PG_V
	.double	0xe3000+PG_RW+PG_V
	.double	0xe4000+PG_RW+PG_V
	.double	0xe5000+PG_RW+PG_V
	.double	0xe6000+PG_RW+PG_V
	.double	0xe7000+PG_RW+PG_V
	.double	0xe8000+PG_RW+PG_V
	.double	0xe9000+PG_RW+PG_V
	.double	0xea000+PG_RW+PG_V
	.double	0xeb000+PG_RW+PG_V
	.double	0xec000+PG_RW+PG_V
	.double	0xed000+PG_RW+PG_V
	.double	0xee000+PG_RW+PG_V
	.double	0xef000+PG_RW+PG_V
	.double	0xf0000+PG_RW+PG_V
	.double	0xf1000+PG_RW+PG_V
	.double	0xf2000+PG_RW+PG_V
	.double	0xf3000+PG_RW+PG_V
	.double	0xf4000+PG_RW+PG_V
	.double	0xf5000+PG_RW+PG_V
	.double	0xf6000+PG_RW+PG_V
	.double	0xf7000+PG_RW+PG_V
	.double	0xf8000+PG_RW+PG_V
	.double	0xf9000+PG_RW+PG_V
	.double	0xfa000+PG_RW+PG_V
	.double	0xfb000+PG_RW+PG_V
	.double	0xfc000+PG_RW+PG_V
	.double	0xfd000+PG_RW+PG_V
	.double	0xfe000+PG_RW+PG_V
	.double	0xff000+PG_RW+PG_V
	.double	0x100000+PG_RW+PG_V	# memory page
	.double	0x101000+PG_RW+PG_V	# memory page
	.double	0x102000+PG_RW+PG_V	# memory page
	.double	0x103000+PG_RW+PG_V	# memory page
	.double	0x104000+PG_RW+PG_V	# memory page
	.double	0x105000+PG_RW+PG_V	# memory page
	.double	0x106000+PG_RW+PG_V	# memory page
	.double	0x107000+PG_RW+PG_V	# memory page
	.double	0x108000+PG_RW+PG_V	# memory page
	.double	0x109000+PG_RW+PG_V	# memory page
	.double	0x10a000+PG_RW+PG_V	# memory page
	.double	0x10b000+PG_RW+PG_V	# memory page
	.double	0x10c000+PG_RW+PG_V	# memory page
	.double	0x10d000+PG_RW+PG_V	# memory page
	.double	0x10e000+PG_RW+PG_V	# memory page
	.double	0x10f000+PG_RW+PG_V	# memory page
	.double	0x110000+PG_RW+PG_V	# memory page
	.double	0x111000+PG_RW+PG_V	# memory page
	.double	0x112000+PG_RW+PG_V	# memory page
	.double	0x113000+PG_RW+PG_V	# memory page
	.double	0x114000+PG_RW+PG_V	# memory page
	.double	0x115000+PG_RW+PG_V	# memory page
	.double	0x116000+PG_RW+PG_V	# memory page
	.double	0x117000+PG_RW+PG_V	# memory page
	.double	0x118000+PG_RW+PG_V	# memory page
	.double	0x119000+PG_RW+PG_V	# memory page
	.double	0x11a000+PG_RW+PG_V	# memory page
	.double	0x11b000+PG_RW+PG_V	# memory page
	.double	0x11c000+PG_RW+PG_V	# memory page
	.double	0x11d000+PG_RW+PG_V	# memory page
	.double	0x11e000+PG_RW+PG_V	# memory page
	.double	0x11f000+PG_RW+PG_V	# memory page
	.double	0x120000+PG_RW+PG_V	# memory page
	.double	0x121000+PG_RW+PG_V	# memory page
	.double	0x122000+PG_RW+PG_V	# memory page
	.double	0x123000+PG_RW+PG_V	# memory page
	.double	0x124000+PG_RW+PG_V	# memory page
	.double	0x125000+PG_RW+PG_V	# memory page
	.double	0x126000+PG_RW+PG_V	# memory page
	.double	0x127000+PG_RW+PG_V	# memory page
	.double	0x128000+PG_RW+PG_V	# memory page
	.double	0x129000+PG_RW+PG_V	# memory page
	.double	0x12a000+PG_RW+PG_V	# memory page
	.double	0x12b000+PG_RW+PG_V	# memory page
	.double	0x12c000+PG_RW+PG_V	# memory page
	.double	0x12d000+PG_RW+PG_V	# memory page
	.double	0x12e000+PG_RW+PG_V	# memory page
	.double	0x12f000+PG_RW+PG_V	# memory page
	.double	0x130000+PG_RW+PG_V	# memory page
	.double	0x131000+PG_RW+PG_V	# memory page
	.double	0x132000+PG_RW+PG_V	# memory page
	.double	0x133000+PG_RW+PG_V	# memory page
	.double	0x134000+PG_RW+PG_V	# memory page
	.double	0x135000+PG_RW+PG_V	# memory page
	.double	0x136000+PG_RW+PG_V	# memory page
	.double	0x137000+PG_RW+PG_V	# memory page
	.double	0x138000+PG_RW+PG_V	# memory page
	.double	0x139000+PG_RW+PG_V	# memory page
	.double	0x13a000+PG_RW+PG_V	# memory page
	.double	0x13b000+PG_RW+PG_V	# memory page
	.double	0x13c000+PG_RW+PG_V	# memory page
	.double	0x13d000+PG_RW+PG_V	# memory page
	.double	0x13e000+PG_RW+PG_V	# memory page
	.double	0x13f000+PG_RW+PG_V	# memory page
	.double	0x140000+PG_RW+PG_V	# memory page
	.double	0x141000+PG_RW+PG_V	# memory page
	.double	0x142000+PG_RW+PG_V	# memory page
	.double	0x143000+PG_RW+PG_V	# memory page
	.double	0x144000+PG_RW+PG_V	# memory page
	.double	0x145000+PG_RW+PG_V	# memory page
	.double	0x146000+PG_RW+PG_V	# memory page
	.double	0x147000+PG_RW+PG_V	# memory page
	.double	0x148000+PG_RW+PG_V	# memory page
	.double	0x149000+PG_RW+PG_V	# memory page
	.double	0x14a000+PG_RW+PG_V	# memory page
	.double	0x14b000+PG_RW+PG_V	# memory page
	.double	0x14c000+PG_RW+PG_V	# memory page
	.double	0x14d000+PG_RW+PG_V	# memory page
	.double	0x14e000+PG_RW+PG_V	# memory page
	.double	0x14f000+PG_RW+PG_V	# memory page
	.double	0x150000+PG_RW+PG_V	# memory page
	.double	0x151000+PG_RW+PG_V	# memory page
	.double	0x152000+PG_RW+PG_V	# memory page
	.double	0x153000+PG_RW+PG_V	# memory page
	.double	0x154000+PG_RW+PG_V	# memory page
	.double	0x155000+PG_RW+PG_V	# memory page
	.double	0x156000+PG_RW+PG_V	# memory page
	.double	0x157000+PG_RW+PG_V	# memory page
	.double	0x158000+PG_RW+PG_V	# memory page
	.double	0x159000+PG_RW+PG_V	# memory page
	.double	0x15a000+PG_RW+PG_V	# memory page
	.double	0x15b000+PG_RW+PG_V	# memory page
	.double	0x15c000+PG_RW+PG_V	# memory page
	.double	0x15d000+PG_RW+PG_V	# memory page
	.double	0x15e000+PG_RW+PG_V	# memory page
	.double	0x15f000+PG_RW+PG_V	# memory page
	.double	0x160000+PG_RW+PG_V	# memory page
	.double	0x161000+PG_RW+PG_V	# memory page
	.double	0x162000+PG_RW+PG_V	# memory page
	.double	0x163000+PG_RW+PG_V	# memory page
	.double	0x164000+PG_RW+PG_V	# memory page
	.double	0x165000+PG_RW+PG_V	# memory page
	.double	0x166000+PG_RW+PG_V	# memory page
	.double	0x167000+PG_RW+PG_V	# memory page
	.double	0x168000+PG_RW+PG_V	# memory page
	.double	0x169000+PG_RW+PG_V	# memory page
	.double	0x16a000+PG_RW+PG_V	# memory page
	.double	0x16b000+PG_RW+PG_V	# memory page
	.double	0x16c000+PG_RW+PG_V	# memory page
	.double	0x16d000+PG_RW+PG_V	# memory page
	.double	0x16e000+PG_RW+PG_V	# memory page
	.double	0x16f000+PG_RW+PG_V	# memory page
	.double	0x170000+PG_RW+PG_V	# memory page
	.double	0x171000+PG_RW+PG_V	# memory page
	.double	0x172000+PG_RW+PG_V	# memory page
	.double	0x173000+PG_RW+PG_V	# memory page
	.double	0x174000+PG_RW+PG_V	# memory page
	.double	0x175000+PG_RW+PG_V	# memory page
	.double	0x176000+PG_RW+PG_V	# memory page
	.double	0x177000+PG_RW+PG_V	# memory page
	.double	0x178000+PG_RW+PG_V	# memory page
	.double	0x179000+PG_RW+PG_V	# memory page
	.double	0x17a000+PG_RW+PG_V	# memory page
	.double	0x17b000+PG_RW+PG_V	# memory page
	.double	0x17c000+PG_RW+PG_V	# memory page
	.double	0x17d000+PG_RW+PG_V	# memory page
	.double	0x17e000+PG_RW+PG_V	# memory page
	.double	0x17f000+PG_RW+PG_V	# memory page
	.double	0x180000+PG_RW+PG_V	# memory page
	.double	0x181000+PG_RW+PG_V	# memory page
	.double	0x182000+PG_RW+PG_V	# memory page
	.double	0x183000+PG_RW+PG_V	# memory page
	.double	0x184000+PG_RW+PG_V	# memory page
	.double	0x185000+PG_RW+PG_V	# memory page
	.double	0x186000+PG_RW+PG_V	# memory page
	.double	0x187000+PG_RW+PG_V	# memory page
	.double	0x188000+PG_RW+PG_V	# memory page
	.double	0x189000+PG_RW+PG_V	# memory page
	.double	0x18a000+PG_RW+PG_V	# memory page
	.double	0x18b000+PG_RW+PG_V	# memory page
	.double	0x18c000+PG_RW+PG_V	# memory page
	.double	0x18d000+PG_RW+PG_V	# memory page
	.double	0x18e000+PG_RW+PG_V	# memory page
	.double	0x18f000+PG_RW+PG_V	# memory page
	.double	0x190000+PG_RW+PG_V	# memory page
	.double	0x191000+PG_RW+PG_V	# memory page
	.double	0x192000+PG_RW+PG_V	# memory page
	.double	0x193000+PG_RW+PG_V	# memory page
	.double	0x194000+PG_RW+PG_V	# memory page
	.double	0x195000+PG_RW+PG_V	# memory page
	.double	0x196000+PG_RW+PG_V	# memory page
	.double	0x197000+PG_RW+PG_V	# memory page
	.double	0x198000+PG_RW+PG_V	# memory page
	.double	0x199000+PG_RW+PG_V	# memory page
	.double	0x19a000+PG_RW+PG_V	# memory page
	.double	0x19b000+PG_RW+PG_V	# memory page
	.double	0x19c000+PG_RW+PG_V	# memory page
	.double	0x19d000+PG_RW+PG_V	# memory page
	.double	0x19e000+PG_RW+PG_V	# memory page
	.double	0x19f000+PG_RW+PG_V	# memory page
	.double	0x1a0000+PG_RW+PG_V	# memory page
	.double	0x1a1000+PG_RW+PG_V	# memory page
	.double	0x1a2000+PG_RW+PG_V	# memory page
	.double	0x1a3000+PG_RW+PG_V	# memory page
	.double	0x1a4000+PG_RW+PG_V	# memory page
	.double	0x1a5000+PG_RW+PG_V	# memory page
	.double	0x1a6000+PG_RW+PG_V	# memory page
	.double	0x1a7000+PG_RW+PG_V	# memory page
	.double	0x1a8000+PG_RW+PG_V	# memory page
	.double	0x1a9000+PG_RW+PG_V	# memory page
	.double	0x1aa000+PG_RW+PG_V	# memory page
	.double	0x1ab000+PG_RW+PG_V	# memory page
	.double	0x1ac000+PG_RW+PG_V	# memory page
	.double	0x1ad000+PG_RW+PG_V	# memory page
	.double	0x1ae000+PG_RW+PG_V	# memory page
	.double	0x1af000+PG_RW+PG_V	# memory page
	.double	0x1b0000+PG_RW+PG_V	# memory page
	.double	0x1b1000+PG_RW+PG_V	# memory page
	.double	0x1b2000+PG_RW+PG_V	# memory page
	.double	0x1b3000+PG_RW+PG_V	# memory page
	.double	0x1b4000+PG_RW+PG_V	# memory page
	.double	0x1b5000+PG_RW+PG_V	# memory page
	.double	0x1b6000+PG_RW+PG_V	# memory page
	.double	0x1b7000+PG_RW+PG_V	# memory page
	.double	0x1b8000+PG_RW+PG_V	# memory page
	.double	0x1b9000+PG_RW+PG_V	# memory page
	.double	0x1ba000+PG_RW+PG_V	# memory page
	.double	0x1bb000+PG_RW+PG_V	# memory page
	.double	0x1bc000+PG_RW+PG_V	# memory page
	.double	0x1bd000+PG_RW+PG_V	# memory page
	.double	0x1be000+PG_RW+PG_V	# memory page
	.double	0x1bf000+PG_RW+PG_V	# memory page
	.double	0x1c0000+PG_RW+PG_V	# memory page
	.double	0x1c1000+PG_RW+PG_V	# memory page
	.double	0x1c2000+PG_RW+PG_V	# memory page
	.double	0x1c3000+PG_RW+PG_V	# memory page
	.double	0x1c4000+PG_RW+PG_V	# memory page
	.double	0x1c5000+PG_RW+PG_V	# memory page
	.double	0x1c6000+PG_RW+PG_V	# memory page
	.double	0x1c7000+PG_RW+PG_V	# memory page
	.double	0x1c8000+PG_RW+PG_V	# memory page
	.double	0x1c9000+PG_RW+PG_V	# memory page
	.double	0x1ca000+PG_RW+PG_V	# memory page
	.double	0x1cb000+PG_RW+PG_V	# memory page
	.double	0x1cc000+PG_RW+PG_V	# memory page
	.double	0x1cd000+PG_RW+PG_V	# memory page
	.double	0x1ce000+PG_RW+PG_V	# memory page
	.double	0x1cf000+PG_RW+PG_V	# memory page
	.double	0x1d0000+PG_RW+PG_V	# memory page
	.double	0x1d1000+PG_RW+PG_V	# memory page
	.double	0x1d2000+PG_RW+PG_V	# memory page
	.double	0x1d3000+PG_RW+PG_V	# memory page
	.double	0x1d4000+PG_RW+PG_V	# memory page
	.double	0x1d5000+PG_RW+PG_V	# memory page
	.double	0x1d6000+PG_RW+PG_V	# memory page
	.double	0x1d7000+PG_RW+PG_V	# memory page
	.double	0x1d8000+PG_RW+PG_V	# memory page
	.double	0x1d9000+PG_RW+PG_V	# memory page
	.double	0x1da000+PG_RW+PG_V	# memory page
	.double	0x1db000+PG_RW+PG_V	# memory page
	.double	0x1dc000+PG_RW+PG_V	# memory page
	.double	0x1dd000+PG_RW+PG_V	# memory page
	.double	0x1de000+PG_RW+PG_V	# memory page
	.double	0x1df000+PG_RW+PG_V	# memory page
	.double	0x1e0000+PG_RW+PG_V	# memory page
	.double	0x1e1000+PG_RW+PG_V	# memory page
	.double	0x1e2000+PG_RW+PG_V	# memory page
	.double	0x1e3000+PG_RW+PG_V	# memory page
	.double	0x1e4000+PG_RW+PG_V	# memory page
	.double	0x1e5000+PG_RW+PG_V	# memory page
	.double	0x1e6000+PG_RW+PG_V	# memory page
	.double	0x1e7000+PG_RW+PG_V	# memory page
	.double	0x1e8000+PG_RW+PG_V	# memory page
	.double	0x1e9000+PG_RW+PG_V	# memory page
	.double	0x1ea000+PG_RW+PG_V	# memory page
	.double	0x1eb000+PG_RW+PG_V	# memory page
	.double	0x1ec000+PG_RW+PG_V	# memory page
	.double	0x1ed000+PG_RW+PG_V	# memory page
	.double	0x1ee000+PG_RW+PG_V	# memory page
	.double	0x1ef000+PG_RW+PG_V	# memory page
	.double	0x1f0000+PG_RW+PG_V	# memory page
	.double	0x1f1000+PG_RW+PG_V	# memory page
	.double	0x1f2000+PG_RW+PG_V	# memory page
	.double	0x1f3000+PG_RW+PG_V	# memory page
	.double	0x1f4000+PG_RW+PG_V	# memory page
	.double	0x1f5000+PG_RW+PG_V	# memory page
	.double	0x1f6000+PG_RW+PG_V	# memory page
	.double	0x1f7000+PG_RW+PG_V	# memory page
	.double	0x1f8000+PG_RW+PG_V	# memory page
	.double	0x1f9000+PG_RW+PG_V	# memory page
	.double	0x1fa000+PG_RW+PG_V	# memory page
	.double	0x1fb000+PG_RW+PG_V	# memory page
	.double	0x1fc000+PG_RW+PG_V	# memory page
	.double	0x1fd000+PG_RW+PG_V	# memory page
	.double	0x1fe000+PG_RW+PG_V	# memory page
	.double	0x1ff000+PG_RW+PG_V	# memory page
	.double	0x200000+PG_RW+PG_V	# memory page
	.double	0x201000+PG_RW+PG_V	# memory page
	.double	0x202000+PG_RW+PG_V	# memory page
	.double	0x203000+PG_RW+PG_V	# memory page
	.double	0x204000+PG_RW+PG_V	# memory page
	.double	0x205000+PG_RW+PG_V	# memory page
	.double	0x206000+PG_RW+PG_V	# memory page
	.double	0x207000+PG_RW+PG_V	# memory page
	.double	0x208000+PG_RW+PG_V	# memory page
	.double	0x209000+PG_RW+PG_V	# memory page
	.double	0x20a000+PG_RW+PG_V	# memory page
	.double	0x20b000+PG_RW+PG_V	# memory page
	.double	0x20c000+PG_RW+PG_V	# memory page
	.double	0x20d000+PG_RW+PG_V	# memory page
	.double	0x20e000+PG_RW+PG_V	# memory page
	.double	0x20f000+PG_RW+PG_V	# memory page
	.double	0x210000+PG_RW+PG_V	# memory page
	.double	0x211000+PG_RW+PG_V	# memory page
	.double	0x212000+PG_RW+PG_V	# memory page
	.double	0x213000+PG_RW+PG_V	# memory page
	.double	0x214000+PG_RW+PG_V	# memory page
	.double	0x215000+PG_RW+PG_V	# memory page
	.double	0x216000+PG_RW+PG_V	# memory page
	.double	0x217000+PG_RW+PG_V	# memory page
	.double	0x218000+PG_RW+PG_V	# memory page
	.double	0x219000+PG_RW+PG_V	# memory page
	.double	0x21a000+PG_RW+PG_V	# memory page
	.double	0x21b000+PG_RW+PG_V	# memory page
	.double	0x21c000+PG_RW+PG_V	# memory page
	.double	0x21d000+PG_RW+PG_V	# memory page
	.double	0x21e000+PG_RW+PG_V	# memory page
	.double	0x21f000+PG_RW+PG_V	# memory page
	.double	0x220000+PG_RW+PG_V	# memory page
	.double	0x221000+PG_RW+PG_V	# memory page
	.double	0x222000+PG_RW+PG_V	# memory page
	.double	0x223000+PG_RW+PG_V	# memory page
	.double	0x224000+PG_RW+PG_V	# memory page
	.double	0x225000+PG_RW+PG_V	# memory page
	.double	0x226000+PG_RW+PG_V	# memory page
	.double	0x227000+PG_RW+PG_V	# memory page
	.double	0x228000+PG_RW+PG_V	# memory page
	.double	0x229000+PG_RW+PG_V	# memory page
	.double	0x22a000+PG_RW+PG_V	# memory page
	.double	0x22b000+PG_RW+PG_V	# memory page
	.double	0x22c000+PG_RW+PG_V	# memory page
	.double	0x22d000+PG_RW+PG_V	# memory page
	.double	0x22e000+PG_RW+PG_V	# memory page
	.double	0x22f000+PG_RW+PG_V	# memory page
	.double	0x230000+PG_RW+PG_V	# memory page
	.double	0x231000+PG_RW+PG_V	# memory page
	.double	0x232000+PG_RW+PG_V	# memory page
	.double	0x233000+PG_RW+PG_V	# memory page
	.double	0x234000+PG_RW+PG_V	# memory page
	.double	0x235000+PG_RW+PG_V	# memory page
	.double	0x236000+PG_RW+PG_V	# memory page
	.double	0x237000+PG_RW+PG_V	# memory page
	.double	0x238000+PG_RW+PG_V	# memory page
	.double	0x239000+PG_RW+PG_V	# memory page
	.double	0x23a000+PG_RW+PG_V	# memory page
	.double	0x23b000+PG_RW+PG_V	# memory page
	.double	0x23c000+PG_RW+PG_V	# memory page
	.double	0x23d000+PG_RW+PG_V	# memory page
	.double	0x23e000+PG_RW+PG_V	# memory page
	.double	0x23f000+PG_RW+PG_V	# memory page
	.double	0x240000+PG_RW+PG_V	# memory page
	.double	0x241000+PG_RW+PG_V	# memory page
	.double	0x242000+PG_RW+PG_V	# memory page
	.double	0x243000+PG_RW+PG_V	# memory page
	.double	0x244000+PG_RW+PG_V	# memory page
	.double	0x245000+PG_RW+PG_V	# memory page
	.double	0x246000+PG_RW+PG_V	# memory page
	.double	0x247000+PG_RW+PG_V	# memory page
	.double	0x248000+PG_RW+PG_V	# memory page
	.double	0x249000+PG_RW+PG_V	# memory page
	.double	0x24a000+PG_RW+PG_V	# memory page
	.double	0x24b000+PG_RW+PG_V	# memory page
	.double	0x24c000+PG_RW+PG_V	# memory page
	.double	0x24d000+PG_RW+PG_V	# memory page
	.double	0x24e000+PG_RW+PG_V	# memory page
	.double	0x24f000+PG_RW+PG_V	# memory page
	.double	0x250000+PG_RW+PG_V	# memory page
	.double	0x251000+PG_RW+PG_V	# memory page
	.double	0x252000+PG_RW+PG_V	# memory page
	.double	0x253000+PG_RW+PG_V	# memory page
	.double	0x254000+PG_RW+PG_V	# memory page
	.double	0x255000+PG_RW+PG_V	# memory page
	.double	0x256000+PG_RW+PG_V	# memory page
	.double	0x257000+PG_RW+PG_V	# memory page
	.double	0x258000+PG_RW+PG_V	# memory page
	.double	0x259000+PG_RW+PG_V	# memory page
	.double	0x25a000+PG_RW+PG_V	# memory page
	.double	0x25b000+PG_RW+PG_V	# memory page
	.double	0x25c000+PG_RW+PG_V	# memory page
	.double	0x25d000+PG_RW+PG_V	# memory page
	.double	0x25e000+PG_RW+PG_V	# memory page
	.double	0x25f000+PG_RW+PG_V	# memory page
	.double	0x260000+PG_RW+PG_V	# memory page
	.double	0x261000+PG_RW+PG_V	# memory page
	.double	0x262000+PG_RW+PG_V	# memory page
	.double	0x263000+PG_RW+PG_V	# memory page
	.double	0x264000+PG_RW+PG_V	# memory page
	.double	0x265000+PG_RW+PG_V	# memory page
	.double	0x266000+PG_RW+PG_V	# memory page
	.double	0x267000+PG_RW+PG_V	# memory page
	.double	0x268000+PG_RW+PG_V	# memory page
	.double	0x269000+PG_RW+PG_V	# memory page
	.double	0x26a000+PG_RW+PG_V	# memory page
	.double	0x26b000+PG_RW+PG_V	# memory page
	.double	0x26c000+PG_RW+PG_V	# memory page
	.double	0x26d000+PG_RW+PG_V	# memory page
	.double	0x26e000+PG_RW+PG_V	# memory page
	.double	0x26f000+PG_RW+PG_V	# memory page
	.double	0x270000+PG_RW+PG_V	# memory page
	.double	0x271000+PG_RW+PG_V	# memory page
	.double	0x272000+PG_RW+PG_V	# memory page
	.double	0x273000+PG_RW+PG_V	# memory page
	.double	0x274000+PG_RW+PG_V	# memory page
	.double	0x275000+PG_RW+PG_V	# memory page
	.double	0x276000+PG_RW+PG_V	# memory page
	.double	0x277000+PG_RW+PG_V	# memory page
	.double	0x278000+PG_RW+PG_V	# memory page
	.double	0x279000+PG_RW+PG_V	# memory page
	.double	0x27a000+PG_RW+PG_V	# memory page
	.double	0x27b000+PG_RW+PG_V	# memory page
	.double	0x27c000+PG_RW+PG_V	# memory page
	.double	0x27d000+PG_RW+PG_V	# memory page
	.double	0x27e000+PG_RW+PG_V	# memory page
	.double	0x27f000+PG_RW+PG_V	# memory page
	.double	0x280000+PG_RW+PG_V	# memory page
	.double	0x281000+PG_RW+PG_V	# memory page
	.double	0x282000+PG_RW+PG_V	# memory page
	.double	0x283000+PG_RW+PG_V	# memory page
	.double	0x284000+PG_RW+PG_V	# memory page
	.double	0x285000+PG_RW+PG_V	# memory page
	.double	0x286000+PG_RW+PG_V	# memory page
	.double	0x287000+PG_RW+PG_V	# memory page
	.double	0x288000+PG_RW+PG_V	# memory page
	.double	0x289000+PG_RW+PG_V	# memory page
	.double	0x28a000+PG_RW+PG_V	# memory page
	.double	0x28b000+PG_RW+PG_V	# memory page
	.double	0x28c000+PG_RW+PG_V	# memory page
	.double	0x28d000+PG_RW+PG_V	# memory page
	.double	0x28e000+PG_RW+PG_V	# memory page
	.double	0x28f000+PG_RW+PG_V	# memory page
	.double	0x290000+PG_RW+PG_V	# memory page
	.double	0x291000+PG_RW+PG_V	# memory page
	.double	0x292000+PG_RW+PG_V	# memory page
	.double	0x293000+PG_RW+PG_V	# memory page
	.double	0x294000+PG_RW+PG_V	# memory page
	.double	0x295000+PG_RW+PG_V	# memory page
	.double	0x296000+PG_RW+PG_V	# memory page
	.double	0x297000+PG_RW+PG_V	# memory page
	.double	0x298000+PG_RW+PG_V	# memory page
	.double	0x299000+PG_RW+PG_V	# memory page
	.double	0x29a000+PG_RW+PG_V	# memory page
	.double	0x29b000+PG_RW+PG_V	# memory page
	.double	0x29c000+PG_RW+PG_V	# memory page
	.double	0x29d000+PG_RW+PG_V	# memory page
	.double	0x29e000+PG_RW+PG_V	# memory page
	.double	0x29f000+PG_RW+PG_V	# memory page
	.double	0x2a0000+PG_RW+PG_V	# memory page
	.double	0x2a1000+PG_RW+PG_V	# memory page
	.double	0x2a2000+PG_RW+PG_V	# memory page
	.double	0x2a3000+PG_RW+PG_V	# memory page
	.double	0x2a4000+PG_RW+PG_V	# memory page
	.double	0x2a5000+PG_RW+PG_V	# memory page
	.double	0x2a6000+PG_RW+PG_V	# memory page
	.double	0x2a7000+PG_RW+PG_V	# memory page
	.double	0x2a8000+PG_RW+PG_V	# memory page
	.double	0x2a9000+PG_RW+PG_V	# memory page
	.double	0x2aa000+PG_RW+PG_V	# memory page
	.double	0x2ab000+PG_RW+PG_V	# memory page
	.double	0x2ac000+PG_RW+PG_V	# memory page
	.double	0x2ad000+PG_RW+PG_V	# memory page
	.double	0x2ae000+PG_RW+PG_V	# memory page
	.double	0x2af000+PG_RW+PG_V	# memory page
	.double	0x2b0000+PG_RW+PG_V	# memory page
	.double	0x2b1000+PG_RW+PG_V	# memory page
	.double	0x2b2000+PG_RW+PG_V	# memory page
	.double	0x2b3000+PG_RW+PG_V	# memory page
	.double	0x2b4000+PG_RW+PG_V	# memory page
	.double	0x2b5000+PG_RW+PG_V	# memory page
	.double	0x2b6000+PG_RW+PG_V	# memory page
	.double	0x2b7000+PG_RW+PG_V	# memory page
	.double	0x2b8000+PG_RW+PG_V	# memory page
	.double	0x2b9000+PG_RW+PG_V	# memory page
	.double	0x2ba000+PG_RW+PG_V	# memory page
	.double	0x2bb000+PG_RW+PG_V	# memory page
	.double	0x2bc000+PG_RW+PG_V	# memory page
	.double	0x2bd000+PG_RW+PG_V	# memory page
	.double	0x2be000+PG_RW+PG_V	# memory page
	.double	0x2bf000+PG_RW+PG_V	# memory page
	.double	0x2c0000+PG_RW+PG_V	# memory page
	.double	0x2c1000+PG_RW+PG_V	# memory page
	.double	0x2c2000+PG_RW+PG_V	# memory page
	.double	0x2c3000+PG_RW+PG_V	# memory page
	.double	0x2c4000+PG_RW+PG_V	# memory page
	.double	0x2c5000+PG_RW+PG_V	# memory page
	.double	0x2c6000+PG_RW+PG_V	# memory page
	.double	0x2c7000+PG_RW+PG_V	# memory page
	.double	0x2c8000+PG_RW+PG_V	# memory page
	.double	0x2c9000+PG_RW+PG_V	# memory page
	.double	0x2ca000+PG_RW+PG_V	# memory page
	.double	0x2cb000+PG_RW+PG_V	# memory page
	.double	0x2cc000+PG_RW+PG_V	# memory page
	.double	0x2cd000+PG_RW+PG_V	# memory page
	.double	0x2ce000+PG_RW+PG_V	# memory page
	.double	0x2cf000+PG_RW+PG_V	# memory page
	.double	0x2d0000+PG_RW+PG_V	# memory page
	.double	0x2d1000+PG_RW+PG_V	# memory page
	.double	0x2d2000+PG_RW+PG_V	# memory page
	.double	0x2d3000+PG_RW+PG_V	# memory page
	.double	0x2d4000+PG_RW+PG_V	# memory page
	.double	0x2d5000+PG_RW+PG_V	# memory page
	.double	0x2d6000+PG_RW+PG_V	# memory page
	.double	0x2d7000+PG_RW+PG_V	# memory page
	.double	0x2d8000+PG_RW+PG_V	# memory page
	.double	0x2d9000+PG_RW+PG_V	# memory page
	.double	0x2da000+PG_RW+PG_V	# memory page
	.double	0x2db000+PG_RW+PG_V	# memory page
	.double	0x2dc000+PG_RW+PG_V	# memory page
	.double	0x2dd000+PG_RW+PG_V	# memory page
	.double	0x2de000+PG_RW+PG_V	# memory page
	.double	0x2df000+PG_RW+PG_V	# memory page
	.double	0x2e0000+PG_RW+PG_V	# memory page
	.double	0x2e1000+PG_RW+PG_V	# memory page
	.double	0x2e2000+PG_RW+PG_V	# memory page
	.double	0x2e3000+PG_RW+PG_V	# memory page
	.double	0x2e4000+PG_RW+PG_V	# memory page
	.double	0x2e5000+PG_RW+PG_V	# memory page
	.double	0x2e6000+PG_RW+PG_V	# memory page
	.double	0x2e7000+PG_RW+PG_V	# memory page
	.double	0x2e8000+PG_RW+PG_V	# memory page
	.double	0x2e9000+PG_RW+PG_V	# memory page
	.double	0x2ea000+PG_RW+PG_V	# memory page
	.double	0x2eb000+PG_RW+PG_V	# memory page
	.double	0x2ec000+PG_RW+PG_V	# memory page
	.double	0x2ed000+PG_RW+PG_V	# memory page
	.double	0x2ee000+PG_RW+PG_V	# memory page
	.double	0x2ef000+PG_RW+PG_V	# memory page
	.double	0x2f0000+PG_RW+PG_V	# memory page
	.double	0x2f1000+PG_RW+PG_V	# memory page
	.double	0x2f2000+PG_RW+PG_V	# memory page
	.double	0x2f3000+PG_RW+PG_V	# memory page
	.double	0x2f4000+PG_RW+PG_V	# memory page
	.double	0x2f5000+PG_RW+PG_V	# memory page
	.double	0x2f6000+PG_RW+PG_V	# memory page
	.double	0x2f7000+PG_RW+PG_V	# memory page
	.double	0x2f8000+PG_RW+PG_V	# memory page
	.double	0x2f9000+PG_RW+PG_V	# memory page
	.double	0x2fa000+PG_RW+PG_V	# memory page
	.double	0x2fb000+PG_RW+PG_V	# memory page
	.double	0x2fc000+PG_RW+PG_V	# memory page
	.double	0x2fd000+PG_RW+PG_V	# memory page
	.double	0x2fe000+PG_RW+PG_V	# memory page
	.double	0x2ff000+PG_RW+PG_V	# memory page
	.double	0x300000+PG_RW+PG_V	# memory page
	.double	0x301000+PG_RW+PG_V	# memory page
	.double	0x302000+PG_RW+PG_V	# memory page
	.double	0x303000+PG_RW+PG_V	# memory page
	.double	0x304000+PG_RW+PG_V	# memory page
	.double	0x305000+PG_RW+PG_V	# memory page
	.double	0x306000+PG_RW+PG_V	# memory page
	.double	0x307000+PG_RW+PG_V	# memory page
	.double	0x308000+PG_RW+PG_V	# memory page
	.double	0x309000+PG_RW+PG_V	# memory page
	.double	0x30a000+PG_RW+PG_V	# memory page
	.double	0x30b000+PG_RW+PG_V	# memory page
	.double	0x30c000+PG_RW+PG_V	# memory page
	.double	0x30d000+PG_RW+PG_V	# memory page
	.double	0x30e000+PG_RW+PG_V	# memory page
	.double	0x30f000+PG_RW+PG_V	# memory page
	.double	0x310000+PG_RW+PG_V	# memory page
	.double	0x311000+PG_RW+PG_V	# memory page
	.double	0x312000+PG_RW+PG_V	# memory page
	.double	0x313000+PG_RW+PG_V	# memory page
	.double	0x314000+PG_RW+PG_V	# memory page
	.double	0x315000+PG_RW+PG_V	# memory page
	.double	0x316000+PG_RW+PG_V	# memory page
	.double	0x317000+PG_RW+PG_V	# memory page
	.double	0x318000+PG_RW+PG_V	# memory page
	.double	0x319000+PG_RW+PG_V	# memory page
	.double	0x31a000+PG_RW+PG_V	# memory page
	.double	0x31b000+PG_RW+PG_V	# memory page
	.double	0x31c000+PG_RW+PG_V	# memory page
	.double	0x31d000+PG_RW+PG_V	# memory page
	.double	0x31e000+PG_RW+PG_V	# memory page
	.double	0x31f000+PG_RW+PG_V	# memory page
	.double	0x320000+PG_RW+PG_V	# memory page
	.double	0x321000+PG_RW+PG_V	# memory page
	.double	0x322000+PG_RW+PG_V	# memory page
	.double	0x323000+PG_RW+PG_V	# memory page
	.double	0x324000+PG_RW+PG_V	# memory page
	.double	0x325000+PG_RW+PG_V	# memory page
	.double	0x326000+PG_RW+PG_V	# memory page
	.double	0x327000+PG_RW+PG_V	# memory page
	.double	0x328000+PG_RW+PG_V	# memory page
	.double	0x329000+PG_RW+PG_V	# memory page
	.double	0x32a000+PG_RW+PG_V	# memory page
	.double	0x32b000+PG_RW+PG_V	# memory page
	.double	0x32c000+PG_RW+PG_V	# memory page
	.double	0x32d000+PG_RW+PG_V	# memory page
	.double	0x32e000+PG_RW+PG_V	# memory page
	.double	0x32f000+PG_RW+PG_V	# memory page
	.double	0x330000+PG_RW+PG_V	# memory page
	.double	0x331000+PG_RW+PG_V	# memory page
	.double	0x332000+PG_RW+PG_V	# memory page
	.double	0x333000+PG_RW+PG_V	# memory page
	.double	0x334000+PG_RW+PG_V	# memory page
	.double	0x335000+PG_RW+PG_V	# memory page
	.double	0x336000+PG_RW+PG_V	# memory page
	.double	0x337000+PG_RW+PG_V	# memory page
	.double	0x338000+PG_RW+PG_V	# memory page
	.double	0x339000+PG_RW+PG_V	# memory page
	.double	0x33a000+PG_RW+PG_V	# memory page
	.double	0x33b000+PG_RW+PG_V	# memory page
	.double	0x33c000+PG_RW+PG_V	# memory page
	.double	0x33d000+PG_RW+PG_V	# memory page
	.double	0x33e000+PG_RW+PG_V	# memory page
	.double	0x33f000+PG_RW+PG_V	# memory page
	.double	0x340000+PG_RW+PG_V	# memory page
	.double	0x341000+PG_RW+PG_V	# memory page
	.double	0x342000+PG_RW+PG_V	# memory page
	.double	0x343000+PG_RW+PG_V	# memory page
	.double	0x344000+PG_RW+PG_V	# memory page
	.double	0x345000+PG_RW+PG_V	# memory page
	.double	0x346000+PG_RW+PG_V	# memory page
	.double	0x347000+PG_RW+PG_V	# memory page
	.double	0x348000+PG_RW+PG_V	# memory page
	.double	0x349000+PG_RW+PG_V	# memory page
	.double	0x34a000+PG_RW+PG_V	# memory page
	.double	0x34b000+PG_RW+PG_V	# memory page
	.double	0x34c000+PG_RW+PG_V	# memory page
	.double	0x34d000+PG_RW+PG_V	# memory page
	.double	0x34e000+PG_RW+PG_V	# memory page
	.double	0x34f000+PG_RW+PG_V	# memory page
	.double	0x350000+PG_RW+PG_V	# memory page
	.double	0x351000+PG_RW+PG_V	# memory page
	.double	0x352000+PG_RW+PG_V	# memory page
	.double	0x353000+PG_RW+PG_V	# memory page
	.double	0x354000+PG_RW+PG_V	# memory page
	.double	0x355000+PG_RW+PG_V	# memory page
	.double	0x356000+PG_RW+PG_V	# memory page
	.double	0x357000+PG_RW+PG_V	# memory page
	.double	0x358000+PG_RW+PG_V	# memory page
	.double	0x359000+PG_RW+PG_V	# memory page
	.double	0x35a000+PG_RW+PG_V	# memory page
	.double	0x35b000+PG_RW+PG_V	# memory page
	.double	0x35c000+PG_RW+PG_V	# memory page
	.double	0x35d000+PG_RW+PG_V	# memory page
	.double	0x35e000+PG_RW+PG_V	# memory page
	.double	0x35f000+PG_RW+PG_V	# memory page
	.double	0x360000+PG_RW+PG_V	# memory page
	.double	0x361000+PG_RW+PG_V	# memory page
	.double	0x362000+PG_RW+PG_V	# memory page
	.double	0x363000+PG_RW+PG_V	# memory page
	.double	0x364000+PG_RW+PG_V	# memory page
	.double	0x365000+PG_RW+PG_V	# memory page
	.double	0x366000+PG_RW+PG_V	# memory page
	.double	0x367000+PG_RW+PG_V	# memory page
	.double	0x368000+PG_RW+PG_V	# memory page
	.double	0x369000+PG_RW+PG_V	# memory page
	.double	0x36a000+PG_RW+PG_V	# memory page
	.double	0x36b000+PG_RW+PG_V	# memory page
	.double	0x36c000+PG_RW+PG_V	# memory page
	.double	0x36d000+PG_RW+PG_V	# memory page
	.double	0x36e000+PG_RW+PG_V	# memory page
	.double	0x36f000+PG_RW+PG_V	# memory page
	.double	0x370000+PG_RW+PG_V	# memory page
	.double	0x371000+PG_RW+PG_V	# memory page
	.double	0x372000+PG_RW+PG_V	# memory page
	.double	0x373000+PG_RW+PG_V	# memory page
	.double	0x374000+PG_RW+PG_V	# memory page
	.double	0x375000+PG_RW+PG_V	# memory page
	.double	0x376000+PG_RW+PG_V	# memory page
	.double	0x377000+PG_RW+PG_V	# memory page
	.double	0x378000+PG_RW+PG_V	# memory page
	.double	0x379000+PG_RW+PG_V	# memory page
	.double	0x37a000+PG_RW+PG_V	# memory page
	.double	0x37b000+PG_RW+PG_V	# memory page
	.double	0x37c000+PG_RW+PG_V	# memory page
	.double	0x37d000+PG_RW+PG_V	# memory page
	.double	0x37e000+PG_RW+PG_V	# memory page
	.double	0x37f000+PG_RW+PG_V	# memory page
	.double	0x380000+PG_RW+PG_V	# memory page
	.double	0x381000+PG_RW+PG_V	# memory page
	.double	0x382000+PG_RW+PG_V	# memory page
	.double	0x383000+PG_RW+PG_V	# memory page
	.double	0x384000+PG_RW+PG_V	# memory page
	.double	0x385000+PG_RW+PG_V	# memory page
	.double	0x386000+PG_RW+PG_V	# memory page
	.double	0x387000+PG_RW+PG_V	# memory page
	.double	0x388000+PG_RW+PG_V	# memory page
	.double	0x389000+PG_RW+PG_V	# memory page
	.double	0x38a000+PG_RW+PG_V	# memory page
	.double	0x38b000+PG_RW+PG_V	# memory page
	.double	0x38c000+PG_RW+PG_V	# memory page
	.double	0x38d000+PG_RW+PG_V	# memory page
	.double	0x38e000+PG_RW+PG_V	# memory page
	.double	0x38f000+PG_RW+PG_V	# memory page
	.double	0x390000+PG_RW+PG_V	# memory page
	.double	0x391000+PG_RW+PG_V	# memory page
	.double	0x392000+PG_RW+PG_V	# memory page
	.double	0x393000+PG_RW+PG_V	# memory page
	.double	0x394000+PG_RW+PG_V	# memory page
	.double	0x395000+PG_RW+PG_V	# memory page
	.double	0x396000+PG_RW+PG_V	# memory page
	.double	0x397000+PG_RW+PG_V	# memory page
	.double	0x398000+PG_RW+PG_V	# memory page
	.double	0x399000+PG_RW+PG_V	# memory page
	.double	0x39a000+PG_RW+PG_V	# memory page
	.double	0x39b000+PG_RW+PG_V	# memory page
	.double	0x39c000+PG_RW+PG_V	# memory page
	.double	0x39d000+PG_RW+PG_V	# memory page
	.double	0x39e000+PG_RW+PG_V	# memory page
	.double	0x39f000+PG_RW+PG_V	# memory page
	.double	0x3a0000+PG_RW+PG_V	# memory page
	.double	0x3a1000+PG_RW+PG_V	# memory page
	.double	0x3a2000+PG_RW+PG_V	# memory page
	.double	0x3a3000+PG_RW+PG_V	# memory page
	.double	0x3a4000+PG_RW+PG_V	# memory page
	.double	0x3a5000+PG_RW+PG_V	# memory page
	.double	0x3a6000+PG_RW+PG_V	# memory page
	.double	0x3a7000+PG_RW+PG_V	# memory page
	.double	0x3a8000+PG_RW+PG_V	# memory page
	.double	0x3a9000+PG_RW+PG_V	# memory page
	.double	0x3aa000+PG_RW+PG_V	# memory page
	.double	0x3ab000+PG_RW+PG_V	# memory page
	.double	0x3ac000+PG_RW+PG_V	# memory page
	.double	0x3ad000+PG_RW+PG_V	# memory page
	.double	0x3ae000+PG_RW+PG_V	# memory page
	.double	0x3af000+PG_RW+PG_V	# memory page
	.double	0x3b0000+PG_RW+PG_V	# memory page
	.double	0x3b1000+PG_RW+PG_V	# memory page
	.double	0x3b2000+PG_RW+PG_V	# memory page
	.double	0x3b3000+PG_RW+PG_V	# memory page
	.double	0x3b4000+PG_RW+PG_V	# memory page
	.double	0x3b5000+PG_RW+PG_V	# memory page
	.double	0x3b6000+PG_RW+PG_V	# memory page
	.double	0x3b7000+PG_RW+PG_V	# memory page
	.double	0x3b8000+PG_RW+PG_V	# memory page
	.double	0x3b9000+PG_RW+PG_V	# memory page
	.double	0x3ba000+PG_RW+PG_V	# memory page
	.double	0x3bb000+PG_RW+PG_V	# memory page
	.double	0x3bc000+PG_RW+PG_V	# memory page
	.double	0x3bd000+PG_RW+PG_V	# memory page
	.double	0x3be000+PG_RW+PG_V	# memory page
	.double	0x3bf000+PG_RW+PG_V	# memory page
	.double	0x3c0000+PG_RW+PG_V	# memory page
	.double	0x3c1000+PG_RW+PG_V	# memory page
	.double	0x3c2000+PG_RW+PG_V	# memory page
	.double	0x3c3000+PG_RW+PG_V	# memory page
	.double	0x3c4000+PG_RW+PG_V	# memory page
	.double	0x3c5000+PG_RW+PG_V	# memory page
	.double	0x3c6000+PG_RW+PG_V	# memory page
	.double	0x3c7000+PG_RW+PG_V	# memory page
	.double	0x3c8000+PG_RW+PG_V	# memory page
	.double	0x3c9000+PG_RW+PG_V	# memory page
	.double	0x3ca000+PG_RW+PG_V	# memory page
	.double	0x3cb000+PG_RW+PG_V	# memory page
	.double	0x3cc000+PG_RW+PG_V	# memory page
	.double	0x3cd000+PG_RW+PG_V	# memory page
	.double	0x3ce000+PG_RW+PG_V	# memory page
	.double	0x3cf000+PG_RW+PG_V	# memory page
	.double	0x3d0000+PG_RW+PG_V	# memory page
	.double	0x3d1000+PG_RW+PG_V	# memory page
	.double	0x3d2000+PG_RW+PG_V	# memory page
	.double	0x3d3000+PG_RW+PG_V	# memory page
	.double	0x3d4000+PG_RW+PG_V	# memory page
	.double	0x3d5000+PG_RW+PG_V	# memory page
	.double	0x3d6000+PG_RW+PG_V	# memory page
	.double	0x3d7000+PG_RW+PG_V	# memory page
	.double	0x3d8000+PG_RW+PG_V	# memory page
	.double	0x3d9000+PG_RW+PG_V	# memory page
	.double	0x3da000+PG_RW+PG_V	# memory page
	.double	0x3db000+PG_RW+PG_V	# memory page
	.double	0x3dc000+PG_RW+PG_V	# memory page
	.double	0x3dd000+PG_RW+PG_V	# memory page
	.double	0x3de000+PG_RW+PG_V	# memory page
	.double	0x3df000+PG_RW+PG_V	# memory page
	.double	0x3e0000+PG_RW+PG_V	# memory page
	.double	0x3e1000+PG_RW+PG_V	# memory page
	.double	0x3e2000+PG_RW+PG_V	# memory page
	.double	0x3e3000+PG_RW+PG_V	# memory page
	.double	0x3e4000+PG_RW+PG_V	# memory page
	.double	0x3e5000+PG_RW+PG_V	# memory page
	.double	0x3e6000+PG_RW+PG_V	# memory page
	.double	0x3e7000+PG_RW+PG_V	# memory page
	.double	0x3e8000+PG_RW+PG_V	# memory page
	.double	0x3e9000+PG_RW+PG_V	# memory page
	.double	0x3ea000+PG_RW+PG_V	# memory page
	.double	0x3eb000+PG_RW+PG_V	# memory page
	.double	0x3ec000+PG_RW+PG_V	# memory page
	.double	0x3ed000+PG_RW+PG_V	# memory page
	.double	0x3ee000+PG_RW+PG_V	# memory page
	.double	0x3ef000+PG_RW+PG_V	# memory page
	.double	0x3f0000+PG_RW+PG_V	# memory page
	.double	0x3f1000+PG_RW+PG_V	# memory page
	.double	0x3f2000+PG_RW+PG_V	# memory page
	.double	0x3f3000+PG_RW+PG_V	# memory page
	.double	0x3f4000+PG_RW+PG_V	# memory page
	.double	0x3f5000+PG_RW+PG_V	# memory page
	.double	0x3f6000+PG_RW+PG_V	# memory page
	.double	0x3f7000+PG_RW+PG_V	# memory page
	.double	0x3f8000+PG_RW+PG_V	# memory page
	.double	0x3f9000+PG_RW+PG_V	# memory page
	.double	0x3fa000+PG_RW+PG_V	# memory page
	.double	0x3fb000+PG_RW+PG_V	# memory page
	.double	0x3fc000+PG_RW+PG_V	# memory page
	.double	0x3fd000+PG_RW+PG_V	# memory page
	.double	0x3fe000+PG_RW+PG_V	# memory page
	.double	0x3ff000+PG_RW+PG_V	# memory page

# PAGE 5
# 2nd level page table entries (4MB-8MB)
paget1:
	.double	0x400000+PG_RW+PG_V	# memory page
	.double	0x401000+PG_RW+PG_V	# memory page
	.double	0x402000+PG_RW+PG_V	# memory page
	.double	0x403000+PG_RW+PG_V	# memory page
	.double	0x404000+PG_RW+PG_V	# memory page
	.double	0x405000+PG_RW+PG_V	# memory page
	.double	0x406000+PG_RW+PG_V	# memory page
	.double	0x407000+PG_RW+PG_V	# memory page
	.double	0x408000+PG_RW+PG_V	# memory page
	.double	0x409000+PG_RW+PG_V	# memory page
	.double	0x40a000+PG_RW+PG_V	# memory page
	.double	0x40b000+PG_RW+PG_V	# memory page
	.double	0x40c000+PG_RW+PG_V	# memory page
	.double	0x40d000+PG_RW+PG_V	# memory page
	.double	0x40e000+PG_RW+PG_V	# memory page
	.double	0x40f000+PG_RW+PG_V	# memory page
	.double	0x410000+PG_RW+PG_V	# memory page
	.double	0x411000+PG_RW+PG_V	# memory page
	.double	0x412000+PG_RW+PG_V	# memory page
	.double	0x413000+PG_RW+PG_V	# memory page
	.double	0x414000+PG_RW+PG_V	# memory page
	.double	0x415000+PG_RW+PG_V	# memory page
	.double	0x416000+PG_RW+PG_V	# memory page
	.double	0x417000+PG_RW+PG_V	# memory page
	.double	0x418000+PG_RW+PG_V	# memory page
	.double	0x419000+PG_RW+PG_V	# memory page
	.double	0x41a000+PG_RW+PG_V	# memory page
	.double	0x41b000+PG_RW+PG_V	# memory page
	.double	0x41c000+PG_RW+PG_V	# memory page
	.double	0x41d000+PG_RW+PG_V	# memory page
	.double	0x41e000+PG_RW+PG_V	# memory page
	.double	0x41f000+PG_RW+PG_V	# memory page
	.double	0x420000+PG_RW+PG_V	# memory page
	.double	0x421000+PG_RW+PG_V	# memory page
	.double	0x422000+PG_RW+PG_V	# memory page
	.double	0x423000+PG_RW+PG_V	# memory page
	.double	0x424000+PG_RW+PG_V	# memory page
	.double	0x425000+PG_RW+PG_V	# memory page
	.double	0x426000+PG_RW+PG_V	# memory page
	.double	0x427000+PG_RW+PG_V	# memory page
	.double	0x428000+PG_RW+PG_V	# memory page
	.double	0x429000+PG_RW+PG_V	# memory page
	.double	0x42a000+PG_RW+PG_V	# memory page
	.double	0x42b000+PG_RW+PG_V	# memory page
	.double	0x42c000+PG_RW+PG_V	# memory page
	.double	0x42d000+PG_RW+PG_V	# memory page
	.double	0x42e000+PG_RW+PG_V	# memory page
	.double	0x42f000+PG_RW+PG_V	# memory page
	.double	0x430000+PG_RW+PG_V	# memory page
	.double	0x431000+PG_RW+PG_V	# memory page
	.double	0x432000+PG_RW+PG_V	# memory page
	.double	0x433000+PG_RW+PG_V	# memory page
	.double	0x434000+PG_RW+PG_V	# memory page
	.double	0x435000+PG_RW+PG_V	# memory page
	.double	0x436000+PG_RW+PG_V	# memory page
	.double	0x437000+PG_RW+PG_V	# memory page
	.double	0x438000+PG_RW+PG_V	# memory page
	.double	0x439000+PG_RW+PG_V	# memory page
	.double	0x43a000+PG_RW+PG_V	# memory page
	.double	0x43b000+PG_RW+PG_V	# memory page
	.double	0x43c000+PG_RW+PG_V	# memory page
	.double	0x43d000+PG_RW+PG_V	# memory page
	.double	0x43e000+PG_RW+PG_V	# memory page
	.double	0x43f000+PG_RW+PG_V	# memory page
	.double	0x440000+PG_RW+PG_V	# memory page
	.double	0x441000+PG_RW+PG_V	# memory page
	.double	0x442000+PG_RW+PG_V	# memory page
	.double	0x443000+PG_RW+PG_V	# memory page
	.double	0x444000+PG_RW+PG_V	# memory page
	.double	0x445000+PG_RW+PG_V	# memory page
	.double	0x446000+PG_RW+PG_V	# memory page
	.double	0x447000+PG_RW+PG_V	# memory page
	.double	0x448000+PG_RW+PG_V	# memory page
	.double	0x449000+PG_RW+PG_V	# memory page
	.double	0x44a000+PG_RW+PG_V	# memory page
	.double	0x44b000+PG_RW+PG_V	# memory page
	.double	0x44c000+PG_RW+PG_V	# memory page
	.double	0x44d000+PG_RW+PG_V	# memory page
	.double	0x44e000+PG_RW+PG_V	# memory page
	.double	0x44f000+PG_RW+PG_V	# memory page
	.double	0x450000+PG_RW+PG_V	# memory page
	.double	0x451000+PG_RW+PG_V	# memory page
	.double	0x452000+PG_RW+PG_V	# memory page
	.double	0x453000+PG_RW+PG_V	# memory page
	.double	0x454000+PG_RW+PG_V	# memory page
	.double	0x455000+PG_RW+PG_V	# memory page
	.double	0x456000+PG_RW+PG_V	# memory page
	.double	0x457000+PG_RW+PG_V	# memory page
	.double	0x458000+PG_RW+PG_V	# memory page
	.double	0x459000+PG_RW+PG_V	# memory page
	.double	0x45a000+PG_RW+PG_V	# memory page
	.double	0x45b000+PG_RW+PG_V	# memory page
	.double	0x45c000+PG_RW+PG_V	# memory page
	.double	0x45d000+PG_RW+PG_V	# memory page
	.double	0x45e000+PG_RW+PG_V	# memory page
	.double	0x45f000+PG_RW+PG_V	# memory page
	.double	0x460000+PG_RW+PG_V	# memory page
	.double	0x461000+PG_RW+PG_V	# memory page
	.double	0x462000+PG_RW+PG_V	# memory page
	.double	0x463000+PG_RW+PG_V	# memory page
	.double	0x464000+PG_RW+PG_V	# memory page
	.double	0x465000+PG_RW+PG_V	# memory page
	.double	0x466000+PG_RW+PG_V	# memory page
	.double	0x467000+PG_RW+PG_V	# memory page
	.double	0x468000+PG_RW+PG_V	# memory page
	.double	0x469000+PG_RW+PG_V	# memory page
	.double	0x46a000+PG_RW+PG_V	# memory page
	.double	0x46b000+PG_RW+PG_V	# memory page
	.double	0x46c000+PG_RW+PG_V	# memory page
	.double	0x46d000+PG_RW+PG_V	# memory page
	.double	0x46e000+PG_RW+PG_V	# memory page
	.double	0x46f000+PG_RW+PG_V	# memory page
	.double	0x470000+PG_RW+PG_V	# memory page
	.double	0x471000+PG_RW+PG_V	# memory page
	.double	0x472000+PG_RW+PG_V	# memory page
	.double	0x473000+PG_RW+PG_V	# memory page
	.double	0x474000+PG_RW+PG_V	# memory page
	.double	0x475000+PG_RW+PG_V	# memory page
	.double	0x476000+PG_RW+PG_V	# memory page
	.double	0x477000+PG_RW+PG_V	# memory page
	.double	0x478000+PG_RW+PG_V	# memory page
	.double	0x479000+PG_RW+PG_V	# memory page
	.double	0x47a000+PG_RW+PG_V	# memory page
	.double	0x47b000+PG_RW+PG_V	# memory page
	.double	0x47c000+PG_RW+PG_V	# memory page
	.double	0x47d000+PG_RW+PG_V	# memory page
	.double	0x47e000+PG_RW+PG_V	# memory page
	.double	0x47f000+PG_RW+PG_V	# memory page
	.double	0x480000+PG_RW+PG_V	# memory page
	.double	0x481000+PG_RW+PG_V	# memory page
	.double	0x482000+PG_RW+PG_V	# memory page
	.double	0x483000+PG_RW+PG_V	# memory page
	.double	0x484000+PG_RW+PG_V	# memory page
	.double	0x485000+PG_RW+PG_V	# memory page
	.double	0x486000+PG_RW+PG_V	# memory page
	.double	0x487000+PG_RW+PG_V	# memory page
	.double	0x488000+PG_RW+PG_V	# memory page
	.double	0x489000+PG_RW+PG_V	# memory page
	.double	0x48a000+PG_RW+PG_V	# memory page
	.double	0x48b000+PG_RW+PG_V	# memory page
	.double	0x48c000+PG_RW+PG_V	# memory page
	.double	0x48d000+PG_RW+PG_V	# memory page
	.double	0x48e000+PG_RW+PG_V	# memory page
	.double	0x48f000+PG_RW+PG_V	# memory page
	.double	0x490000+PG_RW+PG_V	# memory page
	.double	0x491000+PG_RW+PG_V	# memory page
	.double	0x492000+PG_RW+PG_V	# memory page
	.double	0x493000+PG_RW+PG_V	# memory page
	.double	0x494000+PG_RW+PG_V	# memory page
	.double	0x495000+PG_RW+PG_V	# memory page
	.double	0x496000+PG_RW+PG_V	# memory page
	.double	0x497000+PG_RW+PG_V	# memory page
	.double	0x498000+PG_RW+PG_V	# memory page
	.double	0x499000+PG_RW+PG_V	# memory page
	.double	0x49a000+PG_RW+PG_V	# memory page
	.double	0x49b000+PG_RW+PG_V	# memory page
	.double	0x49c000+PG_RW+PG_V	# memory page
	.double	0x49d000+PG_RW+PG_V	# memory page
	.double	0x49e000+PG_RW+PG_V	# memory page
	.double	0x49f000+PG_RW+PG_V	# memory page
	.double	0x4a0000+PG_RW+PG_V	# memory page
	.double	0x4a1000+PG_RW+PG_V	# memory page
	.double	0x4a2000+PG_RW+PG_V	# memory page
	.double	0x4a3000+PG_RW+PG_V	# memory page
	.double	0x4a4000+PG_RW+PG_V	# memory page
	.double	0x4a5000+PG_RW+PG_V	# memory page
	.double	0x4a6000+PG_RW+PG_V	# memory page
	.double	0x4a7000+PG_RW+PG_V	# memory page
	.double	0x4a8000+PG_RW+PG_V	# memory page
	.double	0x4a9000+PG_RW+PG_V	# memory page
	.double	0x4aa000+PG_RW+PG_V	# memory page
	.double	0x4ab000+PG_RW+PG_V	# memory page
	.double	0x4ac000+PG_RW+PG_V	# memory page
	.double	0x4ad000+PG_RW+PG_V	# memory page
	.double	0x4ae000+PG_RW+PG_V	# memory page
	.double	0x4af000+PG_RW+PG_V	# memory page
	.double	0x4b0000+PG_RW+PG_V	# memory page
	.double	0x4b1000+PG_RW+PG_V	# memory page
	.double	0x4b2000+PG_RW+PG_V	# memory page
	.double	0x4b3000+PG_RW+PG_V	# memory page
	.double	0x4b4000+PG_RW+PG_V	# memory page
	.double	0x4b5000+PG_RW+PG_V	# memory page
	.double	0x4b6000+PG_RW+PG_V	# memory page
	.double	0x4b7000+PG_RW+PG_V	# memory page
	.double	0x4b8000+PG_RW+PG_V	# memory page
	.double	0x4b9000+PG_RW+PG_V	# memory page
	.double	0x4ba000+PG_RW+PG_V	# memory page
	.double	0x4bb000+PG_RW+PG_V	# memory page
	.double	0x4bc000+PG_RW+PG_V	# memory page
	.double	0x4bd000+PG_RW+PG_V	# memory page
	.double	0x4be000+PG_RW+PG_V	# memory page
	.double	0x4bf000+PG_RW+PG_V	# memory page
	.double	0x4c0000+PG_RW+PG_V	# memory page
	.double	0x4c1000+PG_RW+PG_V	# memory page
	.double	0x4c2000+PG_RW+PG_V	# memory page
	.double	0x4c3000+PG_RW+PG_V	# memory page
	.double	0x4c4000+PG_RW+PG_V	# memory page
	.double	0x4c5000+PG_RW+PG_V	# memory page
	.double	0x4c6000+PG_RW+PG_V	# memory page
	.double	0x4c7000+PG_RW+PG_V	# memory page
	.double	0x4c8000+PG_RW+PG_V	# memory page
	.double	0x4c9000+PG_RW+PG_V	# memory page
	.double	0x4ca000+PG_RW+PG_V	# memory page
	.double	0x4cb000+PG_RW+PG_V	# memory page
	.double	0x4cc000+PG_RW+PG_V	# memory page
	.double	0x4cd000+PG_RW+PG_V	# memory page
	.double	0x4ce000+PG_RW+PG_V	# memory page
	.double	0x4cf000+PG_RW+PG_V	# memory page
	.double	0x4d0000+PG_RW+PG_V	# memory page
	.double	0x4d1000+PG_RW+PG_V	# memory page
	.double	0x4d2000+PG_RW+PG_V	# memory page
	.double	0x4d3000+PG_RW+PG_V	# memory page
	.double	0x4d4000+PG_RW+PG_V	# memory page
	.double	0x4d5000+PG_RW+PG_V	# memory page
	.double	0x4d6000+PG_RW+PG_V	# memory page
	.double	0x4d7000+PG_RW+PG_V	# memory page
	.double	0x4d8000+PG_RW+PG_V	# memory page
	.double	0x4d9000+PG_RW+PG_V	# memory page
	.double	0x4da000+PG_RW+PG_V	# memory page
	.double	0x4db000+PG_RW+PG_V	# memory page
	.double	0x4dc000+PG_RW+PG_V	# memory page
	.double	0x4dd000+PG_RW+PG_V	# memory page
	.double	0x4de000+PG_RW+PG_V	# memory page
	.double	0x4df000+PG_RW+PG_V	# memory page
	.double	0x4e0000+PG_RW+PG_V	# memory page
	.double	0x4e1000+PG_RW+PG_V	# memory page
	.double	0x4e2000+PG_RW+PG_V	# memory page
	.double	0x4e3000+PG_RW+PG_V	# memory page
	.double	0x4e4000+PG_RW+PG_V	# memory page
	.double	0x4e5000+PG_RW+PG_V	# memory page
	.double	0x4e6000+PG_RW+PG_V	# memory page
	.double	0x4e7000+PG_RW+PG_V	# memory page
	.double	0x4e8000+PG_RW+PG_V	# memory page
	.double	0x4e9000+PG_RW+PG_V	# memory page
	.double	0x4ea000+PG_RW+PG_V	# memory page
	.double	0x4eb000+PG_RW+PG_V	# memory page
	.double	0x4ec000+PG_RW+PG_V	# memory page
	.double	0x4ed000+PG_RW+PG_V	# memory page
	.double	0x4ee000+PG_RW+PG_V	# memory page
	.double	0x4ef000+PG_RW+PG_V	# memory page
	.double	0x4f0000+PG_RW+PG_V	# memory page
	.double	0x4f1000+PG_RW+PG_V	# memory page
	.double	0x4f2000+PG_RW+PG_V	# memory page
	.double	0x4f3000+PG_RW+PG_V	# memory page
	.double	0x4f4000+PG_RW+PG_V	# memory page
	.double	0x4f5000+PG_RW+PG_V	# memory page
	.double	0x4f6000+PG_RW+PG_V	# memory page
	.double	0x4f7000+PG_RW+PG_V	# memory page
	.double	0x4f8000+PG_RW+PG_V	# memory page
	.double	0x4f9000+PG_RW+PG_V	# memory page
	.double	0x4fa000+PG_RW+PG_V	# memory page
	.double	0x4fb000+PG_RW+PG_V	# memory page
	.double	0x4fc000+PG_RW+PG_V	# memory page
	.double	0x4fd000+PG_RW+PG_V	# memory page
	.double	0x4fe000+PG_RW+PG_V	# memory page
	.double	0x4ff000+PG_RW+PG_V	# memory page
	.double	0x500000+PG_RW+PG_V	# memory page
	.double	0x501000+PG_RW+PG_V	# memory page
	.double	0x502000+PG_RW+PG_V	# memory page
	.double	0x503000+PG_RW+PG_V	# memory page
	.double	0x504000+PG_RW+PG_V	# memory page
	.double	0x505000+PG_RW+PG_V	# memory page
	.double	0x506000+PG_RW+PG_V	# memory page
	.double	0x507000+PG_RW+PG_V	# memory page
	.double	0x508000+PG_RW+PG_V	# memory page
	.double	0x509000+PG_RW+PG_V	# memory page
	.double	0x50a000+PG_RW+PG_V	# memory page
	.double	0x50b000+PG_RW+PG_V	# memory page
	.double	0x50c000+PG_RW+PG_V	# memory page
	.double	0x50d000+PG_RW+PG_V	# memory page
	.double	0x50e000+PG_RW+PG_V	# memory page
	.double	0x50f000+PG_RW+PG_V	# memory page
	.double	0x510000+PG_RW+PG_V	# memory page
	.double	0x511000+PG_RW+PG_V	# memory page
	.double	0x512000+PG_RW+PG_V	# memory page
	.double	0x513000+PG_RW+PG_V	# memory page
	.double	0x514000+PG_RW+PG_V	# memory page
	.double	0x515000+PG_RW+PG_V	# memory page
	.double	0x516000+PG_RW+PG_V	# memory page
	.double	0x517000+PG_RW+PG_V	# memory page
	.double	0x518000+PG_RW+PG_V	# memory page
	.double	0x519000+PG_RW+PG_V	# memory page
	.double	0x51a000+PG_RW+PG_V	# memory page
	.double	0x51b000+PG_RW+PG_V	# memory page
	.double	0x51c000+PG_RW+PG_V	# memory page
	.double	0x51d000+PG_RW+PG_V	# memory page
	.double	0x51e000+PG_RW+PG_V	# memory page
	.double	0x51f000+PG_RW+PG_V	# memory page
	.double	0x520000+PG_RW+PG_V	# memory page
	.double	0x521000+PG_RW+PG_V	# memory page
	.double	0x522000+PG_RW+PG_V	# memory page
	.double	0x523000+PG_RW+PG_V	# memory page
	.double	0x524000+PG_RW+PG_V	# memory page
	.double	0x525000+PG_RW+PG_V	# memory page
	.double	0x526000+PG_RW+PG_V	# memory page
	.double	0x527000+PG_RW+PG_V	# memory page
	.double	0x528000+PG_RW+PG_V	# memory page
	.double	0x529000+PG_RW+PG_V	# memory page
	.double	0x52a000+PG_RW+PG_V	# memory page
	.double	0x52b000+PG_RW+PG_V	# memory page
	.double	0x52c000+PG_RW+PG_V	# memory page
	.double	0x52d000+PG_RW+PG_V	# memory page
	.double	0x52e000+PG_RW+PG_V	# memory page
	.double	0x52f000+PG_RW+PG_V	# memory page
	.double	0x530000+PG_RW+PG_V	# memory page
	.double	0x531000+PG_RW+PG_V	# memory page
	.double	0x532000+PG_RW+PG_V	# memory page
	.double	0x533000+PG_RW+PG_V	# memory page
	.double	0x534000+PG_RW+PG_V	# memory page
	.double	0x535000+PG_RW+PG_V	# memory page
	.double	0x536000+PG_RW+PG_V	# memory page
	.double	0x537000+PG_RW+PG_V	# memory page
	.double	0x538000+PG_RW+PG_V	# memory page
	.double	0x539000+PG_RW+PG_V	# memory page
	.double	0x53a000+PG_RW+PG_V	# memory page
	.double	0x53b000+PG_RW+PG_V	# memory page
	.double	0x53c000+PG_RW+PG_V	# memory page
	.double	0x53d000+PG_RW+PG_V	# memory page
	.double	0x53e000+PG_RW+PG_V	# memory page
	.double	0x53f000+PG_RW+PG_V	# memory page
	.double	0x540000+PG_RW+PG_V	# memory page
	.double	0x541000+PG_RW+PG_V	# memory page
	.double	0x542000+PG_RW+PG_V	# memory page
	.double	0x543000+PG_RW+PG_V	# memory page
	.double	0x544000+PG_RW+PG_V	# memory page
	.double	0x545000+PG_RW+PG_V	# memory page
	.double	0x546000+PG_RW+PG_V	# memory page
	.double	0x547000+PG_RW+PG_V	# memory page
	.double	0x548000+PG_RW+PG_V	# memory page
	.double	0x549000+PG_RW+PG_V	# memory page
	.double	0x54a000+PG_RW+PG_V	# memory page
	.double	0x54b000+PG_RW+PG_V	# memory page
	.double	0x54c000+PG_RW+PG_V	# memory page
	.double	0x54d000+PG_RW+PG_V	# memory page
	.double	0x54e000+PG_RW+PG_V	# memory page
	.double	0x54f000+PG_RW+PG_V	# memory page
	.double	0x550000+PG_RW+PG_V	# memory page
	.double	0x551000+PG_RW+PG_V	# memory page
	.double	0x552000+PG_RW+PG_V	# memory page
	.double	0x553000+PG_RW+PG_V	# memory page
	.double	0x554000+PG_RW+PG_V	# memory page
	.double	0x555000+PG_RW+PG_V	# memory page
	.double	0x556000+PG_RW+PG_V	# memory page
	.double	0x557000+PG_RW+PG_V	# memory page
	.double	0x558000+PG_RW+PG_V	# memory page
	.double	0x559000+PG_RW+PG_V	# memory page
	.double	0x55a000+PG_RW+PG_V	# memory page
	.double	0x55b000+PG_RW+PG_V	# memory page
	.double	0x55c000+PG_RW+PG_V	# memory page
	.double	0x55d000+PG_RW+PG_V	# memory page
	.double	0x55e000+PG_RW+PG_V	# memory page
	.double	0x55f000+PG_RW+PG_V	# memory page
	.double	0x560000+PG_RW+PG_V	# memory page
	.double	0x561000+PG_RW+PG_V	# memory page
	.double	0x562000+PG_RW+PG_V	# memory page
	.double	0x563000+PG_RW+PG_V	# memory page
	.double	0x564000+PG_RW+PG_V	# memory page
	.double	0x565000+PG_RW+PG_V	# memory page
	.double	0x566000+PG_RW+PG_V	# memory page
	.double	0x567000+PG_RW+PG_V	# memory page
	.double	0x568000+PG_RW+PG_V	# memory page
	.double	0x569000+PG_RW+PG_V	# memory page
	.double	0x56a000+PG_RW+PG_V	# memory page
	.double	0x56b000+PG_RW+PG_V	# memory page
	.double	0x56c000+PG_RW+PG_V	# memory page
	.double	0x56d000+PG_RW+PG_V	# memory page
	.double	0x56e000+PG_RW+PG_V	# memory page
	.double	0x56f000+PG_RW+PG_V	# memory page
	.double	0x570000+PG_RW+PG_V	# memory page
	.double	0x571000+PG_RW+PG_V	# memory page
	.double	0x572000+PG_RW+PG_V	# memory page
	.double	0x573000+PG_RW+PG_V	# memory page
	.double	0x574000+PG_RW+PG_V	# memory page
	.double	0x575000+PG_RW+PG_V	# memory page
	.double	0x576000+PG_RW+PG_V	# memory page
	.double	0x577000+PG_RW+PG_V	# memory page
	.double	0x578000+PG_RW+PG_V	# memory page
	.double	0x579000+PG_RW+PG_V	# memory page
	.double	0x57a000+PG_RW+PG_V	# memory page
	.double	0x57b000+PG_RW+PG_V	# memory page
	.double	0x57c000+PG_RW+PG_V	# memory page
	.double	0x57d000+PG_RW+PG_V	# memory page
	.double	0x57e000+PG_RW+PG_V	# memory page
	.double	0x57f000+PG_RW+PG_V	# memory page
	.double	0x580000+PG_RW+PG_V	# memory page
	.double	0x581000+PG_RW+PG_V	# memory page
	.double	0x582000+PG_RW+PG_V	# memory page
	.double	0x583000+PG_RW+PG_V	# memory page
	.double	0x584000+PG_RW+PG_V	# memory page
	.double	0x585000+PG_RW+PG_V	# memory page
	.double	0x586000+PG_RW+PG_V	# memory page
	.double	0x587000+PG_RW+PG_V	# memory page
	.double	0x588000+PG_RW+PG_V	# memory page
	.double	0x589000+PG_RW+PG_V	# memory page
	.double	0x58a000+PG_RW+PG_V	# memory page
	.double	0x58b000+PG_RW+PG_V	# memory page
	.double	0x58c000+PG_RW+PG_V	# memory page
	.double	0x58d000+PG_RW+PG_V	# memory page
	.double	0x58e000+PG_RW+PG_V	# memory page
	.double	0x58f000+PG_RW+PG_V	# memory page
	.double	0x590000+PG_RW+PG_V	# memory page
	.double	0x591000+PG_RW+PG_V	# memory page
	.double	0x592000+PG_RW+PG_V	# memory page
	.double	0x593000+PG_RW+PG_V	# memory page
	.double	0x594000+PG_RW+PG_V	# memory page
	.double	0x595000+PG_RW+PG_V	# memory page
	.double	0x596000+PG_RW+PG_V	# memory page
	.double	0x597000+PG_RW+PG_V	# memory page
	.double	0x598000+PG_RW+PG_V	# memory page
	.double	0x599000+PG_RW+PG_V	# memory page
	.double	0x59a000+PG_RW+PG_V	# memory page
	.double	0x59b000+PG_RW+PG_V	# memory page
	.double	0x59c000+PG_RW+PG_V	# memory page
	.double	0x59d000+PG_RW+PG_V	# memory page
	.double	0x59e000+PG_RW+PG_V	# memory page
	.double	0x59f000+PG_RW+PG_V	# memory page
	.double	0x5a0000+PG_RW+PG_V	# memory page
	.double	0x5a1000+PG_RW+PG_V	# memory page
	.double	0x5a2000+PG_RW+PG_V	# memory page
	.double	0x5a3000+PG_RW+PG_V	# memory page
	.double	0x5a4000+PG_RW+PG_V	# memory page
	.double	0x5a5000+PG_RW+PG_V	# memory page
	.double	0x5a6000+PG_RW+PG_V	# memory page
	.double	0x5a7000+PG_RW+PG_V	# memory page
	.double	0x5a8000+PG_RW+PG_V	# memory page
	.double	0x5a9000+PG_RW+PG_V	# memory page
	.double	0x5aa000+PG_RW+PG_V	# memory page
	.double	0x5ab000+PG_RW+PG_V	# memory page
	.double	0x5ac000+PG_RW+PG_V	# memory page
	.double	0x5ad000+PG_RW+PG_V	# memory page
	.double	0x5ae000+PG_RW+PG_V	# memory page
	.double	0x5af000+PG_RW+PG_V	# memory page
	.double	0x5b0000+PG_RW+PG_V	# memory page
	.double	0x5b1000+PG_RW+PG_V	# memory page
	.double	0x5b2000+PG_RW+PG_V	# memory page
	.double	0x5b3000+PG_RW+PG_V	# memory page
	.double	0x5b4000+PG_RW+PG_V	# memory page
	.double	0x5b5000+PG_RW+PG_V	# memory page
	.double	0x5b6000+PG_RW+PG_V	# memory page
	.double	0x5b7000+PG_RW+PG_V	# memory page
	.double	0x5b8000+PG_RW+PG_V	# memory page
	.double	0x5b9000+PG_RW+PG_V	# memory page
	.double	0x5ba000+PG_RW+PG_V	# memory page
	.double	0x5bb000+PG_RW+PG_V	# memory page
	.double	0x5bc000+PG_RW+PG_V	# memory page
	.double	0x5bd000+PG_RW+PG_V	# memory page
	.double	0x5be000+PG_RW+PG_V	# memory page
	.double	0x5bf000+PG_RW+PG_V	# memory page
	.double	0x5c0000+PG_RW+PG_V	# memory page
	.double	0x5c1000+PG_RW+PG_V	# memory page
	.double	0x5c2000+PG_RW+PG_V	# memory page
	.double	0x5c3000+PG_RW+PG_V	# memory page
	.double	0x5c4000+PG_RW+PG_V	# memory page
	.double	0x5c5000+PG_RW+PG_V	# memory page
	.double	0x5c6000+PG_RW+PG_V	# memory page
	.double	0x5c7000+PG_RW+PG_V	# memory page
	.double	0x5c8000+PG_RW+PG_V	# memory page
	.double	0x5c9000+PG_RW+PG_V	# memory page
	.double	0x5ca000+PG_RW+PG_V	# memory page
	.double	0x5cb000+PG_RW+PG_V	# memory page
	.double	0x5cc000+PG_RW+PG_V	# memory page
	.double	0x5cd000+PG_RW+PG_V	# memory page
	.double	0x5ce000+PG_RW+PG_V	# memory page
	.double	0x5cf000+PG_RW+PG_V	# memory page
	.double	0x5d0000+PG_RW+PG_V	# memory page
	.double	0x5d1000+PG_RW+PG_V	# memory page
	.double	0x5d2000+PG_RW+PG_V	# memory page
	.double	0x5d3000+PG_RW+PG_V	# memory page
	.double	0x5d4000+PG_RW+PG_V	# memory page
	.double	0x5d5000+PG_RW+PG_V	# memory page
	.double	0x5d6000+PG_RW+PG_V	# memory page
	.double	0x5d7000+PG_RW+PG_V	# memory page
	.double	0x5d8000+PG_RW+PG_V	# memory page
	.double	0x5d9000+PG_RW+PG_V	# memory page
	.double	0x5da000+PG_RW+PG_V	# memory page
	.double	0x5db000+PG_RW+PG_V	# memory page
	.double	0x5dc000+PG_RW+PG_V	# memory page
	.double	0x5dd000+PG_RW+PG_V	# memory page
	.double	0x5de000+PG_RW+PG_V	# memory page
	.double	0x5df000+PG_RW+PG_V	# memory page
	.double	0x5e0000+PG_RW+PG_V	# memory page
	.double	0x5e1000+PG_RW+PG_V	# memory page
	.double	0x5e2000+PG_RW+PG_V	# memory page
	.double	0x5e3000+PG_RW+PG_V	# memory page
	.double	0x5e4000+PG_RW+PG_V	# memory page
	.double	0x5e5000+PG_RW+PG_V	# memory page
	.double	0x5e6000+PG_RW+PG_V	# memory page
	.double	0x5e7000+PG_RW+PG_V	# memory page
	.double	0x5e8000+PG_RW+PG_V	# memory page
	.double	0x5e9000+PG_RW+PG_V	# memory page
	.double	0x5ea000+PG_RW+PG_V	# memory page
	.double	0x5eb000+PG_RW+PG_V	# memory page
	.double	0x5ec000+PG_RW+PG_V	# memory page
	.double	0x5ed000+PG_RW+PG_V	# memory page
	.double	0x5ee000+PG_RW+PG_V	# memory page
	.double	0x5ef000+PG_RW+PG_V	# memory page
	.double	0x5f0000+PG_RW+PG_V	# memory page
	.double	0x5f1000+PG_RW+PG_V	# memory page
	.double	0x5f2000+PG_RW+PG_V	# memory page
	.double	0x5f3000+PG_RW+PG_V	# memory page
	.double	0x5f4000+PG_RW+PG_V	# memory page
	.double	0x5f5000+PG_RW+PG_V	# memory page
	.double	0x5f6000+PG_RW+PG_V	# memory page
	.double	0x5f7000+PG_RW+PG_V	# memory page
	.double	0x5f8000+PG_RW+PG_V	# memory page
	.double	0x5f9000+PG_RW+PG_V	# memory page
	.double	0x5fa000+PG_RW+PG_V	# memory page
	.double	0x5fb000+PG_RW+PG_V	# memory page
	.double	0x5fc000+PG_RW+PG_V	# memory page
	.double	0x5fd000+PG_RW+PG_V	# memory page
	.double	0x5fe000+PG_RW+PG_V	# memory page
	.double	0x5ff000+PG_RW+PG_V	# memory page
	.double	0x600000+PG_RW+PG_V	# memory page
	.double	0x601000+PG_RW+PG_V	# memory page
	.double	0x602000+PG_RW+PG_V	# memory page
	.double	0x603000+PG_RW+PG_V	# memory page
	.double	0x604000+PG_RW+PG_V	# memory page
	.double	0x605000+PG_RW+PG_V	# memory page
	.double	0x606000+PG_RW+PG_V	# memory page
	.double	0x607000+PG_RW+PG_V	# memory page
	.double	0x608000+PG_RW+PG_V	# memory page
	.double	0x609000+PG_RW+PG_V	# memory page
	.double	0x60a000+PG_RW+PG_V	# memory page
	.double	0x60b000+PG_RW+PG_V	# memory page
	.double	0x60c000+PG_RW+PG_V	# memory page
	.double	0x60d000+PG_RW+PG_V	# memory page
	.double	0x60e000+PG_RW+PG_V	# memory page
	.double	0x60f000+PG_RW+PG_V	# memory page
	.double	0x610000+PG_RW+PG_V	# memory page
	.double	0x611000+PG_RW+PG_V	# memory page
	.double	0x612000+PG_RW+PG_V	# memory page
	.double	0x613000+PG_RW+PG_V	# memory page
	.double	0x614000+PG_RW+PG_V	# memory page
	.double	0x615000+PG_RW+PG_V	# memory page
	.double	0x616000+PG_RW+PG_V	# memory page
	.double	0x617000+PG_RW+PG_V	# memory page
	.double	0x618000+PG_RW+PG_V	# memory page
	.double	0x619000+PG_RW+PG_V	# memory page
	.double	0x61a000+PG_RW+PG_V	# memory page
	.double	0x61b000+PG_RW+PG_V	# memory page
	.double	0x61c000+PG_RW+PG_V	# memory page
	.double	0x61d000+PG_RW+PG_V	# memory page
	.double	0x61e000+PG_RW+PG_V	# memory page
	.double	0x61f000+PG_RW+PG_V	# memory page
	.double	0x620000+PG_RW+PG_V	# memory page
	.double	0x621000+PG_RW+PG_V	# memory page
	.double	0x622000+PG_RW+PG_V	# memory page
	.double	0x623000+PG_RW+PG_V	# memory page
	.double	0x624000+PG_RW+PG_V	# memory page
	.double	0x625000+PG_RW+PG_V	# memory page
	.double	0x626000+PG_RW+PG_V	# memory page
	.double	0x627000+PG_RW+PG_V	# memory page
	.double	0x628000+PG_RW+PG_V	# memory page
	.double	0x629000+PG_RW+PG_V	# memory page
	.double	0x62a000+PG_RW+PG_V	# memory page
	.double	0x62b000+PG_RW+PG_V	# memory page
	.double	0x62c000+PG_RW+PG_V	# memory page
	.double	0x62d000+PG_RW+PG_V	# memory page
	.double	0x62e000+PG_RW+PG_V	# memory page
	.double	0x62f000+PG_RW+PG_V	# memory page
	.double	0x630000+PG_RW+PG_V	# memory page
	.double	0x631000+PG_RW+PG_V	# memory page
	.double	0x632000+PG_RW+PG_V	# memory page
	.double	0x633000+PG_RW+PG_V	# memory page
	.double	0x634000+PG_RW+PG_V	# memory page
	.double	0x635000+PG_RW+PG_V	# memory page
	.double	0x636000+PG_RW+PG_V	# memory page
	.double	0x637000+PG_RW+PG_V	# memory page
	.double	0x638000+PG_RW+PG_V	# memory page
	.double	0x639000+PG_RW+PG_V	# memory page
	.double	0x63a000+PG_RW+PG_V	# memory page
	.double	0x63b000+PG_RW+PG_V	# memory page
	.double	0x63c000+PG_RW+PG_V	# memory page
	.double	0x63d000+PG_RW+PG_V	# memory page
	.double	0x63e000+PG_RW+PG_V	# memory page
	.double	0x63f000+PG_RW+PG_V	# memory page
	.double	0x640000+PG_RW+PG_V	# memory page
	.double	0x641000+PG_RW+PG_V	# memory page
	.double	0x642000+PG_RW+PG_V	# memory page
	.double	0x643000+PG_RW+PG_V	# memory page
	.double	0x644000+PG_RW+PG_V	# memory page
	.double	0x645000+PG_RW+PG_V	# memory page
	.double	0x646000+PG_RW+PG_V	# memory page
	.double	0x647000+PG_RW+PG_V	# memory page
	.double	0x648000+PG_RW+PG_V	# memory page
	.double	0x649000+PG_RW+PG_V	# memory page
	.double	0x64a000+PG_RW+PG_V	# memory page
	.double	0x64b000+PG_RW+PG_V	# memory page
	.double	0x64c000+PG_RW+PG_V	# memory page
	.double	0x64d000+PG_RW+PG_V	# memory page
	.double	0x64e000+PG_RW+PG_V	# memory page
	.double	0x64f000+PG_RW+PG_V	# memory page
	.double	0x650000+PG_RW+PG_V	# memory page
	.double	0x651000+PG_RW+PG_V	# memory page
	.double	0x652000+PG_RW+PG_V	# memory page
	.double	0x653000+PG_RW+PG_V	# memory page
	.double	0x654000+PG_RW+PG_V	# memory page
	.double	0x655000+PG_RW+PG_V	# memory page
	.double	0x656000+PG_RW+PG_V	# memory page
	.double	0x657000+PG_RW+PG_V	# memory page
	.double	0x658000+PG_RW+PG_V	# memory page
	.double	0x659000+PG_RW+PG_V	# memory page
	.double	0x65a000+PG_RW+PG_V	# memory page
	.double	0x65b000+PG_RW+PG_V	# memory page
	.double	0x65c000+PG_RW+PG_V	# memory page
	.double	0x65d000+PG_RW+PG_V	# memory page
	.double	0x65e000+PG_RW+PG_V	# memory page
	.double	0x65f000+PG_RW+PG_V	# memory page
	.double	0x660000+PG_RW+PG_V	# memory page
	.double	0x661000+PG_RW+PG_V	# memory page
	.double	0x662000+PG_RW+PG_V	# memory page
	.double	0x663000+PG_RW+PG_V	# memory page
	.double	0x664000+PG_RW+PG_V	# memory page
	.double	0x665000+PG_RW+PG_V	# memory page
	.double	0x666000+PG_RW+PG_V	# memory page
	.double	0x667000+PG_RW+PG_V	# memory page
	.double	0x668000+PG_RW+PG_V	# memory page
	.double	0x669000+PG_RW+PG_V	# memory page
	.double	0x66a000+PG_RW+PG_V	# memory page
	.double	0x66b000+PG_RW+PG_V	# memory page
	.double	0x66c000+PG_RW+PG_V	# memory page
	.double	0x66d000+PG_RW+PG_V	# memory page
	.double	0x66e000+PG_RW+PG_V	# memory page
	.double	0x66f000+PG_RW+PG_V	# memory page
	.double	0x670000+PG_RW+PG_V	# memory page
	.double	0x671000+PG_RW+PG_V	# memory page
	.double	0x672000+PG_RW+PG_V	# memory page
	.double	0x673000+PG_RW+PG_V	# memory page
	.double	0x674000+PG_RW+PG_V	# memory page
	.double	0x675000+PG_RW+PG_V	# memory page
	.double	0x676000+PG_RW+PG_V	# memory page
	.double	0x677000+PG_RW+PG_V	# memory page
	.double	0x678000+PG_RW+PG_V	# memory page
	.double	0x679000+PG_RW+PG_V	# memory page
	.double	0x67a000+PG_RW+PG_V	# memory page
	.double	0x67b000+PG_RW+PG_V	# memory page
	.double	0x67c000+PG_RW+PG_V	# memory page
	.double	0x67d000+PG_RW+PG_V	# memory page
	.double	0x67e000+PG_RW+PG_V	# memory page
	.double	0x67f000+PG_RW+PG_V	# memory page
	.double	0x680000+PG_RW+PG_V	# memory page
	.double	0x681000+PG_RW+PG_V	# memory page
	.double	0x682000+PG_RW+PG_V	# memory page
	.double	0x683000+PG_RW+PG_V	# memory page
	.double	0x684000+PG_RW+PG_V	# memory page
	.double	0x685000+PG_RW+PG_V	# memory page
	.double	0x686000+PG_RW+PG_V	# memory page
	.double	0x687000+PG_RW+PG_V	# memory page
	.double	0x688000+PG_RW+PG_V	# memory page
	.double	0x689000+PG_RW+PG_V	# memory page
	.double	0x68a000+PG_RW+PG_V	# memory page
	.double	0x68b000+PG_RW+PG_V	# memory page
	.double	0x68c000+PG_RW+PG_V	# memory page
	.double	0x68d000+PG_RW+PG_V	# memory page
	.double	0x68e000+PG_RW+PG_V	# memory page
	.double	0x68f000+PG_RW+PG_V	# memory page
	.double	0x690000+PG_RW+PG_V	# memory page
	.double	0x691000+PG_RW+PG_V	# memory page
	.double	0x692000+PG_RW+PG_V	# memory page
	.double	0x693000+PG_RW+PG_V	# memory page
	.double	0x694000+PG_RW+PG_V	# memory page
	.double	0x695000+PG_RW+PG_V	# memory page
	.double	0x696000+PG_RW+PG_V	# memory page
	.double	0x697000+PG_RW+PG_V	# memory page
	.double	0x698000+PG_RW+PG_V	# memory page
	.double	0x699000+PG_RW+PG_V	# memory page
	.double	0x69a000+PG_RW+PG_V	# memory page
	.double	0x69b000+PG_RW+PG_V	# memory page
	.double	0x69c000+PG_RW+PG_V	# memory page
	.double	0x69d000+PG_RW+PG_V	# memory page
	.double	0x69e000+PG_RW+PG_V	# memory page
	.double	0x69f000+PG_RW+PG_V	# memory page
	.double	0x6a0000+PG_RW+PG_V	# memory page
	.double	0x6a1000+PG_RW+PG_V	# memory page
	.double	0x6a2000+PG_RW+PG_V	# memory page
	.double	0x6a3000+PG_RW+PG_V	# memory page
	.double	0x6a4000+PG_RW+PG_V	# memory page
	.double	0x6a5000+PG_RW+PG_V	# memory page
	.double	0x6a6000+PG_RW+PG_V	# memory page
	.double	0x6a7000+PG_RW+PG_V	# memory page
	.double	0x6a8000+PG_RW+PG_V	# memory page
	.double	0x6a9000+PG_RW+PG_V	# memory page
	.double	0x6aa000+PG_RW+PG_V	# memory page
	.double	0x6ab000+PG_RW+PG_V	# memory page
	.double	0x6ac000+PG_RW+PG_V	# memory page
	.double	0x6ad000+PG_RW+PG_V	# memory page
	.double	0x6ae000+PG_RW+PG_V	# memory page
	.double	0x6af000+PG_RW+PG_V	# memory page
	.double	0x6b0000+PG_RW+PG_V	# memory page
	.double	0x6b1000+PG_RW+PG_V	# memory page
	.double	0x6b2000+PG_RW+PG_V	# memory page
	.double	0x6b3000+PG_RW+PG_V	# memory page
	.double	0x6b4000+PG_RW+PG_V	# memory page
	.double	0x6b5000+PG_RW+PG_V	# memory page
	.double	0x6b6000+PG_RW+PG_V	# memory page
	.double	0x6b7000+PG_RW+PG_V	# memory page
	.double	0x6b8000+PG_RW+PG_V	# memory page
	.double	0x6b9000+PG_RW+PG_V	# memory page
	.double	0x6ba000+PG_RW+PG_V	# memory page
	.double	0x6bb000+PG_RW+PG_V	# memory page
	.double	0x6bc000+PG_RW+PG_V	# memory page
	.double	0x6bd000+PG_RW+PG_V	# memory page
	.double	0x6be000+PG_RW+PG_V	# memory page
	.double	0x6bf000+PG_RW+PG_V	# memory page
	.double	0x6c0000+PG_RW+PG_V	# memory page
	.double	0x6c1000+PG_RW+PG_V	# memory page
	.double	0x6c2000+PG_RW+PG_V	# memory page
	.double	0x6c3000+PG_RW+PG_V	# memory page
	.double	0x6c4000+PG_RW+PG_V	# memory page
	.double	0x6c5000+PG_RW+PG_V	# memory page
	.double	0x6c6000+PG_RW+PG_V	# memory page
	.double	0x6c7000+PG_RW+PG_V	# memory page
	.double	0x6c8000+PG_RW+PG_V	# memory page
	.double	0x6c9000+PG_RW+PG_V	# memory page
	.double	0x6ca000+PG_RW+PG_V	# memory page
	.double	0x6cb000+PG_RW+PG_V	# memory page
	.double	0x6cc000+PG_RW+PG_V	# memory page
	.double	0x6cd000+PG_RW+PG_V	# memory page
	.double	0x6ce000+PG_RW+PG_V	# memory page
	.double	0x6cf000+PG_RW+PG_V	# memory page
	.double	0x6d0000+PG_RW+PG_V	# memory page
	.double	0x6d1000+PG_RW+PG_V	# memory page
	.double	0x6d2000+PG_RW+PG_V	# memory page
	.double	0x6d3000+PG_RW+PG_V	# memory page
	.double	0x6d4000+PG_RW+PG_V	# memory page
	.double	0x6d5000+PG_RW+PG_V	# memory page
	.double	0x6d6000+PG_RW+PG_V	# memory page
	.double	0x6d7000+PG_RW+PG_V	# memory page
	.double	0x6d8000+PG_RW+PG_V	# memory page
	.double	0x6d9000+PG_RW+PG_V	# memory page
	.double	0x6da000+PG_RW+PG_V	# memory page
	.double	0x6db000+PG_RW+PG_V	# memory page
	.double	0x6dc000+PG_RW+PG_V	# memory page
	.double	0x6dd000+PG_RW+PG_V	# memory page
	.double	0x6de000+PG_RW+PG_V	# memory page
	.double	0x6df000+PG_RW+PG_V	# memory page
	.double	0x6e0000+PG_RW+PG_V	# memory page
	.double	0x6e1000+PG_RW+PG_V	# memory page
	.double	0x6e2000+PG_RW+PG_V	# memory page
	.double	0x6e3000+PG_RW+PG_V	# memory page
	.double	0x6e4000+PG_RW+PG_V	# memory page
	.double	0x6e5000+PG_RW+PG_V	# memory page
	.double	0x6e6000+PG_RW+PG_V	# memory page
	.double	0x6e7000+PG_RW+PG_V	# memory page
	.double	0x6e8000+PG_RW+PG_V	# memory page
	.double	0x6e9000+PG_RW+PG_V	# memory page
	.double	0x6ea000+PG_RW+PG_V	# memory page
	.double	0x6eb000+PG_RW+PG_V	# memory page
	.double	0x6ec000+PG_RW+PG_V	# memory page
	.double	0x6ed000+PG_RW+PG_V	# memory page
	.double	0x6ee000+PG_RW+PG_V	# memory page
	.double	0x6ef000+PG_RW+PG_V	# memory page
	.double	0x6f0000+PG_RW+PG_V	# memory page
	.double	0x6f1000+PG_RW+PG_V	# memory page
	.double	0x6f2000+PG_RW+PG_V	# memory page
	.double	0x6f3000+PG_RW+PG_V	# memory page
	.double	0x6f4000+PG_RW+PG_V	# memory page
	.double	0x6f5000+PG_RW+PG_V	# memory page
	.double	0x6f6000+PG_RW+PG_V	# memory page
	.double	0x6f7000+PG_RW+PG_V	# memory page
	.double	0x6f8000+PG_RW+PG_V	# memory page
	.double	0x6f9000+PG_RW+PG_V	# memory page
	.double	0x6fa000+PG_RW+PG_V	# memory page
	.double	0x6fb000+PG_RW+PG_V	# memory page
	.double	0x6fc000+PG_RW+PG_V	# memory page
	.double	0x6fd000+PG_RW+PG_V	# memory page
	.double	0x6fe000+PG_RW+PG_V	# memory page
	.double	0x6ff000+PG_RW+PG_V	# memory page
	.double	0x700000+PG_RW+PG_V	# memory page
	.double	0x701000+PG_RW+PG_V	# memory page
	.double	0x702000+PG_RW+PG_V	# memory page
	.double	0x703000+PG_RW+PG_V	# memory page
	.double	0x704000+PG_RW+PG_V	# memory page
	.double	0x705000+PG_RW+PG_V	# memory page
	.double	0x706000+PG_RW+PG_V	# memory page
	.double	0x707000+PG_RW+PG_V	# memory page
	.double	0x708000+PG_RW+PG_V	# memory page
	.double	0x709000+PG_RW+PG_V	# memory page
	.double	0x70a000+PG_RW+PG_V	# memory page
	.double	0x70b000+PG_RW+PG_V	# memory page
	.double	0x70c000+PG_RW+PG_V	# memory page
	.double	0x70d000+PG_RW+PG_V	# memory page
	.double	0x70e000+PG_RW+PG_V	# memory page
	.double	0x70f000+PG_RW+PG_V	# memory page
	.double	0x710000+PG_RW+PG_V	# memory page
	.double	0x711000+PG_RW+PG_V	# memory page
	.double	0x712000+PG_RW+PG_V	# memory page
	.double	0x713000+PG_RW+PG_V	# memory page
	.double	0x714000+PG_RW+PG_V	# memory page
	.double	0x715000+PG_RW+PG_V	# memory page
	.double	0x716000+PG_RW+PG_V	# memory page
	.double	0x717000+PG_RW+PG_V	# memory page
	.double	0x718000+PG_RW+PG_V	# memory page
	.double	0x719000+PG_RW+PG_V	# memory page
	.double	0x71a000+PG_RW+PG_V	# memory page
	.double	0x71b000+PG_RW+PG_V	# memory page
	.double	0x71c000+PG_RW+PG_V	# memory page
	.double	0x71d000+PG_RW+PG_V	# memory page
	.double	0x71e000+PG_RW+PG_V	# memory page
	.double	0x71f000+PG_RW+PG_V	# memory page
	.double	0x720000+PG_RW+PG_V	# memory page
	.double	0x721000+PG_RW+PG_V	# memory page
	.double	0x722000+PG_RW+PG_V	# memory page
	.double	0x723000+PG_RW+PG_V	# memory page
	.double	0x724000+PG_RW+PG_V	# memory page
	.double	0x725000+PG_RW+PG_V	# memory page
	.double	0x726000+PG_RW+PG_V	# memory page
	.double	0x727000+PG_RW+PG_V	# memory page
	.double	0x728000+PG_RW+PG_V	# memory page
	.double	0x729000+PG_RW+PG_V	# memory page
	.double	0x72a000+PG_RW+PG_V	# memory page
	.double	0x72b000+PG_RW+PG_V	# memory page
	.double	0x72c000+PG_RW+PG_V	# memory page
	.double	0x72d000+PG_RW+PG_V	# memory page
	.double	0x72e000+PG_RW+PG_V	# memory page
	.double	0x72f000+PG_RW+PG_V	# memory page
	.double	0x730000+PG_RW+PG_V	# memory page
	.double	0x731000+PG_RW+PG_V	# memory page
	.double	0x732000+PG_RW+PG_V	# memory page
	.double	0x733000+PG_RW+PG_V	# memory page
	.double	0x734000+PG_RW+PG_V	# memory page
	.double	0x735000+PG_RW+PG_V	# memory page
	.double	0x736000+PG_RW+PG_V	# memory page
	.double	0x737000+PG_RW+PG_V	# memory page
	.double	0x738000+PG_RW+PG_V	# memory page
	.double	0x739000+PG_RW+PG_V	# memory page
	.double	0x73a000+PG_RW+PG_V	# memory page
	.double	0x73b000+PG_RW+PG_V	# memory page
	.double	0x73c000+PG_RW+PG_V	# memory page
	.double	0x73d000+PG_RW+PG_V	# memory page
	.double	0x73e000+PG_RW+PG_V	# memory page
	.double	0x73f000+PG_RW+PG_V	# memory page
	.double	0x740000+PG_RW+PG_V	# memory page
	.double	0x741000+PG_RW+PG_V	# memory page
	.double	0x742000+PG_RW+PG_V	# memory page
	.double	0x743000+PG_RW+PG_V	# memory page
	.double	0x744000+PG_RW+PG_V	# memory page
	.double	0x745000+PG_RW+PG_V	# memory page
	.double	0x746000+PG_RW+PG_V	# memory page
	.double	0x747000+PG_RW+PG_V	# memory page
	.double	0x748000+PG_RW+PG_V	# memory page
	.double	0x749000+PG_RW+PG_V	# memory page
	.double	0x74a000+PG_RW+PG_V	# memory page
	.double	0x74b000+PG_RW+PG_V	# memory page
	.double	0x74c000+PG_RW+PG_V	# memory page
	.double	0x74d000+PG_RW+PG_V	# memory page
	.double	0x74e000+PG_RW+PG_V	# memory page
	.double	0x74f000+PG_RW+PG_V	# memory page
	.double	0x750000+PG_RW+PG_V	# memory page
	.double	0x751000+PG_RW+PG_V	# memory page
	.double	0x752000+PG_RW+PG_V	# memory page
	.double	0x753000+PG_RW+PG_V	# memory page
	.double	0x754000+PG_RW+PG_V	# memory page
	.double	0x755000+PG_RW+PG_V	# memory page
	.double	0x756000+PG_RW+PG_V	# memory page
	.double	0x757000+PG_RW+PG_V	# memory page
	.double	0x758000+PG_RW+PG_V	# memory page
	.double	0x759000+PG_RW+PG_V	# memory page
	.double	0x75a000+PG_RW+PG_V	# memory page
	.double	0x75b000+PG_RW+PG_V	# memory page
	.double	0x75c000+PG_RW+PG_V	# memory page
	.double	0x75d000+PG_RW+PG_V	# memory page
	.double	0x75e000+PG_RW+PG_V	# memory page
	.double	0x75f000+PG_RW+PG_V	# memory page
	.double	0x760000+PG_RW+PG_V	# memory page
	.double	0x761000+PG_RW+PG_V	# memory page
	.double	0x762000+PG_RW+PG_V	# memory page
	.double	0x763000+PG_RW+PG_V	# memory page
	.double	0x764000+PG_RW+PG_V	# memory page
	.double	0x765000+PG_RW+PG_V	# memory page
	.double	0x766000+PG_RW+PG_V	# memory page
	.double	0x767000+PG_RW+PG_V	# memory page
	.double	0x768000+PG_RW+PG_V	# memory page
	.double	0x769000+PG_RW+PG_V	# memory page
	.double	0x76a000+PG_RW+PG_V	# memory page
	.double	0x76b000+PG_RW+PG_V	# memory page
	.double	0x76c000+PG_RW+PG_V	# memory page
	.double	0x76d000+PG_RW+PG_V	# memory page
	.double	0x76e000+PG_RW+PG_V	# memory page
	.double	0x76f000+PG_RW+PG_V	# memory page
	.double	0x770000+PG_RW+PG_V	# memory page
	.double	0x771000+PG_RW+PG_V	# memory page
	.double	0x772000+PG_RW+PG_V	# memory page
	.double	0x773000+PG_RW+PG_V	# memory page
	.double	0x774000+PG_RW+PG_V	# memory page
	.double	0x775000+PG_RW+PG_V	# memory page
	.double	0x776000+PG_RW+PG_V	# memory page
	.double	0x777000+PG_RW+PG_V	# memory page
	.double	0x778000+PG_RW+PG_V	# memory page
	.double	0x779000+PG_RW+PG_V	# memory page
	.double	0x77a000+PG_RW+PG_V	# memory page
	.double	0x77b000+PG_RW+PG_V	# memory page
	.double	0x77c000+PG_RW+PG_V	# memory page
	.double	0x77d000+PG_RW+PG_V	# memory page
	.double	0x77e000+PG_RW+PG_V	# memory page
	.double	0x77f000+PG_RW+PG_V	# memory page
	.double	0x780000+PG_RW+PG_V	# memory page
	.double	0x781000+PG_RW+PG_V	# memory page
	.double	0x782000+PG_RW+PG_V	# memory page
	.double	0x783000+PG_RW+PG_V	# memory page
	.double	0x784000+PG_RW+PG_V	# memory page
	.double	0x785000+PG_RW+PG_V	# memory page
	.double	0x786000+PG_RW+PG_V	# memory page
	.double	0x787000+PG_RW+PG_V	# memory page
	.double	0x788000+PG_RW+PG_V	# memory page
	.double	0x789000+PG_RW+PG_V	# memory page
	.double	0x78a000+PG_RW+PG_V	# memory page
	.double	0x78b000+PG_RW+PG_V	# memory page
	.double	0x78c000+PG_RW+PG_V	# memory page
	.double	0x78d000+PG_RW+PG_V	# memory page
	.double	0x78e000+PG_RW+PG_V	# memory page
	.double	0x78f000+PG_RW+PG_V	# memory page
	.double	0x790000+PG_RW+PG_V	# memory page
	.double	0x791000+PG_RW+PG_V	# memory page
	.double	0x792000+PG_RW+PG_V	# memory page
	.double	0x793000+PG_RW+PG_V	# memory page
	.double	0x794000+PG_RW+PG_V	# memory page
	.double	0x795000+PG_RW+PG_V	# memory page
	.double	0x796000+PG_RW+PG_V	# memory page
	.double	0x797000+PG_RW+PG_V	# memory page
	.double	0x798000+PG_RW+PG_V	# memory page
	.double	0x799000+PG_RW+PG_V	# memory page
	.double	0x79a000+PG_RW+PG_V	# memory page
	.double	0x79b000+PG_RW+PG_V	# memory page
	.double	0x79c000+PG_RW+PG_V	# memory page
	.double	0x79d000+PG_RW+PG_V	# memory page
	.double	0x79e000+PG_RW+PG_V	# memory page
	.double	0x79f000+PG_RW+PG_V	# memory page
	.double	0x7a0000+PG_RW+PG_V	# memory page
	.double	0x7a1000+PG_RW+PG_V	# memory page
	.double	0x7a2000+PG_RW+PG_V	# memory page
	.double	0x7a3000+PG_RW+PG_V	# memory page
	.double	0x7a4000+PG_RW+PG_V	# memory page
	.double	0x7a5000+PG_RW+PG_V	# memory page
	.double	0x7a6000+PG_RW+PG_V	# memory page
	.double	0x7a7000+PG_RW+PG_V	# memory page
	.double	0x7a8000+PG_RW+PG_V	# memory page
	.double	0x7a9000+PG_RW+PG_V	# memory page
	.double	0x7aa000+PG_RW+PG_V	# memory page
	.double	0x7ab000+PG_RW+PG_V	# memory page
	.double	0x7ac000+PG_RW+PG_V	# memory page
	.double	0x7ad000+PG_RW+PG_V	# memory page
	.double	0x7ae000+PG_RW+PG_V	# memory page
	.double	0x7af000+PG_RW+PG_V	# memory page
	.double	0x7b0000+PG_RW+PG_V	# memory page
	.double	0x7b1000+PG_RW+PG_V	# memory page
	.double	0x7b2000+PG_RW+PG_V	# memory page
	.double	0x7b3000+PG_RW+PG_V	# memory page
	.double	0x7b4000+PG_RW+PG_V	# memory page
	.double	0x7b5000+PG_RW+PG_V	# memory page
	.double	0x7b6000+PG_RW+PG_V	# memory page
	.double	0x7b7000+PG_RW+PG_V	# memory page
	.double	0x7b8000+PG_RW+PG_V	# memory page
	.double	0x7b9000+PG_RW+PG_V	# memory page
	.double	0x7ba000+PG_RW+PG_V	# memory page
	.double	0x7bb000+PG_RW+PG_V	# memory page
	.double	0x7bc000+PG_RW+PG_V	# memory page
	.double	0x7bd000+PG_RW+PG_V	# memory page
	.double	0x7be000+PG_RW+PG_V	# memory page
	.double	0x7bf000+PG_RW+PG_V	# memory page
	.double	0x7c0000+PG_RW+PG_V	# memory page
	.double	0x7c1000+PG_RW+PG_V	# memory page
	.double	0x7c2000+PG_RW+PG_V	# memory page
	.double	0x7c3000+PG_RW+PG_V	# memory page
	.double	0x7c4000+PG_RW+PG_V	# memory page
	.double	0x7c5000+PG_RW+PG_V	# memory page
	.double	0x7c6000+PG_RW+PG_V	# memory page
	.double	0x7c7000+PG_RW+PG_V	# memory page
	.double	0x7c8000+PG_RW+PG_V	# memory page
	.double	0x7c9000+PG_RW+PG_V	# memory page
	.double	0x7ca000+PG_RW+PG_V	# memory page
	.double	0x7cb000+PG_RW+PG_V	# memory page
	.double	0x7cc000+PG_RW+PG_V	# memory page
	.double	0x7cd000+PG_RW+PG_V	# memory page
	.double	0x7ce000+PG_RW+PG_V	# memory page
	.double	0x7cf000+PG_RW+PG_V	# memory page
	.double	0x7d0000+PG_RW+PG_V	# memory page
	.double	0x7d1000+PG_RW+PG_V	# memory page
	.double	0x7d2000+PG_RW+PG_V	# memory page
	.double	0x7d3000+PG_RW+PG_V	# memory page
	.double	0x7d4000+PG_RW+PG_V	# memory page
	.double	0x7d5000+PG_RW+PG_V	# memory page
	.double	0x7d6000+PG_RW+PG_V	# memory page
	.double	0x7d7000+PG_RW+PG_V	# memory page
	.double	0x7d8000+PG_RW+PG_V	# memory page
	.double	0x7d9000+PG_RW+PG_V	# memory page
	.double	0x7da000+PG_RW+PG_V	# memory page
	.double	0x7db000+PG_RW+PG_V	# memory page
	.double	0x7dc000+PG_RW+PG_V	# memory page
	.double	0x7dd000+PG_RW+PG_V	# memory page
	.double	0x7de000+PG_RW+PG_V	# memory page
	.double	0x7df000+PG_RW+PG_V	# memory page
	.double	0x7e0000+PG_RW+PG_V	# memory page
	.double	0x7e1000+PG_RW+PG_V	# memory page
	.double	0x7e2000+PG_RW+PG_V	# memory page
	.double	0x7e3000+PG_RW+PG_V	# memory page
	.double	0x7e4000+PG_RW+PG_V	# memory page
	.double	0x7e5000+PG_RW+PG_V	# memory page
	.double	0x7e6000+PG_RW+PG_V	# memory page
	.double	0x7e7000+PG_RW+PG_V	# memory page
	.double	0x7e8000+PG_RW+PG_V	# memory page
	.double	0x7e9000+PG_RW+PG_V	# memory page
	.double	0x7ea000+PG_RW+PG_V	# memory page
	.double	0x7eb000+PG_RW+PG_V	# memory page
	.double	0x7ec000+PG_RW+PG_V	# memory page
	.double	0x7ed000+PG_RW+PG_V	# memory page
	.double	0x7ee000+PG_RW+PG_V	# memory page
	.double	0x7ef000+PG_RW+PG_V	# memory page
	.double	0x7f0000+PG_RW+PG_V	# memory page
	.double	0x7f1000+PG_RW+PG_V	# memory page
	.double	0x7f2000+PG_RW+PG_V	# memory page
	.double	0x7f3000+PG_RW+PG_V	# memory page
	.double	0x7f4000+PG_RW+PG_V	# memory page
	.double	0x7f5000+PG_RW+PG_V	# memory page
	.double	0x7f6000+PG_RW+PG_V	# memory page
	.double	0x7f7000+PG_RW+PG_V	# memory page
	.double	0x7f8000+PG_RW+PG_V	# memory page
	.double	0x7f9000+PG_RW+PG_V	# memory page
	.double	0x7fa000+PG_RW+PG_V	# memory page
	.double	0x7fb000+PG_RW+PG_V	# memory page
	.double	0x7fc000+PG_RW+PG_V	# memory page
	.double	0x7fd000+PG_RW+PG_V	# memory page
	.double	0x7fe000+PG_RW+PG_V	# memory page
	.double	0x7ff000+PG_RW+PG_V	# memory page

# PAGE 6
# 2nd level page table entries (8MB-12MB)
paget2:
	.double	0x800000+PG_RW+PG_V	# memory page
	.double	0x801000+PG_RW+PG_V	# memory page
	.double	0x802000+PG_RW+PG_V	# memory page
	.double	0x803000+PG_RW+PG_V	# memory page
	.double	0x804000+PG_RW+PG_V	# memory page
	.double	0x805000+PG_RW+PG_V	# memory page
	.double	0x806000+PG_RW+PG_V	# memory page
	.double	0x807000+PG_RW+PG_V	# memory page
	.double	0x808000+PG_RW+PG_V	# memory page
	.double	0x809000+PG_RW+PG_V	# memory page
	.double	0x80a000+PG_RW+PG_V	# memory page
	.double	0x80b000+PG_RW+PG_V	# memory page
	.double	0x80c000+PG_RW+PG_V	# memory page
	.double	0x80d000+PG_RW+PG_V	# memory page
	.double	0x80e000+PG_RW+PG_V	# memory page
	.double	0x80f000+PG_RW+PG_V	# memory page
	.double	0x810000+PG_RW+PG_V	# memory page
	.double	0x811000+PG_RW+PG_V	# memory page
	.double	0x812000+PG_RW+PG_V	# memory page
	.double	0x813000+PG_RW+PG_V	# memory page
	.double	0x814000+PG_RW+PG_V	# memory page
	.double	0x815000+PG_RW+PG_V	# memory page
	.double	0x816000+PG_RW+PG_V	# memory page
	.double	0x817000+PG_RW+PG_V	# memory page
	.double	0x818000+PG_RW+PG_V	# memory page
	.double	0x819000+PG_RW+PG_V	# memory page
	.double	0x81a000+PG_RW+PG_V	# memory page
	.double	0x81b000+PG_RW+PG_V	# memory page
	.double	0x81c000+PG_RW+PG_V	# memory page
	.double	0x81d000+PG_RW+PG_V	# memory page
	.double	0x81e000+PG_RW+PG_V	# memory page
	.double	0x81f000+PG_RW+PG_V	# memory page
	.double	0x820000+PG_RW+PG_V	# memory page
	.double	0x821000+PG_RW+PG_V	# memory page
	.double	0x822000+PG_RW+PG_V	# memory page
	.double	0x823000+PG_RW+PG_V	# memory page
	.double	0x824000+PG_RW+PG_V	# memory page
	.double	0x825000+PG_RW+PG_V	# memory page
	.double	0x826000+PG_RW+PG_V	# memory page
	.double	0x827000+PG_RW+PG_V	# memory page
	.double	0x828000+PG_RW+PG_V	# memory page
	.double	0x829000+PG_RW+PG_V	# memory page
	.double	0x82a000+PG_RW+PG_V	# memory page
	.double	0x82b000+PG_RW+PG_V	# memory page
	.double	0x82c000+PG_RW+PG_V	# memory page
	.double	0x82d000+PG_RW+PG_V	# memory page
	.double	0x82e000+PG_RW+PG_V	# memory page
	.double	0x82f000+PG_RW+PG_V	# memory page
	.double	0x830000+PG_RW+PG_V	# memory page
	.double	0x831000+PG_RW+PG_V	# memory page
	.double	0x832000+PG_RW+PG_V	# memory page
	.double	0x833000+PG_RW+PG_V	# memory page
	.double	0x834000+PG_RW+PG_V	# memory page
	.double	0x835000+PG_RW+PG_V	# memory page
	.double	0x836000+PG_RW+PG_V	# memory page
	.double	0x837000+PG_RW+PG_V	# memory page
	.double	0x838000+PG_RW+PG_V	# memory page
	.double	0x839000+PG_RW+PG_V	# memory page
	.double	0x83a000+PG_RW+PG_V	# memory page
	.double	0x83b000+PG_RW+PG_V	# memory page
	.double	0x83c000+PG_RW+PG_V	# memory page
	.double	0x83d000+PG_RW+PG_V	# memory page
	.double	0x83e000+PG_RW+PG_V	# memory page
	.double	0x83f000+PG_RW+PG_V	# memory page
	.double	0x840000+PG_RW+PG_V	# memory page
	.double	0x841000+PG_RW+PG_V	# memory page
	.double	0x842000+PG_RW+PG_V	# memory page
	.double	0x843000+PG_RW+PG_V	# memory page
	.double	0x844000+PG_RW+PG_V	# memory page
	.double	0x845000+PG_RW+PG_V	# memory page
	.double	0x846000+PG_RW+PG_V	# memory page
	.double	0x847000+PG_RW+PG_V	# memory page
	.double	0x848000+PG_RW+PG_V	# memory page
	.double	0x849000+PG_RW+PG_V	# memory page
	.double	0x84a000+PG_RW+PG_V	# memory page
	.double	0x84b000+PG_RW+PG_V	# memory page
	.double	0x84c000+PG_RW+PG_V	# memory page
	.double	0x84d000+PG_RW+PG_V	# memory page
	.double	0x84e000+PG_RW+PG_V	# memory page
	.double	0x84f000+PG_RW+PG_V	# memory page
	.double	0x850000+PG_RW+PG_V	# memory page
	.double	0x851000+PG_RW+PG_V	# memory page
	.double	0x852000+PG_RW+PG_V	# memory page
	.double	0x853000+PG_RW+PG_V	# memory page
	.double	0x854000+PG_RW+PG_V	# memory page
	.double	0x855000+PG_RW+PG_V	# memory page
	.double	0x856000+PG_RW+PG_V	# memory page
	.double	0x857000+PG_RW+PG_V	# memory page
	.double	0x858000+PG_RW+PG_V	# memory page
	.double	0x859000+PG_RW+PG_V	# memory page
	.double	0x85a000+PG_RW+PG_V	# memory page
	.double	0x85b000+PG_RW+PG_V	# memory page
	.double	0x85c000+PG_RW+PG_V	# memory page
	.double	0x85d000+PG_RW+PG_V	# memory page
	.double	0x85e000+PG_RW+PG_V	# memory page
	.double	0x85f000+PG_RW+PG_V	# memory page
	.double	0x860000+PG_RW+PG_V	# memory page
	.double	0x861000+PG_RW+PG_V	# memory page
	.double	0x862000+PG_RW+PG_V	# memory page
	.double	0x863000+PG_RW+PG_V	# memory page
	.double	0x864000+PG_RW+PG_V	# memory page
	.double	0x865000+PG_RW+PG_V	# memory page
	.double	0x866000+PG_RW+PG_V	# memory page
	.double	0x867000+PG_RW+PG_V	# memory page
	.double	0x868000+PG_RW+PG_V	# memory page
	.double	0x869000+PG_RW+PG_V	# memory page
	.double	0x86a000+PG_RW+PG_V	# memory page
	.double	0x86b000+PG_RW+PG_V	# memory page
	.double	0x86c000+PG_RW+PG_V	# memory page
	.double	0x86d000+PG_RW+PG_V	# memory page
	.double	0x86e000+PG_RW+PG_V	# memory page
	.double	0x86f000+PG_RW+PG_V	# memory page
	.double	0x870000+PG_RW+PG_V	# memory page
	.double	0x871000+PG_RW+PG_V	# memory page
	.double	0x872000+PG_RW+PG_V	# memory page
	.double	0x873000+PG_RW+PG_V	# memory page
	.double	0x874000+PG_RW+PG_V	# memory page
	.double	0x875000+PG_RW+PG_V	# memory page
	.double	0x876000+PG_RW+PG_V	# memory page
	.double	0x877000+PG_RW+PG_V	# memory page
	.double	0x878000+PG_RW+PG_V	# memory page
	.double	0x879000+PG_RW+PG_V	# memory page
	.double	0x87a000+PG_RW+PG_V	# memory page
	.double	0x87b000+PG_RW+PG_V	# memory page
	.double	0x87c000+PG_RW+PG_V	# memory page
	.double	0x87d000+PG_RW+PG_V	# memory page
	.double	0x87e000+PG_RW+PG_V	# memory page
	.double	0x87f000+PG_RW+PG_V	# memory page
	.double	0x880000+PG_RW+PG_V	# memory page
	.double	0x881000+PG_RW+PG_V	# memory page
	.double	0x882000+PG_RW+PG_V	# memory page
	.double	0x883000+PG_RW+PG_V	# memory page
	.double	0x884000+PG_RW+PG_V	# memory page
	.double	0x885000+PG_RW+PG_V	# memory page
	.double	0x886000+PG_RW+PG_V	# memory page
	.double	0x887000+PG_RW+PG_V	# memory page
	.double	0x888000+PG_RW+PG_V	# memory page
	.double	0x889000+PG_RW+PG_V	# memory page
	.double	0x88a000+PG_RW+PG_V	# memory page
	.double	0x88b000+PG_RW+PG_V	# memory page
	.double	0x88c000+PG_RW+PG_V	# memory page
	.double	0x88d000+PG_RW+PG_V	# memory page
	.double	0x88e000+PG_RW+PG_V	# memory page
	.double	0x88f000+PG_RW+PG_V	# memory page
	.double	0x890000+PG_RW+PG_V	# memory page
	.double	0x891000+PG_RW+PG_V	# memory page
	.double	0x892000+PG_RW+PG_V	# memory page
	.double	0x893000+PG_RW+PG_V	# memory page
	.double	0x894000+PG_RW+PG_V	# memory page
	.double	0x895000+PG_RW+PG_V	# memory page
	.double	0x896000+PG_RW+PG_V	# memory page
	.double	0x897000+PG_RW+PG_V	# memory page
	.double	0x898000+PG_RW+PG_V	# memory page
	.double	0x899000+PG_RW+PG_V	# memory page
	.double	0x89a000+PG_RW+PG_V	# memory page
	.double	0x89b000+PG_RW+PG_V	# memory page
	.double	0x89c000+PG_RW+PG_V	# memory page
	.double	0x89d000+PG_RW+PG_V	# memory page
	.double	0x89e000+PG_RW+PG_V	# memory page
	.double	0x89f000+PG_RW+PG_V	# memory page
	.double	0x8a0000+PG_RW+PG_V	# memory page
	.double	0x8a1000+PG_RW+PG_V	# memory page
	.double	0x8a2000+PG_RW+PG_V	# memory page
	.double	0x8a3000+PG_RW+PG_V	# memory page
	.double	0x8a4000+PG_RW+PG_V	# memory page
	.double	0x8a5000+PG_RW+PG_V	# memory page
	.double	0x8a6000+PG_RW+PG_V	# memory page
	.double	0x8a7000+PG_RW+PG_V	# memory page
	.double	0x8a8000+PG_RW+PG_V	# memory page
	.double	0x8a9000+PG_RW+PG_V	# memory page
	.double	0x8aa000+PG_RW+PG_V	# memory page
	.double	0x8ab000+PG_RW+PG_V	# memory page
	.double	0x8ac000+PG_RW+PG_V	# memory page
	.double	0x8ad000+PG_RW+PG_V	# memory page
	.double	0x8ae000+PG_RW+PG_V	# memory page
	.double	0x8af000+PG_RW+PG_V	# memory page
	.double	0x8b0000+PG_RW+PG_V	# memory page
	.double	0x8b1000+PG_RW+PG_V	# memory page
	.double	0x8b2000+PG_RW+PG_V	# memory page
	.double	0x8b3000+PG_RW+PG_V	# memory page
	.double	0x8b4000+PG_RW+PG_V	# memory page
	.double	0x8b5000+PG_RW+PG_V	# memory page
	.double	0x8b6000+PG_RW+PG_V	# memory page
	.double	0x8b7000+PG_RW+PG_V	# memory page
	.double	0x8b8000+PG_RW+PG_V	# memory page
	.double	0x8b9000+PG_RW+PG_V	# memory page
	.double	0x8ba000+PG_RW+PG_V	# memory page
	.double	0x8bb000+PG_RW+PG_V	# memory page
	.double	0x8bc000+PG_RW+PG_V	# memory page
	.double	0x8bd000+PG_RW+PG_V	# memory page
	.double	0x8be000+PG_RW+PG_V	# memory page
	.double	0x8bf000+PG_RW+PG_V	# memory page
	.double	0x8c0000+PG_RW+PG_V	# memory page
	.double	0x8c1000+PG_RW+PG_V	# memory page
	.double	0x8c2000+PG_RW+PG_V	# memory page
	.double	0x8c3000+PG_RW+PG_V	# memory page
	.double	0x8c4000+PG_RW+PG_V	# memory page
	.double	0x8c5000+PG_RW+PG_V	# memory page
	.double	0x8c6000+PG_RW+PG_V	# memory page
	.double	0x8c7000+PG_RW+PG_V	# memory page
	.double	0x8c8000+PG_RW+PG_V	# memory page
	.double	0x8c9000+PG_RW+PG_V	# memory page
	.double	0x8ca000+PG_RW+PG_V	# memory page
	.double	0x8cb000+PG_RW+PG_V	# memory page
	.double	0x8cc000+PG_RW+PG_V	# memory page
	.double	0x8cd000+PG_RW+PG_V	# memory page
	.double	0x8ce000+PG_RW+PG_V	# memory page
	.double	0x8cf000+PG_RW+PG_V	# memory page
	.double	0x8d0000+PG_RW+PG_V	# memory page
	.double	0x8d1000+PG_RW+PG_V	# memory page
	.double	0x8d2000+PG_RW+PG_V	# memory page
	.double	0x8d3000+PG_RW+PG_V	# memory page
	.double	0x8d4000+PG_RW+PG_V	# memory page
	.double	0x8d5000+PG_RW+PG_V	# memory page
	.double	0x8d6000+PG_RW+PG_V	# memory page
	.double	0x8d7000+PG_RW+PG_V	# memory page
	.double	0x8d8000+PG_RW+PG_V	# memory page
	.double	0x8d9000+PG_RW+PG_V	# memory page
	.double	0x8da000+PG_RW+PG_V	# memory page
	.double	0x8db000+PG_RW+PG_V	# memory page
	.double	0x8dc000+PG_RW+PG_V	# memory page
	.double	0x8dd000+PG_RW+PG_V	# memory page
	.double	0x8de000+PG_RW+PG_V	# memory page
	.double	0x8df000+PG_RW+PG_V	# memory page
	.double	0x8e0000+PG_RW+PG_V	# memory page
	.double	0x8e1000+PG_RW+PG_V	# memory page
	.double	0x8e2000+PG_RW+PG_V	# memory page
	.double	0x8e3000+PG_RW+PG_V	# memory page
	.double	0x8e4000+PG_RW+PG_V	# memory page
	.double	0x8e5000+PG_RW+PG_V	# memory page
	.double	0x8e6000+PG_RW+PG_V	# memory page
	.double	0x8e7000+PG_RW+PG_V	# memory page
	.double	0x8e8000+PG_RW+PG_V	# memory page
	.double	0x8e9000+PG_RW+PG_V	# memory page
	.double	0x8ea000+PG_RW+PG_V	# memory page
	.double	0x8eb000+PG_RW+PG_V	# memory page
	.double	0x8ec000+PG_RW+PG_V	# memory page
	.double	0x8ed000+PG_RW+PG_V	# memory page
	.double	0x8ee000+PG_RW+PG_V	# memory page
	.double	0x8ef000+PG_RW+PG_V	# memory page
	.double	0x8f0000+PG_RW+PG_V	# memory page
	.double	0x8f1000+PG_RW+PG_V	# memory page
	.double	0x8f2000+PG_RW+PG_V	# memory page
	.double	0x8f3000+PG_RW+PG_V	# memory page
	.double	0x8f4000+PG_RW+PG_V	# memory page
	.double	0x8f5000+PG_RW+PG_V	# memory page
	.double	0x8f6000+PG_RW+PG_V	# memory page
	.double	0x8f7000+PG_RW+PG_V	# memory page
	.double	0x8f8000+PG_RW+PG_V	# memory page
	.double	0x8f9000+PG_RW+PG_V	# memory page
	.double	0x8fa000+PG_RW+PG_V	# memory page
	.double	0x8fb000+PG_RW+PG_V	# memory page
	.double	0x8fc000+PG_RW+PG_V	# memory page
	.double	0x8fd000+PG_RW+PG_V	# memory page
	.double	0x8fe000+PG_RW+PG_V	# memory page
	.double	0x8ff000+PG_RW+PG_V	# memory page
	.double	0x900000+PG_RW+PG_V	# memory page
	.double	0x901000+PG_RW+PG_V	# memory page
	.double	0x902000+PG_RW+PG_V	# memory page
	.double	0x903000+PG_RW+PG_V	# memory page
	.double	0x904000+PG_RW+PG_V	# memory page
	.double	0x905000+PG_RW+PG_V	# memory page
	.double	0x906000+PG_RW+PG_V	# memory page
	.double	0x907000+PG_RW+PG_V	# memory page
	.double	0x908000+PG_RW+PG_V	# memory page
	.double	0x909000+PG_RW+PG_V	# memory page
	.double	0x90a000+PG_RW+PG_V	# memory page
	.double	0x90b000+PG_RW+PG_V	# memory page
	.double	0x90c000+PG_RW+PG_V	# memory page
	.double	0x90d000+PG_RW+PG_V	# memory page
	.double	0x90e000+PG_RW+PG_V	# memory page
	.double	0x90f000+PG_RW+PG_V	# memory page
	.double	0x910000+PG_RW+PG_V	# memory page
	.double	0x911000+PG_RW+PG_V	# memory page
	.double	0x912000+PG_RW+PG_V	# memory page
	.double	0x913000+PG_RW+PG_V	# memory page
	.double	0x914000+PG_RW+PG_V	# memory page
	.double	0x915000+PG_RW+PG_V	# memory page
	.double	0x916000+PG_RW+PG_V	# memory page
	.double	0x917000+PG_RW+PG_V	# memory page
	.double	0x918000+PG_RW+PG_V	# memory page
	.double	0x919000+PG_RW+PG_V	# memory page
	.double	0x91a000+PG_RW+PG_V	# memory page
	.double	0x91b000+PG_RW+PG_V	# memory page
	.double	0x91c000+PG_RW+PG_V	# memory page
	.double	0x91d000+PG_RW+PG_V	# memory page
	.double	0x91e000+PG_RW+PG_V	# memory page
	.double	0x91f000+PG_RW+PG_V	# memory page
	.double	0x920000+PG_RW+PG_V	# memory page
	.double	0x921000+PG_RW+PG_V	# memory page
	.double	0x922000+PG_RW+PG_V	# memory page
	.double	0x923000+PG_RW+PG_V	# memory page
	.double	0x924000+PG_RW+PG_V	# memory page
	.double	0x925000+PG_RW+PG_V	# memory page
	.double	0x926000+PG_RW+PG_V	# memory page
	.double	0x927000+PG_RW+PG_V	# memory page
	.double	0x928000+PG_RW+PG_V	# memory page
	.double	0x929000+PG_RW+PG_V	# memory page
	.double	0x92a000+PG_RW+PG_V	# memory page
	.double	0x92b000+PG_RW+PG_V	# memory page
	.double	0x92c000+PG_RW+PG_V	# memory page
	.double	0x92d000+PG_RW+PG_V	# memory page
	.double	0x92e000+PG_RW+PG_V	# memory page
	.double	0x92f000+PG_RW+PG_V	# memory page
	.double	0x930000+PG_RW+PG_V	# memory page
	.double	0x931000+PG_RW+PG_V	# memory page
	.double	0x932000+PG_RW+PG_V	# memory page
	.double	0x933000+PG_RW+PG_V	# memory page
	.double	0x934000+PG_RW+PG_V	# memory page
	.double	0x935000+PG_RW+PG_V	# memory page
	.double	0x936000+PG_RW+PG_V	# memory page
	.double	0x937000+PG_RW+PG_V	# memory page
	.double	0x938000+PG_RW+PG_V	# memory page
	.double	0x939000+PG_RW+PG_V	# memory page
	.double	0x93a000+PG_RW+PG_V	# memory page
	.double	0x93b000+PG_RW+PG_V	# memory page
	.double	0x93c000+PG_RW+PG_V	# memory page
	.double	0x93d000+PG_RW+PG_V	# memory page
	.double	0x93e000+PG_RW+PG_V	# memory page
	.double	0x93f000+PG_RW+PG_V	# memory page
	.double	0x940000+PG_RW+PG_V	# memory page
	.double	0x941000+PG_RW+PG_V	# memory page
	.double	0x942000+PG_RW+PG_V	# memory page
	.double	0x943000+PG_RW+PG_V	# memory page
	.double	0x944000+PG_RW+PG_V	# memory page
	.double	0x945000+PG_RW+PG_V	# memory page
	.double	0x946000+PG_RW+PG_V	# memory page
	.double	0x947000+PG_RW+PG_V	# memory page
	.double	0x948000+PG_RW+PG_V	# memory page
	.double	0x949000+PG_RW+PG_V	# memory page
	.double	0x94a000+PG_RW+PG_V	# memory page
	.double	0x94b000+PG_RW+PG_V	# memory page
	.double	0x94c000+PG_RW+PG_V	# memory page
	.double	0x94d000+PG_RW+PG_V	# memory page
	.double	0x94e000+PG_RW+PG_V	# memory page
	.double	0x94f000+PG_RW+PG_V	# memory page
	.double	0x950000+PG_RW+PG_V	# memory page
	.double	0x951000+PG_RW+PG_V	# memory page
	.double	0x952000+PG_RW+PG_V	# memory page
	.double	0x953000+PG_RW+PG_V	# memory page
	.double	0x954000+PG_RW+PG_V	# memory page
	.double	0x955000+PG_RW+PG_V	# memory page
	.double	0x956000+PG_RW+PG_V	# memory page
	.double	0x957000+PG_RW+PG_V	# memory page
	.double	0x958000+PG_RW+PG_V	# memory page
	.double	0x959000+PG_RW+PG_V	# memory page
	.double	0x95a000+PG_RW+PG_V	# memory page
	.double	0x95b000+PG_RW+PG_V	# memory page
	.double	0x95c000+PG_RW+PG_V	# memory page
	.double	0x95d000+PG_RW+PG_V	# memory page
	.double	0x95e000+PG_RW+PG_V	# memory page
	.double	0x95f000+PG_RW+PG_V	# memory page
	.double	0x960000+PG_RW+PG_V	# memory page
	.double	0x961000+PG_RW+PG_V	# memory page
	.double	0x962000+PG_RW+PG_V	# memory page
	.double	0x963000+PG_RW+PG_V	# memory page
	.double	0x964000+PG_RW+PG_V	# memory page
	.double	0x965000+PG_RW+PG_V	# memory page
	.double	0x966000+PG_RW+PG_V	# memory page
	.double	0x967000+PG_RW+PG_V	# memory page
	.double	0x968000+PG_RW+PG_V	# memory page
	.double	0x969000+PG_RW+PG_V	# memory page
	.double	0x96a000+PG_RW+PG_V	# memory page
	.double	0x96b000+PG_RW+PG_V	# memory page
	.double	0x96c000+PG_RW+PG_V	# memory page
	.double	0x96d000+PG_RW+PG_V	# memory page
	.double	0x96e000+PG_RW+PG_V	# memory page
	.double	0x96f000+PG_RW+PG_V	# memory page
	.double	0x970000+PG_RW+PG_V	# memory page
	.double	0x971000+PG_RW+PG_V	# memory page
	.double	0x972000+PG_RW+PG_V	# memory page
	.double	0x973000+PG_RW+PG_V	# memory page
	.double	0x974000+PG_RW+PG_V	# memory page
	.double	0x975000+PG_RW+PG_V	# memory page
	.double	0x976000+PG_RW+PG_V	# memory page
	.double	0x977000+PG_RW+PG_V	# memory page
	.double	0x978000+PG_RW+PG_V	# memory page
	.double	0x979000+PG_RW+PG_V	# memory page
	.double	0x97a000+PG_RW+PG_V	# memory page
	.double	0x97b000+PG_RW+PG_V	# memory page
	.double	0x97c000+PG_RW+PG_V	# memory page
	.double	0x97d000+PG_RW+PG_V	# memory page
	.double	0x97e000+PG_RW+PG_V	# memory page
	.double	0x97f000+PG_RW+PG_V	# memory page
	.double	0x980000+PG_RW+PG_V	# memory page
	.double	0x981000+PG_RW+PG_V	# memory page
	.double	0x982000+PG_RW+PG_V	# memory page
	.double	0x983000+PG_RW+PG_V	# memory page
	.double	0x984000+PG_RW+PG_V	# memory page
	.double	0x985000+PG_RW+PG_V	# memory page
	.double	0x986000+PG_RW+PG_V	# memory page
	.double	0x987000+PG_RW+PG_V	# memory page
	.double	0x988000+PG_RW+PG_V	# memory page
	.double	0x989000+PG_RW+PG_V	# memory page
	.double	0x98a000+PG_RW+PG_V	# memory page
	.double	0x98b000+PG_RW+PG_V	# memory page
	.double	0x98c000+PG_RW+PG_V	# memory page
	.double	0x98d000+PG_RW+PG_V	# memory page
	.double	0x98e000+PG_RW+PG_V	# memory page
	.double	0x98f000+PG_RW+PG_V	# memory page
	.double	0x990000+PG_RW+PG_V	# memory page
	.double	0x991000+PG_RW+PG_V	# memory page
	.double	0x992000+PG_RW+PG_V	# memory page
	.double	0x993000+PG_RW+PG_V	# memory page
	.double	0x994000+PG_RW+PG_V	# memory page
	.double	0x995000+PG_RW+PG_V	# memory page
	.double	0x996000+PG_RW+PG_V	# memory page
	.double	0x997000+PG_RW+PG_V	# memory page
	.double	0x998000+PG_RW+PG_V	# memory page
	.double	0x999000+PG_RW+PG_V	# memory page
	.double	0x99a000+PG_RW+PG_V	# memory page
	.double	0x99b000+PG_RW+PG_V	# memory page
	.double	0x99c000+PG_RW+PG_V	# memory page
	.double	0x99d000+PG_RW+PG_V	# memory page
	.double	0x99e000+PG_RW+PG_V	# memory page
	.double	0x99f000+PG_RW+PG_V	# memory page
	.double	0x9a0000+PG_RW+PG_V	# memory page
	.double	0x9a1000+PG_RW+PG_V	# memory page
	.double	0x9a2000+PG_RW+PG_V	# memory page
	.double	0x9a3000+PG_RW+PG_V	# memory page
	.double	0x9a4000+PG_RW+PG_V	# memory page
	.double	0x9a5000+PG_RW+PG_V	# memory page
	.double	0x9a6000+PG_RW+PG_V	# memory page
	.double	0x9a7000+PG_RW+PG_V	# memory page
	.double	0x9a8000+PG_RW+PG_V	# memory page
	.double	0x9a9000+PG_RW+PG_V	# memory page
	.double	0x9aa000+PG_RW+PG_V	# memory page
	.double	0x9ab000+PG_RW+PG_V	# memory page
	.double	0x9ac000+PG_RW+PG_V	# memory page
	.double	0x9ad000+PG_RW+PG_V	# memory page
	.double	0x9ae000+PG_RW+PG_V	# memory page
	.double	0x9af000+PG_RW+PG_V	# memory page
	.double	0x9b0000+PG_RW+PG_V	# memory page
	.double	0x9b1000+PG_RW+PG_V	# memory page
	.double	0x9b2000+PG_RW+PG_V	# memory page
	.double	0x9b3000+PG_RW+PG_V	# memory page
	.double	0x9b4000+PG_RW+PG_V	# memory page
	.double	0x9b5000+PG_RW+PG_V	# memory page
	.double	0x9b6000+PG_RW+PG_V	# memory page
	.double	0x9b7000+PG_RW+PG_V	# memory page
	.double	0x9b8000+PG_RW+PG_V	# memory page
	.double	0x9b9000+PG_RW+PG_V	# memory page
	.double	0x9ba000+PG_RW+PG_V	# memory page
	.double	0x9bb000+PG_RW+PG_V	# memory page
	.double	0x9bc000+PG_RW+PG_V	# memory page
	.double	0x9bd000+PG_RW+PG_V	# memory page
	.double	0x9be000+PG_RW+PG_V	# memory page
	.double	0x9bf000+PG_RW+PG_V	# memory page
	.double	0x9c0000+PG_RW+PG_V	# memory page
	.double	0x9c1000+PG_RW+PG_V	# memory page
	.double	0x9c2000+PG_RW+PG_V	# memory page
	.double	0x9c3000+PG_RW+PG_V	# memory page
	.double	0x9c4000+PG_RW+PG_V	# memory page
	.double	0x9c5000+PG_RW+PG_V	# memory page
	.double	0x9c6000+PG_RW+PG_V	# memory page
	.double	0x9c7000+PG_RW+PG_V	# memory page
	.double	0x9c8000+PG_RW+PG_V	# memory page
	.double	0x9c9000+PG_RW+PG_V	# memory page
	.double	0x9ca000+PG_RW+PG_V	# memory page
	.double	0x9cb000+PG_RW+PG_V	# memory page
	.double	0x9cc000+PG_RW+PG_V	# memory page
	.double	0x9cd000+PG_RW+PG_V	# memory page
	.double	0x9ce000+PG_RW+PG_V	# memory page
	.double	0x9cf000+PG_RW+PG_V	# memory page
	.double	0x9d0000+PG_RW+PG_V	# memory page
	.double	0x9d1000+PG_RW+PG_V	# memory page
	.double	0x9d2000+PG_RW+PG_V	# memory page
	.double	0x9d3000+PG_RW+PG_V	# memory page
	.double	0x9d4000+PG_RW+PG_V	# memory page
	.double	0x9d5000+PG_RW+PG_V	# memory page
	.double	0x9d6000+PG_RW+PG_V	# memory page
	.double	0x9d7000+PG_RW+PG_V	# memory page
	.double	0x9d8000+PG_RW+PG_V	# memory page
	.double	0x9d9000+PG_RW+PG_V	# memory page
	.double	0x9da000+PG_RW+PG_V	# memory page
	.double	0x9db000+PG_RW+PG_V	# memory page
	.double	0x9dc000+PG_RW+PG_V	# memory page
	.double	0x9dd000+PG_RW+PG_V	# memory page
	.double	0x9de000+PG_RW+PG_V	# memory page
	.double	0x9df000+PG_RW+PG_V	# memory page
	.double	0x9e0000+PG_RW+PG_V	# memory page
	.double	0x9e1000+PG_RW+PG_V	# memory page
	.double	0x9e2000+PG_RW+PG_V	# memory page
	.double	0x9e3000+PG_RW+PG_V	# memory page
	.double	0x9e4000+PG_RW+PG_V	# memory page
	.double	0x9e5000+PG_RW+PG_V	# memory page
	.double	0x9e6000+PG_RW+PG_V	# memory page
	.double	0x9e7000+PG_RW+PG_V	# memory page
	.double	0x9e8000+PG_RW+PG_V	# memory page
	.double	0x9e9000+PG_RW+PG_V	# memory page
	.double	0x9ea000+PG_RW+PG_V	# memory page
	.double	0x9eb000+PG_RW+PG_V	# memory page
	.double	0x9ec000+PG_RW+PG_V	# memory page
	.double	0x9ed000+PG_RW+PG_V	# memory page
	.double	0x9ee000+PG_RW+PG_V	# memory page
	.double	0x9ef000+PG_RW+PG_V	# memory page
	.double	0x9f0000+PG_RW+PG_V	# memory page
	.double	0x9f1000+PG_RW+PG_V	# memory page
	.double	0x9f2000+PG_RW+PG_V	# memory page
	.double	0x9f3000+PG_RW+PG_V	# memory page
	.double	0x9f4000+PG_RW+PG_V	# memory page
	.double	0x9f5000+PG_RW+PG_V	# memory page
	.double	0x9f6000+PG_RW+PG_V	# memory page
	.double	0x9f7000+PG_RW+PG_V	# memory page
	.double	0x9f8000+PG_RW+PG_V	# memory page
	.double	0x9f9000+PG_RW+PG_V	# memory page
	.double	0x9fa000+PG_RW+PG_V	# memory page
	.double	0x9fb000+PG_RW+PG_V	# memory page
	.double	0x9fc000+PG_RW+PG_V	# memory page
	.double	0x9fd000+PG_RW+PG_V	# memory page
	.double	0x9fe000+PG_RW+PG_V	# memory page
	.double	0x9ff000+PG_RW+PG_V	# memory page
	.double	0xa00000+PG_RW+PG_V	# memory page
	.double	0xa01000+PG_RW+PG_V	# memory page
	.double	0xa02000+PG_RW+PG_V	# memory page
	.double	0xa03000+PG_RW+PG_V	# memory page
	.double	0xa04000+PG_RW+PG_V	# memory page
	.double	0xa05000+PG_RW+PG_V	# memory page
	.double	0xa06000+PG_RW+PG_V	# memory page
	.double	0xa07000+PG_RW+PG_V	# memory page
	.double	0xa08000+PG_RW+PG_V	# memory page
	.double	0xa09000+PG_RW+PG_V	# memory page
	.double	0xa0a000+PG_RW+PG_V	# memory page
	.double	0xa0b000+PG_RW+PG_V	# memory page
	.double	0xa0c000+PG_RW+PG_V	# memory page
	.double	0xa0d000+PG_RW+PG_V	# memory page
	.double	0xa0e000+PG_RW+PG_V	# memory page
	.double	0xa0f000+PG_RW+PG_V	# memory page
	.double	0xa10000+PG_RW+PG_V	# memory page
	.double	0xa11000+PG_RW+PG_V	# memory page
	.double	0xa12000+PG_RW+PG_V	# memory page
	.double	0xa13000+PG_RW+PG_V	# memory page
	.double	0xa14000+PG_RW+PG_V	# memory page
	.double	0xa15000+PG_RW+PG_V	# memory page
	.double	0xa16000+PG_RW+PG_V	# memory page
	.double	0xa17000+PG_RW+PG_V	# memory page
	.double	0xa18000+PG_RW+PG_V	# memory page
	.double	0xa19000+PG_RW+PG_V	# memory page
	.double	0xa1a000+PG_RW+PG_V	# memory page
	.double	0xa1b000+PG_RW+PG_V	# memory page
	.double	0xa1c000+PG_RW+PG_V	# memory page
	.double	0xa1d000+PG_RW+PG_V	# memory page
	.double	0xa1e000+PG_RW+PG_V	# memory page
	.double	0xa1f000+PG_RW+PG_V	# memory page
	.double	0xa20000+PG_RW+PG_V	# memory page
	.double	0xa21000+PG_RW+PG_V	# memory page
	.double	0xa22000+PG_RW+PG_V	# memory page
	.double	0xa23000+PG_RW+PG_V	# memory page
	.double	0xa24000+PG_RW+PG_V	# memory page
	.double	0xa25000+PG_RW+PG_V	# memory page
	.double	0xa26000+PG_RW+PG_V	# memory page
	.double	0xa27000+PG_RW+PG_V	# memory page
	.double	0xa28000+PG_RW+PG_V	# memory page
	.double	0xa29000+PG_RW+PG_V	# memory page
	.double	0xa2a000+PG_RW+PG_V	# memory page
	.double	0xa2b000+PG_RW+PG_V	# memory page
	.double	0xa2c000+PG_RW+PG_V	# memory page
	.double	0xa2d000+PG_RW+PG_V	# memory page
	.double	0xa2e000+PG_RW+PG_V	# memory page
	.double	0xa2f000+PG_RW+PG_V	# memory page
	.double	0xa30000+PG_RW+PG_V	# memory page
	.double	0xa31000+PG_RW+PG_V	# memory page
	.double	0xa32000+PG_RW+PG_V	# memory page
	.double	0xa33000+PG_RW+PG_V	# memory page
	.double	0xa34000+PG_RW+PG_V	# memory page
	.double	0xa35000+PG_RW+PG_V	# memory page
	.double	0xa36000+PG_RW+PG_V	# memory page
	.double	0xa37000+PG_RW+PG_V	# memory page
	.double	0xa38000+PG_RW+PG_V	# memory page
	.double	0xa39000+PG_RW+PG_V	# memory page
	.double	0xa3a000+PG_RW+PG_V	# memory page
	.double	0xa3b000+PG_RW+PG_V	# memory page
	.double	0xa3c000+PG_RW+PG_V	# memory page
	.double	0xa3d000+PG_RW+PG_V	# memory page
	.double	0xa3e000+PG_RW+PG_V	# memory page
	.double	0xa3f000+PG_RW+PG_V	# memory page
	.double	0xa40000+PG_RW+PG_V	# memory page
	.double	0xa41000+PG_RW+PG_V	# memory page
	.double	0xa42000+PG_RW+PG_V	# memory page
	.double	0xa43000+PG_RW+PG_V	# memory page
	.double	0xa44000+PG_RW+PG_V	# memory page
	.double	0xa45000+PG_RW+PG_V	# memory page
	.double	0xa46000+PG_RW+PG_V	# memory page
	.double	0xa47000+PG_RW+PG_V	# memory page
	.double	0xa48000+PG_RW+PG_V	# memory page
	.double	0xa49000+PG_RW+PG_V	# memory page
	.double	0xa4a000+PG_RW+PG_V	# memory page
	.double	0xa4b000+PG_RW+PG_V	# memory page
	.double	0xa4c000+PG_RW+PG_V	# memory page
	.double	0xa4d000+PG_RW+PG_V	# memory page
	.double	0xa4e000+PG_RW+PG_V	# memory page
	.double	0xa4f000+PG_RW+PG_V	# memory page
	.double	0xa50000+PG_RW+PG_V	# memory page
	.double	0xa51000+PG_RW+PG_V	# memory page
	.double	0xa52000+PG_RW+PG_V	# memory page
	.double	0xa53000+PG_RW+PG_V	# memory page
	.double	0xa54000+PG_RW+PG_V	# memory page
	.double	0xa55000+PG_RW+PG_V	# memory page
	.double	0xa56000+PG_RW+PG_V	# memory page
	.double	0xa57000+PG_RW+PG_V	# memory page
	.double	0xa58000+PG_RW+PG_V	# memory page
	.double	0xa59000+PG_RW+PG_V	# memory page
	.double	0xa5a000+PG_RW+PG_V	# memory page
	.double	0xa5b000+PG_RW+PG_V	# memory page
	.double	0xa5c000+PG_RW+PG_V	# memory page
	.double	0xa5d000+PG_RW+PG_V	# memory page
	.double	0xa5e000+PG_RW+PG_V	# memory page
	.double	0xa5f000+PG_RW+PG_V	# memory page
	.double	0xa60000+PG_RW+PG_V	# memory page
	.double	0xa61000+PG_RW+PG_V	# memory page
	.double	0xa62000+PG_RW+PG_V	# memory page
	.double	0xa63000+PG_RW+PG_V	# memory page
	.double	0xa64000+PG_RW+PG_V	# memory page
	.double	0xa65000+PG_RW+PG_V	# memory page
	.double	0xa66000+PG_RW+PG_V	# memory page
	.double	0xa67000+PG_RW+PG_V	# memory page
	.double	0xa68000+PG_RW+PG_V	# memory page
	.double	0xa69000+PG_RW+PG_V	# memory page
	.double	0xa6a000+PG_RW+PG_V	# memory page
	.double	0xa6b000+PG_RW+PG_V	# memory page
	.double	0xa6c000+PG_RW+PG_V	# memory page
	.double	0xa6d000+PG_RW+PG_V	# memory page
	.double	0xa6e000+PG_RW+PG_V	# memory page
	.double	0xa6f000+PG_RW+PG_V	# memory page
	.double	0xa70000+PG_RW+PG_V	# memory page
	.double	0xa71000+PG_RW+PG_V	# memory page
	.double	0xa72000+PG_RW+PG_V	# memory page
	.double	0xa73000+PG_RW+PG_V	# memory page
	.double	0xa74000+PG_RW+PG_V	# memory page
	.double	0xa75000+PG_RW+PG_V	# memory page
	.double	0xa76000+PG_RW+PG_V	# memory page
	.double	0xa77000+PG_RW+PG_V	# memory page
	.double	0xa78000+PG_RW+PG_V	# memory page
	.double	0xa79000+PG_RW+PG_V	# memory page
	.double	0xa7a000+PG_RW+PG_V	# memory page
	.double	0xa7b000+PG_RW+PG_V	# memory page
	.double	0xa7c000+PG_RW+PG_V	# memory page
	.double	0xa7d000+PG_RW+PG_V	# memory page
	.double	0xa7e000+PG_RW+PG_V	# memory page
	.double	0xa7f000+PG_RW+PG_V	# memory page
	.double	0xa80000+PG_RW+PG_V	# memory page
	.double	0xa81000+PG_RW+PG_V	# memory page
	.double	0xa82000+PG_RW+PG_V	# memory page
	.double	0xa83000+PG_RW+PG_V	# memory page
	.double	0xa84000+PG_RW+PG_V	# memory page
	.double	0xa85000+PG_RW+PG_V	# memory page
	.double	0xa86000+PG_RW+PG_V	# memory page
	.double	0xa87000+PG_RW+PG_V	# memory page
	.double	0xa88000+PG_RW+PG_V	# memory page
	.double	0xa89000+PG_RW+PG_V	# memory page
	.double	0xa8a000+PG_RW+PG_V	# memory page
	.double	0xa8b000+PG_RW+PG_V	# memory page
	.double	0xa8c000+PG_RW+PG_V	# memory page
	.double	0xa8d000+PG_RW+PG_V	# memory page
	.double	0xa8e000+PG_RW+PG_V	# memory page
	.double	0xa8f000+PG_RW+PG_V	# memory page
	.double	0xa90000+PG_RW+PG_V	# memory page
	.double	0xa91000+PG_RW+PG_V	# memory page
	.double	0xa92000+PG_RW+PG_V	# memory page
	.double	0xa93000+PG_RW+PG_V	# memory page
	.double	0xa94000+PG_RW+PG_V	# memory page
	.double	0xa95000+PG_RW+PG_V	# memory page
	.double	0xa96000+PG_RW+PG_V	# memory page
	.double	0xa97000+PG_RW+PG_V	# memory page
	.double	0xa98000+PG_RW+PG_V	# memory page
	.double	0xa99000+PG_RW+PG_V	# memory page
	.double	0xa9a000+PG_RW+PG_V	# memory page
	.double	0xa9b000+PG_RW+PG_V	# memory page
	.double	0xa9c000+PG_RW+PG_V	# memory page
	.double	0xa9d000+PG_RW+PG_V	# memory page
	.double	0xa9e000+PG_RW+PG_V	# memory page
	.double	0xa9f000+PG_RW+PG_V	# memory page
	.double	0xaa0000+PG_RW+PG_V	# memory page
	.double	0xaa1000+PG_RW+PG_V	# memory page
	.double	0xaa2000+PG_RW+PG_V	# memory page
	.double	0xaa3000+PG_RW+PG_V	# memory page
	.double	0xaa4000+PG_RW+PG_V	# memory page
	.double	0xaa5000+PG_RW+PG_V	# memory page
	.double	0xaa6000+PG_RW+PG_V	# memory page
	.double	0xaa7000+PG_RW+PG_V	# memory page
	.double	0xaa8000+PG_RW+PG_V	# memory page
	.double	0xaa9000+PG_RW+PG_V	# memory page
	.double	0xaaa000+PG_RW+PG_V	# memory page
	.double	0xaab000+PG_RW+PG_V	# memory page
	.double	0xaac000+PG_RW+PG_V	# memory page
	.double	0xaad000+PG_RW+PG_V	# memory page
	.double	0xaae000+PG_RW+PG_V	# memory page
	.double	0xaaf000+PG_RW+PG_V	# memory page
	.double	0xab0000+PG_RW+PG_V	# memory page
	.double	0xab1000+PG_RW+PG_V	# memory page
	.double	0xab2000+PG_RW+PG_V	# memory page
	.double	0xab3000+PG_RW+PG_V	# memory page
	.double	0xab4000+PG_RW+PG_V	# memory page
	.double	0xab5000+PG_RW+PG_V	# memory page
	.double	0xab6000+PG_RW+PG_V	# memory page
	.double	0xab7000+PG_RW+PG_V	# memory page
	.double	0xab8000+PG_RW+PG_V	# memory page
	.double	0xab9000+PG_RW+PG_V	# memory page
	.double	0xaba000+PG_RW+PG_V	# memory page
	.double	0xabb000+PG_RW+PG_V	# memory page
	.double	0xabc000+PG_RW+PG_V	# memory page
	.double	0xabd000+PG_RW+PG_V	# memory page
	.double	0xabe000+PG_RW+PG_V	# memory page
	.double	0xabf000+PG_RW+PG_V	# memory page
	.double	0xac0000+PG_RW+PG_V	# memory page
	.double	0xac1000+PG_RW+PG_V	# memory page
	.double	0xac2000+PG_RW+PG_V	# memory page
	.double	0xac3000+PG_RW+PG_V	# memory page
	.double	0xac4000+PG_RW+PG_V	# memory page
	.double	0xac5000+PG_RW+PG_V	# memory page
	.double	0xac6000+PG_RW+PG_V	# memory page
	.double	0xac7000+PG_RW+PG_V	# memory page
	.double	0xac8000+PG_RW+PG_V	# memory page
	.double	0xac9000+PG_RW+PG_V	# memory page
	.double	0xaca000+PG_RW+PG_V	# memory page
	.double	0xacb000+PG_RW+PG_V	# memory page
	.double	0xacc000+PG_RW+PG_V	# memory page
	.double	0xacd000+PG_RW+PG_V	# memory page
	.double	0xace000+PG_RW+PG_V	# memory page
	.double	0xacf000+PG_RW+PG_V	# memory page
	.double	0xad0000+PG_RW+PG_V	# memory page
	.double	0xad1000+PG_RW+PG_V	# memory page
	.double	0xad2000+PG_RW+PG_V	# memory page
	.double	0xad3000+PG_RW+PG_V	# memory page
	.double	0xad4000+PG_RW+PG_V	# memory page
	.double	0xad5000+PG_RW+PG_V	# memory page
	.double	0xad6000+PG_RW+PG_V	# memory page
	.double	0xad7000+PG_RW+PG_V	# memory page
	.double	0xad8000+PG_RW+PG_V	# memory page
	.double	0xad9000+PG_RW+PG_V	# memory page
	.double	0xada000+PG_RW+PG_V	# memory page
	.double	0xadb000+PG_RW+PG_V	# memory page
	.double	0xadc000+PG_RW+PG_V	# memory page
	.double	0xadd000+PG_RW+PG_V	# memory page
	.double	0xade000+PG_RW+PG_V	# memory page
	.double	0xadf000+PG_RW+PG_V	# memory page
	.double	0xae0000+PG_RW+PG_V	# memory page
	.double	0xae1000+PG_RW+PG_V	# memory page
	.double	0xae2000+PG_RW+PG_V	# memory page
	.double	0xae3000+PG_RW+PG_V	# memory page
	.double	0xae4000+PG_RW+PG_V	# memory page
	.double	0xae5000+PG_RW+PG_V	# memory page
	.double	0xae6000+PG_RW+PG_V	# memory page
	.double	0xae7000+PG_RW+PG_V	# memory page
	.double	0xae8000+PG_RW+PG_V	# memory page
	.double	0xae9000+PG_RW+PG_V	# memory page
	.double	0xaea000+PG_RW+PG_V	# memory page
	.double	0xaeb000+PG_RW+PG_V	# memory page
	.double	0xaec000+PG_RW+PG_V	# memory page
	.double	0xaed000+PG_RW+PG_V	# memory page
	.double	0xaee000+PG_RW+PG_V	# memory page
	.double	0xaef000+PG_RW+PG_V	# memory page
	.double	0xaf0000+PG_RW+PG_V	# memory page
	.double	0xaf1000+PG_RW+PG_V	# memory page
	.double	0xaf2000+PG_RW+PG_V	# memory page
	.double	0xaf3000+PG_RW+PG_V	# memory page
	.double	0xaf4000+PG_RW+PG_V	# memory page
	.double	0xaf5000+PG_RW+PG_V	# memory page
	.double	0xaf6000+PG_RW+PG_V	# memory page
	.double	0xaf7000+PG_RW+PG_V	# memory page
	.double	0xaf8000+PG_RW+PG_V	# memory page
	.double	0xaf9000+PG_RW+PG_V	# memory page
	.double	0xafa000+PG_RW+PG_V	# memory page
	.double	0xafb000+PG_RW+PG_V	# memory page
	.double	0xafc000+PG_RW+PG_V	# memory page
	.double	0xafd000+PG_RW+PG_V	# memory page
	.double	0xafe000+PG_RW+PG_V	# memory page
	.double	0xaff000+PG_RW+PG_V	# memory page
	.double	0xb00000+PG_RW+PG_V	# memory page
	.double	0xb01000+PG_RW+PG_V	# memory page
	.double	0xb02000+PG_RW+PG_V	# memory page
	.double	0xb03000+PG_RW+PG_V	# memory page
	.double	0xb04000+PG_RW+PG_V	# memory page
	.double	0xb05000+PG_RW+PG_V	# memory page
	.double	0xb06000+PG_RW+PG_V	# memory page
	.double	0xb07000+PG_RW+PG_V	# memory page
	.double	0xb08000+PG_RW+PG_V	# memory page
	.double	0xb09000+PG_RW+PG_V	# memory page
	.double	0xb0a000+PG_RW+PG_V	# memory page
	.double	0xb0b000+PG_RW+PG_V	# memory page
	.double	0xb0c000+PG_RW+PG_V	# memory page
	.double	0xb0d000+PG_RW+PG_V	# memory page
	.double	0xb0e000+PG_RW+PG_V	# memory page
	.double	0xb0f000+PG_RW+PG_V	# memory page
	.double	0xb10000+PG_RW+PG_V	# memory page
	.double	0xb11000+PG_RW+PG_V	# memory page
	.double	0xb12000+PG_RW+PG_V	# memory page
	.double	0xb13000+PG_RW+PG_V	# memory page
	.double	0xb14000+PG_RW+PG_V	# memory page
	.double	0xb15000+PG_RW+PG_V	# memory page
	.double	0xb16000+PG_RW+PG_V	# memory page
	.double	0xb17000+PG_RW+PG_V	# memory page
	.double	0xb18000+PG_RW+PG_V	# memory page
	.double	0xb19000+PG_RW+PG_V	# memory page
	.double	0xb1a000+PG_RW+PG_V	# memory page
	.double	0xb1b000+PG_RW+PG_V	# memory page
	.double	0xb1c000+PG_RW+PG_V	# memory page
	.double	0xb1d000+PG_RW+PG_V	# memory page
	.double	0xb1e000+PG_RW+PG_V	# memory page
	.double	0xb1f000+PG_RW+PG_V	# memory page
	.double	0xb20000+PG_RW+PG_V	# memory page
	.double	0xb21000+PG_RW+PG_V	# memory page
	.double	0xb22000+PG_RW+PG_V	# memory page
	.double	0xb23000+PG_RW+PG_V	# memory page
	.double	0xb24000+PG_RW+PG_V	# memory page
	.double	0xb25000+PG_RW+PG_V	# memory page
	.double	0xb26000+PG_RW+PG_V	# memory page
	.double	0xb27000+PG_RW+PG_V	# memory page
	.double	0xb28000+PG_RW+PG_V	# memory page
	.double	0xb29000+PG_RW+PG_V	# memory page
	.double	0xb2a000+PG_RW+PG_V	# memory page
	.double	0xb2b000+PG_RW+PG_V	# memory page
	.double	0xb2c000+PG_RW+PG_V	# memory page
	.double	0xb2d000+PG_RW+PG_V	# memory page
	.double	0xb2e000+PG_RW+PG_V	# memory page
	.double	0xb2f000+PG_RW+PG_V	# memory page
	.double	0xb30000+PG_RW+PG_V	# memory page
	.double	0xb31000+PG_RW+PG_V	# memory page
	.double	0xb32000+PG_RW+PG_V	# memory page
	.double	0xb33000+PG_RW+PG_V	# memory page
	.double	0xb34000+PG_RW+PG_V	# memory page
	.double	0xb35000+PG_RW+PG_V	# memory page
	.double	0xb36000+PG_RW+PG_V	# memory page
	.double	0xb37000+PG_RW+PG_V	# memory page
	.double	0xb38000+PG_RW+PG_V	# memory page
	.double	0xb39000+PG_RW+PG_V	# memory page
	.double	0xb3a000+PG_RW+PG_V	# memory page
	.double	0xb3b000+PG_RW+PG_V	# memory page
	.double	0xb3c000+PG_RW+PG_V	# memory page
	.double	0xb3d000+PG_RW+PG_V	# memory page
	.double	0xb3e000+PG_RW+PG_V	# memory page
	.double	0xb3f000+PG_RW+PG_V	# memory page
	.double	0xb40000+PG_RW+PG_V	# memory page
	.double	0xb41000+PG_RW+PG_V	# memory page
	.double	0xb42000+PG_RW+PG_V	# memory page
	.double	0xb43000+PG_RW+PG_V	# memory page
	.double	0xb44000+PG_RW+PG_V	# memory page
	.double	0xb45000+PG_RW+PG_V	# memory page
	.double	0xb46000+PG_RW+PG_V	# memory page
	.double	0xb47000+PG_RW+PG_V	# memory page
	.double	0xb48000+PG_RW+PG_V	# memory page
	.double	0xb49000+PG_RW+PG_V	# memory page
	.double	0xb4a000+PG_RW+PG_V	# memory page
	.double	0xb4b000+PG_RW+PG_V	# memory page
	.double	0xb4c000+PG_RW+PG_V	# memory page
	.double	0xb4d000+PG_RW+PG_V	# memory page
	.double	0xb4e000+PG_RW+PG_V	# memory page
	.double	0xb4f000+PG_RW+PG_V	# memory page
	.double	0xb50000+PG_RW+PG_V	# memory page
	.double	0xb51000+PG_RW+PG_V	# memory page
	.double	0xb52000+PG_RW+PG_V	# memory page
	.double	0xb53000+PG_RW+PG_V	# memory page
	.double	0xb54000+PG_RW+PG_V	# memory page
	.double	0xb55000+PG_RW+PG_V	# memory page
	.double	0xb56000+PG_RW+PG_V	# memory page
	.double	0xb57000+PG_RW+PG_V	# memory page
	.double	0xb58000+PG_RW+PG_V	# memory page
	.double	0xb59000+PG_RW+PG_V	# memory page
	.double	0xb5a000+PG_RW+PG_V	# memory page
	.double	0xb5b000+PG_RW+PG_V	# memory page
	.double	0xb5c000+PG_RW+PG_V	# memory page
	.double	0xb5d000+PG_RW+PG_V	# memory page
	.double	0xb5e000+PG_RW+PG_V	# memory page
	.double	0xb5f000+PG_RW+PG_V	# memory page
	.double	0xb60000+PG_RW+PG_V	# memory page
	.double	0xb61000+PG_RW+PG_V	# memory page
	.double	0xb62000+PG_RW+PG_V	# memory page
	.double	0xb63000+PG_RW+PG_V	# memory page
	.double	0xb64000+PG_RW+PG_V	# memory page
	.double	0xb65000+PG_RW+PG_V	# memory page
	.double	0xb66000+PG_RW+PG_V	# memory page
	.double	0xb67000+PG_RW+PG_V	# memory page
	.double	0xb68000+PG_RW+PG_V	# memory page
	.double	0xb69000+PG_RW+PG_V	# memory page
	.double	0xb6a000+PG_RW+PG_V	# memory page
	.double	0xb6b000+PG_RW+PG_V	# memory page
	.double	0xb6c000+PG_RW+PG_V	# memory page
	.double	0xb6d000+PG_RW+PG_V	# memory page
	.double	0xb6e000+PG_RW+PG_V	# memory page
	.double	0xb6f000+PG_RW+PG_V	# memory page
	.double	0xb70000+PG_RW+PG_V	# memory page
	.double	0xb71000+PG_RW+PG_V	# memory page
	.double	0xb72000+PG_RW+PG_V	# memory page
	.double	0xb73000+PG_RW+PG_V	# memory page
	.double	0xb74000+PG_RW+PG_V	# memory page
	.double	0xb75000+PG_RW+PG_V	# memory page
	.double	0xb76000+PG_RW+PG_V	# memory page
	.double	0xb77000+PG_RW+PG_V	# memory page
	.double	0xb78000+PG_RW+PG_V	# memory page
	.double	0xb79000+PG_RW+PG_V	# memory page
	.double	0xb7a000+PG_RW+PG_V	# memory page
	.double	0xb7b000+PG_RW+PG_V	# memory page
	.double	0xb7c000+PG_RW+PG_V	# memory page
	.double	0xb7d000+PG_RW+PG_V	# memory page
	.double	0xb7e000+PG_RW+PG_V	# memory page
	.double	0xb7f000+PG_RW+PG_V	# memory page
	.double	0xb80000+PG_RW+PG_V	# memory page
	.double	0xb81000+PG_RW+PG_V	# memory page
	.double	0xb82000+PG_RW+PG_V	# memory page
	.double	0xb83000+PG_RW+PG_V	# memory page
	.double	0xb84000+PG_RW+PG_V	# memory page
	.double	0xb85000+PG_RW+PG_V	# memory page
	.double	0xb86000+PG_RW+PG_V	# memory page
	.double	0xb87000+PG_RW+PG_V	# memory page
	.double	0xb88000+PG_RW+PG_V	# memory page
	.double	0xb89000+PG_RW+PG_V	# memory page
	.double	0xb8a000+PG_RW+PG_V	# memory page
	.double	0xb8b000+PG_RW+PG_V	# memory page
	.double	0xb8c000+PG_RW+PG_V	# memory page
	.double	0xb8d000+PG_RW+PG_V	# memory page
	.double	0xb8e000+PG_RW+PG_V	# memory page
	.double	0xb8f000+PG_RW+PG_V	# memory page
	.double	0xb90000+PG_RW+PG_V	# memory page
	.double	0xb91000+PG_RW+PG_V	# memory page
	.double	0xb92000+PG_RW+PG_V	# memory page
	.double	0xb93000+PG_RW+PG_V	# memory page
	.double	0xb94000+PG_RW+PG_V	# memory page
	.double	0xb95000+PG_RW+PG_V	# memory page
	.double	0xb96000+PG_RW+PG_V	# memory page
	.double	0xb97000+PG_RW+PG_V	# memory page
	.double	0xb98000+PG_RW+PG_V	# memory page
	.double	0xb99000+PG_RW+PG_V	# memory page
	.double	0xb9a000+PG_RW+PG_V	# memory page
	.double	0xb9b000+PG_RW+PG_V	# memory page
	.double	0xb9c000+PG_RW+PG_V	# memory page
	.double	0xb9d000+PG_RW+PG_V	# memory page
	.double	0xb9e000+PG_RW+PG_V	# memory page
	.double	0xb9f000+PG_RW+PG_V	# memory page
	.double	0xba0000+PG_RW+PG_V	# memory page
	.double	0xba1000+PG_RW+PG_V	# memory page
	.double	0xba2000+PG_RW+PG_V	# memory page
	.double	0xba3000+PG_RW+PG_V	# memory page
	.double	0xba4000+PG_RW+PG_V	# memory page
	.double	0xba5000+PG_RW+PG_V	# memory page
	.double	0xba6000+PG_RW+PG_V	# memory page
	.double	0xba7000+PG_RW+PG_V	# memory page
	.double	0xba8000+PG_RW+PG_V	# memory page
	.double	0xba9000+PG_RW+PG_V	# memory page
	.double	0xbaa000+PG_RW+PG_V	# memory page
	.double	0xbab000+PG_RW+PG_V	# memory page
	.double	0xbac000+PG_RW+PG_V	# memory page
	.double	0xbad000+PG_RW+PG_V	# memory page
	.double	0xbae000+PG_RW+PG_V	# memory page
	.double	0xbaf000+PG_RW+PG_V	# memory page
	.double	0xbb0000+PG_RW+PG_V	# memory page
	.double	0xbb1000+PG_RW+PG_V	# memory page
	.double	0xbb2000+PG_RW+PG_V	# memory page
	.double	0xbb3000+PG_RW+PG_V	# memory page
	.double	0xbb4000+PG_RW+PG_V	# memory page
	.double	0xbb5000+PG_RW+PG_V	# memory page
	.double	0xbb6000+PG_RW+PG_V	# memory page
	.double	0xbb7000+PG_RW+PG_V	# memory page
	.double	0xbb8000+PG_RW+PG_V	# memory page
	.double	0xbb9000+PG_RW+PG_V	# memory page
	.double	0xbba000+PG_RW+PG_V	# memory page
	.double	0xbbb000+PG_RW+PG_V	# memory page
	.double	0xbbc000+PG_RW+PG_V	# memory page
	.double	0xbbd000+PG_RW+PG_V	# memory page
	.double	0xbbe000+PG_RW+PG_V	# memory page
	.double	0xbbf000+PG_RW+PG_V	# memory page
	.double	0xbc0000+PG_RW+PG_V	# memory page
	.double	0xbc1000+PG_RW+PG_V	# memory page
	.double	0xbc2000+PG_RW+PG_V	# memory page
	.double	0xbc3000+PG_RW+PG_V	# memory page
	.double	0xbc4000+PG_RW+PG_V	# memory page
	.double	0xbc5000+PG_RW+PG_V	# memory page
	.double	0xbc6000+PG_RW+PG_V	# memory page
	.double	0xbc7000+PG_RW+PG_V	# memory page
	.double	0xbc8000+PG_RW+PG_V	# memory page
	.double	0xbc9000+PG_RW+PG_V	# memory page
	.double	0xbca000+PG_RW+PG_V	# memory page
	.double	0xbcb000+PG_RW+PG_V	# memory page
	.double	0xbcc000+PG_RW+PG_V	# memory page
	.double	0xbcd000+PG_RW+PG_V	# memory page
	.double	0xbce000+PG_RW+PG_V	# memory page
	.double	0xbcf000+PG_RW+PG_V	# memory page
	.double	0xbd0000+PG_RW+PG_V	# memory page
	.double	0xbd1000+PG_RW+PG_V	# memory page
	.double	0xbd2000+PG_RW+PG_V	# memory page
	.double	0xbd3000+PG_RW+PG_V	# memory page
	.double	0xbd4000+PG_RW+PG_V	# memory page
	.double	0xbd5000+PG_RW+PG_V	# memory page
	.double	0xbd6000+PG_RW+PG_V	# memory page
	.double	0xbd7000+PG_RW+PG_V	# memory page
	.double	0xbd8000+PG_RW+PG_V	# memory page
	.double	0xbd9000+PG_RW+PG_V	# memory page
	.double	0xbda000+PG_RW+PG_V	# memory page
	.double	0xbdb000+PG_RW+PG_V	# memory page
	.double	0xbdc000+PG_RW+PG_V	# memory page
	.double	0xbdd000+PG_RW+PG_V	# memory page
	.double	0xbde000+PG_RW+PG_V	# memory page
	.double	0xbdf000+PG_RW+PG_V	# memory page
	.double	0xbe0000+PG_RW+PG_V	# memory page
	.double	0xbe1000+PG_RW+PG_V	# memory page
	.double	0xbe2000+PG_RW+PG_V	# memory page
	.double	0xbe3000+PG_RW+PG_V	# memory page
	.double	0xbe4000+PG_RW+PG_V	# memory page
	.double	0xbe5000+PG_RW+PG_V	# memory page
	.double	0xbe6000+PG_RW+PG_V	# memory page
	.double	0xbe7000+PG_RW+PG_V	# memory page
	.double	0xbe8000+PG_RW+PG_V	# memory page
	.double	0xbe9000+PG_RW+PG_V	# memory page
	.double	0xbea000+PG_RW+PG_V	# memory page
	.double	0xbeb000+PG_RW+PG_V	# memory page
	.double	0xbec000+PG_RW+PG_V	# memory page
	.double	0xbed000+PG_RW+PG_V	# memory page
	.double	0xbee000+PG_RW+PG_V	# memory page
	.double	0xbef000+PG_RW+PG_V	# memory page
	.double	0xbf0000+PG_RW+PG_V	# memory page
	.double	0xbf1000+PG_RW+PG_V	# memory page
	.double	0xbf2000+PG_RW+PG_V	# memory page
	.double	0xbf3000+PG_RW+PG_V	# memory page
	.double	0xbf4000+PG_RW+PG_V	# memory page
	.double	0xbf5000+PG_RW+PG_V	# memory page
	.double	0xbf6000+PG_RW+PG_V	# memory page
	.double	0xbf7000+PG_RW+PG_V	# memory page
	.double	0xbf8000+PG_RW+PG_V	# memory page
	.double	0xbf9000+PG_RW+PG_V	# memory page
	.double	0xbfa000+PG_RW+PG_V	# memory page
	.double	0xbfb000+PG_RW+PG_V	# memory page
	.double	0xbfc000+PG_RW+PG_V	# memory page
	.double	0xbfd000+PG_RW+PG_V	# memory page
	.double	0xbfe000+PG_RW+PG_V	# memory page
	.double	0xbff000+PG_RW+PG_V	# memory page

# PAGE 7
# 2nd level page table entries (12MB-16MB)
paget3:
	.double	0xc00000+PG_RW+PG_V	# memory page
	.double	0xc01000+PG_RW+PG_V	# memory page
	.double	0xc02000+PG_RW+PG_V	# memory page
	.double	0xc03000+PG_RW+PG_V	# memory page
	.double	0xc04000+PG_RW+PG_V	# memory page
	.double	0xc05000+PG_RW+PG_V	# memory page
	.double	0xc06000+PG_RW+PG_V	# memory page
	.double	0xc07000+PG_RW+PG_V	# memory page
	.double	0xc08000+PG_RW+PG_V	# memory page
	.double	0xc09000+PG_RW+PG_V	# memory page
	.double	0xc0a000+PG_RW+PG_V	# memory page
	.double	0xc0b000+PG_RW+PG_V	# memory page
	.double	0xc0c000+PG_RW+PG_V	# memory page
	.double	0xc0d000+PG_RW+PG_V	# memory page
	.double	0xc0e000+PG_RW+PG_V	# memory page
	.double	0xc0f000+PG_RW+PG_V	# memory page
	.double	0xc10000+PG_RW+PG_V	# memory page
	.double	0xc11000+PG_RW+PG_V	# memory page
	.double	0xc12000+PG_RW+PG_V	# memory page
	.double	0xc13000+PG_RW+PG_V	# memory page
	.double	0xc14000+PG_RW+PG_V	# memory page
	.double	0xc15000+PG_RW+PG_V	# memory page
	.double	0xc16000+PG_RW+PG_V	# memory page
	.double	0xc17000+PG_RW+PG_V	# memory page
	.double	0xc18000+PG_RW+PG_V	# memory page
	.double	0xc19000+PG_RW+PG_V	# memory page
	.double	0xc1a000+PG_RW+PG_V	# memory page
	.double	0xc1b000+PG_RW+PG_V	# memory page
	.double	0xc1c000+PG_RW+PG_V	# memory page
	.double	0xc1d000+PG_RW+PG_V	# memory page
	.double	0xc1e000+PG_RW+PG_V	# memory page
	.double	0xc1f000+PG_RW+PG_V	# memory page
	.double	0xc20000+PG_RW+PG_V	# memory page
	.double	0xc21000+PG_RW+PG_V	# memory page
	.double	0xc22000+PG_RW+PG_V	# memory page
	.double	0xc23000+PG_RW+PG_V	# memory page
	.double	0xc24000+PG_RW+PG_V	# memory page
	.double	0xc25000+PG_RW+PG_V	# memory page
	.double	0xc26000+PG_RW+PG_V	# memory page
	.double	0xc27000+PG_RW+PG_V	# memory page
	.double	0xc28000+PG_RW+PG_V	# memory page
	.double	0xc29000+PG_RW+PG_V	# memory page
	.double	0xc2a000+PG_RW+PG_V	# memory page
	.double	0xc2b000+PG_RW+PG_V	# memory page
	.double	0xc2c000+PG_RW+PG_V	# memory page
	.double	0xc2d000+PG_RW+PG_V	# memory page
	.double	0xc2e000+PG_RW+PG_V	# memory page
	.double	0xc2f000+PG_RW+PG_V	# memory page
	.double	0xc30000+PG_RW+PG_V	# memory page
	.double	0xc31000+PG_RW+PG_V	# memory page
	.double	0xc32000+PG_RW+PG_V	# memory page
	.double	0xc33000+PG_RW+PG_V	# memory page
	.double	0xc34000+PG_RW+PG_V	# memory page
	.double	0xc35000+PG_RW+PG_V	# memory page
	.double	0xc36000+PG_RW+PG_V	# memory page
	.double	0xc37000+PG_RW+PG_V	# memory page
	.double	0xc38000+PG_RW+PG_V	# memory page
	.double	0xc39000+PG_RW+PG_V	# memory page
	.double	0xc3a000+PG_RW+PG_V	# memory page
	.double	0xc3b000+PG_RW+PG_V	# memory page
	.double	0xc3c000+PG_RW+PG_V	# memory page
	.double	0xc3d000+PG_RW+PG_V	# memory page
	.double	0xc3e000+PG_RW+PG_V	# memory page
	.double	0xc3f000+PG_RW+PG_V	# memory page
	.double	0xc40000+PG_RW+PG_V	# memory page
	.double	0xc41000+PG_RW+PG_V	# memory page
	.double	0xc42000+PG_RW+PG_V	# memory page
	.double	0xc43000+PG_RW+PG_V	# memory page
	.double	0xc44000+PG_RW+PG_V	# memory page
	.double	0xc45000+PG_RW+PG_V	# memory page
	.double	0xc46000+PG_RW+PG_V	# memory page
	.double	0xc47000+PG_RW+PG_V	# memory page
	.double	0xc48000+PG_RW+PG_V	# memory page
	.double	0xc49000+PG_RW+PG_V	# memory page
	.double	0xc4a000+PG_RW+PG_V	# memory page
	.double	0xc4b000+PG_RW+PG_V	# memory page
	.double	0xc4c000+PG_RW+PG_V	# memory page
	.double	0xc4d000+PG_RW+PG_V	# memory page
	.double	0xc4e000+PG_RW+PG_V	# memory page
	.double	0xc4f000+PG_RW+PG_V	# memory page
	.double	0xc50000+PG_RW+PG_V	# memory page
	.double	0xc51000+PG_RW+PG_V	# memory page
	.double	0xc52000+PG_RW+PG_V	# memory page
	.double	0xc53000+PG_RW+PG_V	# memory page
	.double	0xc54000+PG_RW+PG_V	# memory page
	.double	0xc55000+PG_RW+PG_V	# memory page
	.double	0xc56000+PG_RW+PG_V	# memory page
	.double	0xc57000+PG_RW+PG_V	# memory page
	.double	0xc58000+PG_RW+PG_V	# memory page
	.double	0xc59000+PG_RW+PG_V	# memory page
	.double	0xc5a000+PG_RW+PG_V	# memory page
	.double	0xc5b000+PG_RW+PG_V	# memory page
	.double	0xc5c000+PG_RW+PG_V	# memory page
	.double	0xc5d000+PG_RW+PG_V	# memory page
	.double	0xc5e000+PG_RW+PG_V	# memory page
	.double	0xc5f000+PG_RW+PG_V	# memory page
	.double	0xc60000+PG_RW+PG_V	# memory page
	.double	0xc61000+PG_RW+PG_V	# memory page
	.double	0xc62000+PG_RW+PG_V	# memory page
	.double	0xc63000+PG_RW+PG_V	# memory page
	.double	0xc64000+PG_RW+PG_V	# memory page
	.double	0xc65000+PG_RW+PG_V	# memory page
	.double	0xc66000+PG_RW+PG_V	# memory page
	.double	0xc67000+PG_RW+PG_V	# memory page
	.double	0xc68000+PG_RW+PG_V	# memory page
	.double	0xc69000+PG_RW+PG_V	# memory page
	.double	0xc6a000+PG_RW+PG_V	# memory page
	.double	0xc6b000+PG_RW+PG_V	# memory page
	.double	0xc6c000+PG_RW+PG_V	# memory page
	.double	0xc6d000+PG_RW+PG_V	# memory page
	.double	0xc6e000+PG_RW+PG_V	# memory page
	.double	0xc6f000+PG_RW+PG_V	# memory page
	.double	0xc70000+PG_RW+PG_V	# memory page
	.double	0xc71000+PG_RW+PG_V	# memory page
	.double	0xc72000+PG_RW+PG_V	# memory page
	.double	0xc73000+PG_RW+PG_V	# memory page
	.double	0xc74000+PG_RW+PG_V	# memory page
	.double	0xc75000+PG_RW+PG_V	# memory page
	.double	0xc76000+PG_RW+PG_V	# memory page
	.double	0xc77000+PG_RW+PG_V	# memory page
	.double	0xc78000+PG_RW+PG_V	# memory page
	.double	0xc79000+PG_RW+PG_V	# memory page
	.double	0xc7a000+PG_RW+PG_V	# memory page
	.double	0xc7b000+PG_RW+PG_V	# memory page
	.double	0xc7c000+PG_RW+PG_V	# memory page
	.double	0xc7d000+PG_RW+PG_V	# memory page
	.double	0xc7e000+PG_RW+PG_V	# memory page
	.double	0xc7f000+PG_RW+PG_V	# memory page
	.double	0xc80000+PG_RW+PG_V	# memory page
	.double	0xc81000+PG_RW+PG_V	# memory page
	.double	0xc82000+PG_RW+PG_V	# memory page
	.double	0xc83000+PG_RW+PG_V	# memory page
	.double	0xc84000+PG_RW+PG_V	# memory page
	.double	0xc85000+PG_RW+PG_V	# memory page
	.double	0xc86000+PG_RW+PG_V	# memory page
	.double	0xc87000+PG_RW+PG_V	# memory page
	.double	0xc88000+PG_RW+PG_V	# memory page
	.double	0xc89000+PG_RW+PG_V	# memory page
	.double	0xc8a000+PG_RW+PG_V	# memory page
	.double	0xc8b000+PG_RW+PG_V	# memory page
	.double	0xc8c000+PG_RW+PG_V	# memory page
	.double	0xc8d000+PG_RW+PG_V	# memory page
	.double	0xc8e000+PG_RW+PG_V	# memory page
	.double	0xc8f000+PG_RW+PG_V	# memory page
	.double	0xc90000+PG_RW+PG_V	# memory page
	.double	0xc91000+PG_RW+PG_V	# memory page
	.double	0xc92000+PG_RW+PG_V	# memory page
	.double	0xc93000+PG_RW+PG_V	# memory page
	.double	0xc94000+PG_RW+PG_V	# memory page
	.double	0xc95000+PG_RW+PG_V	# memory page
	.double	0xc96000+PG_RW+PG_V	# memory page
	.double	0xc97000+PG_RW+PG_V	# memory page
	.double	0xc98000+PG_RW+PG_V	# memory page
	.double	0xc99000+PG_RW+PG_V	# memory page
	.double	0xc9a000+PG_RW+PG_V	# memory page
	.double	0xc9b000+PG_RW+PG_V	# memory page
	.double	0xc9c000+PG_RW+PG_V	# memory page
	.double	0xc9d000+PG_RW+PG_V	# memory page
	.double	0xc9e000+PG_RW+PG_V	# memory page
	.double	0xc9f000+PG_RW+PG_V	# memory page
	.double	0xca0000+PG_RW+PG_V	# memory page
	.double	0xca1000+PG_RW+PG_V	# memory page
	.double	0xca2000+PG_RW+PG_V	# memory page
	.double	0xca3000+PG_RW+PG_V	# memory page
	.double	0xca4000+PG_RW+PG_V	# memory page
	.double	0xca5000+PG_RW+PG_V	# memory page
	.double	0xca6000+PG_RW+PG_V	# memory page
	.double	0xca7000+PG_RW+PG_V	# memory page
	.double	0xca8000+PG_RW+PG_V	# memory page
	.double	0xca9000+PG_RW+PG_V	# memory page
	.double	0xcaa000+PG_RW+PG_V	# memory page
	.double	0xcab000+PG_RW+PG_V	# memory page
	.double	0xcac000+PG_RW+PG_V	# memory page
	.double	0xcad000+PG_RW+PG_V	# memory page
	.double	0xcae000+PG_RW+PG_V	# memory page
	.double	0xcaf000+PG_RW+PG_V	# memory page
	.double	0xcb0000+PG_RW+PG_V	# memory page
	.double	0xcb1000+PG_RW+PG_V	# memory page
	.double	0xcb2000+PG_RW+PG_V	# memory page
	.double	0xcb3000+PG_RW+PG_V	# memory page
	.double	0xcb4000+PG_RW+PG_V	# memory page
	.double	0xcb5000+PG_RW+PG_V	# memory page
	.double	0xcb6000+PG_RW+PG_V	# memory page
	.double	0xcb7000+PG_RW+PG_V	# memory page
	.double	0xcb8000+PG_RW+PG_V	# memory page
	.double	0xcb9000+PG_RW+PG_V	# memory page
	.double	0xcba000+PG_RW+PG_V	# memory page
	.double	0xcbb000+PG_RW+PG_V	# memory page
	.double	0xcbc000+PG_RW+PG_V	# memory page
	.double	0xcbd000+PG_RW+PG_V	# memory page
	.double	0xcbe000+PG_RW+PG_V	# memory page
	.double	0xcbf000+PG_RW+PG_V	# memory page
	.double	0xcc0000+PG_RW+PG_V	# memory page
	.double	0xcc1000+PG_RW+PG_V	# memory page
	.double	0xcc2000+PG_RW+PG_V	# memory page
	.double	0xcc3000+PG_RW+PG_V	# memory page
	.double	0xcc4000+PG_RW+PG_V	# memory page
	.double	0xcc5000+PG_RW+PG_V	# memory page
	.double	0xcc6000+PG_RW+PG_V	# memory page
	.double	0xcc7000+PG_RW+PG_V	# memory page
	.double	0xcc8000+PG_RW+PG_V	# memory page
	.double	0xcc9000+PG_RW+PG_V	# memory page
	.double	0xcca000+PG_RW+PG_V	# memory page
	.double	0xccb000+PG_RW+PG_V	# memory page
	.double	0xccc000+PG_RW+PG_V	# memory page
	.double	0xccd000+PG_RW+PG_V	# memory page
	.double	0xcce000+PG_RW+PG_V	# memory page
	.double	0xccf000+PG_RW+PG_V	# memory page
	.double	0xcd0000+PG_RW+PG_V	# memory page
	.double	0xcd1000+PG_RW+PG_V	# memory page
	.double	0xcd2000+PG_RW+PG_V	# memory page
	.double	0xcd3000+PG_RW+PG_V	# memory page
	.double	0xcd4000+PG_RW+PG_V	# memory page
	.double	0xcd5000+PG_RW+PG_V	# memory page
	.double	0xcd6000+PG_RW+PG_V	# memory page
	.double	0xcd7000+PG_RW+PG_V	# memory page
	.double	0xcd8000+PG_RW+PG_V	# memory page
	.double	0xcd9000+PG_RW+PG_V	# memory page
	.double	0xcda000+PG_RW+PG_V	# memory page
	.double	0xcdb000+PG_RW+PG_V	# memory page
	.double	0xcdc000+PG_RW+PG_V	# memory page
	.double	0xcdd000+PG_RW+PG_V	# memory page
	.double	0xcde000+PG_RW+PG_V	# memory page
	.double	0xcdf000+PG_RW+PG_V	# memory page
	.double	0xce0000+PG_RW+PG_V	# memory page
	.double	0xce1000+PG_RW+PG_V	# memory page
	.double	0xce2000+PG_RW+PG_V	# memory page
	.double	0xce3000+PG_RW+PG_V	# memory page
	.double	0xce4000+PG_RW+PG_V	# memory page
	.double	0xce5000+PG_RW+PG_V	# memory page
	.double	0xce6000+PG_RW+PG_V	# memory page
	.double	0xce7000+PG_RW+PG_V	# memory page
	.double	0xce8000+PG_RW+PG_V	# memory page
	.double	0xce9000+PG_RW+PG_V	# memory page
	.double	0xcea000+PG_RW+PG_V	# memory page
	.double	0xceb000+PG_RW+PG_V	# memory page
	.double	0xcec000+PG_RW+PG_V	# memory page
	.double	0xced000+PG_RW+PG_V	# memory page
	.double	0xcee000+PG_RW+PG_V	# memory page
	.double	0xcef000+PG_RW+PG_V	# memory page
	.double	0xcf0000+PG_RW+PG_V	# memory page
	.double	0xcf1000+PG_RW+PG_V	# memory page
	.double	0xcf2000+PG_RW+PG_V	# memory page
	.double	0xcf3000+PG_RW+PG_V	# memory page
	.double	0xcf4000+PG_RW+PG_V	# memory page
	.double	0xcf5000+PG_RW+PG_V	# memory page
	.double	0xcf6000+PG_RW+PG_V	# memory page
	.double	0xcf7000+PG_RW+PG_V	# memory page
	.double	0xcf8000+PG_RW+PG_V	# memory page
	.double	0xcf9000+PG_RW+PG_V	# memory page
	.double	0xcfa000+PG_RW+PG_V	# memory page
	.double	0xcfb000+PG_RW+PG_V	# memory page
	.double	0xcfc000+PG_RW+PG_V	# memory page
	.double	0xcfd000+PG_RW+PG_V	# memory page
	.double	0xcfe000+PG_RW+PG_V	# memory page
	.double	0xcff000+PG_RW+PG_V	# memory page
	.double	0xd00000+PG_RW+PG_V	# memory page
	.double	0xd01000+PG_RW+PG_V	# memory page
	.double	0xd02000+PG_RW+PG_V	# memory page
	.double	0xd03000+PG_RW+PG_V	# memory page
	.double	0xd04000+PG_RW+PG_V	# memory page
	.double	0xd05000+PG_RW+PG_V	# memory page
	.double	0xd06000+PG_RW+PG_V	# memory page
	.double	0xd07000+PG_RW+PG_V	# memory page
	.double	0xd08000+PG_RW+PG_V	# memory page
	.double	0xd09000+PG_RW+PG_V	# memory page
	.double	0xd0a000+PG_RW+PG_V	# memory page
	.double	0xd0b000+PG_RW+PG_V	# memory page
	.double	0xd0c000+PG_RW+PG_V	# memory page
	.double	0xd0d000+PG_RW+PG_V	# memory page
	.double	0xd0e000+PG_RW+PG_V	# memory page
	.double	0xd0f000+PG_RW+PG_V	# memory page
	.double	0xd10000+PG_RW+PG_V	# memory page
	.double	0xd11000+PG_RW+PG_V	# memory page
	.double	0xd12000+PG_RW+PG_V	# memory page
	.double	0xd13000+PG_RW+PG_V	# memory page
	.double	0xd14000+PG_RW+PG_V	# memory page
	.double	0xd15000+PG_RW+PG_V	# memory page
	.double	0xd16000+PG_RW+PG_V	# memory page
	.double	0xd17000+PG_RW+PG_V	# memory page
	.double	0xd18000+PG_RW+PG_V	# memory page
	.double	0xd19000+PG_RW+PG_V	# memory page
	.double	0xd1a000+PG_RW+PG_V	# memory page
	.double	0xd1b000+PG_RW+PG_V	# memory page
	.double	0xd1c000+PG_RW+PG_V	# memory page
	.double	0xd1d000+PG_RW+PG_V	# memory page
	.double	0xd1e000+PG_RW+PG_V	# memory page
	.double	0xd1f000+PG_RW+PG_V	# memory page
	.double	0xd20000+PG_RW+PG_V	# memory page
	.double	0xd21000+PG_RW+PG_V	# memory page
	.double	0xd22000+PG_RW+PG_V	# memory page
	.double	0xd23000+PG_RW+PG_V	# memory page
	.double	0xd24000+PG_RW+PG_V	# memory page
	.double	0xd25000+PG_RW+PG_V	# memory page
	.double	0xd26000+PG_RW+PG_V	# memory page
	.double	0xd27000+PG_RW+PG_V	# memory page
	.double	0xd28000+PG_RW+PG_V	# memory page
	.double	0xd29000+PG_RW+PG_V	# memory page
	.double	0xd2a000+PG_RW+PG_V	# memory page
	.double	0xd2b000+PG_RW+PG_V	# memory page
	.double	0xd2c000+PG_RW+PG_V	# memory page
	.double	0xd2d000+PG_RW+PG_V	# memory page
	.double	0xd2e000+PG_RW+PG_V	# memory page
	.double	0xd2f000+PG_RW+PG_V	# memory page
	.double	0xd30000+PG_RW+PG_V	# memory page
	.double	0xd31000+PG_RW+PG_V	# memory page
	.double	0xd32000+PG_RW+PG_V	# memory page
	.double	0xd33000+PG_RW+PG_V	# memory page
	.double	0xd34000+PG_RW+PG_V	# memory page
	.double	0xd35000+PG_RW+PG_V	# memory page
	.double	0xd36000+PG_RW+PG_V	# memory page
	.double	0xd37000+PG_RW+PG_V	# memory page
	.double	0xd38000+PG_RW+PG_V	# memory page
	.double	0xd39000+PG_RW+PG_V	# memory page
	.double	0xd3a000+PG_RW+PG_V	# memory page
	.double	0xd3b000+PG_RW+PG_V	# memory page
	.double	0xd3c000+PG_RW+PG_V	# memory page
	.double	0xd3d000+PG_RW+PG_V	# memory page
	.double	0xd3e000+PG_RW+PG_V	# memory page
	.double	0xd3f000+PG_RW+PG_V	# memory page
	.double	0xd40000+PG_RW+PG_V	# memory page
	.double	0xd41000+PG_RW+PG_V	# memory page
	.double	0xd42000+PG_RW+PG_V	# memory page
	.double	0xd43000+PG_RW+PG_V	# memory page
	.double	0xd44000+PG_RW+PG_V	# memory page
	.double	0xd45000+PG_RW+PG_V	# memory page
	.double	0xd46000+PG_RW+PG_V	# memory page
	.double	0xd47000+PG_RW+PG_V	# memory page
	.double	0xd48000+PG_RW+PG_V	# memory page
	.double	0xd49000+PG_RW+PG_V	# memory page
	.double	0xd4a000+PG_RW+PG_V	# memory page
	.double	0xd4b000+PG_RW+PG_V	# memory page
	.double	0xd4c000+PG_RW+PG_V	# memory page
	.double	0xd4d000+PG_RW+PG_V	# memory page
	.double	0xd4e000+PG_RW+PG_V	# memory page
	.double	0xd4f000+PG_RW+PG_V	# memory page
	.double	0xd50000+PG_RW+PG_V	# memory page
	.double	0xd51000+PG_RW+PG_V	# memory page
	.double	0xd52000+PG_RW+PG_V	# memory page
	.double	0xd53000+PG_RW+PG_V	# memory page
	.double	0xd54000+PG_RW+PG_V	# memory page
	.double	0xd55000+PG_RW+PG_V	# memory page
	.double	0xd56000+PG_RW+PG_V	# memory page
	.double	0xd57000+PG_RW+PG_V	# memory page
	.double	0xd58000+PG_RW+PG_V	# memory page
	.double	0xd59000+PG_RW+PG_V	# memory page
	.double	0xd5a000+PG_RW+PG_V	# memory page
	.double	0xd5b000+PG_RW+PG_V	# memory page
	.double	0xd5c000+PG_RW+PG_V	# memory page
	.double	0xd5d000+PG_RW+PG_V	# memory page
	.double	0xd5e000+PG_RW+PG_V	# memory page
	.double	0xd5f000+PG_RW+PG_V	# memory page
	.double	0xd60000+PG_RW+PG_V	# memory page
	.double	0xd61000+PG_RW+PG_V	# memory page
	.double	0xd62000+PG_RW+PG_V	# memory page
	.double	0xd63000+PG_RW+PG_V	# memory page
	.double	0xd64000+PG_RW+PG_V	# memory page
	.double	0xd65000+PG_RW+PG_V	# memory page
	.double	0xd66000+PG_RW+PG_V	# memory page
	.double	0xd67000+PG_RW+PG_V	# memory page
	.double	0xd68000+PG_RW+PG_V	# memory page
	.double	0xd69000+PG_RW+PG_V	# memory page
	.double	0xd6a000+PG_RW+PG_V	# memory page
	.double	0xd6b000+PG_RW+PG_V	# memory page
	.double	0xd6c000+PG_RW+PG_V	# memory page
	.double	0xd6d000+PG_RW+PG_V	# memory page
	.double	0xd6e000+PG_RW+PG_V	# memory page
	.double	0xd6f000+PG_RW+PG_V	# memory page
	.double	0xd70000+PG_RW+PG_V	# memory page
	.double	0xd71000+PG_RW+PG_V	# memory page
	.double	0xd72000+PG_RW+PG_V	# memory page
	.double	0xd73000+PG_RW+PG_V	# memory page
	.double	0xd74000+PG_RW+PG_V	# memory page
	.double	0xd75000+PG_RW+PG_V	# memory page
	.double	0xd76000+PG_RW+PG_V	# memory page
	.double	0xd77000+PG_RW+PG_V	# memory page
	.double	0xd78000+PG_RW+PG_V	# memory page
	.double	0xd79000+PG_RW+PG_V	# memory page
	.double	0xd7a000+PG_RW+PG_V	# memory page
	.double	0xd7b000+PG_RW+PG_V	# memory page
	.double	0xd7c000+PG_RW+PG_V	# memory page
	.double	0xd7d000+PG_RW+PG_V	# memory page
	.double	0xd7e000+PG_RW+PG_V	# memory page
	.double	0xd7f000+PG_RW+PG_V	# memory page
	.double	0xd80000+PG_RW+PG_V	# memory page
	.double	0xd81000+PG_RW+PG_V	# memory page
	.double	0xd82000+PG_RW+PG_V	# memory page
	.double	0xd83000+PG_RW+PG_V	# memory page
	.double	0xd84000+PG_RW+PG_V	# memory page
	.double	0xd85000+PG_RW+PG_V	# memory page
	.double	0xd86000+PG_RW+PG_V	# memory page
	.double	0xd87000+PG_RW+PG_V	# memory page
	.double	0xd88000+PG_RW+PG_V	# memory page
	.double	0xd89000+PG_RW+PG_V	# memory page
	.double	0xd8a000+PG_RW+PG_V	# memory page
	.double	0xd8b000+PG_RW+PG_V	# memory page
	.double	0xd8c000+PG_RW+PG_V	# memory page
	.double	0xd8d000+PG_RW+PG_V	# memory page
	.double	0xd8e000+PG_RW+PG_V	# memory page
	.double	0xd8f000+PG_RW+PG_V	# memory page
	.double	0xd90000+PG_RW+PG_V	# memory page
	.double	0xd91000+PG_RW+PG_V	# memory page
	.double	0xd92000+PG_RW+PG_V	# memory page
	.double	0xd93000+PG_RW+PG_V	# memory page
	.double	0xd94000+PG_RW+PG_V	# memory page
	.double	0xd95000+PG_RW+PG_V	# memory page
	.double	0xd96000+PG_RW+PG_V	# memory page
	.double	0xd97000+PG_RW+PG_V	# memory page
	.double	0xd98000+PG_RW+PG_V	# memory page
	.double	0xd99000+PG_RW+PG_V	# memory page
	.double	0xd9a000+PG_RW+PG_V	# memory page
	.double	0xd9b000+PG_RW+PG_V	# memory page
	.double	0xd9c000+PG_RW+PG_V	# memory page
	.double	0xd9d000+PG_RW+PG_V	# memory page
	.double	0xd9e000+PG_RW+PG_V	# memory page
	.double	0xd9f000+PG_RW+PG_V	# memory page
	.double	0xda0000+PG_RW+PG_V	# memory page
	.double	0xda1000+PG_RW+PG_V	# memory page
	.double	0xda2000+PG_RW+PG_V	# memory page
	.double	0xda3000+PG_RW+PG_V	# memory page
	.double	0xda4000+PG_RW+PG_V	# memory page
	.double	0xda5000+PG_RW+PG_V	# memory page
	.double	0xda6000+PG_RW+PG_V	# memory page
	.double	0xda7000+PG_RW+PG_V	# memory page
	.double	0xda8000+PG_RW+PG_V	# memory page
	.double	0xda9000+PG_RW+PG_V	# memory page
	.double	0xdaa000+PG_RW+PG_V	# memory page
	.double	0xdab000+PG_RW+PG_V	# memory page
	.double	0xdac000+PG_RW+PG_V	# memory page
	.double	0xdad000+PG_RW+PG_V	# memory page
	.double	0xdae000+PG_RW+PG_V	# memory page
	.double	0xdaf000+PG_RW+PG_V	# memory page
	.double	0xdb0000+PG_RW+PG_V	# memory page
	.double	0xdb1000+PG_RW+PG_V	# memory page
	.double	0xdb2000+PG_RW+PG_V	# memory page
	.double	0xdb3000+PG_RW+PG_V	# memory page
	.double	0xdb4000+PG_RW+PG_V	# memory page
	.double	0xdb5000+PG_RW+PG_V	# memory page
	.double	0xdb6000+PG_RW+PG_V	# memory page
	.double	0xdb7000+PG_RW+PG_V	# memory page
	.double	0xdb8000+PG_RW+PG_V	# memory page
	.double	0xdb9000+PG_RW+PG_V	# memory page
	.double	0xdba000+PG_RW+PG_V	# memory page
	.double	0xdbb000+PG_RW+PG_V	# memory page
	.double	0xdbc000+PG_RW+PG_V	# memory page
	.double	0xdbd000+PG_RW+PG_V	# memory page
	.double	0xdbe000+PG_RW+PG_V	# memory page
	.double	0xdbf000+PG_RW+PG_V	# memory page
	.double	0xdc0000+PG_RW+PG_V	# memory page
	.double	0xdc1000+PG_RW+PG_V	# memory page
	.double	0xdc2000+PG_RW+PG_V	# memory page
	.double	0xdc3000+PG_RW+PG_V	# memory page
	.double	0xdc4000+PG_RW+PG_V	# memory page
	.double	0xdc5000+PG_RW+PG_V	# memory page
	.double	0xdc6000+PG_RW+PG_V	# memory page
	.double	0xdc7000+PG_RW+PG_V	# memory page
	.double	0xdc8000+PG_RW+PG_V	# memory page
	.double	0xdc9000+PG_RW+PG_V	# memory page
	.double	0xdca000+PG_RW+PG_V	# memory page
	.double	0xdcb000+PG_RW+PG_V	# memory page
	.double	0xdcc000+PG_RW+PG_V	# memory page
	.double	0xdcd000+PG_RW+PG_V	# memory page
	.double	0xdce000+PG_RW+PG_V	# memory page
	.double	0xdcf000+PG_RW+PG_V	# memory page
	.double	0xdd0000+PG_RW+PG_V	# memory page
	.double	0xdd1000+PG_RW+PG_V	# memory page
	.double	0xdd2000+PG_RW+PG_V	# memory page
	.double	0xdd3000+PG_RW+PG_V	# memory page
	.double	0xdd4000+PG_RW+PG_V	# memory page
	.double	0xdd5000+PG_RW+PG_V	# memory page
	.double	0xdd6000+PG_RW+PG_V	# memory page
	.double	0xdd7000+PG_RW+PG_V	# memory page
	.double	0xdd8000+PG_RW+PG_V	# memory page
	.double	0xdd9000+PG_RW+PG_V	# memory page
	.double	0xdda000+PG_RW+PG_V	# memory page
	.double	0xddb000+PG_RW+PG_V	# memory page
	.double	0xddc000+PG_RW+PG_V	# memory page
	.double	0xddd000+PG_RW+PG_V	# memory page
	.double	0xdde000+PG_RW+PG_V	# memory page
	.double	0xddf000+PG_RW+PG_V	# memory page
	.double	0xde0000+PG_RW+PG_V	# memory page
	.double	0xde1000+PG_RW+PG_V	# memory page
	.double	0xde2000+PG_RW+PG_V	# memory page
	.double	0xde3000+PG_RW+PG_V	# memory page
	.double	0xde4000+PG_RW+PG_V	# memory page
	.double	0xde5000+PG_RW+PG_V	# memory page
	.double	0xde6000+PG_RW+PG_V	# memory page
	.double	0xde7000+PG_RW+PG_V	# memory page
	.double	0xde8000+PG_RW+PG_V	# memory page
	.double	0xde9000+PG_RW+PG_V	# memory page
	.double	0xdea000+PG_RW+PG_V	# memory page
	.double	0xdeb000+PG_RW+PG_V	# memory page
	.double	0xdec000+PG_RW+PG_V	# memory page
	.double	0xded000+PG_RW+PG_V	# memory page
	.double	0xdee000+PG_RW+PG_V	# memory page
	.double	0xdef000+PG_RW+PG_V	# memory page
	.double	0xdf0000+PG_RW+PG_V	# memory page
	.double	0xdf1000+PG_RW+PG_V	# memory page
	.double	0xdf2000+PG_RW+PG_V	# memory page
	.double	0xdf3000+PG_RW+PG_V	# memory page
	.double	0xdf4000+PG_RW+PG_V	# memory page
	.double	0xdf5000+PG_RW+PG_V	# memory page
	.double	0xdf6000+PG_RW+PG_V	# memory page
	.double	0xdf7000+PG_RW+PG_V	# memory page
	.double	0xdf8000+PG_RW+PG_V	# memory page
	.double	0xdf9000+PG_RW+PG_V	# memory page
	.double	0xdfa000+PG_RW+PG_V	# memory page
	.double	0xdfb000+PG_RW+PG_V	# memory page
	.double	0xdfc000+PG_RW+PG_V	# memory page
	.double	0xdfd000+PG_RW+PG_V	# memory page
	.double	0xdfe000+PG_RW+PG_V	# memory page
	.double	0xdff000+PG_RW+PG_V	# memory page
	.double	0xe00000+PG_RW+PG_V	# memory page
	.double	0xe01000+PG_RW+PG_V	# memory page
	.double	0xe02000+PG_RW+PG_V	# memory page
	.double	0xe03000+PG_RW+PG_V	# memory page
	.double	0xe04000+PG_RW+PG_V	# memory page
	.double	0xe05000+PG_RW+PG_V	# memory page
	.double	0xe06000+PG_RW+PG_V	# memory page
	.double	0xe07000+PG_RW+PG_V	# memory page
	.double	0xe08000+PG_RW+PG_V	# memory page
	.double	0xe09000+PG_RW+PG_V	# memory page
	.double	0xe0a000+PG_RW+PG_V	# memory page
	.double	0xe0b000+PG_RW+PG_V	# memory page
	.double	0xe0c000+PG_RW+PG_V	# memory page
	.double	0xe0d000+PG_RW+PG_V	# memory page
	.double	0xe0e000+PG_RW+PG_V	# memory page
	.double	0xe0f000+PG_RW+PG_V	# memory page
	.double	0xe10000+PG_RW+PG_V	# memory page
	.double	0xe11000+PG_RW+PG_V	# memory page
	.double	0xe12000+PG_RW+PG_V	# memory page
	.double	0xe13000+PG_RW+PG_V	# memory page
	.double	0xe14000+PG_RW+PG_V	# memory page
	.double	0xe15000+PG_RW+PG_V	# memory page
	.double	0xe16000+PG_RW+PG_V	# memory page
	.double	0xe17000+PG_RW+PG_V	# memory page
	.double	0xe18000+PG_RW+PG_V	# memory page
	.double	0xe19000+PG_RW+PG_V	# memory page
	.double	0xe1a000+PG_RW+PG_V	# memory page
	.double	0xe1b000+PG_RW+PG_V	# memory page
	.double	0xe1c000+PG_RW+PG_V	# memory page
	.double	0xe1d000+PG_RW+PG_V	# memory page
	.double	0xe1e000+PG_RW+PG_V	# memory page
	.double	0xe1f000+PG_RW+PG_V	# memory page
	.double	0xe20000+PG_RW+PG_V	# memory page
	.double	0xe21000+PG_RW+PG_V	# memory page
	.double	0xe22000+PG_RW+PG_V	# memory page
	.double	0xe23000+PG_RW+PG_V	# memory page
	.double	0xe24000+PG_RW+PG_V	# memory page
	.double	0xe25000+PG_RW+PG_V	# memory page
	.double	0xe26000+PG_RW+PG_V	# memory page
	.double	0xe27000+PG_RW+PG_V	# memory page
	.double	0xe28000+PG_RW+PG_V	# memory page
	.double	0xe29000+PG_RW+PG_V	# memory page
	.double	0xe2a000+PG_RW+PG_V	# memory page
	.double	0xe2b000+PG_RW+PG_V	# memory page
	.double	0xe2c000+PG_RW+PG_V	# memory page
	.double	0xe2d000+PG_RW+PG_V	# memory page
	.double	0xe2e000+PG_RW+PG_V	# memory page
	.double	0xe2f000+PG_RW+PG_V	# memory page
	.double	0xe30000+PG_RW+PG_V	# memory page
	.double	0xe31000+PG_RW+PG_V	# memory page
	.double	0xe32000+PG_RW+PG_V	# memory page
	.double	0xe33000+PG_RW+PG_V	# memory page
	.double	0xe34000+PG_RW+PG_V	# memory page
	.double	0xe35000+PG_RW+PG_V	# memory page
	.double	0xe36000+PG_RW+PG_V	# memory page
	.double	0xe37000+PG_RW+PG_V	# memory page
	.double	0xe38000+PG_RW+PG_V	# memory page
	.double	0xe39000+PG_RW+PG_V	# memory page
	.double	0xe3a000+PG_RW+PG_V	# memory page
	.double	0xe3b000+PG_RW+PG_V	# memory page
	.double	0xe3c000+PG_RW+PG_V	# memory page
	.double	0xe3d000+PG_RW+PG_V	# memory page
	.double	0xe3e000+PG_RW+PG_V	# memory page
	.double	0xe3f000+PG_RW+PG_V	# memory page
	.double	0xe40000+PG_RW+PG_V	# memory page
	.double	0xe41000+PG_RW+PG_V	# memory page
	.double	0xe42000+PG_RW+PG_V	# memory page
	.double	0xe43000+PG_RW+PG_V	# memory page
	.double	0xe44000+PG_RW+PG_V	# memory page
	.double	0xe45000+PG_RW+PG_V	# memory page
	.double	0xe46000+PG_RW+PG_V	# memory page
	.double	0xe47000+PG_RW+PG_V	# memory page
	.double	0xe48000+PG_RW+PG_V	# memory page
	.double	0xe49000+PG_RW+PG_V	# memory page
	.double	0xe4a000+PG_RW+PG_V	# memory page
	.double	0xe4b000+PG_RW+PG_V	# memory page
	.double	0xe4c000+PG_RW+PG_V	# memory page
	.double	0xe4d000+PG_RW+PG_V	# memory page
	.double	0xe4e000+PG_RW+PG_V	# memory page
	.double	0xe4f000+PG_RW+PG_V	# memory page
	.double	0xe50000+PG_RW+PG_V	# memory page
	.double	0xe51000+PG_RW+PG_V	# memory page
	.double	0xe52000+PG_RW+PG_V	# memory page
	.double	0xe53000+PG_RW+PG_V	# memory page
	.double	0xe54000+PG_RW+PG_V	# memory page
	.double	0xe55000+PG_RW+PG_V	# memory page
	.double	0xe56000+PG_RW+PG_V	# memory page
	.double	0xe57000+PG_RW+PG_V	# memory page
	.double	0xe58000+PG_RW+PG_V	# memory page
	.double	0xe59000+PG_RW+PG_V	# memory page
	.double	0xe5a000+PG_RW+PG_V	# memory page
	.double	0xe5b000+PG_RW+PG_V	# memory page
	.double	0xe5c000+PG_RW+PG_V	# memory page
	.double	0xe5d000+PG_RW+PG_V	# memory page
	.double	0xe5e000+PG_RW+PG_V	# memory page
	.double	0xe5f000+PG_RW+PG_V	# memory page
	.double	0xe60000+PG_RW+PG_V	# memory page
	.double	0xe61000+PG_RW+PG_V	# memory page
	.double	0xe62000+PG_RW+PG_V	# memory page
	.double	0xe63000+PG_RW+PG_V	# memory page
	.double	0xe64000+PG_RW+PG_V	# memory page
	.double	0xe65000+PG_RW+PG_V	# memory page
	.double	0xe66000+PG_RW+PG_V	# memory page
	.double	0xe67000+PG_RW+PG_V	# memory page
	.double	0xe68000+PG_RW+PG_V	# memory page
	.double	0xe69000+PG_RW+PG_V	# memory page
	.double	0xe6a000+PG_RW+PG_V	# memory page
	.double	0xe6b000+PG_RW+PG_V	# memory page
	.double	0xe6c000+PG_RW+PG_V	# memory page
	.double	0xe6d000+PG_RW+PG_V	# memory page
	.double	0xe6e000+PG_RW+PG_V	# memory page
	.double	0xe6f000+PG_RW+PG_V	# memory page
	.double	0xe70000+PG_RW+PG_V	# memory page
	.double	0xe71000+PG_RW+PG_V	# memory page
	.double	0xe72000+PG_RW+PG_V	# memory page
	.double	0xe73000+PG_RW+PG_V	# memory page
	.double	0xe74000+PG_RW+PG_V	# memory page
	.double	0xe75000+PG_RW+PG_V	# memory page
	.double	0xe76000+PG_RW+PG_V	# memory page
	.double	0xe77000+PG_RW+PG_V	# memory page
	.double	0xe78000+PG_RW+PG_V	# memory page
	.double	0xe79000+PG_RW+PG_V	# memory page
	.double	0xe7a000+PG_RW+PG_V	# memory page
	.double	0xe7b000+PG_RW+PG_V	# memory page
	.double	0xe7c000+PG_RW+PG_V	# memory page
	.double	0xe7d000+PG_RW+PG_V	# memory page
	.double	0xe7e000+PG_RW+PG_V	# memory page
	.double	0xe7f000+PG_RW+PG_V	# memory page
	.double	0xe80000+PG_RW+PG_V	# memory page
	.double	0xe81000+PG_RW+PG_V	# memory page
	.double	0xe82000+PG_RW+PG_V	# memory page
	.double	0xe83000+PG_RW+PG_V	# memory page
	.double	0xe84000+PG_RW+PG_V	# memory page
	.double	0xe85000+PG_RW+PG_V	# memory page
	.double	0xe86000+PG_RW+PG_V	# memory page
	.double	0xe87000+PG_RW+PG_V	# memory page
	.double	0xe88000+PG_RW+PG_V	# memory page
	.double	0xe89000+PG_RW+PG_V	# memory page
	.double	0xe8a000+PG_RW+PG_V	# memory page
	.double	0xe8b000+PG_RW+PG_V	# memory page
	.double	0xe8c000+PG_RW+PG_V	# memory page
	.double	0xe8d000+PG_RW+PG_V	# memory page
	.double	0xe8e000+PG_RW+PG_V	# memory page
	.double	0xe8f000+PG_RW+PG_V	# memory page
	.double	0xe90000+PG_RW+PG_V	# memory page
	.double	0xe91000+PG_RW+PG_V	# memory page
	.double	0xe92000+PG_RW+PG_V	# memory page
	.double	0xe93000+PG_RW+PG_V	# memory page
	.double	0xe94000+PG_RW+PG_V	# memory page
	.double	0xe95000+PG_RW+PG_V	# memory page
	.double	0xe96000+PG_RW+PG_V	# memory page
	.double	0xe97000+PG_RW+PG_V	# memory page
	.double	0xe98000+PG_RW+PG_V	# memory page
	.double	0xe99000+PG_RW+PG_V	# memory page
	.double	0xe9a000+PG_RW+PG_V	# memory page
	.double	0xe9b000+PG_RW+PG_V	# memory page
	.double	0xe9c000+PG_RW+PG_V	# memory page
	.double	0xe9d000+PG_RW+PG_V	# memory page
	.double	0xe9e000+PG_RW+PG_V	# memory page
	.double	0xe9f000+PG_RW+PG_V	# memory page
	.double	0xea0000+PG_RW+PG_V	# memory page
	.double	0xea1000+PG_RW+PG_V	# memory page
	.double	0xea2000+PG_RW+PG_V	# memory page
	.double	0xea3000+PG_RW+PG_V	# memory page
	.double	0xea4000+PG_RW+PG_V	# memory page
	.double	0xea5000+PG_RW+PG_V	# memory page
	.double	0xea6000+PG_RW+PG_V	# memory page
	.double	0xea7000+PG_RW+PG_V	# memory page
	.double	0xea8000+PG_RW+PG_V	# memory page
	.double	0xea9000+PG_RW+PG_V	# memory page
	.double	0xeaa000+PG_RW+PG_V	# memory page
	.double	0xeab000+PG_RW+PG_V	# memory page
	.double	0xeac000+PG_RW+PG_V	# memory page
	.double	0xead000+PG_RW+PG_V	# memory page
	.double	0xeae000+PG_RW+PG_V	# memory page
	.double	0xeaf000+PG_RW+PG_V	# memory page
	.double	0xeb0000+PG_RW+PG_V	# memory page
	.double	0xeb1000+PG_RW+PG_V	# memory page
	.double	0xeb2000+PG_RW+PG_V	# memory page
	.double	0xeb3000+PG_RW+PG_V	# memory page
	.double	0xeb4000+PG_RW+PG_V	# memory page
	.double	0xeb5000+PG_RW+PG_V	# memory page
	.double	0xeb6000+PG_RW+PG_V	# memory page
	.double	0xeb7000+PG_RW+PG_V	# memory page
	.double	0xeb8000+PG_RW+PG_V	# memory page
	.double	0xeb9000+PG_RW+PG_V	# memory page
	.double	0xeba000+PG_RW+PG_V	# memory page
	.double	0xebb000+PG_RW+PG_V	# memory page
	.double	0xebc000+PG_RW+PG_V	# memory page
	.double	0xebd000+PG_RW+PG_V	# memory page
	.double	0xebe000+PG_RW+PG_V	# memory page
	.double	0xebf000+PG_RW+PG_V	# memory page
	.double	0xec0000+PG_RW+PG_V	# memory page
	.double	0xec1000+PG_RW+PG_V	# memory page
	.double	0xec2000+PG_RW+PG_V	# memory page
	.double	0xec3000+PG_RW+PG_V	# memory page
	.double	0xec4000+PG_RW+PG_V	# memory page
	.double	0xec5000+PG_RW+PG_V	# memory page
	.double	0xec6000+PG_RW+PG_V	# memory page
	.double	0xec7000+PG_RW+PG_V	# memory page
	.double	0xec8000+PG_RW+PG_V	# memory page
	.double	0xec9000+PG_RW+PG_V	# memory page
	.double	0xeca000+PG_RW+PG_V	# memory page
	.double	0xecb000+PG_RW+PG_V	# memory page
	.double	0xecc000+PG_RW+PG_V	# memory page
	.double	0xecd000+PG_RW+PG_V	# memory page
	.double	0xece000+PG_RW+PG_V	# memory page
	.double	0xecf000+PG_RW+PG_V	# memory page
	.double	0xed0000+PG_RW+PG_V	# memory page
	.double	0xed1000+PG_RW+PG_V	# memory page
	.double	0xed2000+PG_RW+PG_V	# memory page
	.double	0xed3000+PG_RW+PG_V	# memory page
	.double	0xed4000+PG_RW+PG_V	# memory page
	.double	0xed5000+PG_RW+PG_V	# memory page
	.double	0xed6000+PG_RW+PG_V	# memory page
	.double	0xed7000+PG_RW+PG_V	# memory page
	.double	0xed8000+PG_RW+PG_V	# memory page
	.double	0xed9000+PG_RW+PG_V	# memory page
	.double	0xeda000+PG_RW+PG_V	# memory page
	.double	0xedb000+PG_RW+PG_V	# memory page
	.double	0xedc000+PG_RW+PG_V	# memory page
	.double	0xedd000+PG_RW+PG_V	# memory page
	.double	0xede000+PG_RW+PG_V	# memory page
	.double	0xedf000+PG_RW+PG_V	# memory page
	.double	0xee0000+PG_RW+PG_V	# memory page
	.double	0xee1000+PG_RW+PG_V	# memory page
	.double	0xee2000+PG_RW+PG_V	# memory page
	.double	0xee3000+PG_RW+PG_V	# memory page
	.double	0xee4000+PG_RW+PG_V	# memory page
	.double	0xee5000+PG_RW+PG_V	# memory page
	.double	0xee6000+PG_RW+PG_V	# memory page
	.double	0xee7000+PG_RW+PG_V	# memory page
	.double	0xee8000+PG_RW+PG_V	# memory page
	.double	0xee9000+PG_RW+PG_V	# memory page
	.double	0xeea000+PG_RW+PG_V	# memory page
	.double	0xeeb000+PG_RW+PG_V	# memory page
	.double	0xeec000+PG_RW+PG_V	# memory page
	.double	0xeed000+PG_RW+PG_V	# memory page
	.double	0xeee000+PG_RW+PG_V	# memory page
	.double	0xeef000+PG_RW+PG_V	# memory page
	.double	0xef0000+PG_RW+PG_V	# memory page
	.double	0xef1000+PG_RW+PG_V	# memory page
	.double	0xef2000+PG_RW+PG_V	# memory page
	.double	0xef3000+PG_RW+PG_V	# memory page
	.double	0xef4000+PG_RW+PG_V	# memory page
	.double	0xef5000+PG_RW+PG_V	# memory page
	.double	0xef6000+PG_RW+PG_V	# memory page
	.double	0xef7000+PG_RW+PG_V	# memory page
	.double	0xef8000+PG_RW+PG_V	# memory page
	.double	0xef9000+PG_RW+PG_V	# memory page
	.double	0xefa000+PG_RW+PG_V	# memory page
	.double	0xefb000+PG_RW+PG_V	# memory page
	.double	0xefc000+PG_RW+PG_V	# memory page
	.double	0xefd000+PG_RW+PG_V	# memory page
	.double	0xefe000+PG_RW+PG_V	# memory page
	.double	0xeff000+PG_RW+PG_V	# memory page
	.double	0xf00000+PG_RW+PG_V	# memory page
	.double	0xf01000+PG_RW+PG_V	# memory page
	.double	0xf02000+PG_RW+PG_V	# memory page
	.double	0xf03000+PG_RW+PG_V	# memory page
	.double	0xf04000+PG_RW+PG_V	# memory page
	.double	0xf05000+PG_RW+PG_V	# memory page
	.double	0xf06000+PG_RW+PG_V	# memory page
	.double	0xf07000+PG_RW+PG_V	# memory page
	.double	0xf08000+PG_RW+PG_V	# memory page
	.double	0xf09000+PG_RW+PG_V	# memory page
	.double	0xf0a000+PG_RW+PG_V	# memory page
	.double	0xf0b000+PG_RW+PG_V	# memory page
	.double	0xf0c000+PG_RW+PG_V	# memory page
	.double	0xf0d000+PG_RW+PG_V	# memory page
	.double	0xf0e000+PG_RW+PG_V	# memory page
	.double	0xf0f000+PG_RW+PG_V	# memory page
	.double	0xf10000+PG_RW+PG_V	# memory page
	.double	0xf11000+PG_RW+PG_V	# memory page
	.double	0xf12000+PG_RW+PG_V	# memory page
	.double	0xf13000+PG_RW+PG_V	# memory page
	.double	0xf14000+PG_RW+PG_V	# memory page
	.double	0xf15000+PG_RW+PG_V	# memory page
	.double	0xf16000+PG_RW+PG_V	# memory page
	.double	0xf17000+PG_RW+PG_V	# memory page
	.double	0xf18000+PG_RW+PG_V	# memory page
	.double	0xf19000+PG_RW+PG_V	# memory page
	.double	0xf1a000+PG_RW+PG_V	# memory page
	.double	0xf1b000+PG_RW+PG_V	# memory page
	.double	0xf1c000+PG_RW+PG_V	# memory page
	.double	0xf1d000+PG_RW+PG_V	# memory page
	.double	0xf1e000+PG_RW+PG_V	# memory page
	.double	0xf1f000+PG_RW+PG_V	# memory page
	.double	0xf20000+PG_RW+PG_V	# memory page
	.double	0xf21000+PG_RW+PG_V	# memory page
	.double	0xf22000+PG_RW+PG_V	# memory page
	.double	0xf23000+PG_RW+PG_V	# memory page
	.double	0xf24000+PG_RW+PG_V	# memory page
	.double	0xf25000+PG_RW+PG_V	# memory page
	.double	0xf26000+PG_RW+PG_V	# memory page
	.double	0xf27000+PG_RW+PG_V	# memory page
	.double	0xf28000+PG_RW+PG_V	# memory page
	.double	0xf29000+PG_RW+PG_V	# memory page
	.double	0xf2a000+PG_RW+PG_V	# memory page
	.double	0xf2b000+PG_RW+PG_V	# memory page
	.double	0xf2c000+PG_RW+PG_V	# memory page
	.double	0xf2d000+PG_RW+PG_V	# memory page
	.double	0xf2e000+PG_RW+PG_V	# memory page
	.double	0xf2f000+PG_RW+PG_V	# memory page
	.double	0xf30000+PG_RW+PG_V	# memory page
	.double	0xf31000+PG_RW+PG_V	# memory page
	.double	0xf32000+PG_RW+PG_V	# memory page
	.double	0xf33000+PG_RW+PG_V	# memory page
	.double	0xf34000+PG_RW+PG_V	# memory page
	.double	0xf35000+PG_RW+PG_V	# memory page
	.double	0xf36000+PG_RW+PG_V	# memory page
	.double	0xf37000+PG_RW+PG_V	# memory page
	.double	0xf38000+PG_RW+PG_V	# memory page
	.double	0xf39000+PG_RW+PG_V	# memory page
	.double	0xf3a000+PG_RW+PG_V	# memory page
	.double	0xf3b000+PG_RW+PG_V	# memory page
	.double	0xf3c000+PG_RW+PG_V	# memory page
	.double	0xf3d000+PG_RW+PG_V	# memory page
	.double	0xf3e000+PG_RW+PG_V	# memory page
	.double	0xf3f000+PG_RW+PG_V	# memory page
	.double	0xf40000+PG_RW+PG_V	# memory page
	.double	0xf41000+PG_RW+PG_V	# memory page
	.double	0xf42000+PG_RW+PG_V	# memory page
	.double	0xf43000+PG_RW+PG_V	# memory page
	.double	0xf44000+PG_RW+PG_V	# memory page
	.double	0xf45000+PG_RW+PG_V	# memory page
	.double	0xf46000+PG_RW+PG_V	# memory page
	.double	0xf47000+PG_RW+PG_V	# memory page
	.double	0xf48000+PG_RW+PG_V	# memory page
	.double	0xf49000+PG_RW+PG_V	# memory page
	.double	0xf4a000+PG_RW+PG_V	# memory page
	.double	0xf4b000+PG_RW+PG_V	# memory page
	.double	0xf4c000+PG_RW+PG_V	# memory page
	.double	0xf4d000+PG_RW+PG_V	# memory page
	.double	0xf4e000+PG_RW+PG_V	# memory page
	.double	0xf4f000+PG_RW+PG_V	# memory page
	.double	0xf50000+PG_RW+PG_V	# memory page
	.double	0xf51000+PG_RW+PG_V	# memory page
	.double	0xf52000+PG_RW+PG_V	# memory page
	.double	0xf53000+PG_RW+PG_V	# memory page
	.double	0xf54000+PG_RW+PG_V	# memory page
	.double	0xf55000+PG_RW+PG_V	# memory page
	.double	0xf56000+PG_RW+PG_V	# memory page
	.double	0xf57000+PG_RW+PG_V	# memory page
	.double	0xf58000+PG_RW+PG_V	# memory page
	.double	0xf59000+PG_RW+PG_V	# memory page
	.double	0xf5a000+PG_RW+PG_V	# memory page
	.double	0xf5b000+PG_RW+PG_V	# memory page
	.double	0xf5c000+PG_RW+PG_V	# memory page
	.double	0xf5d000+PG_RW+PG_V	# memory page
	.double	0xf5e000+PG_RW+PG_V	# memory page
	.double	0xf5f000+PG_RW+PG_V	# memory page
	.double	0xf60000+PG_RW+PG_V	# memory page
	.double	0xf61000+PG_RW+PG_V	# memory page
	.double	0xf62000+PG_RW+PG_V	# memory page
	.double	0xf63000+PG_RW+PG_V	# memory page
	.double	0xf64000+PG_RW+PG_V	# memory page
	.double	0xf65000+PG_RW+PG_V	# memory page
	.double	0xf66000+PG_RW+PG_V	# memory page
	.double	0xf67000+PG_RW+PG_V	# memory page
	.double	0xf68000+PG_RW+PG_V	# memory page
	.double	0xf69000+PG_RW+PG_V	# memory page
	.double	0xf6a000+PG_RW+PG_V	# memory page
	.double	0xf6b000+PG_RW+PG_V	# memory page
	.double	0xf6c000+PG_RW+PG_V	# memory page
	.double	0xf6d000+PG_RW+PG_V	# memory page
	.double	0xf6e000+PG_RW+PG_V	# memory page
	.double	0xf6f000+PG_RW+PG_V	# memory page
	.double	0xf70000+PG_RW+PG_V	# memory page
	.double	0xf71000+PG_RW+PG_V	# memory page
	.double	0xf72000+PG_RW+PG_V	# memory page
	.double	0xf73000+PG_RW+PG_V	# memory page
	.double	0xf74000+PG_RW+PG_V	# memory page
	.double	0xf75000+PG_RW+PG_V	# memory page
	.double	0xf76000+PG_RW+PG_V	# memory page
	.double	0xf77000+PG_RW+PG_V	# memory page
	.double	0xf78000+PG_RW+PG_V	# memory page
	.double	0xf79000+PG_RW+PG_V	# memory page
	.double	0xf7a000+PG_RW+PG_V	# memory page
	.double	0xf7b000+PG_RW+PG_V	# memory page
	.double	0xf7c000+PG_RW+PG_V	# memory page
	.double	0xf7d000+PG_RW+PG_V	# memory page
	.double	0xf7e000+PG_RW+PG_V	# memory page
	.double	0xf7f000+PG_RW+PG_V	# memory page
	.double	0xf80000+PG_RW+PG_V	# memory page
	.double	0xf81000+PG_RW+PG_V	# memory page
	.double	0xf82000+PG_RW+PG_V	# memory page
	.double	0xf83000+PG_RW+PG_V	# memory page
	.double	0xf84000+PG_RW+PG_V	# memory page
	.double	0xf85000+PG_RW+PG_V	# memory page
	.double	0xf86000+PG_RW+PG_V	# memory page
	.double	0xf87000+PG_RW+PG_V	# memory page
	.double	0xf88000+PG_RW+PG_V	# memory page
	.double	0xf89000+PG_RW+PG_V	# memory page
	.double	0xf8a000+PG_RW+PG_V	# memory page
	.double	0xf8b000+PG_RW+PG_V	# memory page
	.double	0xf8c000+PG_RW+PG_V	# memory page
	.double	0xf8d000+PG_RW+PG_V	# memory page
	.double	0xf8e000+PG_RW+PG_V	# memory page
	.double	0xf8f000+PG_RW+PG_V	# memory page
	.double	0xf90000+PG_RW+PG_V	# memory page
	.double	0xf91000+PG_RW+PG_V	# memory page
	.double	0xf92000+PG_RW+PG_V	# memory page
	.double	0xf93000+PG_RW+PG_V	# memory page
	.double	0xf94000+PG_RW+PG_V	# memory page
	.double	0xf95000+PG_RW+PG_V	# memory page
	.double	0xf96000+PG_RW+PG_V	# memory page
	.double	0xf97000+PG_RW+PG_V	# memory page
	.double	0xf98000+PG_RW+PG_V	# memory page
	.double	0xf99000+PG_RW+PG_V	# memory page
	.double	0xf9a000+PG_RW+PG_V	# memory page
	.double	0xf9b000+PG_RW+PG_V	# memory page
	.double	0xf9c000+PG_RW+PG_V	# memory page
	.double	0xf9d000+PG_RW+PG_V	# memory page
	.double	0xf9e000+PG_RW+PG_V	# memory page
	.double	0xf9f000+PG_RW+PG_V	# memory page
	.double	0xfa0000+PG_RW+PG_V	# memory page
	.double	0xfa1000+PG_RW+PG_V	# memory page
	.double	0xfa2000+PG_RW+PG_V	# memory page
	.double	0xfa3000+PG_RW+PG_V	# memory page
	.double	0xfa4000+PG_RW+PG_V	# memory page
	.double	0xfa5000+PG_RW+PG_V	# memory page
	.double	0xfa6000+PG_RW+PG_V	# memory page
	.double	0xfa7000+PG_RW+PG_V	# memory page
	.double	0xfa8000+PG_RW+PG_V	# memory page
	.double	0xfa9000+PG_RW+PG_V	# memory page
	.double	0xfaa000+PG_RW+PG_V	# memory page
	.double	0xfab000+PG_RW+PG_V	# memory page
	.double	0xfac000+PG_RW+PG_V	# memory page
	.double	0xfad000+PG_RW+PG_V	# memory page
	.double	0xfae000+PG_RW+PG_V	# memory page
	.double	0xfaf000+PG_RW+PG_V	# memory page
	.double	0xfb0000+PG_RW+PG_V	# memory page
	.double	0xfb1000+PG_RW+PG_V	# memory page
	.double	0xfb2000+PG_RW+PG_V	# memory page
	.double	0xfb3000+PG_RW+PG_V	# memory page
	.double	0xfb4000+PG_RW+PG_V	# memory page
	.double	0xfb5000+PG_RW+PG_V	# memory page
	.double	0xfb6000+PG_RW+PG_V	# memory page
	.double	0xfb7000+PG_RW+PG_V	# memory page
	.double	0xfb8000+PG_RW+PG_V	# memory page
	.double	0xfb9000+PG_RW+PG_V	# memory page
	.double	0xfba000+PG_RW+PG_V	# memory page
	.double	0xfbb000+PG_RW+PG_V	# memory page
	.double	0xfbc000+PG_RW+PG_V	# memory page
	.double	0xfbd000+PG_RW+PG_V	# memory page
	.double	0xfbe000+PG_RW+PG_V	# memory page
	.double	0xfbf000+PG_RW+PG_V	# memory page
	.double	0xfc0000+PG_RW+PG_V	# memory page
	.double	0xfc1000+PG_RW+PG_V	# memory page
	.double	0xfc2000+PG_RW+PG_V	# memory page
	.double	0xfc3000+PG_RW+PG_V	# memory page
	.double	0xfc4000+PG_RW+PG_V	# memory page
	.double	0xfc5000+PG_RW+PG_V	# memory page
	.double	0xfc6000+PG_RW+PG_V	# memory page
	.double	0xfc7000+PG_RW+PG_V	# memory page
	.double	0xfc8000+PG_RW+PG_V	# memory page
	.double	0xfc9000+PG_RW+PG_V	# memory page
	.double	0xfca000+PG_RW+PG_V	# memory page
	.double	0xfcb000+PG_RW+PG_V	# memory page
	.double	0xfcc000+PG_RW+PG_V	# memory page
	.double	0xfcd000+PG_RW+PG_V	# memory page
	.double	0xfce000+PG_RW+PG_V	# memory page
	.double	0xfcf000+PG_RW+PG_V	# memory page
	.double	0xfd0000+PG_RW+PG_V	# memory page
	.double	0xfd1000+PG_RW+PG_V	# memory page
	.double	0xfd2000+PG_RW+PG_V	# memory page
	.double	0xfd3000+PG_RW+PG_V	# memory page
	.double	0xfd4000+PG_RW+PG_V	# memory page
	.double	0xfd5000+PG_RW+PG_V	# memory page
	.double	0xfd6000+PG_RW+PG_V	# memory page
	.double	0xfd7000+PG_RW+PG_V	# memory page
	.double	0xfd8000+PG_RW+PG_V	# memory page
	.double	0xfd9000+PG_RW+PG_V	# memory page
	.double	0xfda000+PG_RW+PG_V	# memory page
	.double	0xfdb000+PG_RW+PG_V	# memory page
	.double	0xfdc000+PG_RW+PG_V	# memory page
	.double	0xfdd000+PG_RW+PG_V	# memory page
	.double	0xfde000+PG_RW+PG_V	# memory page
	.double	0xfdf000+PG_RW+PG_V	# memory page
	.double	0xfe0000+PG_RW+PG_V	# memory page
	.double	0xfe1000+PG_RW+PG_V	# memory page
	.double	0xfe2000+PG_RW+PG_V	# memory page
	.double	0xfe3000+PG_RW+PG_V	# memory page
	.double	0xfe4000+PG_RW+PG_V	# memory page
	.double	0xfe5000+PG_RW+PG_V	# memory page
	.double	0xfe6000+PG_RW+PG_V	# memory page
	.double	0xfe7000+PG_RW+PG_V	# memory page
	.double	0xfe8000+PG_RW+PG_V	# memory page
	.double	0xfe9000+PG_RW+PG_V	# memory page
	.double	0xfea000+PG_RW+PG_V	# memory page
	.double	0xfeb000+PG_RW+PG_V	# memory page
	.double	0xfec000+PG_RW+PG_V	# memory page
	.double	0xfed000+PG_RW+PG_V	# memory page
	.double	0xfee000+PG_RW+PG_V	# memory page
	.double	0xfef000+PG_RW+PG_V	# memory page
	.double	0xff0000+PG_RW+PG_V	# memory page
	.double	0xff1000+PG_RW+PG_V	# memory page
	.double	0xff2000+PG_RW+PG_V	# memory page
	.double	0xff3000+PG_RW+PG_V	# memory page
	.double	0xff4000+PG_RW+PG_V	# memory page
	.double	0xff5000+PG_RW+PG_V	# memory page
	.double	0xff6000+PG_RW+PG_V	# memory page
	.double	0xff7000+PG_RW+PG_V	# memory page
	.double	0xff8000+PG_RW+PG_V	# memory page
	.double	0xff9000+PG_RW+PG_V	# memory page
	.double	0xffa000+PG_RW+PG_V	# memory page
	.double	0xffb000+PG_RW+PG_V	# memory page
	.double	0xffc000+PG_RW+PG_V	# memory page
	.double	0xffd000+PG_RW+PG_V	# memory page
	.double	0xffe000+PG_RW+PG_V	# memory page
	.double	0xfff000+PG_RW+PG_V	# memory page

# PAGE 8
# BUS-MASTER MEMORY 2nd level page table entries
pagebm0:
	.double	0x0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x100000+AT_MEM_SPACE+MAPINITWORD
	.double	0x101000+AT_MEM_SPACE+MAPINITWORD
	.double	0x102000+AT_MEM_SPACE+MAPINITWORD
	.double	0x103000+AT_MEM_SPACE+MAPINITWORD
	.double	0x104000+AT_MEM_SPACE+MAPINITWORD
	.double	0x105000+AT_MEM_SPACE+MAPINITWORD
	.double	0x106000+AT_MEM_SPACE+MAPINITWORD
	.double	0x107000+AT_MEM_SPACE+MAPINITWORD
	.double	0x108000+AT_MEM_SPACE+MAPINITWORD
	.double	0x109000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x10f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x110000+AT_MEM_SPACE+MAPINITWORD
	.double	0x111000+AT_MEM_SPACE+MAPINITWORD
	.double	0x112000+AT_MEM_SPACE+MAPINITWORD
	.double	0x113000+AT_MEM_SPACE+MAPINITWORD
	.double	0x114000+AT_MEM_SPACE+MAPINITWORD
	.double	0x115000+AT_MEM_SPACE+MAPINITWORD
	.double	0x116000+AT_MEM_SPACE+MAPINITWORD
	.double	0x117000+AT_MEM_SPACE+MAPINITWORD
	.double	0x118000+AT_MEM_SPACE+MAPINITWORD
	.double	0x119000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x11f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x120000+AT_MEM_SPACE+MAPINITWORD
	.double	0x121000+AT_MEM_SPACE+MAPINITWORD
	.double	0x122000+AT_MEM_SPACE+MAPINITWORD
	.double	0x123000+AT_MEM_SPACE+MAPINITWORD
	.double	0x124000+AT_MEM_SPACE+MAPINITWORD
	.double	0x125000+AT_MEM_SPACE+MAPINITWORD
	.double	0x126000+AT_MEM_SPACE+MAPINITWORD
	.double	0x127000+AT_MEM_SPACE+MAPINITWORD
	.double	0x128000+AT_MEM_SPACE+MAPINITWORD
	.double	0x129000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x12f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x130000+AT_MEM_SPACE+MAPINITWORD
	.double	0x131000+AT_MEM_SPACE+MAPINITWORD
	.double	0x132000+AT_MEM_SPACE+MAPINITWORD
	.double	0x133000+AT_MEM_SPACE+MAPINITWORD
	.double	0x134000+AT_MEM_SPACE+MAPINITWORD
	.double	0x135000+AT_MEM_SPACE+MAPINITWORD
	.double	0x136000+AT_MEM_SPACE+MAPINITWORD
	.double	0x137000+AT_MEM_SPACE+MAPINITWORD
	.double	0x138000+AT_MEM_SPACE+MAPINITWORD
	.double	0x139000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x13f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x140000+AT_MEM_SPACE+MAPINITWORD
	.double	0x141000+AT_MEM_SPACE+MAPINITWORD
	.double	0x142000+AT_MEM_SPACE+MAPINITWORD
	.double	0x143000+AT_MEM_SPACE+MAPINITWORD
	.double	0x144000+AT_MEM_SPACE+MAPINITWORD
	.double	0x145000+AT_MEM_SPACE+MAPINITWORD
	.double	0x146000+AT_MEM_SPACE+MAPINITWORD
	.double	0x147000+AT_MEM_SPACE+MAPINITWORD
	.double	0x148000+AT_MEM_SPACE+MAPINITWORD
	.double	0x149000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x14f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x150000+AT_MEM_SPACE+MAPINITWORD
	.double	0x151000+AT_MEM_SPACE+MAPINITWORD
	.double	0x152000+AT_MEM_SPACE+MAPINITWORD
	.double	0x153000+AT_MEM_SPACE+MAPINITWORD
	.double	0x154000+AT_MEM_SPACE+MAPINITWORD
	.double	0x155000+AT_MEM_SPACE+MAPINITWORD
	.double	0x156000+AT_MEM_SPACE+MAPINITWORD
	.double	0x157000+AT_MEM_SPACE+MAPINITWORD
	.double	0x158000+AT_MEM_SPACE+MAPINITWORD
	.double	0x159000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x15f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x160000+AT_MEM_SPACE+MAPINITWORD
	.double	0x161000+AT_MEM_SPACE+MAPINITWORD
	.double	0x162000+AT_MEM_SPACE+MAPINITWORD
	.double	0x163000+AT_MEM_SPACE+MAPINITWORD
	.double	0x164000+AT_MEM_SPACE+MAPINITWORD
	.double	0x165000+AT_MEM_SPACE+MAPINITWORD
	.double	0x166000+AT_MEM_SPACE+MAPINITWORD
	.double	0x167000+AT_MEM_SPACE+MAPINITWORD
	.double	0x168000+AT_MEM_SPACE+MAPINITWORD
	.double	0x169000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x16f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x170000+AT_MEM_SPACE+MAPINITWORD
	.double	0x171000+AT_MEM_SPACE+MAPINITWORD
	.double	0x172000+AT_MEM_SPACE+MAPINITWORD
	.double	0x173000+AT_MEM_SPACE+MAPINITWORD
	.double	0x174000+AT_MEM_SPACE+MAPINITWORD
	.double	0x175000+AT_MEM_SPACE+MAPINITWORD
	.double	0x176000+AT_MEM_SPACE+MAPINITWORD
	.double	0x177000+AT_MEM_SPACE+MAPINITWORD
	.double	0x178000+AT_MEM_SPACE+MAPINITWORD
	.double	0x179000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x17f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x180000+AT_MEM_SPACE+MAPINITWORD
	.double	0x181000+AT_MEM_SPACE+MAPINITWORD
	.double	0x182000+AT_MEM_SPACE+MAPINITWORD
	.double	0x183000+AT_MEM_SPACE+MAPINITWORD
	.double	0x184000+AT_MEM_SPACE+MAPINITWORD
	.double	0x185000+AT_MEM_SPACE+MAPINITWORD
	.double	0x186000+AT_MEM_SPACE+MAPINITWORD
	.double	0x187000+AT_MEM_SPACE+MAPINITWORD
	.double	0x188000+AT_MEM_SPACE+MAPINITWORD
	.double	0x189000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x18f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x190000+AT_MEM_SPACE+MAPINITWORD
	.double	0x191000+AT_MEM_SPACE+MAPINITWORD
	.double	0x192000+AT_MEM_SPACE+MAPINITWORD
	.double	0x193000+AT_MEM_SPACE+MAPINITWORD
	.double	0x194000+AT_MEM_SPACE+MAPINITWORD
	.double	0x195000+AT_MEM_SPACE+MAPINITWORD
	.double	0x196000+AT_MEM_SPACE+MAPINITWORD
	.double	0x197000+AT_MEM_SPACE+MAPINITWORD
	.double	0x198000+AT_MEM_SPACE+MAPINITWORD
	.double	0x199000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x19f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x1ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x200000+AT_MEM_SPACE+MAPINITWORD
	.double	0x201000+AT_MEM_SPACE+MAPINITWORD
	.double	0x202000+AT_MEM_SPACE+MAPINITWORD
	.double	0x203000+AT_MEM_SPACE+MAPINITWORD
	.double	0x204000+AT_MEM_SPACE+MAPINITWORD
	.double	0x205000+AT_MEM_SPACE+MAPINITWORD
	.double	0x206000+AT_MEM_SPACE+MAPINITWORD
	.double	0x207000+AT_MEM_SPACE+MAPINITWORD
	.double	0x208000+AT_MEM_SPACE+MAPINITWORD
	.double	0x209000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x20f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x210000+AT_MEM_SPACE+MAPINITWORD
	.double	0x211000+AT_MEM_SPACE+MAPINITWORD
	.double	0x212000+AT_MEM_SPACE+MAPINITWORD
	.double	0x213000+AT_MEM_SPACE+MAPINITWORD
	.double	0x214000+AT_MEM_SPACE+MAPINITWORD
	.double	0x215000+AT_MEM_SPACE+MAPINITWORD
	.double	0x216000+AT_MEM_SPACE+MAPINITWORD
	.double	0x217000+AT_MEM_SPACE+MAPINITWORD
	.double	0x218000+AT_MEM_SPACE+MAPINITWORD
	.double	0x219000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x21f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x220000+AT_MEM_SPACE+MAPINITWORD
	.double	0x221000+AT_MEM_SPACE+MAPINITWORD
	.double	0x222000+AT_MEM_SPACE+MAPINITWORD
	.double	0x223000+AT_MEM_SPACE+MAPINITWORD
	.double	0x224000+AT_MEM_SPACE+MAPINITWORD
	.double	0x225000+AT_MEM_SPACE+MAPINITWORD
	.double	0x226000+AT_MEM_SPACE+MAPINITWORD
	.double	0x227000+AT_MEM_SPACE+MAPINITWORD
	.double	0x228000+AT_MEM_SPACE+MAPINITWORD
	.double	0x229000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x22f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x230000+AT_MEM_SPACE+MAPINITWORD
	.double	0x231000+AT_MEM_SPACE+MAPINITWORD
	.double	0x232000+AT_MEM_SPACE+MAPINITWORD
	.double	0x233000+AT_MEM_SPACE+MAPINITWORD
	.double	0x234000+AT_MEM_SPACE+MAPINITWORD
	.double	0x235000+AT_MEM_SPACE+MAPINITWORD
	.double	0x236000+AT_MEM_SPACE+MAPINITWORD
	.double	0x237000+AT_MEM_SPACE+MAPINITWORD
	.double	0x238000+AT_MEM_SPACE+MAPINITWORD
	.double	0x239000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x23f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x240000+AT_MEM_SPACE+MAPINITWORD
	.double	0x241000+AT_MEM_SPACE+MAPINITWORD
	.double	0x242000+AT_MEM_SPACE+MAPINITWORD
	.double	0x243000+AT_MEM_SPACE+MAPINITWORD
	.double	0x244000+AT_MEM_SPACE+MAPINITWORD
	.double	0x245000+AT_MEM_SPACE+MAPINITWORD
	.double	0x246000+AT_MEM_SPACE+MAPINITWORD
	.double	0x247000+AT_MEM_SPACE+MAPINITWORD
	.double	0x248000+AT_MEM_SPACE+MAPINITWORD
	.double	0x249000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x24f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x250000+AT_MEM_SPACE+MAPINITWORD
	.double	0x251000+AT_MEM_SPACE+MAPINITWORD
	.double	0x252000+AT_MEM_SPACE+MAPINITWORD
	.double	0x253000+AT_MEM_SPACE+MAPINITWORD
	.double	0x254000+AT_MEM_SPACE+MAPINITWORD
	.double	0x255000+AT_MEM_SPACE+MAPINITWORD
	.double	0x256000+AT_MEM_SPACE+MAPINITWORD
	.double	0x257000+AT_MEM_SPACE+MAPINITWORD
	.double	0x258000+AT_MEM_SPACE+MAPINITWORD
	.double	0x259000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x25f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x260000+AT_MEM_SPACE+MAPINITWORD
	.double	0x261000+AT_MEM_SPACE+MAPINITWORD
	.double	0x262000+AT_MEM_SPACE+MAPINITWORD
	.double	0x263000+AT_MEM_SPACE+MAPINITWORD
	.double	0x264000+AT_MEM_SPACE+MAPINITWORD
	.double	0x265000+AT_MEM_SPACE+MAPINITWORD
	.double	0x266000+AT_MEM_SPACE+MAPINITWORD
	.double	0x267000+AT_MEM_SPACE+MAPINITWORD
	.double	0x268000+AT_MEM_SPACE+MAPINITWORD
	.double	0x269000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x26f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x270000+AT_MEM_SPACE+MAPINITWORD
	.double	0x271000+AT_MEM_SPACE+MAPINITWORD
	.double	0x272000+AT_MEM_SPACE+MAPINITWORD
	.double	0x273000+AT_MEM_SPACE+MAPINITWORD
	.double	0x274000+AT_MEM_SPACE+MAPINITWORD
	.double	0x275000+AT_MEM_SPACE+MAPINITWORD
	.double	0x276000+AT_MEM_SPACE+MAPINITWORD
	.double	0x277000+AT_MEM_SPACE+MAPINITWORD
	.double	0x278000+AT_MEM_SPACE+MAPINITWORD
	.double	0x279000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x27f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x280000+AT_MEM_SPACE+MAPINITWORD
	.double	0x281000+AT_MEM_SPACE+MAPINITWORD
	.double	0x282000+AT_MEM_SPACE+MAPINITWORD
	.double	0x283000+AT_MEM_SPACE+MAPINITWORD
	.double	0x284000+AT_MEM_SPACE+MAPINITWORD
	.double	0x285000+AT_MEM_SPACE+MAPINITWORD
	.double	0x286000+AT_MEM_SPACE+MAPINITWORD
	.double	0x287000+AT_MEM_SPACE+MAPINITWORD
	.double	0x288000+AT_MEM_SPACE+MAPINITWORD
	.double	0x289000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x28f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x290000+AT_MEM_SPACE+MAPINITWORD
	.double	0x291000+AT_MEM_SPACE+MAPINITWORD
	.double	0x292000+AT_MEM_SPACE+MAPINITWORD
	.double	0x293000+AT_MEM_SPACE+MAPINITWORD
	.double	0x294000+AT_MEM_SPACE+MAPINITWORD
	.double	0x295000+AT_MEM_SPACE+MAPINITWORD
	.double	0x296000+AT_MEM_SPACE+MAPINITWORD
	.double	0x297000+AT_MEM_SPACE+MAPINITWORD
	.double	0x298000+AT_MEM_SPACE+MAPINITWORD
	.double	0x299000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x29f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x2ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x300000+AT_MEM_SPACE+MAPINITWORD
	.double	0x301000+AT_MEM_SPACE+MAPINITWORD
	.double	0x302000+AT_MEM_SPACE+MAPINITWORD
	.double	0x303000+AT_MEM_SPACE+MAPINITWORD
	.double	0x304000+AT_MEM_SPACE+MAPINITWORD
	.double	0x305000+AT_MEM_SPACE+MAPINITWORD
	.double	0x306000+AT_MEM_SPACE+MAPINITWORD
	.double	0x307000+AT_MEM_SPACE+MAPINITWORD
	.double	0x308000+AT_MEM_SPACE+MAPINITWORD
	.double	0x309000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x30f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x310000+AT_MEM_SPACE+MAPINITWORD
	.double	0x311000+AT_MEM_SPACE+MAPINITWORD
	.double	0x312000+AT_MEM_SPACE+MAPINITWORD
	.double	0x313000+AT_MEM_SPACE+MAPINITWORD
	.double	0x314000+AT_MEM_SPACE+MAPINITWORD
	.double	0x315000+AT_MEM_SPACE+MAPINITWORD
	.double	0x316000+AT_MEM_SPACE+MAPINITWORD
	.double	0x317000+AT_MEM_SPACE+MAPINITWORD
	.double	0x318000+AT_MEM_SPACE+MAPINITWORD
	.double	0x319000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x31f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x320000+AT_MEM_SPACE+MAPINITWORD
	.double	0x321000+AT_MEM_SPACE+MAPINITWORD
	.double	0x322000+AT_MEM_SPACE+MAPINITWORD
	.double	0x323000+AT_MEM_SPACE+MAPINITWORD
	.double	0x324000+AT_MEM_SPACE+MAPINITWORD
	.double	0x325000+AT_MEM_SPACE+MAPINITWORD
	.double	0x326000+AT_MEM_SPACE+MAPINITWORD
	.double	0x327000+AT_MEM_SPACE+MAPINITWORD
	.double	0x328000+AT_MEM_SPACE+MAPINITWORD
	.double	0x329000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x32f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x330000+AT_MEM_SPACE+MAPINITWORD
	.double	0x331000+AT_MEM_SPACE+MAPINITWORD
	.double	0x332000+AT_MEM_SPACE+MAPINITWORD
	.double	0x333000+AT_MEM_SPACE+MAPINITWORD
	.double	0x334000+AT_MEM_SPACE+MAPINITWORD
	.double	0x335000+AT_MEM_SPACE+MAPINITWORD
	.double	0x336000+AT_MEM_SPACE+MAPINITWORD
	.double	0x337000+AT_MEM_SPACE+MAPINITWORD
	.double	0x338000+AT_MEM_SPACE+MAPINITWORD
	.double	0x339000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x33f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x340000+AT_MEM_SPACE+MAPINITWORD
	.double	0x341000+AT_MEM_SPACE+MAPINITWORD
	.double	0x342000+AT_MEM_SPACE+MAPINITWORD
	.double	0x343000+AT_MEM_SPACE+MAPINITWORD
	.double	0x344000+AT_MEM_SPACE+MAPINITWORD
	.double	0x345000+AT_MEM_SPACE+MAPINITWORD
	.double	0x346000+AT_MEM_SPACE+MAPINITWORD
	.double	0x347000+AT_MEM_SPACE+MAPINITWORD
	.double	0x348000+AT_MEM_SPACE+MAPINITWORD
	.double	0x349000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x34f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x350000+AT_MEM_SPACE+MAPINITWORD
	.double	0x351000+AT_MEM_SPACE+MAPINITWORD
	.double	0x352000+AT_MEM_SPACE+MAPINITWORD
	.double	0x353000+AT_MEM_SPACE+MAPINITWORD
	.double	0x354000+AT_MEM_SPACE+MAPINITWORD
	.double	0x355000+AT_MEM_SPACE+MAPINITWORD
	.double	0x356000+AT_MEM_SPACE+MAPINITWORD
	.double	0x357000+AT_MEM_SPACE+MAPINITWORD
	.double	0x358000+AT_MEM_SPACE+MAPINITWORD
	.double	0x359000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x35f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x360000+AT_MEM_SPACE+MAPINITWORD
	.double	0x361000+AT_MEM_SPACE+MAPINITWORD
	.double	0x362000+AT_MEM_SPACE+MAPINITWORD
	.double	0x363000+AT_MEM_SPACE+MAPINITWORD
	.double	0x364000+AT_MEM_SPACE+MAPINITWORD
	.double	0x365000+AT_MEM_SPACE+MAPINITWORD
	.double	0x366000+AT_MEM_SPACE+MAPINITWORD
	.double	0x367000+AT_MEM_SPACE+MAPINITWORD
	.double	0x368000+AT_MEM_SPACE+MAPINITWORD
	.double	0x369000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x36f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x370000+AT_MEM_SPACE+MAPINITWORD
	.double	0x371000+AT_MEM_SPACE+MAPINITWORD
	.double	0x372000+AT_MEM_SPACE+MAPINITWORD
	.double	0x373000+AT_MEM_SPACE+MAPINITWORD
	.double	0x374000+AT_MEM_SPACE+MAPINITWORD
	.double	0x375000+AT_MEM_SPACE+MAPINITWORD
	.double	0x376000+AT_MEM_SPACE+MAPINITWORD
	.double	0x377000+AT_MEM_SPACE+MAPINITWORD
	.double	0x378000+AT_MEM_SPACE+MAPINITWORD
	.double	0x379000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x37f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x380000+AT_MEM_SPACE+MAPINITWORD
	.double	0x381000+AT_MEM_SPACE+MAPINITWORD
	.double	0x382000+AT_MEM_SPACE+MAPINITWORD
	.double	0x383000+AT_MEM_SPACE+MAPINITWORD
	.double	0x384000+AT_MEM_SPACE+MAPINITWORD
	.double	0x385000+AT_MEM_SPACE+MAPINITWORD
	.double	0x386000+AT_MEM_SPACE+MAPINITWORD
	.double	0x387000+AT_MEM_SPACE+MAPINITWORD
	.double	0x388000+AT_MEM_SPACE+MAPINITWORD
	.double	0x389000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x38f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x390000+AT_MEM_SPACE+MAPINITWORD
	.double	0x391000+AT_MEM_SPACE+MAPINITWORD
	.double	0x392000+AT_MEM_SPACE+MAPINITWORD
	.double	0x393000+AT_MEM_SPACE+MAPINITWORD
	.double	0x394000+AT_MEM_SPACE+MAPINITWORD
	.double	0x395000+AT_MEM_SPACE+MAPINITWORD
	.double	0x396000+AT_MEM_SPACE+MAPINITWORD
	.double	0x397000+AT_MEM_SPACE+MAPINITWORD
	.double	0x398000+AT_MEM_SPACE+MAPINITWORD
	.double	0x399000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x39f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x3ff000+AT_MEM_SPACE+MAPINITWORD

# PAGE 9
# BUS-MASTER 2nd level page table entries
pagebm1:
	.double	0x400000+AT_MEM_SPACE+MAPINITWORD
	.double	0x401000+AT_MEM_SPACE+MAPINITWORD
	.double	0x402000+AT_MEM_SPACE+MAPINITWORD
	.double	0x403000+AT_MEM_SPACE+MAPINITWORD
	.double	0x404000+AT_MEM_SPACE+MAPINITWORD
	.double	0x405000+AT_MEM_SPACE+MAPINITWORD
	.double	0x406000+AT_MEM_SPACE+MAPINITWORD
	.double	0x407000+AT_MEM_SPACE+MAPINITWORD
	.double	0x408000+AT_MEM_SPACE+MAPINITWORD
	.double	0x409000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x40f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x410000+AT_MEM_SPACE+MAPINITWORD
	.double	0x411000+AT_MEM_SPACE+MAPINITWORD
	.double	0x412000+AT_MEM_SPACE+MAPINITWORD
	.double	0x413000+AT_MEM_SPACE+MAPINITWORD
	.double	0x414000+AT_MEM_SPACE+MAPINITWORD
	.double	0x415000+AT_MEM_SPACE+MAPINITWORD
	.double	0x416000+AT_MEM_SPACE+MAPINITWORD
	.double	0x417000+AT_MEM_SPACE+MAPINITWORD
	.double	0x418000+AT_MEM_SPACE+MAPINITWORD
	.double	0x419000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x41f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x420000+AT_MEM_SPACE+MAPINITWORD
	.double	0x421000+AT_MEM_SPACE+MAPINITWORD
	.double	0x422000+AT_MEM_SPACE+MAPINITWORD
	.double	0x423000+AT_MEM_SPACE+MAPINITWORD
	.double	0x424000+AT_MEM_SPACE+MAPINITWORD
	.double	0x425000+AT_MEM_SPACE+MAPINITWORD
	.double	0x426000+AT_MEM_SPACE+MAPINITWORD
	.double	0x427000+AT_MEM_SPACE+MAPINITWORD
	.double	0x428000+AT_MEM_SPACE+MAPINITWORD
	.double	0x429000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x42f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x430000+AT_MEM_SPACE+MAPINITWORD
	.double	0x431000+AT_MEM_SPACE+MAPINITWORD
	.double	0x432000+AT_MEM_SPACE+MAPINITWORD
	.double	0x433000+AT_MEM_SPACE+MAPINITWORD
	.double	0x434000+AT_MEM_SPACE+MAPINITWORD
	.double	0x435000+AT_MEM_SPACE+MAPINITWORD
	.double	0x436000+AT_MEM_SPACE+MAPINITWORD
	.double	0x437000+AT_MEM_SPACE+MAPINITWORD
	.double	0x438000+AT_MEM_SPACE+MAPINITWORD
	.double	0x439000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x43f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x440000+AT_MEM_SPACE+MAPINITWORD
	.double	0x441000+AT_MEM_SPACE+MAPINITWORD
	.double	0x442000+AT_MEM_SPACE+MAPINITWORD
	.double	0x443000+AT_MEM_SPACE+MAPINITWORD
	.double	0x444000+AT_MEM_SPACE+MAPINITWORD
	.double	0x445000+AT_MEM_SPACE+MAPINITWORD
	.double	0x446000+AT_MEM_SPACE+MAPINITWORD
	.double	0x447000+AT_MEM_SPACE+MAPINITWORD
	.double	0x448000+AT_MEM_SPACE+MAPINITWORD
	.double	0x449000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x44f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x450000+AT_MEM_SPACE+MAPINITWORD
	.double	0x451000+AT_MEM_SPACE+MAPINITWORD
	.double	0x452000+AT_MEM_SPACE+MAPINITWORD
	.double	0x453000+AT_MEM_SPACE+MAPINITWORD
	.double	0x454000+AT_MEM_SPACE+MAPINITWORD
	.double	0x455000+AT_MEM_SPACE+MAPINITWORD
	.double	0x456000+AT_MEM_SPACE+MAPINITWORD
	.double	0x457000+AT_MEM_SPACE+MAPINITWORD
	.double	0x458000+AT_MEM_SPACE+MAPINITWORD
	.double	0x459000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x45f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x460000+AT_MEM_SPACE+MAPINITWORD
	.double	0x461000+AT_MEM_SPACE+MAPINITWORD
	.double	0x462000+AT_MEM_SPACE+MAPINITWORD
	.double	0x463000+AT_MEM_SPACE+MAPINITWORD
	.double	0x464000+AT_MEM_SPACE+MAPINITWORD
	.double	0x465000+AT_MEM_SPACE+MAPINITWORD
	.double	0x466000+AT_MEM_SPACE+MAPINITWORD
	.double	0x467000+AT_MEM_SPACE+MAPINITWORD
	.double	0x468000+AT_MEM_SPACE+MAPINITWORD
	.double	0x469000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x46f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x470000+AT_MEM_SPACE+MAPINITWORD
	.double	0x471000+AT_MEM_SPACE+MAPINITWORD
	.double	0x472000+AT_MEM_SPACE+MAPINITWORD
	.double	0x473000+AT_MEM_SPACE+MAPINITWORD
	.double	0x474000+AT_MEM_SPACE+MAPINITWORD
	.double	0x475000+AT_MEM_SPACE+MAPINITWORD
	.double	0x476000+AT_MEM_SPACE+MAPINITWORD
	.double	0x477000+AT_MEM_SPACE+MAPINITWORD
	.double	0x478000+AT_MEM_SPACE+MAPINITWORD
	.double	0x479000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x47f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x480000+AT_MEM_SPACE+MAPINITWORD
	.double	0x481000+AT_MEM_SPACE+MAPINITWORD
	.double	0x482000+AT_MEM_SPACE+MAPINITWORD
	.double	0x483000+AT_MEM_SPACE+MAPINITWORD
	.double	0x484000+AT_MEM_SPACE+MAPINITWORD
	.double	0x485000+AT_MEM_SPACE+MAPINITWORD
	.double	0x486000+AT_MEM_SPACE+MAPINITWORD
	.double	0x487000+AT_MEM_SPACE+MAPINITWORD
	.double	0x488000+AT_MEM_SPACE+MAPINITWORD
	.double	0x489000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x48f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x490000+AT_MEM_SPACE+MAPINITWORD
	.double	0x491000+AT_MEM_SPACE+MAPINITWORD
	.double	0x492000+AT_MEM_SPACE+MAPINITWORD
	.double	0x493000+AT_MEM_SPACE+MAPINITWORD
	.double	0x494000+AT_MEM_SPACE+MAPINITWORD
	.double	0x495000+AT_MEM_SPACE+MAPINITWORD
	.double	0x496000+AT_MEM_SPACE+MAPINITWORD
	.double	0x497000+AT_MEM_SPACE+MAPINITWORD
	.double	0x498000+AT_MEM_SPACE+MAPINITWORD
	.double	0x499000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x49f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x4ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x500000+AT_MEM_SPACE+MAPINITWORD
	.double	0x501000+AT_MEM_SPACE+MAPINITWORD
	.double	0x502000+AT_MEM_SPACE+MAPINITWORD
	.double	0x503000+AT_MEM_SPACE+MAPINITWORD
	.double	0x504000+AT_MEM_SPACE+MAPINITWORD
	.double	0x505000+AT_MEM_SPACE+MAPINITWORD
	.double	0x506000+AT_MEM_SPACE+MAPINITWORD
	.double	0x507000+AT_MEM_SPACE+MAPINITWORD
	.double	0x508000+AT_MEM_SPACE+MAPINITWORD
	.double	0x509000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x50f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x510000+AT_MEM_SPACE+MAPINITWORD
	.double	0x511000+AT_MEM_SPACE+MAPINITWORD
	.double	0x512000+AT_MEM_SPACE+MAPINITWORD
	.double	0x513000+AT_MEM_SPACE+MAPINITWORD
	.double	0x514000+AT_MEM_SPACE+MAPINITWORD
	.double	0x515000+AT_MEM_SPACE+MAPINITWORD
	.double	0x516000+AT_MEM_SPACE+MAPINITWORD
	.double	0x517000+AT_MEM_SPACE+MAPINITWORD
	.double	0x518000+AT_MEM_SPACE+MAPINITWORD
	.double	0x519000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x51f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x520000+AT_MEM_SPACE+MAPINITWORD
	.double	0x521000+AT_MEM_SPACE+MAPINITWORD
	.double	0x522000+AT_MEM_SPACE+MAPINITWORD
	.double	0x523000+AT_MEM_SPACE+MAPINITWORD
	.double	0x524000+AT_MEM_SPACE+MAPINITWORD
	.double	0x525000+AT_MEM_SPACE+MAPINITWORD
	.double	0x526000+AT_MEM_SPACE+MAPINITWORD
	.double	0x527000+AT_MEM_SPACE+MAPINITWORD
	.double	0x528000+AT_MEM_SPACE+MAPINITWORD
	.double	0x529000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x52f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x530000+AT_MEM_SPACE+MAPINITWORD
	.double	0x531000+AT_MEM_SPACE+MAPINITWORD
	.double	0x532000+AT_MEM_SPACE+MAPINITWORD
	.double	0x533000+AT_MEM_SPACE+MAPINITWORD
	.double	0x534000+AT_MEM_SPACE+MAPINITWORD
	.double	0x535000+AT_MEM_SPACE+MAPINITWORD
	.double	0x536000+AT_MEM_SPACE+MAPINITWORD
	.double	0x537000+AT_MEM_SPACE+MAPINITWORD
	.double	0x538000+AT_MEM_SPACE+MAPINITWORD
	.double	0x539000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x53f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x540000+AT_MEM_SPACE+MAPINITWORD
	.double	0x541000+AT_MEM_SPACE+MAPINITWORD
	.double	0x542000+AT_MEM_SPACE+MAPINITWORD
	.double	0x543000+AT_MEM_SPACE+MAPINITWORD
	.double	0x544000+AT_MEM_SPACE+MAPINITWORD
	.double	0x545000+AT_MEM_SPACE+MAPINITWORD
	.double	0x546000+AT_MEM_SPACE+MAPINITWORD
	.double	0x547000+AT_MEM_SPACE+MAPINITWORD
	.double	0x548000+AT_MEM_SPACE+MAPINITWORD
	.double	0x549000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x54f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x550000+AT_MEM_SPACE+MAPINITWORD
	.double	0x551000+AT_MEM_SPACE+MAPINITWORD
	.double	0x552000+AT_MEM_SPACE+MAPINITWORD
	.double	0x553000+AT_MEM_SPACE+MAPINITWORD
	.double	0x554000+AT_MEM_SPACE+MAPINITWORD
	.double	0x555000+AT_MEM_SPACE+MAPINITWORD
	.double	0x556000+AT_MEM_SPACE+MAPINITWORD
	.double	0x557000+AT_MEM_SPACE+MAPINITWORD
	.double	0x558000+AT_MEM_SPACE+MAPINITWORD
	.double	0x559000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x55f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x560000+AT_MEM_SPACE+MAPINITWORD
	.double	0x561000+AT_MEM_SPACE+MAPINITWORD
	.double	0x562000+AT_MEM_SPACE+MAPINITWORD
	.double	0x563000+AT_MEM_SPACE+MAPINITWORD
	.double	0x564000+AT_MEM_SPACE+MAPINITWORD
	.double	0x565000+AT_MEM_SPACE+MAPINITWORD
	.double	0x566000+AT_MEM_SPACE+MAPINITWORD
	.double	0x567000+AT_MEM_SPACE+MAPINITWORD
	.double	0x568000+AT_MEM_SPACE+MAPINITWORD
	.double	0x569000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x56f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x570000+AT_MEM_SPACE+MAPINITWORD
	.double	0x571000+AT_MEM_SPACE+MAPINITWORD
	.double	0x572000+AT_MEM_SPACE+MAPINITWORD
	.double	0x573000+AT_MEM_SPACE+MAPINITWORD
	.double	0x574000+AT_MEM_SPACE+MAPINITWORD
	.double	0x575000+AT_MEM_SPACE+MAPINITWORD
	.double	0x576000+AT_MEM_SPACE+MAPINITWORD
	.double	0x577000+AT_MEM_SPACE+MAPINITWORD
	.double	0x578000+AT_MEM_SPACE+MAPINITWORD
	.double	0x579000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x57f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x580000+AT_MEM_SPACE+MAPINITWORD
	.double	0x581000+AT_MEM_SPACE+MAPINITWORD
	.double	0x582000+AT_MEM_SPACE+MAPINITWORD
	.double	0x583000+AT_MEM_SPACE+MAPINITWORD
	.double	0x584000+AT_MEM_SPACE+MAPINITWORD
	.double	0x585000+AT_MEM_SPACE+MAPINITWORD
	.double	0x586000+AT_MEM_SPACE+MAPINITWORD
	.double	0x587000+AT_MEM_SPACE+MAPINITWORD
	.double	0x588000+AT_MEM_SPACE+MAPINITWORD
	.double	0x589000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x58f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x590000+AT_MEM_SPACE+MAPINITWORD
	.double	0x591000+AT_MEM_SPACE+MAPINITWORD
	.double	0x592000+AT_MEM_SPACE+MAPINITWORD
	.double	0x593000+AT_MEM_SPACE+MAPINITWORD
	.double	0x594000+AT_MEM_SPACE+MAPINITWORD
	.double	0x595000+AT_MEM_SPACE+MAPINITWORD
	.double	0x596000+AT_MEM_SPACE+MAPINITWORD
	.double	0x597000+AT_MEM_SPACE+MAPINITWORD
	.double	0x598000+AT_MEM_SPACE+MAPINITWORD
	.double	0x599000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x59f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x5ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x600000+AT_MEM_SPACE+MAPINITWORD
	.double	0x601000+AT_MEM_SPACE+MAPINITWORD
	.double	0x602000+AT_MEM_SPACE+MAPINITWORD
	.double	0x603000+AT_MEM_SPACE+MAPINITWORD
	.double	0x604000+AT_MEM_SPACE+MAPINITWORD
	.double	0x605000+AT_MEM_SPACE+MAPINITWORD
	.double	0x606000+AT_MEM_SPACE+MAPINITWORD
	.double	0x607000+AT_MEM_SPACE+MAPINITWORD
	.double	0x608000+AT_MEM_SPACE+MAPINITWORD
	.double	0x609000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x60f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x610000+AT_MEM_SPACE+MAPINITWORD
	.double	0x611000+AT_MEM_SPACE+MAPINITWORD
	.double	0x612000+AT_MEM_SPACE+MAPINITWORD
	.double	0x613000+AT_MEM_SPACE+MAPINITWORD
	.double	0x614000+AT_MEM_SPACE+MAPINITWORD
	.double	0x615000+AT_MEM_SPACE+MAPINITWORD
	.double	0x616000+AT_MEM_SPACE+MAPINITWORD
	.double	0x617000+AT_MEM_SPACE+MAPINITWORD
	.double	0x618000+AT_MEM_SPACE+MAPINITWORD
	.double	0x619000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x61f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x620000+AT_MEM_SPACE+MAPINITWORD
	.double	0x621000+AT_MEM_SPACE+MAPINITWORD
	.double	0x622000+AT_MEM_SPACE+MAPINITWORD
	.double	0x623000+AT_MEM_SPACE+MAPINITWORD
	.double	0x624000+AT_MEM_SPACE+MAPINITWORD
	.double	0x625000+AT_MEM_SPACE+MAPINITWORD
	.double	0x626000+AT_MEM_SPACE+MAPINITWORD
	.double	0x627000+AT_MEM_SPACE+MAPINITWORD
	.double	0x628000+AT_MEM_SPACE+MAPINITWORD
	.double	0x629000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x62f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x630000+AT_MEM_SPACE+MAPINITWORD
	.double	0x631000+AT_MEM_SPACE+MAPINITWORD
	.double	0x632000+AT_MEM_SPACE+MAPINITWORD
	.double	0x633000+AT_MEM_SPACE+MAPINITWORD
	.double	0x634000+AT_MEM_SPACE+MAPINITWORD
	.double	0x635000+AT_MEM_SPACE+MAPINITWORD
	.double	0x636000+AT_MEM_SPACE+MAPINITWORD
	.double	0x637000+AT_MEM_SPACE+MAPINITWORD
	.double	0x638000+AT_MEM_SPACE+MAPINITWORD
	.double	0x639000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x63f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x640000+AT_MEM_SPACE+MAPINITWORD
	.double	0x641000+AT_MEM_SPACE+MAPINITWORD
	.double	0x642000+AT_MEM_SPACE+MAPINITWORD
	.double	0x643000+AT_MEM_SPACE+MAPINITWORD
	.double	0x644000+AT_MEM_SPACE+MAPINITWORD
	.double	0x645000+AT_MEM_SPACE+MAPINITWORD
	.double	0x646000+AT_MEM_SPACE+MAPINITWORD
	.double	0x647000+AT_MEM_SPACE+MAPINITWORD
	.double	0x648000+AT_MEM_SPACE+MAPINITWORD
	.double	0x649000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x64f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x650000+AT_MEM_SPACE+MAPINITWORD
	.double	0x651000+AT_MEM_SPACE+MAPINITWORD
	.double	0x652000+AT_MEM_SPACE+MAPINITWORD
	.double	0x653000+AT_MEM_SPACE+MAPINITWORD
	.double	0x654000+AT_MEM_SPACE+MAPINITWORD
	.double	0x655000+AT_MEM_SPACE+MAPINITWORD
	.double	0x656000+AT_MEM_SPACE+MAPINITWORD
	.double	0x657000+AT_MEM_SPACE+MAPINITWORD
	.double	0x658000+AT_MEM_SPACE+MAPINITWORD
	.double	0x659000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x65f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x660000+AT_MEM_SPACE+MAPINITWORD
	.double	0x661000+AT_MEM_SPACE+MAPINITWORD
	.double	0x662000+AT_MEM_SPACE+MAPINITWORD
	.double	0x663000+AT_MEM_SPACE+MAPINITWORD
	.double	0x664000+AT_MEM_SPACE+MAPINITWORD
	.double	0x665000+AT_MEM_SPACE+MAPINITWORD
	.double	0x666000+AT_MEM_SPACE+MAPINITWORD
	.double	0x667000+AT_MEM_SPACE+MAPINITWORD
	.double	0x668000+AT_MEM_SPACE+MAPINITWORD
	.double	0x669000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x66f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x670000+AT_MEM_SPACE+MAPINITWORD
	.double	0x671000+AT_MEM_SPACE+MAPINITWORD
	.double	0x672000+AT_MEM_SPACE+MAPINITWORD
	.double	0x673000+AT_MEM_SPACE+MAPINITWORD
	.double	0x674000+AT_MEM_SPACE+MAPINITWORD
	.double	0x675000+AT_MEM_SPACE+MAPINITWORD
	.double	0x676000+AT_MEM_SPACE+MAPINITWORD
	.double	0x677000+AT_MEM_SPACE+MAPINITWORD
	.double	0x678000+AT_MEM_SPACE+MAPINITWORD
	.double	0x679000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x67f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x680000+AT_MEM_SPACE+MAPINITWORD
	.double	0x681000+AT_MEM_SPACE+MAPINITWORD
	.double	0x682000+AT_MEM_SPACE+MAPINITWORD
	.double	0x683000+AT_MEM_SPACE+MAPINITWORD
	.double	0x684000+AT_MEM_SPACE+MAPINITWORD
	.double	0x685000+AT_MEM_SPACE+MAPINITWORD
	.double	0x686000+AT_MEM_SPACE+MAPINITWORD
	.double	0x687000+AT_MEM_SPACE+MAPINITWORD
	.double	0x688000+AT_MEM_SPACE+MAPINITWORD
	.double	0x689000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x68f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x690000+AT_MEM_SPACE+MAPINITWORD
	.double	0x691000+AT_MEM_SPACE+MAPINITWORD
	.double	0x692000+AT_MEM_SPACE+MAPINITWORD
	.double	0x693000+AT_MEM_SPACE+MAPINITWORD
	.double	0x694000+AT_MEM_SPACE+MAPINITWORD
	.double	0x695000+AT_MEM_SPACE+MAPINITWORD
	.double	0x696000+AT_MEM_SPACE+MAPINITWORD
	.double	0x697000+AT_MEM_SPACE+MAPINITWORD
	.double	0x698000+AT_MEM_SPACE+MAPINITWORD
	.double	0x699000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x69f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x6ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x700000+AT_MEM_SPACE+MAPINITWORD
	.double	0x701000+AT_MEM_SPACE+MAPINITWORD
	.double	0x702000+AT_MEM_SPACE+MAPINITWORD
	.double	0x703000+AT_MEM_SPACE+MAPINITWORD
	.double	0x704000+AT_MEM_SPACE+MAPINITWORD
	.double	0x705000+AT_MEM_SPACE+MAPINITWORD
	.double	0x706000+AT_MEM_SPACE+MAPINITWORD
	.double	0x707000+AT_MEM_SPACE+MAPINITWORD
	.double	0x708000+AT_MEM_SPACE+MAPINITWORD
	.double	0x709000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x70f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x710000+AT_MEM_SPACE+MAPINITWORD
	.double	0x711000+AT_MEM_SPACE+MAPINITWORD
	.double	0x712000+AT_MEM_SPACE+MAPINITWORD
	.double	0x713000+AT_MEM_SPACE+MAPINITWORD
	.double	0x714000+AT_MEM_SPACE+MAPINITWORD
	.double	0x715000+AT_MEM_SPACE+MAPINITWORD
	.double	0x716000+AT_MEM_SPACE+MAPINITWORD
	.double	0x717000+AT_MEM_SPACE+MAPINITWORD
	.double	0x718000+AT_MEM_SPACE+MAPINITWORD
	.double	0x719000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x71f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x720000+AT_MEM_SPACE+MAPINITWORD
	.double	0x721000+AT_MEM_SPACE+MAPINITWORD
	.double	0x722000+AT_MEM_SPACE+MAPINITWORD
	.double	0x723000+AT_MEM_SPACE+MAPINITWORD
	.double	0x724000+AT_MEM_SPACE+MAPINITWORD
	.double	0x725000+AT_MEM_SPACE+MAPINITWORD
	.double	0x726000+AT_MEM_SPACE+MAPINITWORD
	.double	0x727000+AT_MEM_SPACE+MAPINITWORD
	.double	0x728000+AT_MEM_SPACE+MAPINITWORD
	.double	0x729000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x72f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x730000+AT_MEM_SPACE+MAPINITWORD
	.double	0x731000+AT_MEM_SPACE+MAPINITWORD
	.double	0x732000+AT_MEM_SPACE+MAPINITWORD
	.double	0x733000+AT_MEM_SPACE+MAPINITWORD
	.double	0x734000+AT_MEM_SPACE+MAPINITWORD
	.double	0x735000+AT_MEM_SPACE+MAPINITWORD
	.double	0x736000+AT_MEM_SPACE+MAPINITWORD
	.double	0x737000+AT_MEM_SPACE+MAPINITWORD
	.double	0x738000+AT_MEM_SPACE+MAPINITWORD
	.double	0x739000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x73f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x740000+AT_MEM_SPACE+MAPINITWORD
	.double	0x741000+AT_MEM_SPACE+MAPINITWORD
	.double	0x742000+AT_MEM_SPACE+MAPINITWORD
	.double	0x743000+AT_MEM_SPACE+MAPINITWORD
	.double	0x744000+AT_MEM_SPACE+MAPINITWORD
	.double	0x745000+AT_MEM_SPACE+MAPINITWORD
	.double	0x746000+AT_MEM_SPACE+MAPINITWORD
	.double	0x747000+AT_MEM_SPACE+MAPINITWORD
	.double	0x748000+AT_MEM_SPACE+MAPINITWORD
	.double	0x749000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x74f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x750000+AT_MEM_SPACE+MAPINITWORD
	.double	0x751000+AT_MEM_SPACE+MAPINITWORD
	.double	0x752000+AT_MEM_SPACE+MAPINITWORD
	.double	0x753000+AT_MEM_SPACE+MAPINITWORD
	.double	0x754000+AT_MEM_SPACE+MAPINITWORD
	.double	0x755000+AT_MEM_SPACE+MAPINITWORD
	.double	0x756000+AT_MEM_SPACE+MAPINITWORD
	.double	0x757000+AT_MEM_SPACE+MAPINITWORD
	.double	0x758000+AT_MEM_SPACE+MAPINITWORD
	.double	0x759000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x75f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x760000+AT_MEM_SPACE+MAPINITWORD
	.double	0x761000+AT_MEM_SPACE+MAPINITWORD
	.double	0x762000+AT_MEM_SPACE+MAPINITWORD
	.double	0x763000+AT_MEM_SPACE+MAPINITWORD
	.double	0x764000+AT_MEM_SPACE+MAPINITWORD
	.double	0x765000+AT_MEM_SPACE+MAPINITWORD
	.double	0x766000+AT_MEM_SPACE+MAPINITWORD
	.double	0x767000+AT_MEM_SPACE+MAPINITWORD
	.double	0x768000+AT_MEM_SPACE+MAPINITWORD
	.double	0x769000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x76f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x770000+AT_MEM_SPACE+MAPINITWORD
	.double	0x771000+AT_MEM_SPACE+MAPINITWORD
	.double	0x772000+AT_MEM_SPACE+MAPINITWORD
	.double	0x773000+AT_MEM_SPACE+MAPINITWORD
	.double	0x774000+AT_MEM_SPACE+MAPINITWORD
	.double	0x775000+AT_MEM_SPACE+MAPINITWORD
	.double	0x776000+AT_MEM_SPACE+MAPINITWORD
	.double	0x777000+AT_MEM_SPACE+MAPINITWORD
	.double	0x778000+AT_MEM_SPACE+MAPINITWORD
	.double	0x779000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x77f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x780000+AT_MEM_SPACE+MAPINITWORD
	.double	0x781000+AT_MEM_SPACE+MAPINITWORD
	.double	0x782000+AT_MEM_SPACE+MAPINITWORD
	.double	0x783000+AT_MEM_SPACE+MAPINITWORD
	.double	0x784000+AT_MEM_SPACE+MAPINITWORD
	.double	0x785000+AT_MEM_SPACE+MAPINITWORD
	.double	0x786000+AT_MEM_SPACE+MAPINITWORD
	.double	0x787000+AT_MEM_SPACE+MAPINITWORD
	.double	0x788000+AT_MEM_SPACE+MAPINITWORD
	.double	0x789000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x78f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x790000+AT_MEM_SPACE+MAPINITWORD
	.double	0x791000+AT_MEM_SPACE+MAPINITWORD
	.double	0x792000+AT_MEM_SPACE+MAPINITWORD
	.double	0x793000+AT_MEM_SPACE+MAPINITWORD
	.double	0x794000+AT_MEM_SPACE+MAPINITWORD
	.double	0x795000+AT_MEM_SPACE+MAPINITWORD
	.double	0x796000+AT_MEM_SPACE+MAPINITWORD
	.double	0x797000+AT_MEM_SPACE+MAPINITWORD
	.double	0x798000+AT_MEM_SPACE+MAPINITWORD
	.double	0x799000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x79f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x7ff000+AT_MEM_SPACE+MAPINITWORD

# PAGE A
# BUS-MASTER 2nd level page table entries
pagebm2:
	.double	0x800000+AT_MEM_SPACE+MAPINITWORD
	.double	0x801000+AT_MEM_SPACE+MAPINITWORD
	.double	0x802000+AT_MEM_SPACE+MAPINITWORD
	.double	0x803000+AT_MEM_SPACE+MAPINITWORD
	.double	0x804000+AT_MEM_SPACE+MAPINITWORD
	.double	0x805000+AT_MEM_SPACE+MAPINITWORD
	.double	0x806000+AT_MEM_SPACE+MAPINITWORD
	.double	0x807000+AT_MEM_SPACE+MAPINITWORD
	.double	0x808000+AT_MEM_SPACE+MAPINITWORD
	.double	0x809000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x80f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x810000+AT_MEM_SPACE+MAPINITWORD
	.double	0x811000+AT_MEM_SPACE+MAPINITWORD
	.double	0x812000+AT_MEM_SPACE+MAPINITWORD
	.double	0x813000+AT_MEM_SPACE+MAPINITWORD
	.double	0x814000+AT_MEM_SPACE+MAPINITWORD
	.double	0x815000+AT_MEM_SPACE+MAPINITWORD
	.double	0x816000+AT_MEM_SPACE+MAPINITWORD
	.double	0x817000+AT_MEM_SPACE+MAPINITWORD
	.double	0x818000+AT_MEM_SPACE+MAPINITWORD
	.double	0x819000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x81f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x820000+AT_MEM_SPACE+MAPINITWORD
	.double	0x821000+AT_MEM_SPACE+MAPINITWORD
	.double	0x822000+AT_MEM_SPACE+MAPINITWORD
	.double	0x823000+AT_MEM_SPACE+MAPINITWORD
	.double	0x824000+AT_MEM_SPACE+MAPINITWORD
	.double	0x825000+AT_MEM_SPACE+MAPINITWORD
	.double	0x826000+AT_MEM_SPACE+MAPINITWORD
	.double	0x827000+AT_MEM_SPACE+MAPINITWORD
	.double	0x828000+AT_MEM_SPACE+MAPINITWORD
	.double	0x829000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x82f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x830000+AT_MEM_SPACE+MAPINITWORD
	.double	0x831000+AT_MEM_SPACE+MAPINITWORD
	.double	0x832000+AT_MEM_SPACE+MAPINITWORD
	.double	0x833000+AT_MEM_SPACE+MAPINITWORD
	.double	0x834000+AT_MEM_SPACE+MAPINITWORD
	.double	0x835000+AT_MEM_SPACE+MAPINITWORD
	.double	0x836000+AT_MEM_SPACE+MAPINITWORD
	.double	0x837000+AT_MEM_SPACE+MAPINITWORD
	.double	0x838000+AT_MEM_SPACE+MAPINITWORD
	.double	0x839000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x83f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x840000+AT_MEM_SPACE+MAPINITWORD
	.double	0x841000+AT_MEM_SPACE+MAPINITWORD
	.double	0x842000+AT_MEM_SPACE+MAPINITWORD
	.double	0x843000+AT_MEM_SPACE+MAPINITWORD
	.double	0x844000+AT_MEM_SPACE+MAPINITWORD
	.double	0x845000+AT_MEM_SPACE+MAPINITWORD
	.double	0x846000+AT_MEM_SPACE+MAPINITWORD
	.double	0x847000+AT_MEM_SPACE+MAPINITWORD
	.double	0x848000+AT_MEM_SPACE+MAPINITWORD
	.double	0x849000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x84f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x850000+AT_MEM_SPACE+MAPINITWORD
	.double	0x851000+AT_MEM_SPACE+MAPINITWORD
	.double	0x852000+AT_MEM_SPACE+MAPINITWORD
	.double	0x853000+AT_MEM_SPACE+MAPINITWORD
	.double	0x854000+AT_MEM_SPACE+MAPINITWORD
	.double	0x855000+AT_MEM_SPACE+MAPINITWORD
	.double	0x856000+AT_MEM_SPACE+MAPINITWORD
	.double	0x857000+AT_MEM_SPACE+MAPINITWORD
	.double	0x858000+AT_MEM_SPACE+MAPINITWORD
	.double	0x859000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x85f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x860000+AT_MEM_SPACE+MAPINITWORD
	.double	0x861000+AT_MEM_SPACE+MAPINITWORD
	.double	0x862000+AT_MEM_SPACE+MAPINITWORD
	.double	0x863000+AT_MEM_SPACE+MAPINITWORD
	.double	0x864000+AT_MEM_SPACE+MAPINITWORD
	.double	0x865000+AT_MEM_SPACE+MAPINITWORD
	.double	0x866000+AT_MEM_SPACE+MAPINITWORD
	.double	0x867000+AT_MEM_SPACE+MAPINITWORD
	.double	0x868000+AT_MEM_SPACE+MAPINITWORD
	.double	0x869000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x86f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x870000+AT_MEM_SPACE+MAPINITWORD
	.double	0x871000+AT_MEM_SPACE+MAPINITWORD
	.double	0x872000+AT_MEM_SPACE+MAPINITWORD
	.double	0x873000+AT_MEM_SPACE+MAPINITWORD
	.double	0x874000+AT_MEM_SPACE+MAPINITWORD
	.double	0x875000+AT_MEM_SPACE+MAPINITWORD
	.double	0x876000+AT_MEM_SPACE+MAPINITWORD
	.double	0x877000+AT_MEM_SPACE+MAPINITWORD
	.double	0x878000+AT_MEM_SPACE+MAPINITWORD
	.double	0x879000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x87f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x880000+AT_MEM_SPACE+MAPINITWORD
	.double	0x881000+AT_MEM_SPACE+MAPINITWORD
	.double	0x882000+AT_MEM_SPACE+MAPINITWORD
	.double	0x883000+AT_MEM_SPACE+MAPINITWORD
	.double	0x884000+AT_MEM_SPACE+MAPINITWORD
	.double	0x885000+AT_MEM_SPACE+MAPINITWORD
	.double	0x886000+AT_MEM_SPACE+MAPINITWORD
	.double	0x887000+AT_MEM_SPACE+MAPINITWORD
	.double	0x888000+AT_MEM_SPACE+MAPINITWORD
	.double	0x889000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x88f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x890000+AT_MEM_SPACE+MAPINITWORD
	.double	0x891000+AT_MEM_SPACE+MAPINITWORD
	.double	0x892000+AT_MEM_SPACE+MAPINITWORD
	.double	0x893000+AT_MEM_SPACE+MAPINITWORD
	.double	0x894000+AT_MEM_SPACE+MAPINITWORD
	.double	0x895000+AT_MEM_SPACE+MAPINITWORD
	.double	0x896000+AT_MEM_SPACE+MAPINITWORD
	.double	0x897000+AT_MEM_SPACE+MAPINITWORD
	.double	0x898000+AT_MEM_SPACE+MAPINITWORD
	.double	0x899000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x89f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x8ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0x900000+AT_MEM_SPACE+MAPINITWORD
	.double	0x901000+AT_MEM_SPACE+MAPINITWORD
	.double	0x902000+AT_MEM_SPACE+MAPINITWORD
	.double	0x903000+AT_MEM_SPACE+MAPINITWORD
	.double	0x904000+AT_MEM_SPACE+MAPINITWORD
	.double	0x905000+AT_MEM_SPACE+MAPINITWORD
	.double	0x906000+AT_MEM_SPACE+MAPINITWORD
	.double	0x907000+AT_MEM_SPACE+MAPINITWORD
	.double	0x908000+AT_MEM_SPACE+MAPINITWORD
	.double	0x909000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x90f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x910000+AT_MEM_SPACE+MAPINITWORD
	.double	0x911000+AT_MEM_SPACE+MAPINITWORD
	.double	0x912000+AT_MEM_SPACE+MAPINITWORD
	.double	0x913000+AT_MEM_SPACE+MAPINITWORD
	.double	0x914000+AT_MEM_SPACE+MAPINITWORD
	.double	0x915000+AT_MEM_SPACE+MAPINITWORD
	.double	0x916000+AT_MEM_SPACE+MAPINITWORD
	.double	0x917000+AT_MEM_SPACE+MAPINITWORD
	.double	0x918000+AT_MEM_SPACE+MAPINITWORD
	.double	0x919000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x91f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x920000+AT_MEM_SPACE+MAPINITWORD
	.double	0x921000+AT_MEM_SPACE+MAPINITWORD
	.double	0x922000+AT_MEM_SPACE+MAPINITWORD
	.double	0x923000+AT_MEM_SPACE+MAPINITWORD
	.double	0x924000+AT_MEM_SPACE+MAPINITWORD
	.double	0x925000+AT_MEM_SPACE+MAPINITWORD
	.double	0x926000+AT_MEM_SPACE+MAPINITWORD
	.double	0x927000+AT_MEM_SPACE+MAPINITWORD
	.double	0x928000+AT_MEM_SPACE+MAPINITWORD
	.double	0x929000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x92f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x930000+AT_MEM_SPACE+MAPINITWORD
	.double	0x931000+AT_MEM_SPACE+MAPINITWORD
	.double	0x932000+AT_MEM_SPACE+MAPINITWORD
	.double	0x933000+AT_MEM_SPACE+MAPINITWORD
	.double	0x934000+AT_MEM_SPACE+MAPINITWORD
	.double	0x935000+AT_MEM_SPACE+MAPINITWORD
	.double	0x936000+AT_MEM_SPACE+MAPINITWORD
	.double	0x937000+AT_MEM_SPACE+MAPINITWORD
	.double	0x938000+AT_MEM_SPACE+MAPINITWORD
	.double	0x939000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x93f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x940000+AT_MEM_SPACE+MAPINITWORD
	.double	0x941000+AT_MEM_SPACE+MAPINITWORD
	.double	0x942000+AT_MEM_SPACE+MAPINITWORD
	.double	0x943000+AT_MEM_SPACE+MAPINITWORD
	.double	0x944000+AT_MEM_SPACE+MAPINITWORD
	.double	0x945000+AT_MEM_SPACE+MAPINITWORD
	.double	0x946000+AT_MEM_SPACE+MAPINITWORD
	.double	0x947000+AT_MEM_SPACE+MAPINITWORD
	.double	0x948000+AT_MEM_SPACE+MAPINITWORD
	.double	0x949000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x94f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x950000+AT_MEM_SPACE+MAPINITWORD
	.double	0x951000+AT_MEM_SPACE+MAPINITWORD
	.double	0x952000+AT_MEM_SPACE+MAPINITWORD
	.double	0x953000+AT_MEM_SPACE+MAPINITWORD
	.double	0x954000+AT_MEM_SPACE+MAPINITWORD
	.double	0x955000+AT_MEM_SPACE+MAPINITWORD
	.double	0x956000+AT_MEM_SPACE+MAPINITWORD
	.double	0x957000+AT_MEM_SPACE+MAPINITWORD
	.double	0x958000+AT_MEM_SPACE+MAPINITWORD
	.double	0x959000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x95f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x960000+AT_MEM_SPACE+MAPINITWORD
	.double	0x961000+AT_MEM_SPACE+MAPINITWORD
	.double	0x962000+AT_MEM_SPACE+MAPINITWORD
	.double	0x963000+AT_MEM_SPACE+MAPINITWORD
	.double	0x964000+AT_MEM_SPACE+MAPINITWORD
	.double	0x965000+AT_MEM_SPACE+MAPINITWORD
	.double	0x966000+AT_MEM_SPACE+MAPINITWORD
	.double	0x967000+AT_MEM_SPACE+MAPINITWORD
	.double	0x968000+AT_MEM_SPACE+MAPINITWORD
	.double	0x969000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x96f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x970000+AT_MEM_SPACE+MAPINITWORD
	.double	0x971000+AT_MEM_SPACE+MAPINITWORD
	.double	0x972000+AT_MEM_SPACE+MAPINITWORD
	.double	0x973000+AT_MEM_SPACE+MAPINITWORD
	.double	0x974000+AT_MEM_SPACE+MAPINITWORD
	.double	0x975000+AT_MEM_SPACE+MAPINITWORD
	.double	0x976000+AT_MEM_SPACE+MAPINITWORD
	.double	0x977000+AT_MEM_SPACE+MAPINITWORD
	.double	0x978000+AT_MEM_SPACE+MAPINITWORD
	.double	0x979000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x97f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x980000+AT_MEM_SPACE+MAPINITWORD
	.double	0x981000+AT_MEM_SPACE+MAPINITWORD
	.double	0x982000+AT_MEM_SPACE+MAPINITWORD
	.double	0x983000+AT_MEM_SPACE+MAPINITWORD
	.double	0x984000+AT_MEM_SPACE+MAPINITWORD
	.double	0x985000+AT_MEM_SPACE+MAPINITWORD
	.double	0x986000+AT_MEM_SPACE+MAPINITWORD
	.double	0x987000+AT_MEM_SPACE+MAPINITWORD
	.double	0x988000+AT_MEM_SPACE+MAPINITWORD
	.double	0x989000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x98f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x990000+AT_MEM_SPACE+MAPINITWORD
	.double	0x991000+AT_MEM_SPACE+MAPINITWORD
	.double	0x992000+AT_MEM_SPACE+MAPINITWORD
	.double	0x993000+AT_MEM_SPACE+MAPINITWORD
	.double	0x994000+AT_MEM_SPACE+MAPINITWORD
	.double	0x995000+AT_MEM_SPACE+MAPINITWORD
	.double	0x996000+AT_MEM_SPACE+MAPINITWORD
	.double	0x997000+AT_MEM_SPACE+MAPINITWORD
	.double	0x998000+AT_MEM_SPACE+MAPINITWORD
	.double	0x999000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99a000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99b000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99c000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99d000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99e000+AT_MEM_SPACE+MAPINITWORD
	.double	0x99f000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9a9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9aa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ab000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ac000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ad000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ae000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9af000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9b9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ba000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9bb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9bc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9bd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9be000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9bf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9c9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ca000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9cb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9cc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9cd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ce000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9cf000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9d9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9da000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9db000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9dc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9dd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9de000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9df000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9e9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ea000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9eb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ec000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ed000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ee000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ef000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f0000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f1000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f2000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f3000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f4000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f5000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f6000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f7000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f8000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9f9000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9fa000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9fb000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9fc000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9fd000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9fe000+AT_MEM_SPACE+MAPINITWORD
	.double	0x9ff000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xa9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaa9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xab9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xabb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xabc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xabd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xabe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xabf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xac9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xacb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xacc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xacd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xace000+AT_MEM_SPACE+MAPINITWORD
	.double	0xacf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xad9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xada000+AT_MEM_SPACE+MAPINITWORD
	.double	0xadb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xadc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xadd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xade000+AT_MEM_SPACE+MAPINITWORD
	.double	0xadf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xae9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaed000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaf9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xafa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xafb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xafc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xafd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xafe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xaff000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xb9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xba9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbbb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbbc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbbd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbbe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbbf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbc9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbcb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbcc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbcd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbce000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbcf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbd9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbdb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbdc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbdd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbde000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbdf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbe9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbed000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbf9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbfa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbfb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbfc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbfd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbfe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xbff000+AT_MEM_SPACE+MAPINITWORD

# PAGE B
# BUS-MASTER 2nd level page table entries
pagebm3:
	.double	0xc00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xc9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xca9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcbb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcbc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcbd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcbe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcbf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcc9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xccb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xccc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xccd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcce000+AT_MEM_SPACE+MAPINITWORD
	.double	0xccf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcd9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcdb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcdc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcdd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcde000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcdf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xce9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xceb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xced000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcf9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcfa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcfb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcfc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcfd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcfe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xcff000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xd9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xda9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdbb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdbc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdbd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdbe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdbf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdc9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdcb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdcc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdcd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdce000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdcf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdd9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xddb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xddc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xddd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdde000+AT_MEM_SPACE+MAPINITWORD
	.double	0xddf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xde9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xded000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdf9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdfa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdfb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdfc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdfd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdfe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xdff000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xe9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xea9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xead000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xebb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xebc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xebd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xebe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xebf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xec9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xecb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xecc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xecd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xece000+AT_MEM_SPACE+MAPINITWORD
	.double	0xecf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xed9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xedb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xedc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xedd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xede000+AT_MEM_SPACE+MAPINITWORD
	.double	0xedf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xee9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeed000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xef9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xefa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xefb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xefc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xefd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xefe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xeff000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf00000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf01000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf02000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf03000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf04000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf05000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf06000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf07000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf08000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf09000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf0f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf10000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf11000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf12000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf13000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf14000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf15000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf16000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf17000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf18000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf19000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf1f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf20000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf21000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf22000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf23000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf24000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf25000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf26000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf27000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf28000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf29000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf2f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf30000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf31000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf32000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf33000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf34000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf35000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf36000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf37000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf38000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf39000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf3f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf40000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf41000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf42000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf43000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf44000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf45000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf46000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf47000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf48000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf49000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf4f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf50000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf51000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf52000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf53000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf54000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf55000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf56000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf57000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf58000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf59000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf5f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf60000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf61000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf62000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf63000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf64000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf65000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf66000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf67000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf68000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf69000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf6f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf70000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf71000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf72000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf73000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf74000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf75000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf76000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf77000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf78000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf79000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf7f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf80000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf81000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf82000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf83000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf84000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf85000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf86000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf87000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf88000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf89000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf8f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf90000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf91000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf92000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf93000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf94000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf95000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf96000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf97000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf98000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf99000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9a000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9b000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9c000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9d000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9e000+AT_MEM_SPACE+MAPINITWORD
	.double	0xf9f000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfa9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfaa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfab000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfac000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfad000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfae000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfaf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfb9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfba000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfbb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfbc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfbd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfbe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfbf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfc9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfca000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfcb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfcc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfcd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfce000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfcf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfd9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfda000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfdb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfdc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfdd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfde000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfdf000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfe9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfea000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfeb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfec000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfed000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfee000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfef000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff0000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff1000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff2000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff3000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff4000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff5000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff6000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff7000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff8000+AT_MEM_SPACE+MAPINITWORD
	.double	0xff9000+AT_MEM_SPACE+MAPINITWORD
	.double	0xffa000+AT_MEM_SPACE+MAPINITWORD
	.double	0xffb000+AT_MEM_SPACE+MAPINITWORD
	.double	0xffc000+AT_MEM_SPACE+MAPINITWORD
	.double	0xffd000+AT_MEM_SPACE+MAPINITWORD
	.double	0xffe000+AT_MEM_SPACE+MAPINITWORD
	.double	0xfff000+AT_MEM_SPACE+MAPINITWORD
