;	M88000 __ulimit(2) Routine
;
	file	"__ulimit.s"
	text
	global	__ulimit
__ulimit:
	MCOUNT
	;tb0	0,r0,256+63		; trap to Unix
	or	r9,r0,61
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
