;	M88000 accept(2) Routine
;
;
	file	"accept.s"
	text
	global	__accept
__accept:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,5		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
