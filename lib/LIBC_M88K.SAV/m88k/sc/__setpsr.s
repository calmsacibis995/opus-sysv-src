;	M88000 __setpsr(2) Routine
;
	file	"__setpsr.s"
	text
	global	__setpsr
__setpsr:
	MCOUNT
	or	r9,r0,47
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
