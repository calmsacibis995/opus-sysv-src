;	M88000 ___ioctl(2) Routine
;
	file	"___ioctl.s"
	text
	global	___ioctl
___ioctl:
	MCOUNT
	;tb0	0,r0,256+54		; trap to Unix
	or	r9,r0,23
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
