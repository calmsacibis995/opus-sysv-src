;	M88000 __close(2) Routine
;
	file	"__close.s"
	text
	global	__close
__close:
	MCOUNT
	;tb0	0,r0,256+6		; trap to Unix
	or	r9,r0,7
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
