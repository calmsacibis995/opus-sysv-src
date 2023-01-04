	global	__fpsetsticky
__fpsetsticky:
	fldcr	r3,cr62 	;	save old value
	mask	r2,r2,0x1f
	fstcr	r2,cr62
	jmp.n	r1
	or	r2,r3,r0	;	return old value
