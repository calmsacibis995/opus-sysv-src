;	M88000 __lseek(2) Routine
;
	file	"__lseek.s"
	text
	global	__lseek
__lseek:
	MCOUNT
	;tb0	0,r0,256+19		; trap to Unix
	or	r9,r0,26
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
