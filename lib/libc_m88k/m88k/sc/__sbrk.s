;	@(#)__sbrk.s	6.1
;
;	M88000 __sbrk(2) Routine
;
;	(C) Copyright 1987 by Motorola, Inc.
;
; __sbrk - Unix kernel call
;
	file	"__sbrk.s"
	text
	global	__sbrk
__sbrk:
	MCOUNT
; The following line of code needs to be reinstated, and the
; two lines of code following it removed (in the Motorola libc), once 
; we have a seperate GH crt0.  
; The purpose of this change is to keep the linker from changing the r0
; reference to a r26 reference. 880311saj
;	addu	r3,r0,nd		; get pointer to ending addr. var.
        or.u    r3,r0,hi16(nd)
        or	r3,r3,lo16(nd)
; end change 880311saj
	ld	r3,r0,r3		; get value
	addu	r2,r2,r3		; adjust ending addr. value
	or	r4,r2,r0
	;tb0	0,r0,256+17		; trap to Unix
	or	r9,r0,2
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
; The following line of code needs to be reinstated, and the
; two lines of code following it removed (in the Motorola libc), once 
; we have a seperate GH crt0.  
; The purpose of this change is to keep the linker from changing the r0
; reference to a r26 reference. 880311saj
;	addu	r3,r0,nd		; get pointer to ending addr. var.
        or.u    r3,r0,hi16(nd)
        or	r3,r3,lo16(nd)
; end change 880311saj
	ld	r2,r0,r3
	st	r4,r0,r3
	jmp	r1			; return to caller

; brk - Unix kernel call
;
	text
	global	__brk
__brk:
	MCOUNT
	addu	r4,r0,r2		; save new break value  TA 3/9/88
	;tb0	0,r0,256+17		; trap to Unix
	or	r9,r0,2
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
; The following line of code needs to be reinstated, and the
; two lines of code following it removed (in the Motorola libc), once 
; we have a seperate GH crt0.  
; The purpose of this change is to keep the linker from changing the r0
; reference to a r26 reference. 880311saj
	addu	r3,r0,nd		; get pointer to ending addr. var.
        or.u    r3,r0,hi16(nd)
        or	r3,r3,lo16(nd)
; end change 880311saj
	st	r4,r0,r3		; save new  TA 3/9/88
	jmp.n	r1			; return to caller
	or	r2,r0,r0		; return zero

	data
	align	4
	global	_end
nd:	word	_end
