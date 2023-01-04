	global	__fpgetmask
__fpgetmask:
	fldcr	r2,cr63
	mask	r2,r2,0x1f
	jmp	r1
