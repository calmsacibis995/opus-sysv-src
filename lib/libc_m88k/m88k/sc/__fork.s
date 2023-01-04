;	M88000 __fork(2) Routine
;
	file	"__fork.s"
	text
	global	__fork
__fork:
	MCOUNT
	or	r9,r0,11
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
