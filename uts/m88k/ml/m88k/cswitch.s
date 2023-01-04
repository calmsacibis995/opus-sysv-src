;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)cswitch.m4	6.15	

; MC78000 version
; This file contains the routines used for context switching.

;	The routines contained here are:

	global	_setjmp		; set up environment for external goto
	global	_longjmp	; non-local goto
	global	_save		; saves register sets
	global	_resume		; restores register sets & u, then longjmps


; setjmp (save_area)
; save (save_area)
;	saves registers and return location in save_area


	text
_setjmp:
_save:
; Only registers that are required to be saved by the C calling convention
; need be saved and restored.  These are r29,r30, and r31.  The return address
; must also be saved so that control may return via longjmp or resume.

	st	r1,r2,0		; Save the registers.
	st	r14,r2,4
	st	r15,r2,8
	st	r16,r2,12
	st	r17,r2,16
	st	r18,r2,20
	st	r19,r2,24
	st	r20,r2,28
	st	r21,r2,32
	st	r22,r2,36
	st	r23,r2,40
	st	r24,r2,44
	st	r25,r2,48
	st	r26,r2,52
	st	r27,r2,56
	st	r28,r2,60
	st	r29,r2,64
	st	r30,r2,68
	st	r31,r2,72
	fldcr	r3,cr62		; Save floating point control register.
	st	r3,r2,76
	fldcr	r3,cr63		; Save floating point status register.
	st	r3,r2,80
	jmp.n   r1			; return(0)
	add	r2,r0,0


; longjmp (save_area)
;	resets registers from save_area and returns to the location
;	from WHERE setjmp() WAS CALLED.
;	** Never returns 0 - this lets the returned-to environment know that the
;	return was the result of a long jump.

_longjmp:
	ld	r1,r2,0		; Restore the registers.
	ld	r14,r2,4
	ld	r15,r2,8
	ld	r16,r2,12
	ld	r17,r2,16
	ld	r18,r2,20
	ld	r19,r2,24
	ld	r20,r2,28
	ld	r21,r2,32
	ld	r22,r2,36
	ld	r23,r2,40
	ld	r24,r2,44
	ld	r25,r2,48
	ld	r26,r2,52
	ld	r27,r2,56
	ld	r28,r2,60
	ld	r29,r2,64
	ld	r30,r2,68
	ld	r31,r2,72
	jmp.n   r1			; return(1)
	add	r2,r0,1



; resume (&u, save_area)
;	switches to another process's "u" area. Returns non-zero.

_resume:

;
#ifdef	ASPEN
	def	RP,0		; Root Pointer register
	def	MMUADDR,0x10000038	; in CPU space
#endif

	ldcr	r4,cr1		; Mask interrupts.
	add	r6,r4,0		; Save old cr1.
	set	r4,r4,1<PS_IPL>
	stcr	r4,cr1

#ifdef	ASPEN
; Flush code and data caches.
; Aspen specific.
	or.u	r4,r0,hi16(FSCC)
	or	r4,r4,lo16(FSCC)
	st	r0,r4,4		; Flush supervisor data
	st	r0,r4,8		; Flush user text 
	st	r0,r4,12	; Flush user data
#endif

; Map in new 8k ublock.
;	ld	r4,r0,_mmuupde	; Get pointer to u_block entries.
	or.u	r4,r0,hi16(_mmuupde)
	ld	r4,r4,lo16(_mmuupde) ; Get pointer to u_block entries.
	ld	r5,r2,0		; Get the ublock PTEs #1
	st	r5,r4,0
	ld	r5,r2,4		; Get the ublock PTEs #2
	st	r5,r4,4
#ifdef	ASPEN
	ld	r5,r2,8		; Get the ublock PTEs #3
	st	r5,r4,8
	ld	r5,r2,12	; Get the ublock PTEs #4
	st	r5,r4,12
	ld	r5,r2,16	; Get the ublock PTEs #5
	st	r5,r4,16
	ld	r5,r2,20	; Get the ublock PTEs #6
	st	r5,r4,20
	ld	r5,r2,24	; Get the ublock PTEs #7
	st	r5,r4,24
	ld	r5,r2,28	; Get the ublock PTEs #8
	st	r5,r4,28

; Flush MMU Address cache.
	ld	r2,r0,_fcltbl	; flush by re-writing root pointer.
	or.u	r4,r0,hi16(MMUADDR)
	st	r2,r4,lo16(MMUADDR)
#endif
#ifdef	CMMU
	bsr	_cmmuflush
#endif
	stcr	r6,cr1		; Restore priority level.
; Restore the registers from the new u_block.
	ld	r1,r3,0		; Restore the registers.
	ld	r14,r3,4
	ld	r15,r3,8
	ld	r16,r3,12
	ld	r17,r3,16
	ld	r18,r3,20
	ld	r19,r3,24
	ld	r20,r3,28
	ld	r21,r3,32
	ld	r22,r3,36
	ld	r23,r3,40
	ld	r24,r3,44
	ld	r25,r3,48
	ld	r26,r3,52
	ld	r27,r3,56
	ld	r28,r3,60
	ld	r29,r3,64
	ld	r30,r3,68
	ld	r31,r3,72
	ld	r2,r3,76
	fstcr	r2,cr62		; Restore floating point control register.
	ld	r2,r3,80
	fstcr	r2,cr63		; Restore floating point status register.
	jmp.n   r1			; return(1)
	add	r2,r0,1
