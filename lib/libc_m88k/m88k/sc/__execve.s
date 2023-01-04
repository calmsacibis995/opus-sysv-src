;	M88000 __execve(2) Routine
;
	file	"__execve.s"
	text
	global	__execve
__execve:
	MCOUNT
	;tb0	0,r0,256+59		; trap to Unix
	or	r9,r0,8
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
