;	M88000 recv(2) Routine
;
;
	file	"recv.s"
	text
	global	__recv
__recv:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,11	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
