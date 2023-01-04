;	M88000 __getdents(2) Routine
;
	file	"__getdents.s"
	text
	global	__getdents
__getdents:
	MCOUNT
	;tb0	0,r0,256+81		; trap to Unix
	or	r9,r0,14
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
