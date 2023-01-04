;	M88000 __getuid(2) Routine
;
	file	"__getuid.s"
	text
	global	__getuid
__getuid:
	MCOUNT
	;tb0	0,r0,256+24		; trap to Unix
	or	r9,r0,22
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
