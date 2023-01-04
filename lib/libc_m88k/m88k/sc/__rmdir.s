;	M88000 __rmdir(2) Routine
;
	file	"__rmdir.s"
	text
	global	__rmdir
__rmdir:
	MCOUNT
	;tb0	0,r0,256+79		; trap to Unix
	or	r9,r0,42
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
