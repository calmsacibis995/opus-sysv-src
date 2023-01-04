;	M88000 __open(2) Routine
;
	file	"__open.s"
	text
	global	__open
__open:
	MCOUNT
	;tb0	0,r0,256+5		; trap to Unix
	or	r9,r0,32
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
