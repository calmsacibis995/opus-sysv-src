;	@(#)sigcode.s	6.1 
	file	"sigcode.s"

; __sigcode: the interface routine to user's signal handlers.

; The kernel forces the entry to __sigcode with the stack thus:
;
;LOMEM:
;SP>		<struct sigframe>
;HIMEM:
;
; __sigcode does this:
;
; 1)	save registers    (r1 thru r30)
;
; 2)	align the stack and allocate the argument homing area
;
; 3)	call __sigcall(struct sigframe *) to call the user's signal handler
;
; 4)	restore the registers
;
; 5)	Issue sigret to continue.

	text
	global	__sigcode
	global	__sigcall

__sigcode:
;  Control comes directly here from the kernel via an rte.

	subu	r31,r31,4*36	; Make space for saving registers.

	st	r1,r31,4	; poke both ends of siginfo
	tb1	0,r0,511
	st	r0,r31,140
	tb1	0,r0,511

	st.d	r2,r31,8
	st.d	r4,r31,16
	st.d	r6,r31,24
	st.d	r8,r31,32
	st.d	r10,r31,40
	st.d	r12,r31,48
	st.d	r14,r31,56
	st.d	r16,r31,64
	st.d	r18,r31,72
	st.d	r20,r31,80
	st.d	r22,r31,88
	st.d	r24,r31,96
	st.d	r26,r31,104
	st.d	r28,r31,112
	st	r30,r31,120

	addu	r2,r0,1			; si_version
	st	r2,r31,0

	or	r9,r0,21		; getpsr
	tb0	0,r0,450		; system call
	addu	r2,r0,0x3f0		; error psr
	st	r2,r31,124		; good psr

	fldcr	r2,cr62			; si_fpsr
	fldcr	r3,cr63			; si_fpcr
	st.d	r2,r31,128

	addu	r2,r31,4*36
	st	r2,r31,136		; si_sigframe

;	argument to __sigcall
	addu	r2,r0,r31		; struct siginfo *

;	allocate space for homing args (32) and  
	subu	r31,r31,32

	bsr	__sigcall	; call _sigcall(siginfo *)

	addu	r31,r31,32

	ld.d	r2,r31,128
	fstcr	r2,cr62			; si_fpsr
	fstcr	r3,cr63			; si_fpcr

	ld	r2,r31,124
	and	r2,r2,4			; PSR_MXM
	and.u	r2,r2,0x7000		; PSR_BO + PSR_SER + PSR_C
	or	r9,r0,47		; setpsr
	tb0	0,r0,450		; system call
	or	r0,r0,r0		; error return

	ld	r1,r31,4
	ld.d	r2,r31,8
	ld.d	r4,r31,16
	ld.d	r6,r31,24
	ld.d	r8,r31,32
	ld.d	r10,r31,40
	ld.d	r12,r31,48
	ld.d	r14,r31,56
	ld.d	r16,r31,64
	ld.d	r18,r31,72
	ld.d	r20,r31,80
	ld.d	r22,r31,88
	ld.d	r24,r31,96
	ld.d	r26,r31,104
	ld.d	r28,r31,112
	ld	r30,r31,120

	addu	r31,r31,4*36

;
; at this point we do a sigret
;
	tb0	0,r0,448		; sigret
