;	M88000 __sigsuspend(2) Routine
;
	file	"__sigsuspend.s"
	text
	global	__sigsuspend
__sigsuspend:
	MCOUNT
	or	r9,r0,55
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
