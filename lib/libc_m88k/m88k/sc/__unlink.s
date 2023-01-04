;	M88000 __unlink(2) Routine
;
	file	"__unlink.s"
	text
	global	__unlink
__unlink:
	MCOUNT
	;tb0	0,r0,256+10		; trap to Unix
	or	r9,r0,64
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
