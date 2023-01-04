;	M88000 __read(2) Routine
;
	file	"__read.s"
	text
	global	__read
__read:
	MCOUNT
	;tb0	0,r0,256+3		; trap to Unix
	or	r9,r0,40
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
