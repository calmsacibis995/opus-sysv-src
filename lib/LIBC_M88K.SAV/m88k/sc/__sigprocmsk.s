;	M88000 __sigprocmask(2) Routine
;
	file	"__sigprocmsk.s"
	text
	global	__sigprocmask
__sigprocmask:
	MCOUNT
	or	r9,r0,54
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
