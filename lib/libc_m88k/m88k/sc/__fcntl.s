;	M88000 __fcntl(2) Routine
;
	file	"__fcntl.s"
	text
	global	__fcntl
__fcntl:
	MCOUNT
	;tb0	0,r0,256+62		; trap to Unix
	or	r9,r0,10
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
