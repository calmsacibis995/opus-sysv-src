;	M88000 __lchown(2) Routine
;
	file	"__lchown.s"
	text
	global	__lchown
__lchown:
	MCOUNT
	or	r9,r0,5
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
