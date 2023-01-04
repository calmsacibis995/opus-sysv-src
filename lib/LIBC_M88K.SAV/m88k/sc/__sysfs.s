;	M88000 __sysfs(2) Routine
;
	file	"__sysfs.s"
	text
	global	__sysfs
__sysfs:
	MCOUNT
;	tb0	0,r0,256+84		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,5		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
