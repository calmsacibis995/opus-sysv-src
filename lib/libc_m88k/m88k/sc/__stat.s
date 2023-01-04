;	M88000 __stat(2) Routine
;
	file	"__stat.s"
	text
	global	__stat
__stat:
	MCOUNT
	;tb0	0,r0,256+18		; trap to Unix
	or	r9,r0,56
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
