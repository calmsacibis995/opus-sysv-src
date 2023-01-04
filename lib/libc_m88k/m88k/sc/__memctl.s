;	M88000 __memctl(2) Routine
;
	file	"__memctl.s"
	text
	global	__memctl
__memctl:
	MCOUNT
	or	r9,r0,27
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
