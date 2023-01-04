h24204
s 00070/00000/00000
d D 1.1 90/03/06 12:46:18 root 1 0
c date and time created 90/03/06 12:46:18 by root
e
u
U
t
T
I 1
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)math.m4	6.1 	

; MC78000 version

	text

; unsigned min(a,b)
; unsigned a,b;
; {	return a <= b ? a : b; }

	global	_min
_min:
	sub	r4,r3,r2
	bcnd	ge0,r4,L%min
	jmp.n	r1
	add	r2,r3,0
L%min:
	jmp	r1

; unsigned max(a,b)
; unsigned a,b;
; {	return a >= b ? a : b; }

	global	_max
_max:
	sub	r4,r3,r2
	bcnd	le0,r4,L%max
	jmp.n	r1
	add	r2,r3,0
L%max:
	jmp	r1

; ffs(value)
;  Find first bit set in value.
;  return 0 for no bits set. 1-32 for first bit number.
;
;
; log2(value)
; returns log2 value of passed parameter.
; Note:  Assumes only 1 bit set in passed parameter.  Bad results otherwise.

	global	_log2
_log2:
	jmp.n	r1
	ff1	r2,r2

	global	_ffs
_ffs:
	ff1	r2,r2
	cmp	r13,r2,32
	bb1	eq,r13,nonehigh
	jmp.n	r1
	addu	r2,r2,1
nonehigh:
	jmp.n	r1
	addu	r2,r0,r0
	
; exp2(value)
; returns exp2 value of passed parameter.
; (same as setting bit number "value" and returning. )
; Note: Assumes that passed parameter is between 0 and 31.

	global	_exp2
_exp2:
	add	r2,r2,0x20	; Bit field width 1.
	jmp.n	r1
	set	r2,r0,r2

E 1
