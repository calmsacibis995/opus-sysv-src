;	M88000 __poll(2) Routine
;
	file	"__poll.s"
	text
	global	__poll
__poll:
	MCOUNT
	;tb0	0,r0,256+87		; trap to Unix
	or	r9,r0,36
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
