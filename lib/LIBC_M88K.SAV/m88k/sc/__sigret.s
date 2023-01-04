;	M88000 __sigret(2) Routine
;
	file	"__sigret.s"
	text
	global	__sigret
__sigret:
	MCOUNT
	tb0	0,r0,448		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
