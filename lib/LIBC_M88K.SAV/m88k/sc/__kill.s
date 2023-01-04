;	M88000 __kill(2) Routine
;
	file	"__kill.s"
	text
	global	__kill
__kill:
	MCOUNT
	;tb0	0,r0,256+37		; trap to Unix
	or	r9,r0,24
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
