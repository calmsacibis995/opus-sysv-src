;	M88000 __setpgid(2) Routine
;
	file	"__setpgid.s"
	text
	global	__setpgid
__setpgid:
	MCOUNT
	or	r9,r0,46
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
