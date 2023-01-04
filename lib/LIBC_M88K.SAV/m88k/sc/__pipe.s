;	M88000 __pipe(2) Routine
;
	file	"__pipe.s"
	text
	global	__pipe
__pipe:
	MCOUNT
	addu	r4,r2,r0
	;tb0	0,r0,256+42		; trap to Unix
	or	r9,r0,34
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	st	r2,r4,0
	st	r3,r4,4
	jmp.n	r1			; return to caller
	add	r2,r0,r0
