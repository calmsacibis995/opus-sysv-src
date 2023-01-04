	global	__fpsetround
__fpsetround:
	fldcr	r3,cr63
	or	r4,r0,r3
	mask	r3,r3,0x3FFF
	mask	r2,r2,0xC000
	or	r2,r2,r3
	fstcr	r2,cr63
	mask	r2,r4,0xC000
	jmp	r1
