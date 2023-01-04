;	M88000 __getgid(2) Routine
;
	file	"__getgid.s"
	text
	global	__getgid
__getgid:
	MCOUNT
	;tb0	0,r0,256+47		; trap to Unix
	or	r9,r0,15
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
