;	M88000 __chmod(2) Routine
;
	file	"__chmod.s"
	text
	global	__chmod
__chmod:
	MCOUNT
	;tb0	0,r0,256+15		; trap to Unix
	or	r9,r0,4
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
