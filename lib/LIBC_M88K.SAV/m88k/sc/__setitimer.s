;	M88000 __setitimer(2) Routine
;
	file	"__setitimer.s"
	text
	global	__setitimer
__setitimer:
	MCOUNT
	or	r9,r0,45
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
