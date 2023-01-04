;	M88000 __utime(2) Routine
;
	file	"__utime.s"
	text
	global	__utime
__utime:
	MCOUNT
	;tb0	0,r0,256+30		; trap to Unix
	or	r9,r0,66
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
