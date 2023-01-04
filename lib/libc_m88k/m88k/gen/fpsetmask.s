	global	__fpsetmask
__fpsetmask:
	fldcr	r3,cr63		; get mask
	or	r4,r0,r3	; save a copy
	mask	r3,r3,0xC000	; preserve rounding mode
	mask	r2,r2,0x1f	; isolate mask
	or	r2,r2,r3	; new mask
	fstcr	r2,cr63		; set it
	mask	r2,r4,0x1f	; old mask
	jmp	r1
