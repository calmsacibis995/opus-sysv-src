;	M88000 __setsid(2) Routine
;
	file	"__setsid.s"
	text
	global	__setsid
__setsid:
	MCOUNT
	or	r9,r0,48
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
