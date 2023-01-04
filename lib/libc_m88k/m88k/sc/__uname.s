;	M88000 __uname(2) Routine
;
	file	"__uname.s"
	text
	global	__uname
__uname:
	MCOUNT
	;add	r4,r0,r0
	;tb0	0,r0,256+57		; trap to Unix
	or	r9,r0,63
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0
