;	M88000 __ptrace(2) Routine
;
	file	"__ptrace.s"
	text
	global	__ptrace
__ptrace:
	MCOUNT
	;tb0	0,r0,256+26		; trap to Unix
	or	r9,r0,38
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
