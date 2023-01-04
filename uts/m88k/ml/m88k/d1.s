	file	"divide.c"
;	divide routine that avoids kernel traps
;	r12	numerator
;	r13	denominator
;	r12	result
	text
	align	4
	global	_divide
_divide:
	; ?/?	r12/r13
	bb1	31,r12,@L1
	; +/?
	bb1	31,r13,@L2
	; +/+
	jmp.n	r1
	div	r12,r12,r13

@L1:	; -/?
	bb1.n	31,r13,@L3
	; -/+
	subu	r12,r0,r12
	div	r12,r12,r13
	jmp.n	r1
	subu	r12,r0,r12

@L2:	; +/-
	subu	r13,r0,r13
	div	r12,r12,r13
	jmp.n	r1
	subu	r12,r0,r12

@L3:	; -/-
	subu	r13,r0,r13
	jmp.n	r1
	div	r12,r12,r13

	data
