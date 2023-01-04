;	M88000 __umask(2) Routine
;
	file	"__umask.s"
	text
	global	__umask
__umask:
	MCOUNT
	;tb0	0,r0,256+60		; trap to Unix
	or	r9,r0,62
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
