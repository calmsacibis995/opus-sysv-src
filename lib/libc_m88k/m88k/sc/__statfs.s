;	M88000 __statfs(2) Routine
;
	file	"__statfs.s"
	text
	global	__statfs
__statfs:
	MCOUNT
;	tb0	0,r0,256+35		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,4		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
