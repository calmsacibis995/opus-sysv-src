	global	__fpgetround
__fpgetround:
	fldcr	r2,cr63
	mask	r2,r2,0xC000
	jmp	r1
