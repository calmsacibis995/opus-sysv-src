;	M88000 sendto(2) Routine
;
;
	file	"sendto.s"
	text
	global	__sendto
__sendto:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,9		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
