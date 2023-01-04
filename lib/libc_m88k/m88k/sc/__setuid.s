;	M88000 __setuid(2) Routine
;
	file	"__setuid.s"
	text
	global	__setuid
__setuid:
	MCOUNT
	;tb0	0,r0,256+23		; trap to Unix
	or	r9,r0,49
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
