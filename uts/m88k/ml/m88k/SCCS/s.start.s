h31274
s 00270/00000/00000
d D 1.1 90/03/06 12:46:20 root 1 0
c date and time created 90/03/06 12:46:20 by root
e
u
U
t
T
I 1
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)start.m4	6.8	

; MC78000 version
; The code in this file is executed at system bootstrap.
#ifdef	ASPEN
	def	GRAIN,1024		; (ASPEN specific)
#endif
#ifdef CMMU
	def	GRAIN,	0x1000		; CMMU fixed 4k page
#endif
	def	ROUND,GRAIN-1		; granularity round
	def	GMASK,-GRAIN		; granularity mask
	def	STSIZE,8192		; boot and interrupt stack for system
	def	U_SIZE,8192		; U block for Proc 0
	def	USRPSR,0		; userstate, interrupts enabled.
	def	SCRATCH,16*4		; Scratch space needed for temps.
;					  Scratch is deducted from the 
;					  interrupt stack space so don't
;					  make it too big.

	data	; these variables can't be in bss space
	global	physmem
physmem:	word	0		; total memory
vbr:		word	0		; contents of vector base register
	global	Dconcurrent
	global	_Dconcurrent
_Dconcurrent:
Dconcurrent:	word	0		; diagnostic concurrent mode enabled

bootdriv:	global bootdriv		; boot drive from %d0
		word 0
bootctlr:	global bootctlr		; boot controller from %d1
		word 0
bootinod:	global bootinod		; boot inode from %d2
		word 0
dumpdev:	word -1

	text
start:	global	start
_start:	global	_start
__start:	global	__start

	or.u	r26,r0,0		; special hole for linker synthesis
	or	r26,r26,0		; special hole for linker synthesis
	or.u	r27,r0,0		; special hole for linker synthesis
	or	r27,r27,0		; special hole for linker synthesis
	or.u	r28,r0,0		; special hole for linker synthesis
	or	r28,r28,0		; special hole for linker synthesis
	or.u	r29,r0,0		; special hole for linker synthesis
	or	r29,r29,0		; special hole for linker synthesis
#ifdef	CMMU
	bsr	_cache_init		; QUICKLY INITIALIZE ALL CMMU CACHE
#endif

	or.u	r5,r0,hi16(_linkregs)
	or	r5,r5,lo16(_linkregs)
	st	r26,r5,0
	st	r27,r5,4
	st	r28,r5,8
	st	r29,r5,0xc
	
	ldcr	r5,cr1			; enable fpu and
	and	r5,r5,0xfff3		; misaligned access trap
	stcr	r5,cr1

	tb1	0,r0,0x80		; wait for scorboard to be clear
	stcr	r0,cr3			; clear shadow scoreboard

	fstcr	r0,cr62		; Default Inexact FP exceptions off
	fstcr	r0,cr63		; Default Inexact FP exceptions off
;	lda.b	r5,r0,_end+ROUND	; top of unix
	or.u	r5,r0,hi16(_end)
	lda.b	r5,r5,lo16(_end)
	addu	r5,r5,ROUND		; top of unix
	and	r5,r5,lo16(GMASK)
	add	r5,r5,STSIZE-SCRATCH	; for boot stack area.  This
;					 allocates 16 longs for temp
;					 storage also.
	stcr	r5,cr19
;					  during initial debug.

;	bsr	_kdb_init

	or.u	r5,r0,hi16(HIPRI)	; Set up psr to block interrupts.
	or	r5,r5,lo16(HIPRI)
;
	ldcr	r5,cr0			; Save PID register.
;	st	r5,r0,_chiprev
	or.u	r6,r0,hi16(_chiprev)
	st	r5,r6,lo16(_chiprev)
;
	or.u	r5,r0,hi16(0x88000)	; Set CPU type.
	or	r5,r5,lo16(0x88000)
;	st	r5,r0,_cputype 
	or.u	r6,r0,hi16(_cputype)
	st	r5,r6,lo16(_cputype)
;

;	st	r2,r0,bootdriv		; Save boot loader passed info.
;	st	r3,r0,bootctlr		; 
;	st	r4,r0,bootinod

;	ldcr	r5,cr20			; get rom value in cr20
;	st	r5,r0,_dbgcr20		; and save it (for later restore)

;	or.u	r5,r0,hi16(instint)	; Set up vector to ROM illegal handler.
;	or	r5,r5,lo16(instint)
;	st	r5,r0,_dbgill

;	or.u	r5,r0,hi16(breakpoint)	; Setup vector to ROM breakpoint hndlr.
;	or	r5,r5,lo16(breakpoint)
;	st	r5,r0,_dbgbkpt

;	ldcr	r5,cr7			; save ROMs vbr
;	st	r5,r0,romvbr

	or.u	r5,r0,hi16(_M88Kvec)  	; Setup vbr
	or	r5,r5,lo16(_M88Kvec)
	stcr	r5,cr7
;	st	r5,r0,vbr
	or.u	r6,r0,hi16(vbr)
	st	r5,r6,lo16(vbr)

#ifdef	ASPEN
; ASPEN specific.
	or.u	r5,r0,hi16(FSCC) 	; Flush the caches.
	or	r5,r5,lo16(FSCC) 
	st	r0,r5,0		  	; Flush SCC
	st	r0,r5,4		  	; Flush SDC
	st	r0,r5,8		  	; Flush UCC
	st	r0,r5,12		; Flush UDC

	or.u	r5,r0,hi16(CSR)  ; Setup control register.
	or	r5,r5,lo16(CSR)
	add	r4,r0,CACHEON+VMEINT
;	st.b	r4,r5,0

	or.u	r5,r0,hi16(ASR)  ; Setup Address modifier register.
	or	r5,r5,lo16(ASR)
	add	r4,r0,0xFD
;	st.b	r4,r5,0
; end of ASPEN specific.
#endif

#if 0
;	add	r2,r0,4			; Remove patch for no ramdisk.

	bcnd	lt0,r2,@L%strt		; check for valid boot device.
	lda	r4,r0,_sysdevs		;
	add	r4,r4,4
	mak	r2,r2,2<4>		; Get pointer to boot device info.
	mak	r3,r3,4<6>
	add	r4,r4,r2
	add	r4,r4,r3

	ld.h	r2,r4,0			; Setup boot device info.
	st.h	r2,r0,_rootdev
	ld.h	r2,r4,2
	st.h	r2,r0,_pipedev
	ld.h	r2,r4,4
	st.h	r2,r0,_dumpdev
	ld.h	r2,r4,6
	st.h	r2,r0,_swapdev
	ld	r2,r4,8
	st	r2,r0,_swplo
	ld	r2,r4,12
	st	r2,r0,_nswap
#endif
	
@L%strt:
;	lda.b	r2,r0,_end+ROUND	; top of unix
	or.u	r2,r0,hi16(_end)
	lda.b	r2,r2,lo16(_end)
	addu	r2,r2,ROUND		; top of unix
	and	r2,r2,lo16(GMASK)
	add	r2,r2,STSIZE-SCRATCH	; for boot stack area.  This
;					 allocates 16 longs for temp
;					 storage also.
;	lda.b	r3,r0,_edata
	or.u	r3,r0,hi16(_edata)
	lda.b	r3,r3,lo16(_edata)
	sub	r4,r2,r3		; Get count.

#ifndef SIMULATOR
@L%clr:	st	r0,r3,0			; Clear next word of bss.
	sub	r4,r4,4
	bcnd.n	ge0,r4,@L%clr
	add	r3,r3,4
#endif

; Call C routines to startup system.

;	tb0	0,r0,133		; Call kdb.

	add	r31,r2,0		; Use boot stack area until mmu is on.
					; afterwards, use area as interrupt stk
					; pass start of ram address to mlsetup
	add	r2,r2,SCRATCH		; Start above scratch area.
	subu	r31,r31,HOMESP		; make room for saving passed params 
	bsr	_mlsetup		; mlsetup(physaddr)

	or.u	r31,r0,hi16(_u+U_SIZE)  ; Switch into u block.
	or	r31,r31,lo16(_u+U_SIZE)
	stcr	r31,cr17		; Setup master stack ptr.
	subu	r31,r31,HOMESP		; make room for saving passed params 
	bsr	_main			; main()

					; 1st proc 0 returns here
	or.u	r2,r0,hi16(HIPRI)	; Mask off interrupts while
	or	r2,r2,lo16(HIPRI)	; loading shadows.
	stcr	r2,cr1
	set	r2,r2,1<PS_EXP>
	stcr	r2,cr1
	add	r2,r0,USRPSR		; Setup user psr.
	stcr	r2,cr2			; Setup psr.
	stcr	r0,cr3			; Setup user SSB.
	stcr	r0,cr5			; SNIP.
	set	r2,r0,1<VALIDIP>	; Set initial start at 0.
	stcr	r2,cr6			; SFIP.
	br	goof
goof2:
	or	r0,r0,r0
	rte				; execute user process
goof:
	br	goof2

	data
romillv:
	word	Xtrap
_linkregs:
	word	0
	word	0
	word	0
	word	0

	global	_cputype
_cputype:
	word	0
	global	_chiprev
_chiprev:
	word	0

	global	_dbgswab
_dbgswab:			; vector to ROM/KDB NMI interrupt.
	word	0
	global	_dbgpanic

_dbgpanic:			; vector to ROM/KDB NMI interrupt.
	word	0
	global	_dbgtrace

_dbgtrace:			; vector to ROM/KDB NMI interrupt.
	word	0

	global	_dbgill		; vector to ROM/KDB illegal handler.
_dbgill:
	word	0
	global	_dbgbkpt	; vector to ROM/KDB breakpoint handler.

_dbgbkpt:
	word	0
_dbgcr20:			; ROM value in cr20.
	word	0
romvbr:
	word	0


	text
E 1
