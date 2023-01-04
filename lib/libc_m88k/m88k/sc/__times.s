;	M88000 __times(2) Routine
;
	file	"__times.s"
	text
	global	__times
__times:
	MCOUNT
	;tb0	0,r0,256+43		; trap to Unix
	or	r9,r0,60
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
