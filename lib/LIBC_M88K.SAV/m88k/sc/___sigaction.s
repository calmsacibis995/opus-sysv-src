;	M88000 ___sigaction(2) Routine
;
	file	"___sigaction.s"
	text
	global	___sigaction
___sigaction:
	MCOUNT
	or	r9,r0,51
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
