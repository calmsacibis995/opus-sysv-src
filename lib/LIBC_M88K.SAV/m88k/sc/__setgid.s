;	M88000 __setgid(2) Routine
;
	file	"__setgid.s"
	text
	global	__setgid
__setgid:
	MCOUNT
	;tb0	0,r0,256+46		; trap to Unix
	or	r9,r0,44
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
