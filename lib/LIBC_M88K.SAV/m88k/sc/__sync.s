;	M88000 __sync(2) Routine
;
	file	"__sync.s"
	text
	global	__sync
__sync:
	MCOUNT
	;tb0	0,r0,256+36		; trap to Unix
	or	r9,r0,57
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
