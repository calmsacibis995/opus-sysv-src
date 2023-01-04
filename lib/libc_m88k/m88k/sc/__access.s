;	M88000 __access(2) Routine
;
	file	"__access.s"
	text
	global	__access
__access:
	MCOUNT
	;tb0	0,r0,256+33		; trap to Unix
	or	r9,r0,1
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
