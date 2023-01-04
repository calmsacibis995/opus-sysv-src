	global	__fpgetsticky
__fpgetsticky:
	fldcr	r2,cr62
	mask	r2,r2,0x1f
	jmp	r1
