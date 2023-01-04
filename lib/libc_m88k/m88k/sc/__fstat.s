;	M88000 __fstat(2) Routine
;
	file	"__fstat.s"
	text
	global	__fstat
__fstat:
	MCOUNT
	;tb0	0,r0,256+28		; trap to Unix
	or	r9,r0,13
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
