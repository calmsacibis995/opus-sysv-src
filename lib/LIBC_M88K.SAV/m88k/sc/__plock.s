;	M88000 __plock(2) Routine
;
	file	"__plock.s"
	text
	global	__plock
__plock:
	MCOUNT
	;tb0	0,r0,256+45		; trap to Unix
	or	r9,r0,35
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
