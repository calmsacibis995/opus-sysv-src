;	M88000 __mount(2) Routine
;
	file	"__mount.s"
	text
	global	__mount
__mount:
	MCOUNT
;	tb0	0,r0,256+21		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,1		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
