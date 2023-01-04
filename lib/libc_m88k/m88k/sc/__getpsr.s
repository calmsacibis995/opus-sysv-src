;	M88000 __getpsr(2) Routine
;
	file	"__getpsr.s"
	text
	global	__getpsr
__getpsr:
	MCOUNT
	or	r9,r0,21
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
