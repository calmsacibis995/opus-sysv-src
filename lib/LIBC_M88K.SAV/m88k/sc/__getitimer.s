;	M88000 __getitimer(2) Routine
;
	file	"__getitimer.s"
	text
	global	__getitimer
__getitimer:
	MCOUNT
	or	r9,r0,17
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
