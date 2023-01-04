;	M88000 __getpid(2) Routine
;
	file	"__getpid.s"
	text
	global	__getpid
__getpid:
	MCOUNT
	;tb0	0,r0,256+20		; trap to Unix
	or	r9,r0,20
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
