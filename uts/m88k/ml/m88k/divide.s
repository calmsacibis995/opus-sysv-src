	file	"divide.c"
;	divide routine that avoids kernel traps
;	r12	numerator
;	r13	denominator
;	r12	result
	text
	align	4
	global	_divide
_divide:
	bcnd	le0,r13,@L15
	bb0	31,r12,@L12
	subu	r12,r0,r12
	div	r12,r12,r13
	jmp.n	r1
	subu	r12,r0,r12
@L15:
	bcnd	le0,r12,@L16
	subu	r13,r0,r13
	div	r12,r12,r13
	jmp.n	r1
	subu	r12,r0,r12
@L16:
	subu	r12,r0,r12
	subu	r13,r0,r13
@L12:
	div	r12,r12,r13
	jmp	r1
	data
