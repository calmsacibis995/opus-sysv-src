;	M88000 __exec(2) Routine
;
	file	"__exec.s"
	text
	global	__exec
__exec:
	MCOUNT
	;tb0	0,r0,256+11		; trap to Unix
	or	r4,r0,r0
	or	r9,r0,8
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
