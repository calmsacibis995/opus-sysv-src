;	M88000 __write(2) Routine
;
	file	"__write.s"
	text
	global	__write
__write:
	MCOUNT
	;tb0	0,r0,256+4		; trap to Unix
	or	r9,r0,68
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
