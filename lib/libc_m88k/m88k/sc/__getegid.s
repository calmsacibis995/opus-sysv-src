;	M88000 __getegid(2) Routine
;
	file	"__getegid.s"
	text
	global	__getegid
__getegid:
	MCOUNT
	;tb0	0,r0,256+47		; trap to Unix
	or	r9,r0,15
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	addu	r2,r3,r0
