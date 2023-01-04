;	M88000 __mkdir(2) Routine
;
	file	"__mkdir.s"
	text
	global	__mkdir
__mkdir:
	MCOUNT
	;tb0	0,r0,256+80		; trap to Unix
	or	r9,r0,28
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
