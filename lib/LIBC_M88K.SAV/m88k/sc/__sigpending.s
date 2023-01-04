;	M88000 __sigpending(2) Routine
;
	file	"__sigpending.s"
	text
	global	__sigpending
__sigpending:
	MCOUNT
	or	r9,r0,53
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
