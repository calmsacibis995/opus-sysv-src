;	M88000 __link(2) Routine
;
	file	"__link.s"
	text
	global	__link
__link:
	MCOUNT
	;tb0	0,r0,256+9		; trap to Unix
	or	r9,r0,25
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
