;	M88000 __getpgrp(2) Routine
;
	file	"__getpgrp.s"
	text
	global	__getpgrp

__getpgrp:
	MCOUNT
	;add	r2,r0,0
	;tb0	0,r0,256+39		; trap to Unix
	or	r9,r0,19
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
