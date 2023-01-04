;	M88000 __nice(2) Routine
;
	file	"__nice.s"
	text
	global	__nice
__nice:
	MCOUNT
	;tb0	0,r0,256+34		; trap to Unix
	or	r9,r0,31
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
