h18799
s 00230/00000/00000
d D 1.1 90/03/06 12:46:19 root 1 0
c date and time created 90/03/06 12:46:19 by root
e
u
U
t
T
I 1
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)mmb.s	6.10	
;
#ifdef	ASPEN
; mmbstore(reg, val)		/* store to mmu register */
; register char *reg;	
; register int val;	
;  For now, these routines are included here.  These eventually might
; be broken into macros or maybe go away entirely.
;  This is a ASPEN specific module.

	data
	global _mmbad
_mmbad:	word 0x10000038		; Address of MMC control register.
	text
	global	_mmbstore
_mmbstore:
	ldcr	r5,cr1
	set	r6,r5,1<PS_SER>
	stcr	r6,cr1
	ld	r4,r0,_mmbad
	st	r3,r4,r2
	stcr	r5,cr1
	jmp	r1

;
;  The following routine sets the global flag "do_pmmu" to indicate
; whether or not a PMMU was found.
_chk_mmu: global	_chk_mmu
; This routine should go away.
	st	r0,r0,_do_pmmu   ; Clear it always.
	jmp	r1
	data
	global	_do_pmmu
_do_pmmu:
	word	0
	text

	global	_wakemmu
_wakemmu:
	jmp	r1
#endif
#ifdef	CMMU
;
;	These routine are CMMU specific, they should probably be in new
;	file cmmu.s but are in here for ease in porting
;
	global	_ccacheflush
_ccacheflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	bcnd.n	ne0,r2,@Lccpf
	ld	r13,r13,lo16(CCMMUBASE)

	br.n	@Lccfe
	or	r12,r0,INVALL		; invalidate all code cache
@Lccpf:
	and	r2,r2,0xf000		; clear page offset
	st	r2,r13,SADR		; write page number
	tb1	0,r0,128		; wait for instruction to finish
	or	r12,r0,INVPG		; invalidate page of code cache
@Lccfe:
	st	r12,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	ld	r2,r13,SSR		; wait for mmu completion
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r13,r0,r0		; clear all traces of CMMU addr
					; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

	global	_dcacheflush
_dcacheflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(DCMMUBASE)
	bcnd.n	ne0,r2,@Ldcpf
	ld	r13,r13,lo16(DCMMUBASE)

	br.n	@Ldcfe
	or	r12,r0,WINALL		; write and invalidate all data cache
@Ldcpf:
	and	r2,r2,0xf000		; clear page offset
	st	r2,r13,SADR		; write page number
	tb1	0,r0,128		; wait for instruction to finish
	or	r12,r0,WINPG		; writeback and inv page of data cache
@Ldcfe:
	st	r12,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	ld	r2,r13,SSR		; wait for mmu completion
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	bb1	14,r2,dcerr		; Bus Error on Flush

	or	r13,r0,r0		; clear all traces of CMMU addr
					; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

dcerr:
	ld	r3,r13,SADR
	tb1	0,r0,128		; wait for instruction to finish
	br	_breakpoint

	global	_cmmuflush
_cmmuflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	ld	r13,r13,lo16(CCMMUBASE)
	or.u	r8,r0,hi16(DCMMUBASE)
	ld	r8,r8,lo16(DCMMUBASE)

	or.u	r10,r0,hi16(cmmucmd)
	ld	r10,r10,lo16(cmmucmd)	; get command register bits
	or	r12,r10,INVUATC		; invalidate all users ATC
	st	r12,r13,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r12,r8,SCMR		; flush data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r12,r10,INVSATC		; invalidate all supv ATC
	st	r12,r13,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r12,r8,SCMR		; flush data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r13,r0,r0		; clear all traces of CMMU addr
	or	r8,r0,r0		; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

	global	_cmmu_store
_cmmu_store:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	ld	r13,r13,lo16(CCMMUBASE)
	or.u	r7,r0,hi16(DCMMUBASE)
	ld	r7,r7,lo16(DCMMUBASE)

	bcnd	ne0,r2,@Lnotcmr		; store command register?
	or.u	r12,r0,hi16(cmmucmd)
	ld	r8,r12,lo16(cmmucmd)	; get command register bits
	or	r3,r3,r8		; oring then to command
	st	r3,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r3,r7,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	br	@Lexit

@Lnotcmr:
	or	r3,r3,0x0001		; set valid bit for root pointers

	or.u	r12,r0,hi16(ccacheinh)
	ld	r8,r12,lo16(ccacheinh)	; get code cache inhibit bit

	sub	r10,r2,STOURP		; store user root pointer?
	bcnd	ne0,r10,@Lnotusr

	or.u	r12,r0,hi16(ucacheinh)
	ld	r9,r12,lo16(ucacheinh)	; get data cache inhibit bit

	or	r2,r3,r8		; or in code cache inhibit if set
	st	r2,r13,UARP		; store code side
	tb1	0,r0,128		; wait for instruction to finish
	or	r2,r3,r9		; or in data cache inhibit if set
	st	r2,r7,UARP		; store data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	br	@Lexit

@Lnotusr:
	sub	r10,r2,STOSRP		; store supv root pointer?
	bcnd	ne0,r10,@Lnotsup

	or.u	r12,r0,hi16(dcacheinh)
	ld	r9,r12,lo16(dcacheinh)	; get data cache inhibit bit

	or	r2,r3,r8		; or in code cache inhibit if set
	st	r2,r13,SARP		; else store supv. root pointer
	tb1	0,r0,128		; wait for instruction to finish
	or	r2,r3,r9		; or in cache inhibit if set
	st	r2,r7,SARP		; store data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	br	@Lexit

@Lnotsup:
	st	r3,r2,r0		; use r2 as cmmu address
	tb1	0,r0,128		; wait for instruction to finish

@Lexit:
	or	r13,r0,r0		; clear all traces of CMMU addr
	or	r7,r0,r0		; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

#endif
E 1
