;	M88000 writev(2) Routine
;
;
	file	"writev.s"
	text
	global	__writev
__writev:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,18	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
